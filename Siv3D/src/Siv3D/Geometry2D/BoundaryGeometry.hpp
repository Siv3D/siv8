//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# pragma once
# include <variant>
# include <boost/container/static_vector.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Bezier.hpp>
# include <Siv3D/Geometry2D/Geometry2DCommon.hpp>
# include "PolygonGeometry.hpp"

namespace s3d::detail
{
	// Local implementation types let each consumer specialize its variant dispatch.
	namespace
	{
		enum class ArcRegion : uint8
		{
			Full,
			TopLeft,
			TopRight,
			BottomRight,
			BottomLeft,
		};

		struct CircleArc
		{
			Circle circle;
			ArcRegion region = ArcRegion::Full;
		};

		[[nodiscard]]
		inline Line TriangleDegenerateExtent(const Triangle& triangle) noexcept
		{
			const double d01 = triangle.p0.distanceFromSq(triangle.p1);
			const double d12 = triangle.p1.distanceFromSq(triangle.p2);
			const double d20 = triangle.p2.distanceFromSq(triangle.p0);

			if ((d12 <= d01) && (d20 <= d01))
			{
				return Line{ triangle.p0, triangle.p1 };
			}

			if (d20 <= d12)
			{
				return Line{ triangle.p1, triangle.p2 };
			}

			return Line{ triangle.p2, triangle.p0 };
		}

		template <class Visitor>
		void VisitLineBoundaryPiece(Visitor& visitor, const Line& line)
		{
			if (line.start != line.end)
			{
				visitor(line);
			}
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Line& shape)
		{
			VisitLineBoundaryPiece(visitor, shape);
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Bezier2& shape)
		{
			visitor(shape);
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Bezier3& shape)
		{
			visitor(shape);
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const RectF& shape)
		{
			const auto kind = ClassifyGeometry2DSizedShape(shape);

			if (kind == Geometry2DSizedShapeKind::Empty)
			{
				return;
			}

			if (IsGeometry2DSegment(kind))
			{
				VisitLineBoundaryPiece(visitor, GetGeometry2DDegenerateSegment(shape, kind));
				return;
			}

			const double left = shape.pos.x;
			const double top = shape.pos.y;
			const double right = (left + shape.size.x);
			const double bottom = (top + shape.size.y);
			const Vec2 tl{ left, top };
			const Vec2 tr{ right, top };
			const Vec2 br{ right, bottom };
			const Vec2 bl{ left, bottom };
			VisitLineBoundaryPiece(visitor, Line{ tl, tr });
			VisitLineBoundaryPiece(visitor, Line{ tr, br });
			VisitLineBoundaryPiece(visitor, Line{ br, bl });
			VisitLineBoundaryPiece(visitor, Line{ bl, tl });
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Rect& shape)
		{
			VisitBoundaryPieces(visitor, RectF{ shape });
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Circle& shape)
		{
			if (ClassifyGeometry2DSizedShape(shape) == Geometry2DSizedShapeKind::Area)
			{
				visitor(CircleArc{ shape, ArcRegion::Full });
			}
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Ellipse& shape)
		{
			const auto kind = ClassifyGeometry2DSizedShape(shape);

			if (IsGeometry2DSegment(kind))
			{
				VisitLineBoundaryPiece(visitor, GetGeometry2DDegenerateSegment(shape, kind));
			}
			else if (kind == Geometry2DSizedShapeKind::Area)
			{
				visitor(shape);
			}
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const SuperEllipse& shape)
		{
			const auto kind = ClassifyGeometry2DSizedShape(shape);

			if (IsGeometry2DSegment(kind))
			{
				VisitLineBoundaryPiece(visitor, GetGeometry2DDegenerateSegment(shape, kind));
			}
			else if (kind == Geometry2DSizedShapeKind::Area)
			{
				visitor(shape);
			}
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Triangle& shape)
		{
			if ((shape.p1 - shape.p0).cross(shape.p2 - shape.p0) == 0.0)
			{
				VisitLineBoundaryPiece(visitor, TriangleDegenerateExtent(shape));
				return;
			}

			VisitLineBoundaryPiece(visitor, Line{ shape.p0, shape.p1 });
			VisitLineBoundaryPiece(visitor, Line{ shape.p1, shape.p2 });
			VisitLineBoundaryPiece(visitor, Line{ shape.p2, shape.p0 });
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Quad& shape)
		{
			const double twiceArea = (shape.p0.cross(shape.p1)
				+ shape.p1.cross(shape.p2)
				+ shape.p2.cross(shape.p3)
				+ shape.p3.cross(shape.p0));

			if (twiceArea != 0.0)
			{
				VisitLineBoundaryPiece(visitor, Line{ shape.p0, shape.p1 });
				VisitLineBoundaryPiece(visitor, Line{ shape.p1, shape.p2 });
				VisitLineBoundaryPiece(visitor, Line{ shape.p2, shape.p3 });
				VisitLineBoundaryPiece(visitor, Line{ shape.p3, shape.p0 });
				return;
			}

			if ((shape.p1 == shape.p2) && (shape.p3 == shape.p0))
			{
				VisitLineBoundaryPiece(visitor, Line{ shape.p0, shape.p1 });
				return;
			}

			if ((shape.p0 == shape.p1) && (shape.p2 == shape.p3))
			{
				VisitLineBoundaryPiece(visitor, Line{ shape.p0, shape.p2 });
				return;
			}

			if (shape.p2 == shape.p3)
			{
				VisitBoundaryPieces(visitor, Triangle{ shape.p0, shape.p1, shape.p2 });
				return;
			}

			if (shape.p1 == shape.p2)
			{
				VisitBoundaryPieces(visitor, Triangle{ shape.p0, shape.p1, shape.p3 });
				return;
			}

			if (shape.p0 == shape.p1)
			{
				VisitBoundaryPieces(visitor, Triangle{ shape.p0, shape.p2, shape.p3 });
				return;
			}

			if (shape.p3 == shape.p0)
			{
				VisitBoundaryPieces(visitor, Triangle{ shape.p0, shape.p1, shape.p2 });
				return;
			}

			VisitLineBoundaryPiece(visitor, Line{ shape.p0, shape.p1 });
			VisitLineBoundaryPiece(visitor, Line{ shape.p1, shape.p2 });
			VisitLineBoundaryPiece(visitor, Line{ shape.p2, shape.p3 });
			VisitLineBoundaryPiece(visitor, Line{ shape.p3, shape.p0 });
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const RoundRect& shape)
		{
			const auto kind = ClassifyGeometry2DSizedShape(shape);

			if (kind == Geometry2DSizedShapeKind::Empty)
			{
				return;
			}

			if (IsGeometry2DSegment(kind))
			{
				VisitLineBoundaryPiece(visitor, GetGeometry2DDegenerateSegment(shape, kind));
				return;
			}

			const double r = GetGeometry2DEffectiveRadius(shape);

			if (r == 0.0)
			{
				VisitBoundaryPieces(visitor, shape.rect);
				return;
			}

			const double left = shape.rect.pos.x;
			const double top = shape.rect.pos.y;
			const double right = (left + shape.rect.size.x);
			const double bottom = (top + shape.rect.size.y);
			VisitLineBoundaryPiece(visitor, Line{ Vec2{ left + r, top }, Vec2{ right - r, top } });
			VisitLineBoundaryPiece(visitor, Line{ Vec2{ right, top + r }, Vec2{ right, bottom - r } });
			VisitLineBoundaryPiece(visitor, Line{ Vec2{ right - r, bottom }, Vec2{ left + r, bottom } });
			VisitLineBoundaryPiece(visitor, Line{ Vec2{ left, bottom - r }, Vec2{ left, top + r } });
			visitor(CircleArc{ Circle{ Vec2{ left + r, top + r }, r }, ArcRegion::TopLeft });
			visitor(CircleArc{ Circle{ Vec2{ right - r, top + r }, r }, ArcRegion::TopRight });
			visitor(CircleArc{ Circle{ Vec2{ right - r, bottom - r }, r }, ArcRegion::BottomRight });
			visitor(CircleArc{ Circle{ Vec2{ left + r, bottom - r }, r }, ArcRegion::BottomLeft });
		}

