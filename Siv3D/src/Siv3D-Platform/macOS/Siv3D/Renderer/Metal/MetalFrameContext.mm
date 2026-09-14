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

# include "MetalFrameContext.hpp"
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <atomic>
# include <cassert>
# include <mutex>

namespace s3d
{
	struct MetalFrameContext::CompletionState
	{
		struct ErrorRecord
		{
			NS::SharedPtr<NS::Error> error;

			uint64 submissionID = 0;

			uint64 count = 0;
		};

		dispatch_semaphore_t semaphore = dispatch_semaphore_create(MaxInflightFrames);

		std::atomic<bool> hasError = false;

		std::mutex errorMutex;

		ErrorRecord pendingError;

		~CompletionState()
		{
			dispatch_release(semaphore);
		}
	};

	MetalFrameContext::MetalFrameContext()
		: m_completionState{ std::make_shared<CompletionState>() } {}

	MetalFrameContext::~MetalFrameContext()
	{
		cancel();
	}

	size_t MetalFrameContext::waitForFrame()
	{
		assert(not m_frameAcquired);
		dispatch_semaphore_wait(m_completionState->semaphore, DISPATCH_TIME_FOREVER);

		m_frameAcquired = true;
		return m_frameIndex;
	}

	void MetalFrameContext::beginFrame(MTL::CommandQueue* commandQueue)
	{
		assert(m_frameAcquired);
		assert(not m_commandBuffer);
		const auto autoreleasePool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		m_commandBuffer = NS::RetainPtr(commandQueue->commandBuffer());
		if (not m_commandBuffer)
		{
			cancel();
			throw InternalEngineError{ "MTL::CommandQueue::commandBuffer() failed" };
		}
	}

	MTL::CommandBuffer* MetalFrameContext::getCommandBuffer() const noexcept
	{
		return m_commandBuffer.get();
	}

	void MetalFrameContext::submit()
	{
		assert(m_frameAcquired);
		assert(m_commandBuffer);
		// Context が先に破棄されても、同期オブジェクトとエラー記録先を保持する。
		const auto completionState = m_completionState;
		const uint64 submissionID = ++m_submissionCount;
		m_commandBuffer->addCompletedHandler(^(MTL::CommandBuffer* completed)
		{
			const ScopeExit releaseSlot{ [&]
			{
				dispatch_semaphore_signal(completionState->semaphore);
			} };
			if (completed->status() == MTL::CommandBufferStatusError)
			{
				const std::lock_guard lock{ completionState->errorMutex };
				auto& pending = completionState->pendingError;
				if (pending.count == 0)
				{
					pending.error = NS::RetainPtr(completed->error());
					pending.submissionID = submissionID;
				}
				++pending.count;
				completionState->hasError.store(true, std::memory_order_release);
			}
		});
		m_commandBuffer->commit();
		m_lastSubmittedCommandBuffer = std::move(m_commandBuffer);
		m_frameAcquired = false;
		m_frameIndex = ((m_frameIndex + 1) % MaxInflightFrames);
	}

	void MetalFrameContext::cancel() noexcept
	{
		if (not m_frameAcquired)
		{
			return;
		}

		m_commandBuffer.reset();
		m_frameAcquired = false;
		dispatch_semaphore_signal(m_completionState->semaphore);
	}

	void MetalFrameContext::waitForLastSubmittedFrame() const
	{
		if (m_lastSubmittedCommandBuffer)
		{
			m_lastSubmittedCommandBuffer->waitUntilCompleted();
		}
	}

	void MetalFrameContext::reportErrors()
	{
		if (not m_completionState->hasError.load(std::memory_order_acquire))
		{
			return;
		}

		CompletionState::ErrorRecord record;
		{
			const std::lock_guard lock{ m_completionState->errorMutex };
			record = std::move(m_completionState->pendingError);
			m_completionState->pendingError = {};
			m_completionState->hasError.store(false, std::memory_order_release);
		}

		// NSError の文字列生成とエンジンへのアクセスは、完了コールバックの外で行う。
		const auto autoreleasePool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		if (record.error)
		{
			const NS::String* domain = record.error->domain();
			const NS::String* description = record.error->localizedDescription();
			const char* domainText = (domain ? domain->utf8String() : nullptr);
			const char* details = (description ? description->utf8String() : nullptr);
			LOG_ERROR(fmt::format("Metal GPU execution failed (first submission = {}, failures = {}, domain = {}, code = {}): {}",
				record.submissionID, record.count, (domainText ? domainText : "Unknown"), record.error->code(),
				(details ? details : "No Metal error details")));
		}
		else
		{
			LOG_ERROR(fmt::format("Metal GPU execution failed (first submission = {}, failures = {}): No Metal error details",
				record.submissionID, record.count));
		}
	}

	dispatch_semaphore_t MetalFrameContext::getSemaphore() const noexcept
	{
		return m_completionState->semaphore;
	}
}
