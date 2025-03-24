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
	//	Min
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Type Min(std::initializer_list<Type> values)
	{
		return *std::min_element(values.begin(), values.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	Max
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Type Max(std::initializer_list<Type> values)
	{
		return *std::max_element(values.begin(), values.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	MinMax
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr std::pair<Type, Type> MinMax(std::initializer_list<Type> values)
	{
		const auto [minIt, maxIt] = std::minmax_element(values.begin(), values.end());
		return{ *minIt, *maxIt };
	}
}
