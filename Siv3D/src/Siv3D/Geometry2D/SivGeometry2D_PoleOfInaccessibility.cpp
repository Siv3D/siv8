//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Geometry2D/PoleOfInaccessibility.hpp>
# include <ThirdParty/mapbox/polylabel.hpp>

namespace s3d
{
	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	PoleOfInaccessibility
		//
		////////////////////////////////////////////////////////////////

		Vec2 PoleOfInaccessibility(std::span<const Vec2> points, const double precision)
		{
			std::vector<mapbox::geometry::point<double>> mpoints(points.size());

			for (size_t i = 0; i < points.size(); ++i)
			{
				mpoints[i] = { points[i].x, points[i].y };
			}

			const mapbox::geometry::polygon<double> polygon{ std::move(mpoints) };

			const mapbox::geometry::point<double> pia = mapbox::polylabel(polygon, precision);

			return{ pia.x, pia.y };
		}
	}
}
