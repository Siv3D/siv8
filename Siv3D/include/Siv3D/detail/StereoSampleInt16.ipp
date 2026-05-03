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

	constexpr StereoSampleInt16::StereoSampleInt16(const value_type mono) noexcept
		: left{ mono }
		, right{ mono } {}

	constexpr StereoSampleInt16::StereoSampleInt16(const value_type _left, const value_type _right) noexcept
		: left{ _left }
		, right{ _right } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	constexpr StereoSampleInt16& StereoSampleInt16::operator =(const value_type mono)
	{
		left = mono;
		right = mono;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr StereoSampleInt16& StereoSampleInt16::set(const value_type mono) noexcept
	{
		left = mono;
		right = mono;
		return *this;
	}

	constexpr StereoSampleInt16& StereoSampleInt16::set(const value_type _left, const value_type _right) noexcept
	{
		left = _left;
		right = _right;
		return *this;
	}

	constexpr StereoSampleInt16& StereoSampleInt16::set(const StereoSampleInt16 sample) noexcept
	{
		left = sample.left;
		right = sample.right;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setZero
	//
	////////////////////////////////////////////////////////////////

	constexpr StereoSampleInt16& StereoSampleInt16::setZero() noexcept
	{
		left = 0;
		right = 0;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	swapChannels
	//
	////////////////////////////////////////////////////////////////

	constexpr StereoSampleInt16& StereoSampleInt16::swapChannels() noexcept
	{
		const value_type temp = left;
		left = right;
		right = temp;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	toFloat32
	//
	////////////////////////////////////////////////////////////////

	constexpr StereoSampleFloat32 StereoSampleInt16::toFloat32() const noexcept
	{
		return{ Int16ToFloat32(left), Int16ToFloat32(right) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromFloat32
	//
	////////////////////////////////////////////////////////////////

	constexpr StereoSampleInt16 StereoSampleInt16::FromFloat32(const float mono) noexcept
	{
		const value_type s16 = Float32ToInt16(mono);
		return{ s16, s16 };
	}

	constexpr StereoSampleInt16 StereoSampleInt16::FromFloat32(const float left, const float right) noexcept
	{
		return{ Float32ToInt16(left), Float32ToInt16(right) };
	}

	constexpr StereoSampleInt16 StereoSampleInt16::FromFloat32(const StereoSampleFloat32 sample) noexcept
	{
		return{ Float32ToInt16(sample.left), Float32ToInt16(sample.right) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero, Min, Max
	//
	////////////////////////////////////////////////////////////////

	constexpr StereoSampleInt16 StereoSampleInt16::Zero() noexcept
	{
		return{ 0, 0 };
	}

	constexpr StereoSampleInt16 StereoSampleInt16::Min() noexcept
	{
		return{ Smallest<int16>, Smallest<int16> };
	}

	constexpr StereoSampleInt16 StereoSampleInt16::Max() noexcept
	{
		return{ Largest<int16>, Largest<int16> };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Float32ToInt16, Int16ToFloat32
	//
	////////////////////////////////////////////////////////////////

	constexpr StereoSampleInt16::value_type StereoSampleInt16::Float32ToInt16(const float x) noexcept
	{
		if (x <= -1.0f)
		{
			return Smallest<int16>; // -32768
		}

		if (x >= 1.0f)
		{
			return Largest<int16>; // 32767
		}

		const float scaled = ((x < 0.0f) ? (x * 32768.0f) : (x * 32767.0f));
		const float rounded = ((scaled < 0.0f) ? (scaled - 0.5f) : (scaled + 0.5f));
		return static_cast<value_type>(rounded);
	}

	constexpr float StereoSampleInt16::Int16ToFloat32(const value_type x) noexcept
	{
		return ((x < 0) ? (static_cast<float>(x) / 32768.0f) : (static_cast<float>(x) / 32767.0f));
	}
}
