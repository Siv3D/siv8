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
	struct GradientGlyphRenderer : IGlyphRenderer
	{
		/// @brief グラデーションの開始色
		ColorF startColor = Palette::White;

		/// @brief グラデーションの終了色
		ColorF endColor = Palette::White;

		/// @brief グラデーションの開始位置のオフセット (0.0 から 1.0 の範囲)
		double startOffset = 0.0;

		/// @brief グラデーションの終了位置のオフセット (0.0 から 1.0 の範囲)
		double endOffset = 1.0;

		GradientGlyphRenderer() = default;

		constexpr GradientGlyphRenderer(const ColorF& _startColor, const ColorF& _endColor, double _startOffset = 0.0, double _endOffset = 1.0) noexcept;

		void draw(const TextureRegion& textureRegion, const Vec2& pos, int32 index, double top, double bottom, bool isColorGlyph) const override;
	};
}

# include "detail/GradientGlyphRenderer.ipp"
