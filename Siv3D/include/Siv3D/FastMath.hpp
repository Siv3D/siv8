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
# include <utility>
# include "Common.hpp"
# include "MathConstants.hpp"

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
		/// @param x 逆平方根を計算する値
		/// @return 逆平方根の近似結果
		[[nodiscard]]
		constexpr float InvSqrt(float x) noexcept;

		/// @brief 逆平方根を高速に近似計算します。
		/// @param x 逆平方根を計算する値
		/// @return 逆平方根の近似結果
		[[nodiscard]]
		constexpr double InvSqrt(double x) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	InvSqrtQuality
		//
		////////////////////////////////////////////////////////////////

		/// @brief 精度の高い逆平方根を高速に近似計算します。
		/// @param x 逆平方根を計算する値
		/// @remark `FastMath::InvSqrt()` よりもコストがかかりますが、結果の精度が向上します
		/// @return 逆平方根の近似結果
		[[nodiscard]]
		constexpr double InvSqrtQuality(double x) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Sqrt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 平方根を高速に近似計算します。
		/// @remark `x * FastMath::InvSqrt(x)` を返します。
		/// @param x 平方根を計算する値
		/// @return 平方根の近似結果
		[[nodiscard]]
		constexpr float Sqrt(float x) noexcept;

		/// @brief 平方根を高速に近似計算します。
		/// @remark `x * FastMath::InvSqrt(x)` を返します。
		/// @param x 平方根を計算する値
		/// @return 平方根の近似結果
		[[nodiscard]]
		constexpr double Sqrt(double x) noexcept;
	}
}

# include "detail/FastMath.ipp"
