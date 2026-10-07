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
# include <Siv3D/PointVector.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/Polygon.hpp>

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(5311)
# include <boost/geometry.hpp>
# include <boost/geometry/geometries/register/point.hpp>
# include <boost/geometry/geometries/register/segment.hpp>
# include <boost/geometry/geometries/register/linestring.hpp>
# include <boost/geometry/geometries/register/multi_point.hpp>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	using CwOpenPolygon			= boost::geometry::model::polygon<Vec2, false, false, Array, Array>;
	using CwOpenMultiPolygon	= boost::geometry::model::multi_polygon<CwOpenPolygon>;
	using CWOpenRing			= boost::geometry::model::ring<Vec2, false, false, Array>;
	using InnersType			= Array<boost::geometry::model::ring<Vec2, false, false, Array>>;

	/// @brief 外周頂点と穴の頂点をもとに多角形を三角形分割し、描画します。
	/// @param outer 外周の頂点
	/// @param holes 穴の頂点
	/// @param color 多角形の色
	/// @remark Polygon の作成を経由せずに、三角形分割の結果を描画で使用する関数です。
	void DrawTriangles(std::span<const Vec2> outer, const InnersType& holes, const ColorF& color);

	/// @brief 外周頂点と穴の頂点をもとに多角形を三角形分割し、描画します。
	/// @param outer 外周の頂点
	/// @param holes 穴の頂点
	/// @param pattern 塗りつぶしのパターン
	/// @remark Polygon の作成を経由せずに、三角形分割の結果を描画で使用する関数です。
	void DrawTriangles(std::span<const Vec2> outer, const InnersType& holes, const PatternParameters& pattern);

	namespace detail
	{
		// Boost.Geometry と Siv3D の輪郭は、外周が画面座標で時計回り、穴が反時計回り。
		[[nodiscard]]
		std::span<const Vec2> OpenRingView(std::span<const Vec2> ring) noexcept;

		[[nodiscard]]
		Array<Array<Vec2>> CopyPolygonHoles(const CwOpenPolygon& polygon);

		[[nodiscard]]
		Polygon ToPolygon(const CwOpenPolygon& polygon, SkipValidation skipValidation = SkipValidation::Yes);

		[[nodiscard]]
		CwOpenPolygon ToCwOpenPolygon(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes);

		[[nodiscard]]
		CwOpenPolygon ToCwOpenPolygon(const Polygon& polygon);

		[[nodiscard]]
		PolygonFailureType ValidatePolygon(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes);

		[[nodiscard]]
		PolygonFailureType ValidatePolygon(const CwOpenPolygon& polygon);

		// 修復と検証を行い、三角形化していない有効な輪郭を返す。
		[[nodiscard]]
		CwOpenMultiPolygon CorrectPolygonRings(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes = {});

		[[nodiscard]]
		size_t CalculateCircleQuality(double r) noexcept;
	}
}

BOOST_GEOMETRY_REGISTER_POINT_2D(s3d::Point, s3d::int32, boost::geometry::cs::cartesian, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D(s3d::Float2, float, boost::geometry::cs::cartesian, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D(s3d::Vec2, double, boost::geometry::cs::cartesian, x, y)
BOOST_GEOMETRY_REGISTER_SEGMENT(s3d::Line, s3d::Vec2, start, end)
BOOST_GEOMETRY_REGISTER_LINESTRING(s3d::LineString)
BOOST_GEOMETRY_REGISTER_MULTI_POINT(s3d::Array<s3d::Vec2>)
