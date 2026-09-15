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

	[[nodiscard]]
	inline bool PolygonContainsPoint(const PolygonRingsView polygon, const Vec2& point) noexcept
	{
		constexpr PointContainmentOptions Outer{ .boundary = PointContainmentBoundaryPolicy::Included };
		constexpr PointContainmentOptions Hole{ .boundary = PointContainmentBoundaryPolicy::Excluded };
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

	template <class Piece, class Predicate>
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
