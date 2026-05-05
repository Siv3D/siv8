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

	constexpr StereoSampleFloat32::StereoSampleFloat32(const value_type mono) noexcept
		: left{ mono }
		, right{ mono } {}

	constexpr StereoSampleFloat32::StereoSampleFloat32(const value_type _left, const value_type _right) noexcept
		: left{ _left }
		, right{ _right } {}

	constexpr StereoSampleFloat32::StereoSampleFloat32(const StereoSampleInt16 sample) noexcept
		: left{ Int16ToFloat32(sample.left) }
		, right{ Int16ToFloat32(sample.right) } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	constexpr StereoSampleFloat32& StereoSampleFloat32::operator =(value_type mono)
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

	constexpr StereoSampleFloat32& StereoSampleFloat32::set(const value_type mono) noexcept
	{
		left = mono;
		right = mono;
		return *this;
	}

	constexpr StereoSampleFloat32& StereoSampleFloat32::set(const value_type _left, const value_type _right) noexcept
	{
		left = _left;
		right = _right;
		return *this;
	}

	constexpr StereoSampleFloat32& StereoSampleFloat32::set(const StereoSampleFloat32 sample) noexcept
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

	constexpr StereoSampleFloat32& StereoSampleFloat32::setZero() noexcept
	{
		left = 0.0f;
		right = 0.0f;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	swapChannels
	//
	////////////////////////////////////////////////////////////////

	constexpr StereoSampleFloat32& StereoSampleFloat32::swapChannels() noexcept
	{
		const value_type temp = left;
		left = right;
		right = temp;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	toInt16
	//
	////////////////////////////////////////////////////////////////

	constexpr StereoSampleInt16 StereoSampleFloat32::toInt16() const noexcept
	{
		return StereoSampleInt16::FromFloat32(left, right);
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromInt16
	//
	////////////////////////////////////////////////////////////////

	constexpr StereoSampleFloat32 StereoSampleFloat32::FromInt16(const int16 mono) noexcept
	{
		const value_type f32 = Int16ToFloat32(mono);
		return{ f32, f32 };
	}

	constexpr StereoSampleFloat32 StereoSampleFloat32::FromInt16(const int16 left, const int16 right) noexcept
	{
		return{ Int16ToFloat32(left), Int16ToFloat32(right) };
	}

	constexpr StereoSampleFloat32 StereoSampleFloat32::FromInt16(const StereoSampleInt16 sample) noexcept
	{
		return FromInt16(sample.left, sample.right);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero, Min, Max
	//
	////////////////////////////////////////////////////////////////

	constexpr StereoSampleFloat32 StereoSampleFloat32::Zero() noexcept
	{
		return{ 0.0f, 0.0f };
	}

	constexpr StereoSampleFloat32 StereoSampleFloat32::Min() noexcept
	{
		return{ -1.0f, -1.0f };
	}

	constexpr StereoSampleFloat32 StereoSampleFloat32::Max() noexcept
	{
		return{ 1.0f, 1.0f };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Float32ToInt16, Int16ToFloat32
	//
	////////////////////////////////////////////////////////////////

	constexpr int16 StereoSampleFloat32::Float32ToInt16(const value_type x) noexcept
	{
		return StereoSampleInt16::Float32ToInt16(x);
	}

	constexpr StereoSampleFloat32::value_type StereoSampleFloat32::Int16ToFloat32(const int16 x) noexcept
	{
		return StereoSampleInt16::Int16ToFloat32(x);
	}
}
