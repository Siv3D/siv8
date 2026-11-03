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
# include "PointVector.hpp"

namespace s3d
{
	class Image;

	namespace ImageProcessing
	{
		/// @brief ミップマップのレベル数を計算します。
		/// @param width 画像の幅（ピクセル）
		/// @param height 画像の高さ（ピクセル）
		/// @return ミップマップのレベル数
		[[nodiscard]]
		constexpr uint8 CalculateMipmapLevel(size_t width, size_t height) noexcept;

		/// @brief 完全なミップマップ列の指定レベルのサイズを計算します。
		/// @param baseSize レベル 0 のサイズ（ピクセル）
		/// @param level ミップレベル（0 が元のサイズ）
		/// @return 各辺を切り捨て半分にし、最低 1 としたサイズ。非正サイズ、列の範囲外のレベルでは { 0, 0 }
		[[nodiscard]]
		constexpr Size CalculateMipmapSize(Size baseSize, uint32 level) noexcept;

		[[nodiscard]]
		Image Resize(const Image& src, const Size& size);
	}
}

# include "detail/ImageProcessing.ipp"
