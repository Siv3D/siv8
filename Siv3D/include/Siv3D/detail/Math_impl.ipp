//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
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
				/// @brief 浮動小数点剩余演算を実行します。
				/// @param x 被除数
				/// @param y 除数
				/// @return x を y で割った剰余
				[[nodiscard]]
				double operator ()(const Concept::Arithmetic auto x, const Concept::Arithmetic auto y) const noexcept
				{
					return std::fmod(static_cast<double>(x), static_cast<double>(y));
				}

				/// @brief 浮動小数点剩余演算を実行します。
				/// @param x 被除数
				/// @param y 除数
				/// @return x を y で割った剰余
				[[nodiscard]]
				float operator ()(const float x, const float y) const noexcept
				{
					return std::fmod(x, y);
				}

				/// @brief 浮動小数点剩余演算を実行します。
				/// @param x 被除数
				/// @param y 除数
				/// @return x を y で割った剰余
				[[nodiscard]]
				double operator ()(const double x, const double y) const noexcept
				{
					return std::fmod(x, y);
				}

				/// @brief 浮動小数点剩余演算を要素ごとに実行します。
				/// @param x 被除数ベクトル
				/// @param y 除数ベクトル
				/// @return 各要素について x を y で割った剰余
				[[nodiscard]]
				Float2 operator ()(const Float2 x, const Float2 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y) };
				}

				/// @brief 浮動小数点剩余演算を要素ごとに実行します。
				/// @param x 被除数ベクトル
				/// @param y 除数ベクトル
				/// @return 各要素について x を y で割った剰余
				[[nodiscard]]
				Vec2 operator ()(const Vec2 x, const Vec2 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y) };
				}

				/// @brief 浮動小数点剩余演算を要素ごとに実行します。
				/// @param x 被除数ベクトル
				/// @param y 除数ベクトル
				/// @return 各要素について x を y で割った剰余
				[[nodiscard]]
				Float3 operator ()(const Float3 x, const Float3 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z) };
				}

				/// @brief 浮動小数点剩余演算を要素ごとに実行します。
				/// @param x 被除数ベクトル
				/// @param y 除数ベクトル
				/// @return 各要素について x を y で割った剰余
				[[nodiscard]]
				Vec3 operator ()(const Vec3 x, const Vec3 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z) };
				}

				/// @brief 浮動小数点剩余演算を要素ごとに実行します。
				/// @param x 被除数ベクトル
				/// @param y 除数ベクトル
				/// @return 各要素について x を y で割った剰余
				[[nodiscard]]
				Float4 operator ()(const Float4 x, const Float4 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z), std::fmod(x.w, y.w) };
				}

				/// @brief 浮動小数点剩余演算を要素ごとに実行します。
				/// @param x 被除数ベクトル
				/// @param y 除数ベクトル
				/// @return 各要素について x を y で割った剰余
				[[nodiscard]]
				Vec4 operator ()(const Vec4 x, const Vec4 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z), std::fmod(x.w, y.w) };
				}

				/// @brief 浮動小数点剩余演算を実行します。
				/// @param x 被除数
				/// @param y 除数
				/// @return x を y で割った剰余
				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x, const BigFloat& y) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Fraction_impl
			//
			////////////////////////////////////////////////////////////////

			struct Fraction_impl
			{
				/// @brief 値の小数部を抽出します。
				/// @param x 入力値
				/// @return x の小数部
				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return (x - std::floor(x));
				}

				/// @brief 値の小数部を抽出します。
				/// @param x 入力値
				/// @return x の小数部
				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return (x - std::floor(x));
				}

				/// @brief ベクトルの各要素の小数部を抽出します。
				/// @param v 入力ベクトル
				/// @return 各要素の小数部
				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)) };
				}

				/// @brief ベクトルの各要素の小数部を抽出します。
				/// @param v 入力ベクトル
				/// @return 各要素の小数部
				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)) };
				}

				/// @brief ベクトルの各要素の小数部を抽出します。
				/// @param v 入力ベクトル
				/// @return 各要素の小数部
				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)), (v.z - std::floor(v.z)) };
				}

				/// @brief ベクトルの各要素の小数部を抽出します。
				/// @param v 入力ベクトル
				/// @return 各要素の小数部
				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)), (v.z - std::floor(v.z)) };
				}

				/// @brief ベクトルの各要素の小数部を抽出します。
				/// @param v 入力ベクトル
				/// @return 各要素の小数部
				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)), (v.z - std::floor(v.z)), (v.w - std::floor(v.w)) };
				}

				/// @brief ベクトルの各要素の小数部を抽出します。
				/// @param v 入力ベクトル
				/// @return 各要素の小数部
				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ (v.x - std::floor(v.x)), (v.y - std::floor(v.y)), (v.z - std::floor(v.z)), (v.w - std::floor(v.w)) };
				}

				/// @brief 値の小数部を抽出します。
				/// @param x 入力値
				/// @return x の小数部
				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Frexp_impl
			//
			////////////////////////////////////////////////////////////////

			struct Frexp_impl
			{
				/// @brief 浮動小数点値を仮数と指数部に分解します。
				/// @param x 入力値
				/// @param exp 指数部を格納する参照
				/// @return 仮数部 (0.5 以上 1.0 未満)
				[[nodiscard]]
				float operator ()(const float x, float& exp) const noexcept
				{
					int32 e;
					const auto t = std::frexp(x, &e);
					exp = static_cast<float>(e);
					return t;
				}

				/// @brief 浮動小数点値を仮数と指数部に分解します。
				/// @param x 入力値
				/// @param exp 指数部を格納する参照
				/// @return 仮数部 (0.5 以上 1.0 未満)
				[[nodiscard]]
				double operator ()(const double x, double& exp) const noexcept
				{
					int32 e;
					const auto t = std::frexp(x, &e);
					exp = static_cast<double>(e);
					return t;
				}

				/// @brief ベクトルの各要素を仮数と指数部に分解します。
				/// @param v 入力ベクトル
				/// @param exp 指数部を格納するベクトルの参照
				/// @return 仮数部ベクトル
				[[nodiscard]]
				Float2 operator ()(const Float2 v, Float2& exp) const noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y) };
				}

				/// @brief ベクトルの各要素を仮数と指数部に分解します。
				/// @param v 入力ベクトル
				/// @param exp 指数部を格納するベクトルの参照
				/// @return 仮数部ベクトル
				[[nodiscard]]
				Vec2 operator ()(const Vec2 v, Vec2& exp) const noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y) };
				}

				/// @brief ベクトルの各要素を仮数と指数部に分解します。
				/// @param v 入力ベクトル
				/// @param exp 指数部を格納するベクトルの参照
				/// @return 仮数部ベクトル
				[[nodiscard]]
				Float3 operator ()(const Float3 v, Float3& exp) const noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y), operator ()(v.z, exp.z) };
				}

				/// @brief ベクトルの各要素を仮数と指数部に分解します。
				/// @param v 入力ベクトル
				/// @param exp 指数部を格納するベクトルの参照
				/// @return 仮数部ベクトル
				[[nodiscard]]
				Vec3 operator ()(const Vec3 v, Vec3& exp) const noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y), operator ()(v.z, exp.z) };
				}

				/// @brief ベクトルの各要素を仮数と指数部に分解します。
				/// @param v 入力ベクトル
				/// @param exp 指数部を格納するベクトルの参照
				/// @return 仮数部ベクトル
				[[nodiscard]]
				Float4 operator ()(const Float4 v, Float4& exp) const noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y), operator ()(v.z, exp.z), operator ()(v.w, exp.w) };
				}

				/// @brief ベクトルの各要素を仮数と指数部に分解します。
				/// @param v 入力ベクトル
				/// @param exp 指数部を格納するベクトルの参照
				/// @return 仮数部ベクトル
				[[nodiscard]]
				Vec4 operator ()(const Vec4 v, Vec4& exp) const noexcept
				{
					return{ operator ()(v.x, exp.x), operator ()(v.y, exp.y), operator ()(v.z, exp.z), operator ()(v.w, exp.w) };
				}

				/// @brief 高精度浮動小数点値を仮数と指数部に分解します。
				/// @param x 入力値
				/// @param exp 指数部を格納する参照
				/// @return 仮数部
				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x, int32& exp) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Ldexp_impl
			//
			////////////////////////////////////////////////////////////////

			struct Ldexp_impl
			{
				/// @brief 仮数と指数から浮動小数点値を構成します。
				/// @param x 仮数
				/// @param exp 指数
				/// @return x * 2^exp
				[[nodiscard]]
				float operator ()(const float x, const float exp) const noexcept
				{
					return (x * std::exp2(exp));
				}

				/// @brief 仮数と指数から浮動小数点値を構成します。
				/// @param x 仮数
				/// @param exp 指数
				/// @return x * 2^exp
				[[nodiscard]]
				double operator ()(const double x, const double exp) const noexcept
				{
					return (x * std::exp2(exp));
				}

				/// @brief ベクトルの各要素に対して仮数と指数から値を構成します。
				/// @param v 仮数ベクトル
				/// @param exp 指数ベクトル
				/// @return 各要素に対して v * 2^exp を適用したベクトル
				[[nodiscard]]
				Float2 operator ()(const Float2 v, const Float2 exp) const noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)) };
				}

				/// @brief ベクトルの各要素に対して仮数と指数から値を構成します。
				/// @param v 仮数ベクトル
				/// @param exp 指数ベクトル
				/// @return 各要素に対して v * 2^exp を適用したベクトル
				[[nodiscard]]
				Vec2 operator ()(const Vec2 v, const Vec2 exp) const noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)) };
				}

				/// @brief ベクトルの各要素に対して仮数と指数から値を構成します。
				/// @param v 仮数ベクトル
				/// @param exp 指数ベクトル
				/// @return 各要素に対して v * 2^exp を適用したベクトル
				[[nodiscard]]
				Float3 operator ()(const Float3 v, const Float3 exp) const noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)), (v.z * std::exp2(exp.z)) };
				}

				/// @brief ベクトルの各要素に対して仮数と指数から値を構成します。
				/// @param v 仮数ベクトル
				/// @param exp 指数ベクトル
				/// @return 各要素に対して v * 2^exp を適用したベクトル
				[[nodiscard]]
				Vec3 operator ()(const Vec3 v, const Vec3 exp) const noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)), (v.z * std::exp2(exp.z)) };
				}

				/// @brief ベクトルの各要素に対して仮数と指数から値を構成します。
				/// @param v 仮数ベクトル
				/// @param exp 指数ベクトル
				/// @return 各要素に対して v * 2^exp を適用したベクトル
				[[nodiscard]]
				Float4 operator ()(const Float4 v, const Float4 exp) const noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)), (v.z * std::exp2(exp.z)), (v.w * std::exp2(exp.w)) };
				}

				/// @brief ベクトルの各要素に対して仮数と指数から値を構成します。
				/// @param v 仮数ベクトル
				/// @param exp 指数ベクトル
				/// @return 各要素に対して v * 2^exp を適用したベクトル
				[[nodiscard]]
				Vec4 operator ()(const Vec4 v, const Vec4 exp) const noexcept
				{
					return{ (v.x * std::exp2(exp.x)), (v.y * std::exp2(exp.y)), (v.z * std::exp2(exp.z)), (v.w * std::exp2(exp.w)) };
				}

				/// @brief 高精度浮動小数点値で仮数と指数から値を構成します。
				/// @param x 仮数
				/// @param exp 指数
				/// @return x * 2^exp
				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x, const BigFloat& exp) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Log_impl
			//
			////////////////////////////////////////////////////////////////

			struct Log_impl
			{
				/// @brief 整数値の自然対数を計算します。
				/// @param x 入力値
				/// @return ln(x)
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::log(static_cast<double>(x));
				}

				/// @brief 浮動小数点値の自然対数を計算します。
				/// @param x 入力値
				/// @return ln(x)
				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::log(x);
				}

				/// @brief 浮動小数点値の自然対数を計算します。
				/// @param x 入力値
				/// @return ln(x)
				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::log(x);
				}

				/// @brief ベクトルの各要素の自然対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して ln(v) を適用したベクトル
				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::log(v.x), std::log(v.y) };
				}

				/// @brief ベクトルの各要素の自然対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して ln(v) を適用したベクトル
				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::log(v.x), std::log(v.y) };
				}

				/// @brief ベクトルの各要素の自然対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して ln(v) を適用したベクトル
				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::log(v.x), std::log(v.y), std::log(v.z) };
				}

				/// @brief ベクトルの各要素の自然対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して ln(v) を適用したベクトル
				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::log(v.x), std::log(v.y), std::log(v.z) };
				}

				/// @brief ベクトルの各要素の自然対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して ln(v) を適用したベクトル
				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::log(v.x), std::log(v.y), std::log(v.z), std::log(v.w) };
				}

				/// @brief ベクトルの各要素の自然対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して ln(v) を適用したベクトル
				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::log(v.x), std::log(v.y), std::log(v.z), std::log(v.w) };
				}

				/// @brief 高精度浮動小数点値の自然対数を計算します。
				/// @param x 入力値
				/// @return ln(x)
				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Log2_impl
			//
			////////////////////////////////////////////////////////////////

			struct Log2_impl
			{
				/// @brief 整数値の2を底とする対数を計算します。
				/// @param x 入力値
				/// @return log2(x)
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::log2(static_cast<double>(x));
				}

				/// @brief 浮動小数点値の2を底とする対数を計算します。
				/// @param x 入力値
				/// @return log2(x)
				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::log2(x);
				}

				/// @brief 浮動小数点値の2を底とする対数を計算します。
				/// @param x 入力値
				/// @return log2(x)
				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::log2(x);
				}

				/// @brief ベクトルの各要素の2を底とする対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して log2(v) を適用したベクトル
				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::log2(v.x), std::log2(v.y) };
				}

				/// @brief ベクトルの各要素の2を底とする対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して log2(v) を適用したベクトル
				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::log2(v.x), std::log2(v.y) };
				}

				/// @brief ベクトルの各要素の2を底とする対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して log2(v) を適用したベクトル
				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::log2(v.x), std::log2(v.y), std::log2(v.z) };
				}

				/// @brief ベクトルの各要素の2を底とする対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して log2(v) を適用したベクトル
				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::log2(v.x), std::log2(v.y), std::log2(v.z) };
				}

				/// @brief ベクトルの各要素の2を底とする対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して log2(v) を適用したベクトル
				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::log2(v.x), std::log2(v.y), std::log2(v.z), std::log2(v.w) };
				}

				/// @brief ベクトルの各要素の2を底とする対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して log2(v) を適用したベクトル
				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::log2(v.x), std::log2(v.y), std::log2(v.z), std::log2(v.w) };
				}

				/// @brief 高精度浮動小数点値の2を底とする対数を計算します。
				/// @param x 入力値
				/// @return log2(x)
				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Log10_impl
			//
			////////////////////////////////////////////////////////////////

			struct Log10_impl
			{
				/// @brief 整数値の10を底とする対数を計算します。
				/// @param x 入力値
				/// @return log10(x)
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::log10(static_cast<double>(x));
				}

				/// @brief 浮動小数点値の10を底とする対数を計算します。
				/// @param x 入力値
				/// @return log10(x)
				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::log10(x);
				}

				/// @brief 浮動小数点値の10を底とする対数を計算します。
				/// @param x 入力値
				/// @return log10(x)
				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::log10(x);
				}

				/// @brief ベクトルの各要素の10を底とする対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して log10(v) を適用したベクトル
				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::log10(v.x), std::log10(v.y) };
				}

				/// @brief ベクトルの各要素の10を底とする対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して log10(v) を適用したベクトル
				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::log10(v.x), std::log10(v.y) };
				}

				/// @brief ベクトルの各要素の10を底とする対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して log10(v) を適用したベクトル
				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::log10(v.x), std::log10(v.y), std::log10(v.z) };
				}

				/// @brief ベクトルの各要素の10を底とする対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して log10(v) を適用したベクトル
				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::log10(v.x), std::log10(v.y), std::log10(v.z) };
				}

				/// @brief ベクトルの各要素の10を底とする対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して log10(v) を適用したベクトル
				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::log10(v.x), std::log10(v.y), std::log10(v.z), std::log10(v.w) };
				}

				/// @brief ベクトルの各要素の10を底とする対数を計算します。
				/// @param v 入力ベクトル
				/// @return 各要素に対して log10(v) を適用したベクトル
				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::log10(v.x), std::log10(v.y), std::log10(v.z), std::log10(v.w) };
				}

				/// @brief 高精度浮動小数点値の10を底とする対数を計算します。
				/// @param x 入力値
				/// @return log10(x)
				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Modf_impl
			//
			////////////////////////////////////////////////////////////////

			struct Modf_impl
			{
				/// @brief 浮動小数点値を整数部と小数部に分解します。
				/// @param x 入力値
				/// @param ip 整数部を格納する参照
				/// @return 小数部
				[[nodiscard]]
				float operator ()(const float x, float& ip) const noexcept
				{
					return std::modf(x, &ip);
				}

				/// @brief 浮動小数点値を整数部と小数部に分解します。
				/// @param x 入力値
				/// @param ip 整数部を格納する参照
				/// @return 小数部
				[[nodiscard]]
				double operator ()(const double x, double& ip) const noexcept
				{
					return std::modf(x, &ip);
				}

				/// @brief ベクトルの各要素を整数部と小数部に分解します。
				/// @param v 入力ベクトル
				/// @param ip 整数部を格納するベクトルの参照
				/// @return 小数部ベクトル
				[[nodiscard]]
				Float2 operator ()(const Float2 v, Float2& ip) const noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y) };
				}

				/// @brief ベクトルの各要素を整数部と小数部に分解します。
				/// @param v 入力ベクトル
				/// @param ip 整数部を格納するベクトルの参照
				/// @return 小数部ベクトル
				[[nodiscard]]
				Vec2 operator ()(const Vec2 v, Vec2& ip) const noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y) };
				}

				/// @brief ベクトルの各要素を整数部と小数部に分解します。
				/// @param v 入力ベクトル
				/// @param ip 整数部を格納するベクトルの参照
				/// @return 小数部ベクトル
				[[nodiscard]]
				Float3 operator ()(const Float3 v, Float3& ip) const noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y), std::modf(v.z, &ip.z) };
				}

				/// @brief ベクトルの各要素を整数部と小数部に分解します。
				/// @param v 入力ベクトル
				/// @param ip 整数部を格納するベクトルの参照
				/// @return 小数部ベクトル
				[[nodiscard]]
				Vec3 operator ()(const Vec3 v, Vec3& ip) const noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y), std::modf(v.z, &ip.z) };
				}

				/// @brief ベクトルの各要素を整数部と小数部に分解します。
				/// @param v 入力ベクトル
				/// @param ip 整数部を格納するベクトルの参照
				/// @return 小数部ベクトル
				[[nodiscard]]
				Float4 operator ()(const Float4 v, Float4& ip) const noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y), std::modf(v.z, &ip.z), std::modf(v.w, &ip.w) };
				}

				/// @brief ベクトルの各要素を整数部と小数部に分解します。
				/// @param v 入力ベクトル
				/// @param ip 整数部を格納するベクトルの参照
				/// @return 小数部ベクトル
				[[nodiscard]]
				Vec4 operator ()(const Vec4 v, Vec4& ip) const noexcept
				{
					return{ std::modf(v.x, &ip.x), std::modf(v.y, &ip.y), std::modf(v.z, &ip.z), std::modf(v.w, &ip.w) };
				}

				/// @brief 高精度浮動小数点値を整数部と小数部に分解します。
				/// @param x 入力値
				/// @param ip 整数部を格納する参照
				/// @return 小数部
				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x, BigFloat& ip) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Pow_impl
			//
			////////////////////////////////////////////////////////////////

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

				/// @brief 高精度整数のべき乗演算を実行します。
				/// @param x 底
				/// @param exp 指数
				/// @return x^exp
				[[nodiscard]]
				BigInt operator ()(const BigInt& x, uint32 exp) const;

				/// @brief 高精度浮動小数点数のべき乗演算を実行します。
				/// @param x 底
				/// @param exp 指数
				/// @return x^exp
				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x, const BigFloat& exp) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Sign_impl
			//
			////////////////////////////////////////////////////////////////

			struct Sign_impl
			{
				/// @brief 値の符号を返します。
				/// @param x 入力値
				/// @return x の符号 (-1, 0, 1)
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

				/// @brief 2D ベクトルの各成分の符号を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の符号
				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y) };
				}

				/// @brief 2D ベクトルの各成分の符号を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の符号
				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y) };
				}

				/// @brief 3D ベクトルの各成分の符号を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の符号
				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z) };
				}

				/// @brief 3D ベクトルの各成分の符号を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の符号
				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z) };
				}

				/// @brief 4D ベクトルの各成分の符号を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の符号
				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z), operator ()(v.w) };
				}

				/// @brief 4D ベクトルの各成分の符号を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の符号
				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z), operator ()(v.w) };
				}

				/// @brief 多倍長整数の符号を返します。
				/// @param x 入力値
				/// @return x の符号 (-1, 0, 1)
				[[nodiscard]]
				int32 operator ()(const BigInt& x) const;

				/// @brief 多倍長浮動小数点数の符号を返します。
				/// @param x 入力値
				/// @return x の符号 (-1, 0, 1)
				[[nodiscard]]
				int32 operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	ToRadians_impl
			//
			////////////////////////////////////////////////////////////////

			struct ToRadians_impl
			{
				/// @brief 整数値を度数からラジアンに変換します。
				/// @param x 度数の値
				/// @return ラジアン値
				[[nodiscard]]
				constexpr double operator ()(const Concept::Integral auto x) const noexcept
				{
					return (static_cast<double>(x) * (Math::Pi / 180.0));
				}

				/// @brief 単精度浮動小数点値を度数からラジアンに変換します。
				/// @param x 度数の値
				/// @return ラジアン値
				[[nodiscard]]
				constexpr float operator ()(const float x) const noexcept
				{
					return (x * (Math::PiF / 180.0f));
				}

				/// @brief 倍精度浮動小数点値を度数からラジアンに変換します。
				/// @param x 度数の値
				/// @return ラジアン値
				[[nodiscard]]
				constexpr double operator ()(const double x) const noexcept
				{
					return (x * (Math::Pi / 180.0));
				}

				/// @brief 2D ベクトルの各成分を度数からラジアンに変換します。
				/// @param v 度数の値を持つベクトル
				/// @return ラジアン値のベクトル
				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 v) const noexcept
				{
					const float rad = (Math::PiF / 180.0f);
					return (v * rad);
				}

				/// @brief 2D ベクトルの各成分を度数からラジアンに変換します。
				/// @param v 度数の値を持つベクトル
				/// @return ラジアン値のベクトル
				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 v) const noexcept
				{
					const double rad = (Math::Pi / 180.0);
					return (v * rad);
				}

				/// @brief 3D ベクトルの各成分を度数からラジアンに変換します。
				/// @param v 度数の値を持つベクトル
				/// @return ラジアン値のベクトル
				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 v) const noexcept
				{
					const float rad = (Math::PiF / 180.0f);
					return (v * rad);
				}

				/// @brief 3D ベクトルの各成分を度数からラジアンに変換します。
				/// @param v 度数の値を持つベクトル
				/// @return ラジアン値のベクトル
				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 v) const noexcept
				{
					const double rad = (Math::Pi / 180.0);
					return (v * rad);
				}

				/// @brief 4D ベクトルの各成分を度数からラジアンに変換します。
				/// @param v 度数の値を持つベクトル
				/// @return ラジアン値のベクトル
				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 v) const noexcept
				{
					const float rad = (Math::PiF / 180.0f);
					return (v * rad);
				}

				/// @brief 4D ベクトルの各成分を度数からラジアンに変換します。
				/// @param v 度数の値を持つベクトル
				/// @return ラジアン値のベクトル
				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 v) const noexcept
				{
					const double rad = (Math::Pi / 180.0);
					return (v * rad);
				}

				/// @brief 多倍長整数を度数からラジアンに変換します。
				/// @param x 度数の値
				/// @return ラジアン値
				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				/// @brief 多倍長浮動小数点数を度数からラジアンに変換します。
				/// @param x 度数の値
				/// @return ラジアン値
				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	ToDegrees_impl
			//
			////////////////////////////////////////////////////////////////

			struct ToDegrees_impl
			{
				/// @brief 整数値をラジアンから度数に変換します。
				/// @param x ラジアンの値
				/// @return 度数の値
				[[nodiscard]]
				constexpr double operator ()(const Concept::Integral auto x) const noexcept
				{
					return (static_cast<double>(x) * (180.0 / Math::Pi));
				}

				/// @brief 単精度浮動小数点値をラジアンから度数に変換します。
				/// @param x ラジアンの値
				/// @return 度数の値
				[[nodiscard]]
				constexpr float operator ()(const float x) const noexcept
				{
					return (x * (180.0f / Math::PiF));
				}

				/// @brief 倍精度浮動小数点値をラジアンから度数に変換します。
				/// @param x ラジアンの値
				/// @return 度数の値
				[[nodiscard]]
				constexpr double operator ()(const double x) const noexcept
				{
					return (x * (180.0 / Math::Pi));
				}

				/// @brief 2D ベクトルの各成分をラジアンから度数に変換します。
				/// @param v ラジアンの値を持つベクトル
				/// @return 度数の値を持つベクトル
				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 v) const noexcept
				{
					const float deg = (180.0f / Math::PiF);
					return (v * deg);
				}

				/// @brief 2D ベクトルの各成分をラジアンから度数に変換します。
				/// @param v ラジアンの値を持つベクトル
				/// @return 度数の値を持つベクトル
				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 v) const noexcept
				{
					const double deg = (180.0 / Math::Pi);
					return (v * deg);
				}

				/// @brief 3D ベクトルの各成分をラジアンから度数に変換します。
				/// @param v ラジアンの値を持つベクトル
				/// @return 度数の値を持つベクトル
				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 v) const noexcept
				{
					const float deg = (180.0f / Math::PiF);
					return (v * deg);
				}

				/// @brief 3D ベクトルの各成分をラジアンから度数に変換します。
				/// @param v ラジアンの値を持つベクトル
				/// @return 度数の値を持つベクトル
				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 v) const noexcept
				{
					const double deg = (180.0 / Math::Pi);
					return (v * deg);
				}

				/// @brief 4D ベクトルの各成分をラジアンから度数に変換します。
				/// @param v ラジアンの値を持つベクトル
				/// @return 度数の値を持つベクトル
				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 v) const noexcept
				{
					const float deg = (180.0f / Math::PiF);
					return (v * deg);
				}

				/// @brief 4D ベクトルの各成分をラジアンから度数に変換します。
				/// @param v ラジアンの値を持つベクトル
				/// @return 度数の値を持つベクトル
				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 v) const noexcept
				{
					const double deg = (180.0 / Math::Pi);
					return (v * deg);
				}

				/// @brief 多倍長整数をラジアンから度数に変換します。
				/// @param x ラジアンの値
				/// @return 度数の値
				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				/// @brief 多倍長浮動小数点数をラジアンから度数に変換します。
				/// @param x ラジアンの値
				/// @return 度数の値
				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Abs_impl
			//
			////////////////////////////////////////////////////////////////

			struct Abs_impl
			{
				/// @brief 符号付き整数の絶対値を返します。
				/// @param x 入力値
				/// @return x の絶対値
				[[nodiscard]]
				constexpr auto operator ()(const Concept::SignedIntegral auto x) const noexcept
				{
					return s3d::Abs(x);
				}

				/// @brief 単精度浮動小数点数の絶対値を返します。
				/// @param x 入力値
				/// @return x の絶対値
				[[nodiscard]]
				constexpr float operator ()(const float x) const noexcept
				{
					return s3d::Abs(x);
				}

				/// @brief 倍精度浮動小数点数の絶対値を返します。
				/// @param x 入力値
				/// @return x の絶対値
				[[nodiscard]]
				constexpr double operator ()(const double x) const noexcept
				{
					return s3d::Abs(x);
				}

				/// @brief 2D ベクトルの各成分の絶対値を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の絶対値
				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 v) const noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y) };
				}

				/// @brief 2D ベクトルの各成分の絶対値を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の絶対値
				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y) };
				}

				/// @brief 3D ベクトルの各成分の絶対値を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の絶対値
				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 v) const noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y), s3d::Abs(v.z) };
				}

				/// @brief 3D ベクトルの各成分の絶対値を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の絶対値
				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y), s3d::Abs(v.z) };
				}

				/// @brief 4D ベクトルの各成分の絶対値を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の絶対値
				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 v) const noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y), s3d::Abs(v.z), s3d::Abs(v.w) };
				}

				/// @brief 4D ベクトルの各成分の絶対値を返します。
				/// @param v 入力ベクトル
				/// @return v の各成分の絶対値
				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ s3d::Abs(v.x), s3d::Abs(v.y), s3d::Abs(v.z), s3d::Abs(v.w) };
				}

				/// @brief 多倍長整数の絶対値を返します。
				/// @param x 入力値
				/// @return x の絶対値
				[[nodiscard]]
				BigInt operator ()(const BigInt& x) const;

				/// @brief 多倍長浮動小数点数の絶対値を返します。
				/// @param x 入力値
				/// @return x の絶対値
				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
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

				/// @brief 二つの単精度浮動小数点数の差の絶対値を返します。
				/// @param x 第一の値
				/// @param y 第二の値
				/// @return |x - y|
				[[nodiscard]]
				constexpr float operator ()(const float x, const float y) const noexcept
				{
					return Abs(x - y);
				}

				/// @brief 二つの倍精度浮動小数点数の差の絶対値を返します。
				/// @param x 第一の値
				/// @param y 第二の値
				/// @return |x - y|
				[[nodiscard]]
				constexpr double operator ()(const double x, const double y) const noexcept
				{
					return Abs(x - y);
				}

				/// @brief 二つの 2D ベクトルの各成分の差の絶対値を返します。
				/// @param a 第一のベクトル
				/// @param b 第二のベクトル
				/// @return |a - b|
				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 a, const Float2 b) const noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y) };
				}

				/// @brief 二つの 2D ベクトルの各成分の差の絶対値を返します。
				/// @param a 第一のベクトル
				/// @param b 第二のベクトル
				/// @return |a - b|
				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 a, const Vec2 b) const noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y) };
				}

				/// @brief 二つの 3D ベクトルの各成分の差の絶対値を返します。
				/// @param a 第一のベクトル
				/// @param b 第二のベクトル
				/// @return |a - b|
				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 a, const Float3 b) const noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y), Abs(a.z - b.z) };
				}

				/// @brief 二つの 3D ベクトルの各成分の差の絶対値を返します。
				/// @param a 第一のベクトル
				/// @param b 第二のベクトル
				/// @return |a - b|
				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 a, const Vec3 b) const noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y), Abs(a.z - b.z) };
				}

				/// @brief 二つの 4D ベクトルの各成分の差の絶対値を返します。
				/// @param a 第一のベクトル
				/// @param b 第二のベクトル
				/// @return |a - b|
				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 a, const Float4 b) const noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y), Abs(a.z - b.z), Abs(a.w - b.w) };
				}

				/// @brief 二つの 4D ベクトルの各成分の差の絶対値を返します。
				/// @param a 第一のベクトル
				/// @param b 第二のベクトル
				/// @return |a - b|
				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 a, const Vec4 b) const noexcept
				{
					return{ Abs(a.x - b.x), Abs(a.y - b.y), Abs(a.z - b.z), Abs(a.w - b.w) };
				}

				/// @brief 二つの多倍長整数の差の絶対値を返します。
				/// @param x 第一の値
				/// @param y 第二の値
				/// @return |x - y|
				[[nodiscard]]
				BigInt operator ()(const BigInt& x, const BigInt& y) const;

				/// @brief 二つの多倍長浮動小数点数の差の絶対値を返します。
				/// @param x 第一の値
				/// @param y 第二の値
				/// @return |x - y|
				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x, const BigFloat& y) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Square_impl
			//
			////////////////////////////////////////////////////////////////

			struct Square_impl
			{
				/// @brief 値の二乗を返します。
				/// @param x 入力値
				/// @return x²
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

			////////////////////////////////////////////////////////////////
			//
			//	Exp_impl
			//
			////////////////////////////////////////////////////////////////

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

			////////////////////////////////////////////////////////////////
			//
			//	Exp2_impl
			//
			////////////////////////////////////////////////////////////////

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

			////////////////////////////////////////////////////////////////
			//
			//	Rsqrt_impl
			//
			////////////////////////////////////////////////////////////////

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

			////////////////////////////////////////////////////////////////
			//
			//	Sqrt_impl
			//
			////////////////////////////////////////////////////////////////

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

			////////////////////////////////////////////////////////////////
			//
			//	Ceil_impl
			//
			////////////////////////////////////////////////////////////////

			struct Ceil_impl
			{
				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::ceil(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::ceil(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::ceil(v.x), std::ceil(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::ceil(v.x), std::ceil(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::ceil(v.x), std::ceil(v.y), std::ceil(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::ceil(v.x), std::ceil(v.y), std::ceil(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::ceil(v.x), std::ceil(v.y), std::ceil(v.z), std::ceil(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::ceil(v.x), std::ceil(v.y), std::ceil(v.z), std::ceil(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Floor_impl
			//
			////////////////////////////////////////////////////////////////

			struct Floor_impl
			{
				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::floor(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::floor(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::floor(v.x), std::floor(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::floor(v.x), std::floor(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::floor(v.x), std::floor(v.y), std::floor(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::floor(v.x), std::floor(v.y), std::floor(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::floor(v.x), std::floor(v.y), std::floor(v.z), std::floor(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::floor(v.x), std::floor(v.y), std::floor(v.z), std::floor(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Round_impl
			//
			////////////////////////////////////////////////////////////////

			struct Round_impl
			{
				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::round(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::round(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::round(v.x), std::round(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::round(v.x), std::round(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::round(v.x), std::round(v.y), std::round(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::round(v.x), std::round(v.y), std::round(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::round(v.x), std::round(v.y), std::round(v.z), std::round(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::round(v.x), std::round(v.y), std::round(v.z), std::round(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Trunc_impl
			//
			////////////////////////////////////////////////////////////////

			struct Trunc_impl
			{
				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::trunc(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::trunc(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::trunc(v.x), std::trunc(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::trunc(v.x), std::trunc(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::trunc(v.x), std::trunc(v.y), std::trunc(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::trunc(v.x), std::trunc(v.y), std::trunc(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::trunc(v.x), std::trunc(v.y), std::trunc(v.z), std::trunc(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::trunc(v.x), std::trunc(v.y), std::trunc(v.z), std::trunc(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Max_impl
			//
			////////////////////////////////////////////////////////////////

			struct Max_impl
			{
				[[nodiscard]]
				constexpr float operator ()(const float a, const float b) const noexcept
				{
					return s3d::Max(a, b);
				}

				[[nodiscard]]
				constexpr double operator ()(const double a, const double b) const noexcept
				{
					return s3d::Max(a, b);
				}

				[[nodiscard]]
				constexpr Point operator ()(const Point a, const Point b) const noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y) };
				}

				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 a, const Float2 b) const noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y) };
				}

				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 a, const Vec2 b) const noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y) };
				}

				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 a, const Float3 b) const noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y), s3d::Max(a.z, b.z) };
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 a, const Vec3 b) const noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y), s3d::Max(a.z, b.z) };
				}

				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 a, const Float4 b) const noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y), s3d::Max(a.z, b.z), s3d::Max(a.w, b.w) };
				}

				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 a, const Vec4 b) const noexcept
				{
					return{ s3d::Max(a.x, b.x), s3d::Max(a.y, b.y), s3d::Max(a.z, b.z), s3d::Max(a.w, b.w) };
				}

				[[nodiscard]]
				const BigInt& operator ()(const BigInt& x, const BigInt& y) const;

				[[nodiscard]]
				const BigFloat& operator ()(const BigFloat& x, const BigFloat& y) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Min_impl
			//
			////////////////////////////////////////////////////////////////

			struct Min_impl
			{
				[[nodiscard]]
				constexpr float operator ()(const float a, const float b) const noexcept
				{
					return s3d::Min(a, b);
				}

				[[nodiscard]]
				constexpr double operator ()(const double a, const double b) const noexcept
				{
					return s3d::Min(a, b);
				}

				[[nodiscard]]
				constexpr Point operator ()(const Point a, const Point b) const noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y) };
				}

				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 a, const Float2 b) const noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y) };
				}

				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 a, const Vec2 b) const noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y) };
				}

				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 a, const Float3 b) const noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y), s3d::Min(a.z, b.z) };
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 a, const Vec3 b) const noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y), s3d::Min(a.z, b.z) };
				}

				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 a, const Float4 b) const noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y), s3d::Min(a.z, b.z), s3d::Min(a.w, b.w) };
				}

				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 a, const Vec4 b) const noexcept
				{
					return{ s3d::Min(a.x, b.x), s3d::Min(a.y, b.y), s3d::Min(a.z, b.z), s3d::Min(a.w, b.w) };
				}

				[[nodiscard]]
				const BigInt& operator ()(const BigInt& x, const BigInt& y) const;

				[[nodiscard]]
				const BigFloat& operator ()(const BigFloat& x, const BigFloat& y) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Clamp_impl
			//
			////////////////////////////////////////////////////////////////

			struct Clamp_impl
			{
				[[nodiscard]]
				constexpr float operator ()(const float value, const float min, const float max) const noexcept
				{
					return s3d::Clamp(value, min, max);
				}

				[[nodiscard]]
				constexpr double operator ()(const double value, const double min, const double max) const noexcept
				{
					return s3d::Clamp(value, min, max);
				}

				[[nodiscard]]
				constexpr Point operator ()(const Point value, int32 min, int32 max) const noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max) };
				}
	
				[[nodiscard]]
				constexpr Point operator ()(const Point value, const Point min, const Point max) const noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y) };
				}

				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 value, float min, float max) const noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max) };
				}
				
				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 value, const Float2 min, const Float2 max) const noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y) };
				}

				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 value, double min, double max) const noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max) };
				}

				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 value, const Vec2 min, const Vec2 max) const noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y) };
				}

				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 value, float min, float max) const noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max), s3d::Clamp(value.z, min, max) };
				}

				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 value, const Float3 min, const Float3 max) const noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y), s3d::Clamp(value.z, min.z, max.z) };
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 value, double min, double max) const noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max), s3d::Clamp(value.z, min, max) };
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 value, const Vec3 min, const Vec3 max) const noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y), s3d::Clamp(value.z, min.z, max.z) };
				}

				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 value, float min, float max) const noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max), s3d::Clamp(value.z, min, max), s3d::Clamp(value.w, min, max) };
				}

				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 value, const Float4 min, const Float4 max) const noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y), s3d::Clamp(value.z, min.z, max.z), s3d::Clamp(value.w, min.w, max.w) };
				}

				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 value, double min, double max) const noexcept
				{
					return{ s3d::Clamp(value.x, min, max), s3d::Clamp(value.y, min, max), s3d::Clamp(value.z, min, max), s3d::Clamp(value.w, min, max) };
				}

				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 value, const Vec4 min, const Vec4 max) const noexcept
				{
					return{ s3d::Clamp(value.x, min.x, max.x), s3d::Clamp(value.y, min.y, max.y), s3d::Clamp(value.z, min.z, max.z), s3d::Clamp(value.w, min.w, max.w) };
				}

				[[nodiscard]]
				const BigInt& operator ()(const BigInt& x, const BigInt& min, const BigInt& max) const;

				[[nodiscard]]
				const BigFloat& operator ()(const BigFloat& x, const BigFloat& min, const BigFloat& max) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Saturate_impl
			//
			////////////////////////////////////////////////////////////////

			struct Saturate_impl
			{
				[[nodiscard]]
				constexpr float operator ()(const float x) const noexcept
				{
					return s3d::Clamp(x, 0.0f, 1.0f);
				}

				[[nodiscard]]
				constexpr double operator ()(const double x) const noexcept
				{
					return s3d::Clamp(x, 0.0, 1.0);
				}

				[[nodiscard]]
				constexpr Float2 operator ()(const Float2 v) const noexcept
				{
					return{ s3d::Clamp(v.x, 0.0f, 1.0f), s3d::Clamp(v.y, 0.0f, 1.0f) };
				}

				[[nodiscard]]
				constexpr Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ s3d::Clamp(v.x, 0.0, 1.0), s3d::Clamp(v.y, 0.0, 1.0) };
				}

				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 v) const noexcept
				{
					return{ s3d::Clamp(v.x, 0.0f, 1.0f), s3d::Clamp(v.y, 0.0f, 1.0f), s3d::Clamp(v.z, 0.0f, 1.0f) };
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ s3d::Clamp(v.x, 0.0, 1.0), s3d::Clamp(v.y, 0.0, 1.0), s3d::Clamp(v.z, 0.0, 1.0) };
				}

				[[nodiscard]]
				constexpr Float4 operator ()(const Float4 v) const noexcept
				{
					return{ s3d::Clamp(v.x, 0.0f, 1.0f), s3d::Clamp(v.y, 0.0f, 1.0f), s3d::Clamp(v.z, 0.0f, 1.0f), s3d::Clamp(v.w, 0.0f, 1.0f) };
				}

				[[nodiscard]]
				constexpr Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ s3d::Clamp(v.x, 0.0, 1.0), s3d::Clamp(v.y, 0.0, 1.0), s3d::Clamp(v.z, 0.0, 1.0), s3d::Clamp(v.w, 0.0, 1.0) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Acos_impl
			//
			////////////////////////////////////////////////////////////////

			struct Acos_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::acos(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::acos(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::acos(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::acos(v.x), std::acos(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::acos(v.x), std::acos(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::acos(v.x), std::acos(v.y), std::acos(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::acos(v.x), std::acos(v.y), std::acos(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::acos(v.x), std::acos(v.y), std::acos(v.z), std::acos(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::acos(v.x), std::acos(v.y), std::acos(v.z), std::acos(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Asin_impl
			//
			////////////////////////////////////////////////////////////////

			struct Asin_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::asin(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::asin(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::asin(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::asin(v.x), std::asin(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::asin(v.x), std::asin(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::asin(v.x), std::asin(v.y), std::asin(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::asin(v.x), std::asin(v.y), std::asin(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::asin(v.x), std::asin(v.y), std::asin(v.z), std::asin(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::asin(v.x), std::asin(v.y), std::asin(v.z), std::asin(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Atan_impl
			//
			////////////////////////////////////////////////////////////////

			struct Atan_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::atan(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::atan(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::atan(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::atan(v.x), std::atan(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::atan(v.x), std::atan(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::atan(v.x), std::atan(v.y), std::atan(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::atan(v.x), std::atan(v.y), std::atan(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::atan(v.x), std::atan(v.y), std::atan(v.z), std::atan(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::atan(v.x), std::atan(v.y), std::atan(v.z), std::atan(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Atan2_impl
			//
			////////////////////////////////////////////////////////////////

			struct Atan2_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto y, const Concept::Integral auto x) const noexcept
				{
					if ((x == 0) && (y == 0))
					{
						return 0.0;
					}

					return std::atan2(static_cast<double>(y), static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float y, const float x) const noexcept
				{
					if ((x == 0.0f) && (y == 0.0f))
					{
						return 0.0f;
					}

					return std::atan2(y, x);
				}

				[[nodiscard]]
				double operator ()(const double y, const double x) const noexcept
				{
					if ((x == 0.0) && (y == 0.0))
					{
						return 0.0;
					}

					return std::atan2(y, x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 y, const Float2 x) const noexcept
				{
					return{ operator()(y.x, x.x), operator()(y.y, x.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 y, const Vec2 x) const noexcept
				{
					return{ operator()(y.x, x.x), operator()(y.y, x.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 y, const Float3 x) const noexcept
				{
					return{ operator()(y.x, x.x), operator()(y.y, x.y), operator()(y.z, x.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 y, const Vec3 x) const noexcept
				{
					return{ operator()(y.x, x.x), operator()(y.y, x.y), operator()(y.z, x.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 y, const Float4 x) const noexcept
				{
					return{ operator()(y.x, x.x), operator()(y.y, x.y), operator()(y.z, x.z), operator()(y.w, x.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 y, const Vec4 x) const noexcept
				{
					return{ operator()(y.x, x.x), operator()(y.y, x.y), operator()(y.z, x.z), operator()(y.w, x.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& y, const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& y, const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Cos_impl
			//
			////////////////////////////////////////////////////////////////

			struct Cos_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::cos(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::cos(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::cos(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::cos(v.x), std::cos(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::cos(v.x), std::cos(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::cos(v.x), std::cos(v.y), std::cos(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::cos(v.x), std::cos(v.y), std::cos(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::cos(v.x), std::cos(v.y), std::cos(v.z), std::cos(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::cos(v.x), std::cos(v.y), std::cos(v.z), std::cos(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Cosh_impl
			//
			////////////////////////////////////////////////////////////////

			struct Cosh_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::cosh(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::cosh(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::cosh(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::cosh(v.x), std::cosh(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::cosh(v.x), std::cosh(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::cosh(v.x), std::cosh(v.y), std::cosh(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::cosh(v.x), std::cosh(v.y), std::cosh(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::cosh(v.x), std::cosh(v.y), std::cosh(v.z), std::cosh(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::cosh(v.x), std::cosh(v.y), std::cosh(v.z), std::cosh(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Sin_impl
			//
			////////////////////////////////////////////////////////////////

			struct Sin_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::sin(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::sin(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::sin(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::sin(v.x), std::sin(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::sin(v.x), std::sin(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::sin(v.x), std::sin(v.y), std::sin(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::sin(v.x), std::sin(v.y), std::sin(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::sin(v.x), std::sin(v.y), std::sin(v.z), std::sin(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::sin(v.x), std::sin(v.y), std::sin(v.z), std::sin(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Sinh_impl
			//
			////////////////////////////////////////////////////////////////

			struct Sinh_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::sinh(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::sinh(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::sinh(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::sinh(v.x), std::sinh(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::sinh(v.x), std::sinh(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::sinh(v.x), std::sinh(v.y), std::sinh(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::sinh(v.x), std::sinh(v.y), std::sinh(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::sinh(v.x), std::sinh(v.y), std::sinh(v.z), std::sinh(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::sinh(v.x), std::sinh(v.y), std::sinh(v.z), std::sinh(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Tan_impl
			//
			////////////////////////////////////////////////////////////////

			struct Tan_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::tan(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::tan(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::tan(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::tan(v.x), std::tan(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::tan(v.x), std::tan(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::tan(v.x), std::tan(v.y), std::tan(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::tan(v.x), std::tan(v.y), std::tan(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::tan(v.x), std::tan(v.y), std::tan(v.z), std::tan(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::tan(v.x), std::tan(v.y), std::tan(v.z), std::tan(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Tanh_impl
			//
			////////////////////////////////////////////////////////////////

			struct Tanh_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::tanh(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::tanh(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::tanh(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ std::tanh(v.x), std::tanh(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ std::tanh(v.x), std::tanh(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ std::tanh(v.x), std::tanh(v.y), std::tanh(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ std::tanh(v.x), std::tanh(v.y), std::tanh(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ std::tanh(v.x), std::tanh(v.y), std::tanh(v.z), std::tanh(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ std::tanh(v.x), std::tanh(v.y), std::tanh(v.z), std::tanh(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Dot_impl
			//
			////////////////////////////////////////////////////////////////

			struct Dot_impl
			{
				[[nodiscard]]
				constexpr float operator ()(const Float2 a, const Float2 b) const noexcept
				{
					return ((a.x * b.x) + (a.y * b.y));
				}

				[[nodiscard]]
				constexpr double operator ()(const Float2 a, const Vec2 b) const noexcept
				{
					return ((a.x * b.x) + (a.y * b.y));
				}
				
				[[nodiscard]]
				constexpr double operator ()(const Vec2 a, const Float2 b) const noexcept
				{
					return ((a.x * b.x) + (a.y * b.y));
				}

				[[nodiscard]]
				constexpr double operator ()(const Vec2 a, const Vec2 b) const noexcept
				{
					return ((a.x * b.x) + (a.y * b.y));
				}

				[[nodiscard]]
				constexpr float operator ()(const Float3 a, const Float3 b) const noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
				}

				[[nodiscard]]
				constexpr double operator ()(const Float3 a, const Vec3 b) const noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
				}

				[[nodiscard]]
				constexpr double operator ()(const Vec3 a, const Float3 b) const noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
				}

				[[nodiscard]]
				constexpr double operator ()(const Vec3 a, const Vec3 b) const noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
				}

				[[nodiscard]]
				constexpr float operator ()(const Float4 a, const Float4 b) const noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
				}

				[[nodiscard]]
				constexpr double operator ()(const Float4 a, const Vec4 b) const noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
				}

				[[nodiscard]]
				constexpr double operator ()(const Vec4 a, const Float4 b) const noexcept
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
				}

				[[nodiscard]]
				constexpr double operator ()(const Vec4 a, const Vec4 b) const noexcept
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
				[[nodiscard]]
				constexpr float operator ()(const Float2 a, const Float2 b) noexcept
				{
					return ((a.x * b.y) - (a.y * b.x));
				}

				[[nodiscard]]
				constexpr double operator ()(const Float2 a, const Vec2 b) noexcept
				{
					return ((a.x * b.y) - (a.y * b.x));
				}

				[[nodiscard]]
				constexpr double operator ()(const Vec2 a, const Float2 b) noexcept
				{
					return ((a.x * b.y) - (a.y * b.x));
				}

				[[nodiscard]]
				constexpr double operator ()(const Vec2 a, const Vec2 b) noexcept
				{
					return ((a.x * b.y) - (a.y * b.x));
				}

				[[nodiscard]]
				constexpr Float3 operator ()(const Float3 a, const Float3 b) const noexcept
				{
					return{ ((a.y * b.z) - (a.z * b.y)), ((a.z * b.x) - (a.x * b.z)), ((a.x * b.y) - (a.y * b.x)) };
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Float3 a, const Vec3 b) const noexcept
				{
					return{ ((a.y * b.z) - (a.z * b.y)), ((a.z * b.x) - (a.x * b.z)), ((a.x * b.y) - (a.y * b.x)) };
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 a, const Float3 b) const noexcept
				{
					return{ ((a.y * b.z) - (a.z * b.y)), ((a.z * b.x) - (a.x * b.z)), ((a.x * b.y) - (a.y * b.x)) };
				}

				[[nodiscard]]
				constexpr Vec3 operator ()(const Vec3 a, const Vec3 b) const noexcept
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
				[[nodiscard]]
				constexpr float operator ()(const float x) const noexcept
				{
					if (x == 0.0f)
					{
						return 0.0f;
					}

					return 1.0f;
				}

				[[nodiscard]]
				constexpr double operator ()(const double x) const noexcept
				{
					if (x == 0.0)
					{
						return 0.0;
					}

					return 1.0;
				}

				[[nodiscard]]
				Vec2 operator ()(const Point v) const noexcept
				{
					return Vec2{ v }.normalized();
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return v.normalized();
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return v.normalized();
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return v.normalized();
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return v.normalized();
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return v.normalized();
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return v.normalized();
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	Smoothstep_impl
			//
			////////////////////////////////////////////////////////////////

			struct Smoothstep_impl
			{
				[[nodiscard]]
				constexpr float operator ()(const float min, const float max, float t) const noexcept
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

				[[nodiscard]]
				constexpr double operator ()(const double min, const double max, double t) const noexcept
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

				[[nodiscard]]
				constexpr float operator ()(const float x) const noexcept
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

				[[nodiscard]]
				constexpr double operator ()(const double x) const noexcept
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

				[[nodiscard]]
				Float2 operator ()(const Float2 min, const Float2 max, const Float2 t) const noexcept
				{
					return{ operator ()(min.x, max.x, t.x), operator ()(min.y, max.y, t.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 min, const Vec2 max, const Vec2 t) const noexcept
				{
					return{ operator ()(min.x, max.x, t.x), operator ()(min.y, max.y, t.y) };
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 min, const Float3 max, const Float3 t) const noexcept
				{
					return{ operator ()(min.x, max.x, t.x), operator ()(min.y, max.y, t.y), operator ()(min.z, max.z, t.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 min, const Vec3 max, const Vec3 t) const noexcept
				{
					return{ operator ()(min.x, max.x, t.x), operator ()(min.y, max.y, t.y), operator ()(min.z, max.z, t.z) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 min, const Float4 max, const Float4 t) const noexcept
				{
					return{ operator ()(min.x, max.x, t.x), operator ()(min.y, max.y, t.y), operator ()(min.z, max.z, t.z), operator ()(min.w, max.w, t.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 min, const Vec4 max, const Vec4 t) const noexcept
				{
					return{ operator ()(min.x, max.x, t.x), operator ()(min.y, max.y, t.y), operator ()(min.z, max.z, t.z), operator ()(min.w, max.w, t.w) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z), operator ()(v.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 v) const noexcept
				{
					return{ operator ()(v.x), operator ()(v.y), operator ()(v.z), operator ()(v.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& min, const BigFloat& max, const BigFloat& t) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	ClampAngle_impl
			//
			////////////////////////////////////////////////////////////////

			struct ClampAngle_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Arithmetic auto angle, const Concept::Arithmetic auto min, const Concept::Arithmetic auto max) const noexcept
				{
					const auto start = ((min + max) * 0.5 - Pi);
					
					const auto floor = (std::floor((angle - start) / TwoPi) * TwoPi);
					
					return Clamp(angle, (min + floor), (max + floor));
				}

				[[nodiscard]]
				float operator ()(const float angle, const float min, const float max) const noexcept
				{
					const float start = ((min + max) * 0.5f - PiF);
					
					const float floor = (std::floor((angle - start) / TwoPiF) * TwoPiF);
					
					return Clamp(angle, (min + floor), (max + floor));
				}

				[[nodiscard]]
				double operator ()(const double angle, const double min, const double max) const noexcept
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
				[[nodiscard]]
				double operator ()(const Concept::Arithmetic auto radian_, double center = Pi) const noexcept
				{
					double radian = std::fmod(radian_ + (Pi - center), TwoPi);

					if (radian < 0.0)
					{
						radian += TwoPi;
					}

					return (radian - (Pi - center));
				}

				[[nodiscard]]
				float operator ()(const float radian_, float center = PiF) const noexcept
				{
					float radian = std::fmod(radian_ + (PiF - center), TwoPiF);

					if (radian < 0.0f)
					{
						radian += TwoPiF;
					}

					return (radian - (PiF - center));
				}

				[[nodiscard]]
				double operator ()(const double radian_, double center = Pi) const noexcept
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
				[[nodiscard]]
				constexpr auto operator ()(const Concept::Integral auto a, const Concept::Integral auto b) const noexcept
				{
					return std::gcd(a, b);
				}

				[[nodiscard]]
				BigInt operator ()(const BigInt& a, const BigInt& b) const;
			};

			////////////////////////////////////////////////////////////////
			//
			//	LCM_impl
			//
			////////////////////////////////////////////////////////////////

			struct LCM_impl
			{
				[[nodiscard]]
				constexpr auto operator ()(const Concept::Integral auto a, const Concept::Integral auto b) const noexcept
				{
					return std::lcm(a, b);
				}

				[[nodiscard]]
				BigInt operator ()(const BigInt& a, const BigInt& b) const;
			};
		}
	}
}
