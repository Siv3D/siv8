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

	constexpr PointsPerCircle QualityFactor::toPointsPerCircle(const double r) const noexcept
	{
		const double t = (m_value * Abs(r));

		if (t <= 2.0)
		{
			return PointsPerCircle{ 8u };
		}
		else if (t <= 4.0)
		{
			return PointsPerCircle{ 12u };
		}
		else if (t < 8.0)
		{
			return PointsPerCircle{ 16u };
		}
		else
		{
			const uint32 n = Min((5 + static_cast<uint32>((t - 8.0) / 8.0)), 63u);
			return PointsPerCircle{ n * 4 };
		}
	}

	constexpr QualityFactor QualityFactor::Default() noexcept
	{
		return QualityFactor{ DefaultQuality };
	}
}
