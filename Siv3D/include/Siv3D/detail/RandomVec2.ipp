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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	RandomVec2
	//
	////////////////////////////////////////////////////////////////

	inline Vec2 RandomVec2()
	{
		return RandomVec2(GetDefaultRNG());
	}

	Vec2 RandomVec2(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		const double theta = RandomClosedOpen(0.0, Math::TwoPi, std::forward<decltype(urbg)>(urbg));
		
		return{ std::cos(theta), std::sin(theta) };
	}

	inline Vec2 RandomVec2(const double length)
	{
		return RandomVec2(length, GetDefaultRNG());
	}

	Vec2 RandomVec2(const double length, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return (RandomVec2(std::forward<decltype(urbg)>(urbg)) * length);
	}

	inline Vec2 RandomVec2(const double x, const std::pair<double, double>& yMinMax)
	{
		return RandomVec2(x, yMinMax, GetDefaultRNG());
	}

	Vec2 RandomVec2(const double x, const std::pair<double, double>& yMinMax, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{ x, Random(yMinMax.first, yMinMax.second, std::forward<decltype(urbg)>(urbg)) };
	}

	inline Vec2 RandomVec2(const std::pair<double, double>& xMinMax, const double y)
	{
		return RandomVec2(xMinMax, y, GetDefaultRNG());
	}

	Vec2 RandomVec2(const std::pair<double, double>& xMinMax, const double y, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{ Random(xMinMax.first, xMinMax.second, std::forward<decltype(urbg)>(urbg)), y };
	}

	inline Vec2 RandomVec2(const std::pair<double, double>& xMinMax, const std::pair<double, double>& yMinMax)
	{
		return RandomVec2(xMinMax, yMinMax, GetDefaultRNG());
	}

	Vec2 RandomVec2(const std::pair<double, double>& xMinMax, const std::pair<double, double>& yMinMax, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{ Random(xMinMax.first, xMinMax.second, std::forward<decltype(urbg)>(urbg)),
				Random(yMinMax.first, yMinMax.second, std::forward<decltype(urbg)>(urbg)) };
	}

	inline Vec2 RandomVec2(const double xMax, const double yMax)
	{
		return RandomVec2(xMax, yMax, GetDefaultRNG());
	}

	Vec2 RandomVec2(const double xMax, const double yMax, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{ Random(xMax, std::forward<decltype(urbg)>(urbg)),
				Random(yMax, std::forward<decltype(urbg)>(urbg)) };
	}

	inline Vec2 RandomVec2(const Line& line)
	{
		return RandomVec2(line, GetDefaultRNG());
	}

	Vec2 RandomVec2(const Line& line, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return line.position(Random(0.0, 1.0, std::forward<decltype(urbg)>(urbg)));
	}

	inline Vec2 RandomVec2(const Circle& circle)
	{
		return RandomVec2(circle, GetDefaultRNG());
	}

	Vec2 RandomVec2(const Circle& circle, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		const double r = (std::sqrt(Random(std::forward<decltype(urbg)>(urbg))) * circle.r);
		
		const double theta = RandomClosedOpen(0.0, Math::TwoPi, std::forward<decltype(urbg)>(urbg));
		
		return circle.center.movedBy((std::cos(theta) * r), (std::sin(theta) * r));
	}

	inline Vec2 RandomVec2(const RectF& rect)
	{
		return RandomVec2(rect, GetDefaultRNG());
	}

	Vec2 RandomVec2(const RectF& rect, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{ Random(rect.x, (rect.x + rect.w), std::forward<decltype(urbg)>(urbg)),
				Random(rect.y, (rect.y + rect.h), std::forward<decltype(urbg)>(urbg)) };
	}

	inline Vec2 RandomVec2(const Triangle& triangle)
	{
		return RandomVec2(triangle, GetDefaultRNG());
	}

	Vec2 RandomVec2(const Triangle& triangle, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		double u = Random(std::forward<decltype(urbg)>(urbg));
		double v = Random(std::forward<decltype(urbg)>(urbg));

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
		const Triangle t1 = quad.triangle(0);
		const Triangle t2 = quad.triangle(1);

		const double t1Area = t1.area();
		const double t2Area = t2.area();
		const double area = (t1Area + t2Area);
		const size_t triangleIndex = ((Random(0.0, area, std::forward<decltype(urbg)>(urbg)) < t1Area) ? 0 : 1);

		return RandomVec2(((triangleIndex == 0) ? t1 : t2), std::forward<decltype(urbg)>(urbg));
	}

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
		const double theta = RandomClosedOpen(0.0, Math::TwoPi, std::forward<decltype(urbg)>(urbg));

		return{ std::cos(theta), std::sin(theta) };
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
		const double r = std::sqrt(Random(std::forward<decltype(urbg)>(urbg)));
		
		const double theta = RandomClosedOpen(0.0, Math::TwoPi, std::forward<decltype(urbg)>(urbg));
		
		return{ (std::cos(theta) * r), (std::sin(theta) * r) };
	}
}
