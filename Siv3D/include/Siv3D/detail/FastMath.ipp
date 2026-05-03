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
	namespace FastMath
	{
		////////////////////////////////////////////////////////////////
		//
		//	SinCos
		//
		////////////////////////////////////////////////////////////////

		constexpr std::pair<float, float> SinCos(const float angleRadians) noexcept
		{
			float quotient = (Math::InvTwoPiF * angleRadians);

			if (angleRadians >= 0.0f)
			{
				quotient = static_cast<float>(static_cast<int32>(quotient + 0.5f));
			}
			else
			{
				quotient = static_cast<float>(static_cast<int32>(quotient - 0.5f));
			}

			float y = (angleRadians - (Math::TwoPiF * quotient));
			float sign = 1.0f;

			if (y > Math::HalfPiF)
			{
				y = (Math::PiF - y);
				sign = -1.0f;
			}
			else if (y < -Math::HalfPiF)
			{
				y = (-Math::PiF - y);
				sign = -1.0f;
			}

			const float y2 = (y * y);
			const float sin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;
			const float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
			const float cos = (sign * p);

			return{ sin, cos };
		}

		constexpr std::pair<float, float> SinCos(const double angleRadians) noexcept
		{
			double quotient = (Math::InvTwoPi * angleRadians);

			if (angleRadians >= 0.0)
			{
				quotient = static_cast<double>(static_cast<int32>(quotient + 0.5));
			}
			else
			{
				quotient = static_cast<double>(static_cast<int32>(quotient - 0.5));
			}

			float y = static_cast<float>(angleRadians - (Math::TwoPi * quotient));
			float sign = 1.0f;

			if (y > Math::HalfPiF)
			{
				y = (Math::PiF - y);
				sign = -1.0f;
			}
			else if (y < -Math::HalfPiF)
			{
				y = (-Math::PiF - y);
				sign = -1.0f;
			}

			const float y2 = (y * y);
			const float sin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;
			const float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
			const float cos = (sign * p);

			return{ sin, cos };
		}

		////////////////////////////////////////////////////////////////
		//
		//	InvSqrt
		//
		////////////////////////////////////////////////////////////////

		inline float InvSqrt(const float value) noexcept
		{
		# if SIV3D_INTRINSIC(SSE)
		
			const __m128 half = _mm_set_ss(0.5f);
			const __m128 threeHalves = _mm_set_ss(1.5f);

			const __m128 x = _mm_set_ss(value);
			__m128 y = _mm_rsqrt_ss(x);
			y = _mm_mul_ss(y, _mm_sub_ss(threeHalves, _mm_mul_ss(_mm_mul_ss(half, x), _mm_mul_ss(y, y))));

			return _mm_cvtss_f32(y);
		
		# elif SIV3D_INTRINSIC(NEON)

			float32_t y = vrsqrtes_f32(value);
			y *= vrsqrtss_f32(value * y, y);
			return y;

		# else

			return (1.0f / std::sqrt(value));

		# endif
		}

		inline double InvSqrt(const double value) noexcept
		{
		# if SIV3D_INTRINSIC(NEON)

			float64_t y = vrsqrted_f64(value);
			y *= vrsqrtsd_f64(value * y, y);
			return y;

		# else

			return (1.0 / std::sqrt(value));

		# endif
		}
	}
}
