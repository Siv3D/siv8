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
# include "IGlyphRenderer.hpp"

namespace s3d
{
	struct BasicGlyphRenderer : public IGlyphRenderer
	{
		/// @brief 描画時に使用する色
		ColorF color = Palette::White;

		BasicGlyphRenderer() = default;

		explicit constexpr BasicGlyphRenderer(const ColorF& _color) noexcept;

		void draw(const TextureRegion& textureRegion, const Vec2& pos, int32 index, double top, double bottom, bool isColorGlyph) const override;
	};
}

# include "detail/BasicGlyphRenderer.ipp"
