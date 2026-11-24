//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
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

	uint16 Submit(const BufferCreatorFunc& allocate, const LineStyle& style, const RoundRect& rect, const double inner, const double outer, const float scale)
	{
		const RectF in = rect.rect.stretched(-inner), out = rect.rect.stretched(outer);
		const float ir = static_cast<float>(Clamp(Abs(rect.r) - inner, 0.0, Min(in.w, in.h) * 0.5));
		const float orad = static_cast<float>(Min(Abs(rect.r) + outer, Min(out.w, out.h) * 0.5));
		return Vertex2DBuilder::BuildRoundRectFrameStyled(allocate, style, FloatRect{ in }, ir, FloatRect{ out }, orad,
			static_cast<float>(inner + outer), Float4{ 1, 0.5f, 0.25f, 0.5f }, scale);
	}

	Mesh Build(const LineStyle& style, const RoundRect& rect = RoundRect{ 20, 20, 180, 100, 20 },
		const double inner = 3, const double outer = 5, const bool fail = false, const float scale = 1)
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
		result.count = Submit(allocate, style, rect, inner, outer, scale);
		return result;
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
		ScopedColorMul2D mul{ Palette::White, ScopedColorMul2D::Target::Set };
		ScopedColorAdd2D add{ 0.0, ScopedColorAdd2D::Target::Set };
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

TEST_CASE("StyledRoundRect.bounds_winding_and_piece_budget")
{
	for (const auto preset : { LineStyle::Dotted, LineStyle::Dashed, LineStyle::LongDash, LineStyle::DashDot, LineStyle::RoundDot })
	for (const RoundRect rect : { RoundRect{ 20, 20, 180, 100, 0 }, RoundRect{ 20, 20, 180, 100, 2 },
		RoundRect{ 20, 20, 180, 100, 20 }, RoundRect{ 20, 20, 180, 100, 500 }, RoundRect{ 20, 20, 100, 100, 50 } })
	for (const double phase : { -3.25, -0.5, 0.0, 1.375 })
	for (const float scale : { 1.0f, 64.0f })
	{
		const auto mesh = Build(preset(phase), rect, 3, 5, false, scale);
		REQUIRE(mesh.count > 0);
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
		if (preset.type == LinePattern::RoundDot)
		{
			for (size_t i = 1; i + 3 < mesh.vertices.size() - 1; i += 4)
			{
				CHECK(mesh.vertices[i].tex.x == 0); CHECK(mesh.vertices[i + 2].tex.x == 1);
				CHECK(mesh.vertices[i].pos.distanceFrom(mesh.vertices[i + 1].pos) == Catch::Approx(8).margin(0.0001));
				CHECK(mesh.vertices[i].pos.distanceFrom(mesh.vertices[i + 2].pos) == Catch::Approx(8).margin(0.0001));
			}
		}
	}
	CHECK(Build(LineStyle::Dashed, RoundRect{ 20, 20, 180, 100, 20 }, 3, 5, true).count == 0);
	CHECK(Build(LineStyle::Dashed, RoundRect{ 20, 20, 180, 100, 20 }, 0, 0).allocations == 0);
	CHECK(Build(LineStyle::RoundDot, RoundRect{ 0, 0, 10000, 10000, 0 }, 0.5, 0.5).allocations == 0);
	// With no outer radius, a square perimeter gives an exact count at the limit.
	CHECK(Build(LineStyle::RoundDot(-0.5), RoundRect{ 0, 0, 2049, 2049, 0 }, 1, 0).count == 4096 * 6);
	CHECK(Build(LineStyle::RoundDot(-0.5), RoundRect{ 0, 0, 2049.5, 2049.5, 0 }, 1, 0).allocations == 0);
}

