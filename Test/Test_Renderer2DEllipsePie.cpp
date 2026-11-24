//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "ImageDrawingTestHelper.hpp"

namespace
{
	struct EllipsePieSceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedRenderStates2D states{ BlendState::Default2D };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };

		EllipsePieSceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 480, 280 }, SceneResolution::SceneSize));
		}

		~EllipsePieSceneScope()
		{
			Scene::Configure(config);
		}
	};

	template <class Draw>
	Image CaptureEllipsePie(Draw&& draw)
	{
		const ColorF background = Scene::GetBackground();
		const ScopeExit restore{ [&] { Scene::SetBackground(background); } };
		Scene::SetBackground(Palette::Black);
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const ScopedColorMul2D colorMul{ Palette::White, ScopedColorMul2D::Target::Set };
		const ScopedColorAdd2D colorAdd{ 0.0, ScopedColorAdd2D::Target::Set };
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		Image image = ScreenCapture::GetFrame();
		REQUIRE(image.width() >= 480);
		REQUIRE(image.height() >= 280);
		return image;
	}

	void DrawHalfEllipse(const double sweep, const int32 coloring)
	{
		const Ellipse ellipse{ 0, 0, 60, 30 };
		switch (coloring)
		{
		case 0: CHECK(&ellipse.drawPie(0, sweep, Palette::White) == &ellipse); break;
		case 1: CHECK(&ellipse.drawPie(0, sweep, ColorF{ 1, 0.25, 0.1 }, ColorF{ 0.1, 0.25, 1 }) == &ellipse); break;
		case 2:
			CHECK(&ellipse.drawPie(0, sweep, Pattern::PolkaDot{ .primary = Palette::White, .background = Palette::Green }) == &ellipse);
			break;
		}
	}

	void CheckHalfEllipseSamples(const Image& image, const Mat3x2& transform, const double sweep)
	{
		const Vec2 resolutionScale = (Vec2{ image.size() } / Vec2{ 480, 280 });
		const double sign = ((0 < sweep) ? 1.0 : -1.0);
		const auto check = [&](const Vec2 local, const bool filled)
		{
			const Vec2 pixel = (transform.transformPoint(local) * resolutionScale);
			const int32 x = static_cast<int32>(pixel.x), y = static_cast<int32>(pixel.y);
			CAPTURE(local, x, y, filled);
			REQUIRE(0 <= x);
			REQUIRE(x < image.width());
			REQUIRE(0 <= y);
			REQUIRE(y < image.height());
			CHECK((image[y][x] != Palette::Black) == filled);
		};
		// Interior, outside each semiaxis, and the excluded half; away from AA edges.
		check({ sign * 39, 0 }, true);
		check({ sign * 18, -15 }, true);
		check({ sign * 72, 0 }, false);
		check({ sign * 15, 36 }, false);
		check({ -sign * 39, 0 }, false);
	}
}

TEST_CASE("Renderer2D.ellipse_pie_full_sweeps")
{
	const ColorF inner{ 0.9, 0.2, 0.1, 0.5 }, outer{ 0.1, 0.3, 0.9, 0.5 };
	const Pattern::PolkaDot pattern{ .primary = inner, .background = outer };
	for (const int32 coloring : { 0, 1, 2 })
	{
		INFO(coloring);
		const auto draw = [&](const bool pie)
		{
			int32 i = 0;
			for (const double start : { 0.0, 0.31, -2.4 })
			{
				// Include sweeps just below one revolution that round to a full float sweep.
				for (const double sweep : { Math::TwoPi, -Math::TwoPi, 2 * Math::TwoPi, -2 * Math::TwoPi,
					(Math::TwoPi + 0.2), (-Math::TwoPi - 0.2),
					(Math::TwoPi - 1e-8), (-Math::TwoPi + 1e-8) })
				{
					const Ellipse ellipse{ 40 + (i % 6) * 80, 35 + (i / 6) * 64, (i % 2 ? -32 : 32), 24 };
					if (pie)
					{
						switch (coloring)
						{
						case 0: CHECK(&ellipse.drawPie(start, sweep, inner) == &ellipse); break;
						case 1: CHECK(&ellipse.drawPie(start, sweep, inner, outer) == &ellipse); break;
						case 2: CHECK(&ellipse.drawPie(start, sweep, pattern) == &ellipse); break;
						}
					}
					else
					{
						switch (coloring)
						{
						case 0: ellipse.draw(inner); break;
						case 1: ellipse.draw(inner, outer); break;
						case 2: ellipse.draw(pattern); break;
						}
					}
					++i;
				}
			}
		};
		const Image expected = CaptureEllipsePie([&] { draw(false); });
		CHECK(expected[35][40] != Palette::Black);
		const Image actual = CaptureEllipsePie([&] { draw(true); });
		CHECK((actual == expected));
	}
}

