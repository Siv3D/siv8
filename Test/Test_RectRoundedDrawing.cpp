//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include <chrono>
# include <iostream>
# if SIV3D_PLATFORM(MACOS)
# include <time.h>
# endif

namespace
{
	using Radii = std::array<float, 4>;
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

	double Cross(const Vec2 a, const Vec2 b, const Vec2 c)
	{
		return (b - a).cross(c - a);
	}

	double CheckTriangles(const Mesh& mesh)
	{
		double area = 0;
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			const Vec2 a = mesh.vertices[mesh.indices[i] - Offset].pos;
			const Vec2 b = mesh.vertices[mesh.indices[i + 1] - Offset].pos;
			const Vec2 c = mesh.vertices[mesh.indices[i + 2] - Offset].pos;
			const double twiceArea = Cross(a, b, c);
			CHECK(twiceArea >= -0.002);
			area += twiceArea * 0.5;
		}
		return area;
	}

	double ContourArea(const Mesh& mesh, const size_t stride, const size_t first)
	{
		double twiceArea = 0;
		for (size_t i = first; i < mesh.vertices.size(); i += stride)
		{
			const size_t next = ((i + stride < mesh.vertices.size()) ? i + stride : first);
			twiceArea += Vec2{ mesh.vertices[i].pos }.cross(Vec2{ mesh.vertices[next].pos });
		}
		return twiceArea * 0.5;
	}

	// Independent test oracle for the documented proportional fit.
	Radii Fit(Radii radii, const float w, const float h)
	{
		for (auto& r : radii) r = Max(r, 0.0f);
		float ratio = 1;
		const float sides[]{ w, h, w, h };
		for (size_t i = 0; i < 4; ++i)
		{
			const float sum = radii[i] + radii[(i + 1) % 4];
			if (sum > sides[i]) ratio = Min(ratio, sides[i] / sum);
		}
		for (auto& r : radii) r *= ratio;
		return radii;
	}

	struct SceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		MSAASampleCount msaa = Scene::GetMSAASampleCount();
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D mul{ Palette::White, ScopedColorMul2D::Target::Set };
		ScopedColorAdd2D add{ 0.0, ScopedColorAdd2D::Target::Set };
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

TEST_CASE("RectRoundedDrawing.mesh_contours_and_budget")
{
	const FloatRect rect{ 17, 23, 217, 143 };
	for (const Radii radii : { Radii{ 28, 6, 28, 6 }, Radii{ 0, 0, 0, 0 }, Radii{ 120, 0, 0, 0 },
		Radii{ 100, 100, 20, 20 }, Radii{ 0, 120, 0, 120 } })
	for (const float scale : { 0.0f, 1.0f, 8.0f, 512.0f })
	{
		CAPTURE(radii, scale);
		const auto mesh = CaptureMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildRoundRect(allocate, rect, radii, Ink, scale);
		});
		CHECK(mesh.vertices.size() <= 256);
		CHECK(mesh.indices.size() == (mesh.vertices.size() - 2) * 3);
		const double area = CheckTriangles(mesh);
		CHECK(std::abs(area - ContourArea(mesh, 1, 0)) < 0.01);
		// Every emitted vertex lies on the intended boundary, not a half-size clamp.
		for (const auto& vertex : mesh.vertices)
		{
			const Vec2 p = vertex.pos;
			double distance = Min({ std::abs(p.x - rect.left), std::abs(p.x - rect.right),
				std::abs(p.y - rect.top), std::abs(p.y - rect.bottom) });
			const Vec2 centers[]{ { rect.left + radii[0], rect.top + radii[0] },
				{ rect.right - radii[1], rect.top + radii[1] }, { rect.right - radii[2], rect.bottom - radii[2] },
				{ rect.left + radii[3], rect.bottom - radii[3] } };
			for (size_t i = 0; i < 4; ++i) distance = Min(distance, std::abs(p.distanceFrom(centers[i]) - radii[i]));
			CHECK(distance < 0.0001);
			CHECK(p.x >= rect.left - 0.0001);
			CHECK(p.x <= rect.right + 0.0001);
			CHECK(p.y >= rect.top - 0.0001);
			CHECK(p.y <= rect.bottom + 0.0001);
		}
		if (scale == 512)
		{
			double exactArea = 200 * 120;
			for (auto r : radii) exactArea -= (1 - Math::Pi / 4) * r * r;
			CHECK(std::abs(area - exactArea) < 5);
		}
	}
}

