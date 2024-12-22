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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	JoinStyle
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線の結合方法 | Line join style
	enum class JoinStyle : uint8
	{
		Default,

		Bevel,

		Round,

		Miter,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線の結合方法を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 線の結合方法
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, JoinStyle value);
}
