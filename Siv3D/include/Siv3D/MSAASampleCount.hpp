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
	//	MSAASampleCount
	//
	////////////////////////////////////////////////////////////////

	/// @brief シーンのマルチサンプリングアンチエイリアス (MSAA) の設定
	enum class MSAASampleCount : uint8
	{
		/// @brief MSAA を使用しない
		NoAA	= 1,

		/// @brief 2x MSAA を使用する
		X2MSAA	= 2,

		/// @brief 4x MSAA を使用する
		X4MSAA	= 4,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, MSAASampleCount value);
}
