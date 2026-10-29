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
	struct RestoreScene
	{
		const SceneConfig config = Scene::GetConfig();
		const MSAASampleCount samples = Scene::GetMSAASampleCount();
		const ColorF background = Scene::GetBackground();
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		const ScopedViewport2D viewport{ none };
		const ScopedScissorRect2D scissor{ none };

		~RestoreScene()
		{
			Scene::SetBackground(background);
			Scene::SetMSAASampleCount(samples);
			Scene::Configure(config);
		}
	};

	Image CaptureScene()
	{
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}

	void BeginBlueFrame(const MSAASampleCount samples, const SceneResolution resolution = SceneResolution::SceneSize)
	{
		Scene::Configure(SceneConfig::Fixed({ 80, 60 }, resolution));
		Scene::SetMSAASampleCount(samples);
		Scene::SetBackground(Palette::Blue);
		REQUIRE(System::Update());
	}

	Color AtLogical(const Image& image, const Point point)
	{
		return image[point.y * image.height() / Scene::Height()][point.x * image.width() / Scene::Width()];
	}
}

// Run alone in a fresh process: other rendering tests advance the frame count.
TEST_CASE("SceneBackground.before_first_update", "[.startup]")
{
	REQUIRE(Scene::FrameCount() == 0);
	const RestoreScene restore;
	Scene::Configure(SceneConfig::Fixed({ 80, 60 }));
	Scene::SetBackground(Palette::Red);
	Scene::SetBackground(Palette::Blue);
	CHECK(Scene::GetBackground() == ColorF{ Palette::Blue });
	REQUIRE(System::Update());
	CHECK(Scene::FrameCount() == 1);
	Scene::SetBackground(Palette::Yellow);
	CHECK((CaptureScene() == Image{ Size{ 80, 60 }, Palette::Blue }));
	CHECK((CaptureScene() == Image{ Size{ 80, 60 }, Palette::Yellow }));
}

TEST_CASE("SceneBackground.flush_does_not_change_application_frame")
{
	const RestoreScene restore;
	for (const auto samples : { MSAASampleCount::NoMSAA, Scene::DefaultMSAASampleCount })
	{
		for (const bool flushBefore : { false, true })
		{
			for (const bool flushAfter : { false, true })
			{
				for (const bool draw : { false, true })
				{
					CAPTURE(samples, flushBefore, flushAfter, draw);
					BeginBlueFrame(samples);
					if (draw) { Rect{ 0, 0, 20, 60 }.draw(Palette::Magenta); }
					if (flushBefore) { Graphics2D::Flush(); }
					Scene::SetBackground(Palette::Red);
					// Alpha is retained by the getter but ignored when clearing.
					const ColorF next{ 1.0, 1.0, 0.0, 0.25 };
					Scene::SetBackground(next);
					CHECK(Scene::GetBackground() == next);
					if (flushAfter) { Graphics2D::Flush(); }
					if (draw) { Rect{ 60, 0, 20, 60 }.draw(Palette::Green); }
					Image expected{ Size{ 80, 60 }, Palette::Blue };
					if (draw)
					{
						Rect{ 0, 0, 20, 60 }.overwrite(expected, Palette::Magenta);
						Rect{ 60, 0, 20, 60 }.overwrite(expected, Palette::Green);
					}
					CHECK(CaptureScene() == expected);
					CHECK(Scene::GetBackground() == next);
					CHECK((CaptureScene() == Image{ Size{ 80, 60 }, Palette::Yellow }));
				}
			}
		}
	}
}