TEST_CASE("StyledRoundRect.analytic_centerline_pattern_oracle")
{
	// Independent exact midpoint curve: straight lengths 140, 60, radius 21.
	const double quarter = (21 * Math::HalfPi), perimeter = (400 + 4 * quarter);
	const std::array<Vec2, 4> centers{{ { 180, 40 }, { 180, 100 }, { 40, 100 }, { 40, 40 } }};
	const std::array<Vec2, 4> normals{{ { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } }};
	for (const auto preset : { LineStyle::DottedFlat, LineStyle::DashedFlat, LineStyle::LongDashFlat, LineStyle::DashDotFlat })
	for (const double phase : { -0.5, 0.25, 1.5 })
	{
		const auto mesh = Build(preset(phase));
		const double period = (preset.type == LinePattern::Dotted ? 2 : preset.type == LinePattern::Dashed ? 4 : 10);
		const double unit = perimeter / (std::round(perimeter / (8 * period)) * period);
		const auto triangle = [](double x) { return std::abs(2 * (x - std::floor(x)) - 1); };
		for (size_t i = 0; i < 600; ++i)
		{
			const double distance = (i + 0.371) / 600 * perimeter, x = distance / unit - phase;
			double coverage = 0;
			switch (preset.type)
			{
			case LinePattern::Dotted: coverage = triangle((x - 0.5) / 2) - 0.5; break;
			case LinePattern::Dashed: coverage = triangle((x - 1) / 4) - 0.4; break;
			case LinePattern::LongDash: coverage = triangle((x - 1) / 10) - 0.3; break;
			case LinePattern::DashDot: coverage = Max(triangle((x - 1) / 10) - 0.4, triangle((x - 1) / 10 + 0.5) - 0.9); break;
			default: break;
			}
			double remaining = distance;
			Vec2 point;
			for (size_t corner = 0; corner < 4; ++corner)
			{
				const double straight = ((corner % 2) ? 60 : 140);
				if (remaining < straight)
				{
					point = centers[(corner + 3) % 4] + normals[corner] * 21 + Vec2{ -normals[corner].y, normals[corner].x } * remaining;
					break;
				}
				remaining -= straight;
				if (remaining < quarter)
				{
					const double angle = corner * Math::HalfPi + remaining / 21;
					point = centers[corner] + Vec2{ std::sin(angle), -std::cos(angle) } * 21;
					break;
				}
				remaining -= quarter;
			}
			size_t hits = 0;
			for (size_t j = 1; j <= mesh.count; j += 3)
			{
				const Vec2 p = mesh.vertices[mesh.indices[j] - BaseIndex + 1].pos;
				const Vec2 q = mesh.vertices[mesh.indices[j + 1] - BaseIndex + 1].pos;
				const Vec2 r = mesh.vertices[mesh.indices[j + 2] - BaseIndex + 1].pos;
				hits += ((q - p).cross(point - p) > 0 && (r - q).cross(point - q) > 0 && (p - r).cross(point - r) > 0);
			}
			CHECK(hits == (coverage > 0 ? 1 : 0));
		}
	}
}

TEST_CASE("StyledRoundRect.straight_cuts_remain_perpendicular")
{
	for (const double radius : { 0.0, 2.0, 12.0 })
	for (const auto preset : { LineStyle::Dotted, LineStyle::Dashed, LineStyle::DashDot })
	{
		const auto mesh = Build(preset(1.25), RoundRect{ 20, 20, 180, 100, radius }, 6, 6);
		size_t cuts = 0;
		for (size_t i = 1; i + 1 < mesh.vertices.size() - 1; i += 2)
		{
			const auto p = mesh.vertices[i].pos, q = mesh.vertices[i + 1].pos;
			if (p.y == 26 && q.y == 14 && 33 < p.x && p.x < 187 && 33 < q.x && q.x < 187)
			{
				++cuts;
				CHECK(p.x == Catch::Approx(q.x).margin(0.0001));
			}
		}
		CHECK(cuts >= 4);
	}
}

TEST_CASE("StyledRoundRect.circle_limit_round_dots")
{
	for (const double phase : { -0.5, 0.125, 1.25 })
	{
		const auto rounded = Build(LineStyle::RoundDot(phase), RoundRect{ -40, -40, 80, 80, 40 });
		Array<Vertex2D> vertices;
		Array<uint16> indices;
		const auto allocate = [&](uint16 v, uint32 i)
		{
			vertices.resize(v); indices.resize(i);
			return Vertex2DBufferPointer{ vertices.data(), indices.data(), 0 };
		};
		CHECK(Vertex2DBuilder::BuildArcStyled(allocate, LineStyle::RoundDot(phase), { 0, 0 }, 40, 40, 0, Math::TwoPiF, 3, 5, Float4{ 1, 0.5f, 0.25f, 0.5f }, 1) == rounded.count);
		REQUIRE(vertices.size() + 2 == rounded.vertices.size());
		for (size_t i = 0; i < vertices.size(); ++i)
			CHECK(vertices[i].pos.distanceFrom(rounded.vertices[i + 1].pos) < 0.0001f);
	}
}

