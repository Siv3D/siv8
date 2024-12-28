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
	//	SampleBorderColor
	//
	////////////////////////////////////////////////////////////////

	/// @brief サンプリング時の境界色 | Border color for sampling
	enum class SampleBorderColor : uint8
	{
		/// @brief 透明な黒 (0, 0, 0, 0) | Transparent black (0, 0, 0, 0)
		TransparentBlack = 0,
		
		/// @brief 不透明な黒 (0, 0, 0, 1) | Opaque black (0, 0, 0, 1)
		OpaqueBlack = 1,
		
		/// @brief 不透明な白 (1, 1, 1, 1) | Opaque white (1, 1, 1, 1)
		OpaqueWhite = 2,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief テクスチャフィルタの手法を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value サンプリング時の境界色
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, SampleBorderColor value);
}

