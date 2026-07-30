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
# include "Image.hpp"
# include "Duration.hpp"

namespace s3d
{
	/// @brief アニメーション画像の 1 フレーム
	struct AnimatedImageFrame
	{
		/// @brief 合成済みのフルキャンバス画像
		Image image;

		/// @brief フレームの表示時間
		Duration duration{ 0 };
	};
}
