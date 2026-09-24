//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# pragma once
# include <variant>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/MultiPolygon.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/Geometry2D/PointContainment.hpp>

namespace s3d::detail
{
	struct PolygonRingsView
	{
		std::span<const Vec2> outer;
		std::span<const Array<Vec2>> holes;
	};

	[[nodiscard]]
	inline PolygonRingsView GetPolygonRings(const Polygon& polygon) noexcept
	{
		return { polygon.outer(), polygon.inners() };
	}

	// 入力順に辺を参照し、predicate が true を返した時点で終了する。
	// ゼロ長辺も渡す。呼び出し側で点として扱うか、除外するかを選ぶ。
	template <bool CloseRing, class Predicate>
	[[nodiscard]]
	bool AnyPolylineSegment(const std::span<const Vec2> points, Predicate&& predicate)
	{
		if (points.size() < 2)
		{
			return false;
		}

		for (size_t i = 0; (i + 1) < points.size(); ++i)
		{
			if (predicate(Line{ points[i], points[i + 1] }))
			{
				return true;
			}
		}

		if constexpr (CloseRing)
		{
			return predicate(Line{ points.back(), points.front() });
		}
		else
		{
			return false;
		}
	}

	template <class Predicate>
	[[nodiscard]]
	bool AnyPolygonEdge(const PolygonRingsView polygon, Predicate&& predicate)
	{
		if (AnyPolylineSegment<true>(polygon.outer, predicate))
		{
			return true;
		}

		for (const auto& hole : polygon.holes)
		{
			if (AnyPolylineSegment<true>(hole, predicate))
			{
				return true;
			}
		}

		return false;
	}

	template <class Predicate>
	[[nodiscard]]
	bool AnyPolygonEdge(const Polygon& polygon, Predicate&& predicate)
	{
		return AnyPolygonEdge(GetPolygonRings(polygon), std::forward<Predicate>(predicate));
	}

	template <bool IncludeBoundary = true>
	[[nodiscard]]
	inline bool PolygonContainsPoint(const PolygonRingsView polygon, const Vec2& point) noexcept
	{
		constexpr PointContainmentOptions Outer{ .boundary = IncludeBoundary
			? PointContainmentBoundaryPolicy::Included : PointContainmentBoundaryPolicy::Excluded };
		constexpr PointContainmentOptions Hole{ .boundary = IncludeBoundary
			? PointContainmentBoundaryPolicy::Excluded : PointContainmentBoundaryPolicy::Included };
		if (not Geometry2D::ContainsPoint<Outer>(polygon.outer, point))
		{
			return false;
		}
		for (const auto& hole : polygon.holes)
		{
			if (Geometry2D::ContainsPoint<Hole>(std::span<const Vec2>{ hole }, point))
			{
				return false;
			}
		}
		return true;
	}

	[[nodiscard]]
	inline int32 PolygonRingOrientation(const std::span<const Vec2> ring, const RectF* bounds = nullptr) noexcept
	{
		if (ring.size() < 3)
		{
			return 0;
		}

		auto CornerOrientation = [&](const size_t first)
		{
			size_t previous = ((first + ring.size() - 1) % ring.size());
			size_t next = ((first + 1) % ring.size());
			while ((previous != first) && (ring[previous] == ring[first]))
			{
				previous = ((previous + ring.size() - 1) % ring.size());
			}
			while ((next != first) && (ring[next] == ring[first]))
			{
				next = ((next + 1) % ring.size());
			}
			const Vec2 a = (ring[first] - ring[previous]);
			const Vec2 b = (ring[next] - ring[first]);
			// Compare the products without a fused subtraction: opposite collinear
			// vectors must not acquire a nonzero determinant from asymmetric rounding.
			const double lhs = (a.x * b.y);
			const double rhs = (a.y * b.x);
			return ((rhs < lhs) - (lhs < rhs));
		};

		// A non-flat corner on a supporting bounds edge is convex.
		if (bounds && ((ring.front().x == bounds->x) || (ring.front().y == bounds->y)
			|| (ring.front().x == (bounds->x + bounds->w)) || (ring.front().y == (bounds->y + bounds->h))))
		{
			if (const int32 direction = CornerOrientation(0))
			{
				return direction;
			}
		}

		size_t first = 0;
		for (size_t i = 1; i < ring.size(); ++i)
		{
			if ((ring[i].x < ring[first].x)
				|| ((ring[i].x == ring[first].x) && (ring[i].y < ring[first].y)))
			{
				first = i;
			}
		}
		return CornerOrientation(first);
	}

