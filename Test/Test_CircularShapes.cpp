//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Geometry2D/CircularShapes.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include <chrono>
# include <iostream>
# if SIV3D_PLATFORM(MACOS)
# include <pthread.h>
# include <pthread/qos.h>
# include <time.h>
# endif

namespace
{
	namespace CircularGeometry = detail::CircularShapes;
	using Index = Vertex2D::IndexType;
	constexpr Index Offset = 37;
	constexpr Float4 Ink{ 0.3f, 0.7f, 1.0f, 0.5f };

	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<Index> indices;
	};

	template <class Build>
	Mesh CaptureMesh(Build&& build)
	{
		Mesh mesh;
		Vertex2D guard;
		guard.set(-123, -456, -7, -8, Float4{ -1, -2, -3, -4 });
		uint32 requests = 0;
		const auto allocate = [&](uint16 vc, uint32 ic)
		{
			++requests;
			mesh.vertices.assign(vc + size_t{ 2 }, guard);
			mesh.indices.assign(ic + size_t{ 2 }, Index{ 65535 });
			return Vertex2DBufferPointer{ mesh.vertices.data() + 1, mesh.indices.data() + 1, Offset };
		};
		const auto count = build(allocate);
		REQUIRE(requests == 1);
		REQUIRE(count + size_t{ 2 } == mesh.indices.size());
		for (const auto* v : { &mesh.vertices.front(), &mesh.vertices.back() })
		{
			CHECK(v->pos == guard.pos); CHECK(v->tex == guard.tex); CHECK(v->color == guard.color);
		}
		CHECK(mesh.indices.front() == 65535); CHECK(mesh.indices.back() == 65535);
		mesh.vertices.pop_back(); mesh.vertices.erase(mesh.vertices.begin());
		mesh.indices.pop_back(); mesh.indices.erase(mesh.indices.begin());
		for (const auto& v : mesh.vertices)
		{
			CHECK(v.color == Ink); CHECK(v.tex == guard.tex);
			CHECK(std::isfinite(v.pos.x)); CHECK(std::isfinite(v.pos.y));
		}
		for (const Index index : mesh.indices) REQUIRE((Offset <= index && index < Offset + mesh.vertices.size()));
		return mesh;
	}

	double CheckMesh(const Mesh& mesh, const Shape2D& shape)
	{
		REQUIRE(mesh.vertices.size() == shape.vertices().size());
		REQUIRE(mesh.indices.size() == shape.indices().size() * 3);
		for (size_t i = 0; i < mesh.vertices.size(); ++i) CHECK(mesh.vertices[i].pos == shape.vertices()[i]);
		double area = 0;
		for (size_t i = 0; i < shape.indices().size(); ++i)
		{
			const auto t = shape.indices()[i];
			CHECK(mesh.indices[i * 3] == Offset + t.i0);
			CHECK(mesh.indices[i * 3 + 1] == Offset + t.i1);
			CHECK(mesh.indices[i * 3 + 2] == Offset + t.i2);
			const Vec2 a = shape.vertices()[t.i0], b = shape.vertices()[t.i1], c = shape.vertices()[t.i2];
			const double triangleArea = (b - a).cross(c - a) * 0.5;
			CHECK(triangleArea >= -1e-6);
			area += triangleArea;
		}
		double boundaryArea = 0;
		for (size_t i = 0; i < shape.vertices().size(); ++i)
		{
			const Vec2 a = shape.vertices()[i], b = shape.vertices()[(i + 1) % shape.vertices().size()];
			boundaryArea += a.cross(b) * 0.5;
		}
		CHECK(area == Catch::Approx(boundaryArea).margin(1e-7));
		const Polygon polygon{ shape };
		CHECK(polygon.area() == Catch::Approx(area).margin(1e-5));
		return area;
	}

	bool Inside(const Vec2 p, const Array<Float2>& boundary)
	{
		bool inside = false;
		for (size_t i = 0, j = boundary.size() - 1; i < boundary.size(); j = i++)
		{
			const Vec2 a = boundary[i], b = boundary[j];
			if (((a.y > p.y) != (b.y > p.y)) && (p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x))
				inside = not inside;
		}
		return inside;
	}

	void CheckCoverage(const Shape2D& shape)
	{
		const RectF bounds = shape.boundingRect();
		for (int32 y = 0; y < 31; ++y)
		for (int32 x = 0; x < 31; ++x)
		{
			const Vec2 p{ bounds.x + bounds.w * ((x + 0.371) / 31), bounds.y + bounds.h * ((y + 0.619) / 31) };
			int32 coverage = 0;
			bool edge = false;
			for (const auto t : shape.indices())
			{
				const Vec2 a = shape.vertices()[t.i0], b = shape.vertices()[t.i1], c = shape.vertices()[t.i2];
				const double s0 = (b - a).cross(p - a), s1 = (c - b).cross(p - b), s2 = (a - c).cross(p - c);
				if ((Abs(s0) < 1e-6) || (Abs(s1) < 1e-6) || (Abs(s2) < 1e-6)) edge = true;
				if ((0 < s0) && (0 < s1) && (0 < s2)) ++coverage;
			}
			if (not edge) CHECK(coverage == int32(Inside(p, shape.vertices())));
		}
	}

	struct SceneScope
	{
		const SceneConfig config = Scene::GetConfig();
		const ColorF background = Scene::GetBackground();
		const bool vsync = Graphics::IsVSyncEnabled();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedRenderStates2D blend{ BlendState::Default2D };
		ScopedColorMul2D mul{ Palette::White };
		ScopedColorAdd2D add{ 0.0 };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };
		SceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 480, 360 }));
			Scene::SetBackground(Palette::Black);
		}
		~SceneScope()
		{
			Scene::Configure(config); Scene::SetBackground(background); Graphics::SetVSyncEnabled(vsync);
		}
	};

	template <class Draw>
	Image Capture(Draw&& draw)
	{
		REQUIRE(System::Update()); draw(); ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update()); REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}
}

