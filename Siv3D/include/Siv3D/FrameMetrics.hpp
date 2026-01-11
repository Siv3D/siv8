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
# include "Common.hpp"
# include "String.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	FrameMetrics
	//
	////////////////////////////////////////////////////////////////

	struct FrameMetrics
	{
		/// @brief フレームのインデックス
		int64 frameIndex = 0;

		/// @brief Draw call の回数
		int64 drawCalls = 0;

		/// @brief 描画した三角形の総数
		int64 triangleCount = 0;

		/// @brief メインループ開始処理（エンジンコード）の CPU 時間（マイクロ秒）
		int64 engineBeginTimeUs = 0;

		/// @brief メインループ（ユーザーコード）の CPU 時間（マイクロ秒）
		int64 userUpdateTimeUs = 0;

		/// @brief メインループ終了処理（エンジンコード）の CPU 時間（マイクロ秒）
		int64 engineEndTimeUs = 0;

		/// @brief 次フレーム開始待ちの CPU 時間（マイクロ秒）
		int64 gpuWaitTimeUs = 0;

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		String format() const;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief FrameMetrics を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value FrameMetrics
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const FrameMetrics& value);
	};
}
