//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(WINDOWS)

# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>

namespace s3d
{
	namespace
	{
		struct RestorePresentation
		{
			const SceneConfig config = Scene::GetConfig();
			const ColorF background = Scene::GetBackground();
			const ColorF letterbox = Scene::GetLetterbox();

			~RestorePresentation()
			{
				Scene::Configure(config);
				Scene::SetBackground(background);
				Scene::SetLetterbox(letterbox);
			}
		};
	}

	TEST_CASE("ScenePresentation.D3D11.scaling_filter_and_buffer_reuse")
	{
		const RestorePresentation restore;
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		const ScopedViewport2D viewport{ none };
		const ScopedScissorRect2D scissor{ none };
		REQUIRE(System::Update());
		Scene::SetBackground(Palette::Black);
		Scene::SetLetterbox(Palette::Green);
		Scene::Configure(SceneConfig::PixelArt({ 2, 2 }));
		auto* renderer = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer));
		auto& buffer = renderer->getBackBuffer();
		D3D11ScreenCapture readback;
		readback.init(renderer->getDevice());
		// Hold COM references so a replacement cannot reuse a released address.
		const ComPtr<ID3D11Texture2D> scene{ buffer.getSceneTexture() };
		const ComPtr<ID3D11Texture2D> resolved{ buffer.getCaptureTexture().first };
		D3D11_TEXTURE2D_DESC desc{};
		scene->GetDesc(&desc);
		REQUIRE(desc.SampleDesc.Count == FromEnum(Scene::DefaultMSAASampleCount));
		REQUIRE(desc.SampleDesc.Count > 1);

		for (const auto filter : { TextureFilter::Nearest, TextureFilter::Linear, TextureFilter::Nearest })
		{
			CAPTURE(filter);
			auto config = Scene::GetConfig();
			config.scalingFilter = filter;
			Scene::Configure(config);
			CHECK(buffer.getSceneTexture() == scene.Get());
			CHECK(buffer.getCaptureTexture().first == resolved.Get());
			Rect{ 0, 0, 1, 2 }.draw(Palette::Red);
			Rect{ 1, 0, 1, 2 }.draw(Palette::Blue);
			Graphics2D::Flush();
			// Run the actual swap-chain composition, including MSAA resolve and sampler binding.
			buffer.renderSceneToBackBuffer();
			REQUIRE(readback.capture(buffer.getBackBufferTexture(), buffer.getBackBufferSize()));
			const auto [scale, area] = buffer.getLetterboxComposition();
			REQUIRE(scale > 8.0);
			const Point center = area.center().asPoint();
			const auto& image = readback.getImage();
			const Color left = image[center.y][center.x - 1];
			const Color right = image[center.y][center.x];
			if (filter == TextureFilter::Nearest)
			{
				CHECK(left == Palette::Red);
				CHECK(right == Palette::Blue);
			}
			else
			{
				CHECK(InRange(left.r, uint8{ 100 }, uint8{ 155 }));
				CHECK(InRange(left.b, uint8{ 100 }, uint8{ 155 }));
				CHECK(InRange(right.r, uint8{ 100 }, uint8{ 155 }));
				CHECK(InRange(right.b, uint8{ 100 }, uint8{ 155 }));
			}
			CHECK(left.g == 0);
			CHECK(right.g == 0);
			if (area.x >= 1) { CHECK(image[center.y][0] == Palette::Green); }
			if (area.y >= 1) { CHECK(image[0][center.x] == Palette::Green); }
			REQUIRE(System::Update());
		}

		Scene::Configure(SceneConfig::Fixed({ 4, 4 }, SceneResolution::Display));
		const Size resolution = Scene::RenderResolution();
		const ComPtr<ID3D11Texture2D> beforeScene{ buffer.getSceneTexture() };
		const ComPtr<ID3D11Texture2D> beforeResolved{ buffer.getCaptureTexture().first };
		Scene::Rect().draw(Palette::Red);
		Graphics2D::Flush();
		// Both fields change in one call; only the scene coordinates change in the final state.
		Scene::Configure(SceneConfig::Fixed(resolution));
		CHECK(Scene::Size() == resolution);
		CHECK(Scene::RenderResolution() == resolution);
		CHECK(buffer.getSceneTexture() == beforeScene.Get());
		CHECK(buffer.getCaptureTexture().first == beforeResolved.Get());
		buffer.renderSceneToBackBuffer();
		REQUIRE(readback.capture(buffer.getBackBufferTexture(), buffer.getBackBufferSize()));
		const Point center = buffer.getLetterboxComposition().second.center().asPoint();
		CHECK(readback.getImage()[center.y][center.x] == Palette::Black);
	}

	TEST_CASE("ScenePresentation.D3D11.msaa_direct_resolve")
	{
		const RestorePresentation restore;
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		const ScopedViewport2D viewport{ none };
		const ScopedScissorRect2D scissor{ none };
		REQUIRE(System::Update());
		auto* renderer = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer));
		auto& buffer = renderer->getBackBuffer();
		const Size size = buffer.getBackBufferSize();
		Scene::SetBackground(Palette::Black);
		Scene::Configure(SceneConfig::Fixed(size));
		CHECK(buffer.getLetterboxComposition().second == RectF{ size });
		D3D11_TEXTURE2D_DESC desc{};
		buffer.getSceneTexture()->GetDesc(&desc);
		REQUIRE(desc.SampleDesc.Count > 1);
		const int32 edge = (size.x / 2);
		RectF{ 0, 0, edge + 0.5, size.y }.draw(Palette::Red);
		Graphics2D::Flush();
		buffer.renderSceneToBackBuffer();
		D3D11ScreenCapture readback;
		readback.init(renderer->getDevice());
		REQUIRE(readback.capture(buffer.getBackBufferTexture(), size));
		const Image presented = readback.getImage();
		CHECK(presented[size.y / 2][edge - 1] == Palette::Red);
		CHECK(presented[size.y / 2][edge + 1] == Palette::Black);
		CHECK(InRange(presented[size.y / 2][edge].r, uint8{ 1 }, uint8{ 254 }));
		// The direct-to-swap-chain resolve must match the scene capture resolve, pixel for pixel.
		const auto [texture, capturedSize] = buffer.getCaptureTexture();
		CHECK(capturedSize == size);
		REQUIRE(readback.capture(texture, capturedSize));
		CHECK(presented == readback.getImage());
	}
}

