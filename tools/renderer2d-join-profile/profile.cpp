// Isolated measurements of production implementations; not linked into the engine.
# include <algorithm>
# include <array>
# include <chrono>
# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <new>
# include <random>
# include <string>
# include <vector>

# ifdef SIV3D_JOIN_COUNT_ALLOCATIONS
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

# include "../../Siv3D/src/Siv3D/Polygon/ClosedLineString.cpp"
# include "../../Siv3D/src/Siv3D/Polygon/Triangulate.cpp"
# include "../../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"

namespace
{
	using namespace s3d;
	using Clock = std::chrono::steady_clock;
	template<class T> void Escape(const T& value) { asm volatile("" : : "g"(&value) : "memory"); }

	template<class F> void Measure(const std::string& name, F&& function)
	{
		function(); // Warm reusable storage before both kinds of measurement.
# ifdef SIV3D_JOIN_COUNT_ALLOCATIONS
		allocations = allocatedBytes = 0;
		counting = true;
		function();
		counting = false;
		std::cout << name << ',' << allocations << ',' << allocatedBytes << '\n';
# else
		size_t iterations = 1;
		for (;;)
		{
			const auto start = Clock::now();
			for (size_t i = 0; i < iterations; ++i) function();
			if ((Clock::now() - start) >= std::chrono::milliseconds{ 3 }) break;
			iterations *= 2;
		}
		std::vector<double> samples;
		for (int round = 0; round < 11; ++round)
		{
			const auto start = Clock::now();
			for (size_t i = 0; i < iterations; ++i) function();
			samples.push_back(std::chrono::duration<double, std::nano>(Clock::now() - start).count() / iterations);
		}
		std::sort(samples.begin(), samples.end());
		std::cout << name << ',' << samples[5] << ',' << samples[1] << ',' << samples[9] << '\n';
# endif
	}
}

