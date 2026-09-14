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
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <condition_variable>
# include <future>
# include <mutex>

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
			const auto commandBuffer = NS::RetainPtr(renderer->getCommandQueue()->commandBuffer());
			REQUIRE(commandBuffer);
			context.releaseOnCompletion(commandBuffer.get());
			commandBuffer->commit();
			commandBuffer->waitUntilCompleted();
			CHECK(commandBuffer->status() == MTL::CommandBufferStatusCompleted);
			CHECK(AvailableFrameSlots(context.getSemaphore()) == FrameSlots);
		}
	}

	TEST_CASE("MetalFrameSynchronization.context_returns_unsubmitted_slot_on_destruction")
	{
		auto context = std::make_unique<MetalFrameContext>();
		// This test retains the semaphore so its count can be checked after teardown.
		dispatch_semaphore_t semaphore = context->getSemaphore();
		CHECK(context->waitForFrame() == 1);
		CHECK(AvailableFrameSlots(semaphore) == (FrameSlots - 1));
		context.reset();
		CHECK(AvailableFrameSlots(semaphore) == FrameSlots);
	}

	TEST_CASE("MetalFrameSynchronization.completion_outlives_context")
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const auto event = NS::TransferPtr(renderer->getDevice()->newSharedEvent());
		const auto commandBuffer = NS::RetainPtr(renderer->getCommandQueue()->commandBuffer());
		REQUIRE(event);
		REQUIRE(commandBuffer);
		commandBuffer->encodeWait(event.get(), 1);
		const ScopeExit unblockOnExit{ [&] { event->setSignaledValue(1); } };

		auto context = std::make_unique<MetalFrameContext>();
		// A weak reference ensures the test itself does not keep the semaphore alive.
		__weak dispatch_semaphore_t semaphore = context->getSemaphore();
		CHECK(context->waitForFrame() == 1);
		context->releaseOnCompletion(commandBuffer.get());
		commandBuffer->commit();
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
}

# endif
