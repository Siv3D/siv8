//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "ImageFormat.hpp"
# include "ImagePixelFormat.hpp"
# include "PointVector.hpp"

namespace s3d
{	
	////////////////////////////////////////////////////////////////
	//
	//	ImageInfo
	//
	////////////////////////////////////////////////////////////////

	/// @brief 画像ファイルの情報
	struct ImageInfo
	{
		/// @brief 画像のサイズ（ピクセル）
		Size size{ 0, 0 };

		/// @brief 画像のエンコード形式
		ImageFormat imageFormat = ImageFormat::Unknown;

		/// @brief 画像のピクセルフォーマット
		ImagePixelFormat pixelFormat = ImagePixelFormat::Unknown;

		/// @brief アニメーションの有無
		bool isAnimated = false;
	};
}

