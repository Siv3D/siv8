
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
# include "../Optional.hpp"
# include "../2DShapes.hpp"

namespace s3d
{
	namespace Geometry2D
	{
		template <class Shape, class PointType>
		Optional<Vec2> ClosestPointOnBoundary(const Shape&, const PointType&) = delete;

		template <class Shape, class PointType>
		double SignedDistance(const Shape&, const PointType&) = delete;

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Rect, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Rect& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const Rect& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Rect& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const Rect& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(RectF, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const RectF& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const RectF& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const RectF& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const RectF& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Circle, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Circle& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const Circle& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Circle& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const Circle& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Ellipse, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Ellipse& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const Ellipse& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Ellipse& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const Ellipse& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(SuperEllipse, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const SuperEllipse& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const SuperEllipse& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const SuperEllipse& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const SuperEllipse& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Triangle, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Triangle& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const Triangle& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Triangle& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const Triangle& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Quad, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Quad& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const Quad& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Quad& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const Quad& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(RoundRect, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const RoundRect& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const RoundRect& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const RoundRect& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const RoundRect& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Polygon, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Polygon& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const Polygon& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Polygon& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const Polygon& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(MultiPolygon, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const MultiPolygon& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const MultiPolygon& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const MultiPolygon& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const MultiPolygon& shape, const Vec2& point);
	}
}
