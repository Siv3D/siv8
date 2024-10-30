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
	constexpr QualityFactor::QualityFactor(const double value) noexcept
		: m_value{ Max(value, 0.0) } {}

	constexpr QualityFactor QualityFactor::operator *(const double s) const noexcept
	{
		return QualityFactor{ m_value * s };
	}

	constexpr double QualityFactor::value() const noexcept
	{
		return m_value;
	}
}
