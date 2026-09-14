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
		// 未送信の枠を保持していないときに呼ぶ。
		[[nodiscard]]
		size_t waitForFrame();

		// waitForFrame() の後に一度呼び、記録用のコマンドバッファを作成する。
		// 作成に失敗した場合は枠を返し、InternalEngineError を送出する。
		void beginFrame(MTL::CommandQueue* commandQueue);

		// 記録中のコマンドバッファを借用する。記録中でなければ nullptr。
		// 借用は submit() / cancel() まで。送信は Context に任せること。
		[[nodiscard]]
		MTL::CommandBuffer* getCommandBuffer() const noexcept;

		// 記録中のフレームに完了通知を登録し、送信する。すべての encoder を終了してから呼ぶ。
		void submit();

		// 未送信のフレームを破棄し、枠を返す。取得した枠がなければ何もしない。
		// すべての encoder を終了してから呼ぶ。次の waitForFrame() は取り消した番号を返す。
		void cancel() noexcept;

		// 最後に送信したフレームの完了を待つ。一度も送信していなければ何もしない。
		void waitForLastSubmittedFrame() const;

		// 未送信のフレームを取り消し、全フレームの GPU 完了とエラー記録を待つ。
		// 描画スレッドから、すべての encoder を終了し、新たな送信を停止して呼ぶこと。
		// Context 外で送信した転送などは待たない。ログ出力は reportErrors() で行う。
		void drain() noexcept;

		// 描画スレッドから呼び、完了通知が記録したエラーをログに出力する。
		// 前回の回収以降に記録された先頭のエラーと件数をまとめ、一度だけ出力する。
		void reportErrors();

		[[nodiscard]]
		dispatch_semaphore_t getSemaphore() const noexcept;

	private:

		struct CompletionState;

		std::shared_ptr<CompletionState> m_completionState;

		NS::SharedPtr<MTL::CommandBuffer> m_commandBuffer;

		NS::SharedPtr<MTL::CommandBuffer> m_lastSubmittedCommandBuffer;

		// 番号は送信時に進める。取り消した番号は次回も再利用する。
		size_t m_frameIndex = 1;

		uint64 m_submissionCount = 0;

		bool m_frameAcquired = false;
	};
}
