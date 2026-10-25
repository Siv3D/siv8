//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include <cmath>
# include <limits>

# if SIV3D_PLATFORM(MACOS)
# include <ThirdParty/nanobench/nanobench.h>
# include <iostream>
# include <time.h>
# include <pthread.h>
# include <pthread/qos.h>
# endif

namespace
{
	using Index = Vertex2D::IndexType;
	constexpr Index Guard = std::numeric_limits<Index>::max();
	const Float4 Red{ 1, 0, 0, 0.5f }, Blue{ 0, 0, 1, 0.5f };
	const FloatRect Rect{ -10, -7, 10, 7 }, UV{ 0.2f, 0.3f, 0.8f, 0.9f };
	const Float2 Positions[]{ { 0, 0 }, { 4, 0 }, { 4, 3 }, { 0, 3 } };
	const TriangleIndex Triangles[]{ { 0, 1, 2 }, { 2, 3, 0 } };
	const Index FlatIndices[]{ 0, 1, 2, 2, 3, 0 };

	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<Index> indices;
		Index offset = 0;
		uint32 count = 0;
	};

	template <class Build>
	Mesh Capture(Build&& build, const bool atVertexLimit = false)
	{
		Mesh mesh;
		uint32 requests = 0;
		Vertex2D sentinel;
		sentinel.set(-123, -456, -7, -8, Float4{ -1, -2, -3, -4 });
		const auto allocate = [&](const uint16 vertices, const uint32 indices)
		{
			++requests;
			mesh.vertices.assign(static_cast<size_t>(vertices) + 2, sentinel);
			mesh.indices.assign(static_cast<size_t>(indices) + 2, Guard);
			mesh.offset = (atVertexLimit ? static_cast<Index>(Guard - vertices + 1) : Index{ 37 });
			return Vertex2DBufferPointer{ mesh.vertices.data() + 1, mesh.indices.data() + 1, mesh.offset };
		};
		mesh.count = build(allocate);
		REQUIRE(requests == 1);
		REQUIRE(mesh.indices.size() == (static_cast<size_t>(mesh.count) + 2));
		CHECK(mesh.indices.front() == Guard);
		CHECK(mesh.indices.back() == Guard);
		for (const auto* guard : { &mesh.vertices.front(), &mesh.vertices.back() })
		{
			CHECK(guard->pos == sentinel.pos);
			CHECK(guard->tex == sentinel.tex);
			CHECK(guard->color == sentinel.color);
		}
		for (size_t i = 1; i <= mesh.count; ++i)
		{
			REQUIRE(mesh.offset <= mesh.indices[i]);
			REQUIRE(mesh.indices[i] < (mesh.offset + mesh.vertices.size() - 2));
		}
		return mesh;
	}

	void CheckIndices(const Mesh& mesh, const std::span<const Index> expected)
	{
		REQUIRE(mesh.count == expected.size());
		for (size_t i = 0; i < expected.size(); ++i)
		{
			CAPTURE(i);
			CHECK(mesh.indices[i + 1] == (mesh.offset + expected[i]));
		}
	}

	double CheckWindingAndArea(const Mesh& mesh)
	{
		double area = 0;
		for (size_t i = 1; i <= mesh.count; i += 3)
		{
			const Vec2 a = mesh.vertices[1 + mesh.indices[i] - mesh.offset].pos;
			const Vec2 b = mesh.vertices[1 + mesh.indices[i + 1] - mesh.offset].pos;
			const Vec2 c = mesh.vertices[1 + mesh.indices[i + 2] - mesh.offset].pos;
			const double cross = (b - a).cross(c - a);
			CHECK(cross > 0);
			area += (cross * 0.5);
		}
		return area;
	}

	enum class Shape { Circle, Ellipse, SuperEllipse, TexturedCircle, RoundRect, RoundRectTB, RoundRectLR, TexturedRoundRect };
	constexpr Shape Shapes[]{ Shape::Circle, Shape::Ellipse, Shape::SuperEllipse, Shape::TexturedCircle,
		Shape::RoundRect, Shape::RoundRectTB, Shape::RoundRectLR, Shape::TexturedRoundRect };

	uint32 BuildShape(const BufferCreatorFunc& allocate, const Shape shape, const float scale)
	{
		using namespace Vertex2DBuilder;
		switch (shape)
		{
		case Shape::Circle:
			return BuildCircle(allocate, { 3, 5 }, 1, ColorFillDirection::InOut, Red, Blue, scale);
		case Shape::Ellipse:
			return BuildEllipse(allocate, { 3, 5 }, 1, 0.5f, ColorFillDirection::TopBottom, Red, Blue, scale);
		case Shape::SuperEllipse:
			return BuildSuperEllipse(allocate, { 3, 5 }, 1, 0.5f, 2, ColorFillDirection::LeftRight, Red, Blue, scale);
		case Shape::TexturedCircle:
			return BuildTexturedCircle(allocate, Circle{ 3, 5, 1 }, UV, Red, scale);
		case Shape::RoundRect:
			return BuildRoundRect(allocate, Rect, 1, Red, scale);
		case Shape::RoundRectTB:
			return BuildRoundRect(allocate, Rect, 1, ColorFillDirection::TopBottom, Red, Blue, scale);
		case Shape::RoundRectLR:
			return BuildRoundRect(allocate, Rect, 1, ColorFillDirection::LeftRight, Red, Blue, scale);
		case Shape::TexturedRoundRect:
			return BuildTexturedRoundRect(allocate, Rect, 20, 14, 1, UV, Red, scale);
		}
		return 0;
	}

	uint32 BuildIndexed(const BufferCreatorFunc& allocate, const int32 variant, const std::span<const TriangleIndex> triangles,
		const std::span<const Index> indices)
	{
		switch (variant)
		{
		case 0:
			return Vertex2DBuilder::BuildPolygon(allocate, Positions, triangles, Float2{ 2, 3 }, Red);
		case 1:
			return Vertex2DBuilder::BuildPolygon(allocate, Positions, indices, Red);
		case 2:
			return Vertex2DBuilder::BuildPolygonTransformed(allocate, Positions, triangles, 1, 0, { 2, 3 }, Red);
		default:
			Vertex2D vertices[4];
			for (size_t i = 0; i < 4; ++i) vertices[i].set(Positions[i], Float2{ 0.25f, 0.75f }, Red);
			return Vertex2DBuilder::BuildMesh2D(allocate, vertices, triangles, Float2{ 2, 3 });
		}
	}

	enum class RoundedShape
	{
		CirclePie, EllipsePie, CircleSegment, ArcFlat, ArcSquare, ArcRoundRadial, ArcRoundAlong,
		CircleFrame, EllipseFrame, CircleDashed, EllipseDashed, RoundRectDashed,
		RoundRectFrame, RoundRectFrameTB, RoundRectFrameLR, ShadowFill, ShadowRing,
		LineRound, ArrowRound, LineStringRound, LineStringGradientRound,
	};
	constexpr RoundedShape RoundedShapes[]{ RoundedShape::CirclePie, RoundedShape::EllipsePie, RoundedShape::CircleSegment,
		RoundedShape::ArcFlat, RoundedShape::ArcSquare, RoundedShape::ArcRoundRadial, RoundedShape::ArcRoundAlong,
		RoundedShape::CircleFrame, RoundedShape::EllipseFrame, RoundedShape::CircleDashed, RoundedShape::EllipseDashed,
		RoundedShape::RoundRectDashed, RoundedShape::RoundRectFrame, RoundedShape::RoundRectFrameTB, RoundedShape::RoundRectFrameLR,
		RoundedShape::ShadowFill, RoundedShape::ShadowRing, RoundedShape::LineRound, RoundedShape::ArrowRound,
		RoundedShape::LineStringRound, RoundedShape::LineStringGradientRound };

	uint32 BuildRoundedShape(const BufferCreatorFunc& allocate, const RoundedShape shape, const float scale, const float angle = Math::HalfPiF)
	{
		using namespace Vertex2DBuilder;
		const FloatRect inner{ -9.5f, -6.5f, 9.5f, 6.5f };
		const Vec2 points[]{ { 0, 0 }, { 4, 0 }, { 5, 3 } };
		switch (shape)
		{
		case RoundedShape::CirclePie:
			return BuildCirclePie(allocate, { 3, 5 }, 1, 0.2f, angle, Red, Blue, scale);
		case RoundedShape::EllipsePie:
			return BuildEllipsePie(allocate, { 3, 5 }, 1, 0.5f, 0.2f, angle, Red, Blue, scale);
		case RoundedShape::CircleSegment:
			return BuildCircleSegment(allocate, { 3, 5 }, 1, 0.2f, angle, Red, scale);
		case RoundedShape::ArcFlat:
		case RoundedShape::ArcSquare:
		case RoundedShape::ArcRoundRadial:
		case RoundedShape::ArcRoundAlong:
			return BuildCircleArc(allocate, ((shape == RoundedShape::ArcFlat) ? LineCap::Flat
				: ((shape == RoundedShape::ArcSquare) ? LineCap::Square : LineCap::Round)),
				{ 3, 5 }, 0.75f, 0.2f, angle, 0.5f,
				((shape == RoundedShape::ArcRoundRadial) ? ColorFillDirection::InOut : ColorFillDirection::LeftRight), Red, Blue, scale);
		case RoundedShape::CircleFrame:
			return BuildCircleFrame(allocate, { 3, 5 }, 0.75f, 0.5f, Red, Blue, scale);
		case RoundedShape::EllipseFrame:
			return BuildEllipseFrame(allocate, { 3, 5 }, 1, 0.5f, 0.1f, 0.25f, Red, Blue, scale);
		case RoundedShape::CircleDashed:
			return BuildCircleDashedFrame(allocate, { 3, 5 }, 0.75f, 0.2f, 0.5f, 0.25f, 8, Red, Blue, scale);
		case RoundedShape::EllipseDashed:
			return BuildEllipseDashedFrame(allocate, { 3, 5 }, 1, 0.5f, 0.1f, 0.25f, 0.2f, 0.25f, 8, Red, Blue, scale);
		case RoundedShape::RoundRectDashed:
			return BuildRoundRectDashedFrame(allocate, inner, 0.5f, Rect, 1, 0.2f, 0.25f, 8, Red, scale);
		case RoundedShape::RoundRectFrame:
			return BuildRoundRectFrame(allocate, inner, 0.5f, Rect, 1, Red, scale);
		case RoundedShape::RoundRectFrameTB:
		case RoundedShape::RoundRectFrameLR:
			return BuildRoundRectFrame(allocate, inner, 0.5f, Rect, 1,
				((shape == RoundedShape::RoundRectFrameTB) ? ColorFillDirection::TopBottom : ColorFillDirection::LeftRight), Red, Blue, scale);
		case RoundedShape::ShadowFill:
		case RoundedShape::ShadowRing:
			return BuildCircleShadow(allocate, Circle{ 3, 5, 1 }, 0.5f, Red, scale, (shape == RoundedShape::ShadowFill));
		case RoundedShape::LineRound:
			return BuildLine(allocate, LineCap::Round, LineCap::Round, { 0, 0 }, { 4, 0 }, 0.5f, { Red, Blue }, scale);
		case RoundedShape::ArrowRound:
			return BuildArrow(allocate, LineCap::Round, { 0, 0 }, { 4, 0 }, 0.5f, { 2, 1 }, { Red, Blue }, scale);
		case RoundedShape::LineStringRound:
			return BuildLineString(allocate, LineCap::Round, LineCap::Round, points, none, 0.5f, false, CloseRing::No, Red, scale);
		case RoundedShape::LineStringGradientRound:
			return BuildLineString(allocate, LineCap::Round, LineCap::Round, points, none, 0.5f, false, Red, Blue, scale);
		}
		return 0;
	}
}

