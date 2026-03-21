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
	//	DragItemType
	//
	////////////////////////////////////////////////////////////////

	/// @brief ウィンドウ上でドラッグ中のアイテムを、Siv3D がどの種類として扱うかを表します。
	/// @remark この列挙は高レベル API 向けの分類です。
	/// @remark OS ネイティブのすべての形式をそのまま公開するものではありません。
	enum class DragItemType : uint8
	{
		/// @brief ファイルパス群として扱うアイテムです。
		FilePaths,

		/// @brief テキストとして扱うアイテムです。
		Text,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief DragItemType を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value DragItemType
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, DragItemType value);
}
