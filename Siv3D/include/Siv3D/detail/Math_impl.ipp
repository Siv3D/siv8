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
	namespace Math
	{
		namespace detail
		{
			////////////////////////////////////////////////////////////////
			//
			//	Fmod_impl
			//
			////////////////////////////////////////////////////////////////
			
			struct Fmod_impl
			{
				/// @brief 剰余を返します。
				/// @param x 被除数
				/// @param y 除数
				/// @return x を y で割った剰余
				[[nodiscard]]
				static double operator ()(const Concept::Arithmetic auto x, const Concept::Arithmetic auto y) noexcept
				{
					return std::fmod(static_cast<double>(x), static_cast<double>(y));
				}

				/// @brief 剰余を返します。
				/// @param x 被除数
				/// @param y 除数
				/// @return x を y で割った剰余
				[[nodiscard]]
				static float operator ()(const float x, const float y) noexcept
				{
					return std::fmod(x, y);
				}

				/// @brief 剰余を返します。
				/// @param x 被除数
				/// @param y 除数
				/// @return x を y で割った剰余
				[[nodiscard]]
				static double operator ()(const double x, const double y) noexcept
				{
					return std::fmod(x, y);
				}

				/// @brief ベクトルの各成分ごとの剰余の結果を返します。
				/// @param x 被除数ベクトル
				/// @param y 除数ベクトル
				/// @return 各成分について x を y で割った剰余
				[[nodiscard]]
				static Float2 operator ()(const Float2 x, const Float2 y) noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y) };
				}

				/// @brief ベクトルの各成分ごとの剰余の結果を返します。
				/// @param x 被除数ベクトル
				/// @param y 除数ベクトル
				/// @return 各成分について x を y で割った剰余
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 x, const Vec2 y) noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y) };
				}

				/// @brief ベクトルの各成分ごとの剰余の結果を返します。
				/// @param x 被除数ベクトル
				/// @param y 除数ベクトル
				/// @return 各成分について x を y で割った剰余
				[[nodiscard]]
				static Float3 operator ()(const Float3 x, const Float3 y) noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z) };
				}

				/// @brief ベクトルの各成分ごとの剰余の結果を返します。
				/// @param x 被除数ベクトル
				/// @param y 除数ベクトル
				/// @return 各成分について x を y で割った剰余
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 x, const Vec3 y) noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z) };
				}

				/// @brief ベクトルの各成分ごとの剰余の結果を返します。
				/// @param x 被除数ベクトル
				/// @param y 除数ベクトル
				/// @return 各成分について x を y で割った剰余
				[[nodiscard]]
				static Float4 operator ()(const Float4 x, const Float4 y) noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z), std::fmod(x.w, y.w) };
				}

				/// @brief ベクトルの各成分ごとの剰余の結果を返します。
				/// @param x 被除数ベクトル
				/// @param y 除数ベクトル
				/// @return 各成分について x を y で割った剰余
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 x, const Vec4 y) noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z), std::fmod(x.w, y.w) };
				}

				/// @brief 剰余を返します。
				/// @param x 被除数
				/// @param y 除数
				/// @return x を y で割った剰余
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x, const BigFloat& y);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Fraction_impl
			//
			////////////////////////////////////////////////////////////////

			struct Fraction_impl
			{
				/// @brief 小数部を返します。
				/// @param x 入力値
				/// @return x の小数部
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return (x - std::floor(x));
				}

				/// @brief 小数部を返します。
				/// @param x 入力値
				/// @return x の小数部
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return (x - std::floor(x));
				}

				/// @brief ベクトルの各成分の小数部を返します。
				/// @param v 入力ベクトル
				/// @return 各成分の小数部
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)) };
				}

				/// @brief ベクトルの各成分の小数部を返します。
				/// @param v 入力ベクトル
				/// @return 各成分の小数部
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)) };
				}

				/// @brief ベクトルの各成分の小数部を返します。
				/// @param v 入力ベクトル
				/// @return 各成分の小数部
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)), (v.z - std::floor(v.z)) };
				}

				/// @brief ベクトルの各成分の小数部を返します。
				/// @param v 入力ベクトル
				/// @return 各成分の小数部
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)), (v.z - std::floor(v.z)) };
				}

				/// @brief ベクトルの各成分の小数部を返します。
				/// @param v 入力ベクトル
				/// @return 各成分の小数部
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)), (v.z - std::floor(v.z)), (v.w - std::floor(v.w)) };
				}

				/// @brief ベクトルの各成分の小数部を返します。
				/// @param v 入力ベクトル
				/// @return 各成分の小数部
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)), (v.z - std::floor(v.z)), (v.w - std::floor(v.w)) };
				}

				/// @brief 小数部を返します。
				/// @param x 入力値
				/// @return x の小数部
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Frexp_impl
			//
			////////////////////////////////////////////////////////////////

			struct Frexp_impl
			{
				/// @brief 値を仮数と指数部に分解します。
				/// @param x 入力値
				/// @param exp 指数部を格納する参照
				/// @return 仮数部 (0.5 以上 1.0 未満)
				[[nodiscard]]
				static float operator ()(const float x, float& exp) noexcept
				{
					int32 e;
					const auto t = std::frexp(x, &e);
					exp = static_cast<float>(e);
					return t;
				}

				/// @brief 値を仮数と指数部に分解します。
				/// @param x 入力値
				/// @param exp 指数部を格納する参照
				/// @return 仮数部 (0.5 以上 1.0 未満)
				[[nodiscard]]
				static double operator ()(const double x, double& exp) noexcept
				{
					int32 e;
					const auto t = std::frexp(x, &e);
					exp = static_cast<double>(e);
					return t;
				}

				/// @brief ベクトルの各成分を仮数と指数部に分解します。
				/// @param v 入力ベクトル
				/// @param exp 指数部を格納するベクトルの参照
				/// @return 仮数部ベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v, Float2& exp) noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y) };
				}

				/// @brief ベクトルの各成分を仮数と指数部に分解します。
				/// @param v 入力ベクトル
				/// @param exp 指数部を格納するベクトルの参照
				/// @return 仮数部ベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v, Vec2& exp) noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y) };
				}

				/// @brief ベクトルの各成分を仮数と指数部に分解します。
				/// @param v 入力ベクトル
				/// @param exp 指数部を格納するベクトルの参照
				/// @return 仮数部ベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v, Float3& exp) noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y), operator ()(v.z, exp.z) };
				}

				/// @brief ベクトルの各成分を仮数と指数部に分解します。
				/// @param v 入力ベクトル
				/// @param exp 指数部を格納するベクトルの参照
				/// @return 仮数部ベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v, Vec3& exp) noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y), operator ()(v.z, exp.z) };
				}

				/// @brief ベクトルの各成分を仮数と指数部に分解します。
				/// @param v 入力ベクトル
				/// @param exp 指数部を格納するベクトルの参照
				/// @return 仮数部ベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v, Float4& exp) noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y), operator ()(v.z, exp.z), operator ()(v.w, exp.w) };
				}

				/// @brief ベクトルの各成分を仮数と指数部に分解します。
				/// @param v 入力ベクトル
				/// @param exp 指数部を格納するベクトルの参照
				/// @return 仮数部ベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v, Vec4& exp) noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y), operator ()(v.z, exp.z), operator ()(v.w, exp.w) };
				}

				/// @brief 値を仮数と指数部に分解します。
				/// @param x 入力値
				/// @param exp 指数部を格納する参照
				/// @return 仮数部
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x, int32& exp);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Ldexp_impl
			//
			////////////////////////////////////////////////////////////////

			struct Ldexp_impl
			{
				/// @brief 仮数と指数から値を構成します。
				/// @param x 仮数
				/// @param exp 指数
				/// @return x * 2^exp
				[[nodiscard]]
				static float operator ()(const float x, const float exp) noexcept
				{
					return (x * std::exp2(exp));
				}

				/// @brief 仮数と指数から値を構成します。
				/// @param x 仮数
				/// @param exp 指数
				/// @return x * 2^exp
				[[nodiscard]]
				static double operator ()(const double x, const double exp) noexcept
				{
					return (x * std::exp2(exp));
				}

				/// @brief ベクトルの各成分に対して仮数と指数から値を構成します。
				/// @param v 仮数ベクトル
				/// @param exp 指数ベクトル
				/// @return 各成分に対して v * 2^exp を適用したベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v, const Float2 exp) noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)) };
				}

				/// @brief ベクトルの各成分に対して仮数と指数から値を構成します。
				/// @param v 仮数ベクトル
				/// @param exp 指数ベクトル
				/// @return 各成分に対して v * 2^exp を適用したベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v, const Vec2 exp) noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)) };
				}

				/// @brief ベクトルの各成分に対して仮数と指数から値を構成します。
				/// @param v 仮数ベクトル
				/// @param exp 指数ベクトル
				/// @return 各成分に対して v * 2^exp を適用したベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v, const Float3 exp) noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)), (v.z * std::exp2(exp.z)) };
				}

				/// @brief ベクトルの各成分に対して仮数と指数から値を構成します。
				/// @param v 仮数ベクトル
				/// @param exp 指数ベクトル
				/// @return 各成分に対して v * 2^exp を適用したベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v, const Vec3 exp) noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)), (v.z * std::exp2(exp.z)) };
				}

				/// @brief ベクトルの各成分に対して仮数と指数から値を構成します。
				/// @param v 仮数ベクトル
				/// @param exp 指数ベクトル
				/// @return 各成分に対して v * 2^exp を適用したベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v, const Float4 exp) noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)), (v.z * std::exp2(exp.z)), (v.w * std::exp2(exp.w)) };
				}

				/// @brief ベクトルの各成分に対して仮数と指数から値を構成します。
				/// @param v 仮数ベクトル
				/// @param exp 指数ベクトル
				/// @return 各成分に対して v * 2^exp を適用したベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v, const Vec4 exp) noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)), (v.z * std::exp2(exp.z)), (v.w * std::exp2(exp.w)) };
				}

				/// @brief 仮数と指数から値を構成します。
				/// @param x 仮数
				/// @param exp 指数
				/// @return x * 2^exp
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x, const BigFloat& exp);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Log_impl
			//
			////////////////////////////////////////////////////////////////

			struct Log_impl
			{
				/// @brief 自然対数を返します。
				/// @param x 入力値
				/// @return ln(x)
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::log(static_cast<double>(x));
				}

				/// @brief 自然対数を返します。
				/// @param x 入力値
				/// @return ln(x)
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::log(x);
				}

				/// @brief 自然対数を返します。
				/// @param x 入力値
				/// @return ln(x)
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::log(x);
				}

				/// @brief ベクトルの各成分の自然対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して ln(v) を適用したベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::log(v.x), std::log(v.y) };
				}

				/// @brief ベクトルの各成分の自然対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して ln(v) を適用したベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::log(v.x), std::log(v.y) };
				}

				/// @brief ベクトルの各成分の自然対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して ln(v) を適用したベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::log(v.x), std::log(v.y), std::log(v.z) };
				}

				/// @brief ベクトルの各成分の自然対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して ln(v) を適用したベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::log(v.x), std::log(v.y), std::log(v.z) };
				}

				/// @brief ベクトルの各成分の自然対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して ln(v) を適用したベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::log(v.x), std::log(v.y), std::log(v.z), std::log(v.w) };
				}

				/// @brief ベクトルの各成分の自然対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して ln(v) を適用したベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::log(v.x), std::log(v.y), std::log(v.z), std::log(v.w) };
				}

				/// @brief 自然対数を返します。
				/// @param x 入力値
				/// @return ln(x)
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Log2_impl
			//
			////////////////////////////////////////////////////////////////

			struct Log2_impl
			{
				/// @brief 2 を底とする対数を返します。
				/// @param x 入力値
				/// @return log2(x)
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::log2(static_cast<double>(x));
				}

				/// @brief 2 を底とする対数を返します。
				/// @param x 入力値
				/// @return log2(x)
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::log2(x);
				}

				/// @brief 2 を底とする対数を返します。
				/// @param x 入力値
				/// @return log2(x)
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::log2(x);
				}

				/// @brief ベクトルの各成分の 2 を底とする対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して log2(v) を適用したベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::log2(v.x), std::log2(v.y) };
				}

				/// @brief ベクトルの各成分の 2 を底とする対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して log2(v) を適用したベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::log2(v.x), std::log2(v.y) };
				}

				/// @brief ベクトルの各成分の 2 を底とする対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して log2(v) を適用したベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::log2(v.x), std::log2(v.y), std::log2(v.z) };
				}

				/// @brief ベクトルの各成分の 2 を底とする対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して log2(v) を適用したベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::log2(v.x), std::log2(v.y), std::log2(v.z) };
				}

				/// @brief ベクトルの各成分の 2 を底とする対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して log2(v) を適用したベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::log2(v.x), std::log2(v.y), std::log2(v.z), std::log2(v.w) };
				}

				/// @brief ベクトルの各成分の 2 を底とする対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して log2(v) を適用したベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::log2(v.x), std::log2(v.y), std::log2(v.z), std::log2(v.w) };
				}

				/// @brief 2 を底とする対数を返します。
				/// @param x 入力値
				/// @return log2(x)
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Log10_impl
			//
			////////////////////////////////////////////////////////////////

			struct Log10_impl
			{
				/// @brief 10 を底とする対数を返します。
				/// @param x 入力値
				/// @return log10(x)
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::log10(static_cast<double>(x));
				}

				/// @brief 10 を底とする対数を返します。
				/// @param x 入力値
				/// @return log10(x)
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::log10(x);
				}

				/// @brief 10 を底とする対数を返します。
				/// @param x 入力値
				/// @return log10(x)
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::log10(x);
				}

				/// @brief ベクトルの各成分の 10 を底とする対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して log10(v) を適用したベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::log10(v.x), std::log10(v.y) };
				}

				/// @brief ベクトルの各成分の 10 を底とする対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して log10(v) を適用したベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::log10(v.x), std::log10(v.y) };
				}

				/// @brief ベクトルの各成分の 10 を底とする対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して log10(v) を適用したベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::log10(v.x), std::log10(v.y), std::log10(v.z) };
				}

				/// @brief ベクトルの各成分の 10 を底とする対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して log10(v) を適用したベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::log10(v.x), std::log10(v.y), std::log10(v.z) };
				}

				/// @brief ベクトルの各成分の 10 を底とする対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して log10(v) を適用したベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::log10(v.x), std::log10(v.y), std::log10(v.z), std::log10(v.w) };
				}

				/// @brief ベクトルの各成分の 10 を底とする対数を返します。
				/// @param v 入力ベクトル
				/// @return 各成分に対して log10(v) を適用したベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::log10(v.x), std::log10(v.y), std::log10(v.z), std::log10(v.w) };
				}

				/// @brief 10 を底とする対数を返します。
				/// @param x 入力値
				/// @return log10(x)
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Modf_impl
			//
			////////////////////////////////////////////////////////////////

			struct Modf_impl
			{
				/// @brief 値を整数部と小数部に分解します。
				/// @param x 入力値
				/// @param ip 整数部を格納する参照
				/// @return 小数部
				[[nodiscard]]
				static float operator ()(const float x, float& ip) noexcept
				{
					return std::modf(x, &ip);
				}

				/// @brief 値を整数部と小数部に分解します。
				/// @param x 入力値
				/// @param ip 整数部を格納する参照
				/// @return 小数部
				[[nodiscard]]
				static double operator ()(const double x, double& ip) noexcept
				{
					return std::modf(x, &ip);
				}

				/// @brief ベクトルの各成分を整数部と小数部に分解します。
				/// @param v 入力ベクトル
				/// @param ip 整数部を格納するベクトルの参照
				/// @return 小数部ベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v, Float2& ip) noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y) };
				}

				/// @brief ベクトルの各成分を整数部と小数部に分解します。
				/// @param v 入力ベクトル
				/// @param ip 整数部を格納するベクトルの参照
				/// @return 小数部ベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v, Vec2& ip) noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y) };
				}

				/// @brief ベクトルの各成分を整数部と小数部に分解します。
				/// @param v 入力ベクトル
				/// @param ip 整数部を格納するベクトルの参照
				/// @return 小数部ベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v, Float3& ip) noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y), std::modf(v.z, &ip.z) };
				}

				/// @brief ベクトルの各成分を整数部と小数部に分解します。
				/// @param v 入力ベクトル
				/// @param ip 整数部を格納するベクトルの参照
				/// @return 小数部ベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v, Vec3& ip) noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y), std::modf(v.z, &ip.z) };
				}

				/// @brief ベクトルの各成分を整数部と小数部に分解します。
				/// @param v 入力ベクトル
				/// @param ip 整数部を格納するベクトルの参照
				/// @return 小数部ベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v, Float4& ip) noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y), std::modf(v.z, &ip.z), std::modf(v.w, &ip.w) };
				}

				/// @brief ベクトルの各成分を整数部と小数部に分解します。
				/// @param v 入力ベクトル
				/// @param ip 整数部を格納するベクトルの参照
				/// @return 小数部ベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v, Vec4& ip) noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y), std::modf(v.z, &ip.z), std::modf(v.w, &ip.w) };
				}

				/// @brief 値を整数部と小数部に分解します。
				/// @param x 入力値
				/// @param ip 整数部を格納する参照
				/// @return 小数部
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x, BigFloat& ip);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Pow_impl
			//
			////////////////////////////////////////////////////////////////

			struct Pow_impl
			{
				/// @brief べき乗を返します。
				/// @param x 底
				/// @param exp 指数
				/// @return x^exp
				[[nodiscard]]
				static auto operator ()(const Concept::Arithmetic auto x, const Concept::Arithmetic auto exp) noexcept
				{
					return std::pow(x, exp);
				}

				/// @brief ベクトルの各成分ごとのべき乗を返します。
				/// @param v ベクトル
				/// @param exp 指数
				/// @return 各成分について v^exp を適用したベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v, const Concept::Arithmetic auto exp) noexcept
				{
					return{ std::pow(v.x, exp), std::pow(v.y, exp) };
				}

				/// @brief ベクトルの各成分ごとのべき乗を返します。
				/// @param v ベクトル
				/// @param exp 指数
				/// @return 各成分について v^exp を適用したベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v, const Concept::Arithmetic auto exp) noexcept
				{
					return{ std::pow(v.x, exp), std::pow(v.y, exp) };
				}

				/// @brief ベクトルの各成分ごとのべき乗を返します。
				/// @param v ベクトル
				/// @param exp 指数
				/// @return 各成分について v^exp を適用したベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v, const Concept::Arithmetic auto exp) noexcept
				{
					return{ std::pow(v.x, exp), std::pow(v.y, exp), std::pow(v.z, exp) };
				}

				/// @brief ベクトルの各成分ごとのべき乗を返します。
				/// @param v ベクトル
				/// @param exp 指数
				/// @return 各成分について v^exp を適用したベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v, const Concept::Arithmetic auto exp) noexcept
				{
					return{ std::pow(v.x, exp), std::pow(v.y, exp), std::pow(v.z, exp) };
				}

				/// @brief ベクトルの各成分ごとのべき乗を返します。
				/// @param v ベクトル
				/// @param exp 指数
				/// @return 各成分について v^exp を適用したベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v, const Concept::Arithmetic auto exp) noexcept
				{
					return{ std::pow(v.x, exp), std::pow(v.y, exp), std::pow(v.z, exp), std::pow(v.w, exp) };
				}

				/// @brief ベクトルの各成分ごとのべき乗を返します。
				/// @param v ベクトル
				/// @param exp 指数
				/// @return 各成分について v^exp を適用したベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v, const Concept::Arithmetic auto exp) noexcept
				{
					return{ std::pow(v.x, exp), std::pow(v.y, exp), std::pow(v.z, exp), std::pow(v.w, exp) };
				}

				/// @brief べき乗を返します。
				/// @param x 底
				/// @param exp 指数
				/// @return x^exp
				[[nodiscard]]
				static BigInt operator ()(const BigInt& x, uint32 exp);

				/// @brief べき乗を返します。
				/// @param x 底
				/// @param exp 指数
				/// @return x^exp
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x, const BigFloat& exp);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Sign_impl
			//
			////////////////////////////////////////////////////////////////

			struct Sign_impl
			{
				/// @brief 符号を返します。
				/// @param x 入力値
				/// @return x の符号 (-1, 0, 1)
				[[nodiscard]]
				static constexpr int32 operator ()(const Concept::Arithmetic auto x) noexcept
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

				/// @brief ベクトルの各成分の符号を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の符号
				[[nodiscard]]
				static constexpr Float2 operator ()(const Float2 v) noexcept
				{
					return{ operator ()(v.x), operator ()(v.y) };
				}

				/// @brief ベクトルの各成分の符号を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の符号
				[[nodiscard]]
				static constexpr Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ operator ()(v.x), operator ()(v.y) };
				}

				/// @brief ベクトルの各成分の符号を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の符号
				[[nodiscard]]
				static constexpr Float3 operator ()(const Float3 v) noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z) };
				}

				/// @brief ベクトルの各成分の符号を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の符号
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z) };
				}

				/// @brief ベクトルの各成分の符号を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の符号
				[[nodiscard]]
				static constexpr Float4 operator ()(const Float4 v) noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z), operator ()(v.w) };
				}

				/// @brief ベクトルの各成分の符号を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の符号
				[[nodiscard]]
				static constexpr Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z), operator ()(v.w) };
				}

				/// @brief 符号を返します。
				/// @param x 入力値
				/// @return x の符号 (-1, 0, 1)
				[[nodiscard]]
				static int32 operator ()(const BigInt& x);

				/// @brief 符号を返します。
				/// @param x 入力値
				/// @return x の符号 (-1, 0, 1)
				[[nodiscard]]
				static int32 operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	ToRadians_impl
			//
			////////////////////////////////////////////////////////////////

			struct ToRadians_impl
			{
				/// @brief 度数からラジアンに変換します。
				/// @param x 度数の値
				/// @return ラジアン値
				[[nodiscard]]
				static constexpr double operator ()(const Concept::Integral auto x) noexcept
				{
					return (static_cast<double>(x) * (Math::Pi / 180.0));
				}

				/// @brief 度数からラジアンに変換します。
				/// @param x 度数の値
				/// @return ラジアン値
				[[nodiscard]]
				static constexpr float operator ()(const float x) noexcept
				{
					return (x * (Math::PiF / 180.0f));
				}

				/// @brief 度数からラジアンに変換します。
				/// @param x 度数の値
				/// @return ラジアン値
				[[nodiscard]]
				static constexpr double operator ()(const double x) noexcept
				{
					return (x * (Math::Pi / 180.0));
				}

				/// @brief ベクトルの各成分を度数からラジアンに変換します。
				/// @param v 度数の値を持つベクトル
				/// @return ラジアン値のベクトル
				[[nodiscard]]
				static constexpr Float2 operator ()(const Float2 v) noexcept
				{
					const float rad = (Math::PiF / 180.0f);
					return (v * rad);
				}

				/// @brief ベクトルの各成分を度数からラジアンに変換します。
				/// @param v 度数の値を持つベクトル
				/// @return ラジアン値のベクトル
				[[nodiscard]]
				static constexpr Vec2 operator ()(const Vec2 v) noexcept
				{
					const double rad = (Math::Pi / 180.0);
					return (v * rad);
				}

				/// @brief ベクトルの各成分を度数からラジアンに変換します。
				/// @param v 度数の値を持つベクトル
				/// @return ラジアン値のベクトル
				[[nodiscard]]
				static constexpr Float3 operator ()(const Float3 v) noexcept
				{
					const float rad = (Math::PiF / 180.0f);
					return (v * rad);
				}

				/// @brief ベクトルの各成分を度数からラジアンに変換します。
				/// @param v 度数の値を持つベクトル
				/// @return ラジアン値のベクトル
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Vec3 v) noexcept
				{
					const double rad = (Math::Pi / 180.0);
					return (v * rad);
				}

				/// @brief ベクトルの各成分を度数からラジアンに変換します。
				/// @param v 度数の値を持つベクトル
				/// @return ラジアン値のベクトル
				[[nodiscard]]
				static constexpr Float4 operator ()(const Float4 v) noexcept
				{
					const float rad = (Math::PiF / 180.0f);
					return (v * rad);
				}

				/// @brief ベクトルの各成分を度数からラジアンに変換します。
				/// @param v 度数の値を持つベクトル
				/// @return ラジアン値のベクトル
				[[nodiscard]]
				static constexpr Vec4 operator ()(const Vec4 v) noexcept
				{
					const double rad = (Math::Pi / 180.0);
					return (v * rad);
				}

				/// @brief 度数からラジアンに変換します。
				/// @param x 度数の値
				/// @return ラジアン値
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief 度数からラジアンに変換します。
				/// @param x 度数の値
				/// @return ラジアン値
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	ToDegrees_impl
			//
			////////////////////////////////////////////////////////////////

			struct ToDegrees_impl
			{
				/// @brief ラジアンから度数に変換します。
				/// @param x ラジアンの値
				/// @return 度数の値
				[[nodiscard]]
				static constexpr double operator ()(const Concept::Integral auto x) noexcept
				{
					return (static_cast<double>(x) * (180.0 / Math::Pi));
				}

				/// @brief ラジアンから度数に変換します。
				/// @param x ラジアンの値
				/// @return 度数の値
				[[nodiscard]]
				static constexpr float operator ()(const float x) noexcept
				{
					return (x * (180.0f / Math::PiF));
				}

				/// @brief ラジアンから度数に変換します。
				/// @param x ラジアンの値
				/// @return 度数の値
				[[nodiscard]]
				static constexpr double operator ()(const double x) noexcept
				{
					return (x * (180.0 / Math::Pi));
				}

				/// @brief ベクトルの各成分をラジアンから度数に変換します。
				/// @param v ラジアンの値を持つベクトル
				/// @return 度数の値を持つベクトル
				[[nodiscard]]
				static constexpr Float2 operator ()(const Float2 v) noexcept
				{
					const float deg = (180.0f / Math::PiF);
					return (v * deg);
				}

				/// @brief ベクトルの各成分をラジアンから度数に変換します。
				/// @param v ラジアンの値を持つベクトル
				/// @return 度数の値を持つベクトル
				[[nodiscard]]
				static constexpr Vec2 operator ()(const Vec2 v) noexcept
				{
					const double deg = (180.0 / Math::Pi);
					return (v * deg);
				}

				/// @brief ベクトルの各成分をラジアンから度数に変換します。
				/// @param v ラジアンの値を持つベクトル
				/// @return 度数の値を持つベクトル
				[[nodiscard]]
				static constexpr Float3 operator ()(const Float3 v) noexcept
				{
					const float deg = (180.0f / Math::PiF);
					return (v * deg);
				}

				/// @brief ベクトルの各成分をラジアンから度数に変換します。
				/// @param v ラジアンの値を持つベクトル
				/// @return 度数の値を持つベクトル
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Vec3 v) noexcept
				{
					const double deg = (180.0 / Math::Pi);
					return (v * deg);
				}

				/// @brief ベクトルの各成分をラジアンから度数に変換します。
				/// @param v ラジアンの値を持つベクトル
				/// @return 度数の値を持つベクトル
				[[nodiscard]]
				static constexpr Float4 operator ()(const Float4 v) noexcept
				{
					const float deg = (180.0f / Math::PiF);
					return (v * deg);
				}

				/// @brief ベクトルの各成分をラジアンから度数に変換します。
				/// @param v ラジアンの値を持つベクトル
				/// @return 度数の値を持つベクトル
				[[nodiscard]]
				static constexpr Vec4 operator ()(const Vec4 v) noexcept
				{
					const double deg = (180.0 / Math::Pi);
					return (v * deg);
				}

				/// @brief ラジアンから度数に変換します。
				/// @param x ラジアンの値
				/// @return 度数の値
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief ラジアンから度数に変換します。
				/// @param x ラジアンの値
				/// @return 度数の値
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Abs_impl
			//
			////////////////////////////////////////////////////////////////

			struct Abs_impl
			{
				/// @brief 絶対値を返します。
				/// @param x 入力値
				/// @return x の絶対値
				[[nodiscard]]
				static constexpr auto operator ()(const Concept::SignedIntegral auto x) noexcept
				{
					return s3d::Abs(x);
				}

				/// @brief 絶対値を返します。
				/// @param x 入力値
				/// @return x の絶対値
				[[nodiscard]]
				static constexpr float operator ()(const float x) noexcept
				{
					return s3d::Abs(x);
				}

				/// @brief 絶対値を返します。
				/// @param x 入力値
				/// @return x の絶対値
				[[nodiscard]]
				static constexpr double operator ()(const double x) noexcept
				{
					return s3d::Abs(x);
				}

				/// @brief ベクトルの各成分の絶対値を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の絶対値
				[[nodiscard]]
				static constexpr Float2 operator ()(const Float2 v) noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y) };
				}

				/// @brief ベクトルの各成分の絶対値を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の絶対値
				[[nodiscard]]
				static constexpr Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y) };
				}

				/// @brief ベクトルの各成分の絶対値を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の絶対値
				[[nodiscard]]
				static constexpr Float3 operator ()(const Float3 v) noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y), s3d::Abs(v.z) };
				}

				/// @brief ベクトルの各成分の絶対値を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の絶対値
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y), s3d::Abs(v.z) };
				}

				/// @brief ベクトルの各成分の絶対値を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の絶対値
				[[nodiscard]]
				static constexpr Float4 operator ()(const Float4 v) noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y), s3d::Abs(v.z), s3d::Abs(v.w) };
				}

				/// @brief ベクトルの各成分の絶対値を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の絶対値
				[[nodiscard]]
				static constexpr Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y), s3d::Abs(v.z), s3d::Abs(v.w) };
				}

				/// @brief 絶対値を返します。
				/// @param x 入力値
				/// @return x の絶対値
				[[nodiscard]]
				static BigInt operator ()(const BigInt& x);

				/// @brief 絶対値を返します。
				/// @param x 入力値
				/// @return x の絶対値
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	AbsDiff_impl
			//
			////////////////////////////////////////////////////////////////

			struct AbsDiff_impl
			{
				/// @brief 二つの値の差の絶対値を返します。
				/// @param x 第一の値
				/// @param y 第二の値
				/// @return |x - y|
				[[nodiscard]]
				static constexpr auto operator ()(const Concept::Arithmetic auto x, const Concept::Arithmetic auto y) noexcept
				{
					using T1 = decltype(x);
					using T2 = decltype(y);

					if constexpr (std::conjunction_v<std::is_integral<T1>, std::is_integral<T2>>)
					{
						using U = std::make_unsigned_t<std::common_type_t<T1, T2>>;

						return std::cmp_greater(x, y)
							? (static_cast<U>(x) - static_cast<U>(y))
							: (static_cast<U>(y) - static_cast<U>(x));
					}
					else
					{
						return Abs(x - y);
					}
				}

				/// @brief 二つの値の差の絶対値を返します。
				/// @param x 第一の値
				/// @param y 第二の値
				/// @return |x - y|
				[[nodiscard]]
				static constexpr float operator ()(const float x, const float y) noexcept
				{
					return Abs(x - y);
				}

				/// @brief 二つの値の差の絶対値を返します。
				/// @param x 第一の値
				/// @param y 第二の値
				/// @return |x - y|
				[[nodiscard]]
				static constexpr double operator ()(const double x, const double y) noexcept
				{
					return Abs(x - y);
				}

				/// @brief 二つのベクトルの各成分の差の絶対値を返します。
				/// @param a 第一のベクトル
				/// @param b 第二のベクトル
				/// @return |a - b|
				[[nodiscard]]
				static constexpr Float2 operator ()(const Float2 a, const Float2 b) noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y) };
				}

				/// @brief 二つのベクトルの各成分の差の絶対値を返します。
				/// @param a 第一のベクトル
				/// @param b 第二のベクトル
				/// @return |a - b|
				[[nodiscard]]
				static constexpr Vec2 operator ()(const Vec2 a, const Vec2 b) noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y) };
				}

				/// @brief 二つのベクトルの各成分の差の絶対値を返します。
				/// @param a 第一のベクトル
				/// @param b 第二のベクトル
				/// @return |a - b|
				[[nodiscard]]
				static constexpr Float3 operator ()(const Float3 a, const Float3 b) noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y), Abs(a.z - b.z) };
				}

				/// @brief 二つのベクトルの各成分の差の絶対値を返します。
				/// @param a 第一のベクトル
				/// @param b 第二のベクトル
				/// @return |a - b|
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Vec3 a, const Vec3 b) noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y), Abs(a.z - b.z) };
				}

				/// @brief 二つのベクトルの各成分の差の絶対値を返します。
				/// @param a 第一のベクトル
				/// @param b 第二のベクトル
				/// @return |a - b|
				[[nodiscard]]
				static constexpr Float4 operator ()(const Float4 a, const Float4 b) noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y), Abs(a.z - b.z), Abs(a.w - b.w) };
				}

				/// @brief 二つのベクトルの各成分の差の絶対値を返します。
				/// @param a 第一のベクトル
				/// @param b 第二のベクトル
				/// @return |a - b|
				[[nodiscard]]
				static constexpr Vec4 operator ()(const Vec4 a, const Vec4 b) noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y), Abs(a.z - b.z), Abs(a.w - b.w) };
				}

				/// @brief 二つの値の差の絶対値を返します。
				/// @param x 第一の値
				/// @param y 第二の値
				/// @return |x - y|
				[[nodiscard]]
				static BigInt operator ()(const BigInt& x, const BigInt& y);

				/// @brief 二つの値の差の絶対値を返します。
				/// @param x 第一の値
				/// @param y 第二の値
				/// @return |x - y|
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x, const BigFloat& y);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Square_impl
			//
			////////////////////////////////////////////////////////////////

			struct Square_impl
			{
				/// @brief 二乗を返します。
				/// @param x 入力値
				/// @return x²
				[[nodiscard]]
				static constexpr auto operator ()(const Concept::Arithmetic auto x) noexcept
				{
					return (x * x);
				}

				/// @brief 二乗を返します。
				/// @param x 入力値
				/// @return x²
				[[nodiscard]]
				static constexpr float operator ()(const float x) noexcept
				{
					return (x * x);
				}

				/// @brief 二乗を返します。
				/// @param x 入力値
				/// @return x²
				[[nodiscard]]
				static constexpr double operator ()(const double x) noexcept
				{
					return (x * x);
				}

				/// @brief ベクトルの各成分の二乗を返します。
				/// @param v ベクトル
				/// @return 各成分の二乗
				[[nodiscard]]
				static constexpr Float2 operator ()(const Float2 v) noexcept
				{
					return{ (v.x * v.x), (v.y * v.y) };
				}

				/// @brief ベクトルの各成分の二乗を返します。
				/// @param v ベクトル
				/// @return 各成分の二乗
				[[nodiscard]]
				static constexpr Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ (v.x * v.x), (v.y * v.y) };
				}

				/// @brief ベクトルの各成分の二乗を返します。
				/// @param v ベクトル
				/// @return 各成分の二乗
				[[nodiscard]]
				static constexpr Float3 operator ()(const Float3 v) noexcept
				{
					return{ (v.x * v.x), (v.y * v.y), (v.z * v.z) };
				}

				/// @brief ベクトルの各成分の二乗を返します。
				/// @param v ベクトル
				/// @return 各成分の二乗
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ (v.x * v.x), (v.y * v.y), (v.z * v.z) };
				}

				/// @brief ベクトルの各成分の二乗を返します。
				/// @param v ベクトル
				/// @return 各成分の二乗
				[[nodiscard]]
				static constexpr Float4 operator ()(const Float4 v) noexcept
				{
					return{ (v.x * v.x), (v.y * v.y), (v.z * v.z), (v.w * v.w) };
				}

				/// @brief ベクトルの各成分の二乗を返します。
				/// @param v ベクトル
				/// @return 各成分の二乗
				[[nodiscard]]
				static constexpr Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ (v.x * v.x), (v.y * v.y), (v.z * v.z), (v.w * v.w) };
				}

				/// @brief 二乗を返します。
				/// @param x 入力値
				/// @return x²
				[[nodiscard]]
				static BigInt operator ()(const BigInt& x);

				/// @brief 二乗を返します。
				/// @param x 入力値
				/// @return x²
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Exp_impl
			//
			////////////////////////////////////////////////////////////////

			struct Exp_impl
			{
				/// @brief 値の指数関数を返します。
				/// @param x 入力値
				/// @return e^x
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::exp(static_cast<double>(x));
				}

				/// @brief 値の指数関数を返します。
				/// @param x 入力値
				/// @return e^x
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::exp(x);
				}

				/// @brief 値の指数関数を返します。
				/// @param x 入力値
				/// @return e^x
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::exp(x);
				}

				/// @brief ベクトルの各成分に指数関数を適用します。
				/// @param v ベクトル
				/// @return 各成分について e^v を適用したベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::exp(v.x), std::exp(v.y) };
				}

				/// @brief ベクトルの各成分に指数関数を適用します。
				/// @param v ベクトル
				/// @return 各成分について e^v を適用したベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::exp(v.x), std::exp(v.y) };
				}

				/// @brief ベクトルの各成分に指数関数を適用します。
				/// @param v ベクトル
				/// @return 各成分について e^v を適用したベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::exp(v.x), std::exp(v.y), std::exp(v.z) };
				}

				/// @brief ベクトルの各成分に指数関数を適用します。
				/// @param v ベクトル
				/// @return 各成分について e^v を適用したベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::exp(v.x), std::exp(v.y), std::exp(v.z) };
				}

				/// @brief ベクトルの各成分に指数関数を適用します。
				/// @param v ベクトル
				/// @return 各成分について e^v を適用したベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::exp(v.x), std::exp(v.y), std::exp(v.z), std::exp(v.w) };
				}

				/// @brief ベクトルの各成分に指数関数を適用します。
				/// @param v ベクトル
				/// @return 各成分について e^v を適用したベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::exp(v.x), std::exp(v.y), std::exp(v.z), std::exp(v.w) };
				}

				/// @brief 値の指数関数を返します。
				/// @param x 入力値
				/// @return e^x
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);
				
				/// @brief 値の指数関数を返します。
				/// @param x 入力値
				/// @return e^x
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Exp2_impl
			//
			////////////////////////////////////////////////////////////////

			struct Exp2_impl
			{
				/// @brief 整数の 2 のべき乗を返します。
				/// @param x 入力値
				/// @return 2^x
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::exp2(static_cast<double>(x));
				}

				/// @brief 値の 2 のべき乗を返します。
				/// @param x 入力値
				/// @return 2^x
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::exp2(x);
				}

				/// @brief 値の 2 のべき乗を返します。
				/// @param x 入力値
				/// @return 2^x
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::exp2(x);
				}

				/// @brief ベクトルの各成分に 2 のべき乗を適用します。
				/// @param v ベクトル
				/// @return 各成分について 2^v を適用したベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::exp2(v.x), std::exp2(v.y) };
				}

				/// @brief ベクトルの各成分に 2 のべき乗を適用します。
				/// @param v ベクトル
				/// @return 各成分について 2^v を適用したベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::exp2(v.x), std::exp2(v.y) };
				}

				/// @brief ベクトルの各成分に 2 のべき乗を適用します。
				/// @param v ベクトル
				/// @return 各成分について 2^v を適用したベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::exp2(v.x), std::exp2(v.y), std::exp2(v.z) };
				}

				/// @brief ベクトルの各成分に 2 のべき乗を適用します。
				/// @param v ベクトル
				/// @return 各成分について 2^v を適用したベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::exp2(v.x), std::exp2(v.y), std::exp2(v.z) };
				}

				/// @brief ベクトルの各成分に 2 のべき乗を適用します。
				/// @param v ベクトル
				/// @return 各成分について 2^v を適用したベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::exp2(v.x), std::exp2(v.y), std::exp2(v.z), std::exp2(v.w) };
				}

				/// @brief ベクトルの各成分に 2 のべき乗を適用します。
				/// @param v ベクトル
				/// @return 各成分について 2^v を適用したベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::exp2(v.x), std::exp2(v.y), std::exp2(v.z), std::exp2(v.w) };
				}

				/// @brief 値の 2 のべき乗を返します。
				/// @param x 
				/// @return 2^x
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief 値の 2 のべき乗を返します。
				/// @param x
				/// @return 2^x
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Rsqrt_impl
			//
			////////////////////////////////////////////////////////////////

			struct Rsqrt_impl
			{
				/// @brief 逆平方根を返します。
				/// @param x 入力値
				/// @return 1/√x
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return (1.0 / std::sqrt(static_cast<double>(x)));
				}

				/// @brief 逆平方根を返します。
				/// @param x 入力値
				/// @return 1/√x
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return (1.0f / std::sqrt(x));
				}

				/// @brief 逆平方根を返します。
				/// @param x 入力値
				/// @return 1/√x
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return (1.0 / std::sqrt(x));
				}

				/// @brief ベクトルの各成分の逆平方根を返します。
				/// @param v ベクトル
				/// @return 各成分について 1/√v を適用したベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ (1.0f / std::sqrt(v.x)), (1.0f / std::sqrt(v.y)) };
				}

				/// @brief ベクトルの各成分の逆平方根を返します。
				/// @param v ベクトル
				/// @return 各成分について 1/√v を適用したベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ (1.0 / std::sqrt(v.x)), (1.0 / std::sqrt(v.y)) };
				}

				/// @brief ベクトルの各成分の逆平方根を返します。
				/// @param v ベクトル
				/// @return 各成分について 1/√v を適用したベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ (1.0f / std::sqrt(v.x)), (1.0f / std::sqrt(v.y)), (1.0f / std::sqrt(v.z)) };
				}

				/// @brief ベクトルの各成分の逆平方根を返します。
				/// @param v ベクトル
				/// @return 各成分について 1/√v を適用したベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ (1.0 / std::sqrt(v.x)), (1.0 / std::sqrt(v.y)), (1.0 / std::sqrt(v.z)) };
				}

				/// @brief ベクトルの各成分の逆平方根を返します。
				/// @param v ベクトル
				/// @return 各成分について 1/√v を適用したベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ (1.0f / std::sqrt(v.x)), (1.0f / std::sqrt(v.y)), (1.0f / std::sqrt(v.z)), (1.0f / std::sqrt(v.w)) };
				}

				/// @brief ベクトルの各成分の逆平方根を返します。
				/// @param v ベクトル
				/// @return 各成分について 1/√v を適用したベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ (1.0 / std::sqrt(v.x)), (1.0 / std::sqrt(v.y)), (1.0 / std::sqrt(v.z)), (1.0 / std::sqrt(v.w)) };
				}

				/// @brief 逆平方根を返します。
				/// @param x 入力値
				/// @return 1/√x
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief 逆平方根を返します。
				/// @param x 入力値
				/// @return 1/√x
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Sqrt_impl
			//
			////////////////////////////////////////////////////////////////

			struct Sqrt_impl
			{
				/// @brief 平方根を返します。
				/// @param x 入力値
				/// @return √x
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::sqrt(static_cast<double>(x));
				}

				/// @brief 平方根を返します。
				/// @param x 入力値
				/// @return √x
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::sqrt(x);
				}

				/// @brief 平方根を返します。
				/// @param x 入力値
				/// @return √x
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::sqrt(x);
				}

				/// @brief ベクトルの各成分の平方根を返します。
				/// @param v ベクトル
				/// @return 各成分について √v を適用したベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::sqrt(v.x), std::sqrt(v.y) };
				}

				/// @brief ベクトルの各成分の平方根を返します。
				/// @param v ベクトル
				/// @return 各成分について √v を適用したベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::sqrt(v.x), std::sqrt(v.y) };
				}

				/// @brief ベクトルの各成分の平方根を返します。
				/// @param v ベクトル
				/// @return 各成分について √v を適用したベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z) };
				}

				/// @brief ベクトルの各成分の平方根を返します。
				/// @param v ベクトル
				/// @return 各成分について √v を適用したベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z) };
				}

				/// @brief ベクトルの各成分の平方根を返します。
				/// @param v ベクトル
				/// @return 各成分について √v を適用したベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z), std::sqrt(v.w) };
				}

				/// @brief ベクトルの各成分の平方根を返します。
				/// @param v ベクトル
				/// @return 各成分について √v を適用したベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z), std::sqrt(v.w) };
				}

				/// @brief 平方根を返します。
				/// @param x 入力値
				/// @return √x
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief 平方根を返します。
				/// @param x 入力値
				/// @return √x
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Ceil_impl
			//
			////////////////////////////////////////////////////////////////

			struct Ceil_impl
			{
				/// @brief 値の天井関数（切り上げ）を返します。
				/// @param x 入力値
				/// @return x 以上の最小の整数
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::ceil(x);
				}

				/// @brief 値の天井関数（切り上げ）を返します。
				/// @param x 入力値
				/// @return x 以上の最小の整数
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::ceil(x);
				}

				/// @brief ベクトルの各成分に天井関数（切り上げ）を適用した結果を返します。
				/// @param v ベクトル
				/// @return 各成分について最小の整数に切り上げたベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::ceil(v.x), std::ceil(v.y) };
				}

				/// @brief ベクトルの各成分に天井関数（切り上げ）を適用した結果を返します。
				/// @param v ベクトル
				/// @return 各成分について最小の整数に切り上げたベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::ceil(v.x), std::ceil(v.y) };
				}

				/// @brief ベクトルの各成分に天井関数（切り上げ）を適用した結果を返します。
				/// @param v ベクトル
				/// @return 各成分について最小の整数に切り上げたベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::ceil(v.x), std::ceil(v.y), std::ceil(v.z) };
				}

				/// @brief ベクトルの各成分に天井関数（切り上げ）を適用した結果を返します。
				/// @param v ベクトル
				/// @return 各成分について最小の整数に切り上げたベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::ceil(v.x), std::ceil(v.y), std::ceil(v.z) };
				}

				/// @brief ベクトルの各成分に天井関数（切り上げ）を適用した結果を返します。
				/// @param v ベクトル
				/// @return 各成分について最小の整数に切り上げたベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::ceil(v.x), std::ceil(v.y), std::ceil(v.z), std::ceil(v.w) };
				}

				/// @brief ベクトルの各成分に天井関数（切り上げ）を適用した結果を返します。
				/// @param v ベクトル
				/// @return 各成分について最小の整数に切り上げたベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::ceil(v.x), std::ceil(v.y), std::ceil(v.z), std::ceil(v.w) };
				}

				/// @brief 値の天井関数（切り上げ）を返します。
				/// @param x 入力値
				/// @return x 以上の最小の整数
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Floor_impl
			//
			////////////////////////////////////////////////////////////////

			struct Floor_impl
			{
				/// @brief 値の床関数（切り捨て）を返します。
				/// @param x 入力値
				/// @return x 以下の最大の整数
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::floor(x);
				}

				/// @brief 値の床関数（切り捨て）を返します。
				/// @param x 入力値
				/// @return x 以下の最大の整数
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::floor(x);
				}

				/// @brief ベクトルの各成分に床関数（切り捨て）を適用した結果を返します。
				/// @param v ベクトル
				/// @return 各成分について最大の整数に切り捨てたベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::floor(v.x), std::floor(v.y) };
				}

				/// @brief ベクトルの各成分に床関数（切り捨て）を適用した結果を返します。
				/// @param v ベクトル
				/// @return 各成分について最大の整数に切り捨てたベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::floor(v.x), std::floor(v.y) };
				}

				/// @brief ベクトルの各成分に床関数（切り捨て）を適用した結果を返します。
				/// @param v ベクトル
				/// @return 各成分について最大の整数に切り捨てたベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::floor(v.x), std::floor(v.y), std::floor(v.z) };
				}

				/// @brief ベクトルの各成分に床関数（切り捨て）を適用した結果を返します。
				/// @param v ベクトル
				/// @return 各成分について最大の整数に切り捨てたベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::floor(v.x), std::floor(v.y), std::floor(v.z) };
				}

				/// @brief ベクトルの各成分に床関数（切り捨て）を適用した結果を返します。
				/// @param v ベクトル
				/// @return 各成分について最大の整数に切り捨てたベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::floor(v.x), std::floor(v.y), std::floor(v.z), std::floor(v.w) };
				}

				/// @brief ベクトルの各成分に床関数（切り捨て）を適用した結果を返します。
				/// @param v ベクトル
				/// @return 各成分について最大の整数に切り捨てたベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::floor(v.x), std::floor(v.y), std::floor(v.z), std::floor(v.w) };
				}

				/// @brief 値の床関数（切り捨て）を返します。
				/// @param x 入力値
				/// @return x 以下の最大の整数
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Round_impl
			//
			////////////////////////////////////////////////////////////////

			struct Round_impl
			{
				/// @brief 四捨五入した値を返します。
				/// @param x 入力値
				/// @return x に最も近い整数
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::round(x);
				}

				/// @brief 四捨五入した値を返します。
				/// @param x 入力値
				/// @return x に最も近い整数
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::round(x);
				}

				/// @brief ベクトルの各成分を四捨五入した値を返します。
				/// @param v ベクトル
				/// @return 各成分について四捨五入したベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::round(v.x), std::round(v.y) };
				}

				/// @brief ベクトルの各成分を四捨五入した値を返します。
				/// @param v ベクトル
				/// @return 各成分について四捨五入したベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::round(v.x), std::round(v.y) };
				}

				/// @brief ベクトルの各成分を四捨五入した値を返します。
				/// @param v ベクトル
				/// @return 各成分について四捨五入したベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::round(v.x), std::round(v.y), std::round(v.z) };
				}

				/// @brief ベクトルの各成分を四捨五入した値を返します。
				/// @param v ベクトル
				/// @return 各成分について四捨五入したベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::round(v.x), std::round(v.y), std::round(v.z) };
				}

				/// @brief ベクトルの各成分を四捨五入した値を返します。
				/// @param v ベクトル
				/// @return 各成分について四捨五入したベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::round(v.x), std::round(v.y), std::round(v.z), std::round(v.w) };
				}

				/// @brief ベクトルの各成分を四捨五入した値を返します。
				/// @param v ベクトル
				/// @return 各成分について四捨五入したベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::round(v.x), std::round(v.y), std::round(v.z), std::round(v.w) };
				}

				/// @brief 四捨五入した値を返します。
				/// @param x 入力値
				/// @return x に最も近い整数
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Trunc_impl
			//
			////////////////////////////////////////////////////////////////

			struct Trunc_impl
			{
				/// @brief 小数部を切り捨てた値を返します。
				/// @param x 入力値
				/// @return x の整数部
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::trunc(x);
				}

				/// @brief 小数部を切り捨てた値を返します。
				/// @param x 入力値
				/// @return x の整数部
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::trunc(x);
				}

				/// @brief ベクトルの各成分の小数部を切り捨てた値を返します。
				/// @param v ベクトル
				/// @return 各成分の整数部で構成されたベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::trunc(v.x), std::trunc(v.y) };
				}

				/// @brief ベクトルの各成分の小数部を切り捨てた値を返します。
				/// @param v ベクトル
				/// @return 各成分の整数部で構成されたベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::trunc(v.x), std::trunc(v.y) };
				}

				/// @brief ベクトルの各成分の小数部を切り捨てた値を返します。
				/// @param v ベクトル
				/// @return 各成分の整数部で構成されたベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::trunc(v.x), std::trunc(v.y), std::trunc(v.z) };
				}

				/// @brief ベクトルの各成分の小数部を切り捨てた値を返します。
				/// @param v ベクトル
				/// @return 各成分の整数部で構成されたベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::trunc(v.x), std::trunc(v.y), std::trunc(v.z) };
				}

				/// @brief ベクトルの各成分の小数部を切り捨てた値を返します。
				/// @param v ベクトル
				/// @return 各成分の整数部で構成されたベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::trunc(v.x), std::trunc(v.y), std::trunc(v.z), std::trunc(v.w) };
				}

				/// @brief ベクトルの各成分の小数部を切り捨てた値を返します。
				/// @param v ベクトル
				/// @return 各成分の整数部で構成されたベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::trunc(v.x), std::trunc(v.y), std::trunc(v.z), std::trunc(v.w) };
				}

				/// @brief 小数部を切り捨てた値を返します。
				/// @param x 入力値
				/// @return x の整数部
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Max_impl
			//
			////////////////////////////////////////////////////////////////

			struct Max_impl
			{
				/// @brief 値の最大値を返します。
				/// @param a 一方の値
				/// @param b もう一方の値
				/// @return a と b のうち大きい方の値
				[[nodiscard]]
				static constexpr float operator ()(const float a, const float b) noexcept
				{
					return s3d::Max(a, b);
				}

				/// @brief 値の最大値を返します。
				/// @param a 一方の値
				/// @param b もう一方の値
				/// @return a と b のうち大きい方の値
				[[nodiscard]]
				static constexpr double operator ()(const double a, const double b) noexcept
				{
					return s3d::Max(a, b);
				}

				/// @brief ベクトルの成分ごとに最大値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最大値で構成されたベクトル
				[[nodiscard]]
				static constexpr Point operator ()(const Point a, const Point b) noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y) };
				}

				/// @brief ベクトルの成分ごとに最大値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最大値で構成されたベクトル
				[[nodiscard]]
				static constexpr Float2 operator ()(const Float2 a, const Float2 b) noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y) };
				}

				/// @brief ベクトルの成分ごとに最大値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最大値で構成されたベクトル
				[[nodiscard]]
				static constexpr Vec2 operator ()(const Vec2 a, const Vec2 b) noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y) };
				}

				/// @brief ベクトルの成分ごとに最大値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最大値で構成されたベクトル
				[[nodiscard]]
				static constexpr Float3 operator ()(const Float3 a, const Float3 b) noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y), s3d::Max(a.z, b.z) };
				}

				/// @brief ベクトルの成分ごとに最大値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最大値で構成されたベクトル
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Vec3 a, const Vec3 b) noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y), s3d::Max(a.z, b.z) };
				}

				/// @brief ベクトルの成分ごとに最大値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最大値で構成されたベクトル
				[[nodiscard]]
				static constexpr Float4 operator ()(const Float4 a, const Float4 b) noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y), s3d::Max(a.z, b.z), s3d::Max(a.w, b.w) };
				}

				/// @brief ベクトルの成分ごとに最大値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最大値で構成されたベクトル
				[[nodiscard]]
				static constexpr Vec4 operator ()(const Vec4 a, const Vec4 b) noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y), s3d::Max(a.z, b.z), s3d::Max(a.w, b.w) };
				}

				/// @brief 値の最大値を返します。
				/// @param x 一方の値
				/// @param y もう一方の値
				/// @return x と y のうち大きい方の値
				[[nodiscard]]
				static const BigInt& operator ()(const BigInt& x, const BigInt& y);

				/// @brief 値の最大値を返します。
				/// @param x 一方の値
				/// @param y もう一方の値
				/// @return x と y のうち大きい方の値
				[[nodiscard]]
				static const BigFloat& operator ()(const BigFloat& x, const BigFloat& y);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Min_impl
			//
			////////////////////////////////////////////////////////////////

			struct Min_impl
			{
				/// @brief 値の最小値を返します。
				/// @param a 一方の値
				/// @param b もう一方の値
				/// @return a と b のうち小さい方の値
				[[nodiscard]]
				static constexpr float operator ()(const float a, const float b) noexcept
				{
					return s3d::Min(a, b);
				}

				/// @brief 値の最小値を返します。
				/// @param a 一方の値
				/// @param b もう一方の値
				/// @return a と b のうち小さい方の値
				[[nodiscard]]
				static constexpr double operator ()(const double a, const double b) noexcept
				{
					return s3d::Min(a, b);
				}

				/// @brief ベクトルの成分ごとの最小値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最小値で構成されたベクトル
				[[nodiscard]]
				static constexpr Point operator ()(const Point a, const Point b) noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y) };
				}

				/// @brief ベクトルの成分ごとの最小値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最小値で構成されたベクトル
				[[nodiscard]]
				static constexpr Float2 operator ()(const Float2 a, const Float2 b) noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y) };
				}

				/// @brief ベクトルの成分ごとの最小値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最小値で構成されたベクトル
				[[nodiscard]]
				static constexpr Vec2 operator ()(const Vec2 a, const Vec2 b) noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y) };
				}

				/// @brief ベクトルの成分ごとの最小値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最小値で構成されたベクトル
				[[nodiscard]]
				static constexpr Float3 operator ()(const Float3 a, const Float3 b) noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y), s3d::Min(a.z, b.z) };
				}

				/// @brief ベクトルの成分ごとの最小値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最小値で構成されたベクトル
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Vec3 a, const Vec3 b) noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y), s3d::Min(a.z, b.z) };
				}

				/// @brief ベクトルの成分ごとの最小値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最小値で構成されたベクトル
				[[nodiscard]]
				static constexpr Float4 operator ()(const Float4 a, const Float4 b) noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y), s3d::Min(a.z, b.z), s3d::Min(a.w, b.w) };
				}

				/// @brief ベクトルの成分ごとの最小値を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 各成分の最小値で構成されたベクトル
				[[nodiscard]]
				static constexpr Vec4 operator ()(const Vec4 a, const Vec4 b) noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y), s3d::Min(a.z, b.z), s3d::Min(a.w, b.w) };
				}

				/// @brief 値の最小値を返します。
				/// @param x 一方の値
				/// @param y もう一方の値
				/// @return x と y のうち小さい方の値
				[[nodiscard]]
				static const BigInt& operator ()(const BigInt& x, const BigInt& y);

				/// @brief 値の最小値を返します。
				/// @param x 一方の値
				/// @param y もう一方の値
				/// @return x と y のうち小さい方の値
				[[nodiscard]]
				static const BigFloat& operator ()(const BigFloat& x, const BigFloat& y);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Clamp_impl
			//
			////////////////////////////////////////////////////////////////

			struct Clamp_impl
			{
				/// @brief 値をクランプします。
				/// @param value 値
				/// @param min 最小値
				/// @param max 最大値
				/// @return min 以上 max 以下にクランプされた値
				[[nodiscard]]
				static constexpr float operator ()(const float value, const float min, const float max) noexcept
				{
					return s3d::Clamp(value, min, max);
				}

				/// @brief 値をクランプします。
				/// @param value 値
				/// @param min 最小値
				/// @param max 最大値
				/// @return min 以上 max 以下にクランプされた値
				[[nodiscard]]
				static constexpr double operator ()(const double value, const double min, const double max) noexcept
				{
					return s3d::Clamp(value, min, max);
				}

				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値
				/// @param max 最大値
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Point operator ()(const Point value, int32 min, int32 max) noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max) };
				}
	
				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値ベクトル
				/// @param max 最大値ベクトル
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Point operator ()(const Point value, const Point min, const Point max) noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y) };
				}

				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値
				/// @param max 最大値
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Float2 operator ()(const Float2 value, float min, float max) noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max) };
				}
				
				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値ベクトル
				/// @param max 最大値ベクトル
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Float2 operator ()(const Float2 value, const Float2 min, const Float2 max) noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y) };
				}

				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値
				/// @param max 最大値
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Vec2 operator ()(const Vec2 value, double min, double max) noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max) };
				}

				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値ベクトル
				/// @param max 最大値ベクトル
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Vec2 operator ()(const Vec2 value, const Vec2 min, const Vec2 max) noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y) };
				}

				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値
				/// @param max 最大値
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Float3 operator ()(const Float3 value, float min, float max) noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max), s3d::Clamp(value.z, min, max) };
				}

				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値ベクトル
				/// @param max 最大値ベクトル
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Float3 operator ()(const Float3 value, const Float3 min, const Float3 max) noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y), s3d::Clamp(value.z, min.z, max.z) };
				}

				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値
				/// @param max 最大値
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Vec3 value, double min, double max) noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max), s3d::Clamp(value.z, min, max) };
				}

				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値ベクトル
				/// @param max 最大値ベクトル
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Vec3 value, const Vec3 min, const Vec3 max) noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y), s3d::Clamp(value.z, min.z, max.z) };
				}

				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値
				/// @param max 最大値
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Float4 operator ()(const Float4 value, float min, float max) noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max), s3d::Clamp(value.z, min, max), s3d::Clamp(value.w, min, max) };
				}

				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値ベクトル
				/// @param max 最大値ベクトル
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Float4 operator ()(const Float4 value, const Float4 min, const Float4 max) noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y), s3d::Clamp(value.z, min.z, max.z), s3d::Clamp(value.w, min.w, max.w) };
				}

				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値
				/// @param max 最大値
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Vec4 operator ()(const Vec4 value, double min, double max) noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max), s3d::Clamp(value.z, min, max), s3d::Clamp(value.w, min, max) };
				}

				/// @brief ベクトルの各成分をクランプします。
				/// @param value ベクトル
				/// @param min 最小値ベクトル
				/// @param max 最大値ベクトル
				/// @return 各成分がクランプされたベクトル
				[[nodiscard]]
				static constexpr Vec4 operator ()(const Vec4 value, const Vec4 min, const Vec4 max) noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y), s3d::Clamp(value.z, min.z, max.z), s3d::Clamp(value.w, min.w, max.w) };
				}

				/// @brief 値をクランプします。
				/// @param x 入力値
				/// @param min 最小値
				/// @param max 最大値
				/// @return min 以上 max 以下にクランプされた値
				[[nodiscard]]
				static const BigInt& operator ()(const BigInt& x, const BigInt& min, const BigInt& max);

				/// @brief 値をクランプします。
				/// @param x 入力値
				/// @param min 最小値
				/// @param max 最大値
				/// @return min 以上 max 以下にクランプされた値
				[[nodiscard]]
				static const BigFloat& operator ()(const BigFloat& x, const BigFloat& min, const BigFloat& max);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Saturate_impl
			//
			////////////////////////////////////////////////////////////////

			struct Saturate_impl
			{
				/// @brief 値を [0, 1] の範囲にクランプします。
				/// @param x 入力値
				/// @return 0.0 以上 1.0 以下にクランプされた値
				[[nodiscard]]
				static constexpr float operator ()(const float x) noexcept
				{
					return s3d::Clamp(x, 0.0f, 1.0f);
				}

				/// @brief 値を [0, 1] の範囲にクランプします。
				/// @param x 入力値
				/// @return 0.0 以上 1.0 以下にクランプされた値
				[[nodiscard]]
				static constexpr double operator ()(const double x) noexcept
				{
					return s3d::Clamp(x, 0.0, 1.0);
				}

				/// @brief ベクトルの各成分を [0, 1] の範囲にクランプします。
				/// @param v ベクトル
				/// @return 各成分が 0.0 以上 1.0 以下にクランプされたベクトル
				[[nodiscard]]
				static constexpr Float2 operator ()(const Float2 v) noexcept
				{
					return{ s3d::Clamp(v.x, 0.0f, 1.0f), s3d::Clamp(v.y, 0.0f, 1.0f) };
				}

				/// @brief ベクトルの各成分を [0, 1] の範囲にクランプします。
				/// @param v ベクトル
				/// @return 各成分が 0.0 以上 1.0 以下にクランプされたベクトル
				[[nodiscard]]
				static constexpr Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ s3d::Clamp(v.x, 0.0, 1.0), s3d::Clamp(v.y, 0.0, 1.0) };
				}

				/// @brief ベクトルの各成分を [0, 1] の範囲にクランプします。
				/// @param v ベクトル
				/// @return 各成分が 0.0 以上 1.0 以下にクランプされたベクトル
				[[nodiscard]]
				static constexpr Float3 operator ()(const Float3 v) noexcept
				{
					return{ s3d::Clamp(v.x, 0.0f, 1.0f), s3d::Clamp(v.y, 0.0f, 1.0f), s3d::Clamp(v.z, 0.0f, 1.0f) };
				}

				/// @brief ベクトルの各成分を [0, 1] の範囲にクランプします。
				/// @param v ベクトル
				/// @return 各成分が 0.0 以上 1.0 以下にクランプされたベクトル
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ s3d::Clamp(v.x, 0.0, 1.0), s3d::Clamp(v.y, 0.0, 1.0), s3d::Clamp(v.z, 0.0, 1.0) };
				}

				/// @brief ベクトルの各成分を [0, 1] の範囲にクランプします。
				/// @param v ベクトル
				/// @return 各成分が 0.0 以上 1.0 以下にクランプされたベクトル
				[[nodiscard]]
				static constexpr Float4 operator ()(const Float4 v) noexcept
				{
					return{ s3d::Clamp(v.x, 0.0f, 1.0f), s3d::Clamp(v.y, 0.0f, 1.0f), s3d::Clamp(v.z, 0.0f, 1.0f), s3d::Clamp(v.w, 0.0f, 1.0f) };
				}

				/// @brief ベクトルの各成分を [0, 1] の範囲にクランプします。
				/// @param v ベクトル
				/// @return 各成分が 0.0 以上 1.0 以下にクランプされたベクトル
				[[nodiscard]]
				static constexpr Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ s3d::Clamp(v.x, 0.0, 1.0), s3d::Clamp(v.y, 0.0, 1.0), s3d::Clamp(v.z, 0.0, 1.0), s3d::Clamp(v.w, 0.0, 1.0) };
				}

				/// @brief 値を [0, 1] の範囲にクランプします。
				/// @param x 入力値
				/// @return 0.0 以上 1.0 以下にクランプされた値
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Acos_impl
			//
			////////////////////////////////////////////////////////////////

			struct Acos_impl
			{
				/// @brief 逆余弦を返します。
				/// @param x 入力値
				/// @return 逆余弦値（ラジアン）
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::acos(static_cast<double>(x));
				}

				/// @brief 逆余弦を返します。
				/// @param x 入力値
				/// @return 逆余弦値（ラジアン）
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::acos(x);
				}

				/// @brief 逆余弦を返します。
				/// @param x 入力値
				/// @return 逆余弦値（ラジアン）
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::acos(x);
				}

				/// @brief ベクトルの各成分の逆余弦を返します。
				/// @param v ベクトル
				/// @return 各成分の逆余弦値（ラジアン）
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::acos(v.x), std::acos(v.y) };
				}

				/// @brief ベクトルの各成分の逆余弦を返します。
				/// @param v ベクトル
				/// @return 各成分の逆余弦値（ラジアン）
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::acos(v.x), std::acos(v.y) };
				}

				/// @brief ベクトルの各成分の逆余弦を返します。
				/// @param v ベクトル
				/// @return 各成分の逆余弦値（ラジアン）
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::acos(v.x), std::acos(v.y), std::acos(v.z) };
				}

				/// @brief ベクトルの各成分の逆余弦を返します。
				/// @param v ベクトル
				/// @return 各成分の逆余弦値（ラジアン）
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::acos(v.x), std::acos(v.y), std::acos(v.z) };
				}

				/// @brief ベクトルの各成分の逆余弦を返します。
				/// @param v ベクトル
				/// @return 各成分の逆余弦値（ラジアン）
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::acos(v.x), std::acos(v.y), std::acos(v.z), std::acos(v.w) };
				}

				/// @brief ベクトルの各成分の逆余弦を返します。
				/// @param v ベクトル
				/// @return 各成分の逆余弦値（ラジアン）
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::acos(v.x), std::acos(v.y), std::acos(v.z), std::acos(v.w) };
				}

				/// @brief 逆余弦を返します。
				/// @param x 入力値
				/// @return 逆余弦値（ラジアン）
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief 逆余弦を返します。
				/// @param x 入力値
				/// @return 逆余弦値（ラジアン）
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Asin_impl
			//
			////////////////////////////////////////////////////////////////

			struct Asin_impl
			{
				/// @brief 逆正弦を返します。
				/// @param x 入力値
				/// @return 逆正弦値（ラジアン）
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::asin(static_cast<double>(x));
				}

				/// @brief 逆正弦を返します。
				/// @param x 入力値
				/// @return 逆正弦値（ラジアン）
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::asin(x);
				}

				/// @brief 逆正弦を返します。
				/// @param x 入力値
				/// @return 逆正弦値（ラジアン）
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::asin(x);
				}

				/// @brief ベクトルの各成分の逆正弦を返します。
				/// @param v ベクトル
				/// @return 各成分の逆正弦値（ラジアン）
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::asin(v.x), std::asin(v.y) };
				}

				/// @brief ベクトルの各成分の逆正弦を返します。
				/// @param v ベクトル
				/// @return 各成分の逆正弦値（ラジアン）
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::asin(v.x), std::asin(v.y) };
				}

				/// @brief ベクトルの各成分の逆正弦を返します。
				/// @param v ベクトル
				/// @return 各成分の逆正弦値（ラジアン）
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::asin(v.x), std::asin(v.y), std::asin(v.z) };
				}

				/// @brief ベクトルの各成分の逆正弦を返します。
				/// @param v ベクトル
				/// @return 各成分の逆正弦値（ラジアン）
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::asin(v.x), std::asin(v.y), std::asin(v.z) };
				}

				/// @brief ベクトルの各成分の逆正弦を返します。
				/// @param v ベクトル
				/// @return 各成分の逆正弦値（ラジアン）
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::asin(v.x), std::asin(v.y), std::asin(v.z), std::asin(v.w) };
				}

				/// @brief ベクトルの各成分の逆正弦を返します。
				/// @param v ベクトル
				/// @return 各成分の逆正弦値（ラジアン）
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::asin(v.x), std::asin(v.y), std::asin(v.z), std::asin(v.w) };
				}

				/// @brief 逆正弦を返します。
				/// @param x 入力値
				/// @return 逆正弦値（ラジアン）
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief 逆正弦を返します。
				/// @param x 入力値
				/// @return 逆正弦値（ラジアン）
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Atan_impl
			//
			////////////////////////////////////////////////////////////////

			struct Atan_impl
			{
				/// @brief 逆正切を返します。
				/// @param x 入力値
				/// @return 逆正切値（ラジアン）
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::atan(static_cast<double>(x));
				}

				/// @brief 逆正切を返します。
				/// @param x 入力値
				/// @return 逆正切値（ラジアン）
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::atan(x);
				}

				/// @brief 逆正切を返します。
				/// @param x 入力値
				/// @return 逆正切値（ラジアン）
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::atan(x);
				}

				/// @brief ベクトルの各成分の逆正切を返します。
				/// @param v ベクトル
				/// @return 各成分の逆正切値（ラジアン）
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::atan(v.x), std::atan(v.y) };
				}

				/// @brief ベクトルの各成分の逆正切を返します。
				/// @param v ベクトル
				/// @return 各成分の逆正切値（ラジアン）
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::atan(v.x), std::atan(v.y) };
				}

				/// @brief ベクトルの各成分の逆正切を返します。
				/// @param v ベクトル
				/// @return 各成分の逆正切値（ラジアン）
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::atan(v.x), std::atan(v.y), std::atan(v.z) };
				}

				/// @brief ベクトルの各成分の逆正切を返します。
				/// @param v ベクトル
				/// @return 各成分の逆正切値（ラジアン）
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::atan(v.x), std::atan(v.y), std::atan(v.z) };
				}

				/// @brief ベクトルの各成分の逆正切を返します。
				/// @param v ベクトル
				/// @return 各成分の逆正切値（ラジアン）
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::atan(v.x), std::atan(v.y), std::atan(v.z), std::atan(v.w) };
				}

				/// @brief ベクトルの各成分の逆正切を返します。
				/// @param v ベクトル
				/// @return 各成分の逆正切値（ラジアン）
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::atan(v.x), std::atan(v.y), std::atan(v.z), std::atan(v.w) };
				}

				/// @brief 逆正切を返します。
				/// @param x 入力値
				/// @return 逆正切値（ラジアン）
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief 逆正切を返します。
				/// @param x 入力値
				/// @return 逆正切値（ラジアン）
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Atan2_impl
			//
			////////////////////////////////////////////////////////////////

			struct Atan2_impl
			{
				/// @brief Atan2 の結果を返します。
				/// @param y y 座標値
				/// @param x x 座標値
				/// @return 逆正切値（ラジアン）
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto y, const Concept::Integral auto x) noexcept
				{
					if ((x == 0) && (y == 0))
					{
						return 0.0;
					}

					return std::atan2(static_cast<double>(y), static_cast<double>(x));
				}

				/// @brief Atan2 の結果を返します。
				/// @param y y 座標値
				/// @param x x 座標値
				/// @return 逆正切値（ラジアン）
				[[nodiscard]]
				static float operator ()(const float y, const float x) noexcept
				{
					if ((x == 0.0f) && (y == 0.0f))
					{
						return 0.0f;
					}

					return std::atan2(y, x);
				}

				/// @brief Atan2 の結果を返します。
				/// @param y y 座標値
				/// @param x x 座標値
				/// @return 逆正切値（ラジアン）
				[[nodiscard]]
				static double operator ()(const double y, const double x) noexcept
				{
					if ((x == 0.0) && (y == 0.0))
					{
						return 0.0;
					}

					return std::atan2(y, x);
				}

				/// @brief ベクトルの各成分ごとの Atan2 の結果を返します。
				/// @param y y 座標ベクトル
				/// @param x x 座標ベクトル
				/// @return 各成分の逆正切値（ラジアン）
				[[nodiscard]]
				static Float2 operator ()(const Float2 y, const Float2 x) noexcept
				{
					return{ operator()(y.x, x.x), operator()(y.y, x.y) };
				}

				/// @brief ベクトルの各成分ごとの Atan2 の結果を返します。
				/// @param y y 座標ベクトル
				/// @param x x 座標ベクトル
				/// @return 各成分の逆正切値（ラジアン）
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 y, const Vec2 x) noexcept
				{
					return{ operator()(y.x, x.x), operator()(y.y, x.y) };
				}

				/// @brief ベクトルの各成分ごとの Atan2 の結果を返します。
				/// @param y y 座標ベクトル
				/// @param x x 座標ベクトル
				/// @return 各成分の逆正切値（ラジアン）
				[[nodiscard]]
				static Float3 operator ()(const Float3 y, const Float3 x) noexcept
				{
					return{ operator()(y.x, x.x), operator()(y.y, x.y), operator()(y.z, x.z) };
				}

				/// @brief ベクトルの各成分ごとの Atan2 の結果を返します。
				/// @param y y 座標ベクトル
				/// @param x x 座標ベクトル
				/// @return 各成分の逆正切値（ラジアン）
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 y, const Vec3 x) noexcept
				{
					return{ operator()(y.x, x.x), operator()(y.y, x.y), operator()(y.z, x.z) };
				}

				/// @brief ベクトルの各成分ごとの Atan2 の結果を返します。
				/// @param y y 座標ベクトル
				/// @param x x 座標ベクトル
				/// @return 各成分の逆正切値（ラジアン）
				[[nodiscard]]
				static Float4 operator ()(const Float4 y, const Float4 x) noexcept
				{
					return{ operator()(y.x, x.x), operator()(y.y, x.y), operator()(y.z, x.z), operator()(y.w, x.w) };
				}

				/// @brief ベクトルの各成分ごとの Atan2 の結果を返します。
				/// @param y y 座標ベクトル
				/// @param x x 座標ベクトル
				/// @return 各成分の逆正切値（ラジアン）
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 y, const Vec4 x) noexcept
				{
					return{ operator()(y.x, x.x), operator()(y.y, x.y), operator()(y.z, x.z), operator()(y.w, x.w) };
				}

				/// @brief Atan2 の結果を返します。
				/// @param y y 座標値
				/// @param x x 座標値
				/// @return 逆正切値（ラジアン）
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& y, const BigInt& x);

				/// @brief Atan2 の結果を返します。
				/// @param y y 座標値
				/// @param x x 座標値
				/// @return 逆正切値（ラジアン）
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& y, const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Cos_impl
			//
			////////////////////////////////////////////////////////////////

			struct Cos_impl
			{
				/// @brief 余弦を返します。
				/// @param x 入力値（ラジアン）
				/// @return 余弦値
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::cos(static_cast<double>(x));
				}

				/// @brief 余弦を返します。
				/// @param x 入力値（ラジアン）
				/// @return 余弦値
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::cos(x);
				}

				/// @brief 余弦を返します。
				/// @param x 入力値（ラジアン）
				/// @return 余弦値
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::cos(x);
				}

				/// @brief ベクトルの各成分の余弦を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の余弦値
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::cos(v.x), std::cos(v.y) };
				}

				/// @brief ベクトルの各成分の余弦を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の余弦値
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::cos(v.x), std::cos(v.y) };
				}

				/// @brief ベクトルの各成分の余弦を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の余弦値
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::cos(v.x), std::cos(v.y), std::cos(v.z) };
				}

				/// @brief ベクトルの各成分の余弦を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の余弦値
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::cos(v.x), std::cos(v.y), std::cos(v.z) };
				}

				/// @brief ベクトルの各成分の余弦を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の余弦値
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::cos(v.x), std::cos(v.y), std::cos(v.z), std::cos(v.w) };
				}

				/// @brief ベクトルの各成分の余弦を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の余弦値
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::cos(v.x), std::cos(v.y), std::cos(v.z), std::cos(v.w) };
				}

				/// @brief 余弦を返します。
				/// @param x 入力値（ラジアン）
				/// @return 余弦値
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief 余弦を返します。
				/// @param x 入力値（ラジアン）
				/// @return 余弦値
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Cosh_impl
			//
			////////////////////////////////////////////////////////////////

			struct Cosh_impl
			{
				/// @brief 双曲線余弦を返します。
				/// @param x 入力値
				/// @return 双曲線余弦値
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::cosh(static_cast<double>(x));
				}

				/// @brief 双曲線余弦を返します。
				/// @param x 入力値
				/// @return 双曲線余弦値
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::cosh(x);
				}

				/// @brief 双曲線余弦を返します。
				/// @param x 入力値
				/// @return 双曲線余弦値
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::cosh(x);
				}

				/// @brief ベクトルの各成分の双曲線余弦を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線余弦値
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::cosh(v.x), std::cosh(v.y) };
				}

				/// @brief ベクトルの各成分の双曲線余弦を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線余弦値
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::cosh(v.x), std::cosh(v.y) };
				}

				/// @brief ベクトルの各成分の双曲線余弦を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線余弦値
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::cosh(v.x), std::cosh(v.y), std::cosh(v.z) };
				}

				/// @brief ベクトルの各成分の双曲線余弦を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線余弦値
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::cosh(v.x), std::cosh(v.y), std::cosh(v.z) };
				}

				/// @brief ベクトルの各成分の双曲線余弦を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線余弦値
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::cosh(v.x), std::cosh(v.y), std::cosh(v.z), std::cosh(v.w) };
				}

				/// @brief ベクトルの各成分の双曲線余弦を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線余弦値
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::cosh(v.x), std::cosh(v.y), std::cosh(v.z), std::cosh(v.w) };
				}

				/// @brief 双曲線余弦を返します。
				/// @param x 入力値
				/// @return 双曲線余弦値
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief 双曲線余弦を返します。
				/// @param x 入力値
				/// @return 双曲線余弦値
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Sin_impl
			//
			////////////////////////////////////////////////////////////////

			struct Sin_impl
			{
				/// @brief 正弦を返します。
				/// @param x 入力値（ラジアン）
				/// @return 正弦値
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::sin(static_cast<double>(x));
				}

				/// @brief 正弦を返します。
				/// @param x 入力値（ラジアン）
				/// @return 正弦値
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::sin(x);
				}

				/// @brief 正弦を返します。
				/// @param x 入力値（ラジアン）
				/// @return 正弦値
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::sin(x);
				}

				/// @brief ベクトルの各成分の正弦を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の正弦値
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::sin(v.x), std::sin(v.y) };
				}

				/// @brief ベクトルの各成分の正弦を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の正弦値
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::sin(v.x), std::sin(v.y) };
				}

				/// @brief ベクトルの各成分の正弦を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の正弦値
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::sin(v.x), std::sin(v.y), std::sin(v.z) };
				}

				/// @brief ベクトルの各成分の正弦を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の正弦値
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::sin(v.x), std::sin(v.y), std::sin(v.z) };
				}

				/// @brief ベクトルの各成分の正弦を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の正弦値
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::sin(v.x), std::sin(v.y), std::sin(v.z), std::sin(v.w) };
				}

				/// @brief ベクトルの各成分の正弦を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の正弦値
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::sin(v.x), std::sin(v.y), std::sin(v.z), std::sin(v.w) };
				}

				/// @brief 正弦を返します。
				/// @param x 入力値（ラジアン）
				/// @return 正弦値
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief 正弦を返します。
				/// @param x 入力値（ラジアン）
				/// @return 正弦値
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Sinh_impl
			//
			////////////////////////////////////////////////////////////////

			struct Sinh_impl
			{
				/// @brief 双曲線正弦を返します。
				/// @param x 入力値
				/// @return 双曲線正弦値
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::sinh(static_cast<double>(x));
				}

				/// @brief 双曲線正弦を返します。
				/// @param x 入力値
				/// @return 双曲線正弦値
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::sinh(x);
				}

				/// @brief 双曲線正弦を返します。
				/// @param x 入力値
				/// @return 双曲線正弦値
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::sinh(x);
				}

				/// @brief ベクトルの各成分の双曲線正弦を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線正弦値
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::sinh(v.x), std::sinh(v.y) };
				}

				/// @brief ベクトルの各成分の双曲線正弦を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線正弦値
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::sinh(v.x), std::sinh(v.y) };
				}

				/// @brief ベクトルの各成分の双曲線正弦を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線正弦値
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::sinh(v.x), std::sinh(v.y), std::sinh(v.z) };
				}

				/// @brief ベクトルの各成分の双曲線正弦を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線正弦値
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::sinh(v.x), std::sinh(v.y), std::sinh(v.z) };
				}

				/// @brief ベクトルの各成分の双曲線正弦を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線正弦値
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::sinh(v.x), std::sinh(v.y), std::sinh(v.z), std::sinh(v.w) };
				}

				/// @brief ベクトルの各成分の双曲線正弦を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線正弦値
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::sinh(v.x), std::sinh(v.y), std::sinh(v.z), std::sinh(v.w) };
				}

				/// @brief 双曲線正弦を返します。
				/// @param x 入力値
				/// @return 双曲線正弦値
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief 双曲線正弦を返します。
				/// @param x 入力値
				/// @return 双曲線正弦値
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Tan_impl
			//
			////////////////////////////////////////////////////////////////

			struct Tan_impl
			{
				/// @brief 正切を返します。
				/// @param x 入力値（ラジアン）
				/// @return 正切値
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::tan(static_cast<double>(x));
				}

				/// @brief 正切を返します。
				/// @param x 入力値（ラジアン）
				/// @return 正切値
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::tan(x);
				}

				/// @brief 正切を返します。
				/// @param x 入力値（ラジアン）
				/// @return 正切値
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::tan(x);
				}

				/// @brief ベクトルの各成分の正切を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の正切値
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::tan(v.x), std::tan(v.y) };
				}

				/// @brief ベクトルの各成分の正切を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の正切値
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::tan(v.x), std::tan(v.y) };
				}

				/// @brief ベクトルの各成分の正切を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の正切値
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::tan(v.x), std::tan(v.y), std::tan(v.z) };
				}

				/// @brief ベクトルの各成分の正切を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の正切値
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::tan(v.x), std::tan(v.y), std::tan(v.z) };
				}

				/// @brief ベクトルの各成分の正切を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の正切値
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::tan(v.x), std::tan(v.y), std::tan(v.z), std::tan(v.w) };
				}

				/// @brief ベクトルの各成分の正切を返します。
				/// @param v ベクトル（ラジアン）
				/// @return 各成分の正切値
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::tan(v.x), std::tan(v.y), std::tan(v.z), std::tan(v.w) };
				}

				/// @brief 正切を返します。
				/// @param x 入力値（ラジアン）
				/// @return 正切値
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief 正切を返します。
				/// @param x 入力値（ラジアン）
				/// @return 正切値
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Tanh_impl
			//
			////////////////////////////////////////////////////////////////

			struct Tanh_impl
			{
				/// @brief 双曲線正切を返します。
				/// @param x 入力値
				/// @return 双曲線正切値
				[[nodiscard]]
				static double operator ()(const Concept::Integral auto x) noexcept
				{
					return std::tanh(static_cast<double>(x));
				}

				/// @brief 双曲線正切を返します。
				/// @param x 入力値
				/// @return 双曲線正切値
				[[nodiscard]]
				static float operator ()(const float x) noexcept
				{
					return std::tanh(x);
				}

				/// @brief 双曲線正切を返します。
				/// @param x 入力値
				/// @return 双曲線正切値
				[[nodiscard]]
				static double operator ()(const double x) noexcept
				{
					return std::tanh(x);
				}

				/// @brief ベクトルの各成分の双曲線正切を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線正切値
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ std::tanh(v.x), std::tanh(v.y) };
				}

				/// @brief ベクトルの各成分の双曲線正切を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線正切値
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ std::tanh(v.x), std::tanh(v.y) };
				}

				/// @brief ベクトルの各成分の双曲線正切を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線正切値
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ std::tanh(v.x), std::tanh(v.y), std::tanh(v.z) };
				}

				/// @brief ベクトルの各成分の双曲線正切を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線正切値
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ std::tanh(v.x), std::tanh(v.y), std::tanh(v.z) };
				}

				/// @brief ベクトルの各成分の双曲線正切を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線正切値
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ std::tanh(v.x), std::tanh(v.y), std::tanh(v.z), std::tanh(v.w) };
				}

				/// @brief ベクトルの各成分の双曲線正切を返します。
				/// @param v ベクトル
				/// @return 各成分の双曲線正切値
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ std::tanh(v.x), std::tanh(v.y), std::tanh(v.z), std::tanh(v.w) };
				}

				/// @brief 双曲線正切を返します。
				/// @param x 入力値
				/// @return 双曲線正切値
				[[nodiscard]]
				static BigFloat operator ()(const BigInt& x);

				/// @brief 双曲線正切を返します。
				/// @param x 入力値
				/// @return 双曲線正切値
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Dot_impl
			//
			////////////////////////////////////////////////////////////////

			struct Dot_impl
			{
				/// @brief ベクトルの内積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 内積値
				[[nodiscard]]
				static constexpr float operator ()(const Float2 a, const Float2 b) noexcept
				{
					return ((a.x * b.x) + (a.y * b.y));
				}

				/// @brief ベクトルの内積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 内積値
				[[nodiscard]]
				static constexpr double operator ()(const Float2 a, const Vec2 b) noexcept
				{
					return ((a.x * b.x) + (a.y * b.y));
				}
				
				/// @brief ベクトルの内積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 内積値
				[[nodiscard]]
				static constexpr double operator ()(const Vec2 a, const Float2 b) noexcept
				{
					return ((a.x * b.x) + (a.y * b.y));
				}

				/// @brief ベクトルの内積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 内積値
				[[nodiscard]]
				static constexpr double operator ()(const Vec2 a, const Vec2 b) noexcept
				{
					return ((a.x * b.x) + (a.y * b.y));
				}

				/// @brief ベクトルの内積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 内積値
				[[nodiscard]]
				static constexpr float operator ()(const Float3 a, const Float3 b) noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
				}

				/// @brief ベクトルの内積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 内積値
				[[nodiscard]]
				static constexpr double operator ()(const Float3 a, const Vec3 b) noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
				}

				/// @brief ベクトルの内積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 内積値
				[[nodiscard]]
				static constexpr double operator ()(const Vec3 a, const Float3 b) noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
				}

				/// @brief ベクトルの内積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 内積値
				[[nodiscard]]
				static constexpr double operator ()(const Vec3 a, const Vec3 b) noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
				}

				/// @brief ベクトルの内積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 内積値
				[[nodiscard]]
				static constexpr float operator ()(const Float4 a, const Float4 b) noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
				}

				/// @brief ベクトルの内積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 内積値
				[[nodiscard]]
				static constexpr double operator ()(const Float4 a, const Vec4 b) noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
				}

				/// @brief ベクトルの内積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 内積値
				[[nodiscard]]
				static constexpr double operator ()(const Vec4 a, const Float4 b) noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
				}

				/// @brief ベクトルの内積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 内積値
				[[nodiscard]]
				static constexpr double operator ()(const Vec4 a, const Vec4 b) noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
				}
			};

			////////////////////////////////////////////////////////////////
			//
			//	Cross_impl
			//
			////////////////////////////////////////////////////////////////

			struct Cross_impl
			{
				/// @brief 2 次元ベクトルの外積（スカラー）を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 外積値
				[[nodiscard]]
				static constexpr float operator ()(const Float2 a, const Float2 b) noexcept
				{
					return ((a.x * b.y) - (a.y * b.x));
				}

				/// @brief 2 次元ベクトルの外積（スカラー）を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 外積値
				[[nodiscard]]
				static constexpr double operator ()(const Float2 a, const Vec2 b) noexcept
				{
					return ((a.x * b.y) - (a.y * b.x));
				}

				/// @brief ベクトルの外積（スカラー）を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 外積値
				[[nodiscard]]
				static constexpr double operator ()(const Vec2 a, const Float2 b) noexcept
				{
					return ((a.x * b.y) - (a.y * b.x));
				}

				/// @brief ベクトルの外積（スカラー）を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 外積値
				[[nodiscard]]
				static constexpr double operator ()(const Vec2 a, const Vec2 b) noexcept
				{
					return ((a.x * b.y) - (a.y * b.x));
				}

				/// @brief 3 次元ベクトルの外積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 外積ベクトル
				[[nodiscard]]
				static constexpr Float3 operator ()(const Float3 a, const Float3 b) noexcept
				{
					return{ ((a.y * b.z) - (a.z * b.y)), ((a.z * b.x) - (a.x * b.z)), ((a.x * b.y) - (a.y * b.x)) };
				}

				/// @brief 3 次元ベクトルの外積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 外積ベクトル
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Float3 a, const Vec3 b) noexcept
				{
					return{ ((a.y * b.z) - (a.z * b.y)), ((a.z * b.x) - (a.x * b.z)), ((a.x * b.y) - (a.y * b.x)) };
				}

				/// @brief ベクトルの外積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 外積ベクトル
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Vec3 a, const Float3 b) noexcept
				{
					return{ ((a.y * b.z) - (a.z * b.y)), ((a.z * b.x) - (a.x * b.z)), ((a.x * b.y) - (a.y * b.x)) };
				}

				/// @brief ベクトルの外積を返します。
				/// @param a 第 1 のベクトル
				/// @param b 第 2 のベクトル
				/// @return 外積ベクトル
				[[nodiscard]]
				static constexpr Vec3 operator ()(const Vec3 a, const Vec3 b) noexcept
				{
					return{ ((a.y * b.z) - (a.z * b.y)), ((a.z * b.x) - (a.x * b.z)), ((a.x * b.y) - (a.y * b.x)) };
				}
			};

			////////////////////////////////////////////////////////////////
			//
			//	Normalize_impl
			//
			////////////////////////////////////////////////////////////////

			struct Normalize_impl
			{
				/// @brief 値を符号方向に正規化した値を返します。
				/// @param x 入力値
				/// @return x < 0 の場合は -1, x == 0 の場合は 0, x > 0 の場合は 1
				[[nodiscard]]
				static constexpr float operator ()(const float x) noexcept
				{
					if (x < 0.0f)
					{
						return -1.0f;
					}
					else if (0.0f < x)
					{
						return 1.0f;
					}

					return 0.0f;
				}

				/// @brief 値を符号方向に正規化した値を返します。
				/// @param x 入力値
				/// @return x < 0 の場合は -1, x == 0 の場合は 0, x > 0 の場合は 1
				[[nodiscard]]
				static constexpr double operator ()(const double x) noexcept
				{
					if (x < 0.0)
					{
						return -1.0;
					}
					else if (0.0 < x)
					{
						return 1.0;
					}

					return 0.0;
				}

				/// @brief 正規化したベクトルを返します。
				/// @param v 入力ベクトル
				/// @return 正規化したベクトル。ゼロベクトルの場合はゼロベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Point v) noexcept
				{
					return Vec2{ v }.normalized();
				}

				/// @brief 正規化したベクトルを返します。
				/// @param v 入力ベクトル
				/// @return 正規化したベクトル。ゼロベクトルの場合はゼロベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return v.normalized();
				}

				/// @brief 正規化したベクトルを返します。
				/// @param v 入力ベクトル
				/// @return 正規化したベクトル。ゼロベクトルの場合はゼロベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return v.normalized();
				}

				/// @brief 正規化したベクトルを返します。
				/// @param v 入力ベクトル
				/// @return 正規化したベクトル。ゼロベクトルの場合はゼロベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return v.normalized();
				}

				/// @brief 正規化したベクトルを返します。
				/// @param v 入力ベクトル
				/// @return 正規化したベクトル。ゼロベクトルの場合はゼロベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return v.normalized();
				}

				/// @brief 正規化したベクトルを返します。
				/// @param v 入力ベクトル
				/// @return 正規化したベクトル。ゼロベクトルの場合はゼロベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return v.normalized();
				}

				/// @brief 正規化したベクトルを返します。
				/// @param v 入力ベクトル
				/// @return 正規化したベクトル。ゼロベクトルの場合はゼロベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return v.normalized();
				}

				/// @brief 正規化した値を返します。
				/// @param x 入力値
				/// @return 正規化値（0 の場合は 0, それ以外は 1）
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	Smoothstep_impl
			//
			////////////////////////////////////////////////////////////////

			struct Smoothstep_impl
			{
				/// @brief 2 つの値の間をエルミート補間した値を返します。
				/// @param min 範囲の最小値
				/// @param max 範囲の最大値
				/// @param t 補間パラメータ
				/// @return 補間値
				[[nodiscard]]
				static constexpr float operator ()(const float min, const float max, float t) noexcept
				{
					if (t <= min)
					{
						return 0.0f;
					}
					else if (max <= t)
					{
						return 1.0f;
					}

					t = ((t - min) / (max - min));

					return (t * t * (3.0f - 2.0f * t));
				}

				/// @brief 2 つの値の間をエルミート補間した値を返します。
				/// @param min 範囲の最小値
				/// @param max 範囲の最大値
				/// @param t 補間パラメータ
				/// @return 補間値
				[[nodiscard]]
				static constexpr double operator ()(const double min, const double max, double t) noexcept
				{
					if (t <= min)
					{
						return 0.0;
					}
					else if (max <= t)
					{
						return 1.0;
					}

					t = ((t - min) / (max - min));

					return (t * t * (3.0 - 2.0 * t));
				}

				/// @brief 0.0 から 1.0 の間をエルミート補間した値を返します。
				/// @param x 補間パラメータ
				/// @return 補間値
				[[nodiscard]]
				static constexpr float operator ()(const float x) noexcept
				{
					if (x <= 0.0f)
					{
						return 0.0f;
					}
					else if (1.0f <= x)
					{
						return 1.0f;
					}

					return (x * x * (3.0f - 2.0f * x));
				}

				/// @brief 0.0 から 1.0 の間をエルミート補間した値を返します。
				/// @param x 補間パラメータ
				/// @return 補間値
				[[nodiscard]]
				static constexpr double operator ()(const double x) noexcept
				{
					if (x <= 0.0)
					{
						return 0.0;
					}
					else if (1.0 <= x)
					{
						return 1.0;
					}

					return (x * x * (3.0 - 2.0 * x));
				}

				/// @brief ベクトルの各成分について、2 つの値の間をエルミート補間したベクトルを返します。
				/// @param min 範囲の最小値
				/// @param max 範囲の最大値
				/// @param t 補間パラメータ
				/// @return 補間ベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 min, const Float2 max, const Float2 t) noexcept
				{
					return{ operator ()(min.x, max.x, t.x), operator ()(min.y, max.y, t.y) };
				}

				/// @brief ベクトルの各成分について、2 つの値の間をエルミート補間したベクトルを返します。
				/// @param min 範囲の最小値
				/// @param max 範囲の最大値
				/// @param t 補間パラメータ
				/// @return 補間ベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 min, const Vec2 max, const Vec2 t) noexcept
				{
					return{ operator ()(min.x, max.x, t.x), operator ()(min.y, max.y, t.y) };
				}

				/// @brief ベクトルの各成分について、0.0 から 1.0 の間をエルミート補間したベクトルを返します。
				/// @param v 補間パラメータ
				/// @return 補間ベクトル
				[[nodiscard]]
				static Float2 operator ()(const Float2 v) noexcept
				{
					return{ operator ()(v.x), operator ()(v.y) };
				}

				/// @brief ベクトルの各成分について、0.0 から 1.0 の間をエルミート補間したベクトルを返します。
				/// @param v 補間パラメータ
				/// @return 補間ベクトル
				[[nodiscard]]
				static Vec2 operator ()(const Vec2 v) noexcept
				{
					return{ operator ()(v.x), operator ()(v.y) };
				}

				/// @brief ベクトルの各成分について、2 つの値の間をエルミート補間したベクトルを返します。
				/// @param min 範囲の最小値
				/// @param max 範囲の最大値
				/// @param t 補間パラメータ
				/// @return 補間ベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 min, const Float3 max, const Float3 t) noexcept
				{
					return{ operator ()(min.x, max.x, t.x), operator ()(min.y, max.y, t.y), operator ()(min.z, max.z, t.z) };
				}

				/// @brief ベクトルの各成分について、2 つの値の間をエルミート補間したベクトルを返します。
				/// @param min 範囲の最小値
				/// @param max 範囲の最大値
				/// @param t 補間パラメータ
				/// @return 補間ベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 min, const Vec3 max, const Vec3 t) noexcept
				{
					return{ operator ()(min.x, max.x, t.x), operator ()(min.y, max.y, t.y), operator ()(min.z, max.z, t.z) };
				}

				/// @brief ベクトルの各成分について、0.0 から 1.0 の間をエルミート補間したベクトルを返します。
				/// @param v 補間パラメータ
				/// @return 補間ベクトル
				[[nodiscard]]
				static Float3 operator ()(const Float3 v) noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z) };
				}

				/// @brief ベクトルの各成分について、0.0 から 1.0 の間をエルミート補間したベクトルを返します。
				/// @param v 補間パラメータ
				/// @return 補間ベクトル
				[[nodiscard]]
				static Vec3 operator ()(const Vec3 v) noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z) };
				}

				/// @brief ベクトルの各成分について、2 つの値の間をエルミート補間したベクトルを返します。
				/// @param min 範囲の最小値
				/// @param max 範囲の最大値
				/// @param t 補間パラメータ
				/// @return 補間ベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 min, const Float4 max, const Float4 t) noexcept
				{
					return{ operator ()(min.x, max.x, t.x), operator ()(min.y, max.y, t.y), operator ()(min.z, max.z, t.z), operator ()(min.w, max.w, t.w) };
				}

				/// @brief ベクトルの各成分について、2 つの値の間をエルミート補間したベクトルを返します。
				/// @param min 範囲の最小値
				/// @param max 範囲の最大値
				/// @param t 補間パラメータ
				/// @return 補間ベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 min, const Vec4 max, const Vec4 t) noexcept
				{
					return{ operator ()(min.x, max.x, t.x), operator ()(min.y, max.y, t.y), operator ()(min.z, max.z, t.z), operator ()(min.w, max.w, t.w) };
				}

				/// @brief ベクトルの各成分について、0.0 から 1.0 の間をエルミート補間したベクトルを返します。
				/// @param v 補間パラメータ
				/// @return 補間ベクトル
				[[nodiscard]]
				static Float4 operator ()(const Float4 v) noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z), operator ()(v.w) };
				}

				/// @brief ベクトルの各成分について、0.0 から 1.0 の間をエルミート補間したベクトルを返します。
				/// @param v 補間パラメータ
				/// @return 補間ベクトル
				[[nodiscard]]
				static Vec4 operator ()(const Vec4 v) noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z), operator ()(v.w) };
				}

				/// @brief 2 つの値の間をエルミート補間した値を返します。
				/// @param min 範囲の最小値
				/// @param max 範囲の最大値
				/// @param t 補間パラメータ
				/// @return 補間値
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& min, const BigFloat& max, const BigFloat& t);

				/// @brief 0.0 から 1.0 の間をエルミート補間した値を返します。
				/// @param x 補間パラメータ
				/// @return 補間値
				[[nodiscard]]
				static BigFloat operator ()(const BigFloat& x);
			};

			////////////////////////////////////////////////////////////////
			//
			//	ClampAngle_impl
			//
			////////////////////////////////////////////////////////////////

			struct ClampAngle_impl
			{
				/// @brief 角度を 2π 周期で正規化し、[min, max] の範囲にクランプした角度を返します。
				/// @param angle 角度（ラジアン）
				/// @param min 最小角度（ラジアン）
				/// @param max 最大角度（ラジアン）
				/// @return 正規化・クランプ後の角度
				[[nodiscard]]
				static double operator ()(const Concept::Arithmetic auto angle, const Concept::Arithmetic auto min, const Concept::Arithmetic auto max) noexcept
				{
					const auto start = ((min + max) * 0.5 - Pi);
					
					const auto floor = (std::floor((angle - start) / TwoPi) * TwoPi);
					
					return Clamp(angle, (min + floor), (max + floor));
				}
				
				/// @brief 角度を 2π 周期で正規化し、[min, max] の範囲にクランプした角度を返します。
				/// @param angle 角度（ラジアン）
				/// @param min 最小角度（ラジアン）
				/// @param max 最大角度（ラジアン）
				/// @return 正規化・クランプ後の角度
				[[nodiscard]]
				static float operator ()(const float angle, const float min, const float max) noexcept
				{
					const float start = ((min + max) * 0.5f - PiF);
					
					const float floor = (std::floor((angle - start) / TwoPiF) * TwoPiF);
					
					return Clamp(angle, (min + floor), (max + floor));
				}

				/// @brief 角度を 2π 周期で正規化し、[min, max] の範囲にクランプした角度を返します。
				/// @param angle 角度（ラジアン）
				/// @param min 最小角度（ラジアン）
				/// @param max 最大角度（ラジアン）
				/// @return 正規化・クランプ後の角度
				[[nodiscard]]
				static double operator ()(const double angle, const double min, const double max) noexcept
				{
					const double start = ((min + max) * 0.5 - Pi);
					
					const double floor = (std::floor((angle - start) / TwoPi) * TwoPi);
					
					return Clamp(angle, (min + floor), (max + floor));
				}
			};

			////////////////////////////////////////////////////////////////
			//
			//	NormalizeAngle_impl
			//
			////////////////////////////////////////////////////////////////

			struct NormalizeAngle_impl
			{
				/// @brief 指定した中心角度から ±π の範囲に収まるように角度を正規化して返します。
				/// @param radian_ 角度（ラジアン）
				/// @param center 中心角度（ラジアン）
				/// @return 正規化後の角度（ラジアン）
				[[nodiscard]]
				static double operator ()(const Concept::Arithmetic auto radian_, double center = Pi) noexcept
				{
					double radian = std::fmod(radian_ + (Pi - center), TwoPi);

					if (radian < 0.0)
					{
						radian += TwoPi;
					}

					return (radian - (Pi - center));
				}
				
				/// @brief 指定した中心角度から ±π の範囲に収まるように角度を正規化して返します。
				/// @param radian_ 角度（ラジアン）
				/// @param center 中心角度（ラジアン）
				/// @return 正規化後の角度（ラジアン）
				[[nodiscard]]
				static float operator ()(const float radian_, float center = PiF) noexcept
				{
					float radian = std::fmod(radian_ + (PiF - center), TwoPiF);

					if (radian < 0.0f)
					{
						radian += TwoPiF;
					}

					return (radian - (PiF - center));
				}

				/// @brief 指定した中心角度から ±π の範囲に収まるように角度を正規化して返します。
				/// @param radian_ 角度（ラジアン）
				/// @param center 中心角度（ラジアン）
				/// @return 正規化後の角度（ラジアン）
				[[nodiscard]]
				static double operator ()(const double radian_, double center = Pi) noexcept
				{
					double radian = std::fmod(radian_ + (Pi - center), TwoPi);

					if (radian < 0.0)
					{
						radian += TwoPi;
					}

					return (radian - (Pi - center));
				}
			};

			////////////////////////////////////////////////////////////////
			//
			//	GCD_impl
			//
			////////////////////////////////////////////////////////////////

			struct GCD_impl
			{
				/// @brief 最大公約数を返します。
				/// @param a 一方の整数
				/// @param b もう一方の整数
				/// @return 最大公約数
				[[nodiscard]]
				static constexpr auto operator ()(const Concept::Integral auto a, const Concept::Integral auto b) noexcept
				{
					return std::gcd(a, b);
				}

				/// @brief 最大公約数を返します。
				/// @param a 一方の整数
				/// @param b もう一方の整数
				/// @return 最大公約数
				[[nodiscard]]
				static BigInt operator ()(const BigInt& a, const BigInt& b);
			};

			////////////////////////////////////////////////////////////////
			//
			//	LCM_impl
			//
			////////////////////////////////////////////////////////////////

			struct LCM_impl
			{
				/// @brief 最小公倍数を返します。
				/// @param a 一方の整数
				/// @param b もう一方の整数
				/// @return 最小公倍数
				[[nodiscard]]
				static constexpr auto operator ()(const Concept::Integral auto a, const Concept::Integral auto b) noexcept
				{
					return std::lcm(a, b);
				}

				/// @brief 最小公倍数を返します。
				/// @param a 一方の整数
				/// @param b もう一方の整数
				/// @return 最小公倍数
				[[nodiscard]]
				static BigInt operator ()(const BigInt& a, const BigInt& b);
			};
		}
	}
}