TEST_CASE("RectRoundedDrawing.frame_topology_and_collapsed_corners")
{
	for (const Radii radii : { Radii{ 28, 6, 28, 6 }, Radii{ 0, 0, 0, 0 }, Radii{ 80, 0, 0, 0 },
		Radii{ 70, 30, 50, 10 }, Radii{ 0, 80, 0, 80 } })
	for (const float inset : { 0.0f, 5.0f, 20.0f, 39.5f })
	for (const float outset : { 0.0f, 7.0f })
	{
		if ((inset == 0) && (outset == 0)) continue;
		CAPTURE(radii, inset, outset);
		Radii inner = radii, outer = radii;
		for (auto& r : inner) r -= inset;
		inner = Fit(inner, 100 - 2 * inset, 80 - 2 * inset);
		for (auto& r : outer) r += outset;
		const auto mesh = CaptureMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildRoundRectFrame(allocate,
				FloatRect{ inset, inset, 100 - inset, 80 - inset }, inner,
				FloatRect{ -outset, -outset, 100 + outset, 80 + outset }, outer, Ink, 1);
		});
		CHECK(mesh.vertices.size() <= 512);
		CHECK(mesh.indices.size() == mesh.vertices.size() * 3);
		const double area = CheckTriangles(mesh);
		CHECK(std::abs(area - (ContourArea(mesh, 2, 0) - ContourArea(mesh, 2, 1))) < 0.01);
		// Count strict triangle interiors on an offset grid; shared edges are excluded.
		uint32 maxCoverage = 0;
		for (double y = -outset + 0.37; y < 80 + outset; y += 3.1)
		for (double x = -outset + 0.19; x < 100 + outset; x += 3.1)
		{
			uint32 coverage = 0;
			const Vec2 p{ x, y };
			for (size_t i = 0; i < mesh.indices.size(); i += 3)
			{
				const Vec2 a = mesh.vertices[mesh.indices[i] - Offset].pos;
				const Vec2 b = mesh.vertices[mesh.indices[i + 1] - Offset].pos;
				const Vec2 c = mesh.vertices[mesh.indices[i + 2] - Offset].pos;
				coverage += ((Cross(a, b, p) > 1e-5) && (Cross(b, c, p) > 1e-5) && (Cross(c, a, p) > 1e-5));
			}
			maxCoverage = Max(maxCoverage, coverage);
		}
		CHECK(maxCoverage == 1);
	}
}

TEST_CASE("RectRoundedDrawing.buffer_allocation_failure")
{
	int32 calls = 0;
	const auto fail = [&](uint16, uint32) { ++calls; return Vertex2DBufferPointer{}; };
	const Radii outer{ 30, 8, 12, 0 }, inner{ 25, 3, 7, 0 };
	CHECK(Vertex2DBuilder::BuildRoundRect(fail, FloatRect{ 0, 0, 100, 80 }, outer, Ink, 1) == 0);
	CHECK(Vertex2DBuilder::BuildRoundRectFrame(fail, FloatRect{ 5, 5, 95, 75 }, inner,
		FloatRect{ 0, 0, 100, 80 }, outer, Ink, 1) == 0);
	CHECK(calls == 2);
}

