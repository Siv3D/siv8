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

	/// @brief メッセージボックスの結果
	enum class MessageBoxResult : uint8
	{
		/// @brief [OK] が押された
		Ok,

		/// @brief [キャンセル] が押されたか、メッセージボックスが閉じられた
		Cancel,

		/// @brief [はい] が押された
		Yes,

		/// @brief [いいえ] が押された
		No,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief メッセージボックスの結果を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value メッセージボックスの結果
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, MessageBoxResult value);
}