		template <bool IncludePointPolygons, class Predicate>
		[[nodiscard]]
		bool AnyPolygonBoundaryPiece(const Polygon& polygon, Predicate&& predicate)
		{
			if constexpr (IncludePointPolygons)
			{
				// A zero-scale polygon retains one point, regardless of its vertex count.
				Vec2 point;
				if (TryGetPolygonPoint(polygon, point))
				{
					return predicate(Line{ point, point });
				}
			}
			return AnyPolygonEdge(polygon, [&](const Line& edge)
				{
					return (edge.start != edge.end) && predicate(edge);
				});
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Polygon& shape)
		{
			(void)AnyPolygonBoundaryPiece<true>(shape, [&](const Line& edge)
				{
					visitor(edge);
					return false;
				});
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const MultiPolygon& shape)
		{
			for (const Polygon& polygon : shape)
			{
				VisitBoundaryPieces(visitor, polygon);
			}
		}

		// Point geometries are handled before boundary generation.
		template <class Visitor>
		void VisitBoundaryPieces(Visitor&, const Point&) {}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor&, const Vec2&) {}

		// RoundRect needs at most four segments and four arcs; variable-size shapes are borrowed.
		template <class Piece>
		using BoundaryPieceBuffer = boost::container::static_vector<Piece, 8>;

		template <class Piece>
		using BoundarySource = std::variant<BoundaryPieceBuffer<Piece>, std::span<const Vec2>, std::span<const Polygon>>;

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
				BoundaryPieceBuffer<Piece> pieces;
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
							if (AnyPolygonBoundaryPiece<IncludePointPolygons>(polygon,
								[&](const Line& edge) { return predicate(Piece{ edge }); }))
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
}
