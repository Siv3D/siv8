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
	////////////////////////////////////////////////////////////////
	//
	//	UnicodeDecodeStatus
	//
	////////////////////////////////////////////////////////////////

	/// @brief 逐次 Unicode デコードの結果
	enum class UnicodeDecodeStatus : uint8
	{
		/// @brief 入力を消費しましたが、文字の完成には追加入力が必要です。出力はありません。
		NeedMore,

		/// @brief 入力を消費し、Unicode スカラー値を出力しました。
		Ready,

		/// @brief 不正なシーケンスの maximal subpart を U+FFFD に置換して出力しました。
		Invalid,
	};
}
