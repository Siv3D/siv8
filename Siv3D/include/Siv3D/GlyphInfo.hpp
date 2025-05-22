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

		float left = 0.0f;

		float top = 0.0f;

		float width = 0.0f;

		float height = 0.0f;

		float ascender = 0.0f;

		float descender = 0.0f;

		/// @brief 次のグリフへの移動量
		float advance = 0.0f;

		[[nodiscard]]
		constexpr Vec2 getOffset(double scale = 1.0) const noexcept;

		[[nodiscard]]
		constexpr Vec2 getBase(double scale = 1.0) const noexcept;
	};
}

# include "detail/GlyphInfo.ipp"
