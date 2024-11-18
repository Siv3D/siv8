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
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr PointsPerCircle::PointsPerCircle(const uint32 value) noexcept
		: m_value{ Max(value, MinPoints) } {}

	////////////////////////////////////////////////////////////////
	//
	//	value
	//
	////////////////////////////////////////////////////////////////

	constexpr uint32 PointsPerCircle::value() const noexcept
	{
		return m_value;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Default
	//
	////////////////////////////////////////////////////////////////

	constexpr PointsPerCircle PointsPerCircle::Default() noexcept
	{
		return PointsPerCircle{ DefaultPoints };
	}
}
