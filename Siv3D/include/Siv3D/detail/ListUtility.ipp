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
	//	Min
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Type Min(std::initializer_list<Type> values)
	{
		return std::ranges::min(values);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Max
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Type Max(std::initializer_list<Type> values)
	{
		return std::ranges::max(values);
	}

	////////////////////////////////////////////////////////////////
	//
	//	MinMax
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr auto MinMax(std::initializer_list<Type> values)
	{
		return std::ranges::minmax(values);
	}
}
