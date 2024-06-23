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
	//	CullMode
	//
	////////////////////////////////////////////////////////////////

	/// @brief カリングモード | Culling mode
	enum class CullMode : uint8
	{
		/// @brief カリングを無効にする | Disable culling
		None	= 1,

		/// @brief 前面をカリングする | Cull front faces
		Front	= 2,

		/// @brief 裏面をカリングする | Cull back faces
		Back	= 3,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, CullMode value);
}