TEST_CASE("Renderer2D.ellipse_pie_partial_and_empty_sweeps")
{
	for (const double sweep : { 0.0, Math::HalfPi, -Math::HalfPi, (Math::TwoPi - 0.2) })
	{
		INFO(sweep);
		const Image actual = CaptureEllipsePie([&] { Ellipse{ 140, 120, 80, 40 }.drawPie(0, sweep); });
		CHECK((actual[110][160] == Palette::White) == (0.0 < sweep));
		CHECK((actual[110][120] == Palette::White) == ((sweep < 0.0) || (Math::Pi < sweep)));
		CHECK((actual[130][160] == Palette::White) == (Math::Pi < sweep));
		CHECK(actual[170][140] == Palette::Black);
	}
}

TEST_CASE("Renderer2D.ellipse_pie_transform_scaling")
{
	const EllipsePieSceneScope scene;
	struct Transforms { Mat3x2 local, camera; };
	const std::array transforms{
		Transforms{ Mat3x2::Identity(), Mat3x2::Identity() },
		Transforms{ Mat3x2::Scale(0.5f), Mat3x2::Identity() },
		Transforms{ Mat3x2::Scale(2.0f), Mat3x2::Identity() },
		Transforms{ Mat3x2::Identity(), Mat3x2::Scale(2.0f) },
		Transforms{ Mat3x2::Scale(2.0f, 0.75f), Mat3x2::Rotate(30_deg) },
		Transforms{ Mat3x2::ShearX(0.5f), Mat3x2::Scale(1.0f, 1.5f) },
		Transforms{ Mat3x2::Scale(2.0f), Mat3x2::Scale(0.5f) } };
	for (const int32 coloring : { 0, 1, 2 })
	for (const double sweep : { Math::Pi, -Math::Pi })
	{
		std::array<int64, transforms.size()> triangles{};
		for (size_t i = 0; i < transforms.size(); ++i)
		{
			CAPTURE(coloring, sweep, i);
			const auto& t = transforms[i];
			const Mat3x2 camera = t.camera.translated(240, 140);
			const Image image = CaptureEllipsePie([&]
			{
				const Transformer2D localScope{ t.local, Transformer2D::Target::SetLocal };
				const Transformer2D cameraScope{ camera, Transformer2D::Target::SetCamera };
				DrawHalfEllipse(sweep, coloring);
			});
			triangles[i] = Profiler::GetFrameMetrics().triangleCount;
			CheckHalfEllipseSamples(image, (t.local * camera), sweep);
		}
		// Only tessellation density, not the source radii, follows the drawing scale.
		CHECK(triangles[1] < triangles[0]);
		CHECK(triangles[2] > triangles[0]);
		CHECK(triangles[3] == triangles[2]);
		CHECK(triangles[6] == triangles[0]);
	}
}

TEST_CASE("Renderer2D.ellipse_pie_scene_resolution")
{
	const EllipsePieSceneScope scene;
	const Mat3x2 placement = Mat3x2::Translate(240, 140);
	for (const int32 coloring : { 0, 1, 2 })
	for (const double sweep : { Math::Pi, -Math::Pi })
	{
		CAPTURE(coloring, sweep);
		const auto draw = [&]
		{
			const Transformer2D transform{ placement };
			DrawHalfEllipse(sweep, coloring);
		};
		Scene::Configure(SceneConfig::Fixed({ 480, 280 }, SceneResolution::SceneSize));
		CheckHalfEllipseSamples(CaptureEllipsePie(draw), placement, sweep);
		const auto baseTriangles = Profiler::GetFrameMetrics().triangleCount;
		Scene::Configure(SceneConfig::Fixed({ 480, 280 }, SceneResolution::Display));
		const Size pixels = Scene::RenderResolution();
		REQUIRE(pixels.x > 480);
		const Image highResolution = CaptureEllipsePie(draw);
		const auto highTriangles = Profiler::GetFrameMetrics().triangleCount;
		CheckHalfEllipseSamples(highResolution, placement, sweep);
		CHECK(highTriangles > baseTriangles);
		Scene::Configure(SceneConfig::Fixed(pixels, SceneResolution::SceneSize));
		const Image reference = CaptureEllipsePie([&]
		{
			const Transformer2D transform{ placement * Mat3x2::Scale(Float2{ float(pixels.x) / 480, float(pixels.y) / 280 }) };
			DrawHalfEllipse(sweep, coloring);
		});
		CHECK(highTriangles == Profiler::GetFrameMetrics().triangleCount);
		// The scene-scale and explicit-transform paths can round gradient RGB by
		// one UNORM8 step; retain exact alpha and reject larger per-channel errors.
		CHECK_THAT(highResolution, ImageDrawingTest::ImageEquals(reference, 1));
	}
}
