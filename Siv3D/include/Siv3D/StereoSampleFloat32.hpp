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
	struct FormatData;

	struct StereoSampleFloat32
	{
		using value_type = float;
		
		value_type left = 0.0f;
		
		value_type right = 0.0f;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		StereoSampleFloat32() = default;
		
		[[nodiscard]]
		StereoSampleFloat32(const StereoSampleFloat32&) = default;
		
		[[nodiscard]]
		explicit constexpr StereoSampleFloat32(value_type mono) noexcept;

		[[nodiscard]]
		constexpr StereoSampleFloat32(value_type _left, value_type _right) noexcept;

		[[nodiscard]]
		constexpr StereoSampleFloat32(StereoSampleInt16 sample) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		constexpr StereoSampleFloat32& operator =(const StereoSampleFloat32& sample) = default;

		constexpr StereoSampleFloat32& operator =(value_type mono);

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		constexpr StereoSampleFloat32& set(value_type mono) noexcept;

		constexpr StereoSampleFloat32& set(value_type _left, value_type _right) noexcept;

		constexpr StereoSampleFloat32& set(StereoSampleFloat32 sample) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setZero
		//
		////////////////////////////////////////////////////////////////

		constexpr StereoSampleFloat32& setZero() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swapChannels
		//
		////////////////////////////////////////////////////////////////

		constexpr StereoSampleFloat32& swapChannels() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toInt16
		//
		////////////////////////////////////////////////////////////////

		constexpr StereoSampleInt16 toInt16() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend bool operator ==(const StereoSampleFloat32& lhs, const StereoSampleFloat32& rhs) = default;

		////////////////////////////////////////////////////////////////
		//
		//	FromInt16
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr StereoSampleFloat32 FromInt16(const int16 mono) noexcept;

		[[nodiscard]]
		static constexpr StereoSampleFloat32 FromInt16(const int16 left, const int16 right) noexcept;

		[[nodiscard]]
		static constexpr StereoSampleFloat32 FromInt16(const StereoSampleInt16 sample) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Zero, Min, Max
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr StereoSampleFloat32 Zero() noexcept;
		
		[[nodiscard]]
		static constexpr StereoSampleFloat32 Min() noexcept;
		
		[[nodiscard]]
		static constexpr StereoSampleFloat32 Max() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Float32ToInt16, Int16ToFloat32
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr int16 Float32ToInt16(value_type x) noexcept;

		[[nodiscard]]
		static constexpr value_type Int16ToFloat32(int16 x) noexcept;
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 波形サンプルを文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 波形サンプル
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, StereoSampleFloat32 value);
}
