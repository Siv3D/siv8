﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	template <Concept::FloatingPoint Float>
	inline constexpr KahanSummation<Float>::KahanSummation(const Float init) noexcept
		: m_sum{ init } {}

	template <Concept::FloatingPoint Float>
	inline constexpr KahanSummation<Float>& KahanSummation<Float>::operator +=(const Float value) noexcept
	{
		const Float y = (value - m_err);
		const Float t = (m_sum + y);
		m_err = ((t - m_sum) - y);
		m_sum = t;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	inline constexpr KahanSummation<Float>& KahanSummation<Float>::operator -=(const Float value) noexcept
	{
		return (*this += (-value));
	}

	template <Concept::FloatingPoint Float>
	inline constexpr Float KahanSummation<Float>::value() const noexcept
	{
		return m_sum;
	}
}
