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
# include "ReadingDirection.hpp"

namespace s3d
{
	/// @brief TextEffect のためのグリフコンテキスト
	struct GlyphContext
	{
		Vec2 pos;

		int32 index = 0;

		int32 totalGlyphCount = 0;

		double top = 0.0;

		double bottom = 0.0;

		ReadingDirection readingDirection = ReadingDirection::LeftToRight;

		bool isColorGlyph = false;
	};
}
