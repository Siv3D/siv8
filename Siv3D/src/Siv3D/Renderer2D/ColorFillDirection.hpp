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

namespace s3d
{
	enum class ColorFillDirection
	{
		/// @brief color0 が内側、color1 が外側
		InOut,

		/// @brief color0 が上、color1 が下
		TopBottom,

		/// @brief color0 が左、color1 が右
		LeftRight,
	};
}
