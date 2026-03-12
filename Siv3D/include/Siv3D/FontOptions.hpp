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
	/// @brief フォントのオプション
	struct FontOptions
	{
		/// @brief コレクション（1 ファイルに複数の書体が含まれる場合）のインデックス
		size_t faceIndex = 0;

		/// @brief Variable Font の場合の定義済みスタイル名
		String styleName = U"";

		/// @brief ボールド修飾を適用するか
		bool bold = false;

		/// @brief イタリック修飾を適用するか
		bool italic = false;

		/// @brief 書体が埋め込みビットマップ（絵文字など）を持っていればそれを優先して利用するか
		bool preferEmbeddedBitmap = false;
	};
}
