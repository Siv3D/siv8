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
# include <span>
# include <Siv3D/PointVector.hpp>

namespace s3d
{
	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Center
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief 与えられた多角形の到達不能極（Pole of Inaccessibility）を計算します。
		/// @param points 多角形の頂点の配列
		/// @param precision 計算精度
		/// @return 到達不能極の座標
		[[nodiscard]]
		Vec2 PoleOfInaccessibility(std::span<const Vec2> points, double precision = 0.1);
	}
}
