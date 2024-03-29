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
# include "Common.hpp"

namespace s3d
{
	class BigInt;
	class BigFloat;

	namespace Math
	{
		[[nodiscard]]
		BigFloat Fmod(const BigFloat& x, const BigFloat& y);

		[[nodiscard]]
		BigFloat Fraction(const BigFloat& x);

		[[nodiscard]]
		BigFloat Frexp(const BigFloat& x, int32& exp);

		[[nodiscard]]
		BigFloat Ldexp(const BigFloat& x, const BigFloat& y);

		[[nodiscard]]
		BigFloat Log(const BigFloat& x);

		[[nodiscard]]
		BigFloat Log2(const BigFloat& x);

		[[nodiscard]]
		BigFloat Log10(const BigFloat& x);

		[[nodiscard]]
		BigFloat Modf(const BigFloat& x, BigFloat& exp);

		[[nodiscard]]
		BigInt Pow(const BigInt& x, uint32 y);

		[[nodiscard]]
		BigFloat Pow(const BigFloat& x, const BigFloat& y);

		[[nodiscard]]
		int32 Sign(const BigInt& x);

		[[nodiscard]]
		int32 Sign(const BigFloat& x);

		[[nodiscard]]
		BigFloat ToRadians(const BigFloat& x);

		[[nodiscard]]
		BigFloat ToDegrees(const BigFloat& x);

		[[nodiscard]]
		BigFloat Abs(const BigInt& x);

		[[nodiscard]]
		BigFloat Abs(const BigFloat& x);

		[[nodiscard]]
		BigFloat AbsDiff(const BigFloat& x, const BigFloat& y);

		[[nodiscard]]
		BigFloat Square(const BigInt& x);

		[[nodiscard]]
		BigFloat Square(const BigFloat& x);

		[[nodiscard]]
		BigFloat Exp(const BigFloat& x);

		[[nodiscard]]
		BigFloat Exp2(const BigFloat& x);

		[[nodiscard]]
		BigFloat Rsqrt(const BigFloat& x);

		[[nodiscard]]
		BigFloat Sqrt(const BigFloat& x);

		[[nodiscard]]
		BigFloat Ceil(const BigFloat& x);

		[[nodiscard]]
		BigFloat Floor(const BigFloat& x);

		[[nodiscard]]
		BigFloat Round(const BigFloat& x);

		[[nodiscard]]
		BigFloat Clamp(const BigFloat& x, const BigFloat& min, const BigFloat& max);

		[[nodiscard]]
		BigFloat Saturate(const BigFloat& x);

		[[nodiscard]]
		BigFloat Acos(const BigFloat& x);

		[[nodiscard]]
		BigFloat Asin(const BigFloat& x);

		[[nodiscard]]
		BigFloat Atan(const BigFloat& x);

		[[nodiscard]]
		BigFloat Atan2(const BigFloat& y, const BigFloat& x);

		[[nodiscard]]
		BigFloat Cos(const BigFloat& x);

		[[nodiscard]]
		BigFloat Cosh(const BigFloat& x);

		[[nodiscard]]
		BigFloat Sin(const BigFloat& x);

		[[nodiscard]]
		BigFloat Sinh(const BigFloat& x);

		[[nodiscard]]
		BigFloat Tan(const BigFloat& x);

		[[nodiscard]]
		BigFloat Tanh(const BigFloat& x);

		[[nodiscard]]
		BigFloat Normalize(const BigFloat& x);

		[[nodiscard]]
		BigFloat Smoothstep(const BigFloat& min, const BigFloat& max, const BigFloat& x);

		[[nodiscard]]
		BigFloat Smoothstep(const BigFloat& x);
	}
}