TEST_CASE("CircularShapes.crescent_geometry_mesh_and_quality")
{
	for (const double radius : { -64.0, 64.0 })
	for (const double width : { 0.01, 1.0, 36.0, 64.0, 120.0, 127.999 })
	for (const double angle : { -0.7, 0.0, Math::HalfPi })
	for (const float quality : { 0.0f, 1.0f, 16.0f })
	{
		CAPTURE(radius, width, angle, quality);
		const Circle circle{ 20, -30, radius };
		const auto shape = Shape2D::Crescent(circle, angle, width, QualityFactor{ quality });
		const auto mesh = CaptureMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildCircleCrescent(allocate, circle, angle, width, Ink, quality);
		});
		CHECK(shape.vertices().size() <= 504);
		CHECK(shape.indices().size() + 2 == shape.vertices().size());
		const double area = CheckMesh(mesh, shape);
		const Vec2 axis{ std::sin(angle), -std::cos(angle) }, cut = circle.center + axis * width;
		const auto placement = CircularGeometry::PlaceCrescent(circle, angle, width);
		REQUIRE(placement); REQUIRE_FALSE(placement->circleOnly);
		const size_t n = shape.vertices().size() / 2;
		for (size_t i = 0; i < shape.vertices().size(); ++i)
		{
			const Vec2 p = shape.vertices()[i];
			CHECK(p.distanceFrom((i <= n) ? circle.center : cut) == Catch::Approx(64).margin(1e-5));
		}
		const Vec2 perpendicular{ -axis.y, axis.x };
		const Vec2 origin = circle.center + axis * (width * 0.5);
		CHECK(shape.vertices()[0] == Float2{ origin + perpendicular * placement->halfChord });
		CHECK(shape.vertices()[n] == Float2{ origin - perpendicular * placement->halfChord });
		if (quality == 16)
		{
			const double expected = (2 * 64 * 64 * std::asin(width / 128) + width * std::sqrt(64 * 64 - width * width / 4));
			CHECK(area == Catch::Approx(expected).epsilon(0.001));
		}
	}
}

