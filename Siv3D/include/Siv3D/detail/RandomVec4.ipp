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
	//	RandomVec4
	//
	////////////////////////////////////////////////////////////////

	inline Vec4 RandomVec4(
		const std::pair<double, double>& xMinMax,
		const std::pair<double, double>& yMinMax,
		const std::pair<double, double>& zMinMax,
		const std::pair<double, double>& wMinMax)
	{
		return RandomVec4(xMinMax, yMinMax, zMinMax, wMinMax, GetDefaultRNG());
	}

	Vec4 RandomVec4(
		const std::pair<double, double>& xMinMax,
		const std::pair<double, double>& yMinMax,
		const std::pair<double, double>& zMinMax,
		const std::pair<double, double>& wMinMax, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{
			RandomClosed(xMinMax.first, xMinMax.second, std::forward<decltype(urbg)>(urbg)),
			RandomClosed(yMinMax.first, yMinMax.second, std::forward<decltype(urbg)>(urbg)),
			RandomClosed(zMinMax.first, zMinMax.second, std::forward<decltype(urbg)>(urbg)),
			RandomClosed(wMinMax.first, wMinMax.second, std::forward<decltype(urbg)>(urbg)),
		};
	}

	inline Vec4 RandomVec4(const double xMax, const double yMax, const double zMax, const double wMax)
	{
		return RandomVec4(xMax, yMax, zMax, wMax, GetDefaultRNG());
	}

	Vec4 RandomVec4(const double xMax, const double yMax, const double zMax, const double wMax, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{
			RandomClosed(0.0, xMax, std::forward<decltype(urbg)>(urbg)),
			RandomClosed(0.0, yMax, std::forward<decltype(urbg)>(urbg)),
			RandomClosed(0.0, zMax, std::forward<decltype(urbg)>(urbg)),
			RandomClosed(0.0, wMax, std::forward<decltype(urbg)>(urbg)),
		};
	}
}
