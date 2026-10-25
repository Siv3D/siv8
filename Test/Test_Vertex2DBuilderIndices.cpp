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
		std::array<Vertex2D, 512> vertices;
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

# endif
