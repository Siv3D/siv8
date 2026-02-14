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
	constexpr Transition::Transition(const Duration& inDuration, const Duration& outDuration, const double initialValue) noexcept
		: m_inDuration{ static_cast<double>(inDuration.count()) }
		, m_outDuration{ static_cast<double>(outDuration.count()) }
		, m_value{ Clamp(initialValue, 0.0, 1.0) } {}

	constexpr void Transition::update(const bool in, const double deltaSec)
	{
		if (in)
		{
			if (m_inDuration <= 0.0)
			{
				m_value = 1.0;
				return;
			}

			m_value = Min(m_value + (deltaSec / m_inDuration), 1.0);
		}
		else
		{
			if (m_outDuration <= 0.0)
			{
				m_value = 0.0;
				return;
			}

			m_value = Max(m_value - (deltaSec / m_outDuration), 0.0);
		}
	}

	constexpr double Transition::value() const noexcept
	{
		return m_value;
	}

	constexpr bool Transition::isZero() const noexcept
	{
		return (m_value == 0.0);
	}

	constexpr bool Transition::isOne() const noexcept
	{
		return (m_value == 1.0);
	}

	constexpr double Transition::easeIn(double easingFunction(double)) const
	{
		return EaseIn(easingFunction, m_value);
	}

	constexpr double Transition::easeOut(double easingFunction(double)) const
	{
		return EaseOut(easingFunction, m_value);
	}

	constexpr double Transition::easeInOut(double easingFunction(double)) const
	{
		return EaseInOut(easingFunction, m_value);
	}

	constexpr void Transition::reset(const double initialValue) noexcept
	{
		m_value = Clamp(initialValue, 0.0, 1.0);
	}
}
