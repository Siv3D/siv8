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

// Xcode compiles this file as Objective-C++ with NO_S3D_USING so the native
// Metal headers can coexist with the shared test PCH's Siv3D types.
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <condition_variable>
# include <future>
# include <mutex>

// Inject allocation failure without adding a test hook to the frame context.
@interface Siv3DTestCommandQueue : NSObject
@property(nonatomic, strong) id<MTLCommandQueue> backingQueue;
@property(nonatomic) BOOL failCreation;
- (id<MTLCommandBuffer>)commandBuffer;
@end

@implementation Siv3DTestCommandQueue
- (id<MTLCommandBuffer>)commandBuffer
{
	return self.failCreation ? nil : [self.backingQueue commandBuffer];
}
@end

namespace s3d
{
	namespace
	{
		constexpr size_t FrameSlots = MetalFrameContext::MaxInflightFrames;

		void CompleteSubmittedFrames()
		{
			// Screenshot readback waits for the submitted frame and its completion
			// handlers. Update then acquires the slot for the next, unsubmitted frame.
			ScreenCapture::RequestCurrentFrame();
			REQUIRE(System::Update());
			REQUIRE(ScreenCapture::HasNewFrame());
		}

		size_t AvailableFrameSlots(dispatch_semaphore_t semaphore)
		{
			size_t available = 0;
			while (dispatch_semaphore_wait(semaphore, DISPATCH_TIME_NOW) == 0)
			{
				++available;
			}
			// Restore every permit; probing must not change the renderer's state.
			for (size_t i = 0; i < available; ++i)
			{
				dispatch_semaphore_signal(semaphore);
			}
			return available;
		}
	}

