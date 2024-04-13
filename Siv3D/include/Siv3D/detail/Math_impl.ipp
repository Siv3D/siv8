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

			struct Modf_impl
			{
				[[nodiscard]]
				float operator ()(const float x, float& ip) const noexcept
				{
					return std::modf(x, &ip);
				}

				[[nodiscard]]
				double operator ()(const double x, double& ip) const noexcept
				{
					return std::modf(x, &ip);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v, Float2& ip) const noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v, Vec2& ip) const noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v, Float3& ip) const noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y), std::modf(v.z, &ip.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v, Vec3& ip) const noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y), std::modf(v.z, &ip.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v, Float4& ip) const noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y), std::modf(v.z, &ip.z), std::modf(v.w, &ip.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v, Vec4& ip) const noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y), std::modf(v.z, &ip.z), std::modf(v.w, &ip.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x, BigFloat& ip) const;
			};

			//////////////////////////////////////////////////
			//
			//	Pow_impl
			//
			//////////////////////////////////////////////////

			struct Pow_impl
			{
				[[nodiscard]]
				auto operator ()(const Concept::Arithmetic auto x, const Concept::Arithmetic auto exp) const noexcept
				{
					return std::pow(x, exp);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v, const Concept::Arithmetic auto exp) const noexcept
				{
					return{ std::pow(v.x, exp), std::pow(v.y, exp) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v, const Concept::Arithmetic auto exp) const noexcept
				{
					return{ std::pow(v.x, exp), std::pow(v.y, exp) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v, const Concept::Arithmetic auto exp) const noexcept
				{
					return{ std::pow(v.x, exp), std::pow(v.y, exp), std::pow(v.z, exp) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v, const Concept::Arithmetic auto exp) const noexcept
				{
					return{ std::pow(v.x, exp), std::pow(v.y, exp), std::pow(v.z, exp) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v, const Concept::Arithmetic auto exp) const noexcept
				{
					return{ std::pow(v.x, exp), std::pow(v.y, exp), std::pow(v.z, exp), std::pow(v.w, exp) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v, const Concept::Arithmetic auto exp) const noexcept
				{
					return{ std::pow(v.x, exp), std::pow(v.y, exp), std::pow(v.z, exp), std::pow(v.w, exp) };
				}

				[[nodiscard]]
				BigInt operator ()(const BigInt& x, uint32 exp) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x, const BigFloat& exp) const;
			};

			//////////////////////////////////////////////////
			//
			//	Sign_impl
			//
			//////////////////////////////////////////////////

			struct Sign_impl
			{
				[[nodiscard]]
				constexpr int32 operator ()(const Concept::Arithmetic auto x) const noexcept
				{
					if (x < 0)
					{
						return -1;
					}
					else if (0 < x)
					{
						return 1;
					}
					else
					{
						return 0;
					}
				}

				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y) };
				}

				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y) };
				}

				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z) };
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z) };
				}

				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z), operator ()(v.w) };
				}

				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z), operator ()(v.w) };
				}

				[[nodiscard]]
				int32 operator ()(const BigInt& x) const;

				[[nodiscard]]
				int32 operator ()(const BigFloat& x) const;
			};

			//////////////////////////////////////////////////
			//
			//	ToRadians_impl
			//
			//////////////////////////////////////////////////

			struct ToRadians_impl
			{
				[[nodiscard]]
				constexpr double operator ()(const Concept::Integral auto x) const noexcept
				{
					return (static_cast<double>(x) * (Math::Pi / 180.0));
				}

				[[nodiscard]]
				constexpr float operator ()(const float x) const noexcept
				{
					return (x * (Math::PiF / 180.0f));
				}

				[[nodiscard]]
				constexpr double operator ()(const double x) const noexcept
				{
					return (x * (Math::Pi / 180.0));
				}

				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 v) const noexcept
				{
					const float rad = (Math::PiF / 180.0f);
					return (v * rad);
				}

				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 v) const noexcept
				{
					const double rad = (Math::Pi / 180.0);
					return (v * rad);
				}

				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 v) const noexcept
				{
					const float rad = (Math::PiF / 180.0f);
					return (v * rad);
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 v) const noexcept
				{
					const double rad = (Math::Pi / 180.0);
					return (v * rad);
				}

				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 v) const noexcept
				{
					const float rad = (Math::PiF / 180.0f);
					return (v * rad);
				}

				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 v) const noexcept
				{
					const double rad = (Math::Pi / 180.0);
					return (v * rad);
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			//////////////////////////////////////////////////
			//
			//	ToDegrees_impl
			//
			//////////////////////////////////////////////////

			struct ToDegrees_impl
			{
				[[nodiscard]]
				constexpr double operator ()(const Concept::Integral auto x) const noexcept
				{
					return (static_cast<double>(x) * (180.0 / Math::Pi));
				}

				[[nodiscard]]
				constexpr float operator ()(const float x) const noexcept
				{
					return (x * (180.0f / Math::PiF));
				}

				[[nodiscard]]
				constexpr double operator ()(const double x) const noexcept
				{
					return (x * (180.0 / Math::Pi));
				}

				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 v) const noexcept
				{
					const float deg = (180.0f / Math::PiF);
					return (v * deg);
				}

				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 v) const noexcept
				{
					const double deg = (180.0 / Math::Pi);
					return (v * deg);
				}

				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 v) const noexcept
				{
					const float deg = (180.0f / Math::PiF);
					return (v * deg);
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 v) const noexcept
				{
					const double deg = (180.0 / Math::Pi);
					return (v * deg);
				}

				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 v) const noexcept
				{
					const float deg = (180.0f / Math::PiF);
					return (v * deg);
				}

				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 v) const noexcept
				{
					const double deg = (180.0 / Math::Pi);
					return (v * deg);
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			//////////////////////////////////////////////////
			//
			//	Abs_impl
			//
			//////////////////////////////////////////////////

			struct Abs_impl
			{
				[[nodiscard]]
				constexpr auto operator ()(const Concept::SignedIntegral auto x) const noexcept
				{
					return s3d::Abs(x);
				}

				[[nodiscard]]
				constexpr float operator ()(const float x) const noexcept
				{
					return s3d::Abs(x);
				}

				[[nodiscard]]
				constexpr double operator ()(const double x) const noexcept
				{
					return s3d::Abs(x);
				}

				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 v) const noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y) };
				}

				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y) };
				}

				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 v) const noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y), s3d::Abs(v.z) };
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y), s3d::Abs(v.z) };
				}

				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 v) const noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y), s3d::Abs(v.z), s3d::Abs(v.w) };
				}

				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y), s3d::Abs(v.z), s3d::Abs(v.w) };
				}

				[[nodiscard]]
				BigInt operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			//////////////////////////////////////////////////
			//
			//	AbsDiff_impl
			//
			//////////////////////////////////////////////////

			struct AbsDiff_impl
			{
				[[nodiscard]]
				constexpr auto operator ()(const Concept::Arithmetic auto x, const Concept::Arithmetic auto y) const noexcept
				{
					using T1 = decltype(x);
					using T2 = decltype(y);

					if constexpr (std::conjunction_v<std::is_integral<T1>, std::is_integral<T2>>)
					{
						using U = std::make_unsigned_t<std::common_type_t<T1, T2>>;
						return (x > y) ? (static_cast<U>(x) - static_cast<U>(y))
							: (static_cast<U>(y) - static_cast<U>(x));
					}
					else
					{
						return Abs(x - y);
					}
				}

				[[nodiscard]]
				constexpr float operator ()(const float x, const float y) const noexcept
				{
					return Abs(x - y);
				}

				[[nodiscard]]
				constexpr double operator ()(const double x, const double y) const noexcept
				{
					return Abs(x - y);
				}

				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 a, const Float2 b) const noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y) };
				}

				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 a, const Vec2 b) const noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y) };
				}

				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 a, const Float3 b) const noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y), Abs(a.z - b.z) };
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 a, const Vec3 b) const noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y), Abs(a.z - b.z) };
				}

				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 a, const Float4 b) const noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y), Abs(a.z - b.z), Abs(a.w - b.w) };
				}

				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 a, const Vec4 b) const noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y), Abs(a.z - b.z), Abs(a.w - b.w) };
				}

				[[nodiscard]]
				BigInt operator ()(const BigInt& x, const BigInt& y) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x, const BigFloat& y) const;
			};

			//////////////////////////////////////////////////
			//
			//	Square_impl
			//
			//////////////////////////////////////////////////

			struct Square_impl
			{
				[[nodiscard]]
				constexpr auto operator ()(const Concept::Arithmetic auto x) const noexcept
				{
					return (x * x);
				}

				[[nodiscard]]
				constexpr float operator ()(const float x) const noexcept
				{
					return (x * x);
				}

				[[nodiscard]]
				constexpr double operator ()(const double x) const noexcept
				{
					return (x * x);
				}

				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 v) const noexcept
				{
					return{ (v.x * v.x), (v.y * v.y) };
				}

				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ (v.x * v.x), (v.y * v.y) };
				}

				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 v) const noexcept
				{
					return{ (v.x * v.x), (v.y * v.y), (v.z * v.z) };
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ (v.x * v.x), (v.y * v.y), (v.z * v.z) };
				}

				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 v) const noexcept
				{
					return{ (v.x * v.x), (v.y * v.y), (v.z * v.z), (v.w * v.w) };
				}

				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ (v.x * v.x), (v.y * v.y), (v.z * v.z), (v.w * v.w) };
				}

				[[nodiscard]]
				BigInt operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			//////////////////////////////////////////////////
			//
			//	Exp_impl
			//
			//////////////////////////////////////////////////

			struct Exp_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::exp(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::exp(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::exp(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::exp(v.x), std::exp(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::exp(v.x), std::exp(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::exp(v.x), std::exp(v.y), std::exp(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::exp(v.x), std::exp(v.y), std::exp(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::exp(v.x), std::exp(v.y), std::exp(v.z), std::exp(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::exp(v.x), std::exp(v.y), std::exp(v.z), std::exp(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			//////////////////////////////////////////////////
			//
			//	Exp2_impl
			//
			//////////////////////////////////////////////////

			struct Exp2_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::exp2(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::exp2(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::exp2(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::exp2(v.x), std::exp2(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::exp2(v.x), std::exp2(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::exp2(v.x), std::exp2(v.y), std::exp2(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::exp2(v.x), std::exp2(v.y), std::exp2(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::exp2(v.x), std::exp2(v.y), std::exp2(v.z), std::exp2(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::exp2(v.x), std::exp2(v.y), std::exp2(v.z), std::exp2(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			//////////////////////////////////////////////////
			//
			//	Rsqrt_impl
			//
			//////////////////////////////////////////////////

			struct Rsqrt_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return (1.0 / std::sqrt(static_cast<double>(x)));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return (1.0f / std::sqrt(x));
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return (1.0 / std::sqrt(x));
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ (1.0f / std::sqrt(v.x)), (1.0f / std::sqrt(v.y)) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ (1.0 / std::sqrt(v.x)), (1.0 / std::sqrt(v.y)) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ (1.0f / std::sqrt(v.x)), (1.0f / std::sqrt(v.y)), (1.0f / std::sqrt(v.z)) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ (1.0 / std::sqrt(v.x)), (1.0 / std::sqrt(v.y)), (1.0 / std::sqrt(v.z)) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ (1.0f / std::sqrt(v.x)), (1.0f / std::sqrt(v.y)), (1.0f / std::sqrt(v.z)), (1.0f / std::sqrt(v.w)) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ (1.0 / std::sqrt(v.x)), (1.0 / std::sqrt(v.y)), (1.0 / std::sqrt(v.z)), (1.0 / std::sqrt(v.w)) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

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

		//////////////////////////////////////////////////
		//
		//	Ceil_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Floor_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Round_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Trunc_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Max_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Min_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Clamp_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Saturate_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Acos_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Atan_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Atan2_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Cos_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Cosh_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Sin_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Sinh_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Tan_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Tanh_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Dot_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Cross_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Normalize_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Smoothstep_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	NormalizeAngle_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	GCD_impl
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	LCM_impl
		//
		//////////////////////////////////////////////////

	}
}