TEST_CASE("StyledRoundRect.public_rendering_caps_phase_alpha_and_fallback")
{
	const SceneScope scope;
	const RoundRect rect{ 100, 100, 240, 120, 24 };
	for (const auto preset : { LineStyle::Square, LineStyle::Round, LineStyle::Flat, LineStyle::Dotted, LineStyle::Dashed,
		LineStyle::LongDash, LineStyle::DashDot, LineStyle::RoundDot })
	{
		const LineStyle style{ preset(-0.5) };
		const auto actual = Capture([&] { CHECK(&rect.drawFrame(style, 3, 5, ColorF{ 1.0, 0.5 }) == &rect); });
		LineStyle flat = style; flat.cap = LineCap::Flat;
		CHECK(actual == Capture([&] { rect.drawFrame(flat, 3, 5, ColorF{ 1.0, 0.5 }); }));
		LineStyle periodic = style;
		periodic.dotOffset += (style.type == LinePattern::Dashed ? 4 : style.type == LinePattern::LongDash || style.type == LinePattern::DashDot ? 10 : 2);
		CHECK(actual == Capture([&] { rect.drawFrame(periodic, 3, 5, ColorF{ 1.0, 0.5 }); }));
		if (style.type == LinePattern::Solid)
			CHECK(actual == Capture([&] { rect.drawFrame(3, 5, ColorF{ 1.0, 0.5 }); }));
		uint8 maximum = 0;
		for (const auto pixel : actual) maximum = Max(maximum, pixel.r);
		CHECK(maximum <= 129);
		CHECK(Capture([&] { rect.drawFrame(style, 8); }) == Capture([&] { rect.drawFrame(style, 4, 4); }));
		CHECK(Capture([&] { rect.drawFrame(style, 120, 4); }) == Capture([&] { rect.drawFrame(120, 4); }));
	}
	const auto blank = Capture([] {});
	CHECK(blank == Capture([&]
	{
		rect.drawFrame(LineStyle::Dashed, 0);
		rect.drawFrame(LineStyle::RoundDot, -1, 2);
		rect.drawFrame(LineStyle::RoundDot, 1e-12);
		RoundRect{ 0, 0, 0, 100, 10 }.drawFrame(LineStyle::Dashed, 4);
	}));
	for (const auto matrix : { Mat3x2::Identity(), Mat3x2::Rotate(0.4, { 220, 160 }), Mat3x2::Scale(1.2, 0.75, { 220, 160 }) })
		CHECK(blank != Capture([&] { const Transformer2D transform{ matrix }; rect.drawFrame(LineStyle::DashDot, 8); }));
}

TEST_CASE("StyledRoundRect.submission_benchmark", "[.benchmark]")
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
	const std::array<RoundRect, 3> cases{ RoundRect{ -1000, -1000, 200, 100, 20 },
		RoundRect{ -1000, -1000, 120, 40, 20 }, RoundRect{ -1000, -1000, 400, 240, 48 } };
	Array<Vertex2D> vertices(20000); Array<uint16> indices(30000);
	const auto allocate = [&](const uint16 v, const uint32 i)
	{
		if (v > vertices.size() || i > indices.size()) return Vertex2DBufferPointer{};
		return Vertex2DBufferPointer{ vertices.data(), indices.data(), 0 };
	};
	for (size_t shape = 0; shape < cases.size(); ++shape)
	for (const auto preset : { LineStyle::DashedFlat, LineStyle::DashDot, LineStyle::RoundDot })
	{
		const auto& rect = cases[shape];
		const double width = (shape == 2 ? 2 : 8);
		constexpr size_t Batch = 64;
		Array<double> times[3]; uint32 counts[3]{};
		REQUIRE(System::Update());
		for (int32 round = -4; round < 21; ++round)
		for (int32 slot = 0; slot < 3; ++slot)
		{
			const int32 method = ((round + 6 + slot) % 3);
			const auto warm = std::chrono::steady_clock::now();
			while (std::chrono::steady_clock::now() - warm < std::chrono::milliseconds(5)) std::atomic_signal_fence(std::memory_order_seq_cst);
			uint32 indexCount = 0;
			const float scale = SIV3D_ENGINE(Renderer2D)->getRasterScaling();
			const auto begin = std::chrono::steady_clock::now();
			for (size_t i = 0; i < Batch; ++i)
			{
				const LineStyle style{ preset(double(i % 13) * 0.125) };
				if (method == 0) rect.drawFrame(style, width);
				else if (method == 1) indexCount += Submit(allocate, style, rect, width * 0.5, width * 0.5, scale);
				else rect.drawFrame(width);
				std::atomic_signal_fence(std::memory_order_seq_cst);
			}
			const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / Batch;
			if (round >= 0) times[method].push_back(ns);
			REQUIRE(System::Update());
			const auto triangles = static_cast<uint32>(method == 1 ? indexCount / 3 : Profiler::GetFrameMetrics().triangleCount);
			if (round == 0) counts[method] = triangles;
			if (round > 0) CHECK(triangles == counts[method]);
		}
		CHECK(counts[0] == counts[1]);
		for (size_t method = 0; method < 3; ++method)
		{
			times[method].sort();
			constexpr const char* names[]{ "public", "heap-builder", "solid-context" };
			std::cout << "StyledRoundRect " << shape << " pattern=" << int(preset.type) << ' ' << names[method]
				<< " ns/call=" << times[method][10] << " p10=" << times[method][2] << " p90=" << times[method][18]
				<< " triangles/call=" << double(counts[method]) / Batch << '\n';
		}
	}
}
