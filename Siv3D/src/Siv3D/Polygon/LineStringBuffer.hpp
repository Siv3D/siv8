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

	/// @brief 折れ線を指定した距離だけ miter join で太らせた Polygon を作成します。
	/// @param vertices 対象の折れ線
	/// @param distance 折れ線からのバッファ距離。太さの半径に相当します。
	/// @param closeRing 折れ線を閉じた輪郭として扱う場合は `CloseRing::Yes`
	/// @return 作成された Polygon. 作成に失敗した場合は空の Polygon
	Polygon ComputeMiterBufferPolygon(const LineString& vertices, const double distance, const CloseRing closeRing);

	////////////////////////////////////////////////////////////////
	//
	//	ComputeRoundBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	/// @brief 折れ線を指定した距離だけ round join で太らせた Polygon を作成します。
	/// @param vertices 対象の折れ線
	/// @param distance 折れ線からのバッファ距離。太さの半径に相当します。
	/// @param closeRing 折れ線を閉じた輪郭として扱う場合は `CloseRing::Yes`
	/// @param qualityFactor 角および端点の丸め部分の分割品質
	/// @return 作成された Polygon. 作成に失敗した場合は空の Polygon
	Polygon ComputeRoundBufferPolygon(const LineString& vertices, const double distance, const CloseRing closeRing, const QualityFactor& qualityFactor);
}
