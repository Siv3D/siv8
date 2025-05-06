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
# include <ThirdParty/EnumBitmask/EnumBitmask.hpp>

namespace s3d
{
	struct FormatData;

	/// @brief フォントのスタイル
	enum class FontStyle : uint8
	{
		/// @brief デフォルト
		Default				= 0x0,

		/// @brief ボールド
		Bold				= 0x01,

		/// @brief イタリック
		Italic				= 0x02,

		/// @brief ビットマップ
		Bitmap				= 0x04,

		/// @brief ボールド + イタリック
		BoldItalic			= (Bold | Italic),

		/// @brief ボールド + ビットマップ
		BoldBitmap			= (Bold | Bitmap),

		/// @brief イタリック + ビットマップ
		ItalicBitmap		= (Italic | Bitmap),

		/// @brief ボールド + イタリック + ビットマップ
		BoldItalicBitmap	= (Bold | Italic | Bitmap),
	};
	DEFINE_BITMASK_OPERATORS(FontStyle);

	/// @brief フォントのスタイルを文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value フォントのスタイル
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, FontStyle value);
}
