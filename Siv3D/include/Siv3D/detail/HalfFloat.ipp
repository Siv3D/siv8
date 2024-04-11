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
	inline HalfFloat::HalfFloat(const float value) noexcept
		: m_bits{ DirectX::PackedVector::XMConvertFloatToHalf(value) } {}

	HalfFloat::HalfFloat(const Concept::Arithmetic auto value) noexcept
		: HalfFloat{ static_cast<float>(value) } {}

	inline HalfFloat& HalfFloat::operator =(const float value) noexcept
	{
		m_bits = DirectX::PackedVector::XMConvertFloatToHalf(value);
		return *this;
	}

	HalfFloat& HalfFloat::operator =(const Concept::Arithmetic auto value)
	{
		return operator =(static_cast<float>(value));
	}

	constexpr HalfFloat HalfFloat::operator +() const noexcept
	{
		return *this;
	}

	constexpr HalfFloat HalfFloat::operator -() const noexcept
	{
		HalfFloat result;
		result.m_bits = (m_bits ^ SignMask);
		return result;
	}

	inline float HalfFloat::asFloat() const noexcept
	{
		return DirectX::PackedVector::XMConvertHalfToFloat(m_bits);
	}

	inline HalfFloat::operator float() const noexcept
	{
		return asFloat();
	}

	constexpr uint16 HalfFloat::getBits() const noexcept
	{
		return m_bits;
	}

	constexpr void HalfFloat::setBits(const uint16 bits) noexcept
	{
		m_bits = bits;
	}

	constexpr bool HalfFloat::isZero() const noexcept
	{
		return IsZero(m_bits);
	}

	constexpr bool HalfFloat::isNaN() const noexcept
	{
		return IsNaN(m_bits);
	}

	constexpr bool HalfFloat::isInfinity() const noexcept
	{
		return ((m_bits & 0x7FFF) == 0x7C00);
	}

	constexpr bool HalfFloat::signBit() const noexcept
	{
		return static_cast<bool>(m_bits & SignMask);
	}

	constexpr HalfFloat HalfFloat::abs() const noexcept
	{
		HalfFloat result;
		result.m_bits = (m_bits & 0x7FFF);
		return result;
	}

	inline uint64 HalfFloat::hash() const noexcept
	{
		return Hash(m_bits);
	}

	constexpr bool HalfFloat::IsZero(const uint16 bits) noexcept
	{
		return ((bits & 0x7FFF) == 0);
	}

	constexpr bool HalfFloat::IsNaN(const uint16 bits) noexcept
	{
		return (0x7C00 < (bits & 0x7FFF));
	}
}