	TEST_CASE("MetalFrameSynchronization.current_frame_reserves_slot")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		CompleteSubmittedFrames();
		// This invariant also detects an uncounted initial frame after other
		// tests have already submitted frames: the extra permit persists.
		CHECK(AvailableFrameSlots(renderer->getFrameContext().getSemaphore()) == (FrameSlots - 1));
	}

	TEST_CASE("MetalFrameSynchronization.delayed_gpu_completion")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		CompleteSubmittedFrames();

		const auto event = NS::TransferPtr(renderer->getDevice()->newSharedEvent());
		const auto gate = NS::RetainPtr(renderer->getCommandQueue()->commandBuffer());
		REQUIRE(event);
		REQUIRE(gate);
		gate->encodeWait(event.get(), 1);

		// A watchdog releases the GPU if a regression blocks System::Update().
		// On success it is notified immediately, without a fixed sleep.
		std::mutex mutex;
		std::condition_variable condition;
		bool releaseRequested = false;
		auto watchdog = std::async(std::launch::async, [&]
		{
			std::unique_lock lock{ mutex };
			if (condition.wait_for(lock, std::chrono::seconds{ 5 }, [&] { return releaseRequested; }))
			{
				return false;
			}
			event->setSignaledValue(1);
			return true;
		});
		const auto releaseGPU = [&]
		{
			event->setSignaledValue(1);
			{
				const std::lock_guard lock{ mutex };
				releaseRequested = true;
			}
			condition.notify_one();
		};
		const ScopeExit unblockOnExit{ releaseGPU };
		gate->commit();

		// The current frame already owns one slot. Two submissions fill the
		// remaining slots while the event prevents GPU completion signals.
		for (size_t i = 0; i < (FrameSlots - 1); ++i)
		{
			REQUIRE(System::Update());
		}
		CHECK(event->signaledValue() == 0);
		CHECK(AvailableFrameSlots(renderer->getFrameContext().getSemaphore()) == 0);

		releaseGPU();
		CHECK_FALSE(watchdog.get());
		gate->waitUntilCompleted();
		CHECK(gate->status() == MTL::CommandBufferStatusCompleted);
		CompleteSubmittedFrames();
		CHECK(AvailableFrameSlots(renderer->getFrameContext().getSemaphore()) == (FrameSlots - 1));
	}

	TEST_CASE("MetalFrameSynchronization.context_reuses_completed_frames")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		MetalFrameContext context;
		CHECK(AvailableFrameSlots(context.getSemaphore()) == FrameSlots);

		// Exercise two complete rotations without involving Renderer2D.
		for (size_t i = 0; i < (FrameSlots * 2); ++i)
		{
			CHECK(context.waitForFrame() == ((i + 1) % FrameSlots));
			CHECK(AvailableFrameSlots(context.getSemaphore()) == (FrameSlots - 1));
			context.beginFrame(renderer->getCommandQueue());
			const auto commandBuffer = NS::RetainPtr(context.getCommandBuffer());
			REQUIRE(commandBuffer);
			context.submit();
			CHECK(context.getCommandBuffer() == nullptr);
			context.waitForLastSubmittedFrame();
			CHECK(commandBuffer->status() == MTL::CommandBufferStatusCompleted);
			CHECK(AvailableFrameSlots(context.getSemaphore()) == FrameSlots);
		}
	}

	TEST_CASE("MetalFrameSynchronization.context_returns_unsubmitted_slot_on_destruction")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		auto context = std::make_unique<MetalFrameContext>();
		// This test retains the semaphore so its count can be checked after teardown.
		dispatch_semaphore_t semaphore = context->getSemaphore();
		CHECK(context->waitForFrame() == 1);
		CHECK(AvailableFrameSlots(semaphore) == (FrameSlots - 1));
		SECTION("Before command buffer creation") {}
		SECTION("After command buffer creation")
		{
			context->beginFrame(renderer->getCommandQueue());
			REQUIRE(context->getCommandBuffer());
		}
		context.reset();
		CHECK(AvailableFrameSlots(semaphore) == FrameSlots);
	}

	TEST_CASE("MetalFrameSynchronization.completion_outlives_context")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const auto event = NS::TransferPtr(renderer->getDevice()->newSharedEvent());
		REQUIRE(event);
		auto context = std::make_unique<MetalFrameContext>();
		CHECK(context->waitForFrame() == 1);
		context->beginFrame(renderer->getCommandQueue());
		const auto commandBuffer = NS::RetainPtr(context->getCommandBuffer());
		REQUIRE(commandBuffer);
		commandBuffer->encodeWait(event.get(), 1);
		const ScopeExit unblockOnExit{ [&] { event->setSignaledValue(1); } };

		// A weak reference ensures the test itself does not keep the semaphore alive.
		__weak dispatch_semaphore_t semaphore = context->getSemaphore();
		context->submit();
		context.reset();
		REQUIRE(semaphore != nil);
		// After verifying callback ownership, retain it to inspect the returned slot.
		dispatch_semaphore_t survivingSemaphore = semaphore;
		CHECK(AvailableFrameSlots(survivingSemaphore) == (FrameSlots - 1));

		event->setSignaledValue(1);
		commandBuffer->waitUntilCompleted();
		CHECK(commandBuffer->status() == MTL::CommandBufferStatusCompleted);
		CHECK(AvailableFrameSlots(survivingSemaphore) == FrameSlots);
	}

	TEST_CASE("MetalFrameSynchronization.cancel_discards_commands")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const auto buffer = NS::TransferPtr(renderer->getDevice()->newBuffer(sizeof(uint32), MTL::ResourceStorageModeShared));
		REQUIRE(buffer);
		auto* value = static_cast<uint32*>(buffer->contents());
		*value = 0;

		MetalFrameContext context;
		// Cancellation is also safe before a slot or command buffer exists.
		context.cancel();
		context.waitForLastSubmittedFrame();
		CHECK(context.waitForFrame() == 1);
		context.cancel();
		CHECK(context.waitForFrame() == 1);
		context.beginFrame(renderer->getCommandQueue());
		const auto discarded = NS::RetainPtr(context.getCommandBuffer());
		REQUIRE(discarded);
		auto* encoder = discarded->blitCommandEncoder();
		REQUIRE(encoder);
		encoder->fillBuffer(buffer.get(), NS::Range{ 0, sizeof(uint32) }, 0xFF);
		encoder->endEncoding();
		context.cancel();
		context.cancel();
		CHECK(context.getCommandBuffer() == nullptr);
		REQUIRE(AvailableFrameSlots(context.getSemaphore()) == FrameSlots);
		CHECK(discarded->status() == MTL::CommandBufferStatusNotEnqueued);

		// A later submission completes without executing the discarded write.
		CHECK(context.waitForFrame() == 1);
		context.beginFrame(renderer->getCommandQueue());
		context.submit();
		context.waitForLastSubmittedFrame();
		CHECK(*value == 0);
		CHECK(AvailableFrameSlots(context.getSemaphore()) == FrameSlots);
	}

	TEST_CASE("MetalFrameSynchronization.cancel_reuses_slot_while_gpu_is_busy")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const auto event = NS::TransferPtr(renderer->getDevice()->newSharedEvent());
		REQUIRE(event);
		MetalFrameContext context;
		const ScopeExit unblockOnExit{ [&] { event->setSignaledValue(1); } };

		// Fill two slots. Both submissions remain in flight behind the GPU event.
		for (size_t i = 0; i < (FrameSlots - 1); ++i)
		{
			CHECK(context.waitForFrame() == (i + 1));
			context.beginFrame(renderer->getCommandQueue());
			if (i == 0)
			{
				context.getCommandBuffer()->encodeWait(event.get(), 1);
			}
			context.submit();
		}
		context.cancel(); // Submitted slots belong to their completion callbacks.
		REQUIRE(AvailableFrameSlots(context.getSemaphore()) == 1);

		for (size_t i = 0; i < (FrameSlots * 2); ++i)
		{
			// Every retry must use slot 0; slots 1 and 2 are still owned by the GPU.
			CHECK(context.waitForFrame() == 0);
			context.beginFrame(renderer->getCommandQueue());
			CHECK(AvailableFrameSlots(context.getSemaphore()) == 0);
			context.cancel();
			context.cancel();
			REQUIRE(AvailableFrameSlots(context.getSemaphore()) == 1);
		}
		CHECK(event->signaledValue() == 0);
		event->setSignaledValue(1);
		// Cancelling newer frames must preserve the previous submission for waiting.
		context.waitForLastSubmittedFrame();
		CHECK(AvailableFrameSlots(context.getSemaphore()) == FrameSlots);
	}

	TEST_CASE("MetalFrameSynchronization.uninitialized_renderer_destruction")
	{
		auto renderer = std::make_unique<CRenderer_Metal>();
		dispatch_semaphore_t semaphore = renderer->getFrameContext().getSemaphore();
		CHECK_NOTHROW(renderer.reset());
		CHECK(AvailableFrameSlots(semaphore) == FrameSlots);
	}

	TEST_CASE("MetalFrameSynchronization.drain_cancels_unsubmitted_frame")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		MetalFrameContext context;
		NS::SharedPtr<MTL::CommandBuffer> discarded;
		SECTION("No frame acquired") {}
		SECTION("Slot acquired before command buffer creation")
		{
			CHECK(context.waitForFrame() == 1);
		}
		SECTION("Command buffer created but not submitted")
		{
			CHECK(context.waitForFrame() == 1);
			context.beginFrame(renderer->getCommandQueue());
			discarded = NS::RetainPtr(context.getCommandBuffer());
			REQUIRE(discarded);
		}
		context.drain();
		CHECK(context.getCommandBuffer() == nullptr);
		CHECK(AvailableFrameSlots(context.getSemaphore()) == FrameSlots);
		if (discarded)
		{
			CHECK(discarded->status() == MTL::CommandBufferStatusNotEnqueued);
		}
		context.drain();
		CHECK(AvailableFrameSlots(context.getSemaphore()) == FrameSlots);
	}

	TEST_CASE("MetalFrameSynchronization.drain_waits_for_all_inflight_frames")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const auto event = NS::TransferPtr(renderer->getDevice()->newSharedEvent());
		REQUIRE(event);
		MetalFrameContext context;
		std::promise<void> started;
		auto start = started.get_future();
		std::future<void> draining;
		// Release the GPU before the future is destroyed, even if an assertion fails.
		const ScopeExit unblockOnExit{ [&] { event->setSignaledValue(1); } };
		for (size_t i = 0; i < (FrameSlots - 1); ++i)
		{
			(void)context.waitForFrame();
			context.beginFrame(renderer->getCommandQueue());
			context.getCommandBuffer()->encodeWait(event.get(), 1);
			context.submit();
		}
		(void)context.waitForFrame();
		context.beginFrame(renderer->getCommandQueue());
		const auto discarded = NS::RetainPtr(context.getCommandBuffer());
		CHECK(AvailableFrameSlots(context.getSemaphore()) == 0);
		// Transfer exclusive access to the context to this thread for the wait.
		draining = std::async(std::launch::async, [&]
		{
			@autoreleasepool
			{
				started.set_value();
				context.drain();
			}
		});
		REQUIRE(start.wait_for(std::chrono::seconds{ 5 }) == std::future_status::ready);
		CHECK(draining.wait_for(std::chrono::milliseconds{ 50 }) == std::future_status::timeout);
		event->setSignaledValue(1);
		REQUIRE(draining.wait_for(std::chrono::seconds{ 5 }) == std::future_status::ready);
		draining.get();
		CHECK(discarded->status() == MTL::CommandBufferStatusNotEnqueued);
		CHECK(context.getCommandBuffer() == nullptr);
		CHECK(AvailableFrameSlots(context.getSemaphore()) == FrameSlots);
	}

	TEST_CASE("MetalFrameSynchronization.command_buffer_creation_failure_returns_slot")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		__attribute__((objc_precise_lifetime)) Siv3DTestCommandQueue* queue = [[Siv3DTestCommandQueue alloc] init];
		queue.backingQueue = (__bridge id<MTLCommandQueue>)renderer->getCommandQueue();
		queue.failCreation = YES;
		MetalFrameContext context;
		for (size_t i = 0; i < 2; ++i)
		{
			CHECK(context.waitForFrame() == 1);
			try
			{
				context.beginFrame((__bridge MTL::CommandQueue*)queue);
				FAIL_CHECK("Command buffer creation failure must throw InternalEngineError");
			}
			catch (const InternalEngineError& error)
			{
				CHECK(error.messageUTF8().contains("MTL::CommandQueue::commandBuffer() failed"));
			}
			CHECK(context.getCommandBuffer() == nullptr);
			REQUIRE(AvailableFrameSlots(context.getSemaphore()) == FrameSlots);
		}

		queue.failCreation = NO;
		CHECK(context.waitForFrame() == 1);
		context.beginFrame((__bridge MTL::CommandQueue*)queue);
		REQUIRE(context.getCommandBuffer());
		context.submit();
		context.waitForLastSubmittedFrame();
		CHECK(AvailableFrameSlots(context.getSemaphore()) == FrameSlots);
		CHECK(context.waitForFrame() == 2);
	}
}

# endif
