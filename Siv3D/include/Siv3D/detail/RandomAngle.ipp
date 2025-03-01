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
	//	RandomAngle
	//
	////////////////////////////////////////////////////////////////

	inline double RandomAngle() noexcept
	{
		return RandomAngle(GetDefaultRNG());
	}

	double RandomAngle(Concept::UniformRandomBitGenerator auto&& urbg) noexcept
	{
		return RandomClosedOpen(0.0, Math::Constants::TwoPi, std::forward<decltype(urbg)>(urbg));
	}
}
