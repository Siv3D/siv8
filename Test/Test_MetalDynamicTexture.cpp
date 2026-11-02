//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(MACOS)
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Texture/Metal/MetalTexture.hpp>

@interface Siv3DDynamicTextureTestQueue : NSObject
@property(nonatomic, strong) id<MTLCommandQueue> queue;
@property(nonatomic) BOOL fail;
@property(nonatomic) NSUInteger requests;
- (id<MTLCommandBuffer>)commandBuffer;
@end
@implementation Siv3DDynamicTextureTestQueue
- (id<MTLCommandBuffer>)commandBuffer
{
	++self.requests;
	return self.fail ? nil : [self.queue commandBuffer];
}
@end

namespace s3d
{
	namespace
	{
		void CheckPixels(MetalTexture& texture, const Color expected)
		{
			std::array<Color, 16> pixels;
			texture.getTexture()->getBytes(pixels.data(), 4 * sizeof(Color), MTL::Region::Make2D(0, 0, 4, 4), 0);
			for (const auto pixel : pixels) { CHECK(pixel == expected); }
		}
	}

	TEST_CASE("DynamicTexture.Metal.continuation_allocation_failure_and_standalone_updates")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		auto* queue = renderer->getCommandQueue();
		const Image blue{ 4, 4, Palette::Blue };
		MetalTexture texture{ MetalTexture::Dynamic{}, MetalTexture::NoMipmap{}, renderer->getDevice(), queue,
			blue.size(), std::as_bytes(std::span{ blue }), TextureFormat::R8G8B8A8_Unorm, TextureDesc::NoMipmap };
		REQUIRE(texture.isInitialized());
		Siv3DDynamicTextureTestQueue* proxy = [[Siv3DDynamicTextureTestQueue alloc] init];
		proxy.queue = (__bridge id<MTLCommandQueue>)queue;
		MetalFrameContext frame;
		(void)frame.waitForFrame();
		frame.beginFrame(queue);
		const auto initial = NS::RetainPtr(frame.getCommandBuffer());
		proxy.fail = YES;
		CHECK_FALSE(texture.fill((__bridge MTL::CommandQueue*)proxy, frame, Palette::Red, true));
		CHECK(frame.getCommandBuffer() == initial.get());
		CHECK(initial->status() == MTL::CommandBufferStatusNotEnqueued);
		CheckPixels(texture, Palette::Blue);
		proxy.fail = NO;
		REQUIRE(texture.fill((__bridge MTL::CommandQueue*)proxy, frame, Palette::Red, true));
		CHECK(frame.getCommandBuffer() != initial.get());
		REQUIRE(frame.flushAndWait(queue));
		CheckPixels(texture, Palette::Red);
		frame.cancel();
		// Operations before/after an acquired frame still submit and track uploads.
		REQUIRE(texture.fill(queue, frame, Palette::Green, true));
		REQUIRE(texture.fillRegion(queue, frame, Palette::Blue, Rect{ 4, 4 }));
		const auto barrier = NS::RetainPtr(queue->commandBuffer());
		barrier->commit();
		barrier->waitUntilCompleted();
		CheckPixels(texture, Palette::Blue);
	}

	TEST_CASE("DynamicTexture.Metal.busy_probe_does_not_copy_split_or_submit")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		auto* queue = renderer->getCommandQueue();
		const Image blue{ 4, 4, Palette::Blue };
		MetalTexture texture{ MetalTexture::Dynamic{}, MetalTexture::NoMipmap{}, renderer->getDevice(), queue,
			blue.size(), std::as_bytes(std::span{ blue }), TextureFormat::R8G8B8A8_Unorm, TextureDesc::NoMipmap };
		REQUIRE(texture.isInitialized());
		const auto event = NS::TransferPtr(renderer->getDevice()->newSharedEvent());
		REQUIRE(event);
		MetalFrameContext frame;
		const ScopeExit unblock{ [&] { event->setSignaledValue(1); } };
		(void)frame.waitForFrame();
		frame.beginFrame(queue);
		frame.getCommandBuffer()->encodeWait(event.get(), 1);
		REQUIRE(texture.fill(queue, frame, Palette::Red, true));
		const auto continuation = NS::RetainPtr(frame.getCommandBuffer());
		Siv3DDynamicTextureTestQueue* proxy = [[Siv3DDynamicTextureTestQueue alloc] init];
		proxy.queue = (__bridge id<MTLCommandQueue>)queue;
		CHECK_FALSE(texture.fill((__bridge MTL::CommandQueue*)proxy, frame, Palette::Green, false));
		CHECK(proxy.requests == 0);
		CHECK(frame.getCommandBuffer() == continuation.get());
		CHECK(continuation->status() == MTL::CommandBufferStatusNotEnqueued);
		event->setSignaledValue(1);
		REQUIRE(frame.flushAndWait(queue));
		CheckPixels(texture, Palette::Red);
		REQUIRE(texture.fill(queue, frame, Palette::Green, true));
		REQUIRE(frame.flushAndWait(queue));
		CheckPixels(texture, Palette::Green);
	}
}
# endif
