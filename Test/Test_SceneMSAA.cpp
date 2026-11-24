//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# if SIV3D_PLATFORM(WINDOWS)
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# elif SIV3D_PLATFORM(MACOS)
// Xcode compiles this file as Objective-C++ with NO_S3D_USING.
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# endif

namespace s3d
{
	namespace
	{
		constexpr std::array SampleCounts{ MSAASampleCount::NoMSAA, MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA };

		struct SceneTest
		{
			const SceneConfig config = Scene::GetConfig();
			const MSAASampleCount samples = Scene::GetMSAASampleCount();
			const ColorF background = Scene::GetBackground();
			const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
			const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
			const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
			const ScopedViewport2D viewport{ none };
			const ScopedScissorRect2D scissor{ none };

			SceneTest()
			{
				Scene::SetBackground(Palette::Black);
				REQUIRE(System::Update());
				Scene::Configure(SceneConfig::Fixed({ 80, 60 }));
			}

			~SceneTest()
			{
				Scene::SetBackground(background);
				Scene::SetMSAASampleCount(samples);
				Scene::Configure(config);
			}
		};

		bool Supports(const MSAASampleCount samples)
		{
		# if SIV3D_PLATFORM(WINDOWS)
			auto* renderer = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer));
			UINT qualityLevels = 0;
			return (SUCCEEDED(renderer->getDevice().getDevice()->CheckMultisampleQualityLevels(
				DXGI_FORMAT_R8G8B8A8_UNORM, FromEnum(samples), &qualityLevels)) && (qualityLevels > 0));
		# else
			auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
			return renderer->getDevice()->supportsTextureSampleCount(FromEnum(samples));
		# endif
		}

		struct SceneResources
		{
		# if SIV3D_PLATFORM(WINDOWS)
			ComPtr<ID3D11Texture2D> scene;
			ComPtr<ID3D11Texture2D> resolved;

			const void* sceneAddress() const { return scene.Get(); }
			const void* resolvedAddress() const { return resolved.Get(); }
			uint32 sampleCount() const
			{
				D3D11_TEXTURE2D_DESC desc{};
				scene->GetDesc(&desc);
				return desc.SampleDesc.Count;
			}
		# else
			MetalInternalTexture2D scene;
			MetalInternalTexture2D resolved;

			const void* sceneAddress() const { return scene.getTexture(); }
			const void* resolvedAddress() const { return resolved.getTexture(); }
			uint32 sampleCount() const { return static_cast<uint32>(scene.getTexture()->sampleCount()); }
		# endif
		};

		SceneResources GetResources()
		{
		# if SIV3D_PLATFORM(WINDOWS)
			auto& buffer = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer))->getBackBuffer();
			return { ComPtr<ID3D11Texture2D>{ buffer.getSceneTexture() },
				ComPtr<ID3D11Texture2D>{ buffer.getCaptureTexture().first } };
		# else
			auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
			return { ((Scene::GetMSAASampleCount() == MSAASampleCount::NoMSAA)
				? renderer->getSceneTextureNonMSAA() : renderer->getSceneTextureMSAA()),
				renderer->getSceneTextureNonMSAA() };
		# endif
		}

		Image CaptureScene()
		{
			ScreenCapture::RequestCurrentFrame();
			REQUIRE(System::Update());
			REQUIRE(ScreenCapture::HasNewFrame());
			return ScreenCapture::GetFrame();
		}
	}

	TEST_CASE("SceneMSAA.transitions_preserve_pending_draws")
	{
		const SceneTest restore;
		for (const auto from : SampleCounts)
		{
			if (not Supports(from)) { continue; }
			for (const auto to : SampleCounts)
			{
				if (not Supports(to)) { continue; }
				CAPTURE(from, to);
				Scene::SetMSAASampleCount(from);
				// Retain both resources to detect replacement even if an allocator reuses addresses.
				const auto before = GetResources();
				Scene::Rect().draw(Palette::Red);
				Graphics2D::Flush();
				{
					const Transformer2D move{ Mat3x2::Translate(10, 10) };
					const Texture texture{ Image{ 10, 10, Palette::Green } };
					texture.draw();
				}
				const auto config = Scene::GetConfig();
				const auto cursorTransform = Cursor::GetBaseWindowTransform();
				Scene::SetMSAASampleCount(to);
				CHECK(Scene::GetMSAASampleCount() == to);
				CHECK(Scene::GetConfig() == config);
				CHECK(Scene::Size() == Size{ 80, 60 });
				CHECK(Scene::RenderResolution() == Size{ 80, 60 });
				CHECK(Cursor::GetBaseWindowTransform() == cursorTransform);
				const auto after = GetResources();
				CHECK(after.sampleCount() == FromEnum(to));
				CHECK(after.resolvedAddress() == before.resolvedAddress());
				CHECK((after.sceneAddress() == before.sceneAddress()) == (from == to));
				Rect{ 40, 30, 10, 10 }.draw(Palette::Blue);
				const Image image = CaptureScene();
				CHECK(image.size() == Size{ 80, 60 });
				CHECK(image[5][5] == ((from == to) ? Palette::Red : Palette::Black));
				CHECK(image[15][15] == Palette::Green);
				CHECK(image[35][45] == Palette::Blue);
			}
		}
	}

	TEST_CASE("SceneMSAA.capture_size_and_resolved_edges")
	{
		const SceneTest restore;
		// Include a one-pixel boundary, an odd size, and display-resolution rendering.
		for (const auto config : { SceneConfig::Fixed({ 1, 1 }), SceneConfig::Fixed({ 81, 63 }),
			SceneConfig::Fixed({ 80, 60 }, SceneResolution::Display) })
		{
			Scene::Configure(config);
			const Size expectedSize = Scene::RenderResolution();
			for (const auto samples : SampleCounts)
			{
				if (not Supports(samples)) { continue; }
				CAPTURE(samples, expectedSize);
				Scene::SetMSAASampleCount(samples);
				CHECK(Scene::RenderResolution() == expectedSize);
				Scene::Rect().draw(Palette::Red);
				const Image image = CaptureScene();
				CHECK(image.size() == expectedSize);
				CHECK((image == Image{ expectedSize, Palette::Red }));
			}
		}
		// Check partial coverage without depending on backend-specific sample positions.
		Scene::Configure(SceneConfig::Fixed({ 80, 60 }));
		for (const auto samples : SampleCounts)
		{
			if (not Supports(samples)) { continue; }
			CAPTURE(samples);
			Scene::SetMSAASampleCount(samples);
			Triangle{ Vec2{ 5.25, 5.25 }, Vec2{ 73.25, 17.25 }, Vec2{ 19.25, 53.25 } }.draw(Palette::White);
			const Image image = CaptureScene();
			bool hasPartialCoverage = false;
			for (const auto pixel : image)
			{
				hasPartialCoverage |= ((0 < pixel.r) && (pixel.r < 255));
			}
			CHECK(hasPartialCoverage == (samples != MSAASampleCount::NoMSAA));
		}
	}

	TEST_CASE("SceneMSAA.empty_drawing_and_repeated_changes")
	{
		const SceneTest restore;
		for (const auto from : SampleCounts)
		{
			if (not Supports(from)) { continue; }
			for (const auto to : SampleCounts)
			{
				if ((from == to) || not Supports(to)) { continue; }
				CAPTURE(from, to);
				Scene::SetMSAASampleCount(from);
				Scene::Rect().draw(Palette::Red);
				Graphics2D::Flush();
				Scene::SetMSAASampleCount(to);
				// No draws follow the change, but presentation and capture must see a cleared scene.
				CHECK((CaptureScene() == Image{ Size{ 80, 60 }, Palette::Black }));
			}
		}
		Scene::Rect().draw(Palette::Green);
		for (int32 i = 0; i < 3; ++i)
		{
			for (const auto samples : SampleCounts)
			{
				if (Supports(samples)) { Scene::SetMSAASampleCount(samples); }
			}
		}
		Graphics2D::Flush();
		Rect{ 10, 10, 10, 10 }.draw(Palette::Blue);
		const Image image = CaptureScene();
		CHECK(image[5][5] == Palette::Green);
		CHECK(image[15][15] == Palette::Blue);
	}

	TEST_CASE("SceneMSAA.invalid_or_unsupported_preserves_scene")
	{
		const SceneTest restore;
		for (const auto requested : { uint8{ 0 }, uint8{ 3 }, uint8{ 8 }, uint8{ 255 }, uint8{ 1 }, uint8{ 2 }, uint8{ 4 } })
		{
			const auto samples = static_cast<MSAASampleCount>(requested);
			const bool valid = ((requested == 1) || (requested == 2) || (requested == 4));
			if (valid && Supports(samples)) { continue; }
			CAPTURE(requested);
			const auto previous = Scene::GetMSAASampleCount();
			const auto before = GetResources();
			Scene::Rect().draw(Palette::Red);
			Graphics2D::Flush();
			Rect{ 10, 10, 10, 10 }.draw(Palette::Green);
			CHECK_THROWS_AS(Scene::SetMSAASampleCount(samples), Error);
			CHECK(Scene::GetMSAASampleCount() == previous);
			const auto after = GetResources();
			CHECK(after.sceneAddress() == before.sceneAddress());
			CHECK(after.resolvedAddress() == before.resolvedAddress());
			const Image image = CaptureScene();
			CHECK(image[5][5] == Palette::Red);
			CHECK(image[15][15] == Palette::Green);
		}
	}

	TEST_CASE("SceneMSAA.configuration_and_resize_preserve_samples")
	{
		const SceneTest restore;
		for (const auto samples : SampleCounts)
		{
			if (not Supports(samples)) { continue; }
			CAPTURE(samples);
			Scene::SetMSAASampleCount(samples);
			for (const auto config : { SceneConfig::Fixed({ 73, 51 }), SceneConfig::Fixed({ 123, 87 }),
				SceneConfig::Fixed({ 80, 60 }, SceneResolution::Display), SceneConfig::Window(), SceneConfig::PixelArt({ 40, 30 }) })
			{
				Scene::Configure(config);
				CHECK(Scene::GetMSAASampleCount() == samples);
				CHECK(GetResources().sampleCount() == FromEnum(samples));
				const Size expectedSize = Scene::RenderResolution();
				Scene::Rect().draw(Palette::Blue);
				const Image image = CaptureScene();
				CHECK(image.size() == expectedSize);
				CHECK((image == Image{ expectedSize, Palette::Blue }));
				CHECK(Scene::GetMSAASampleCount() == samples);
			}
		}
	}

	TEST_CASE("SceneMSAA.backend_rejects_unsupported_count")
	{
		const SceneTest restore;
		// Exercise the backend capability check separately from public enum validation.
		constexpr uint32 unsupported = 3;
		REQUIRE_FALSE(Supports(static_cast<MSAASampleCount>(unsupported)));
		const auto previous = Scene::GetMSAASampleCount();
		const auto before = GetResources();
		Scene::Rect().draw(Palette::Red);
		Graphics2D::Flush();
		Rect{ 10, 10, 10, 10 }.draw(Palette::Green);
		CHECK_THROWS_AS(SIV3D_ENGINE(Renderer)->setSceneSampleCount(unsupported), Error);
		CHECK(Scene::GetMSAASampleCount() == previous);
		const auto after = GetResources();
		CHECK(after.sceneAddress() == before.sceneAddress());
		CHECK(after.resolvedAddress() == before.resolvedAddress());
		const Image image = CaptureScene();
		CHECK(image[5][5] == Palette::Red);
		CHECK(image[15][15] == Palette::Green);
	}

	TEST_CASE("SceneMSAA.presentation_at_native_and_scaled_sizes")
	{
		const SceneTest restore;
		const Size targetSize = Window::GetState().frameBufferSize;
	# if SIV3D_PLATFORM(WINDOWS)
		auto* renderer = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer));
		auto& buffer = renderer->getBackBuffer();
		D3D11ScreenCapture readback;
		readback.init(renderer->getDevice());
	# else
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		auto descriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		descriptor->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
		descriptor->setWidth(targetSize.x);
		descriptor->setHeight(targetSize.y);
		descriptor->setUsage(MTL::TextureUsageRenderTarget);
		descriptor->setStorageMode(MTL::StorageModeShared);
		auto target = NS::TransferPtr(renderer->getDevice()->newTexture(descriptor.get()));
		REQUIRE(target.get());
	# endif
		for (const Size size : { targetSize, Size{ 80, 60 } })
		{
			Scene::Configure(SceneConfig::Fixed(size));
			for (const auto samples : SampleCounts)
			{
				if (not Supports(samples)) { continue; }
				CAPTURE(size, samples);
				Scene::SetMSAASampleCount(samples);
				Scene::Rect().draw(Palette::Red);
				Graphics2D::Flush();
				const Point center = renderer->getLetterboxComposition().second.center().asPoint();
			# if SIV3D_PLATFORM(WINDOWS)
				// Equal dimensions use CopyResource / direct resolve; smaller scenes use composition.
				buffer.renderSceneToBackBuffer();
				REQUIRE(readback.capture(buffer.getBackBufferTexture(), targetSize));
				CHECK(readback.getImage()[center.y][center.x] == Palette::Red);
			# else
				auto command = NS::RetainPtr(renderer->getFrameContext().getCommandBuffer());
				renderer->renderSceneToBackBuffer(command.get(), target.get());
			# endif
				const Image image = CaptureScene();
				CHECK(image.size() == size);
				CHECK(image[size.y / 2][size.x / 2] == Palette::Red);
			# if SIV3D_PLATFORM(MACOS)
				command->waitUntilCompleted();
				REQUIRE(command->status() == MTL::CommandBufferStatusCompleted);
				uint8 pixel[4]{};
				target->getBytes(pixel, sizeof(pixel), MTL::Region::Make2D(center.x, center.y, 1, 1), 0);
				CHECK(pixel[0] == 0); // BGRA
				CHECK(pixel[1] == 0);
				CHECK(pixel[2] == 255);
				CHECK(pixel[3] == 255);
			# endif
			}
		}
	}
}
