//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include "../Siv3D/src/Siv3D/Geometry2D/DashPattern.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/IRenderer2D.hpp"
# include "../Siv3D/src/Siv3D/Engine/Siv3DEngine.hpp"
# include <array>
# include <atomic>
# include <chrono>
# include <iostream>
# if SIV3D_PLATFORM(MACOS)
# include <pthread.h>
# include <pthread/qos.h>
# endif

namespace
{
	constexpr uint16 BaseIndex = 37;
	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<uint16> indices;
		uint16 count = 0;
		uint32 allocations = 0;
	};

	Mesh Build(const LineStyle& style, const Float2 axes = { 80, 80 }, const float start = 0,
		const float sweep = 4.5f, const float inner = 3, const float outer = 5, const bool fail = false, const float scale = 1)
	{
		Mesh result;
		const auto allocate = [&](const uint16 v, const uint32 i)
		{
			++result.allocations;
			if (fail) return Vertex2DBufferPointer{};
			result.vertices.resize(size_t(v) + 2);
			result.indices.assign(size_t(i) + 2, 65535);
			result.vertices.front().pos = result.vertices.back().pos = Float2{ -1234, -5678 };
			return Vertex2DBufferPointer{ result.vertices.data() + 1, result.indices.data() + 1, BaseIndex };
		};
		result.count = Vertex2DBuilder::BuildArcStyled(allocate, style, { 0, 0 }, axes.x, axes.y, start, sweep, inner, outer, Float4{ 1, 0.5f, 0.25f, 0.5f }, scale);
		return result;
	}

	Vec2 MiddleAt(const Vec2 axes, const double t, const double offset)
	{
		const Vec2 normal = Vec2{ std::sin(t) / axes.x, -std::cos(t) / axes.y }.normalized();
		return Vec2{ axes.x * std::sin(t), -axes.y * std::cos(t) } + normal * offset;
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
		SceneScope() { Scene::Configure(SceneConfig::Fixed({ 480, 360 })); Scene::SetBackground(Palette::Black); }
		~SceneScope() { Scene::Configure(config); Scene::SetBackground(background); Graphics::SetVSyncEnabled(vsync); }
	};

	template <class Draw>
	Image Capture(Draw draw)
	{
		REQUIRE(System::Update()); draw(); ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update()); REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}
}

TEST_CASE("StyledArc.upload_bounds_normals_and_winding")
{
	for (const auto preset : { LineStyle::Dotted, LineStyle::DashedFlat, LineStyle::LongDash, LineStyle::DashDot, LineStyle::RoundDot, LineStyle::RoundDotFlat })
	for (const Float2 axes : { Float2{ 80, 80 }, Float2{ 80, 40 }, Float2{ 80, 5 } })
	for (const float sweep : { 0.001f, 2.1f, -4.7f, Math::TwoPiF, -Math::TwoPiF })
	for (const float scale : { 0.5f, 4.0f })
	{
		const auto mesh = Build(preset(-0.375), axes, 0.4f, sweep, 0.1f, 1.9f, false, scale);
		if (not mesh.count) continue;
		CHECK(mesh.allocations == 1);
		CHECK(mesh.vertices.front().pos == Float2{ -1234, -5678 });
		CHECK(mesh.vertices.back().pos == Float2{ -1234, -5678 });
		CHECK(mesh.indices.front() == 65535); CHECK(mesh.indices.back() == 65535);
		for (size_t i = 1; i <= mesh.count; ++i)
		{
			REQUIRE(mesh.indices[i] >= BaseIndex);
			REQUIRE(mesh.indices[i] < BaseIndex + mesh.vertices.size() - 2);
		}
		for (size_t i = 1; i + 1 < mesh.vertices.size(); ++i)
		{
			CHECK(std::isfinite(mesh.vertices[i].pos.x)); CHECK(std::isfinite(mesh.vertices[i].pos.y));
			CHECK(mesh.vertices[i].color == Float4{ 1, 0.5f, 0.25f, 0.5f });
		}
		for (size_t i = 1; i <= mesh.count; i += 3)
		{
			const Vec2 p = mesh.vertices[mesh.indices[i] - BaseIndex + 1].pos;
			const Vec2 q = mesh.vertices[mesh.indices[i + 1] - BaseIndex + 1].pos;
			const Vec2 r = mesh.vertices[mesh.indices[i + 2] - BaseIndex + 1].pos;
			CHECK((q - p).cross(r - p) >= -0.002);
		}
	}
}

