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
# include <Siv3D/Common.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Polygon.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ComputeMiterBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線分を指定した距離だけ膨張させた Polygon を作成します。
	/// @param line 対象の線分
	/// @param distance 線分からの膨張距離
	/// @return 作成された Polygon. 作成に失敗した場合は空の Polygon
	[[nodiscard]]
	Polygon ComputeMiterBufferPolygon(const Line& line, double distance);

	////////////////////////////////////////////////////////////////
	//
	//	ComputeRoundBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線分を指定した距離だけ膨張させたカプセル形状の Polygon を作成します。
	/// @param line 対象の線分
	/// @param distance 線分からの膨張距離。カプセルの半径に相当します。
	/// @param qualityFactor 半円部分の分割品質
	/// @return 作成された Polygon. 作成に失敗した場合は空の Polygon
	[[nodiscard]]
	Polygon ComputeRoundBufferPolygon(const Line& line, double distance, const QualityFactor& qualityFactor);

	////////////////////////////////////////////////////////////////
	//
	//	ComputeRoundedQuadPolygon
	//
	////////////////////////////////////////////////////////////////

	/// @brief 四角形の各辺を内側に縮めてから外側に丸く膨張させ、角丸四角形の Polygon を作成します。
	/// @param quad 対象の四角形
	/// @param distance 角丸の半径に相当する距離
	/// @param qualityFactor 角丸部分の分割品質
	/// @return 作成された Polygon. 作成に失敗した場合、または異常な Polygon になった場合は空の Polygon
	[[nodiscard]]
	Polygon ComputeRoundedQuadPolygon(const Quad& quad, double distance, const QualityFactor& qualityFactor);

	////////////////////////////////////////////////////////////////
	//
	//	ComputeMiterBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	/// @brief 多角形を指定した距離だけ miter join でオフセットした Polygon を作成します。
	/// @param outer 対象多角形の外周頂点列
	/// @param distance オフセット距離。正の値で外側、負の値で内側にオフセットします。
	/// @return 作成された Polygon. 作成に失敗した場合は空の Polygon
	[[nodiscard]]
	Polygon ComputeMiterBufferPolygon(std::span<const Vec2> outer, double distance);

	////////////////////////////////////////////////////////////////
	//
	//	ComputeRoundBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	/// @brief 多角形を指定した距離だけ round join で膨張または収縮した Polygon を作成します。
	/// @param outer 対象多角形の外周頂点列
	/// @param distance バッファ距離。正の値で外側、負の値で内側にバッファを作成します。
	/// @param qualityFactor 丸め部分の分割品質
	/// @return 作成された Polygon. 作成に失敗した場合は空の Polygon
	[[nodiscard]]
	Polygon ComputeRoundBufferPolygon(std::span<const Vec2> outer, double distance, const QualityFactor& qualityFactor);
}
