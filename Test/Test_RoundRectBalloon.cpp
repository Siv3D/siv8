//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/RoundRectBalloon.hpp"
# include <chrono>
# include <iostream>
# if SIV3D_PLATFORM(MACOS)
# include <time.h>
# include <pthread/qos.h>
# endif

namespace
{
	namespace Balloon = detail::RoundRectBalloon;
	using Index = Vertex2D::IndexType;
	constexpr Float4 Ink{ 0.8f, 0.4f, 0.2f, 0.5f };
	constexpr Index Offset = 37;

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
		const auto allocate = [&](uint16 vertices, uint32 indices)
		{
			++requests;
			mesh.vertices.assign(vertices + size_t{ 2 }, guard);
			mesh.indices.assign(indices + size_t{ 2 }, Index{ 65535 });
			return Vertex2DBufferPointer{ mesh.vertices.data() + 1, mesh.indices.data() + 1, Offset };
		};
		const auto count = build(allocate);
		REQUIRE(requests == 1);
		REQUIRE(count + size_t{ 2 } == mesh.indices.size());
		for (const auto* v : { &mesh.vertices.front(), &mesh.vertices.back() })
		{
			CHECK(v->pos == guard.pos);
			CHECK(v->tex == guard.tex);
			CHECK(v->color == guard.color);
		}
		CHECK(mesh.indices.front() == 65535);
		CHECK(mesh.indices.back() == 65535);
		mesh.vertices.pop_back();
		mesh.vertices.erase(mesh.vertices.begin());
		mesh.indices.pop_back();
		mesh.indices.erase(mesh.indices.begin());
		for (const auto& v : mesh.vertices)
		{
			CHECK(v.color == Ink);
			CHECK(v.tex == guard.tex);
		}
		for (const auto index : mesh.indices)
		{
			CHECK(index >= Offset);
			CHECK(index < (Offset + mesh.vertices.size()));
		}
		return mesh;
	}

	struct SceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		bool vsync = Graphics::IsVSyncEnabled();
		MSAASampleCount msaa = Scene::GetMSAASampleCount();
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D mul{ Palette::White };
		ScopedColorAdd2D add{ 0.0 };
		ScopedRenderStates2D states{ BlendState::Default2D };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };

		SceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 320, 240 }));
			Scene::SetBackground(Palette::Black);
			Scene::SetMSAASampleCount(MSAASampleCount::X4MSAA);
		}
		~SceneScope()
		{
			Scene::Configure(config);
			Graphics::SetVSyncEnabled(vsync);
			Scene::SetMSAASampleCount(msaa);
			Scene::SetBackground(background);
		}
	};

	template <class Draw>
	Image Capture(Draw&& draw)
	{
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}
}