TEST_CASE("Vertex2DBuilder.indices_closed_fan_order")
{
	constexpr Index Expected[]{ 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5,
		0, 5, 6, 0, 6, 7, 0, 7, 8, 0, 8, 1 };
	for (const auto shape : { Shape::Circle, Shape::Ellipse, Shape::SuperEllipse, Shape::TexturedCircle })
	{
		CAPTURE(static_cast<int32>(shape));
		for (const bool limit : { false, true })
		{
			CheckIndices(Capture([&](const BufferCreatorFunc& allocate) { return BuildShape(allocate, shape, 1); }, limit), Expected);
		}
	}
}

TEST_CASE("Vertex2DBuilder.indices_round_rect_order")
{
	constexpr Index Expected[]{ 0, 1, 12, 1, 2, 12, 3, 4, 13, 4, 5, 13,
		6, 7, 14, 7, 8, 14, 9, 10, 15, 10, 11, 15,
		11, 0, 15, 15, 0, 12, 9, 2, 8, 8, 2, 3, 14, 13, 6, 6, 13, 5 };
	for (const auto shape : { Shape::RoundRect, Shape::RoundRectTB, Shape::RoundRectLR, Shape::TexturedRoundRect })
	{
		CAPTURE(static_cast<int32>(shape));
		for (const bool limit : { false, true })
		{
			CheckIndices(Capture([&](const BufferCreatorFunc& allocate) { return BuildShape(allocate, shape, 1); }, limit), Expected);
		}
	}
}

