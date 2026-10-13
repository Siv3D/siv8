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
		// Catch2 retargets std::clog while reporting assertions. Confine captures to
		// the assertion-free helpers below, joining workers before reading the log.
		class LogCapture
		{
		public:

			LogCapture() : m_previous{ std::clog.rdbuf(m_stream.rdbuf()) } {}

			~LogCapture()
			{
				std::clog.rdbuf(m_previous);
			}

			std::string text() const
			{
				return m_stream.str();
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

		std::string CaptureReportedErrors(MetalFrameContext& context)
		{
			LogCapture log;
			context.reportErrors();
			return log.text();
		}

		std::string CaptureCompletionLog(Siv3DTestFrameCommandBuffer* buffer,
			MTLCommandBufferStatus status, NSError* error = nil)
		{
			LogCapture log;
			auto completion = std::async(std::launch::async, [buffer, status, error]
			{
				@autoreleasepool
				{
					[buffer completeWithStatus:status error:error];
				}
			});
			completion.get();
			return log.text();
		}

		std::string CaptureDrainedErrors(MetalFrameContext& context)
		{
			LogCapture log;
			context.drain();
			context.reportErrors();
			return log.text();
		}

		using SubmittedFrames = std::array<Siv3DTestFrameCommandBuffer*, MetalFrameContext::MaxInflightFrames>;

		std::string CaptureDrainedErrors(MetalFrameContext& context, const SubmittedFrames& buffers)
		{
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
			completion.get();
			return log.text();
		}

		std::string CaptureConcurrentErrors(MetalFrameContext& context, const SubmittedFrames& buffers)
		{
			LogCapture log;
			std::latch start{ 1 };
			std::array<std::future<void>, MetalFrameContext::MaxInflightFrames> completions;
			ScopeExit releaseStart{ [&] { start.count_down(); } };
			for (size_t i = 0; i < buffers.size(); ++i)
			{
				auto* buffer = buffers[i];
				completions[i] = std::async(std::launch::async, [buffer, &start]
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
			return log.text();
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
		CHECK(CaptureReportedErrors(context).empty());
		auto* buffer = Submit(context, queue);
		CHECK(CaptureCompletionLog(buffer, MTLCommandBufferStatusCompleted).empty());
		CHECK(CaptureReportedErrors(context).empty());
		CHECK(AvailableFrameSlots(context.getSemaphore()) == MetalFrameContext::MaxInflightFrames);
	}

	TEST_CASE("MetalFrameErrors.details_are_deferred_and_consumed_once")
	{
		MetalFrameContext context;
		Siv3DTestFrameCommandQueue* queue = [[Siv3DTestFrameCommandQueue alloc] init];
		// Successful submissions also advance the diagnostic sequence number.
		auto* successful = Submit(context, queue);
		CHECK(CaptureCompletionLog(successful, MTLCommandBufferStatusCompleted).empty());
		auto* buffer = Submit(context, queue);
		__weak NSError* weakError;
		std::string completionLog;
		@autoreleasepool
		{
			NSError* error = [NSError errorWithDomain:@"Siv3DTestGPUError" code:9
				userInfo:@{ NSLocalizedDescriptionKey: @"Injected GPU failure" }];
			weakError = error;
			completionLog = CaptureCompletionLog(buffer, MTLCommandBufferStatusError, error);
			// The pending record must own the error independently of the buffer.
			buffer.error = nil;
		}
		REQUIRE(weakError != nil);
		CHECK(completionLog.empty());
		CHECK(AvailableFrameSlots(context.getSemaphore()) == MetalFrameContext::MaxInflightFrames);

		const auto output = CaptureReportedErrors(context);
		CHECK(output.contains("[error] Metal GPU execution failed"));
		CHECK(output.contains("first submission = 2"));
		CHECK(output.contains("failures = 1"));
		CHECK(output.contains("domain = Siv3DTestGPUError"));
		CHECK(output.contains("code = 9"));
		CHECK(output.contains("Injected GPU failure"));
		CHECK(weakError == nil);
		CHECK(CaptureReportedErrors(context).empty());
	}

	TEST_CASE("MetalFrameErrors.missing_details_and_multiple_failures")
	{
		MetalFrameContext context;
		Siv3DTestFrameCommandQueue* queue = [[Siv3DTestFrameCommandQueue alloc] init];
		for (size_t i = 0; i < MetalFrameContext::MaxInflightFrames; ++i)
		{
			NSError* error = ((i == 0) ? nil : [NSError errorWithDomain:@"LaterError" code:2 userInfo:nil]);
			auto* buffer = Submit(context, queue);
			CHECK(CaptureCompletionLog(buffer, MTLCommandBufferStatusError, error).empty());
		}
		const auto output = CaptureReportedErrors(context);
		CHECK(output.contains("first submission = 1"));
		CHECK(output.contains("failures = 3"));
		CHECK(output.contains("No Metal error details"));
		CHECK_FALSE(output.contains("LaterError"));
		CHECK(AvailableFrameSlots(context.getSemaphore()) == MetalFrameContext::MaxInflightFrames);
		CHECK(CaptureReportedErrors(context).empty());

		auto* buffer = Submit(context, queue);
		CHECK(CaptureCompletionLog(buffer, MTLCommandBufferStatusError).empty());
		const auto next = CaptureReportedErrors(context);
		CHECK(next.contains("first submission = 4"));
		CHECK(next.contains("failures = 1"));
	}

	TEST_CASE("MetalFrameErrors.completion_outlives_context")
	{
		auto context = std::make_unique<MetalFrameContext>();
		Siv3DTestFrameCommandQueue* queue = [[Siv3DTestFrameCommandQueue alloc] init];
		auto* buffer = Submit(*context, queue);
		__weak dispatch_semaphore_t weakSemaphore = context->getSemaphore();
		context.reset();
		REQUIRE(weakSemaphore != nil);
		dispatch_semaphore_t semaphore = weakSemaphore;
		CHECK(AvailableFrameSlots(semaphore) == (MetalFrameContext::MaxInflightFrames - 1));
		const auto output = CaptureCompletionLog(buffer, MTLCommandBufferStatusError);
		CHECK(AvailableFrameSlots(semaphore) == MetalFrameContext::MaxInflightFrames);
		CHECK(output.empty());
	}

	TEST_CASE("MetalFrameErrors.shutdown_drains_pending_errors")
	{
		MetalFrameContext context;
		Siv3DTestFrameCommandQueue* queue = [[Siv3DTestFrameCommandQueue alloc] init];
		SubmittedFrames buffers;
		for (auto& buffer : buffers)
		{
			buffer = Submit(context, queue);
		}
		const auto output = CaptureDrainedErrors(context, buffers);
		CHECK(ReportedFailures(output) == MetalFrameContext::MaxInflightFrames);
		CHECK(AvailableFrameSlots(context.getSemaphore()) == MetalFrameContext::MaxInflightFrames);
		CHECK(CaptureDrainedErrors(context).empty());
	}

	TEST_CASE("MetalFrameErrors.concurrent_completion_and_reporting")
	{
		MetalFrameContext context;
		Siv3DTestFrameCommandQueue* queue = [[Siv3DTestFrameCommandQueue alloc] init];
		for (size_t round = 0; round < 8; ++round)
		{
			SubmittedFrames buffers;
			for (auto& buffer : buffers)
			{
				buffer = Submit(context, queue);
			}
			const auto output = CaptureConcurrentErrors(context, buffers);
			CHECK(ReportedFailures(output) == MetalFrameContext::MaxInflightFrames);
			CHECK(AvailableFrameSlots(context.getSemaphore()) == MetalFrameContext::MaxInflightFrames);
			CHECK(CaptureReportedErrors(context).empty());
		}
	}
}

# endif
