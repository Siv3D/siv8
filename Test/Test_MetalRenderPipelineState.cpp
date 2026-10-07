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

# if SIV3D_PLATFORM(MACOS)

// Xcode compiles this file as Objective-C++ with NO_S3D_USING.
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Shader/Metal/CShader_Metal.hpp>

// Intercept only pipeline creation; successful requests use the real device.
@interface Siv3DTestPipelineDevice : NSObject
@property(nonatomic, strong) id<MTLDevice> backingDevice;
@property(nonatomic) BOOL failCreation;
@property(nonatomic, strong) NSError* failure;
@property(nonatomic, readonly) NSUInteger creationCount;
- (id<MTLRenderPipelineState>)newRenderPipelineStateWithDescriptor:(MTLRenderPipelineDescriptor*)descriptor error:(NSError**)error;
@end

@implementation Siv3DTestPipelineDevice
- (id<MTLRenderPipelineState>)newRenderPipelineStateWithDescriptor:(MTLRenderPipelineDescriptor*)descriptor error:(NSError**)error
{
	++_creationCount;
	if (self.failCreation)
	{
		// Leave the out parameter untouched when no error is supplied, so the
		// caller must initialize it before trying to read diagnostic details.
		if (error && self.failure)
		{
			*error = self.failure;
		}
		return nil;
	}
	return [self.backingDevice newRenderPipelineStateWithDescriptor:descriptor error:error];
}
@end

namespace s3d
{
	namespace
	{
		PipelineStateDesc MakeDesc(const bool scene)
		{
			return
			{
				.vs = SIV3D_ENGINE(EngineShader)->getVS(scene ? EngineVS::Shape2D : EngineVS::FullScreenTriangle).id(),
				.ps = SIV3D_ENGINE(EngineShader)->getPS(scene ? EnginePS::Shape2D : EnginePS::FullScreenTriangle).id(),
				.pixelFormat = static_cast<uint16>(scene ? MTL::PixelFormatRGBA8Unorm : MTL::PixelFormatBGRA8Unorm),
				.sampleCount = static_cast<uint16>(scene ? 4 : 1),
				.blendState = BlendState::Opaque,
			};
		}

		Siv3DTestPipelineDevice* MakeDevice()
		{
			auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
			Siv3DTestPipelineDevice* device = [[Siv3DTestPipelineDevice alloc] init];
			device.backingDevice = (__bridge id<MTLDevice>)renderer->getDevice();
			return device;
		}
	}

