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
//	Authors (OpenSiv3D challenge participants)
//	- Ebishu
//	- fal_rnd
//	- きつねび
//-----------------------------------------------

# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Bezier.hpp>
# include <Siv3D/PolynomialSolver.hpp>
# include <Siv3D/Geometry2D/Distance.hpp>
# include <Siv3D/Polygon/GeometryCommon.hpp>

namespace s3d
{
	namespace Geometry2D
	{
		//////////////////////////////////////////////////
		//
		//	Distance(Vec2, _)
		//
		//////////////////////////////////////////////////

		double Distance(const Vec2& a, const Line& b)
		{
			return b.distanceFrom(a);
		}

		double Distance(const Vec2& a, const Bezier2& b)
		{
			return a.distanceFrom(b.closestPoint(a));
		}

		double Distance(const Vec2& a, const Bezier3& b)
		{
			return a.distanceFrom(b.closestPoint(a));
		}
	}
}
