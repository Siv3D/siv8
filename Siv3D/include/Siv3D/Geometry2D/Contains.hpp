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
# include "../2DShapes.hpp"
# include "Geometry2DCommon.hpp"

namespace s3d
{
	class Polygon;
	class MultiPolygon;

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Contains(Rect, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Contains(const Rect& a, const Point& b) noexcept;

		[[nodiscard]]
		bool Contains(const Rect& a, const Vec2& b) noexcept;

		[[nodiscard]]
		bool Contains(const Rect& a, const Line& b) noexcept;

		[[nodiscard]]
		bool Contains(const Rect& a, const LineString& b) noexcept;

		[[nodiscard]]
		bool Contains(const Rect& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Contains(const Rect& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Contains(const Rect& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Contains(const Rect& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const Rect& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const Rect& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Contains(const Rect& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Contains(const Rect& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Contains(const Rect& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Contains(const Rect& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Contains(RectF, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Contains(const RectF& a, const Point& b) noexcept;

		[[nodiscard]]
		bool Contains(const RectF& a, const Vec2& b) noexcept;

		[[nodiscard]]
		bool Contains(const RectF& a, const Line& b) noexcept;

		[[nodiscard]]
		bool Contains(const RectF& a, const LineString& b) noexcept;

		[[nodiscard]]
		bool Contains(const RectF& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Contains(const RectF& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Contains(const RectF& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Contains(const RectF& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const RectF& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const RectF& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Contains(const RectF& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Contains(const RectF& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Contains(const RectF& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Contains(const RectF& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Contains(Circle, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Contains(const Circle& a, const Point& b) noexcept;

		[[nodiscard]]
		bool Contains(const Circle& a, const Vec2& b) noexcept;

		[[nodiscard]]
		bool Contains(const Circle& a, const Line& b) noexcept;

		[[nodiscard]]
		bool Contains(const Circle& a, const LineString& b) noexcept;

		[[nodiscard]]
		bool Contains(const Circle& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Contains(const Circle& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Contains(const Circle& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Contains(const Circle& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const Circle& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const Circle& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Contains(const Circle& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Contains(const Circle& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Contains(const Circle& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Contains(const Circle& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Contains(Ellipse, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Contains(const Ellipse& a, const Point& b) noexcept;

		[[nodiscard]]
		bool Contains(const Ellipse& a, const Vec2& b) noexcept;

		[[nodiscard]]
		bool Contains(const Ellipse& a, const Line& b) noexcept;

		[[nodiscard]]
		bool Contains(const Ellipse& a, const LineString& b) noexcept;

		[[nodiscard]]
		bool Contains(const Ellipse& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Contains(const Ellipse& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Contains(const Ellipse& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Contains(const Ellipse& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const Ellipse& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const Ellipse& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Contains(const Ellipse& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Contains(const Ellipse& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Contains(const Ellipse& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Contains(const Ellipse& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Contains(SuperEllipse, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const Point& b) noexcept;

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const Vec2& b) noexcept;

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const Line& b) noexcept;

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const LineString& b) noexcept;

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Contains(const SuperEllipse& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Contains(Triangle, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Contains(const Triangle& a, const Point& b) noexcept;

		[[nodiscard]]
		bool Contains(const Triangle& a, const Vec2& b) noexcept;

		[[nodiscard]]
		bool Contains(const Triangle& a, const Line& b) noexcept;

		[[nodiscard]]
		bool Contains(const Triangle& a, const LineString& b) noexcept;

		[[nodiscard]]
		bool Contains(const Triangle& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Contains(const Triangle& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Contains(const Triangle& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Contains(const Triangle& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const Triangle& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const Triangle& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Contains(const Triangle& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Contains(const Triangle& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Contains(const Triangle& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Contains(const Triangle& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Contains(Quad, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Contains(const Quad& a, const Point& b) noexcept;

		[[nodiscard]]
		bool Contains(const Quad& a, const Vec2& b) noexcept;

		[[nodiscard]]
		bool Contains(const Quad& a, const Line& b) noexcept;

		[[nodiscard]]
		bool Contains(const Quad& a, const LineString& b) noexcept;

		[[nodiscard]]
		bool Contains(const Quad& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Contains(const Quad& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Contains(const Quad& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Contains(const Quad& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const Quad& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const Quad& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Contains(const Quad& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Contains(const Quad& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Contains(const Quad& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Contains(const Quad& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Contains(RoundRect, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Contains(const RoundRect& a, const Point& b) noexcept;

		[[nodiscard]]
		bool Contains(const RoundRect& a, const Vec2& b) noexcept;

		[[nodiscard]]
		bool Contains(const RoundRect& a, const Line& b) noexcept;

		[[nodiscard]]
		bool Contains(const RoundRect& a, const LineString& b) noexcept;

		[[nodiscard]]
		bool Contains(const RoundRect& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Contains(const RoundRect& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Contains(const RoundRect& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Contains(const RoundRect& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const RoundRect& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const RoundRect& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Contains(const RoundRect& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Contains(const RoundRect& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Contains(const RoundRect& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Contains(const RoundRect& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Contains(Polygon, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Contains(const Polygon& a, const Point& b) noexcept;

		[[nodiscard]]
		bool Contains(const Polygon& a, const Vec2& b) noexcept;

		[[nodiscard]]
		bool Contains(const Polygon& a, const Line& b) noexcept;

		[[nodiscard]]
		bool Contains(const Polygon& a, const LineString& b) noexcept;

		[[nodiscard]]
		bool Contains(const Polygon& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Contains(const Polygon& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Contains(const Polygon& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Contains(const Polygon& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const Polygon& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const Polygon& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Contains(const Polygon& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Contains(const Polygon& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Contains(const Polygon& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Contains(const Polygon& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Contains(MultiPolygon, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const Point& b) noexcept;

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const Vec2& b) noexcept;

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const Line& b) noexcept;

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const LineString& b) noexcept;

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Contains(const MultiPolygon& a, const MultiPolygon& b) noexcept;
	}
}
