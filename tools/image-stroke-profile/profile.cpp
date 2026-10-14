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

int main(int argc, char** argv)
{
	using namespace s3d;
# ifdef SIV3D_STROKE_COUNT_ALLOCATIONS
	std::cout << "case,allocations,requested_bytes\n";
# else
	std::cout << "case,ns\n";
# endif
	if (argc > 1 && std::string{argv[1]} == "frames")
	{
		for (const int size : {32, 256, 1024})
			for (const int shape : {0, 1, 2, 3})
				for (const double width : {1.0, 8.0, 32.0})
				{
					const Vec2 fixtures[][4]{
						{{5.21,4.37}, {29.21,8.37}, {12.21,29.37}, {}},
						{{5.21,4.37}, {27.21,6.37}, {29.21,26.37}, {4.21,28.37}},
						{{3.21,3.37}, {29.21,4.37}, {13.21,13.37}, {8.21,28.37}},
						{{4.21,4.37}, {28.21,28.37}, {4.21,28.37}, {28.21,4.37}},
					};
					Vec2 points[4];
					const size_t n = shape == 0 ? 3 : 4;
					for (size_t i = 0; i < n; ++i) points[i] = fixtures[shape][i] * (size / 32.0);
					const std::span<const Vec2> contour{points, n};
					std::vector<Color> pixels(size * size, Color{40,70,100,255});
					const ImageView view{pixels.data(), size, size, static_cast<size_t>(size)};
					for (const auto style : {JoinStyle::Bevel, JoinStyle::Miter})
						for (const int mode : {0, 1, 2, 3})
						{
							const std::string id = std::to_string(size) + "/" + std::to_string(shape) + "/" + std::to_string(int(width))
								+ (style == JoinStyle::Bevel ? "/Bevel" : "/Miter") + (mode == 0 ? "/center" : mode == 1 ? "/inner" : mode == 2 ? "/outer" : "/mixed");
							const auto prepare = [&](ConvexStrokePiece* output, BoundsD& box)
							{
								const double firstWidth = width * (mode == 0 ? 0.5 : mode == 3 ? 0.25 : 1.0);
								size_t count = MakeJoinedFramePolygons(contour, style, firstWidth, output, box);
								const size_t innerCount = mode == 2 ? 0 : count;
								if (mode == 3) count += MakeJoinedFramePolygons(contour, style, width * 0.75, output + count, box);
								return std::pair{count, innerCount};
							};
							ConvexStrokePiece parts[16];
							BoundsD bounds;
							const auto [count, innerCount] = prepare(parts, bounds);
							const std::span<const ConvexStrokePiece> polygons{parts, count};
							Measure(id + "/prepare", [&]
							{
								ConvexStrokePiece output[16];
								BoundsD box;
								const auto result = prepare(output, box);
								Escape(output); Escape(box); Escape(result);
							});
							for (const bool aa : {false, true})
							{
								Measure(id + (aa ? "/AA" : "/noAA"), [&]
								{
									const auto raster = [&]<bool AA>()
									{
										if (mode == 0) DrawJoinedFramePolygons<AA>(view, polygons, bounds, Color{220,60,130,113}, ImagePixel::BlendMode::SourceOver, ImageDraw::DstAlpha::UseStored);
										else DrawJoinedFramePolygons<AA, true>(view, polygons, bounds, Color{220,60,130,113}, ImagePixel::BlendMode::SourceOver, ImageDraw::DstAlpha::UseStored, contour, innerCount);
									};
									if (aa) raster.template operator()<true>(); else raster.template operator()<false>();
									Escape(pixels);
								});
							}
						}
				}
		return 0;
	}
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
				const auto build = [&](auto& out) { BuildOpenStrokePrimitives(view, effective, width, LineCap::Round, LineCap::Round, out); Escape(out); };
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
						const auto raster = [&](const auto& parts) { DrawStrokePrimitives(view, parts, width, Color{ 220, 60, 130, 113 }, mode, aa, ImageDraw::DstAlpha::UseStored); Escape(pixels); };
						const auto suffix = std::string(aa ? "/AA" : "/noAA") + (mode == ImagePixel::BlendMode::Overwrite ? "/overwrite" : "/paint");
						Measure(id + "/raster" + suffix, [&] { raster(primitives); });
						Measure(id + "/whole" + suffix, [&]
						{
							std::vector<Vec2> out;
							collect(out);
							std::vector<StrokePrimitive> parts;
							BuildOpenStrokePrimitives(view, out, width, LineCap::Round, LineCap::Round, parts);
							raster(parts);
						});
					}
			}
}