TEST_CASE("Vertex2DBuilder.indices_scaled_topology")
{
	for (const auto shape : Shapes)
	{
		CAPTURE(static_cast<int32>(shape));
		for (const float scale : { 1.0f, 8.0f, 512.0f })
		{
			CAPTURE(scale);
			const auto mesh = Capture([&](const BufferCreatorFunc& allocate) { return BuildShape(allocate, shape, scale); }, true);
			const double area = CheckWindingAndArea(mesh);
			const bool roundedRect = (Shape::RoundRect <= shape);
			if (roundedRect)
			{
				CHECK(mesh.count == ((mesh.vertices.size() - 10) * 3 + 18));
				if (scale == 512) CHECK(std::abs(area - (280 - (4 - Math::Pi))) < 0.01);
			}
			else
			{
				CHECK(mesh.count == ((mesh.vertices.size() - 3) * 3));
				const double expected = (((shape == Shape::Ellipse) || (shape == Shape::SuperEllipse)) ? (Math::Pi * 0.5) : Math::Pi);
				if (scale == 512) CHECK(std::abs(area - expected) < 0.01);
			}
		}
	}
}

TEST_CASE("Vertex2DBuilder.indices_copy_offset_and_count")
{
	for (const size_t triangleCount : { 0u, 2u, 21846u })
	{
		CAPTURE(triangleCount);
		Array<TriangleIndex> triangles(Max(size_t{ 1 }, triangleCount));
		Array<Index> indices(Max(size_t{ 1 }, triangleCount * 3));
		for (size_t i = 0; i < triangleCount; ++i)
		{
			triangles[i] = Triangles[i % 2];
			for (size_t j = 0; j < 3; ++j) indices[i * 3 + j] = FlatIndices[(i * 3 + j) % 6];
		}
		for (int32 variant = 0; variant < 4; ++variant)
		{
			CAPTURE(variant);
			const auto mesh = Capture([&](const BufferCreatorFunc& allocate)
			{
				return BuildIndexed(allocate, variant, { triangles.data(), triangleCount }, { indices.data(), triangleCount * 3 });
			}, true);
			CheckIndices(mesh, { indices.data(), triangleCount * 3 });
			for (size_t i = 0; i < 4; ++i)
			{
				const Float2 expected = ((variant == 1) ? Positions[i] : ((variant == 2)
					? Float2{ -Positions[i].y + 2, Positions[i].x + 3 } : (Positions[i] + Float2{ 2, 3 })));
				CHECK(mesh.vertices[i + 1].pos == expected);
				CHECK(mesh.vertices[i + 1].color == Red);
				if (variant == 3) CHECK(mesh.vertices[i + 1].tex == Float2{ 0.25f, 0.75f });
			}
		}
	}
}

