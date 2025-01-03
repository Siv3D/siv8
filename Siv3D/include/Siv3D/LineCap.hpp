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
# include "Common.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	LineCap
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線端のスタイル | Line cap
	enum class LineCap : uint8
	{
		/// @brief フラット | Flat
		Flat,

		/// @brief 正方形 | Square
		Square,

		/// @brief 丸 | Round
		Round,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線端のスタイルを文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 線端のスタイル
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, LineCap value);
}
