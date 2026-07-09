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
# include "Common.hpp"
# include "PolynomialRoots.hpp"

namespace s3d
{
	namespace Math
	{
		////////////////////////////////////////////////////////////////
		//
		//	SolveLinearEquation
		//
		////////////////////////////////////////////////////////////////

		/// @brief 1 次方程式 ax + b = 0 の実数解を求めます。
		/// @param a x の係数 a です。
		/// @param b 定数項 b です。
		/// @return 昇順に並んだ有限実数解を返します。実数解が存在しない場合は count が 0 です。恒等式の場合は hasInfiniteSolutions が true です。
		/// @remark 極端な係数比では、実行時の実用性を優先して解を返さない場合があります。
		[[nodiscard]]
		PolynomialRoots SolveLinearEquation(double a, double b);

		////////////////////////////////////////////////////////////////
		//
		//	SolveQuadraticEquation
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 次方程式 ax^2 + bx + c = 0 の実数解を求めます。
		/// @param a x^2 の係数 a です。
		/// @param b x の係数 b です。
		/// @param c 定数項 c です。
		/// @return 数値的に近い重複解を除き、昇順に並んだ有限実数解を返します。実数解が存在しない場合は count が 0 です。恒等式の場合は hasInfiniteSolutions が true です。
		/// @remark 0 近傍では実用上の絶対許容誤差により、非常に近い別解が 1 つにマージされる場合があります。
		/// @remark 極端に小さい最高次係数は、実行時の実用性を優先して 1 次方程式として扱う場合があります。
		[[nodiscard]]
		PolynomialRoots SolveQuadraticEquation(double a, double b, double c);

		////////////////////////////////////////////////////////////////
		//
		//	SolveCubicEquation
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3 次方程式 x^3 + px + q = 0 の実数解を求めます。
		/// @param p x の係数 p です。
		/// @param q 定数項 q です。
		/// @return 数値的に近い重複解を除き、昇順に並んだ有限実数解を返します。
		/// @remark 0 近傍では実用上の絶対許容誤差により、非常に近い別解が 1 つにマージされる場合があります。
		[[nodiscard]]
		PolynomialRoots SolveCubicEquation(double p, double q);

		/// @brief 3 次方程式 x^3 + ax^2 + bx + c = 0 の実数解を求めます。
		/// @param a x^2 の係数 a です。
		/// @param b x の係数 b です。
		/// @param c 定数項 c です。
		/// @return 数値的に近い重複解を除き、昇順に並んだ有限実数解を返します。
		/// @remark 0 近傍では実用上の絶対許容誤差により、非常に近い別解が 1 つにマージされる場合があります。
		[[nodiscard]]
		PolynomialRoots SolveCubicEquation(double a, double b, double c);

		/// @brief 3 次方程式 ax^3 + bx^2 + cx + d = 0 の実数解を求めます。
		/// @param a x^3 の係数 a です。
		/// @param b x^2 の係数 b です。
		/// @param c x の係数 c です。
		/// @param d 定数項 d です。
		/// @return 数値的に近い重複解を除き、昇順に並んだ有限実数解を返します。実数解が存在しない場合は count が 0 です。恒等式の場合は hasInfiniteSolutions が true です。
		/// @remark 0 近傍では実用上の絶対許容誤差により、非常に近い別解が 1 つにマージされる場合があります。
		/// @remark 極端に小さい最高次係数は、実行時の実用性を優先して低次方程式として扱う場合があります。
		[[nodiscard]]
		PolynomialRoots SolveCubicEquation(double a, double b, double c, double d);
	}
}
