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
# include "PointVector.hpp"
# include "GlyphIndex.hpp"

namespace s3d
{
	/// @brief グリフ情報
	struct GlyphInfo
	{
		/// @brief グリフインデックス
		GlyphIndex glyphIndex = 0;

		int16 bufferThickness = 0;
	};
}
