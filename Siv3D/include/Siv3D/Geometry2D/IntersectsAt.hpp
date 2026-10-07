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
# include "../Common.hpp"
# include "../Array.hpp"
# include "../Optional.hpp"
# include "../PointVector.hpp"
# include "../2DShapes.hpp"

namespace s3d
{
	namespace Geometry2D
	{
		/// @defgroup geometry2d_intersection_points 交点配列の取得
		/// @ingroup geometry2d_queries
		/// @brief IntersectsAt() と各図形の intersectsAt() の返り値。
		///
		/// | 返り値 | 意味 |
		/// |---|---|
		/// | none | Intersects() が false と判定した。 |
		/// | 空配列 | Intersects() は true だが、列挙できた孤立点はない。 |
		/// | 空でない配列 | 列挙できた孤立点の座標。 |
		///
		/// 図形同士の共有部分の孤立点、および境界同士の孤立した交点を返します。
		/// 点・線分・曲線では、その点集合全体を境界として扱います。共有線分・共有曲線とその端点は列挙しません。
		/// 例えば、長方形の内部にある点との交差はその点を返し、内部に収まる長さのある線分との交差は空配列を返します。
		/// 境界を横切る線分では境界上の交点を返します。返す座標の順序は未規定で、近接した交点を一つにまとめる場合があります。
		///
		/// Bezier 曲線同士、および n が 1, 2 以外の SuperEllipse と曲線との交点は、処理量に上限を設けた近似計算です。
		/// 交点を取りこぼす場合があるため、空配列は数学的な孤立点が存在しないことを保証しません。
		/// @see @ref geometry2d_queries
		/// @{

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Point, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Point& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Vec2, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Line, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Line& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(LineString, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Bezier2, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Bezier3, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Rect, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(RectF, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Circle, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Ellipse, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(SuperEllipse, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Triangle, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Quad, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(RoundRect, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Polygon, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const MultiPolygon& b);

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(MultiPolygon, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Point& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Vec2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Line& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const LineString& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Bezier2& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Bezier3& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Rect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const RectF& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Circle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Ellipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const SuperEllipse& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Triangle& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Quad& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const RoundRect& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Polygon& b);

		[[nodiscard]]
		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const MultiPolygon& b);

		/// @}
	}
}
