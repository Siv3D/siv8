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

		// Frame completion alone does not join callbacks from intermediate buffers.
		dispatch_group_t partialCompletions = dispatch_group_create();

		std::atomic<bool> hasError = false;

		std::mutex errorMutex;

		ErrorRecord pendingError;

		~CompletionState()
		{
			dispatch_release(semaphore);
			dispatch_release(partialCompletions);
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
		commit(true);
		m_lastSubmittedCommandBuffer = std::move(m_commandBuffer);
		m_lastPartialCommandBuffer.reset();
		m_frameAcquired = false;
		m_frameIndex = ((m_frameIndex + 1) % MaxInflightFrames);
	}

	void MetalFrameContext::submitAndContinue(NS::SharedPtr<MTL::CommandBuffer> next)
	{
		assert(m_frameAcquired && m_commandBuffer && next);
		assert(next.get() != m_commandBuffer.get());
		commit(false);
		m_lastPartialCommandBuffer = std::move(m_commandBuffer);
		m_commandBuffer = std::move(next);
	}

	void MetalFrameContext::commit(const bool releaseFrameSlot)
	{
		// Context が先に破棄されても、同期オブジェクトとエラー記録先を保持する。
		const auto completionState = m_completionState;
		const uint64 submissionID = ++m_submissionCount;
		if (not releaseFrameSlot)
		{
			dispatch_group_enter(completionState->partialCompletions);
		}
		m_commandBuffer->addCompletedHandler(^(MTL::CommandBuffer* completed)
		{
			const ScopeExit releaseSlot{ [&]
			{
				if (releaseFrameSlot)
				{
					dispatch_semaphore_signal(completionState->semaphore);
				}
				else
				{
					dispatch_group_leave(completionState->partialCompletions);
				}
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
	}

	bool MetalFrameContext::flushAndWait(MTL::CommandQueue* commandQueue)
	{
		assert(m_frameAcquired && m_commandBuffer);
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		// 次のバッファを確保してから現在の記録を送信する。
		auto next = NS::RetainPtr(commandQueue->commandBuffer());
		if (not next)
		{
			return false;
		}
		m_commandBuffer->commit();
		m_commandBuffer->waitUntilCompleted();
		// 同じ queue の先行する途中送信も完了済み。
		m_lastPartialCommandBuffer.reset();
		const bool succeeded = (m_commandBuffer->status() == MTL::CommandBufferStatusCompleted);
		if (not succeeded)
		{
			LOG_ERROR("RenderTexture: Metal readback command buffer failed");
		}
		m_commandBuffer = std::move(next);
		return succeeded;
	}

	void MetalFrameContext::cancel() noexcept
	{
		if (not m_frameAcquired)
		{
			return;
		}

		m_commandBuffer.reset();
		if (m_lastPartialCommandBuffer)
		{
			m_lastPartialCommandBuffer->waitUntilCompleted();
			m_lastPartialCommandBuffer.reset();
		}
		m_frameAcquired = false;
		dispatch_semaphore_signal(m_completionState->semaphore);
	}

	bool MetalFrameContext::waitForLastSubmittedFrame() const
	{
		if (not m_lastSubmittedCommandBuffer)
		{
			return false;
		}

		m_lastSubmittedCommandBuffer->waitUntilCompleted();
		return (m_lastSubmittedCommandBuffer->status() == MTL::CommandBufferStatusCompleted);
	}

	void MetalFrameContext::drain() noexcept
	{
		cancel();

		// 完了通知が全枠を返すまで待つことで、各フレームのエラー記録も完了する。
		// 全枠を取得してから返すことで、同じ空き枠を繰り返し取得するのを防ぐ。
		for (size_t i = 0; i < MaxInflightFrames; ++i)
		{
			dispatch_semaphore_wait(m_completionState->semaphore, DISPATCH_TIME_FOREVER);
		}
		for (size_t i = 0; i < MaxInflightFrames; ++i)
		{
			dispatch_semaphore_signal(m_completionState->semaphore);
		}
		dispatch_group_wait(m_completionState->partialCompletions, DISPATCH_TIME_FOREVER);
		m_lastSubmittedCommandBuffer.reset();
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
