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
# include "PointVector.hpp"
# include "DragItemType.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	DragStatus
	//
	////////////////////////////////////////////////////////////////

	/// @brief ウィンドウ上で hover 中の OS ドラッグ状態です。
	/// @remark この API が扱うのは外部アプリケーションとのドラッグ＆ドロップです。
	/// @remark アプリ内 DnD は対象外です。
	/// @remark ドロップ完了後のアイテムはこの構造体ではなく内部キューへ蓄積されます。
	struct DragStatus
	{
		/// @brief ドロップされた場合に使われる想定のクライアント座標です。
		Point cursorPos{ 0, 0 };

		/// @brief 現在のドラッグを、Siv3D がどの種類として扱うかを表します。
		/// @remark ファイルパスとテキストの両方として解釈可能な場合は、ファイルパスが優先されます。
		DragItemType itemType = DragItemType::FilePaths;
	};
}
