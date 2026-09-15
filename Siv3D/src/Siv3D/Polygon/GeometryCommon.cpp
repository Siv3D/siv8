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

# include "GeometryCommon.hpp"

namespace s3d
{
	namespace detail
	{
		std::span<const Vec2> OpenRingView(std::span<const Vec2> ring) noexcept
		{
			if ((2 < ring.size()) && (ring.front() == ring.back()))
			{
				ring = ring.first(ring.size() - 1);
			}

			return ring;
		}

		Array<Array<Vec2>> CopyPolygonHoles(const CwOpenPolygon& polygon)
		{
			Array<Array<Vec2>> holes{ Arg::reserve = polygon.inners().size() };

			for (const auto& inner : polygon.inners())
			{
				const auto ring = OpenRingView(inner);
				holes.emplace_back(ring.begin(), ring.end());
			}

			return holes;
		}

		Polygon ToPolygon(const CwOpenPolygon& polygon, const SkipValidation skipValidation)
		{
			return Polygon{ OpenRingView(polygon.outer()), CopyPolygonHoles(polygon), skipValidation };
		}

		CwOpenPolygon ToCwOpenPolygon(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes)
		{
			CwOpenPolygon polygon;
			polygon.outer().assign_range(outer);
			polygon.inners().reserve(holes.size());

			for (const auto& hole : holes)
			{
				polygon.inners().emplace_back(hole.begin(), hole.end());
			}

			return polygon;
		}

		CwOpenPolygon ToCwOpenPolygon(const Polygon& polygon)
		{
			return ToCwOpenPolygon(polygon.outer(), polygon.inners());
		}

		size_t CalculateCircleQuality(const double r) noexcept
		{
			if (r <= 2.0)
			{
				return 6;
			}
			else if (r <= 4.0)
			{
				return 10;
			}
			else if (r < 8.0)
			{
				return 12;
			}
			else
			{
				return Min((12 + static_cast<int32>((r - 8.0) / 2.0)), 256);
			}
		}
	}
}
