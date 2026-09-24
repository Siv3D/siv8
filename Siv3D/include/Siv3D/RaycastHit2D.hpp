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
# include "PointVector.hpp"

namespace s3d
{
	/// @brief レイと境界のヒット情報
	/// @see @ref geometry2d_raycast
	struct RaycastHit2D
	{
		/// @brief ヒット位置
		Vec2 position{ 0.0, 0.0 };
		
		/// @brief 図形の外側へ向く単位法線
		Vec2 normal{ 0.0, 0.0 };
		
		/// @brief レイ始点からヒット位置までの距離
		double distance = 0.0;
		
		/// @brief レイ始点が図形の内部にある場合 true。境界上と穴の内部は false。
		bool startsInside = false;
	};
}
