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
	//	TriangleFillMode
	//
	////////////////////////////////////////////////////////////////

	/// @brief 三角形の塗りつぶしモード | Triangle fill mode
	enum class TriangleFillMode : uint8
	{
		/// @brief ワイヤーフレーム | Wireframe
		Wireframe	= 0,

		/// @brief ソリッド | Solid
		Solid		= 1,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, TriangleFillMode value);
}
