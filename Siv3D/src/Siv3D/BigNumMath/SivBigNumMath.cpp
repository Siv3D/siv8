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

# include <Siv3D/Math.hpp>
# include <Siv3D/BigInt.hpp>
# include <Siv3D/BigFloat.hpp>
# include <Siv3D/BigInt/BigIntDetail.hpp>
# include <Siv3D/BigFloat/BigFloatDetail.hpp>

namespace s3d
{
	namespace Math
	{
		namespace detail
		{
			//////////////////////////////////////////////////
			//
			//	Fmod_impl
			//
			//////////////////////////////////////////////////

			BigFloat Fmod_impl::operator ()(const BigFloat& x, const BigFloat& y) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::fmod(x._detail().value, y._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Fraction_impl
			//
			//////////////////////////////////////////////////

			BigFloat Fraction_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = (x._detail().value - boost::multiprecision::floor(x._detail().value));
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Frexp_impl
			//
			//////////////////////////////////////////////////

			BigFloat Frexp_impl::operator ()(const BigFloat& x, int32& exp) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::frexp(x._detail().value, &exp);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Ldexp_impl
			//
			//////////////////////////////////////////////////

			BigFloat Ldexp_impl::operator ()(const BigFloat& x, const BigFloat& exp) const
			{
				BigFloat result;
				result._detail().value = (x._detail().value * boost::multiprecision::exp2(exp._detail().value));
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Log_impl
			//
			//////////////////////////////////////////////////

			BigFloat Log_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::log(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Log2_impl
			//
			//////////////////////////////////////////////////

			BigFloat Log2_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::log2(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Log10_impl
			//
			//////////////////////////////////////////////////

			BigFloat Log10_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::log10(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Modf_impl
			//
			//////////////////////////////////////////////////

			//////////////////////////////////////////////////
			//
			//	Pow_impl
			//
			//////////////////////////////////////////////////

			//////////////////////////////////////////////////
			//
			//	Sign_impl
			//
			//////////////////////////////////////////////////

			//////////////////////////////////////////////////
			//
			//	Sqrt_impl
			//
			//////////////////////////////////////////////////

			BigFloat Sqrt_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat Sqrt_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::sqrt(x._detail().value);
				return result;
			}
		}


		BigFloat Modf(const BigFloat& x, BigFloat& exp)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::modf(x._detail().value, &exp._detail().value);
			return result;
		}

		BigInt Pow(const BigInt& x, const uint32 y)
		{
			BigInt result;
			result._detail().value = boost::multiprecision::pow(x._detail().value, y);
			return result;
		}

		BigFloat Pow(const BigFloat& x, const BigFloat& y)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::pow(x._detail().value, y._detail().value);
			return result;
		}

		int32 Sign(const BigInt& x)
		{
			const int32 c = x.compare(0);

			if (c < 0)
			{
				return -1;
			}
			else if (0 < c)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}

		int32 Sign(const BigFloat& x)
		{
			const int32 c = x.compare(0);

			if (c < 0)
			{
				return -1;
			}
			else if (0 < c)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}

		BigFloat ToRadians(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = (x._detail().value * (boost::math::constants::pi<boost::multiprecision::cpp_dec_float_100>() / 180));
			return result;
		}

		BigFloat ToDegrees(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = (x._detail().value * (180 / boost::math::constants::pi<boost::multiprecision::cpp_dec_float_100>()));
			return result;
		}

		BigFloat Abs(const BigInt& x)
		{
			return x.abs();
		}

		BigFloat Abs(const BigFloat& x)
		{
			return x.abs();
		}

		BigFloat AbsDiff(const BigFloat& x, const BigFloat& y)
		{
			return (x - y).abs();
		}

		BigFloat Square(const BigInt& x)
		{
			return (x * x);
		}

		BigFloat Square(const BigFloat& x)
		{
			return (x * x);
		}

		BigFloat Exp(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::exp(x._detail().value);
			return result;
		}

		BigFloat Exp2(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::exp2(x._detail().value);
			return result;
		}

		BigFloat Rsqrt(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = (1 / boost::multiprecision::sqrt(x._detail().value));
			return result;
		}

		BigFloat Ceil(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::ceil(x._detail().value);
			return result;
		}

		BigFloat Floor(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::floor(x._detail().value);
			return result;
		}

		BigFloat Round(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::round(x._detail().value);
			return result;
		}

		BigFloat Clamp(const BigFloat& x, const BigFloat& min, const BigFloat& max)
		{
			if (max < x)
			{
				return max;
			}
			else if (x < min)
			{
				return min;
			}

			return x;
		}

		BigFloat Saturate(const BigFloat& x)
		{
			if (1 < x)
			{
				return 1;
			}
			else if (x < 0)
			{
				return 0;
			}

			return x;
		}

		BigFloat Acos(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::acos(x._detail().value);
			return result;
		}

		BigFloat Asin(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::asin(x._detail().value);
			return result;
		}

		BigFloat Atan(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::atan(x._detail().value);
			return result;
		}

		BigFloat Atan2(const BigFloat& y, const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::atan2(y._detail().value, x._detail().value);
			return result;
		}

		BigFloat Cos(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::cos(x._detail().value);
			return result;
		}

		BigFloat Cosh(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::cosh(x._detail().value);
			return result;
		}

		BigFloat Sin(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::sin(x._detail().value);
			return result;
		}

		BigFloat Sinh(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::sinh(x._detail().value);
			return result;
		}

		BigFloat Tan(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::tan(x._detail().value);
			return result;
		}

		BigFloat Tanh(const BigFloat& x)
		{
			BigFloat result;
			result._detail().value = boost::multiprecision::tanh(x._detail().value);
			return result;
		}

		BigFloat Normalize(const BigFloat& x)
		{
			if (x.isZero())
			{
				return 0;
			}

			return 1;
		}

		BigFloat Smoothstep(const BigFloat& min, const BigFloat& max, const BigFloat& x)
		{
			if (x <= min)
			{
				return 0;
			}
			else if (max <= x)
			{
				return 1;
			}

			const BigFloat t = ((x - min) / (max - min));

			return (t * t * (3 - 2 * t));
		}

		BigFloat Smoothstep(const BigFloat& x)
		{
			if (x <= 0)
			{
				return 0;
			}
			else if (1 <= x)
			{
				return 1;
			}

			return (x * x * (3 - 2 * x));
		}
	}
}
