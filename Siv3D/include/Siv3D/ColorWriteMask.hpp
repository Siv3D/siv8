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
# include <ThirdParty/EnumBitmask/EnumBitmask.hpp>

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	ColorWriteMask
	//
	////////////////////////////////////////////////////////////////

	/// @brief 色の書き込みマスク
	enum class ColorWriteMask : uint8
	{
		/// @brief 赤成分
		Red		= 0b0001,

		/// @brief 緑成分
		Green	= 0b0010,

		/// @brief 青成分
		Blue	= 0b0100,

		/// @brief アルファ成分
		Alpha	= 0b1000,

		/// @brief 赤・緑・青・アルファ成分
		All		= (Red | Green | Blue | Alpha)
	};
	DEFINE_BITMASK_OPERATORS(ColorWriteMask);

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, ColorWriteMask value);
}
