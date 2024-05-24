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
# include "Common.hpp"
# include "PointVector.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	RandomVec4
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	Vec4 RandomVec4(
		const std::pair<double, double>& xMinMax,
		const std::pair<double, double>& yMinMax,
		const std::pair<double, double>& zMinMax,
		const std::pair<double, double>& wMinMax);

	[[nodiscard]]
	Vec4 RandomVec4(
		const std::pair<double, double>& xMinMax,
		const std::pair<double, double>& yMinMax,
		const std::pair<double, double>& zMinMax,
		const std::pair<double, double>& wMinMax, Concept::UniformRandomBitGenerator auto&& urbg);

	[[nodiscard]]
	Vec4 RandomVec4(double xMax, double yMax, double zMax, double wMax);

	[[nodiscard]]
	Vec4 RandomVec4(double xMax, double yMax, double zMax, double wMax, Concept::UniformRandomBitGenerator auto&& urbg);
}

# include "detail/RandomVec4.ipp"

