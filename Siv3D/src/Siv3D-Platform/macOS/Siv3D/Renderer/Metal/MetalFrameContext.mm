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
		// 未送信のフレームには完了通知が来ないので、保持中の枠をここで返す。
		if (m_frameAcquired)
		{
			dispatch_semaphore_signal(m_semaphore->handle);
		}
	}

	size_t MetalFrameContext::waitForFrame()
	{
		assert(not m_frameAcquired);
		dispatch_semaphore_wait(m_semaphore->handle, DISPATCH_TIME_FOREVER);

		m_frameIndex = ((m_frameIndex + 1) % MaxInflightFrames);
		m_frameAcquired = true;
		return m_frameIndex;
	}

	void MetalFrameContext::releaseOnCompletion(MTL::CommandBuffer* commandBuffer)
	{
		assert(m_frameAcquired);
		// Context が先に破棄されても、同期オブジェクトは GPU 完了まで保持する。
		const auto semaphore = m_semaphore;
		commandBuffer->addCompletedHandler(^(MTL::CommandBuffer*)
		{
			dispatch_semaphore_signal(semaphore->handle);
		});
		m_frameAcquired = false;
	}

	dispatch_semaphore_t MetalFrameContext::getSemaphore() const noexcept
	{
		return m_semaphore->handle;
	}
}