	TEST_CASE("MetalRenderPipelineState.creation_and_cache")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		__attribute__((objc_precise_lifetime)) Siv3DTestPipelineDevice* device = MakeDevice();
		MetalRenderPipelineState cache;
		cache.init((__bridge MTL::Device*)device, static_cast<CShader_Metal*>(SIV3D_ENGINE(Shader)));
		NSUInteger expectedCreations = 0;
		for (const bool scene : { false, true })
		{
			const auto desc = MakeDesc(scene);
			const auto* pipeline = cache.get(desc);
			REQUIRE(pipeline);
			++expectedCreations;
			CHECK(device.creationCount == expectedCreations);
			CHECK(cache.get(desc) == pipeline);
			CHECK(device.creationCount == expectedCreations);
		}
	}

	TEST_CASE("MetalRenderPipelineState.failure_retry_and_cache")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		for (const bool withError : { true, false })
		{
			INFO("withError = " << withError);
			__attribute__((objc_precise_lifetime)) Siv3DTestPipelineDevice* device = MakeDevice();
			device.failCreation = YES;
			if (withError)
			{
				device.failure = [NSError errorWithDomain:@"Siv3DTestPipelineError" code:7
					userInfo:@{ NSLocalizedDescriptionKey: @"Injected pipeline creation failure" }];
			}
			MetalRenderPipelineState cache;
			cache.init((__bridge MTL::Device*)device, static_cast<CShader_Metal*>(SIV3D_ENGINE(Shader)));
			const auto desc = MakeDesc(false);
			for (NSUInteger attempt = 1; attempt <= 2; ++attempt)
			{
				try
				{
					(void)cache.get(desc);
					FAIL_CHECK("Pipeline creation failure must throw InternalEngineError");
				}
				catch (const InternalEngineError& error)
				{
					const std::string message = error.messageUTF8();
					CHECK(message.contains("newRenderPipelineState"));
					CHECK(message.contains(fmt::format("vs = {}", desc.vs.value())));
					CHECK(message.contains(fmt::format("ps = {}", desc.ps.value())));
					CHECK(message.contains(fmt::format("pixelFormat = {}", desc.pixelFormat)));
					CHECK(message.contains("sampleCount = 1"));
					CHECK(message.contains(withError ? "Injected pipeline creation failure" : "No Metal error details"));
				}
				CHECK(device.creationCount == attempt);
			}
			device.failCreation = NO;
			const auto* pipeline = cache.get(desc);
			CHECK(pipeline != nullptr);
			CHECK(device.creationCount == 3);
			CHECK(cache.get(desc) == pipeline);
			CHECK(device.creationCount == 3);
		}
	}

	TEST_CASE("MetalRenderPipelineState.draw_recovers_after_failure")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		auto* shader = static_cast<CShader_Metal*>(SIV3D_ENGINE(Shader));
		const PixelShader customPS = PixelShader::MSL(U"PS_Shape");
		REQUIRE(customPS);
		const ColorF background = Scene::GetBackground();
		const ScopeExit restoreBackground{ [&] { Scene::SetBackground(background); } };
		Scene::SetBackground(Palette::Black);
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const ScopedColorMul2D colorMul{ Palette::White };
		const ScopedColorAdd2D colorAdd{ 0.0 };
		const ScopedViewport2D viewport{ none };
		const ScopedScissorRect2D scissor{ none };
		const ScopedRenderStates2D states{ BlendState::Opaque, RasterizerState::Default2D };
		for (const auto [withPriorDraws, emptyFlush] : { std::pair{ false, false }, std::pair{ true, false }, std::pair{ true, true } })
		{
			INFO("withPriorDraws = " << withPriorDraws << ", emptyFlush = " << emptyFlush);
			REQUIRE(System::Update());
			__attribute__((objc_precise_lifetime)) Siv3DTestPipelineDevice* device = MakeDevice();

			// Keep the presentation pipeline alive and restore the cache on every exit.
			auto& cache = renderer->getRenderPipelineState();
			auto savedCache = std::move(cache);
			const ScopeExit restoreCache{ [&] { cache = std::move(savedCache); } };
			cache = {};
			cache.init((__bridge MTL::Device*)device, shader);
			if (withPriorDraws)
			{
				RectF{ 0, 0, 16, 16 }.draw(Palette::Lime);
				renderer->flush();
				// This draw uses the cached pipeline and succeeds in the failing flush.
				RectF{ 20, 0, 16, 16 }.draw(Palette::Yellow);
			}
			device.failCreation = YES;
			const NSUInteger creationsBeforeFailure = device.creationCount;
			{
				const ScopedCustomShader2D custom{ customPS };
				RectF{ 40, 0, 16, 16 }.draw(Palette::Blue);
			}
			// Discard the rest of this flush too, including a different index count.
			Triangle{ Vec2{ 60, 0 }, Vec2{ 76, 0 }, Vec2{ 60, 16 } }.draw(Palette::Blue);
			CHECK_THROWS_AS(renderer->flush(), InternalEngineError);
			CHECK(device.creationCount == (creationsBeforeFailure + 1));

			// Neither an empty flush nor a later draw may replay the discarded geometry.
			device.failCreation = NO;
			if (emptyFlush)
			{
				renderer->flush();
			}
			RectF{ 80, 0, 16, 16 }.draw(Palette::Red);
			renderer->flush();
			RectF{ 100, 0, 16, 16 }.draw(Palette::Cyan);
			ScreenCapture::RequestCurrentFrame();
			REQUIRE(System::Update());
			REQUIRE(ScreenCapture::HasNewFrame());
			const Image& frame = ScreenCapture::GetFrame();
			REQUIRE(frame.width() >= 116);
			REQUIRE(frame.height() >= 16);
			CHECK(frame[8][8] == (withPriorDraws ? Palette::Lime : Palette::Black));
			CHECK(frame[8][28] == (withPriorDraws ? Palette::Yellow : Palette::Black));
			CHECK(frame[8][48] == Palette::Black);
			CHECK(frame[4][64] == Palette::Black);
			CHECK(frame[8][88] == Palette::Red);
			CHECK(frame[8][108] == Palette::Cyan);
		}
	}
}

# endif
