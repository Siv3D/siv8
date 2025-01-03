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
	//	TextureFilter
	//
	////////////////////////////////////////////////////////////////

	/// @brief テクスチャフィルタの手法 | Texture filter method
	enum class TextureFilter : uint8
	{
		/// @brief 最近傍補間
		Nearest	= 0,

		/// @brief バイリニア補間
		Linear	= 1,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief テクスチャフィルタの手法を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value テクスチャフィルタの手法
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, TextureFilter value);
}
