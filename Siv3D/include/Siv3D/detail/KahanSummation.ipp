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

	template <Concept::FloatingPoint Float>
	constexpr KahanSummation<Float>::KahanSummation(const Float init) noexcept
		: m_sum{ init } {}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr KahanSummation<Float>& KahanSummation<Float>::operator +=(const Float value) noexcept
	{
		const Float y = (value - m_err);
		const Float t = (m_sum + y);
		m_err = ((t - m_sum) - y);
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
		return m_sum;
	}
}