TEST_CASE("Vertex2DBuilder.indices_allocation_failure")
{
	uint32 requests = 0;
	const auto fail = [&](uint16, uint32)
	{
		++requests;
		return Vertex2DBufferPointer{ nullptr, nullptr, 37 };
	};
	for (const auto shape : Shapes)
	{
		CAPTURE(static_cast<int32>(shape));
		requests = 0;
		CHECK(BuildShape(fail, shape, 512) == 0);
		CHECK(requests == 1);
	}
	for (int32 variant = 0; variant < 4; ++variant)
	{
		CAPTURE(variant);
		requests = 0;
		CHECK(BuildIndexed(fail, variant, Triangles, FlatIndices) == 0);
		CHECK(requests == 1);
	}
	for (const auto shape : RoundedShapes)
	{
		CAPTURE(static_cast<int32>(shape));
		requests = 0;
		CHECK(BuildRoundedShape(fail, shape, 512) == 0);
		CHECK(requests == 1);
	}
}

TEST_CASE("Vertex2DBuilder.indices_open_fan_and_strip_order")
{
	constexpr Index Pie[]{ 0, 1, 2, 0, 2, 3 };
	constexpr Index Segment[]{ 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5 };
	constexpr Index Arc[]{ 0, 1, 2, 2, 1, 3, 2, 3, 4, 4, 3, 5 };
	constexpr Index SquareArc[]{ 0, 1, 2, 2, 1, 3, 2, 3, 4, 4, 3, 5,
		4, 5, 6, 6, 5, 7, 6, 7, 8, 8, 7, 9 };
	constexpr Index RoundArc[]{ 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5,
		6, 7, 8, 8, 7, 9, 8, 9, 10, 10, 9, 11,
		12, 13, 14, 12, 14, 15, 12, 15, 16, 12, 16, 17 };
	constexpr Index RoundLine[]{ 0, 1, 2, 2, 1, 3,
		4, 5, 6, 4, 6, 7, 4, 7, 8, 4, 8, 9,
		10, 11, 12, 10, 12, 13, 10, 13, 14, 10, 14, 15 };
	for (const bool limit : { false, true })
	{
		for (const float sign : { -1.0f, 1.0f })
		{
			const auto check = [&](const RoundedShape shape, const std::span<const Index> expected)
			{
				CAPTURE(static_cast<int32>(shape), sign, limit);
				CheckIndices(Capture([&](const BufferCreatorFunc& allocate)
					{ return BuildRoundedShape(allocate, shape, 1, (sign * Math::HalfPiF)); }, limit), expected);
			};
			check(RoundedShape::CirclePie, Pie);
			check(RoundedShape::EllipsePie, Pie);
			check(RoundedShape::CircleSegment, Segment);
			check(RoundedShape::ArcFlat, Arc);
			check(RoundedShape::ArcSquare, SquareArc);
			check(RoundedShape::ArcRoundRadial, RoundArc);
			check(RoundedShape::ArcRoundAlong, RoundArc);
			check(RoundedShape::LineRound, RoundLine);
		}
	}
}

