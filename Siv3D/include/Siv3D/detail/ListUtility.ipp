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
}
