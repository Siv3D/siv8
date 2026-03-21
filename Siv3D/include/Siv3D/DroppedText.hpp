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
# include "String.hpp"
# include "PointVector.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	DroppedText
	//
	////////////////////////////////////////////////////////////////

	/// @brief ドロップ済みテキスト 1 件の情報です。
	struct DroppedText
	{
		/// @brief テキストです。
		String text;

		/// @brief ドロップされた位置（クライアント座標）です。
		Point pos;

		/// @brief ドロップされた時刻（ミリ秒）です。
		/// @remark `Time::GetMillisec()` で取得する値と比較できます。
		uint64 timeMillisec;
	};
}