	struct PolygonSegmentEvent
	{
		double parameter;
		int32 boundaryDelta;
		bool crossing;
	};

	enum class PolygonSegmentTest { Covered, InteriorIntersection, AreaOverlap };

	[[nodiscard]]
	inline double PolygonCross(const Vec2& a, const Vec2& b) noexcept
	{
		// Round both products before subtraction to preserve identical-edge contacts.
		const double lhs = (a.x * b.y);
		const double rhs = (a.y * b.x);
		return (lhs - rhs);
	}

	[[nodiscard]]
	inline bool PolygonRingHasArea(const std::span<const Vec2> ring, const RectF& bounds) noexcept
	{
		// Any non-collinear corner establishes area; winding is not needed here.
		return (3 <= ring.size())
			&& ((PolygonCross((ring.front() - ring.back()), (ring[1] - ring.front())) != 0.0)
				|| (PolygonRingOrientation(ring, &bounds) != 0));
	}

	// Sweep along the segment. Crossings toggle the interior parity; collinear
	// edges cover closed boundary intervals. Equal vertex events are grouped
	// exactly, so small gaps and holes are never merged by a tolerance.
	template <PolygonSegmentTest Test, class EventContainer>
	[[nodiscard]]
	bool TestPolygonSegment(const PolygonRingsView polygon, const Line& segment,
		EventContainer& events, const int32 requiredDirection = 0)
	{
		events.clear();
		if (segment.start == segment.end)
		{
			if constexpr (Test == PolygonSegmentTest::Covered)
			{
				return PolygonContainsPoint(polygon, segment.start);
			}
			return false;
		}

		const Vec2 direction = (segment.end - segment.start);
		const bool useX = (Abs(direction.y) <= Abs(direction.x));
		const double axisDirection = (useX ? direction.x : direction.y);
		bool inside = false;
		int32 boundaryCount = 0;

		const bool boundaryMatch = AnyPolygonEdge(polygon, [&](const Line& edge)
			{
				const Vec2 a = (edge.start - segment.start);
				const Vec2 b = (edge.end - segment.start);
				const double ca = PolygonCross(direction, a);
				const double cb = PolygonCross(direction, b);
				const double ax = (useX ? a.x : a.y);
				const double bx = (useX ? b.x : b.y);

				if ((ca == 0.0) && (cb == 0.0))
				{
					const double ta = (ax / axisDirection);
					const double tb = (bx / axisDirection);
					const double start = Max(0.0, Min(ta, tb));
					const double end = Min(1.0, Max(ta, tb));
					if (start < end)
					{
						// Coincident boundaries overlap in area only when their filled sides agree.
						if constexpr (Test == PolygonSegmentTest::AreaOverlap)
						{
							if ((ta < tb) == (0 < requiredDirection))
							{
								return true;
							}
						}
						else if ((requiredDirection != 0) && ((ta < tb) != (0 < requiredDirection)))
						{
							return true;
						}
						if (start == 0.0)
						{
							++boundaryCount;
						}
						else
						{
							events.push_back({ start, 1, false });
						}
						if (end < 1.0)
						{
							events.push_back({ end, -1, false });
						}
					}
				}
				else if ((0.0 < ca) != (0.0 < cb))
				{
					// Project a shared vertex directly so both incident edges use
					// the same parameter, including tangent and collinear contacts.
					const double t = ((ca == 0.0) ? (ax / axisDirection)
						: ((cb == 0.0) ? (bx / axisDirection)
							: (PolygonCross(a, b) / (cb - ca))));
					if (t <= 0.0)
					{
						inside = not inside;
					}
					else if (t < 1.0)
					{
						if constexpr (Test == PolygonSegmentTest::AreaOverlap)
						{
							if ((ca != 0.0) && (cb != 0.0))
							{
								return true;
							}
						}
						events.push_back({ t, 0, true });
					}
				}
				return false;
			});
		if (boundaryMatch)
		{
			return (Test == PolygonSegmentTest::AreaOverlap);
		}

		auto Matches = [&]()
		{
			if constexpr (Test == PolygonSegmentTest::Covered)
			{
				return (inside || (0 < boundaryCount));
			}
			else
			{
				return (inside && (boundaryCount == 0));
			}
		};
		std::sort(events.begin(), events.end(), [](const auto& a, const auto& b)
			{ return (a.parameter < b.parameter); });

		for (size_t i = 0; i < events.size();)
		{
			if constexpr (Test == PolygonSegmentTest::Covered)
			{
				if (not Matches())
				{
					return false;
				}
			}
			else if (Matches())
			{
				return true;
			}
			const double t = events[i].parameter;
			do
			{
				inside ^= events[i].crossing;
				boundaryCount += events[i].boundaryDelta;
				++i;
			} while ((i < events.size()) && (events[i].parameter == t));
		}
		return Matches();
	}

