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
# include "../Common.hpp"
# include "../Polygon.hpp"

namespace s3d
{
	namespace Geometry2D
	{
		//////////////////////////////////////////////////
		//
		//	ConvexHull
		//
		//////////////////////////////////////////////////

		/// @brief 一連の点から凸包を計算します。
		/// @param points 一連の点
		/// @return 凸包
		[[nodiscard]]
		Polygon ConvexHull(const std::span<const Point> points);
		
		/// @brief 一連の点から凸包を計算します。
		/// @param points 一連の点
		/// @return 凸包
		[[nodiscard]]
		Polygon ConvexHull(const std::span<const Float2> points);
		
		/// @brief 一連の点から凸包を計算します。
		/// @param points 一連の点
		/// @return 凸包
		[[nodiscard]]
		Polygon ConvexHull(const std::span<const Vec2> points);
	}
}
