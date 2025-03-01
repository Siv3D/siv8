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
	//	ImagePixelFormat
	//
	////////////////////////////////////////////////////////////////

	/// @brief 画像のピクセルフォーマット
	enum class ImagePixelFormat : uint8
	{
		/// @brief 不明
		Unknown,

		/// @brief R 8-bit, G 8-bit, B 8-bit
		R8G8B8,

		/// @brief R 8-bit, G 8-bit, B 8-bit, X 8-bit
		R8G8B8X8,

		/// @brief R 8-bit, G 8-bit, B 8-bit, A 8-bit
		R8G8B8A8,

		/// @brief Gray 8-bit
		Gray8,

		/// @brief Gray 16-bit
		Gray16,

		/// @brief Gray 8-bit, A 8-bit
		Gray8A8,

		/// @brief Gray 16-bit, A 16-bit
		Gray16A16,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////
	
	/// @brief 画像のピクセルフォーマットを文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 画像のピクセルフォーマット
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, ImagePixelFormat value);
}
