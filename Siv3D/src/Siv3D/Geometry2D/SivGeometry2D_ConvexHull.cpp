//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Geometry2D/ConvexHull.hpp>
# include <Siv3D/Polygon/GeometryCommon.hpp>

namespace s3d
{
	namespace
	{
		template <class PointType>
		static Polygon ConvexHull_impl(const std::span<const PointType> points)
		{
			if (points.size() < 3)
			{
				return{};
			}

			CWOpenRing result;

			boost::geometry::convex_hull(boost::geometry::model::multi_point<PointType>(points.begin(), points.end()), result);

			return Polygon{ result };
		}
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	ConvexHull
		//
		////////////////////////////////////////////////////////////////
		
		Polygon ConvexHull(const std::span<const Point> points)
		{
			return ConvexHull_impl(points);
		}

		Polygon ConvexHull(const std::span<const Float2> points)
		{
			return ConvexHull_impl(points);
		}

		Polygon ConvexHull(const std::span<const Vec2> points)
		{
			return ConvexHull_impl(points);
		}
	}
}