# elif SIV3D_PLATFORM(MACOS)

// Xcode compiles this file as Objective-C++ with NO_S3D_USING.
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>

namespace s3d
{
	TEST_CASE("ScenePresentation.macOS.scaling_filter_and_buffer_reuse")
	{
		const SceneConfig previous = Scene::GetConfig();
		const ScopeExit restore{ [&] { Scene::Configure(previous); } };
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		REQUIRE(System::Update());
		Scene::Configure(SceneConfig::PixelArt({ 2, 2 }));
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const auto sceneTexture = renderer->getSceneTextureNonMSAA();
		const auto sceneMSAA = renderer->getSceneTextureMSAA();
		const Size targetSize = Window::GetState().frameBufferSize;
		auto descriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		descriptor->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
		descriptor->setWidth(targetSize.x);
		descriptor->setHeight(targetSize.y);
		descriptor->setUsage(MTL::TextureUsageRenderTarget);
		descriptor->setStorageMode(MTL::StorageModeShared);
		auto target = NS::TransferPtr(renderer->getDevice()->newTexture(descriptor.get()));
		REQUIRE(target.get());

		for (const auto filter : { TextureFilter::Nearest, TextureFilter::Linear, TextureFilter::Nearest })
		{
			CAPTURE(filter);
			auto config = Scene::GetConfig();
			config.scalingFilter = filter;
			Scene::Configure(config);
			CHECK(renderer->getSceneTextureNonMSAA().getTexture() == sceneTexture.getTexture());
			CHECK(renderer->getSceneTextureMSAA().getTexture() == sceneMSAA.getTexture());
			Rect{ 0, 0, 1, 2 }.draw(Palette::Red);
			Rect{ 1, 0, 1, 2 }.draw(Palette::Blue);
			Graphics2D::Flush();
			const auto [scale, area] = renderer->getLetterboxComposition();
			REQUIRE(scale > 8.0);
			auto command = NS::RetainPtr(renderer->getFrameContext().getCommandBuffer());
			// Exercise the same composition, shader, and sampler binding as present().
			renderer->renderSceneToBackBuffer(command.get(), target.get());
			REQUIRE(System::Update());
			command->waitUntilCompleted();
			REQUIRE(command->status() == MTL::CommandBufferStatusCompleted);
			uint8 pixels[8]{};
			const auto center = area.center().asPoint();
			target->getBytes(pixels, sizeof(pixels),
				MTL::Region::Make2D(center.x - 1, center.y, 2, 1), 0);
			if (filter == TextureFilter::Nearest)
			{
				CHECK(pixels[2] == 255); // BGRA: left pixel is red, right pixel is blue.
				CHECK(pixels[0] == 0);
				CHECK(pixels[4] == 255);
				CHECK(pixels[6] == 0);
			}
			else
			{
				CHECK(InRange(pixels[0], uint8{ 100 }, uint8{ 155 }));
				CHECK(InRange(pixels[2], uint8{ 100 }, uint8{ 155 }));
				CHECK(InRange(pixels[4], uint8{ 100 }, uint8{ 155 }));
				CHECK(InRange(pixels[6], uint8{ 100 }, uint8{ 155 }));
			}
			CHECK(pixels[1] == 0);
			CHECK(pixels[3] == 255);
			CHECK(pixels[5] == 0);
			CHECK(pixels[7] == 255);
		}

		// Both fields change, but the final render resolution stays the same.
		// Retain the resources so a replacement cannot reuse a released object's address.
		Scene::Configure(SceneConfig::Fixed({ 4, 4 }, SceneResolution::Display));
		const Size resolution = Scene::RenderResolution();
		const auto beforeTexture = renderer->getSceneTextureNonMSAA();
		const auto beforeMSAA = renderer->getSceneTextureMSAA();
		Scene::Configure(SceneConfig::Fixed(resolution));
		CHECK(Scene::Size() == resolution);
		CHECK(Scene::RenderResolution() == resolution);
		CHECK(renderer->getSceneTextureNonMSAA().getTexture() == beforeTexture.getTexture());
		CHECK(renderer->getSceneTextureMSAA().getTexture() == beforeMSAA.getTexture());
	}
}

# endif
