//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(MACOS)
# include "MetalTextureAllocationFailure.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
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

		void DrawExecutedAndPending()
		{
			Scene::Rect().draw(Palette::Red);
			Graphics2D::Flush();
			Rect{ 0, 0, 16, 16 }.draw(Palette::Blue);
		}

		void CheckCapturedPixels(const Size size, const Color executed)
		{
			ScreenCapture::RequestCurrentFrame();
			REQUIRE(System::Update());
			REQUIRE(ScreenCapture::HasNewFrame());
			const auto& image = ScreenCapture::GetFrame();
			REQUIRE(image.size() == size);
			CHECK(image[30][40] == executed);
			CHECK(image[8][8] == Palette::Blue);
		}
	}

	TEST_CASE("SceneAllocationFailure.macOS.configure_preserves_state_and_recovers")
	{
		REQUIRE(System::Update());
		const RestoreScene restore;
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		Scene::SetBackground(Palette::Black);
		const auto original = SceneConfig::Fixed({ 80, 60 });
		auto requested = SceneConfig::PixelArt({ 96, 72 });
		requested.fullscreen = FullscreenSceneMode::FollowWindow;
		struct Case { MSAASampleCount samples; int32 failOnAttempt; };
		for (const auto [samples, failOnAttempt] : {
			Case{ MSAASampleCount::NoMSAA, 1 },
			Case{ Scene::DefaultMSAASampleCount, 1 },
			Case{ Scene::DefaultMSAASampleCount, 2 } })
		{
			CAPTURE(samples, failOnAttempt);
			Scene::Configure(original);
			Scene::SetMSAASampleCount(samples);
			const auto scene = renderer->getSceneTextureMSAA();
			const auto resolved = renderer->getSceneTextureNonMSAA();
			const auto cursor = Cursor::GetBaseWindowTransform();
			DrawExecutedAndPending();
			{
				Test::TextureAllocationFailure failure{ renderer->getDevice(), failOnAttempt };
				CHECK_THROWS_AS(Scene::Configure(requested), Error);
				CHECK(failure.attempts.load() == failOnAttempt);
				CHECK(failure.successes.load() == (failOnAttempt - 1));
			}
			CHECK(Scene::GetConfig() == original);
			CHECK(Scene::GetMSAASampleCount() == samples);
			CHECK(Scene::Size() == Size{ 80, 60 });
			CHECK(Scene::RenderResolution() == Size{ 80, 60 });
			CHECK(Cursor::GetBaseWindowTransform() == cursor);
			CHECK(renderer->getSceneTextureMSAA().getTexture() == scene.getTexture());
			CHECK(renderer->getSceneTextureNonMSAA().getTexture() == resolved.getTexture());
			CheckCapturedPixels({ 80, 60 }, Palette::Red);
			CHECK(Scene::GetConfig() == original);

			DrawExecutedAndPending();
			Scene::Configure(requested);
			CHECK(Scene::GetConfig() == requested);
			CHECK(Scene::GetMSAASampleCount() == samples);
			CHECK(Scene::Size() == Size{ 96, 72 });
			CHECK(Scene::RenderResolution() == Size{ 96, 72 });
			CHECK(renderer->getSceneTextureNonMSAA().getTexture() != resolved.getTexture());
			CHECK(Scene::ClientToScene(Window::GetState().frameBufferSize * 0.5).epsilonEquals(Vec2{ 48, 36 }, 1e-6));
			CheckCapturedPixels({ 96, 72 }, Palette::Black);
		}
	}

	TEST_CASE("SceneAllocationFailure.macOS.msaa_preserves_state_and_recovers")
	{
		REQUIRE(System::Update());
		const RestoreScene restore;
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const auto config = SceneConfig::Fixed({ 80, 60 });
		Scene::Configure(config);
		Scene::SetMSAASampleCount(MSAASampleCount::NoMSAA);
		Scene::SetBackground(Palette::Black);
		const auto scene = renderer->getSceneTextureMSAA();
		const auto resolved = renderer->getSceneTextureNonMSAA();
		const auto cursor = Cursor::GetBaseWindowTransform();
		DrawExecutedAndPending();
		{
			Test::TextureAllocationFailure failure{ renderer->getDevice(), 1 };
			CHECK_THROWS_AS(Scene::SetMSAASampleCount(Scene::DefaultMSAASampleCount), Error);
			CHECK(failure.attempts.load() == 1);
			CHECK(failure.successes.load() == 0);
		}
		CHECK(Scene::GetConfig() == config);
		CHECK(Scene::GetMSAASampleCount() == MSAASampleCount::NoMSAA);
		CHECK(Scene::Size() == Size{ 80, 60 });
		CHECK(Scene::RenderResolution() == Size{ 80, 60 });
		CHECK(Cursor::GetBaseWindowTransform() == cursor);
		CHECK(renderer->getSceneTextureMSAA().getTexture() == scene.getTexture());
		CHECK(renderer->getSceneTextureNonMSAA().getTexture() == resolved.getTexture());
		CheckCapturedPixels({ 80, 60 }, Palette::Red);
		CHECK(Scene::GetMSAASampleCount() == MSAASampleCount::NoMSAA);

		DrawExecutedAndPending();
		Scene::SetMSAASampleCount(Scene::DefaultMSAASampleCount);
		CHECK(Scene::GetMSAASampleCount() == Scene::DefaultMSAASampleCount);
		CHECK(Scene::GetConfig() == config);
		CHECK(Cursor::GetBaseWindowTransform() == cursor);
		CHECK(renderer->getSceneTextureMSAA().getTexture()->sampleCount() == FromEnum(Scene::DefaultMSAASampleCount));
		CHECK(renderer->getSceneTextureNonMSAA().getTexture() == resolved.getTexture());
		CheckCapturedPixels({ 80, 60 }, Palette::Black);

		DrawExecutedAndPending();
		{
			// Disabling MSAA reuses the single-sample target and needs no allocation.
			Test::TextureAllocationFailure failure{ renderer->getDevice(), 1 };
			CHECK_NOTHROW(Scene::SetMSAASampleCount(MSAASampleCount::NoMSAA));
			CHECK(failure.attempts.load() == 0);
		}
		CHECK(Scene::GetMSAASampleCount() == MSAASampleCount::NoMSAA);
		CHECK(Scene::GetConfig() == config);
		CHECK(renderer->getSceneTextureNonMSAA().getTexture() == resolved.getTexture());
		CheckCapturedPixels({ 80, 60 }, Palette::Black);
	}
}

# endif
