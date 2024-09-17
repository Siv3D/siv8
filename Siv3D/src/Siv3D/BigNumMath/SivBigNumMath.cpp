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
			namespace
			{
				static const auto cRad = (boost::math::constants::pi<boost::multiprecision::cpp_dec_float_100>() / 180);

				static const auto cDeg = (180 / boost::math::constants::pi<boost::multiprecision::cpp_dec_float_100>());
			}

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

			BigFloat Modf_impl::operator ()(const BigFloat& x, BigFloat& ip) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::modf(x._detail().value, &ip._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Pow_impl
			//
			//////////////////////////////////////////////////

			BigInt Pow_impl::operator ()(const BigInt& x, const uint32 exp) const
			{
				BigInt result;
				result._detail().value = boost::multiprecision::pow(x._detail().value, exp);
				return result;
			}

			BigFloat Pow_impl::operator ()(const BigFloat& x, const BigFloat& exp) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::pow(x._detail().value, exp._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Sign_impl
			//
			//////////////////////////////////////////////////

			int32 Sign_impl::operator ()(const BigInt& x) const
			{
				return x._detail().value.sign();
			}

			int32 Sign_impl::operator ()(const BigFloat& x) const
			{
				return x._detail().value.sign();
			}

			//////////////////////////////////////////////////
			//
			//	ToRadians_impl
			//
			//////////////////////////////////////////////////

			BigFloat ToRadians_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat ToRadians_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = (x._detail().value * cRad);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	ToDegrees_impl
			//
			//////////////////////////////////////////////////

			BigFloat ToDegrees_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat ToDegrees_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = (x._detail().value * cDeg);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Abs_impl
			//
			//////////////////////////////////////////////////

			BigInt Abs_impl::operator ()(const BigInt& x) const
			{
				return x.abs();
			}

			BigFloat Abs_impl::operator ()(const BigFloat& x) const
			{
				return x.abs();
			}

			//////////////////////////////////////////////////
			//
			//	AbsDiff_impl
			//
			//////////////////////////////////////////////////

			BigInt AbsDiff_impl::operator ()(const BigInt& x, const BigInt& y) const
			{
				return (x - y).abs();
			}

			BigFloat AbsDiff_impl::operator ()(const BigFloat& x, const BigFloat& y) const
			{
				return (x - y).abs();
			}

			//////////////////////////////////////////////////
			//
			//	Square_impl
			//
			//////////////////////////////////////////////////

			BigInt Square_impl::operator ()(const BigInt& x) const
			{
				return (x * x);
			}

			BigFloat Square_impl::operator ()(const BigFloat& x) const
			{
				return (x * x);
			}

			//////////////////////////////////////////////////
			//
			//	Exp_impl
			//
			//////////////////////////////////////////////////

			BigFloat Exp_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat Exp_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::exp(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Exp2_impl
			//
			//////////////////////////////////////////////////

			BigFloat Exp2_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat Exp2_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::exp2(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Rsqrt_impl
			//
			//////////////////////////////////////////////////

			BigFloat Rsqrt_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat Rsqrt_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = (1 / boost::multiprecision::sqrt(x._detail().value));
				return result;
			}

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

			//////////////////////////////////////////////////
			//
			//	Ceil_impl
			//
			//////////////////////////////////////////////////

			BigFloat Ceil_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::ceil(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Floor_impl
			//
			//////////////////////////////////////////////////

			BigFloat Floor_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::floor(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Round_impl
			//
			//////////////////////////////////////////////////

			BigFloat Round_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::round(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Trunc_impl
			//
			//////////////////////////////////////////////////

			BigFloat Trunc_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::trunc(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Max_impl
			//
			//////////////////////////////////////////////////

			const BigInt& Max_impl::operator ()(const BigInt& x, const BigInt& y) const
			{
				return std::max(x, y);
			}

			const BigFloat& Max_impl::operator ()(const BigFloat& x, const BigFloat& y) const
			{
				return std::max(x, y);
			}

			//////////////////////////////////////////////////
			//
			//	Min_impl
			//
			//////////////////////////////////////////////////

			const BigInt& Min_impl::operator ()(const BigInt& x, const BigInt& y) const
			{
				return std::min(x, y);
			}

			const BigFloat& Min_impl::operator ()(const BigFloat& x, const BigFloat& y) const
			{
				return std::min(x, y);
			}

			//////////////////////////////////////////////////
			//
			//	Clamp_impl
			//
			//////////////////////////////////////////////////

			const BigInt& Clamp_impl::operator ()(const BigInt& x, const BigInt& min, const BigInt& max) const
			{
				return s3d::Clamp(x, min, max);
			}

			const BigFloat& Clamp_impl::operator ()(const BigFloat& x, const BigFloat& min, const BigFloat& max) const
			{
				return s3d::Clamp(x, min, max);
			}

			//////////////////////////////////////////////////
			//
			//	Saturate_impl
			//
			//////////////////////////////////////////////////

			BigFloat Saturate_impl::operator ()(const BigFloat& x) const
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

			//////////////////////////////////////////////////
			//
			//	Acos_impl
			//
			//////////////////////////////////////////////////

			BigFloat Acos_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat Acos_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::acos(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Asin_impl
			//
			//////////////////////////////////////////////////
			
			BigFloat Asin_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat Asin_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::asin(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Atan_impl
			//
			//////////////////////////////////////////////////

			BigFloat Atan_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat Atan_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::atan(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Atan2_impl
			//
			//////////////////////////////////////////////////

			BigFloat Atan2_impl::operator ()(const BigInt& y, const BigInt& x) const
			{
				return operator()(BigFloat{ y }, BigFloat{ x });
			}

			BigFloat Atan2_impl::operator ()(const BigFloat& y, const BigFloat& x) const
			{
				if (x.isZero() && y.isZero())
				{
					return 0.0;
				}

				BigFloat result;
				result._detail().value = boost::multiprecision::atan2(y._detail().value, x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Cos_impl
			//
			//////////////////////////////////////////////////

			BigFloat Cos_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat Cos_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::cos(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Cosh_impl
			//
			//////////////////////////////////////////////////

			BigFloat Cosh_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat Cosh_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::cosh(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Sin_impl
			//
			//////////////////////////////////////////////////

			BigFloat Sin_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat Sin_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::sin(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Sinh_impl
			//
			//////////////////////////////////////////////////

			BigFloat Sinh_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat Sinh_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::sinh(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Tan_impl
			//
			//////////////////////////////////////////////////

			BigFloat Tan_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat Tan_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::tan(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Tanh_impl
			//
			//////////////////////////////////////////////////

			BigFloat Tanh_impl::operator ()(const BigInt& x) const
			{
				return operator()(BigFloat{ x });
			}

			BigFloat Tanh_impl::operator ()(const BigFloat& x) const
			{
				BigFloat result;
				result._detail().value = boost::multiprecision::tanh(x._detail().value);
				return result;
			}

			//////////////////////////////////////////////////
			//
			//	Normalize_impl
			//
			//////////////////////////////////////////////////

			BigFloat Normalize_impl::operator ()(const BigFloat& x) const
			{
				if (x.isZero())
				{
					return 0;
				}

				return 1;
			}

			//////////////////////////////////////////////////
			//
			//	Smoothstep_impl
			//
			//////////////////////////////////////////////////

			BigFloat Smoothstep_impl::operator ()(const BigFloat& min, const BigFloat& max, const BigFloat& t) const
			{
				if (t <= min)
				{
					return 0;
				}
				else if (max <= t)
				{
					return 1;
				}

				const BigFloat _t = ((t - min) / (max - min));

				return (_t * _t * (3 - 2 * _t));
			}

			BigFloat Smoothstep_impl::operator ()(const BigFloat& x) const
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

			//////////////////////////////////////////////////
			//
			//	GCD_impl
			//
			//////////////////////////////////////////////////
			
			BigInt GCD_impl::operator ()(const BigInt& a, const BigInt& b) const
			{
				return a.gcd(b);
			}

			//////////////////////////////////////////////////
			//
			//	LCM_impl
			//
			//////////////////////////////////////////////////
			
			BigInt LCM_impl::operator ()(const BigInt& a, const BigInt& b) const
			{
				return a.lcm(b);
			}
		}
	}
}
