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
# include <Siv3D/Renderer/Metal/MetalFrameContext.hpp>
# include <future>
# include <iostream>
# include <latch>
# include <ranges>
# include <sstream>

// Deliver controlled completion results through the real registered handler.
// These objects implement only the selectors used by MetalFrameContext.
@interface Siv3DTestFrameCommandBuffer : NSObject
@property(nonatomic) MTLCommandBufferStatus status;
@property(nonatomic, strong) NSError* error;
@property(nonatomic, copy) MTLCommandBufferHandler completion;
- (void)addCompletedHandler:(MTLCommandBufferHandler)handler;
- (void)commit;
- (void)completeWithStatus:(MTLCommandBufferStatus)status error:(NSError*)error;
@end

@implementation Siv3DTestFrameCommandBuffer
- (void)addCompletedHandler:(MTLCommandBufferHandler)handler
{
	self.completion = handler;
}
- (void)commit
{
	self.status = MTLCommandBufferStatusCommitted;
}
- (void)completeWithStatus:(MTLCommandBufferStatus)status error:(NSError*)error
{
	self.status = status;
	self.error = error;
	MTLCommandBufferHandler handler = self.completion;
	self.completion = nil;
	handler((id<MTLCommandBuffer>)self);
}
@end

@interface Siv3DTestFrameCommandQueue : NSObject
- (id<MTLCommandBuffer>)commandBuffer;
@end

@implementation Siv3DTestFrameCommandQueue
- (id<MTLCommandBuffer>)commandBuffer
{
	return (id<MTLCommandBuffer>)[[Siv3DTestFrameCommandBuffer alloc] init];
}
@end

namespace s3d
{
	namespace
	{
		class LogCapture
		{
		public:

			LogCapture() : m_previous{ std::clog.rdbuf(m_stream.rdbuf()) } {}

			~LogCapture()
			{
				std::clog.rdbuf(m_previous);
			}

			std::string take()
			{
				auto text = m_stream.str();
				m_stream.str({});
				return text;
			}

		private:

			std::ostringstream m_stream;

			std::streambuf* m_previous;
		};

		Siv3DTestFrameCommandBuffer* Submit(MetalFrameContext& context, Siv3DTestFrameCommandQueue* queue)
		{
			(void)context.waitForFrame();
			context.beginFrame((__bridge MTL::CommandQueue*)queue);
			auto* buffer = (__bridge Siv3DTestFrameCommandBuffer*)context.getCommandBuffer();
			context.submit();
			REQUIRE(buffer.status == MTLCommandBufferStatusCommitted);
			return buffer;
		}

		auto CompleteAsync(Siv3DTestFrameCommandBuffer* buffer, MTLCommandBufferStatus status, NSError* error = nil)
		{
			return std::async(std::launch::async, [buffer, status, error]
			{
				@autoreleasepool
				{
					[buffer completeWithStatus:status error:error];
				}
			});
		}

		size_t AvailableFrameSlots(dispatch_semaphore_t semaphore)
		{
			size_t available = 0;
			while (dispatch_semaphore_wait(semaphore, DISPATCH_TIME_NOW) == 0)
			{
				++available;
			}
			for (size_t i = 0; i < available; ++i)
			{
				dispatch_semaphore_signal(semaphore);
			}
			return available;
		}

		uint64 ReportedFailures(const std::string& log)
		{
			constexpr std::string_view prefix = "failures = ";
			uint64 count = 0;
			for (size_t pos = log.find(prefix); pos != std::string::npos; pos = log.find(prefix, (pos + 1)))
			{
				count += std::stoull(log.substr(pos + prefix.size()));
			}
			return count;
		}
	}

	TEST_CASE("MetalFrameErrors.success_does_not_report")
	{
		MetalFrameContext context;
		Siv3DTestFrameCommandQueue* queue = [[Siv3DTestFrameCommandQueue alloc] init];
		LogCapture log;
		context.reportErrors();
		auto* buffer = Submit(context, queue);
		CompleteAsync(buffer, MTLCommandBufferStatusCompleted).get();
		CHECK(log.take().empty());
		context.reportErrors();
		CHECK(log.take().empty());
		CHECK(AvailableFrameSlots(context.getSemaphore()) == MetalFrameContext::MaxInflightFrames);
	}

	TEST_CASE("MetalFrameErrors.details_are_deferred_and_consumed_once")
	{
		MetalFrameContext context;
		Siv3DTestFrameCommandQueue* queue = [[Siv3DTestFrameCommandQueue alloc] init];
		LogCapture log;
		// Successful submissions also advance the diagnostic sequence number.
		CompleteAsync(Submit(context, queue), MTLCommandBufferStatusCompleted).get();
		auto* buffer = Submit(context, queue);
		__weak NSError* weakError;
		@autoreleasepool
		{
			NSError* error = [NSError errorWithDomain:@"Siv3DTestGPUError" code:9
				userInfo:@{ NSLocalizedDescriptionKey: @"Injected GPU failure" }];
			weakError = error;
			CompleteAsync(buffer, MTLCommandBufferStatusError, error).get();
			// The pending record must own the error independently of the buffer.
			buffer.error = nil;
		}
		REQUIRE(weakError != nil);
		CHECK(log.take().empty());
		CHECK(AvailableFrameSlots(context.getSemaphore()) == MetalFrameContext::MaxInflightFrames);

		context.reportErrors();
		const auto output = log.take();
		CHECK(output.contains("[error] Metal GPU execution failed"));
		CHECK(output.contains("first submission = 2"));
		CHECK(output.contains("failures = 1"));
		CHECK(output.contains("domain = Siv3DTestGPUError"));
		CHECK(output.contains("code = 9"));
		CHECK(output.contains("Injected GPU failure"));
		CHECK(weakError == nil);
		context.reportErrors();
		CHECK(log.take().empty());
	}

