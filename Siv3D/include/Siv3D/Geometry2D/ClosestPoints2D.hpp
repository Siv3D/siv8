//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "../PointVector.hpp"

namespace s3d
{
	/// @brief 最近点対とその距離
	/// @see @ref geometry2d_distance
	struct ClosestPoints2D
	{
		/// @brief ClosestPoints(a, b) の a 上の点
		Vec2 pointA;
		
		/// @brief ClosestPoints(a, b) の b 上の点
		Vec2 pointB;
		
		/// @brief 点対間の距離
		double distance = 0.0;
	};
}
