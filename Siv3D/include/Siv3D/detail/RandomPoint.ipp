//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	RandomPoint
	//
	////////////////////////////////////////////////////////////////

	inline Point RandomPoint(const std::pair<int32, int32> xMinMax, const std::pair<int32, int32> yMinMax)
	{
		return RandomPoint(xMinMax, yMinMax, GetDefaultRNG());
	}

	Point RandomPoint(const std::pair<int32, int32> xMinMax, const std::pair<int32, int32> yMinMax, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{ Random(xMinMax.first, xMinMax.second, std::forward<decltype(urbg)>(urbg)),
			Random(yMinMax.first, yMinMax.second, std::forward<decltype(urbg)>(urbg)) };
	}

	inline Point RandomPoint(const Rect& rect)
	{
		return RandomPoint(rect, GetDefaultRNG());
	}

	Point RandomPoint(const Rect& rect, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{ Random(rect.x, (rect.x + rect.w - 1), std::forward<decltype(urbg)>(urbg)),
			Random(rect.y, (rect.y + rect.h - 1), std::forward<decltype(urbg)>(urbg)) };
	}

	inline Point RandomPoint(const int32 xMax, const int32 yMax)
	{
		return RandomPoint(xMax, yMax, GetDefaultRNG());
	}

	Point RandomPoint(const int32 xMax, const int32 yMax, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return{ Random(xMax, std::forward<decltype(urbg)>(urbg)),
			Random(yMax, std::forward<decltype(urbg)>(urbg)) };
	}
}