TEST_CASE("CircularShapes.teardrop_tangents_mesh_and_quality")
{
	for (const double radius : { -32.0, 32.0 })
	for (const double distance : { 32.01, 40.0, 100.0, 1000.0 })
	for (const double angle : { -0.7, 0.0, 1.8 })
	for (const float quality : { 0.0f, 1.0f, 16.0f })
	{
		CAPTURE(radius, distance, angle, quality);
		const Circle circle{ -10, 30, radius };
		const Vec2 tip = circle.center + Vec2{ std::cos(angle), std::sin(angle) } * distance;
		const auto shape = Shape2D::Teardrop(circle, tip, QualityFactor{ quality });
		const auto mesh = CaptureMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildCircleTeardrop(allocate, circle, tip, Ink, quality);
		});
		CHECK(shape.vertices().size() <= 254);
		const double area = CheckMesh(mesh, shape);
		CHECK(shape.vertices().front() == Float2{ tip });
		for (size_t i = 1; i < shape.vertices().size(); ++i)
			CHECK(Vec2{ shape.vertices()[i] }.distanceFrom(circle.center) == Catch::Approx(32).margin(1e-5));
		for (const Vec2 root : { Vec2{ shape.vertices()[1] }, Vec2{ shape.vertices().back() } })
			CHECK(Abs((root - circle.center).dot(tip - root)) < 0.02);
		if (quality == 16)
		{
			const double expected = (32 * 32 * (Math::Pi - std::acos(32 / distance)) + 32 * std::sqrt(distance * distance - 32 * 32));
			CHECK(area == Catch::Approx(expected).epsilon(0.001));
		}
	}
}

TEST_CASE("CircularShapes.single_coverage_and_image_outline")
{
	for (const double width : { 0.1, 24.0, 64.0, 127.9 })
		CheckCoverage(Shape2D::Crescent(Circle{ 0, 0, 64 }, 0.37, width));
	for (const Vec2 tip : { Vec2{ 0, -64.1 }, Vec2{ 100, 150 }, Vec2{ -1000, 0 } })
		CheckCoverage(Shape2D::Teardrop(Circle{ 0, 0, 64 }, tip));
	for (const auto& shape : { Shape2D::Crescent(Circle{ 120, 120, 60 }, Math::HalfPi, 30),
		Shape2D::Teardrop(Circle{ 120, 100, 35 }, { 120, 190 }) })
	{
		Image image{ 240, 240, Palette::Black };
		shape.paint(image, Palette::White, EnableAntialiasing::No);
		int32 white = 0;
		for (const auto pixel : image) if (pixel.r == 255) ++white;
		CHECK(white > 500);
		const Polygon polygon{ shape };
		CHECK(polygon.outer().size() == shape.vertices().size());
	}
}

