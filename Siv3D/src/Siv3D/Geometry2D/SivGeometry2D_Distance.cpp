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

# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Geometry2D/Distance.hpp>
# include <Siv3D/Polygon/GeometryCommon.hpp>

namespace s3d
{
	namespace Geometry2D
	{
		//////////////////////////////////////////////////
		//
		//	Distance
		//
		//////////////////////////////////////////////////

		double Distance(const Vec2& a, const Line& b)
		{
			return boost::geometry::distance(GVec2{ a.x, a.y }, GSegment{ b.start, b.end });
		}
	}
}
