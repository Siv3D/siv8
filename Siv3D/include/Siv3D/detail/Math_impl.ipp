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
			
			struct Fmod_impl
			{
				[[nodiscard]]
				auto operator ()(const Concept::Arithmetic auto x, const Concept::Arithmetic auto y) const noexcept
				{
					using R = CommonFloat_t<decltype(x), decltype(y)>;

					return std::fmod(static_cast<R>(x), static_cast<R>(y));
				}

				[[nodiscard]]
				float operator ()(const float x, const float y) const noexcept
				{
					return std::fmod(x, y);
				}

				[[nodiscard]]
				double operator ()(const double x, const double y) const noexcept
				{
					return std::fmod(x, y);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 x, const Float2 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 x, const Vec2 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 x, const Float3 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 x, const Vec3 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 x, const Float4 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z), std::fmod(x.w, y.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 x, const Vec4 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z), std::fmod(x.w, y.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x, const BigFloat& y) const;
			};

			//////////////////////////////////////////////////
			//
			//	Fraction_impl
			//
			//////////////////////////////////////////////////

			struct Fraction_impl
			{
				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return (x - std::floor(x));
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return (x - std::floor(x));
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)), (v.z - std::floor(v.z)) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)), (v.z - std::floor(v.z)) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)), (v.z - std::floor(v.z)), (v.w - std::floor(v.w)) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)), (v.z - std::floor(v.z)), (v.w - std::floor(v.w)) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			//////////////////////////////////////////////////
			//
			//	Frexp_impl
			//
			//////////////////////////////////////////////////

			struct Frexp_impl
			{
				[[nodiscard]]
				float operator ()(const float x, float& exp) const noexcept
				{
					int32 e;
					const auto t = std::frexp(x, &e);
					exp = static_cast<float>(e);
					return t;
				}

				[[nodiscard]]
				double operator ()(const double x, double& exp) const noexcept
				{
					int32 e;
					const auto t = std::frexp(x, &e);
					exp = static_cast<double>(e);
					return t;
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v, Float2& exp) const noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v, Vec2& exp) const noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v, Float3& exp) const noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y), operator ()(v.z, exp.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v, Vec3& exp) const noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y), operator ()(v.z, exp.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v, Float4& exp) const noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y), operator ()(v.z, exp.z), operator ()(v.w, exp.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v, Vec4& exp) const noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y), operator ()(v.z, exp.z), operator ()(v.w, exp.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x, int32& exp) const;
			};

			//////////////////////////////////////////////////
			//
			//	Ldexp_impl
			//
			//////////////////////////////////////////////////

			struct Ldexp_impl
			{
				[[nodiscard]]
				float operator ()(const float x, const float exp) const noexcept
				{
					return (x * std::exp2(exp));
				}

				[[nodiscard]]
				double operator ()(const double x, const double exp) const noexcept
				{
					return (x * std::exp2(exp));
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v, const Float2 exp) const noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v, const Vec2 exp) const noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v, const Float3 exp) const noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)), (v.z * std::exp2(exp.z)) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v, const Vec3 exp) const noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)), (v.z * std::exp2(exp.z)) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v, const Float4 exp) const noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)), (v.z * std::exp2(exp.z)), (v.w * std::exp2(exp.w)) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v, const Vec4 exp) const noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)), (v.z * std::exp2(exp.z)), (v.w * std::exp2(exp.w)) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x, const BigFloat& exp) const;
			};

			//////////////////////////////////////////////////
			//
			//	Log_impl
			//
			//////////////////////////////////////////////////

			struct Log_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::log(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::log(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::log(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::log(v.x), std::log(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::log(v.x), std::log(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::log(v.x), std::log(v.y), std::log(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::log(v.x), std::log(v.y), std::log(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::log(v.x), std::log(v.y), std::log(v.z), std::log(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::log(v.x), std::log(v.y), std::log(v.z), std::log(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			//////////////////////////////////////////////////
			//
			//	Log2_impl
			//
			//////////////////////////////////////////////////

			struct Log2_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::log2(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::log2(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::log2(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::log2(v.x), std::log2(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::log2(v.x), std::log2(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::log2(v.x), std::log2(v.y), std::log2(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::log2(v.x), std::log2(v.y), std::log2(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::log2(v.x), std::log2(v.y), std::log2(v.z), std::log2(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::log2(v.x), std::log2(v.y), std::log2(v.z), std::log2(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			//////////////////////////////////////////////////
			//
			//	Log10_impl
			//
			//////////////////////////////////////////////////

			struct Log10_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::log10(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::log10(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::log10(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::log10(v.x), std::log10(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::log10(v.x), std::log10(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::log10(v.x), std::log10(v.y), std::log10(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::log10(v.x), std::log10(v.y), std::log10(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::log10(v.x), std::log10(v.y), std::log10(v.z), std::log10(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::log10(v.x), std::log10(v.y), std::log10(v.z), std::log10(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

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

			struct Sqrt_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::sqrt(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::sqrt(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::sqrt(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::sqrt(v.x), std::sqrt(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::sqrt(v.x), std::sqrt(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z), std::sqrt(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z), std::sqrt(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};
		}
	}
}