TEST_CASE("CircularShapes.empty_circle_fallback_and_allocation_failure")
{
	const Circle circle{ 10, 20, 50 };
	for (const double width : { -1.0, 0.0 })
		CHECK(Shape2D::Crescent(circle, 1, width).vertices().empty());
	CHECK(Shape2D::Crescent(Circle{ 0, 0, 0 }, 0, 30).vertices().empty());
	CHECK(Shape2D::Teardrop(Circle{ 0, 0, 0 }, { 20, 20 }).vertices().empty());
	for (const double width : { 100.0, 200.0 })
	{
		const auto p = CircularGeometry::PlaceCrescent(circle, 0.7, width);
		REQUIRE(p); CHECK(p->circleOnly);
		const Polygon polygon{ Shape2D::Crescent(circle, 0.7, width) };
		CHECK(polygon.area() == Catch::Approx(circle.area()).epsilon(0.01));
	}
	for (const Vec2 tip : { Vec2{ 10, 20 }, Vec2{ 10, -30 }, Vec2{ 40, 60 } })
	{
		const auto p = CircularGeometry::PlaceTeardrop(circle, tip);
		REQUIRE(p); CHECK(p->circleOnly);
		const Polygon polygon{ Shape2D::Teardrop(circle, tip) };
		CHECK(polygon.area() == Catch::Approx(circle.area()).epsilon(0.01));
	}
	uint32 calls = 0;
	const auto failure = [&](uint16, uint32) { ++calls; return Vertex2DBufferPointer{}; };
	CHECK(Vertex2DBuilder::BuildCircleCrescent(failure, circle, 0, 0, Ink, 1) == 0);
	CHECK(Vertex2DBuilder::BuildCircleTeardrop(failure, Circle{ 0, 0, 0 }, { 20, 20 }, Ink, 1) == 0);
	CHECK(calls == 0);
	CHECK(Vertex2DBuilder::BuildCircleCrescent(failure, circle, 0, 30, Ink, 1) == 0);
	CHECK(Vertex2DBuilder::BuildCircleTeardrop(failure, circle, { 100, 120 }, Ink, 1) == 0);
	CHECK(Vertex2DBuilder::BuildCircleCrescent(failure, circle, 0, 100, Ink, 1) == 0);
	CHECK(Vertex2DBuilder::BuildCircleTeardrop(failure, circle, circle.center, Ink, 1) == 0);
	CHECK(calls == 4);
}

TEST_CASE("CircularShapes.public_rendering_transform_alpha_and_fallback")
{
	const SceneScope scope;
	const Circle circle{ 230, 170, 64 };
	const Vec2 tip{ 340, 290 };
	const ColorF ink{ 1.0, 0.5 };
	for (const Mat3x2 matrix : { Mat3x2::Identity(), Mat3x2{ -0.8f, 0.1f, 0.15f, 0.9f, 400, 10 } })
	for (const bool crescent : { false, true })
	{
		const auto direct = Capture([&]
		{
			const Transformer2D t{ matrix };
			if (crescent) CHECK(&circle.drawCrescent(0.7, 36, ink) == &circle);
			else CHECK(&circle.drawTeardrop(tip, ink) == &circle);
		});
		const auto saved = Capture([&]
		{
			const Transformer2D t{ matrix };
			const auto q = QualityFactor::FromSceneScaling();
			const Shape2D shape = (crescent ? Shape2D::Crescent(circle, 0.7, 36, q) : Shape2D::Teardrop(circle, tip, q));
			shape.draw(ink);
		});
		CHECK(direct == saved);
		uint8 maximum = 0;
		for (const auto pixel : direct) maximum = Max(maximum, pixel.r);
		CHECK(maximum == 128);
	}
	const auto disk = Capture([&] { circle.draw(ink); });
	CHECK(disk == Capture([&] { circle.drawCrescent(0.3, 128, ink); }));
	CHECK(disk == Capture([&] { circle.drawTeardrop(circle.center, ink); }));
	const auto blank = Capture([] {});
	CHECK(blank == Capture([&]
	{
		circle.drawCrescent(0, 0);
		Circle{ 0, 0, 0 }.drawTeardrop({ 30, 40 });
		Circle{ 0, 0, 0 }.drawCrescent(0, 10);
	}));
	const Circle negative{ circle.center, -circle.r };
	CHECK(Capture([&] { circle.drawCrescent(0.3, 36, ink); })
		== Capture([&] { negative.drawCrescent(0.3, 36, ink); }));
	CHECK(Capture([&] { circle.drawTeardrop(tip, ink); })
		== Capture([&] { negative.drawTeardrop(tip, ink); }));
}

