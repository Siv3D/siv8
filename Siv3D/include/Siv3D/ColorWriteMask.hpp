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
# include "Common.hpp"
# include <ThirdParty/EnumBitmask/EnumBitmask.hpp>

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	ColorWriteMask
	//
	////////////////////////////////////////////////////////////////

	/// @brief 色書き込みマスク | Color write mask
	enum class ColorWriteMask : uint8
	{
		/// @brief 赤成分を書き込む
		Red		= 0b0001,

		/// @brief 緑成分を書き込む
		Green	= 0b0010,

		/// @brief 青成分を書き込む
		Blue	= 0b0100,

		/// @brief アルファ成分を書き込む
		Alpha	= 0b1000,

		/// @brief 赤・緑・青・アルファ成分を書き込む
		All		= (Red | Green | Blue | Alpha)
	};
	DEFINE_BITMASK_OPERATORS(ColorWriteMask);

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 色書き込みマスクを文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 色書き込みマスク
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, ColorWriteMask value);
}
