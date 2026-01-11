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
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr KahanSummation<Float>::KahanSummation(const Float init) noexcept
		: m_sum{ init } {}

	////////////////////////////////////////////////////////////////
	//
	//	reset
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr void KahanSummation<Float>::reset(const Float init) noexcept
	{
		m_sum = init;
		m_c = 0;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr KahanSummation<Float>& KahanSummation<Float>::operator +=(const Float value) noexcept
	{
		const Float t = (m_sum + value);

		if (Abs(value) <= Abs(m_sum))
		{
			m_c += ((m_sum - t) + value);
		}
		else
		{
			m_c += ((value - t) + m_sum);
		}

		m_sum = t;
		return *this;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr KahanSummation<Float>& KahanSummation<Float>::operator -=(const Float value) noexcept
	{
		return (*this += (-value));
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	value
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Float KahanSummation<Float>::value() const noexcept
	{
		return (m_sum + m_c);
	}

	////////////////////////////////////////////////////////////////
	//
	//	correction
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Float KahanSummation<Float>::correction() const noexcept
	{
		return m_c;
	}
}
