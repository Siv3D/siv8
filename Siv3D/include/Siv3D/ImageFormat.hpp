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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	ImageFormat
	//
	////////////////////////////////////////////////////////////////

	/// @brief 画像のエンコード形式
	enum class ImageFormat : uint8
	{
		/// @brief 不明
		Unknown,

		/// @brief BCn
		BCn,

		/// @brief PNG
		PNG,

		/// @brief JPEG
		JPEG,

		/// @brief JPEG 2000
		JPEG2000,

		/// @brief JPEG XL
		JPEG_XL,

		/// @brief BMP
		BMP,

		/// @brief WebP
		WebP,

		/// @brief GIF
		GIF,

		/// @brief TIFF
		TIFF,

		/// @brief TGA
		TGA,

		/// @brief PPM
		PPM,

		/// @brief SVG
		SVG,

		/// @brief 指定しない（データと拡張子から判断）
		Unspecified = Unknown,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 画像のエンコード形式を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 画像のエンコード形式
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, ImageFormat value);
}
