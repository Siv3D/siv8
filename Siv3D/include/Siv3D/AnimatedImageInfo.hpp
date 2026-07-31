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
# include "Duration.hpp"

namespace s3d
{
	/// @brief アニメーション画像の情報
	struct AnimatedImageInfo
	{
		/// @brief アニメーション画像のサイズ
		Size imageSize{ 0, 0 };

		/// @brief 1 周分のフレーム数
		size_t frameCount = 0;

		/// @brief 1 周分の再生時間
		Duration duration{ 0 };

		/// @brief 総再生回数。0 は無限再生
		uint32 playCount = 1;
	};
}
