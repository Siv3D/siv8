//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include "SplineLocation.hpp"
# include "PointVector.hpp"

namespace s3d
{
	/// @brief 指定した点に最も近い Spline2D 上の位置です。
	struct SplineClosestPoint
	{
		/// @brief 曲線上の区間番号とパラメータ
		SplineLocation location;

		/// @brief 曲線上の座標
		Vec2 point{ 0, 0 };

		/// @brief 検索に使った点から point までの距離の二乗
		double distanceSq = 0.0;
	};
}