	TEST_CASE("MetalFrameErrors.missing_details_and_multiple_failures")
	{
		MetalFrameContext context;
		Siv3DTestFrameCommandQueue* queue = [[Siv3DTestFrameCommandQueue alloc] init];
		LogCapture log;
		for (size_t i = 0; i < MetalFrameContext::MaxInflightFrames; ++i)
		{
			NSError* error = ((i == 0) ? nil : [NSError errorWithDomain:@"LaterError" code:2 userInfo:nil]);
			CompleteAsync(Submit(context, queue), MTLCommandBufferStatusError, error).get();
		}
		CHECK(log.take().empty());
		context.reportErrors();
		const auto output = log.take();
		CHECK(output.contains("first submission = 1"));
		CHECK(output.contains("failures = 3"));
		CHECK(output.contains("No Metal error details"));
		CHECK_FALSE(output.contains("LaterError"));
		CHECK(AvailableFrameSlots(context.getSemaphore()) == MetalFrameContext::MaxInflightFrames);
		context.reportErrors();
		CHECK(log.take().empty());

		CompleteAsync(Submit(context, queue), MTLCommandBufferStatusError).get();
		context.reportErrors();
		const auto next = log.take();
		CHECK(next.contains("first submission = 4"));
		CHECK(next.contains("failures = 1"));
	}

	TEST_CASE("MetalFrameErrors.completion_outlives_context")
	{
		auto context = std::make_unique<MetalFrameContext>();
		Siv3DTestFrameCommandQueue* queue = [[Siv3DTestFrameCommandQueue alloc] init];
		LogCapture log;
		auto* buffer = Submit(*context, queue);
		__weak dispatch_semaphore_t weakSemaphore = context->getSemaphore();
		context.reset();
		REQUIRE(weakSemaphore != nil);
		dispatch_semaphore_t semaphore = weakSemaphore;
		CHECK(AvailableFrameSlots(semaphore) == (MetalFrameContext::MaxInflightFrames - 1));
		CompleteAsync(buffer, MTLCommandBufferStatusError).get();
		CHECK(AvailableFrameSlots(semaphore) == MetalFrameContext::MaxInflightFrames);
		CHECK(log.take().empty());
	}

	TEST_CASE("MetalFrameErrors.shutdown_drains_pending_errors")
	{
		MetalFrameContext context;
		Siv3DTestFrameCommandQueue* queue = [[Siv3DTestFrameCommandQueue alloc] init];
		std::array<Siv3DTestFrameCommandBuffer*, MetalFrameContext::MaxInflightFrames> buffers;
		for (auto& buffer : buffers)
		{
			buffer = Submit(context, queue);
		}
		LogCapture log;
		auto completion = std::async(std::launch::async, [&]
		{
			@autoreleasepool
			{
				// Complete the newest first so waiting for only that frame is insufficient.
				for (auto* buffer : buffers | std::views::reverse)
				{
					[buffer completeWithStatus:MTLCommandBufferStatusError error:nil];
				}
			}
		});
		context.drain();
		context.reportErrors();
		CHECK(ReportedFailures(log.take()) == MetalFrameContext::MaxInflightFrames);
		completion.get();
		CHECK(AvailableFrameSlots(context.getSemaphore()) == MetalFrameContext::MaxInflightFrames);
		context.drain();
		context.reportErrors();
		CHECK(log.take().empty());
	}

	TEST_CASE("MetalFrameErrors.concurrent_completion_and_reporting")
	{
		MetalFrameContext context;
		Siv3DTestFrameCommandQueue* queue = [[Siv3DTestFrameCommandQueue alloc] init];
		LogCapture log;
		for (size_t round = 0; round < 8; ++round)
		{
			std::latch start{ 1 };
			std::array<std::future<void>, MetalFrameContext::MaxInflightFrames> completions;
			ScopeExit releaseStart{ [&] { start.count_down(); } };
			for (auto& completion : completions)
			{
				auto* buffer = Submit(context, queue);
				completion = std::async(std::launch::async, [buffer, &start]
				{
					start.wait();
					@autoreleasepool
					{
						[buffer completeWithStatus:MTLCommandBufferStatusError error:nil];
					}
				});
			}
			start.count_down();
			releaseStart.release();
			context.reportErrors();
			for (auto& completion : completions)
			{
				completion.get();
			}
			context.reportErrors();
			CHECK(ReportedFailures(log.take()) == MetalFrameContext::MaxInflightFrames);
			CHECK(AvailableFrameSlots(context.getSemaphore()) == MetalFrameContext::MaxInflightFrames);
			context.reportErrors();
			CHECK(log.take().empty());
		}
	}
}

# endif