TEST_CASE("StyledArc.circle_intervals_follow_shader_pattern")
{
	for (const auto preset : { LineStyle::DottedFlat, LineStyle::DashedFlat, LineStyle::LongDashFlat, LineStyle::DashDotFlat })
	for (const float direction : { -1.0f, 1.0f })
	for (const double phase : { -1.25, 0.0, 2.75 })
	{
		const auto mesh = Build(preset(phase), { 80, 80 }, 0, direction * 4.5f, 3, 5);
		const auto triangle = [](const double x) { return std::abs(2 * (x - std::floor(x)) - 1); };
		for (int32 i = 0; i < 300; ++i)
		{
			const double distance = (i + 0.371) / 300 * (81 * 4.5);
			const double x = distance / 8 - phase;
			double coverage = 0;
			switch (preset.type)
			{
			case LinePattern::Dotted: coverage = triangle((x - 0.5) / 2) - 0.5; break;
			case LinePattern::Dashed: coverage = triangle((x - 1) / 4) - 0.4; break;
			case LinePattern::LongDash: coverage = triangle((x - 1) / 10) - 0.3; break;
			case LinePattern::DashDot: coverage = std::max(triangle((x - 1) / 10) - 0.4, triangle((x - 1) / 10 + 0.5) - 0.9); break;
			default: break;
			}
			const double t = (direction * distance / 81);
			const Vec2 point{ 81 * std::sin(t), -81 * std::cos(t) };
			bool covered = false;
			for (size_t j = 1; j <= mesh.count; j += 3)
			{
				const Vec2 p = mesh.vertices[mesh.indices[j] - BaseIndex + 1].pos;
				const Vec2 q = mesh.vertices[mesh.indices[j + 1] - BaseIndex + 1].pos;
				const Vec2 r = mesh.vertices[mesh.indices[j + 2] - BaseIndex + 1].pos;
				covered |= ((q - p).cross(point - p) >= 0 && (r - q).cross(point - q) >= 0 && (p - r).cross(point - r) >= 0);
			}
			CHECK(covered == (coverage > 0));
		}
	}
}

TEST_CASE("StyledArc.ellipse_distance_oracle_and_offset_cache")
{
	// Independent dense double-precision integration of the offset centerline.
	for (const Float2 axes : { Float2{ 80, 40 }, Float2{ 80, 12 }, Float2{ 40, 80 } })
	for (const float offset : { 0.0f, 1.0f, -0.5f, 0.0f })
	for (const float direction : { -1.0f, 1.0f })
	{
		const float start = 0.3f, sweep = direction * 4.7f;
		const auto mesh = Build(LineStyle::RoundDotFlat(0.125), axes, start, sweep, (1 - offset), (1 + offset));
		REQUIRE(mesh.count > 0);
		constexpr size_t Steps = 100000;
		Array<Vec2> points(Steps + 1);
		Array<double> distances(Steps + 1, 0.0);
		for (size_t i = 0; i <= Steps; ++i)
		{
			points[i] = MiddleAt(axes, double(start) + double(sweep) * i / Steps, offset);
			if (i) distances[i] = distances[i - 1] + points[i].distanceFrom(points[i - 1]);
		}
		for (size_t i = 0; i < static_cast<size_t>(mesh.count / 6); ++i)
		{
			const auto* quad = mesh.vertices.data() + 1 + i * 4;
			if (quad[0].tex.x != 0 || quad[2].tex.x != 1) continue;
			const double target = (1.25 + i * 4);
			const auto it = std::lower_bound(distances.begin(), distances.end(), target);
			REQUIRE(it != distances.end());
			const size_t j = static_cast<size_t>(it - distances.begin());
			REQUIRE(j > 0);
			const Vec2 expected = points[j - 1].lerp(points[j], (target - distances[j - 1]) / (distances[j] - distances[j - 1]));
			const Vec2 actual = (Vec2{ quad[0].pos } + Vec2{ quad[1].pos } + Vec2{ quad[2].pos } + Vec2{ quad[3].pos }) * 0.25;
			CHECK(actual.distanceFrom(expected) < 0.025);
		}
	}
}

TEST_CASE("StyledArc.empty_budget_and_allocation_failure")
{
	for (const auto preset : { LineStyle::DottedFlat, LineStyle::RoundDotFlat })
	{
		CHECK(Build(preset, { 80, 80 }, 0, 0).allocations == 0);
		CHECK(Build(preset, { 0, 80 }).allocations == 0);
		CHECK(Build(preset, { 80, 80 }, 0, 1, 0, 0).allocations == 0);
		CHECK(Build(preset, { 80, 80 }, 0, 1, 200, 0).allocations == 0);
		CHECK(Build(preset, { 80, 80 }, 0, 1, 3, 5, true).count == 0);
		CHECK(Build(preset, { 4096, 4096 }, 0, 2, 0.5f, 0.5f).count > 0);
		CHECK(Build(preset, { 4096, 4096 }, 0, 2.0001f, 0.5f, 0.5f).allocations == 0);
	}
}

