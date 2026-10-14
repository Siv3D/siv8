// Isolated measurements of the real stroke implementation; never linked into the engine.
# include <algorithm>
# include <chrono>
# include <cstdlib>
# include <iostream>
# include <new>
# include <string>
# include <vector>

# ifdef SIV3D_STROKE_COUNT_ALLOCATIONS
namespace
{
	bool counting = false;
	size_t allocations = 0, allocatedBytes = 0;
}
void* operator new(const size_t size)
{
	if (void* p = std::malloc(size ? size : 1))
	{
		if (counting) { ++allocations; allocatedBytes += size; }
		return p;
	}
	throw std::bad_alloc{};
}
void operator delete(void* p) noexcept { std::free(p); }
void* operator new[](size_t n) { return ::operator new(n); }
void operator delete[](void* p) noexcept { ::operator delete(p); }
# endif

// Including the implementation exposes anonymous helpers only to this isolated tool.
# include "../../Siv3D/src/Siv3D/ImageDraw/SivImageDraw.cpp"

namespace
{
	using namespace s3d;
	using Clock = std::chrono::steady_clock;
	template<class T> void Escape(const T& value) { asm volatile("" : : "g"(&value) : "memory"); }

	template<class F> void Measure(const std::string& name, F&& function)
	{
# ifdef SIV3D_STROKE_COUNT_ALLOCATIONS
		allocations = allocatedBytes = 0;
		counting = true;
		function();
		counting = false;
		std::cout << name << ',' << allocations << ',' << allocatedBytes << '\n';
# else
		function();
		size_t iterations = 1;
		for (;;)
		{
			const auto start = Clock::now();
			for (size_t i = 0; i < iterations; ++i) function();
			if ((Clock::now() - start) >= std::chrono::milliseconds{ 2 }) break;
			iterations *= 2;
		}
		std::vector<double> samples;
		for (int round = 0; round < 9; ++round)
		{
			const auto start = Clock::now();
			for (size_t i = 0; i < iterations; ++i) function();
			samples.push_back(std::chrono::duration<double, std::nano>(Clock::now() - start).count() / iterations);
		}
		std::sort(samples.begin(), samples.end());
		std::cout << name << ',' << samples[4] << '\n';
# endif
	}
}

int main()
{
	using namespace s3d;
# ifdef SIV3D_STROKE_COUNT_ALLOCATIONS
	std::cout << "case,allocations,requested_bytes\n";
# else
	std::cout << "case,ns\n";
# endif
	for (const int count : { 4, 32, 128 })
		for (const int clip : { 0, 128, 240 })
			for (const double width : { 1.5, 12.0 })
			{
				constexpr int Size = 256;
				std::vector<Color> pixels(Size * Size, Color{ 40, 70, 100, 255 });
				const ImageView view{ pixels.data(), Size, Size, Size };
				std::vector<Vec2> points;
				for (int i = 0; i <= count; ++i)
					points.emplace_back(Size * (0.1 + 0.8 * i / count) - clip, Size * (0.5 + 0.3 * std::sin(i * 0.7)));
				const std::string id = std::to_string(count) + (clip == 0 ? "/inside" : (clip == 128 ? "/partial" : "/outside")) + (width == 1.5 ? "/thin" : "/wide");
				std::vector<Vec2> effective;
				const auto collect = [&](auto& out) { CollectStrokePoints(points.data(), points.size(), Vec2{ 0, 0 }, false, out); Escape(out); };
				collect(effective);
				Measure(id + "/points-fresh", [&] { std::vector<Vec2> out; collect(out); });
				Measure(id + "/points-reuse", [&] { collect(effective); });
				std::vector<StrokePrimitive> primitives;
				const auto build = [&](auto& out) { BuildOpenStrokePrimitives(view, effective, width, LineCap::Round, LineCap::Round, StrokeJoinStyle::Round, out); Escape(out); };
				build(primitives);
				Measure(id + "/primitives-fresh", [&] { std::vector<StrokePrimitive> out; build(out); });
				Measure(id + "/primitives-reuse", [&] { build(primitives); });
				std::vector<StrokeRowEvent> events;
				int32 y0 = 0, y1 = 0;
				const auto eventList = [&](auto& out) { const bool valid = BuildStrokeRowEvents(primitives, out, y0, y1); Escape(valid); Escape(out); };
				eventList(events);
				Measure(id + "/events-fresh", [&] { std::vector<StrokeRowEvent> out; eventList(out); });
				Measure(id + "/events-reuse", [&] { eventList(events); });

				// Cache row spans to measure distance coverage without event/sort/compositing work.
				struct Row { int y; StrokeRowSpan span; };
				std::vector<Row> spans;
				std::vector<size_t> active;
				size_t eventIndex = 0;
				for (int y = y0; y < y1; ++y)
				{
					ApplyStrokeRowEvents(events, eventIndex, y, active);
					const size_t first = spans.size();
					for (size_t index : active)
					{
						int32 x0 = 0, x1 = 0;
						if (GetStrokePrimitiveSpanAtY(primitives[index], true, y, x0, x1))
						{
							ClipSpanToWidth(x0, x1, Size);
							if (x0 < x1) spans.push_back({y, {{x0, x1}, index}});
						}
					}
					std::sort(spans.begin() + first, spans.end(), [](const Row& a, const Row& b)
					{
						const auto x = a.span.span, y = b.span.span;
						return (x.x0 < y.x0) || ((x.x0 == y.x0) && (x.x1 < y.x1));
					});
				}
				std::vector<uint8> coverage(Size * Size);
				Measure(id + "/coverage-only", [&]
				{
					std::fill(coverage.begin(), coverage.end(), 0);
					if (width < 2.0)
					{
						for (const auto& row : spans)
							AddStrokePrimitiveCoverageToSpan<false>(primitives[row.span.primitiveIndex], row.y,
								row.span.span, coverage.data() + row.y * Size + row.span.span.x0);
					}
					else
					{
						for (const auto& row : spans)
							AddStrokePrimitiveCoverageToSpan<true>(primitives[row.span.primitiveIndex], row.y,
								row.span.span, coverage.data() + row.y * Size + row.span.span.x0);
					}
					Escape(coverage);
				});
				for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
					for (const auto mode : { ImagePixel::BlendMode::Overwrite, ImagePixel::BlendMode::SourceOver })
					{
						const auto raster = [&](const auto& parts) { DrawStrokePrimitives(view, parts, width, Color{ 220, 60, 130, 113 }, mode, aa, ImageDraw::DstAlpha::Preserve); Escape(pixels); };
						const auto suffix = std::string(aa ? "/AA" : "/noAA") + (mode == ImagePixel::BlendMode::Overwrite ? "/overwrite" : "/paint");
						Measure(id + "/raster" + suffix, [&] { raster(primitives); });
						Measure(id + "/whole" + suffix, [&]
						{
							std::vector<Vec2> out;
							collect(out);
							std::vector<StrokePrimitive> parts;
							BuildOpenStrokePrimitives(view, out, width, LineCap::Round, LineCap::Round, StrokeJoinStyle::Round, parts);
							raster(parts);
						});
					}
			}
}
