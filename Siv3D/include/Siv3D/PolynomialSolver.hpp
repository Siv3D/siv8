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
# include "Optional.hpp"
# include "Array.hpp"

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
		/// @return 実数解が存在する場合は昇順に並んだ解の配列を返します。解が存在しない場合は none を返します。
		[[nodiscard]]
		Optional<Array<double>> SolveLinearEquation(double a, double b);

		////////////////////////////////////////////////////////////////
		//
		//	SolveQuadraticEquation
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 次方程式 ax^2 + bx + c = 0 の実数解を求めます。
		/// @param a x^2 の係数 a です。
		/// @param b x の係数 b です。
		/// @param c 定数項 c です。
		/// @return 実数解が存在する場合は重複を除き昇順に並んだ解の配列を返します。実数解が存在しない場合は none を返します。
		[[nodiscard]]
		Optional<Array<double>> SolveQuadraticEquation(double a, double b, double c);

		////////////////////////////////////////////////////////////////
		//
		//	SolveCubicEquation
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3 次方程式 x^3 + px + q = 0 の実数解を求めます。
		/// @param p x の係数 p です。
		/// @param q 定数項 q です。
		/// @return 実数解が存在する場合は重複を除き昇順に並んだ解の配列を返します。実数解が存在しない場合は none を返します。
		[[nodiscard]]
		Optional<Array<double>> SolveCubicEquation(double p, double q);

		/// @brief 3 次方程式 x^3 + ax^2 + bx + c = 0 の実数解を求めます。
		/// @param a x^2 の係数 a です。
		/// @param b x の係数 b です。
		/// @param c 定数項 c です。
		/// @return 実数解が存在する場合は重複を除き昇順に並んだ解の配列を返します。実数解が存在しない場合は none を返します。
		[[nodiscard]]
		Optional<Array<double>> SolveCubicEquation(double a, double b, double c);

		/// @brief 3 次方程式 ax^3 + bx^2 + cx + d = 0 の実数解を求めます。
		/// @param a x^3 の係数 a です。
		/// @param b x^2 の係数 b です。
		/// @param c x の係数 c です。
		/// @param d 定数項 d です。
		/// @return 実数解が存在する場合は重複を除き昇順に並んだ解の配列を返します。実数解が存在しない場合は none を返します。
		[[nodiscard]]
		Optional<Array<double>> SolveCubicEquation(double a, double b, double c, double d);
	}
}