TEST_CASE("RectRoundedDrawing.public_normalization_and_empty_inputs")
{
	const SceneScope scene;
	const RectF rect{ 60, 70, 120, 80 };
	const ColorF color{ 0.8, 0.4, 0.2, 0.5 };
	CHECK(Capture([&] { CHECK(&rect.drawRounded(120, 80, 0, -10, color) == &rect); })
		== Capture([&] { rect.drawRounded(72, 48, 0, 0, color); }));
	CHECK(Capture([&] { rect.drawRoundedFrame(120, 80, 0, -10, 10, 4, color); })
		== Capture([&] { rect.drawRoundedFrame(72, 48, 0, 0, 10, 4, color); }));
	const Image blank = Capture([] {});
	CHECK(Capture([&]
	{
		for (const RectF empty : { RectF{ 60, 70, 0, 80 }, RectF{ 60, 70, 120, 0 }, RectF{ 60, 70, -120, 80 } })
		{
			empty.drawRounded(28, 6, 28, 6);
			empty.drawRoundedFrame(28, 6, 28, 6, 5, 5);
		}
		rect.drawRoundedFrame(28, 6, 28, 6, 0);
		rect.drawRoundedFrame(28, 6, 28, 6, -2);
		rect.drawRoundedFrame(28, 6, 28, 6, -1, 5);
		rect.drawRoundedFrame(28, 6, 28, 6, 5, -1);
	}) == blank);
	CHECK(Capture([&] { rect.drawRounded(0, -5, 0, -2, color); }) == Capture([&] { rect.draw(color); }));
	for (double r : { 0.0, 12.0, 40.0 })
	{
		CHECK(Capture([&] { rect.drawRounded(r, r, r, r, color); }) == Capture([&] { rect.rounded(r).draw(color); }));
		CHECK(Capture([&] { rect.drawRoundedFrame(r, r, r, r, 6, 4, color); }) == Capture([&] { rect.rounded(r).drawFrame(6, 4, color); }));
	}
	CHECK(Capture([&] { rect.drawRoundedFrame(80, 0, 0, 0, 40, 5, color); })
		== Capture([&] { rect.stretched(5).drawRounded(85, 5, 5, 5, color); }));
	CHECK(Capture([&] { rect.drawRoundedFrame(28, 6, 28, 6); })
		== Capture([&] { rect.drawRoundedFrame(28, 6, 28, 6, 0.5, 0.5); }));
}

TEST_CASE("RectRoundedDrawing.public_rect_transform_and_transparency")
{
	const SceneScope scene;
	const Rect rect{ 15, 20, 100, 80 };
	const RectF rectF{ rect };
	const ColorF color{ 1, 1, 1, 0.5 };
	for (const auto resolution : { SceneResolution::SceneSize, SceneResolution::Display })
	for (const Mat3x2 matrix : { Mat3x2::Translate(60, 50), Mat3x2{ 1.6f, 0.1f, 0.25f, 1.2f, 15, 20 } })
	{
		Scene::Configure(SceneConfig::Fixed({ 320, 240 }, resolution));
		const Transformer2D local{ matrix, Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Scale(1.1f, 0.9f), Transformer2D::Target::SetCamera };
		for (const bool frame : { false, true })
		{
			const Image actual = Capture([&]
			{
				if (frame) CHECK(&rect.drawRoundedFrame(80, 0, 0, 0, 38, 4, color) == &rect);
				else CHECK(&rect.drawRounded(80, 0, 0, 0, color) == &rect);
			});
			CHECK(actual == Capture([&]
			{
				if (frame) CHECK(&rectF.drawRoundedFrame(80, 0, 0, 0, 38, 4, color) == &rectF);
				else CHECK(&rectF.drawRounded(80, 0, 0, 0, color) == &rectF);
			}));
			uint8 maximum = 0;
			for (const Color pixel : actual) maximum = Max(maximum, pixel.r);
			CHECK(maximum >= 127);
			CHECK(maximum <= 129);
		}
	}
}

