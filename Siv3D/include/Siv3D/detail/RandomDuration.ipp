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
	//	Random
	//
	////////////////////////////////////////////////////////////////

	inline Duration Random(const Duration& max) noexcept
	{
		return Random(max, GetDefaultRNG());
	}

	Duration Random(const Duration& max, Concept::UniformRandomBitGenerator auto&& urbg) noexcept
	{
		return Duration{ Random(max.count(), std::forward<decltype(urbg)>(urbg))};
	}

	inline Duration Random(const Duration& min, const Duration& max) noexcept
	{
		return Random(min, max, GetDefaultRNG());
	}

	[[nodiscard]]
	Duration Random(const Duration& min, const Duration& max, Concept::UniformRandomBitGenerator auto&& urbg) noexcept
	{
		return Duration{ Random(min.count(), max.count(), std::forward<decltype(urbg)>(urbg))};
	}
}
