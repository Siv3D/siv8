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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	MP3Quality
	//
	////////////////////////////////////////////////////////////////

	/// @brief MP3 エンコードの品質 | MP3 encoding quality
	enum class MP3Quality : uint8
	{
		/// @brief 低品質 | Low quality
		Low,

		/// @brief 中品質 | Medium quality
		Medium,

		/// @brief 高品質 | High quality
		High,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief MP3 エンコードの品質を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value MP3 エンコードの品質
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, MP3Quality value);
}
