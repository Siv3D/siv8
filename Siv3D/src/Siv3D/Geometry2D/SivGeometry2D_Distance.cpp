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
			return boost::geometry::distance(GVec2{ a.x, a.y }, GSegment{ b.start, b.end });
		}

		double Distance(const Vec2& a, const Bezier2& b)
		{
			constexpr double Eps = 1e-12;
			constexpr double DegenerateRel = 1e-24; // for M.dot(M) relative check (squared magnitude)

			const Vec2 M = (b.p0 - 2 * b.p1 + b.p2);
			const Vec2 N = (b.p1 - b.p0);
			const Vec2 K = (a - b.p0);

			const double t1 = M.dot(M);
			const double t2 = (3 * M.dot(N));
			const double t3 = (2 * N.dot(N) - M.dot(K));
			const double t4 = -N.dot(K);

			// 退化: (ほぼ)直線 → 線分 [p0, p2] への距離に落とす
			{
				const Vec2 chord = (b.p2 - b.p0);
				const double scale = Max(1.0, chord.dot(chord)); // length^2 scale
				
				if (t1 <= (DegenerateRel * scale))
				{
					return Line{ b.p0, b.p2 }.distanceFrom(a);
				}
			}

			double minDistSq = Min(a.distanceFromSq(b.p0), a.distanceFromSq(b.p2));
			
			if (const auto opt = Math::SolveCubicEquation(t1, t2, t3, t4))
			{
				for (const double x : *opt)
				{
					if (InRange(x, -Eps, (1.0 + Eps)))
					{
						const double t = Clamp(x, 0.0, 1.0);
						minDistSq = Min(minDistSq, a.distanceFromSq(b.getPos(t)));
					}
				}
			}

			return std::sqrt(minDistSq);
		}
	}
}
