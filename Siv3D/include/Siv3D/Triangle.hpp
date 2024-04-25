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
# include "PointVector.hpp"
# include "ColorHSV.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Triangle
	//
	////////////////////////////////////////////////////////////////

	/// @brief 三角形
	struct Triangle
	{
		using position_type = Vec2;

		using value_type = position_type::value_type;

		position_type p0;

		position_type p1;

		position_type p2;

	};
}