TEST_CASE("StyledArc.public_apis_solid_circle_equivalence_and_alpha")
{
	const SceneScope scope;
	const Circle circle{ 180, 170, 90 };
	const Ellipse ellipse{ 180, 170, 90, 90 };
	const ColorF ink{ 1.0, 0.5 };
	for (const auto preset : { LineStyle::Square, LineStyle::Round, LineStyle::Flat, LineStyle::Dotted, LineStyle::Dashed, LineStyle::LongDash, LineStyle::DashDot, LineStyle::RoundDot, LineStyle::DottedFlat, LineStyle::DashedFlat, LineStyle::LongDashFlat, LineStyle::DashDotFlat, LineStyle::RoundDotFlat })
	for (const double sweep : { 90_deg, -270_deg, 360_deg })
	{
		const LineStyle style{ preset(0.25) };
		const auto actual = Capture([&] { CHECK(&circle.drawArc(style, 0, sweep, 3, 9, ink) == &circle); });
		if (style.type == LinePattern::Solid)
		{
			CHECK(actual == Capture([&] { circle.drawArc(style.cap, 0, sweep, 3, 9, ink); }));
			CHECK(Capture([&] { ellipse.drawArc(style, 0, sweep, 3, 9, ink); }) == Capture([&] { ellipse.drawArc(style.cap, 0, sweep, 3, 9, ink); }));
		}
		else
		{
			CHECK(actual == Capture([&] { CHECK(&ellipse.drawArc(style, 0, sweep, 3, 9, ink) == &ellipse); }));
			// Periodic round dots may overlap at the seam. Ordinary dashed bands do not.
			if (style.type != LinePattern::RoundDot)
			{
				uint8 maximum = 0;
				for (const auto pixel : actual) maximum = Max(maximum, pixel.r);
				CHECK(maximum <= 129);
			}
		}
	}
	const auto blank = Capture([] {});
	CHECK(blank == Capture([&]
	{
		circle.drawArc(LineStyle::Dashed, 0, 0, 3, 5);
		ellipse.drawArc(LineStyle::RoundDot, 0, 1, 0, 0);
		circle.drawArc(LineStyle::Dotted, 0, 1, 1e-12, 1e-12);
	}));
	const Ellipse flat{ 220, 170, 110, 25 };
	for (const auto matrix : { Mat3x2::Identity(), Mat3x2::Rotate(0.4, { 220, 170 }), Mat3x2::Scale(1.2, 0.75, { 220, 170 }) })
	{
		const auto actual = Capture([&] { const Transformer2D t{ matrix }; flat.drawArc(LineStyle::DashDotFlat, 20_deg, -250_deg, 2, 6, ink); });
		CHECK(actual != blank);
	}
}

TEST_CASE("StyledArc.closed_seams_phase_periods_and_radial_endpoints")
{
	const SceneScope scope;
	const Ellipse ellipse{ 240, 180, 115, 55 };
	const ColorF ink{ 1.0, 0.5 };
	for (const auto preset : { LineStyle::Dashed, LineStyle::DashDot, LineStyle::RoundDot })
	for (const double sweep : { 360_deg, -360_deg })
	{
		const LineStyle style{ preset(-0.5) };
		LineStyle flat = style; flat.cap = LineCap::Flat;
		LineStyle periodic = style; periodic.dotOffset += (preset.type == LinePattern::Dashed ? 4 : preset.type == LinePattern::RoundDot ? 2 : 10);
		const auto actual = Capture([&] { ellipse.drawArc(style, 35_deg, sweep, 2, 6, ink); });
		CHECK(actual == Capture([&] { ellipse.drawArc(flat, 35_deg, sweep, 2, 6, ink); }));
		CHECK(actual == Capture([&] { ellipse.drawArc(periodic, 35_deg, sweep * 2, 2, 6, ink); }));
		if (preset.type != LinePattern::RoundDot)
		{
			uint8 maximum = 0;
			for (const auto pixel : actual) maximum = Max(maximum, pixel.r);
			CHECK(maximum <= 129);
		}
	}
	// A short visible interval starts on the radial direction, not the ellipse's
	// parameter angle. The ordinary Flat arc is an independent public oracle.
	for (const double start : { 35_deg, 215_deg, -145_deg })
	for (const double sweep : { 0.5_deg, -0.5_deg })
	{
		const auto actual = Capture([&] { ellipse.drawArc(LineStyle::LongDashFlat, start, sweep, 2, 6); });
		const auto expected = Capture([&] { ellipse.drawArc(start, sweep, 2, 6); });
		CHECK(actual == expected);
	}
}

