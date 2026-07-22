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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	RandomUnitVec2
	//
	////////////////////////////////////////////////////////////////

	inline Vec2 RandomUnitVec2()
	{
		return RandomUnitVec2(GetDefaultRNG());
	}

	Vec2 RandomUnitVec2(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		const double theta = RandomClosedOpen(0.0, Math::TwoPi, urbg);
		return{ std::cos(theta), std::sin(theta) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomVec2
	//
	////////////////////////////////////////////////////////////////

	inline Vec2 RandomVec2(const double length)
	{
		return RandomVec2(length, GetDefaultRNG());
	}

	Vec2 RandomVec2(const double length, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return (RandomUnitVec2(urbg) * length);
	}

	inline Vec2 RandomVec2(const std::pair<double, double>& xMinMax, const std::pair<double, double>& yMinMax)
	{
		return RandomVec2(xMinMax, yMinMax, GetDefaultRNG());
	}

	Vec2 RandomVec2(const std::pair<double, double>& xMinMax, const std::pair<double, double>& yMinMax, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{
			RandomClosedOpen(xMinMax.first, xMinMax.second, urbg),
			RandomClosedOpen(yMinMax.first, yMinMax.second, urbg)
		};
	}

	inline Vec2 RandomVec2(const double xMax, const double yMax)
	{
		return RandomVec2(xMax, yMax, GetDefaultRNG());
	}

	Vec2 RandomVec2(const double xMax, const double yMax, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{
			RandomClosedOpen(0.0, xMax, urbg),
			RandomClosedOpen(0.0, yMax, urbg)
		};
	}

	inline Vec2 RandomVec2(const Line& line)
	{
		return RandomVec2(line, GetDefaultRNG());
	}

	Vec2 RandomVec2(const Line& line, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return line.interpolatedPointAt(RandomClosed(0.0, 1.0, urbg));
	}

	inline Vec2 RandomVec2(const Circle& circle)
	{
		return RandomVec2(circle, GetDefaultRNG());
	}

	Vec2 RandomVec2(const Circle& circle, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		const double r = (std::sqrt(Random(urbg)) * circle.r);
		const double theta = RandomClosedOpen(0.0, Math::TwoPi, urbg);
		return circle.center.movedBy((std::cos(theta) * r), (std::sin(theta) * r));
	}

	inline Vec2 RandomVec2(const RectF& rect)
	{
		return RandomVec2(rect, GetDefaultRNG());
	}

	Vec2 RandomVec2(const RectF& rect, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{
			RandomClosedOpen(rect.x, (rect.x + rect.w), urbg),
			RandomClosedOpen(rect.y, (rect.y + rect.h), urbg)
		};
	}

	inline Vec2 RandomVec2(const Triangle& triangle)
	{
		return RandomVec2(triangle, GetDefaultRNG());
	}

	Vec2 RandomVec2(const Triangle& triangle, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		double u = Random(urbg);
		double v = Random(urbg);

		if (1.0 < (u + v))
		{
			u = (1.0 - u);
			v = (1.0 - v);
		}

		const Vec2 v0 = (triangle.p1 - triangle.p0);
		const Vec2 v1 = (triangle.p2 - triangle.p0);
		return (triangle.p0 + u * v0 + v * v1);
	}

	inline Vec2 RandomVec2(const Quad& quad)
	{
		return RandomVec2(quad, GetDefaultRNG());
	}

	Vec2 RandomVec2(const Quad& quad, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		const Triangle t1 = quad.triangleAtIndex(0);
		const Triangle t2 = quad.triangleAtIndex(1);

		const double t1Area = t1.area();
		const double t2Area = t2.area();
		const double totalArea = (t1Area + t2Area);

		if (totalArea == 0.0)
		{
			return quad.p0;
		}

		const bool selectFirst = (RandomClosedOpen(0.0, totalArea, urbg) < t1Area);
		return RandomVec2((selectFirst ? t1 : t2), urbg);
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomVec2InsideUnitCircle
	//
	////////////////////////////////////////////////////////////////

	inline Vec2 RandomVec2InsideUnitCircle()
	{
		return RandomVec2InsideUnitCircle(GetDefaultRNG());
	}

	Vec2 RandomVec2InsideUnitCircle(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		const double r = std::sqrt(Random(urbg));
		const double theta = RandomClosedOpen(0.0, Math::TwoPi, urbg);
		return{
			(std::cos(theta) * r),
			(std::sin(theta) * r)
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomVec2InsideAnnulus
	//
	////////////////////////////////////////////////////////////////

	inline Vec2 RandomVec2InsideAnnulus(const double innerRadius, const double outerRadius)
	{
		return RandomVec2InsideAnnulus(innerRadius, outerRadius, GetDefaultRNG());
	}

	Vec2 RandomVec2InsideAnnulus(const double innerRadius, const double outerRadius, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		const double u = Random(urbg);
		const double innerRadiusSq = (innerRadius * innerRadius);
		const double outerRadiusSq = (outerRadius * outerRadius);
		const double radius = std::sqrt(Math::Lerp(innerRadiusSq, outerRadiusSq, u));
		const double theta = RandomClosedOpen(0.0, Math::TwoPi, urbg);
		return{
			(std::cos(theta) * radius),
			(std::sin(theta) * radius)
		};
	}
}
