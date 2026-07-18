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

namespace s3d
{
	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Intersects / IntersectsAt(Point, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr bool Intersects(const Point& a, const Point& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Point& a, const Vec2& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Point& a, const Line& b) noexcept;

		[[nodiscard]]
		bool Intersects(const Point& a, const LineString& b) noexcept;

		[[nodiscard]]
		bool Intersects(const Point& a, const Bezier2& b);

		[[nodiscard]]
		bool Intersects(const Point& a, const Bezier3& b);

		[[nodiscard]]
		constexpr bool Intersects(const Point& a, const Rect& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Point& a, const RectF& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Point& a, const Circle& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Point& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Intersects(const Point& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Point& a, const Triangle& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Point& a, const Quad& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Point& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Intersects(const Point& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Intersects(const Point& a, const MultiPolygon& b) noexcept;

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
		//	Intersects / IntersectsAt(Vec2, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr bool Intersects(const Vec2& a, const Point& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Vec2& a, const Vec2& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Vec2& a, const Line& b) noexcept;

		[[nodiscard]]
		bool Intersects(const Vec2& a, const LineString& b) noexcept;

		[[nodiscard]]
		bool Intersects(const Vec2& a, const Bezier2& b);

		[[nodiscard]]
		bool Intersects(const Vec2& a, const Bezier3& b);

		[[nodiscard]]
		constexpr bool Intersects(const Vec2& a, const Rect& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Vec2& a, const RectF& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Vec2& a, const Circle& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Vec2& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		bool Intersects(const Vec2& a, const SuperEllipse& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Vec2& a, const Triangle& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Vec2& a, const Quad& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersects(const Vec2& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Intersects(const Vec2& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Intersects(const Vec2& a, const MultiPolygon& b) noexcept;

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
	}
}
