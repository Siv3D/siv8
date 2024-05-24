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
# include "2DShapes.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	RandomPoint
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、指定した範囲内のランダムな点を生成して返します。
	/// @param xMinMax 生成する点の x 座標の範囲
	/// @param yMinMax 生成する点の y 座標の範囲
	/// @return 生成されたランダムな点
	[[nodiscard]]
	Point RandomPoint(const std::pair<int32, int32> xMinMax, const std::pair<int32, int32> yMinMax);

	/// @brief 指定した範囲内のランダムな点を生成して返します。
	/// @param xMinMax 生成する点の x 座標の範囲
	/// @param yMinMax 生成する点の y 座標の範囲
	/// @param urbg 乱数エンジン
	/// @return 生成されたランダムな点
	[[nodiscard]]
	Point RandomPoint(const std::pair<int32, int32> xMinMax, const std::pair<int32, int32> yMinMax, Concept::UniformRandomBitGenerator auto&& urbg);

	/// @brief 現在のスレッドの乱数エンジンを用いて、指定した範囲内のランダムな点を生成して返します。
	/// @param rect 生成する点の範囲
	/// @return 生成されたランダムな点
	[[nodiscard]]
	Point RandomPoint(const Rect& rect);

	/// @brief 指定した範囲内のランダムな点を生成して返します。
	/// @param rect 生成する点の範囲
	/// @param urbg 乱数エンジン
	/// @return 生成されたランダムな点
	[[nodiscard]]
	Point RandomPoint(const Rect& rect, Concept::UniformRandomBitGenerator auto&& urbg);

	/// @brief 現在のスレッドの乱数エンジンを用いて、指定した範囲内のランダムな点を生成して返します。
	/// @param xMax 生成する点の x 座標の最大値。生成される x 座標は [0, xMax] の範囲になります。
	/// @param yMax 生成する点の y 座標の最大値。生成される y 座標は [0, yMax] の範囲になります。
	/// @return 生成されたランダムな点
	[[nodiscard]]
	Point RandomPoint(int32 xMax, int32 yMax);

	/// @brief 指定した範囲内のランダムな点を生成して返します。
	/// @param xMax 生成する点の x 座標の最大値。生成される x 座標は [0, xMax] の範囲になります。
	/// @param yMax 生成する点の y 座標の最大値。生成される y 座標は [0, yMax] の範囲になります。
	/// @param urbg 乱数エンジン
	/// @return 生成されたランダムな点
	[[nodiscard]]
	Point RandomPoint(int32 xMax, int32 yMax, Concept::UniformRandomBitGenerator auto&& urbg);
}

# include "detail/RandomPoint.ipp"