TEST_CASE("SceneBackground.animated_background_and_shapes")
{
	const RestoreScene restore;
	BeginBlueFrame(MSAASampleCount::NoMSAA);
	Color previous = Palette::Blue;
	for (const Color color : { Palette::Red, Palette::Green, Palette::Blue, Palette::Yellow })
	{
		Scene::SetBackground(color);
		Rect{ 40, 0, 40, 60 }.draw(color);
		Image expected{ Size{ 80, 60 }, previous };
		Rect{ 40, 0, 40, 60 }.overwrite(expected, color);
		CHECK(CaptureScene() == expected);
		previous = color;
	}
	// The selected color persists when no further changes are made.
	for (int32 i = 0; i < 2; ++i)
	{
		CHECK((CaptureScene() == Image{ Size{ 80, 60 }, previous }));
	}
}

TEST_CASE("SceneBackground.scene_changes_keep_frame_color")
{
	const RestoreScene restore;
	for (const auto samples : { MSAASampleCount::NoMSAA, Scene::DefaultMSAASampleCount })
	{
		for (const bool flush : { false, true })
		{
			for (int32 change = 0; change < 5; ++change)
			{
				CAPTURE(samples, flush, change);
				BeginBlueFrame(samples, ((change == 3) ? SceneResolution::Display : SceneResolution::SceneSize));
				Rect{ 0, 0, 20, 20 }.draw(Palette::Red);
				if (flush) { Graphics2D::Flush(); }
				Scene::SetBackground(Palette::Yellow);
				// This draw must survive even when the scene buffer is replaced.
				Rect{ 30, 30, 20, 20 }.draw(Palette::Green);
				const Size oldResolution = Scene::RenderResolution();
				auto config = Scene::GetConfig();
				switch (change)
				{
				case 0: // No size or sample change: executed pixels remain.
					Scene::Configure(config);
					Scene::SetMSAASampleCount(samples);
					break;
				case 1:
					config.fixedSceneSize = { 96, 72 };
					Scene::Configure(config);
					break;
				case 2:
					config.resolution = SceneResolution::Display;
					Scene::Configure(config);
					REQUIRE(Scene::RenderResolution() != oldResolution);
					break;
				case 3: // Logical size changes while the render resolution stays the same.
					config.fixedSceneSize = { 160, 120 };
					Scene::Configure(config);
					REQUIRE(Scene::RenderResolution() == oldResolution);
					break;
				case 4:
					Scene::SetMSAASampleCount((samples == MSAASampleCount::NoMSAA)
						? Scene::DefaultMSAASampleCount : MSAASampleCount::NoMSAA);
					break;
				}
				Graphics2D::Flush();
				Scene::SetBackground(Palette::Magenta);
				const Image image = CaptureScene();
				CHECK(image.size() == Scene::RenderResolution());
				CHECK(AtLogical(image, { 10, 10 }) == ((flush && (change != 0)) ? Palette::Blue : Palette::Red));
				CHECK(AtLogical(image, { 40, 40 }) == Palette::Green);
				CHECK(AtLogical(image, { 70, 50 }) == Palette::Blue);
				CHECK(Scene::GetBackground() == ColorF{ Palette::Magenta });
				CHECK((CaptureScene() == Image{ Scene::RenderResolution(), Palette::Magenta }));
			}
		}
	}
}

TEST_CASE("SceneBackground.failed_scene_changes_keep_frame_color")
{
	const RestoreScene restore;
	BeginBlueFrame(MSAASampleCount::NoMSAA);
	Rect{ 0, 0, 20, 20 }.draw(Palette::Red);
	Graphics2D::Flush();
	Scene::SetBackground(Palette::Yellow);
	CHECK_THROWS_AS(Scene::Configure(SceneConfig::Fixed({ 0, 60 })), Error);
	CHECK_THROWS_AS(Scene::SetMSAASampleCount(static_cast<MSAASampleCount>(255)), Error);
	Rect{ 30, 30, 20, 20 }.draw(Palette::Green);
	const Image image = CaptureScene();
	CHECK(image[10][10] == Palette::Red);
	CHECK(image[40][40] == Palette::Green);
	CHECK(image[50][70] == Palette::Blue);
	CHECK((CaptureScene() == Image{ Size{ 80, 60 }, Palette::Yellow }));
}