int main(int argc, char** argv)
{
	using namespace s3d;
# ifdef SIV3D_JOIN_COUNT_ALLOCATIONS
	std::cout << "case,allocations,requested_bytes\n";
# else
	std::cout << "case,median_ns,p10_ns,p90_ns\n";
# endif
	if (argc > 1 && std::string{ argv[1] } == "scan")
	{
		std::minstd_rand rng{ 20261014 };
		size_t cases = 0, multi = 0, empty = 0, invalid = 0;
		const size_t limit = (argc > 2 ? std::stoul(argv[2]) : 500);
		for (size_t i = 0; i < limit; ++i)
		{
			Array<Vec2> points;
			for (size_t k = 0; k < 3 + (i % 2); ++k)
				points << Vec2{static_cast<double>(rng() % 241), static_cast<double>(rng() % 241)};
			if (i % 10 == 0) points[1] = points[0];
			points << points[0] << points[1];
			for (const double width : {0.0, 0.25, 12.0, 160.0})
				for (const auto style : {JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
				{
					std::cerr << "scan case " << i << " width " << width << " style " << static_cast<int>(style);
					for (const auto p : points) std::cerr << " (" << p.x << "," << p.y << ")";
					std::cerr << '\n';
					const auto polygon = MakeBuffer(points, style, width);
					++cases;
					empty += polygon.empty();
					std::string reason;
					if (not boost::geometry::is_valid(polygon, reason))
					{
						++invalid;
						std::cout << "invalid,case=" << i << ",width=" << width << ",style=" << static_cast<int>(style) << ",reason=" << reason << '\n';
					}
					if (polygon.size() > 1)
					{
						++multi;
						std::cout << "multi,case=" << i << ",width=" << width << ",style=" << static_cast<int>(style) << ",polygons=" << polygon.size() << '\n';
					}
				}
		}
		std::cout << "scan,cases=" << cases << ",empty=" << empty << ",multi=" << multi << ",invalid=" << invalid << '\n';
		return invalid ? 1 : 0;
	}
	if (argc > 1 && std::string{argv[1]} == "scan-frames")
	{
		std::minstd_rand rng{20261016};
		size_t cases = 0, invalid = 0, multiple = 0;
		const size_t limit = argc > 2 ? std::stoul(argv[2]) : 500;
		for (size_t i = 0; i < limit; ++i)
		{
			Array<Vec2> points;
			for (size_t k = 0; k < 3 + (i % 2); ++k) points << Vec2{double(rng() % 241), double(rng() % 241)};
			if (i % 10 == 0) points[1] = points[0];
			for (const Vec2 widths : {Vec2{0, 0.25}, Vec2{12, 0}, Vec2{4, 16}, Vec2{160, 8}})
				for (const auto style : {JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
				{
					std::cerr << "frame case " << i << " inner " << widths.x << " outer " << widths.y << " style " << int(style) << '\n';
					const auto polygon = MakePolygonFrame(points, style, widths.x, widths.y);
					++cases;
					multiple += polygon.size() > 1;
					std::string reason;
					if (!boost::geometry::is_valid(polygon, reason))
					{
						++invalid;
						std::cout << "invalid,case=" << i << ",inner=" << widths.x << ",outer=" << widths.y << ",style=" << int(style) << ",reason=" << reason << '\n';
					}
				}
		}
		std::cout << "scan-frames,cases=" << cases << ",multiple=" << multiple << ",invalid=" << invalid << '\n';
		return invalid ? 1 : 0;
	}
	if (argc > 1 && std::string{argv[1]} == "frames")
	{
		std::array<Vertex2D, 4096> vertices;
		std::array<Vertex2D::IndexType, 16384> indices;
		const auto allocate = [&](uint16 v, uint32 i)
		{
			if (v > vertices.size() || i > indices.size()) std::abort();
			return Vertex2DBufferPointer{vertices.data(), indices.data(), 0};
		};
		const Float4 color{1, 0.5f, 0, 0.5f};
		for (const int n : {3, 4})
			for (const Vec2 widths : {Vec2{0, 20}, Vec2{20, 0}, Vec2{4, 16}, Vec2{16, 4}})
			{
				const Vec2 points[]{{40, 30}, {280, 50}, {260, 240}, {60, 220}};
				const std::span<const Vec2> input{points, size_t(n)};
				const std::string id = std::to_string(n) + "/" + std::to_string(int(widths.x)) + "/" + std::to_string(int(widths.y));
				Measure(id + "/legacy", [&]
				{
					Vec2 shifted[4];
					const double offset = (widths.y - widths.x) * 0.5;
					if (n == 3)
					{
						const auto t = Triangle{points[0], points[1], points[2]}.stretched(offset);
						shifted[0] = t.p0; shifted[1] = t.p1; shifted[2] = t.p2;
					}
					else
					{
						const auto q = Quad{points[0], points[1], points[2], points[3]}.stretched(offset);
						shifted[0] = q.p0; shifted[1] = q.p1; shifted[2] = q.p2; shifted[3] = q.p3;
					}
					const auto count = Vertex2DBuilder::BuildLineString(allocate, LineCap::Flat, LineCap::Flat,
						{shifted, size_t(n)}, none, float(widths.x + widths.y), widths.y == 0, CloseRing::Yes, color, 1);
					Escape(count); Escape(vertices); Escape(indices);
				});
				for (const auto style : {JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
				{
					const std::string label = id + (style == JoinStyle::Bevel ? "/Bevel" : style == JoinStyle::Round ? "/Round" : "/Miter");
					Measure(label + "/direct", [&]
					{
						const auto count = Vertex2DBuilder::BuildClosedLineString(allocate, input, style, widths.x, widths.y, color, 1);
						if (!count) std::abort();
						Escape(count); Escape(vertices); Escape(indices);
					});
					Measure(label + "/fallback", [&]
					{
						const auto polygons = MakePolygonFrame(input, style, widths.x, widths.y);
						for (const auto& polygon : polygons)
						{
							const auto triangles = MakeTriangleIndices(std::span<const Vec2>{polygon.outer()}, polygon.inners());
							const auto positions = MakeVertices(polygon.outer(), polygon.inners());
							const auto count = Vertex2DBuilder::BuildPolygon(allocate, positions, triangles, color);
							Escape(count); Escape(vertices); Escape(indices);
						}
					});
				}
			}
		return 0;
	}
	if (argc > 1)
	{
		std::cout << std::setprecision(17);
		for (const auto shape : { Array<Vec2>{ {80,60}, {240,80}, {130,210} }, Array<Vec2>{ {100,100}, {200,100}, {200,200}, {100,200} },
			Array<Vec2>{ {100,100}, {200,110}, {200,90} }, Array<Vec2>{ {100,100}, {200,100}, {200,200}, {150,150} },
			Array<Vec2>{ {100,100}, {200,200}, {100,200}, {200,100} } })
			for (const bool reverse : { false, true })
				for (const auto style : { JoinStyle::Bevel, JoinStyle::Miter, JoinStyle::Round })
				{
					auto points = shape;
					if (reverse) points.reverse();
					points << points[0] << points[1];
					const auto out = MakeBuffer(points, style, 12);
					std::cout << "shape " << shape.size() << " reverse " << reverse << " style " << static_cast<int>(style) << " polygons " << out.size() << '\n';
					for (const auto& p : out)
					{
						std::cout << "holes " << p.inners().size() << " outer:";
						for (const auto v : p.outer()) std::cout << " (" << v.x << "," << v.y << ")";
						std::cout << '\n';
					}
				}
		return 0;
	}
	std::array<Vertex2D, 4096> vertices;
	std::array<Vertex2D::IndexType, 16384> indices;
	uint16 vertexCount = 0;
	uint32 indexCount = 0;
	const auto allocate = [&](uint16 v, uint32 i)
	{
		if (v > vertices.size() || i > indices.size()) std::abort();
		vertexCount = v;
		indexCount = i;
		return Vertex2DBufferPointer{ vertices.data(), indices.data(), 0 };
	};
	const Float4 color{ 1, 0.5f, 0, 0.5f };
	for (const int n : { 3, 4 })
		for (const double thickness : { 2.0, 20.0, 80.0 })
		{
			const Array<Vec2> points = (n == 3)
				? Array<Vec2>{ { 40, 30 }, { 280, 50 }, { 120, 240 }, { 40, 30 }, { 280, 50 } }
				: Array<Vec2>{ { 40, 30 }, { 280, 50 }, { 260, 240 }, { 60, 220 }, { 40, 30 }, { 280, 50 } };
			const std::string id = std::to_string(n) + "/" + std::to_string(static_cast<int>(thickness));
			Measure(id + "/legacy-strip-array", [&]
			{
				const Array<Vec2> input = points;
				const auto count = Vertex2DBuilder::BuildLineString(allocate, LineCap::Square, LineCap::Square,
					input, none, static_cast<float>(thickness), false, CloseRing::Yes, color, 1);
				Escape(count); Escape(vertices); Escape(indices);
			});
			Measure(id + "/legacy-strip-stack", [&]
			{
				std::array<Vec2, 6> input;
				std::copy(points.begin(), points.end(), input.begin());
				const auto count = Vertex2DBuilder::BuildLineString(allocate, LineCap::Square, LineCap::Square,
					std::span<const Vec2>{ input.data(), points.size() }, none, static_cast<float>(thickness), false, CloseRing::Yes, color, 1);
				Escape(count); Escape(vertices); Escape(indices);
			});

			for (const auto style : { JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter })
			{
				const std::string label = id + (style == JoinStyle::Bevel ? "/Bevel" : style == JoinStyle::Round ? "/Round" : "/Miter");
				const auto polygon = MakeBuffer(points, style, thickness);
				if (polygon.empty()) std::abort();
				const auto& p = polygon[0];
				const auto triangles = MakeTriangleIndices(std::span<const Vec2>{ p.outer() }, p.inners());
				const auto positions = MakeVertices(p.outer(), p.inners());
				std::cerr << label << ",polygons=" << polygon.size() << ",vertices=" << positions.size() << ",indices=" << triangles.size() << '\n';
				Measure(label + "/buffer", [&] { const auto out = MakeBuffer(points, style, thickness); Escape(out); });
				Measure(label + "/earcut", [&] { const auto out = MakeTriangleIndices(std::span<const Vec2>{ p.outer() }, p.inners()); Escape(out); });
				Measure(label + "/float2", [&] { const auto out = MakeVertices(p.outer(), p.inners()); Escape(out); });
				Measure(label + "/write", [&] { const auto count = Vertex2DBuilder::BuildPolygon(allocate, positions, triangles, color); Escape(count); Escape(vertices); Escape(indices); });
				Measure(label + "/mesh", [&]
				{
					const auto count = Vertex2DBuilder::BuildClosedLineString(allocate, std::span<const Vec2>{ points.data(), static_cast<size_t>(n) }, style, thickness, color, 1);
					if (not count) std::abort();
					Escape(count); Escape(vertices); Escape(indices);
				});
				Measure(label + "/new-total", [&]
				{
					std::array<Vec2, 6> input;
					std::copy(points.begin(), points.end(), input.begin());
					const auto count = Vertex2DBuilder::BuildClosedLineString(allocate,
						std::span<const Vec2>{ input.data(), static_cast<size_t>(n) }, style, thickness, color, 1);
					if (not count) std::abort();
					Escape(count); Escape(vertices); Escape(indices);
				});
				std::cerr << label << "/mesh,vertices=" << vertexCount << ",indices=" << indexCount << '\n';
				Measure(label + "/fallback-total", [&]
				{
					std::array<Vec2, 6> input;
					std::copy(points.begin(), points.end(), input.begin());
					const auto out = MakeBuffer(std::span<const Vec2>{ input.data(), points.size() }, style, thickness);
					const auto tris = MakeTriangleIndices(std::span<const Vec2>{ out[0].outer() }, out[0].inners());
					const auto verts = MakeVertices(out[0].outer(), out[0].inners());
					const auto count = Vertex2DBuilder::BuildPolygon(allocate, verts, tris, color);
					Escape(count); Escape(vertices); Escape(indices);
				});
			}
		}
}