	[[nodiscard]]
	inline bool TryGetPolygonPoint(const Polygon& polygon, Vec2& point) noexcept
	{
		const RectF& bounds = polygon.boundingRect();
		if ((bounds.w == 0.0) && (bounds.h == 0.0) && (not polygon.isEmpty()))
		{
			point = polygon.outer().front();
			return true;
		}
		return false;
	}

	template <class Piece>
	using BoundarySource = std::variant<Array<Piece>, std::span<const Vec2>, std::span<const Polygon>>;

	template <class Piece, class Shape, class Append>
	[[nodiscard]]
	BoundarySource<Piece> MakeBoundarySource(const Shape& shape, Append&& append)
	{
		if constexpr (std::is_same_v<Shape, LineString>)
		{
			return std::span<const Vec2>{ shape };
		}
		else if constexpr (std::is_same_v<Shape, Polygon>)
		{
			return std::span<const Polygon>{ &shape, 1 };
		}
		else if constexpr (std::is_same_v<Shape, MultiPolygon>)
		{
			return std::span<const Polygon>{ shape.data(), shape.size() };
		}
		else
		{
			Array<Piece> pieces;
			append(pieces);
			return pieces;
		}
	}

	template <bool IncludePointPolygons = false, class Piece, class Predicate>
	[[nodiscard]]
	bool AnyBoundaryPiece(const BoundarySource<Piece>& source, Predicate&& predicate)
	{
		auto TestEdge = [&](const Line& edge)
		{
			return (edge.start != edge.end) && predicate(Piece{ edge });
		};
		return std::visit([&](const auto& pieces)
			{
				using Source = std::decay_t<decltype(pieces)>;
				if constexpr (std::is_same_v<Source, std::span<const Vec2>>)
				{
					return AnyPolylineSegment<false>(pieces, TestEdge);
				}
				else if constexpr (std::is_same_v<Source, std::span<const Polygon>>)
				{
					for (const auto& polygon : pieces)
					{
						if constexpr (IncludePointPolygons)
						{
							// A zero-scale polygon retains one point, regardless of its vertex count.
							Vec2 point;
							if (TryGetPolygonPoint(polygon, point))
							{
								if (predicate(Piece{ Line{ point, point } }))
								{
									return true;
								}
								continue;
							}
						}
						if (AnyPolygonEdge(polygon, TestEdge))
						{
							return true;
						}
					}
				}
				else
				{
					for (const Piece& piece : pieces)
					{
						if (predicate(piece))
						{
							return true;
						}
					}
				}
				return false;
			}, source);
	}
}
