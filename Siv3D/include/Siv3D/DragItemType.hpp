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
}
