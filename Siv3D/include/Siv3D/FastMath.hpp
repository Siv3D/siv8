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
# include <utility>
# include "Common.hpp"
# include "MathConstants.hpp"
# include "SIMD.hpp"

namespace s3d
{
	namespace FastMath
	{
		////////////////////////////////////////////////////////////////
		//
		//	SinCos
		//
		////////////////////////////////////////////////////////////////

		/// @brief sin, cos を高速に近似計算します。
		/// @param angleRadians ラジアン角
		/// @return sin, cos の近似結果
		[[nodiscard]]
		constexpr std::pair<float, float> SinCos(float angleRadians) noexcept;

		/// @brief sin, cos を高速に近似計算します。
		/// @param angleRadians ラジアン角
		/// @return sin, cos の近似結果
		[[nodiscard]]
		constexpr std::pair<float, float> SinCos(double angleRadians) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	InvSqrt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 逆平方根を高速に近似計算します。
		/// @param value 逆平方根を計算する値
		/// @return 逆平方根の近似結果
		[[nodiscard]]
		float InvSqrt(float value) noexcept;

		/// @brief 逆平方根を高速に近似計算します。
		/// @param value 逆平方根を計算する値
		/// @return 逆平方根の近似結果
		[[nodiscard]]
		double InvSqrt(double value) noexcept;
	}
}

# include "detail/FastMath.ipp"
