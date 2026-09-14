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

# pragma once
# include <memory>
# include "Metal.hpp"

namespace s3d
{
	// GPU で処理中のフレーム数と、再利用するフレームの番号を管理する。
	// フレームは同一の command queue に順番に送信すること。
	class MetalFrameContext
	{
	public:

		static constexpr size_t MaxInflightFrames = 3;

		MetalFrameContext();

		~MetalFrameContext();

		MetalFrameContext(const MetalFrameContext&) = delete;

		MetalFrameContext& operator =(const MetalFrameContext&) = delete;

		// 空き枠を取得し、このフレームで使用できるバッファ番号を返す。
		[[nodiscard]]
		size_t waitForFrame();

		// 取得した枠を GPU 完了時に返す。呼び出し元で commandBuffer を commit すること。
		void releaseOnCompletion(MTL::CommandBuffer* commandBuffer);

		[[nodiscard]]
		dispatch_semaphore_t getSemaphore() const noexcept;

	private:

		struct Semaphore;

		std::shared_ptr<Semaphore> m_semaphore;

		size_t m_frameIndex = 0;

		bool m_frameAcquired = false;
	};
}