TEST_CASE("Vertex2DBuilder.indices_closed_strip_order")
{
	constexpr Index Ring[]{ 0, 2, 1, 1, 2, 3, 2, 4, 3, 3, 4, 5, 4, 6, 5, 5, 6, 7,
		6, 8, 7, 7, 8, 9, 8, 10, 9, 9, 10, 11, 10, 12, 11, 11, 12, 13,
		12, 14, 13, 13, 14, 15, 14, 0, 15, 15, 0, 1 };
	constexpr Index RoundRectRing[]{ 0, 2, 1, 1, 2, 3, 2, 4, 3, 3, 4, 5, 4, 6, 5, 5, 6, 7,
		6, 8, 7, 7, 8, 9, 8, 10, 9, 9, 10, 11, 10, 12, 11, 11, 12, 13,
		12, 14, 13, 13, 14, 15, 14, 16, 15, 15, 16, 17, 16, 18, 17, 17, 18, 19,
		18, 20, 19, 19, 20, 21, 20, 22, 21, 21, 22, 23, 22, 0, 23, 23, 0, 1 };
	for (const bool limit : { false, true })
	{
		for (const auto shape : { RoundedShape::CircleFrame, RoundedShape::EllipseFrame, RoundedShape::ShadowRing,
			RoundedShape::RoundRectFrame, RoundedShape::RoundRectFrameTB, RoundedShape::RoundRectFrameLR })
		{
			CAPTURE(static_cast<int32>(shape), limit);
			const auto mesh = Capture([&](const BufferCreatorFunc& allocate) { return BuildRoundedShape(allocate, shape, 1); }, limit);
			CheckIndices(mesh, ((shape >= RoundedShape::RoundRectFrame) && (shape <= RoundedShape::RoundRectFrameLR))
				? std::span<const Index>{ RoundRectRing } : std::span<const Index>{ Ring });
			CheckWindingAndArea(mesh);
		}
		// A filled shadow puts the center before the ring and emits the inner fan after it.
		const auto mesh = Capture([&](const BufferCreatorFunc& allocate)
			{ return BuildRoundedShape(allocate, RoundedShape::ShadowFill, 1); }, limit);
		Array<Index> filled;
		for (const auto index : Ring) filled.push_back(index + 1);
		constexpr Index Inner[]{ 0, 2, 4, 0, 4, 6, 0, 6, 8, 0, 8, 10, 0, 10, 12, 0, 12, 14, 0, 14, 16, 0, 16, 2 };
		filled.insert(filled.end(), std::begin(Inner), std::end(Inner));
		CheckIndices(mesh, filled);
		CheckWindingAndArea(mesh);
	}
}