TEST_CASE("RoundRectBalloon.placement_and_omitted_tails")
{
	const RoundRect body{ 40, 40, 160, 100, 20 };
	struct Case { Vec2 tip, a, b; uint8 side; };
	const std::array cases{
		Case{ { 120, 0 }, { 100, 40 }, { 140, 40 }, 0 },
		Case{ { 240, 90 }, { 200, 70 }, { 200, 110 }, 1 },
		Case{ { 120, 180 }, { 140, 140 }, { 100, 140 }, 2 },
		Case{ { 0, 90 }, { 40, 110 }, { 40, 70 }, 3 },
		Case{ { 280, -10 }, { 200, 60 }, { 200, 100 }, 1 } };
	for (const auto& c : cases)
	{
		const auto p = Balloon::Place(body, c.tip, 40);
		REQUIRE(p);
		CHECK(p->side == c.side);
		CHECK(p->tip == c.tip);
		CHECK(p->base0 == c.a);
		CHECK(p->base1 == c.b);
	}
	const auto wide = Balloon::Place(body, { 240, 90 }, 1000);
	REQUIRE(wide);
	CHECK(wide->base0 == Vec2{ 200, 60 });
	CHECK(wide->base1 == Vec2{ 200, 120 });
	for (const Vec2 target : { Vec2{ 120, 90 }, Vec2{ 40, 40 }, Vec2{ 200, 140 }, Vec2{ 200, 90 } })
		CHECK_FALSE(Balloon::Place(body, target, 40));
	for (const double width : { 0.0, -10.0 }) CHECK_FALSE(Balloon::Place(body, { 240, 90 }, width));
	CHECK_FALSE(Balloon::Place(RoundRect{ 40, 40, 0, 100, 20 }, { 240, 90 }, 40));
	CHECK_FALSE(Balloon::Place(RoundRect{ 40, 40, 160, -1, 20 }, { 240, 90 }, 40));
	CHECK_FALSE(Balloon::Place(RoundRect{ 40, 40, 160, 100, 1000 }, { 240, 90 }, 40));
	const auto capsule = Balloon::Place(RoundRect{ 40, 40, 160, 100, -1000 }, { 120, 0 }, 100);
	REQUIRE(capsule);
	CHECK(capsule->body.r == 50);
	CHECK(capsule->base0 == Vec2{ 90, 40 });
	CHECK(capsule->base1 == Vec2{ 150, 40 });
	CHECK(Balloon::Place(RoundRect{ 40, 40, 160, 100, -20 }, { 240, 90 }, 40)->body.r == 20);
}

TEST_CASE("RoundRectBalloon.fill_mesh_partition_and_frame_bounds")
{
	for (const double radius : { 0.0, 20.0, 50.0 })
	for (const double width : { 0.1, 24.0, 1000.0 })
	for (const float scale : { 0.25f, 1.0f, 8.0f, 512.0f })
	for (const Vec2 target : { Vec2{ 120, 0 }, Vec2{ 250, 90 }, Vec2{ 120, 140.01 }, Vec2{ -10, -30 } })
	{
		CAPTURE(radius, width, scale, target);
		const auto p = Balloon::Place(RoundRect{ 40, 40, 160, 100, radius }, target, width);
		if (not p) continue;
		Balloon::Contour contour;
		Balloon::MakeContour(contour, *p, scale);
		REQUIRE(contour.count <= contour.vertices.size());
		REQUIRE(contour.count >= 5);
		CHECK(contour.vertices[contour.tipIndex] == target);
		double outlineArea = 0;
		for (size_t i = 0; i < contour.count; ++i)
		{
			const auto a = contour.vertices[i], b = contour.vertices[(i + 1) % contour.count];
			CHECK(a != b);
			outlineArea += a.cross(b) * 0.5;
		}
		const auto fill = CaptureMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildRoundRectBalloon(allocate, *p, Ink, scale);
		});
		CHECK(fill.vertices.size() <= 260);
		double trianglesArea = 0;
		for (size_t i = 0; i < fill.indices.size(); i += 3)
		{
			const Vec2 a = fill.vertices[fill.indices[i] - Offset].pos;
			const Vec2 b = fill.vertices[fill.indices[i + 1] - Offset].pos;
			const Vec2 c = fill.vertices[fill.indices[i + 2] - Offset].pos;
			const double area = (b - a).cross(c - a) * 0.5;
			CHECK(area >= -0.002);
			trianglesArea += area;
		}
		CHECK(std::abs(trianglesArea - outlineArea) < 0.02);
		for (const float thickness : { 1.0f, 8.0f, 160.0f })
		{
			const auto frame = CaptureMesh([&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildRoundRectBalloonFrame(allocate, *p, thickness, Ink, scale);
			});
			CHECK(frame.vertices.size() <= 2 * (contour.count + 3));
			for (const auto& v : frame.vertices)
			{
				CHECK(std::isfinite(v.pos.x));
				CHECK(std::isfinite(v.pos.y));
			}
		}
	}
}

