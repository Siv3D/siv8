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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	AsyncTaskStatus
	//
	////////////////////////////////////////////////////////////////

	/// @brief 非同期タスクの状態 | Asynchronous task status
	enum class AsyncTaskStatus : uint8
	{
		/// @brief 非同期処理を持っていない | No asynchronous operation
		Invalid,

		/// @brief 非同期処理を実行中 | Asynchronous operation is running
		Running,

		/// @brief 非同期処理が完了し、結果を取得できる | Asynchronous operation is ready
		Ready,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 非同期タスクの状態を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 非同期タスクの状態
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, AsyncTaskStatus value);
}
