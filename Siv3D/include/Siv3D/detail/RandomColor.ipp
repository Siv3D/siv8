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
	//	RandomColor
	//
	////////////////////////////////////////////////////////////////

	inline Color RandomColor()
	{
		return RandomColor(GetDefaultRNG());
	}

	Color RandomColor(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return HueToColor(RandomClosedOpen(0.0, 360.0, urbg));
	}

	inline Color RandomColor(
		const std::pair<uint32, uint32>& rMinMax,
		const std::pair<uint32, uint32>& gMinMax,
		const std::pair<uint32, uint32>& bMinMax)
	{
		return RandomColor(rMinMax, gMinMax, bMinMax, GetDefaultRNG());
	}

	Color RandomColor(
		const std::pair<uint32, uint32>& rMinMax,
		const std::pair<uint32, uint32>& gMinMax,
		const std::pair<uint32, uint32>& bMinMax,
		Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{
			static_cast<uint8>(Random(rMinMax.first, rMinMax.second, urbg)),
			static_cast<uint8>(Random(gMinMax.first, gMinMax.second, urbg)),
			static_cast<uint8>(Random(bMinMax.first, bMinMax.second, urbg))
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomColorF
	//
	////////////////////////////////////////////////////////////////

	inline ColorF RandomColorF()
	{
		return RandomColorF(GetDefaultRNG());
	}

	ColorF RandomColorF(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return HueToColor(RandomClosedOpen(0.0, 360.0, urbg));
	}

	inline ColorF RandomColorF(
		const std::pair<double, double>& rMinMax,
		const std::pair<double, double>& gMinMax,
		const std::pair<double, double>& bMinMax)
	{
		return RandomColorF(rMinMax, gMinMax, bMinMax, GetDefaultRNG());
	}

	ColorF RandomColorF(
		const std::pair<double, double>& rMinMax,
		const std::pair<double, double>& gMinMax,
		const std::pair<double, double>& bMinMax,
		Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{
			Random(rMinMax.first, rMinMax.second, urbg),
			Random(gMinMax.first, gMinMax.second, urbg),
			Random(bMinMax.first, bMinMax.second, urbg)
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomHSV
	//
	////////////////////////////////////////////////////////////////

	inline HSV RandomHSV()
	{
		return RandomHSV(GetDefaultRNG());
	}

	HSV RandomHSV(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{ RandomClosedOpen(0.0, 360.0, urbg), 1.0, 1.0 };
	}

	inline HSV RandomHSV(
		const std::pair<double, double>& hMinMax,
		const std::pair<double, double>& sMinMax,
		const std::pair<double, double>& vMinMax)
	{
		return RandomHSV(hMinMax, sMinMax, vMinMax, GetDefaultRNG());
	}

	HSV RandomHSV(
		const std::pair<double, double>& hMinMax,
		const std::pair<double, double>& sMinMax,
		const std::pair<double, double>& vMinMax,
		Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{
			Random(hMinMax.first, hMinMax.second, urbg),
			Random(sMinMax.first, sMinMax.second, urbg),
			Random(vMinMax.first, vMinMax.second, urbg)
		};
	}
}