TEST_CASE("RoundRectBalloon.allocation_failure")
{
	const auto p = Balloon::Place(RoundRect{ 40, 40, 160, 100, 20 }, { 240, 90 }, 40);
	REQUIRE(p);
	uint32 requests = 0;
	const auto fail = [&](uint16, uint32) { ++requests; return Vertex2DBufferPointer{}; };
	CHECK(Vertex2DBuilder::BuildRoundRectBalloon(fail, *p, Ink, 1) == 0);
	CHECK(requests == 1);
	CHECK(Vertex2DBuilder::BuildRoundRectBalloonFrame(fail, *p, 8, Ink, 1) == 0);
	CHECK(requests == 2);
}

TEST_CASE("RoundRectBalloon.render_alpha_seam_transform_and_fallback")
{
	const SceneScope scene;
	const RoundRect body{ 60, 50, 180, 100, 20 };
	const Vec2 target{ 280, 100 };
	const ColorF ink{ 1, 0.5 };
	const Image fill = Capture([&] { CHECK(&body.drawBalloon(target, 40, ink) == &body); });
	for (const auto pixel : fill) CHECK(pixel.r <= 129);
	for (const Point p : { Point{ 100, 100 }, Point{ 239, 100 }, Point{ 240, 100 }, Point{ 260, 100 } })
		CHECK(fill[p].r >= 127);
	const Image frame = Capture([&] { CHECK(&body.drawBalloonFrame(target, 40, 8, ink) == &body); });
	CHECK(frame[Point{ 240, 100 }].r == 0); // No edge across the tail base.
	CHECK(frame[Point{ 100, 100 }].r == 0);
	CHECK(frame[Point{ 60, 100 }].r >= 127);
	CHECK(frame[Point{ 260, 90 }].r >= 127);
	for (const bool stroke : { false, true })
	{
		const Image expected = Capture([&] { if (stroke) body.drawFrame(8, ink); else body.draw(ink); });
		for (const auto [tip, width] : { std::pair{ Vec2{ 60, 50 }, 40.0 }, std::pair{ target, 0.0 } })
		{
			const Image actual = Capture([&]
			{
				if (stroke) body.drawBalloonFrame(tip, width, 8, ink);
				else body.drawBalloon(tip, width, ink);
			});
			CHECK(actual == expected);
		}
	}
	const auto placement = Balloon::Place(body, target, 40);
	Balloon::Contour contour;
	Balloon::MakeContour(contour, *placement, 1);
	const Polygon reference{ contour.points(), SkipValidation::Yes };
	for (const Mat3x2 transform : { Mat3x2::Identity(), Mat3x2::Scale(-0.8, 0.7).translated(300, 30), Mat3x2::Rotate(0.1, { 160, 120 }) })
	{
		const Transformer2D local{ transform };
		const Image actual = Capture([&] { body.drawBalloon(target, 40, ink); });
		const Image expected = Capture([&] { reference.draw(ink); });
		int32 different = 0;
		for (size_t i = 0; i < actual.pixelCount(); ++i)
			if (std::abs(int32(actual.data()[i].r) - expected.data()[i].r) > 33) ++different;
		CHECK(different < 30); // Raster scaling can change the corner subdivision.
	}
	const Image blank = Capture([] {});
	CHECK(Capture([&]
	{
		RoundRect{ 60, 50, 0, 100, 20 }.drawBalloon(target, 40);
		RoundRect{ 60, 50, 180, -1, 20 }.drawBalloonFrame(target, 40, 8);
		body.drawBalloonFrame(target, 40, 0);
		body.drawBalloonFrame(target, 40, -8);
	}) == blank);
}

