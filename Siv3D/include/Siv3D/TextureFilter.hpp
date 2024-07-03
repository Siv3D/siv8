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

	void Formatter(FormatData& formatData, TextureFilter value);
}