TEST_CASE("Vertex2DBuilder.indices_rounded_bounds_and_closure")
{
	for (const auto shape : RoundedShapes)
	{
		for (const float scale : { 1.0f, 8.0f, 512.0f })
		{
			CAPTURE(static_cast<int32>(shape), scale);
			const auto mesh = Capture([&](const BufferCreatorFunc& allocate) { return BuildRoundedShape(allocate, shape, scale); }, true);
			CHECK(mesh.count > 0);
			CHECK((mesh.count % 3) == 0);
			const bool roundRect = ((RoundedShape::RoundRectFrame <= shape) && (shape <= RoundedShape::RoundRectFrameLR));
			if ((shape == RoundedShape::CircleFrame) || (shape == RoundedShape::EllipseFrame) || (shape == RoundedShape::ShadowRing) || roundRect)
			{
				const size_t vertices = (mesh.vertices.size() - 2);
				CHECK(mesh.count == (vertices * 3));
				CHECK(mesh.indices[mesh.count - 3] == (mesh.offset + vertices - 1));
				CHECK(mesh.indices[mesh.count - 2] == (mesh.offset + vertices - 1));
				CHECK(mesh.indices[mesh.count - 1] == mesh.offset);
				CHECK(mesh.indices[mesh.count] == (mesh.offset + 1));
				CheckWindingAndArea(mesh);
			}
		}
	}

	// Tiny arcs still have an edge; a zero sweep emits nothing for pies/segments/flat arcs.
	for (const auto shape : { RoundedShape::CirclePie, RoundedShape::EllipsePie, RoundedShape::ArcFlat })
	{
		const auto mesh = Capture([&](const BufferCreatorFunc& allocate) { return BuildRoundedShape(allocate, shape, 1, 0.001f); }, true);
		CHECK(mesh.count == ((shape == RoundedShape::ArcFlat) ? 6 : 3));
	}
	for (const auto shape : { RoundedShape::CirclePie, RoundedShape::EllipsePie, RoundedShape::CircleSegment, RoundedShape::ArcFlat })
	{
		uint32 requests = 0;
		const auto fail = [&](uint16, uint32) { ++requests; return Vertex2DBufferPointer{}; };
		CHECK(BuildRoundedShape(fail, shape, 1, 0) == 0);
		CHECK(requests == 0);
	}
}

# if SIV3D_PLATFORM(MACOS)

namespace
{
	int64 BuilderThreadTime()
	{
		timespec time{};
		REQUIRE(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time) == 0);
		return (int64(time.tv_sec) * 1'000'000'000 + time.tv_nsec);
	}

	template <class Build>
	void BenchmarkBuilder(const char* name, const char* parameter, const uint32 value, Build&& build)
	{
		std::array<Vertex2D, 1024> vertices;
		Array<Index> indices(65538);
		const auto allocate = [&](uint16, uint32)
		{
			return Vertex2DBufferPointer{ vertices.data(), indices.data(), 37 };
		};
		const auto run = [&](const uint32 iterations)
		{
			const int64 start = BuilderThreadTime();
			for (uint32 i = 0; i < iterations; ++i)
			{
				ankerl::nanobench::doNotOptimizeAway(build(allocate));
			}
			return (BuilderThreadTime() - start);
		};
		const int64 warmupEnd = (BuilderThreadTime() + 10'000'000);
		while (BuilderThreadTime() < warmupEnd) run(256);
		const uint32 iterations = static_cast<uint32>(Clamp(int64{ 5'000'000 * 256 } / Max(run(256), int64{ 1 }), int64{ 256 }, int64{ 1'000'000 }));
		Array<double> samples;
		for (int32 i = 0; i < 21; ++i) samples.push_back(double(run(iterations)) / iterations);
		samples.sort();
		std::cout << "builder=" << name << ' ' << parameter << '=' << value << " cpu_ns=" << samples[10]
			<< " p10=" << samples[2] << " p90=" << samples[18] << '\n';
	}
}

