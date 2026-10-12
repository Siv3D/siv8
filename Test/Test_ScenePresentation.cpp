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