TEST_CASE("RectRoundedDrawing.submission_benchmark", "[.benchmark]")
{
	const SceneScope scene;
	const RectF rect{ 20, 20, 200, 90 };
	constexpr int32 Calls = 256;
	Array<Polygon> cached, sameMesh;
	double polygonVertices = 0, directVertices = 0;
	for (int32 i = 0; i < Calls; ++i)
	{
		cached.push_back(rect.rounded(15 + i % 50, 6, 28, 6));
		const auto mesh = CaptureMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildRoundRect(allocate, FloatRect{ rect }, Radii{ float(15 + i % 50), 6, 28, 6 }, Ink, 1);
		});
		Array<Vec2> points;
		Array<TriangleIndex> indices;
		for (const auto& v : mesh.vertices) points.push_back(v.pos);
		for (size_t k = 0; k < mesh.indices.size(); k += 3)
		{
			indices.push_back({ Index(mesh.indices[k] - Offset), Index(mesh.indices[k + 1] - Offset), Index(mesh.indices[k + 2] - Offset) });
		}
		sameMesh.emplace_back(points, std::move(indices), rect, SkipValidation::Yes);
		polygonVertices += cached.back().vertices().size();
		directVertices += mesh.vertices.size();
	}
	std::cout << "RectRoundedDrawing mean fill vertices: direct=" << directVertices / Calls
		<< " rounded Polygon=" << polygonVertices / Calls << '\n';
	for (const bool frame : { false, true })
	{
		std::array<Array<double>, 4> times;
# if SIV3D_PLATFORM(MACOS)
		const auto threadTime = []
		{
			timespec t{};
			REQUIRE(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t) == 0);
			return (int64(t.tv_sec) * 1'000'000'000 + t.tv_nsec);
		};
		std::array<Array<double>, 4> cpuTimes;
# endif
		std::array<int64, 4> triangles{}, drawCalls{};
		const int32 methods = (frame ? 3 : 4);
		for (int32 round = -3; round < 11; ++round)
		for (int32 order = 0; order < methods; ++order)
		{
			const int32 method = ((round + order + methods) % methods);
			REQUIRE(System::Update());
			{
				const Transformer2D offscreen{ Mat3x2::Translate(2000, 2000) };
# if SIV3D_PLATFORM(MACOS)
				const int64 cpuBegin = threadTime();
# endif
				const auto begin = std::chrono::steady_clock::now();
				for (int32 i = 0; i < Calls; ++i)
				{
					if (method == 0)
					{
						if (frame) rect.drawRoundedFrame(15 + i % 50, 6, 28, 6, 8);
						else rect.drawRounded(15 + i % 50, 6, 28, 6);
					}
					else if (method == 1)
					{
						if (frame) rect.rounded(15 + i % 50, 6, 28, 6).drawFrame(8);
						else rect.rounded(15 + i % 50, 6, 28, 6).draw();
					}
					else if (method == 3) sameMesh[i].draw();
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
			if (round >= 0)
			{
				triangles[method] = metrics.triangleCount;
				drawCalls[method] = metrics.drawCalls;
				CHECK(metrics.triangleCount > 0);
			}
		}
		const char* names[]{ "direct", "temporary Polygon", "cached Polygon", "cached identical mesh" };
		for (int32 method = 0; method < methods; ++method)
		{
			times[method].sort();
			std::cout << "RectRoundedDrawing " << (frame ? "frame" : "fill") << ' ' << names[method]
				<< " wall_ns=" << times[method][5] << " p10=" << times[method][1] << " p90=" << times[method][9];
# if SIV3D_PLATFORM(MACOS)
			cpuTimes[method].sort();
			std::cout << " cpu_ns=" << cpuTimes[method][5];
# endif
			std::cout << " triangles/call=" << double(triangles[method]) / Calls << " drawCalls/batch=" << drawCalls[method] << '\n';
		}
		if (not frame) CHECK(triangles[0] == triangles[3]);
	}
	REQUIRE(System::Update());
}