TEST_CASE("Vertex2DBuilder.indices_cpu_benchmark", "[.benchmark]")
{
	const auto qos = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
	REQUIRE(qos);
	const ScopeExit restoreQoS{ [=] { pthread_override_qos_class_end_np(qos); } };
	const char* names[]{ "Circle", "Ellipse", "SuperEllipse", "TexturedCircle", "RoundRect", "RoundRectTB", "RoundRectLR", "TexturedRoundRect" };
	for (const auto shape : Shapes)
	{
		for (const float scale : { 1.0f, 512.0f })
		{
			BenchmarkBuilder(names[static_cast<int32>(shape)], "scale", static_cast<uint32>(scale),
				[&](const BufferCreatorFunc& allocate) { return BuildShape(allocate, shape, scale); });
		}
	}
	const char* indexedNames[]{ "Polygon", "RawPolygon", "TransformedPolygon", "Mesh" };
	for (const size_t triangleCount : { 2u, 21846u })
	{
		Array<TriangleIndex> triangles(triangleCount, Triangles[0]);
		Array<Index> indices(triangleCount * 3, Index{ 0 });
		for (int32 variant = 0; variant < 4; ++variant)
		{
			BenchmarkBuilder(indexedNames[variant], "triangles", static_cast<uint32>(triangleCount),
				[&](const BufferCreatorFunc& allocate) { return BuildIndexed(allocate, variant, triangles, indices); });
		}
	}
}

TEST_CASE("Vertex2DBuilder.ellipse_vertices_cpu_benchmark", "[.benchmark]")
{
	const auto qos = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
	REQUIRE(qos);
	const ScopeExit restoreQoS{ [=] { pthread_override_qos_class_end_np(qos); } };
	const char* names[]{ "CircleSolid", "CircleRadial", "CircleTB", "CircleLR",
		"EllipseSolid", "EllipseRadial", "EllipseTB", "EllipseLR", "TexturedCircle" };
	for (int32 variant = 0; variant < 9; ++variant)
	{
		const int32 paint = (variant % 4);
		const auto direction = ((paint == 2) ? ColorFillDirection::TopBottom
			: (paint == 3) ? ColorFillDirection::LeftRight : ColorFillDirection::InOut);
		const Float4& rimColor = ((paint == 0) ? Red : Blue);
		for (const float scale : { 1.0f, 512.0f })
		{
			BenchmarkBuilder(names[variant], "scale", static_cast<uint32>(scale), [&](const BufferCreatorFunc& allocate)
			{
				using namespace Vertex2DBuilder;
				if (variant < 4) return BuildCircle(allocate, { 3, 5 }, 1, direction, Red, rimColor, scale);
				if (variant < 8) return BuildEllipse(allocate, { 3, 5 }, 1, 0.5f, direction, Red, rimColor, scale);
				return BuildTexturedCircle(allocate, Circle{ 3, 5, 1 }, UV, Red, scale);
			});
		}
	}
}

TEST_CASE("Vertex2DBuilder.indices_rounded_cpu_benchmark", "[.benchmark]")
{
	const auto qos = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
	REQUIRE(qos);
	const ScopeExit restoreQoS{ [=] { pthread_override_qos_class_end_np(qos); } };
	const char* names[]{ "CirclePie", "EllipsePie", "CircleSegment", "ArcFlat", "ArcSquare", "ArcRoundRadial", "ArcRoundAlong",
		"CircleFrame", "EllipseFrame", "CircleDashed", "EllipseDashed", "RoundRectDashed",
		"RoundRectFrame", "RoundRectFrameTB", "RoundRectFrameLR", "ShadowFill", "ShadowRing", "LineRound", "ArrowRound",
		"LineStringRound", "LineStringGradientRound" };
	for (const auto shape : RoundedShapes)
	{
		for (const float scale : { 1.0f, 512.0f })
		{
			BenchmarkBuilder(names[static_cast<int32>(shape)], "scale", static_cast<uint32>(scale),
				[&](const BufferCreatorFunc& allocate) { return BuildRoundedShape(allocate, shape, scale); });
		}
	}
}

# endif
