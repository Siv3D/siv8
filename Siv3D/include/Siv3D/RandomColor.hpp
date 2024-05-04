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
# include "Color.hpp"
# include "ColorF.hpp"
# include "HSV.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	RandomColor
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	Color RandomColor();

	[[nodiscard]]
	Color RandomColor(Concept::UniformRandomBitGenerator auto&& urbg);

	[[nodiscard]]
	Color RandomColor(
		const std::pair<uint32, uint32>& rMinMax,
		const std::pair<uint32, uint32>& gMinMax,
		const std::pair<uint32, uint32>& bMinMax);

	[[nodiscard]]
	Color RandomColor(
		const std::pair<uint32, uint32>& rMinMax,
		const std::pair<uint32, uint32>& gMinMax,
		const std::pair<uint32, uint32>& bMinMax,
		Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomColorF
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	ColorF RandomColorF();

	[[nodiscard]]
	ColorF RandomColorF(Concept::UniformRandomBitGenerator auto&& urbg);

	[[nodiscard]]
	ColorF RandomColorF(
		const std::pair<double, double>& rMinMax,
		const std::pair<double, double>& gMinMax,
		const std::pair<double, double>& bMinMax);

	[[nodiscard]]
	ColorF RandomColorF(
		const std::pair<double, double>& rMinMax,
		const std::pair<double, double>& gMinMax,
		const std::pair<double, double>& bMinMax,
		Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomHSV
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	HSV RandomHSV();

	[[nodiscard]]
	HSV RandomHSV(Concept::UniformRandomBitGenerator auto&& urbg);

	[[nodiscard]]
	HSV RandomHSV(
		const std::pair<double, double>& hMinMax,
		const std::pair<double, double>& sMinMax,
		const std::pair<double, double>& vMinMax);

	[[nodiscard]]
	HSV RandomHSV(
		const std::pair<double, double>& hMinMax,
		const std::pair<double, double>& sMinMax,
		const std::pair<double, double>& vMinMax,
		Concept::UniformRandomBitGenerator auto&& urbg);
}

# include "detail/RandomColor.ipp"
