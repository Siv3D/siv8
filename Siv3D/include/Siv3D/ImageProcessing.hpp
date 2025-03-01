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

namespace s3d
{
	namespace ImageProcessing
	{
		/// @brief ミップマップのレベル数を計算します。
		/// @param width 画像の幅（ピクセル）
		/// @param height 画像の高さ（ピクセル）
		/// @return ミップマップのレベル数
		[[nodiscard]]
		constexpr uint8 CalculateMipmapLevel(size_t width, size_t height) noexcept;
	}
}

# include "detail/ImageProcessing.ipp"
