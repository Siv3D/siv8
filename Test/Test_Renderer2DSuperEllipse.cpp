//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

namespace
{
	struct FrameSceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D colorMul{ Palette::White };
		ScopedColorAdd2D colorAdd{ 0.0 };
		ScopedRenderStates2D states{ BlendState::Default2D };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };

		FrameSceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 240, 200 }, SceneResolution::SceneSize));
			Scene::SetBackground(Palette::Black);
		}

		~FrameSceneScope()
		{
			Scene::Configure(config);
			Scene::SetBackground(background);
		}
	};

	struct FrameCapture
	{
		Image image;
		int64 triangles;
	};

	template <class Draw>
	FrameCapture Capture(Draw&& draw)
	{
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return{ ScreenCapture::GetFrame(), Profiler::GetFrameMetrics().triangleCount };
	}

	void DrawFrame(const SuperEllipse& shape, const double thickness, const bool patterned)
	{
		if (patterned)
		{
			const Pattern::PolkaDot white{ .primary = Palette::White, .background = Palette::White };
			CHECK(&shape.drawFrame(thickness, white) == &shape);
		}
		else
		{
			CHECK(&shape.drawFrame(thickness, Palette::White) == &shape);
		}
	}
}

TEST_CASE("Renderer2D.superellipse_frame_uniform_scaling")
{
	const FrameSceneScope scene;
	for (const bool patterned : { false, true })
	for (const double exponent : { 1.0, 4.0 })
	for (const double scale : { 0.5, 1.0, 4.0 })
	{
		CAPTURE(patterned, exponent, scale);
		const auto actual = Capture([&]
		{
			const Transformer2D transform{ Mat3x2::Scale(scale).translated(120, 100) };
			DrawFrame(SuperEllipse{ 0, 0, 24, 16, exponent }, 4, patterned);
		});
		// Scaling the geometry and stroke explicitly must select the same detail.
		const auto expected = Capture([&]
		{
			DrawFrame(SuperEllipse{ 120, 100, 24 * scale, 16 * scale, exponent }, 4 * scale, patterned);
		});
		CHECK(actual.triangles == expected.triangles);
		CHECK(actual.image == expected.image);
		CHECK(actual.triangles > 0);
	}
}

TEST_CASE("Renderer2D.superellipse_frame_affine_rms")
{
	const FrameSceneScope scene;
	for (const bool patterned : { false, true })
	{
		CAPTURE(patterned);
		const auto count = [&](const Mat3x2& local, const Mat3x2& camera)
		{
			return Capture([&]
			{
				const Transformer2D localScope{ local, Transformer2D::Target::SetLocal };
				const Transformer2D cameraScope{ camera.translated(120, 100), Transformer2D::Target::SetCamera };
				DrawFrame(SuperEllipse{ 0, 0, 24, 16, 4 }, 4, patterned);
			}).triangles;
		};
		const auto base = count(Mat3x2::Identity(), Mat3x2::Identity());
		const auto stretched = count(Mat3x2::Identity(), Mat3x2::Scale(4.0f, 1.0f));
		CHECK(stretched > base);
		CHECK(count(Mat3x2::Rotate(45_deg), Mat3x2::Scale(4.0f, 1.0f)) == stretched);
		CHECK(count(Mat3x2::Scale(4.0f, 1.0f), Mat3x2::Scale(0.25f, 1.0f)) == base);
	}
}

TEST_CASE("Renderer2D.superellipse_frame_scene_resolution")
{
	const FrameSceneScope scene;
	const Size logical{ 240, 200 };
	for (const bool patterned : { false, true })
	{
		CAPTURE(patterned);
		const auto draw = [&] { DrawFrame(SuperEllipse{ 120, 100, 24, 16, 4 }, 4, patterned); };
		Scene::Configure(SceneConfig::Fixed(logical, SceneResolution::SceneSize));
		const auto base = Capture(draw);
		Scene::Configure(SceneConfig::Fixed(logical, SceneResolution::Display));
		const Size pixels = Scene::RenderResolution();
		REQUIRE(pixels.x > logical.x);
		const auto highResolution = Capture(draw);
		CHECK(highResolution.triangles > base.triangles);
		CHECK(Graphics2D::GetRMSScaling() == 1.0f);
		Scene::Configure(SceneConfig::Fixed(pixels, SceneResolution::SceneSize));
		const auto expected = Capture([&]
		{
			const Transformer2D transform{ Mat3x2::Scale(Float2{ float(pixels.x) / logical.x, float(pixels.y) / logical.y }) };
			draw();
		});
		CHECK(highResolution.triangles == expected.triangles);
		CHECK(highResolution.image == expected.image);
	}
}
