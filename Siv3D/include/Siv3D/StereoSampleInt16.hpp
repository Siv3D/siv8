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
# include "Common.hpp"
# include "Number.hpp"

namespace s3d
{
	struct StereoSampleFloat32;

	struct StereoSampleInt16
	{
		using value_type = int16;

		value_type left = 0;

		value_type right = 0;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		StereoSampleInt16() = default;

		[[nodiscard]]
		StereoSampleInt16(const StereoSampleInt16&) = default;

		[[nodiscard]]
		explicit constexpr StereoSampleInt16(value_type mono) noexcept;

		[[nodiscard]]
		constexpr StereoSampleInt16(value_type _left, value_type _right) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		constexpr StereoSampleInt16& operator =(const StereoSampleInt16& sample) = default;

		constexpr StereoSampleInt16& operator =(value_type mono);

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		constexpr StereoSampleInt16& set(value_type mono) noexcept;

		constexpr StereoSampleInt16& set(value_type _left, value_type _right) noexcept;

		constexpr StereoSampleInt16& set(StereoSampleInt16 sample) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setZero
		//
		////////////////////////////////////////////////////////////////

		constexpr StereoSampleInt16& setZero() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swapChannels
		//
		////////////////////////////////////////////////////////////////

		constexpr StereoSampleInt16& swapChannels() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toFloat32
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr StereoSampleFloat32 toFloat32() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend bool operator ==(const StereoSampleInt16& lhs, const StereoSampleInt16& rhs) = default;

		////////////////////////////////////////////////////////////////
		//
		//	FromFloat32
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr StereoSampleInt16 FromFloat32(float mono) noexcept;

		[[nodiscard]]
		static constexpr StereoSampleInt16 FromFloat32(float left, float right) noexcept;

		[[nodiscard]]
		static constexpr StereoSampleInt16 FromFloat32(StereoSampleFloat32 sample) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Zero, Min, Max
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr StereoSampleInt16 Zero() noexcept;

		[[nodiscard]]
		static constexpr StereoSampleInt16 Min() noexcept;

		[[nodiscard]]
		static constexpr StereoSampleInt16 Max() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Float32ToInt16, Int16ToFloat32
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr value_type Float32ToInt16(float x) noexcept;

		[[nodiscard]]
		static constexpr float Int16ToFloat32(value_type x) noexcept;
	};
}
