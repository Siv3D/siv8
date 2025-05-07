//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "GlyphIndex.hpp"

namespace s3d
{
	struct ResolvedGlyph
	{
		/// @brief フォントのインデックス（フォールバックフォントの場合 1 以上の値）
		uint32 fontIndex = 0;

		/// @brief `fontIndex` に対応するフォントでのグリフインデックス
		GlyphIndex glyphIndex = 0;

		/// @brief 入力文字列における位置
		uint32 pos = 0;
	};
}