TEST_CASE("RoundRectBalloon.submission_benchmark", "[.benchmark]")
{
	const SceneScope scene;
	Graphics::SetVSyncEnabled(false);
# if SIV3D_PLATFORM(MACOS)
	struct QoS
	{
		pthread_override_t value = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
		~QoS() { if (value) pthread_override_qos_class_end_np(value); }
	} qos;
	const auto threadTime = []
	{
		timespec t{};
		REQUIRE(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t) == 0);
		return (int64(t.tv_sec) * 1'000'000'000 + t.tv_nsec);
	};
# endif
	constexpr int32 Calls = 128;
	for (const bool changing : { false, true })
	{
		Array<RoundRect> bodies;
		Array<Vec2> tips;
		Array<Polygon> cached;
		double directVertices = 0, polygonVertices = 0;
		const auto makePolygon = [](const RoundRect& body, const Vec2 tip)
		{
			const auto p = Balloon::Place(body, tip, 40);
			Balloon::Contour contour;
			Balloon::MakeContour(contour, *p, 1);
			return Polygon{ contour.points(), SkipValidation::Yes };
		};
		for (int32 i = 0; i < Calls; ++i)
		{
			bodies.emplace_back(40, 40, 160, 100, (changing ? 12 + i % 25 : 20));
			tips.emplace_back(260, (changing ? 30 + i % 120 : 90));
			cached.push_back(makePolygon(bodies.back(), tips.back()));
			polygonVertices += cached.back().vertices().size();
			directVertices += cached.back().outer().size() + 1;
		}
		std::cout << "RoundRectBalloon " << (changing ? "changing" : "static")
			<< " fill vertices direct=" << directVertices / Calls << " Polygon=" << polygonVertices / Calls << '\n';
		for (const bool frame : { false, true })
		{
			std::array<Array<double>, 3> times;
# if SIV3D_PLATFORM(MACOS)
			std::array<Array<double>, 3> cpuTimes;
# endif
			std::array<int64, 3> triangles{}, batches{};
			for (int32 round = -8; round < 21; ++round)
			for (int32 order = 0; order < 3; ++order)
			{
				const int32 method = ((round + order + 9) % 3);
				REQUIRE(System::Update());
				{
					const Transformer2D offscreen{ Mat3x2::Translate(2000, 2000) };
# if SIV3D_PLATFORM(MACOS)
					const auto cpuBegin = threadTime();
# endif
					const auto begin = std::chrono::steady_clock::now();
					for (int32 i = 0; i < Calls; ++i)
					{
						if (method == 0)
						{
							if (frame) bodies[i].drawBalloonFrame(tips[i], 40, 8);
							else bodies[i].drawBalloon(tips[i], 40);
						}
						else if (method == 1)
						{
							if (frame) makePolygon(bodies[i], tips[i]).drawFrame(8);
							else makePolygon(bodies[i], tips[i]).draw();
						}
						else if (frame) cached[i].drawFrame(8);
						else cached[i].draw();
					}
					const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / Calls;
# if SIV3D_PLATFORM(MACOS)
					const double cpuNs = double(threadTime() - cpuBegin) / Calls;
					if (round >= 0) cpuTimes[method].push_back(cpuNs);
# endif
					if (round >= 0) times[method].push_back(ns);
				}
				REQUIRE(System::Update());
				const auto metrics = Profiler::GetFrameMetrics();
				triangles[method] = metrics.triangleCount;
				batches[method] = metrics.drawCalls;
				CHECK(metrics.triangleCount > 0);
			}
			const char* names[]{ "direct", "temporary Polygon", "cached Polygon" };
			for (int32 method = 0; method < 3; ++method)
			{
				times[method].sort();
				std::cout << "RoundRectBalloon " << (changing ? "changing " : "static ") << (frame ? "frame " : "fill ")
					<< names[method] << " wall_ns=" << times[method][10] << " p10=" << times[method][2] << " p90=" << times[method][18];
# if SIV3D_PLATFORM(MACOS)
				cpuTimes[method].sort();
				std::cout << " cpu_ns=" << cpuTimes[method][10];
# endif
				std::cout << " triangles/call=" << double(triangles[method]) / Calls << " drawCalls/batch=" << batches[method] << '\n';
			}
		}
	}
	REQUIRE(System::Update());
}
