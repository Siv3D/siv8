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
	//	TextureAddressMode
	//
	////////////////////////////////////////////////////////////////

	/// @brief テクスチャアドレスモード | Texture address mode
	enum class TextureAddressMode : uint8
	{
		/// @brief 繰り返し
		Repeat		= 1,

		/// @brief ミラーで繰り返し
		Mirror		= 2,

		/// @brief -1.0 ～ 1.0 の範囲にクランプ
		MirrorClamp	= 3,

		/// @brief 0.0 ～ 1.0 の範囲にクランプ
		Clamp		= 4,

		/// @brief 0.0 ～ 1.0 の範囲にクランプし、範囲外のピクセルは `borderColor`
		BorderColor = 5,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, TextureAddressMode value);
}
