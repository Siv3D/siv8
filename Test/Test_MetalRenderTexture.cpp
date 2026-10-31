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

@interface Siv3DRenderTextureTestQueue : NSObject
@property(nonatomic, strong) id<MTLCommandQueue> queue;
@property(nonatomic) BOOL fail;
- (id<MTLCommandBuffer>)commandBuffer;
@end
@implementation Siv3DRenderTextureTestQueue
- (id<MTLCommandBuffer>)commandBuffer
{
	return self.fail ? nil : [self.queue commandBuffer];
}
@end

namespace s3d
{
	namespace
	{
		size_t AvailableSlots(dispatch_semaphore_t semaphore)
		{
			size_t slots = 0;
			while (dispatch_semaphore_wait(semaphore, DISPATCH_TIME_NOW) == 0) { ++slots; }
			for (size_t i = 0; i < slots; ++i) { dispatch_semaphore_signal(semaphore); }
			return slots;
		}
	}

	TEST_CASE("RenderTexture.Metal.allocation_failure_and_retry")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const RenderTexture existing{ Size{ 3, 2 }, Palette::Blue };
		{
			Test::TextureAllocationFailure failure{ renderer->getDevice(), 1 };
			const RenderTexture failed{ Size{ 7, 5 }, Palette::Red, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
			CHECK_FALSE(failed);
			CHECK(failure.attempts == 1);
		}
		Image image;
		REQUIRE(existing.readAsImage(image));
		CHECK(image == Image{ 3, 2, Palette::Blue });
		const RenderTexture retry{ Size{ 7, 5 }, Palette::Red, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
		REQUIRE(retry.readAsImage(image, 2));
		CHECK(image == Image{ 1, 1, Palette::Red });
	}

	TEST_CASE("RenderTexture.Metal.readback_keeps_frame_slot_and_handles_continuation_failure")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		Siv3DRenderTextureTestQueue* queue = [[Siv3DRenderTextureTestQueue alloc] init];
		queue.queue = (__bridge id<MTLCommandQueue>)renderer->getCommandQueue();
		MetalFrameContext context;
		const size_t frameIndex = context.waitForFrame();
		context.beginFrame((__bridge MTL::CommandQueue*)queue);
		const auto initial = NS::RetainPtr(context.getCommandBuffer());
		queue.fail = YES;
		CHECK_FALSE(context.flushAndWait((__bridge MTL::CommandQueue*)queue));
		CHECK(context.getCommandBuffer() == initial.get());
		CHECK(initial->status() == MTL::CommandBufferStatusNotEnqueued);
		CHECK(AvailableSlots(context.getSemaphore()) == (MetalFrameContext::MaxInflightFrames - 1));
		queue.fail = NO;
		REQUIRE(context.flushAndWait((__bridge MTL::CommandQueue*)queue));
		CHECK(initial->status() == MTL::CommandBufferStatusCompleted);
		CHECK(context.getCommandBuffer() != initial.get());
		CHECK(AvailableSlots(context.getSemaphore()) == (MetalFrameContext::MaxInflightFrames - 1));
		context.cancel();
		CHECK(context.waitForFrame() == frameIndex);
		context.beginFrame((__bridge MTL::CommandQueue*)queue);
		REQUIRE(context.flushAndWait((__bridge MTL::CommandQueue*)queue));
		context.submit();
		REQUIRE(context.waitForLastSubmittedFrame());
		context.drain();
		CHECK(AvailableSlots(context.getSemaphore()) == MetalFrameContext::MaxInflightFrames);
	}
}
# endif
