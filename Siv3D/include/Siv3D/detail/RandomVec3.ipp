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
# include <cmath>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	RandomUnitVec3
	//
	////////////////////////////////////////////////////////////////

	inline Vec3 RandomUnitVec3()
	{
		return RandomUnitVec3(GetDefaultRNG());
	}

	Vec3 RandomUnitVec3(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		const double z = RandomClosedOpen(-1.0, 1.0, urbg);
		const double phi = RandomClosedOpen(0.0, Math::TwoPi, urbg);
		const double r = std::sqrt(1.0 - z * z);
		return{ (r * std::cos(phi)), (r * std::sin(phi)), z };
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomVec3
	//
	////////////////////////////////////////////////////////////////

	inline Vec3 RandomVec3(const double length)
	{
		return RandomVec3(length, GetDefaultRNG());
	}

	Vec3 RandomVec3(const double length, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return (RandomUnitVec3(urbg) * length);
	}

	inline Vec3 RandomVec3(
		const std::pair<double, double>& xMinMax,
		const std::pair<double, double>& yMinMax,
		const std::pair<double, double>& zMinMax)
	{
		return RandomVec3(xMinMax, yMinMax, zMinMax, GetDefaultRNG());
	}

	Vec3 RandomVec3(
		const std::pair<double, double>& xMinMax,
		const std::pair<double, double>& yMinMax,
		const std::pair<double, double>& zMinMax, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{
			RandomClosedOpen(xMinMax.first, xMinMax.second, urbg),
			RandomClosedOpen(yMinMax.first, yMinMax.second, urbg),
			RandomClosedOpen(zMinMax.first, zMinMax.second, urbg)
		};
	}

	inline Vec3 RandomVec3(const double xMax, const double yMax, const double zMax)
	{
		return RandomVec3(xMax, yMax, zMax, GetDefaultRNG());
	}

	Vec3 RandomVec3(const double xMax, const double yMax, const double zMax, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return RandomVec3({ 0.0, xMax }, { 0.0, yMax }, { 0.0, zMax }, urbg);
	}

	inline Vec3 RandomVec3(const Box& box)
	{
		return RandomVec3(box, GetDefaultRNG());
	}

	Vec3 RandomVec3(const Box& box, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		const Vec3 halfSize = (box.size * 0.5);
		return RandomVec3(
			{ (box.center.x - halfSize.x), (box.center.x + halfSize.x) },
			{ (box.center.y - halfSize.y), (box.center.y + halfSize.y) },
			{ (box.center.z - halfSize.z), (box.center.z + halfSize.z) }, urbg);
	}

	inline Vec3 RandomVec3(const Sphere& sphere)
	{
		return RandomVec3(sphere, GetDefaultRNG());
	}

	Vec3 RandomVec3(const Sphere& sphere, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return (sphere.center + RandomVec3InsideUnitSphere(urbg) * sphere.r);
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomVec3InsideUnitSphere
	//
	////////////////////////////////////////////////////////////////

	inline Vec3 RandomVec3InsideUnitSphere()
	{
		return RandomVec3InsideUnitSphere(GetDefaultRNG());
	}

	Vec3 RandomVec3InsideUnitSphere(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		for (;;)
		{
			const Vec3 v{
				(2.0 * Random(urbg) - 1.0),
				(2.0 * Random(urbg) - 1.0),
				(2.0 * Random(urbg) - 1.0)
			};

			if (v.lengthSq() < 1.0)
			{
				return v;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomVec3On
	//
	////////////////////////////////////////////////////////////////

	inline Vec3 RandomVec3On(const Sphere& sphere)
	{
		return RandomVec3On(sphere, GetDefaultRNG());
	}

	Vec3 RandomVec3On(const Sphere& sphere, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return (sphere.center + RandomUnitVec3(urbg) * sphere.r);
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomVec3InsideSphericalShell
	//
	////////////////////////////////////////////////////////////////

	inline Vec3 RandomVec3InsideSphericalShell(const double innerRadius, const double outerRadius)
	{
		return RandomVec3InsideSphericalShell(innerRadius, outerRadius, GetDefaultRNG());
	}

	Vec3 RandomVec3InsideSphericalShell(const double innerRadius, const double outerRadius, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		if (outerRadius == 0.0)
		{
			return Vec3::Zero();
		}

		const double innerRatio = (innerRadius / outerRadius);
		const double innerRatioCubed = (innerRatio * innerRatio * innerRatio);
		const double radius = (outerRadius * std::cbrt(innerRatioCubed + (1.0 - innerRatioCubed) * Random(urbg)));
		return (RandomUnitVec3(urbg) * radius);
	}
}
