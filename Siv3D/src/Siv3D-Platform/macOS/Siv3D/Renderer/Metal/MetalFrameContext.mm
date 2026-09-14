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
# include <cassert>

namespace s3d
{
	struct MetalFrameContext::Semaphore
	{
		dispatch_semaphore_t handle = dispatch_semaphore_create(MaxInflightFrames);

		~Semaphore()
		{
			dispatch_release(handle);
		}
	};

	MetalFrameContext::MetalFrameContext()
		: m_semaphore{ std::make_shared<Semaphore>() } {}

	MetalFrameContext::~MetalFrameContext()
	{
		cancel();
	}

	size_t MetalFrameContext::waitForFrame()
	{
		assert(not m_frameAcquired);
		dispatch_semaphore_wait(m_semaphore->handle, DISPATCH_TIME_FOREVER);

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
		// Context が先に破棄されても、同期オブジェクトは GPU 完了まで保持する。
		const auto semaphore = m_semaphore;
		m_commandBuffer->addCompletedHandler(^(MTL::CommandBuffer*)
		{
			dispatch_semaphore_signal(semaphore->handle);
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
		dispatch_semaphore_signal(m_semaphore->handle);
	}

	void MetalFrameContext::waitForLastSubmittedFrame() const
	{
		if (m_lastSubmittedCommandBuffer)
		{
			m_lastSubmittedCommandBuffer->waitUntilCompleted();
		}
	}

	dispatch_semaphore_t MetalFrameContext::getSemaphore() const noexcept
	{
		return m_semaphore->handle;
	}
}
