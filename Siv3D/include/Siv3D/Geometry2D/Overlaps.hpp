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
		//	Overlaps(Rect, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Overlaps(const Rect& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Rect& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Rect& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Rect& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Rect& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Rect& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Rect& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Rect& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Rect& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Rect& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(RectF, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Overlaps(const RectF& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RectF& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RectF& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RectF& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RectF& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RectF& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RectF& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RectF& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RectF& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RectF& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(Circle, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Overlaps(const Circle& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Circle& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Circle& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Circle& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Circle& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Circle& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Circle& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Circle& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Circle& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Circle& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(Ellipse, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Overlaps(const Ellipse& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Ellipse& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Ellipse& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Ellipse& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Ellipse& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Ellipse& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Ellipse& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Ellipse& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Ellipse& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Ellipse& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(SuperEllipse, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Overlaps(const SuperEllipse& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const SuperEllipse& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const SuperEllipse& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const SuperEllipse& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const SuperEllipse& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const SuperEllipse& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const SuperEllipse& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const SuperEllipse& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const SuperEllipse& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const SuperEllipse& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(Triangle, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Overlaps(const Triangle& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Triangle& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Triangle& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Triangle& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Triangle& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Triangle& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Triangle& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Triangle& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Triangle& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Triangle& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(Quad, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Overlaps(const Quad& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Quad& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Quad& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Quad& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Quad& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Quad& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Quad& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Quad& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Quad& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Quad& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(RoundRect, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Overlaps(const RoundRect& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RoundRect& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RoundRect& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RoundRect& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RoundRect& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RoundRect& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RoundRect& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RoundRect& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RoundRect& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const RoundRect& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(Polygon, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Overlaps(const Polygon& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Polygon& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Polygon& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Polygon& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Polygon& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Polygon& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Polygon& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Polygon& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Polygon& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const Polygon& a, const MultiPolygon& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(MultiPolygon, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Overlaps(const MultiPolygon& a, const Rect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const MultiPolygon& a, const RectF& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const MultiPolygon& a, const Circle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const MultiPolygon& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const MultiPolygon& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const MultiPolygon& a, const Triangle& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const MultiPolygon& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const MultiPolygon& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const MultiPolygon& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Overlaps(const MultiPolygon& a, const MultiPolygon& b) noexcept;
	}
}
