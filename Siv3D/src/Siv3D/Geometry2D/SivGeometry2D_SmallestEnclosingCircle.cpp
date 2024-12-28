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
//    Authors (OpenSiv3D challenge #19)
//    - あぷりばーど
//    - Nachia
//    - Luke256
//    - ラクラムシ
//    - polyester
//    - sasa
//-----------------------------------------------

# include <Siv3D/Geometry2D/SmallestEnclosingCircle.hpp>
# include <Siv3D/Random.hpp>

namespace s3d
{
	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	SmallestEnclosingCircle
		//
		////////////////////////////////////////////////////////////////
		
		Circle SmallestEnclosingCircle(const Vec2& p0, const Vec2& p1, const Vec2& p2)
		{
			// 三角形 (p0, p1, p2) に対して鈍角の存在を判定し、もしあればその対辺（最長辺）を弦とする円が最小の円となる。
			if ((p1 - p0).dot(p2 - p0) <= 0.0)
			{
				return Circle{ p1, p2 };
			}

			if ((p0 - p1).dot(p2 - p1) <= 0.0)
			{
				return Circle{ p0, p2 };
			}
			if ((p0 - p2).dot(p1 - p2) <= 0.0)
			{
				return Circle{ p0, p1 };
			}

			// 鋭角三角形の場合は (p0, p1, p2) の外接円が最小となる。
			return Triangle{ p0, p1, p2 }.getCircumscribedCircle();
		}
		
		Circle SmallestEnclosingCircle(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, const double tolerance)
		{
			Circle circle = SmallestEnclosingCircle(p0, p1, p2);

			if (not detail::Contains(circle, p3, tolerance))
			{
				circle = SmallestEnclosingCircle(p0, p1, p3);

				if (not detail::Contains(circle, p2, tolerance))
				{
					circle = SmallestEnclosingCircle(p0, p2, p3);

					if (not detail::Contains(circle, p1, tolerance))
					{
						circle = SmallestEnclosingCircle(p1, p2, p3);
					}
				}
			}

			return circle;
		}
		
		Circle SmallestEnclosingCircle(Array<Vec2> points, const double tolerance)
		{
			return SmallestEnclosingCircle(std::move(points), tolerance, GetDefaultRNG());
		}
	}
}
