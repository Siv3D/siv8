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

# include "PolygonBuffer.hpp"
# include "GeometryCommon.hpp"
# include <Siv3D/LineString.hpp>
# include <algorithm>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		bool IsClosedLineString(const LineString& points)
		{
			return ((1 < points.size()) && (points.front() == points.back()));
		}

		[[nodiscard]]
		size_t GetClosedStrokeBaseSize(const LineString& points)
		{
			if (IsClosedLineString(points))
			{
				return (points.size() - 1);
			}

			return points.size();
		}

		[[nodiscard]]
		bool NeedsClosedStrokeLineString(const LineString& points, const CloseRing closeRing)
		{
			return (closeRing && (2 < GetClosedStrokeBaseSize(points)));
		}

		[[nodiscard]]
		LineString MakeClosedStrokeLineString(const LineString& points)
		{
			const size_t baseSize = GetClosedStrokeBaseSize(points);

			LineString lines;
			lines.reserve(baseSize + 2);

			lines.insert(lines.end(), points.begin(), (points.begin() + baseSize));

			// Boost.Geometry の LineString buffer は開いた折れ線として扱われる。
			// 閉じた折れ線の継ぎ目にも join_* を適用するため、
			// p0, p1 を末尾に追加して pN -> p0 -> p1 を内部頂点化する。
			lines.insert(lines.end(), lines.begin(), (lines.begin() + 2));

			return lines;
		}

		template <class Geometry, class JoinStrategy, class EndStrategy, class PointStrategy>
		void BufferLineString(
			const Geometry& lines,
			CwOpenMultiPolygon& multiPolygon,
			const double distance,
			const JoinStrategy& joinStrategy,
			const EndStrategy& endStrategy,
			const PointStrategy& pointStrategy)
		{
			boost::geometry::buffer(lines, multiPolygon,
				boost::geometry::strategy::buffer::distance_symmetric<double>{ distance },
				boost::geometry::strategy::buffer::side_straight{},
				joinStrategy,
				endStrategy,
				pointStrategy);
		}

		template <class Geometry>
		void BufferLineStringWithMiterJoin(
			const Geometry& lines,
			CwOpenMultiPolygon& multiPolygon,
			const double distance)
		{
			BufferLineString(lines, multiPolygon, distance,
				boost::geometry::strategy::buffer::join_miter{},
				boost::geometry::strategy::buffer::end_flat{},
				boost::geometry::strategy::buffer::point_square{});
		}

		template <class Geometry, class EndStrategy>
		void BufferLineStringWithRoundJoin(
			const Geometry& lines,
			CwOpenMultiPolygon& multiPolygon,
			const double distance,
			const size_t quality,
			const EndStrategy& endStrategy)
		{
			BufferLineString(lines, multiPolygon, distance,
				boost::geometry::strategy::buffer::join_round{ quality },
				endStrategy,
				boost::geometry::strategy::buffer::point_circle{ quality });
		}

		void NormalizeRing(CWOpenRing& ring)
		{
			ring.erase(std::unique(ring.begin(), ring.end()), ring.end());
			ring.resize(detail::OpenRingView(ring).size());
		}

		[[nodiscard]]
		Polygon ToPolygonFromBufferedLineString(CwOpenPolygon& polygon)
		{
			NormalizeRing(polygon.outer());

			if (polygon.outer().size() < 3)
			{
				return{};
			}

			for (auto& inner : polygon.inners())
			{
				NormalizeRing(inner);
			}

			polygon.inners().erase_all_if([](const auto& hole) { return (hole.size() < 3); });
			return detail::ToPolygon(polygon, SkipValidation::No);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ComputeMiterBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon ComputeMiterBufferPolygon(const LineString& vertices, const double distance, const CloseRing closeRing)
	{
		if (vertices.isEmpty() || (distance <= 0.0))
		{
			return{};
		}

		CwOpenMultiPolygon multiPolygon;

		if (NeedsClosedStrokeLineString(vertices, closeRing))
		{
			const LineString lines = MakeClosedStrokeLineString(vertices);

			BufferLineStringWithMiterJoin(lines, multiPolygon, distance);
		}
		else
		{
			BufferLineStringWithMiterJoin(vertices, multiPolygon, distance);
		}

		if (multiPolygon.size() != 1)
		{
			return{};
		}

		return ToPolygonFromBufferedLineString(multiPolygon.front());
	}

	////////////////////////////////////////////////////////////////
	//
	//	ComputeRoundBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon ComputeRoundBufferPolygon(const LineString& vertices, const double distance, const CloseRing closeRing, const QualityFactor& qualityFactor)
	{
		if (vertices.isEmpty() || (distance <= 0.0))
		{
			return{};
		}

		const size_t quality = detail::CalculateCircleQuality(Abs(distance) * qualityFactor.value());

		CwOpenMultiPolygon multiPolygon;

		if (NeedsClosedStrokeLineString(vertices, closeRing))
		{
			const LineString lines = MakeClosedStrokeLineString(vertices);

			BufferLineStringWithRoundJoin(lines, multiPolygon, distance, quality,
				boost::geometry::strategy::buffer::end_flat{});
		}
		else if (IsClosedLineString(vertices))
		{
			BufferLineStringWithRoundJoin(vertices, multiPolygon, distance, quality,
				boost::geometry::strategy::buffer::end_flat{});
		}
		else
		{
			BufferLineStringWithRoundJoin(vertices, multiPolygon, distance, quality,
				boost::geometry::strategy::buffer::end_round{ quality });
		}

		if (multiPolygon.size() != 1)
		{
			return{};
		}

		return ToPolygonFromBufferedLineString(multiPolygon.front());
	}
}