TEST_CASE("StyledArc.submission_benchmark", "[.benchmark]")
{
	const SceneScope scope;
	Graphics::SetVSyncEnabled(false);
# if SIV3D_PLATFORM(MACOS)
	struct QoS
	{
		pthread_override_t value = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
		~QoS() { if (value) pthread_override_qos_class_end_np(value); }
	} qos;
# endif
	struct Case { const char* name; Float2 axes; float width; };
	constexpr Case cases[]{ { "circle-small", { 20, 20 }, 2 }, { "circle", { 120, 120 }, 8 },
		{ "ellipse", { 120, 60 }, 8 }, { "ellipse-flat", { 120, 12 }, 8 }, { "ellipse-large-thin", { 400, 200 }, 2 } };
	constexpr size_t Batch = 64;
	Array<Vertex2D> vertices(20000);
	Array<uint16> indices(30000);
	const auto allocate = [&](const uint16 v, const uint32 i)
	{
		if ((v > vertices.size()) || (i > indices.size())) return Vertex2DBufferPointer{};
		return Vertex2DBufferPointer{ vertices.data(), indices.data(), 0 };
	};
	for (const auto& c : cases)
	for (const auto preset : { LineStyle::DashedFlat, LineStyle::DashDot, LineStyle::RoundDot })
	for (const int32 mode : { 0, 1, 2 })
	{
		Array<double> times[3];
		uint32 counts[3]{}, draws[3]{};
		REQUIRE(System::Update());
		for (int32 round = -4; round < 21; ++round)
		for (int32 slot = 0; slot < 3; ++slot)
		{
			const int32 method = ((round + 6 + slot) % 3);
			const auto warm = std::chrono::steady_clock::now();
			while (std::chrono::steady_clock::now() - warm < std::chrono::milliseconds(5))
				std::atomic_signal_fence(std::memory_order_seq_cst);
			uint32 indexCount = 0;
			const float scale = SIV3D_ENGINE(Renderer2D)->getRasterScaling();
			const auto begin = std::chrono::steady_clock::now();
			for (size_t i = 0; i < Batch; ++i)
			{
				const float factor = mode == 2 ? (1 + float(i % 8) * 0.01f) : 1.0f;
				const Float2 axes = c.axes * factor;
				const LineStyle style{ preset(mode ? double(i % 13) * 0.125 : 0.25) };
				if (method == 0)
				{
					if (axes.x == axes.y) Circle{ -1000, -1000, axes.x }.drawArc(style, 0, 270_deg, c.width * 0.5, c.width * 0.5);
					else Ellipse{ -1000, -1000, axes.x, axes.y }.drawArc(style, 0, 270_deg, c.width * 0.5, c.width * 0.5);
				}
				else if (method == 1)
				{
					// Match the public radial-angle conversion before comparing CPU
					// storage with actual upload storage, including its float rounding.
					float span = static_cast<float>(270_deg);
					if (axes.x != axes.y)
					{
						const auto [s, c0] = FastMath::SinCos(span);
						span = (std::atan2(axes.y * s, axes.x * c0) + Math::TwoPiF);
					}
					indexCount += Vertex2DBuilder::BuildArcStyled(allocate, style, { -1000, -1000 }, axes.x, axes.y,
						0, span, c.width * 0.5f, c.width * 0.5f, Float4{ 1, 1, 1, 1 }, scale);
					std::atomic_signal_fence(std::memory_order_seq_cst);
				}
				else Ellipse{ -1000, -1000, axes.x, axes.y }.drawArc(0, 270_deg, c.width * 0.5, c.width * 0.5);
			}
			const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / Batch;
			if (round >= 0) times[method].push_back(ns);
			REQUIRE(System::Update());
			const auto metrics = Profiler::GetFrameMetrics();
			const auto triangles = static_cast<uint32>(method == 1 ? indexCount / 3 : metrics.triangleCount);
			if (round == 0) { counts[method] = triangles; draws[method] = static_cast<uint32>(metrics.drawCalls); }
			if (round > 0) CHECK(triangles == counts[method]);
		}
		CHECK(counts[0] == counts[1]);
		for (size_t method = 0; method < 3; ++method)
		{
			times[method].sort();
			constexpr const char* names[]{ "public", "heap-builder", "solid-context" };
			std::cout << "StyledArc " << c.name << " pattern=" << int(preset.type) << " mode=" << mode << ' ' << names[method]
				<< " ns/call=" << times[method][10] << " p10=" << times[method][2] << " p90=" << times[method][18]
				<< " triangles/call=" << double(counts[method]) / Batch << " draws/batch=" << draws[method] << '\n';
		}
	}
}