TEST_CASE("CircularShapes.submission_benchmark", "[.benchmark]")
{
	const SceneScope scope;
	Graphics::SetVSyncEnabled(false);
# if SIV3D_PLATFORM(MACOS)
	struct QoS
	{
		pthread_override_t value = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
		~QoS() { if (value) pthread_override_qos_class_end_np(value); }
	} qos;
	const auto cpuTime = []
	{
		timespec t{};
		REQUIRE(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t) == 0);
		return (int64(t.tv_sec) * 1'000'000'000 + t.tv_nsec);
	};
# endif
	const char* methods[]{ "direct", "temporaryShape", "cachedShape", "cachedPolygon" };
	for (const bool crescent : { true, false })
	for (const double radius : { 64.0, 1000.0 })
	for (const bool changing : { false, true })
	{
		const Circle circle{ 230, 170, radius };
		Array<Shape2D> shapes;
		Array<Polygon> polygons;
		for (int32 i = 0; i < 32; ++i)
		{
			const double phase = i / 31.0;
			const Vec2 tip = circle.center + Vec2{ 1.3 + phase, 1.8 } * radius;
			shapes.push_back(crescent ? Shape2D::Crescent(circle, 0.7 + phase, radius * (0.2 + phase))
				: Shape2D::Teardrop(circle, tip));
			polygons.emplace_back(shapes.back());
		}
		std::array<Array<double>, 4> wall;
# if SIV3D_PLATFORM(MACOS)
		std::array<Array<double>, 4> cpu;
# endif
		std::array<int64, 4> triangles{}, draws{};
		for (int32 round = -8; round < 21; ++round)
		for (int32 offset = 0; offset < 4; ++offset)
		{
			const int32 method = ((round + offset + 8) % 4);
			REQUIRE(System::Update());
			{
				const Transformer2D offscreen{ Mat3x2::Translate(10000, 10000) };
# if SIV3D_PLATFORM(MACOS)
				const int64 cpuBegin = cpuTime();
# endif
				const auto begin = std::chrono::steady_clock::now();
				for (int32 i = 0; i < 128; ++i)
				{
					const int32 index = (changing ? (i % 32) : 0);
					const double phase = index / 31.0;
					const Vec2 tip = circle.center + Vec2{ 1.3 + phase, 1.8 } * radius;
					if (method == 0)
					{
						if (crescent) circle.drawCrescent(0.7 + phase, radius * (0.2 + phase));
						else circle.drawTeardrop(tip);
					}
					else if (method == 1)
					{
						if (crescent) Shape2D::Crescent(circle, 0.7 + phase, radius * (0.2 + phase)).draw();
						else Shape2D::Teardrop(circle, tip).draw();
					}
					else if (method == 2) shapes[index].draw();
					else polygons[index].draw();
				}
				const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / 128;
# if SIV3D_PLATFORM(MACOS)
				const double cpuNs = double(cpuTime() - cpuBegin) / 128;
				if (round >= 0) cpu[method].push_back(cpuNs);
# endif
				if (round >= 0) wall[method].push_back(ns);
			}
			REQUIRE(System::Update());
			const auto metrics = Profiler::GetFrameMetrics();
			if (round == 0) { triangles[method] = metrics.triangleCount; draws[method] = metrics.drawCalls; }
			if (round >= 0) { CHECK(metrics.triangleCount == triangles[method]); CHECK(metrics.drawCalls == draws[method]); }
		}
		for (int32 method = 0; method < 4; ++method)
		{
			CHECK(triangles[method] == triangles[0]);
			CHECK(draws[method] == draws[0]);
			wall[method].sort();
			std::cout << "CircularShapes " << (crescent ? "crescent" : "teardrop") << " r=" << radius
				<< (changing ? " changing " : " fixed ") << methods[method] << " ns/call=" << wall[method][10]
				<< " p10=" << wall[method][2] << " p90=" << wall[method][18]
				<< " triangles/call=" << double(triangles[method]) / 128 << " draws/batch=" << draws[method];
# if SIV3D_PLATFORM(MACOS)
			cpu[method].sort();
			std::cout << " cpu_ns/call=" << cpu[method][10] << " cpu_p10=" << cpu[method][2] << " cpu_p90=" << cpu[method][18];
# endif
			std::cout << '\n';
		}
	}
}
