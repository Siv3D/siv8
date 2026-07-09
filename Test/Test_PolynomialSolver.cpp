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

# include "Siv3DTest.hpp"
# include <cmath>

//
//	Round 4 acceptance tests for PolynomialSolver.
//
//	許容誤差はすべて修正版プロトタイプ（deflation + 最終 dedup の x 空間移動
//	+ 元係数空間での backcheck）の実測値に基づく。実測値より 3〜6 桁緩い値を
//	採用し、libm（cbrt / acos）やコンパイラの FMA 差によるプラットフォーム間の
//	ぶれを吸収する。
//
//	[R3-FAIL] とマークされたケースは Round 3 実装では失敗する（修正のゲート）。
//

namespace
{
	using namespace s3d;

	// 相対誤差比較。expected == 0.0 の場合は absEps による絶対比較。
	// doctest::Approx は既定で scale=1 の項を含み、1e-300 級の微小根では
	// あらゆる値が一致扱いになるため使用しない。
	[[nodiscard]]
	static bool IsClose(const double value, const double expected, const double relEps, const double absEps = 0.0) noexcept
	{
		if (expected == 0.0)
		{
			return (std::abs(value) <= absEps);
		}

		return (std::abs(value - expected) <= (relEps * std::abs(expected)));
	}

	// 全ての有限実数解が finite であること（inf / nan の混入禁止）。
	[[nodiscard]]
	static bool AllFinite(const PolynomialRoots& roots) noexcept
	{
		for (const double root : roots)
		{
			if (not std::isfinite(root))
			{
				return false;
			}
		}

		return true;
	}

	// 解が昇順かつ RootMergeEpsilon 相当で重複除去済みであること。
	[[nodiscard]]
	static bool IsSortedStrictly(const PolynomialRoots& roots) noexcept
	{
		for (uint32 i = 1; i < roots.count; ++i)
		{
			if (roots.roots[i] <= roots.roots[i - 1])
			{
				return false;
			}
		}

		return true;
	}

	// 指定値に相対誤差 relEps で一致する解が含まれること。
	[[nodiscard]]
	static bool HasRootNear(const PolynomialRoots& roots, const double expected, const double relEps) noexcept
	{
		for (const double root : roots)
		{
			if (IsClose(root, expected, relEps))
			{
				return true;
			}
		}

		return false;
	}
}

// 恒等式・解なし・基本形の契約を確認する。
TEST_CASE("PolynomialSolver.contract.basic")
{
	{
		const PolynomialRoots roots = Math::SolveLinearEquation(0.0, 0.0);
		CHECK(roots.hasInfiniteSolutions);
		CHECK_EQ(roots.count, 0u);
	}

	{
		const PolynomialRoots roots = Math::SolveCubicEquation(0.0, 0.0, 0.0, 0.0);
		CHECK(roots.hasInfiniteSolutions);
		CHECK_EQ(roots.count, 0u);
	}

	{
		const PolynomialRoots roots = Math::SolveLinearEquation(0.0, 5.0);
		CHECK(not roots.hasInfiniteSolutions);
		CHECK_EQ(roots.count, 0u);
	}

	{
		const PolynomialRoots roots = Math::SolveQuadraticEquation(1.0, 0.0, 1.0);
		CHECK_EQ(roots.count, 0u); // 実数解なし
	}

	{
		// 4 引数版の低次ディスパッチ: 0x^3 + x^2 + 0x - 4 = 0
		const PolynomialRoots roots = Math::SolveCubicEquation(0.0, 1.0, 0.0, -4.0);
		REQUIRE_EQ(roots.count, 2u);
		CHECK(IsClose(roots.roots[0], -2.0, 1e-12));
		CHECK(IsClose(roots.roots[1], 2.0, 1e-12));
	}

	{
		// (x-1)(x-2)(x-3), 4 引数版 (a=2)。実測相対誤差 ~2e-15。
		const PolynomialRoots roots = Math::SolveCubicEquation(2.0, -12.0, 22.0, -12.0);
		REQUIRE_EQ(roots.count, 3u);
		CHECK(AllFinite(roots));
		CHECK(IsSortedStrictly(roots));
		CHECK(IsClose(roots.roots[0], 1.0, 1e-12));
		CHECK(IsClose(roots.roots[1], 2.0, 1e-12));
		CHECK(IsClose(roots.roots[2], 3.0, 1e-12));
	}

	{
		// x^3 + x + 1 = 0（実根 1 つ）。参照値は高精度計算による。
		const PolynomialRoots roots = Math::SolveCubicEquation(1.0, 0.0, 1.0, 1.0);
		REQUIRE_EQ(roots.count, 1u);
		CHECK(IsClose(roots.roots[0], -0.6823278038280193, 1e-12));
	}

	{
		// Citardauq: 大小の離れた 2 根 (x - 1e8)(x - 1e-8)
		const PolynomialRoots roots = Math::SolveQuadraticEquation(1.0, -(1e8 + 1e-8), 1.0);
		REQUIRE_EQ(roots.count, 2u);
		CHECK(IsClose(roots.roots[0], 1e-8, 1e-12));
		CHECK(IsClose(roots.roots[1], 1e8, 1e-12));
	}
}

// [R3-FAIL] 分離根ファミリー (x-1)(x-2)(x-R)。
// Round 3 は R >= 3e6 で近接ペア {1, 2} を無音で削除し count=1 を返す。
// 修正版の実測: R <= 1e12 で小根の相対誤差 <= 5e-16（backcheck の Newton による）。
// テスト許容: 小根 1e-9、支配根 1e-12（プラットフォーム余裕込み）。
TEST_CASE("PolynomialSolver.Cubic.separatedRoots")
{
	constexpr std::array Ratios{ 1e6, 3e6, 1e8, 1e10, 1e12 };

	for (const double R : Ratios)
	{
		// (x-1)(x-2)(x-R) = x^3 - (R+3)x^2 + (3R+2)x - 2R
		// R <= 1e15 では全係数が double で正確に表現される（整数 < 2^53）。
		const PolynomialRoots roots = Math::SolveCubicEquation(-(R + 3.0), (3.0 * R + 2.0), (-2.0 * R));

		REQUIRE_EQ(roots.count, 3u);
		CHECK(AllFinite(roots));
		CHECK(IsSortedStrictly(roots));
		CHECK(IsClose(roots.roots[0], 1.0, 1e-9));
		CHECK(IsClose(roots.roots[1], 2.0, 1e-9));
		CHECK(IsClose(roots.roots[2], R, 1e-12));
	}

	{
		// 4 引数版も同じ修正を継承していること (R = 1e10)
		const double R = 1e10;
		const PolynomialRoots roots = Math::SolveCubicEquation(1.0, -(R + 3.0), (3.0 * R + 2.0), (-2.0 * R));

		REQUIRE_EQ(roots.count, 3u);
		CHECK(IsClose(roots.roots[0], 1.0, 1e-9));
		CHECK(IsClose(roots.roots[1], 2.0, 1e-9));
		CHECK(IsClose(roots.roots[2], R, 1e-12));
	}
}

// 分離根ファミリーの契約限界 (R = 1e14)。
// ペア間隔 (=1) が支配根の 1e-12 倍 (RootMergeEpsilon 相当の粒度) を下回るため、
// 近接ペアの復元は実用契約の対象外。支配根の復元と有限性・非偽根のみを要求し、
// 将来の実装がペアを復元しても失敗しないよう count は範囲で検査する。
TEST_CASE("PolynomialSolver.Cubic.separatedRoots.contractLimit")
{
	const double R = 1e14;
	const PolynomialRoots roots = Math::SolveCubicEquation(-(R + 3.0), (3.0 * R + 2.0), (-2.0 * R));

	REQUIRE(1 <= roots.count);
	CHECK(roots.count <= 3);
	CHECK(AllFinite(roots));
	CHECK(HasRootNear(roots, R, 1e-12));

	// 偽根の禁止: 返る根は真根 {1, 2, R} のいずれかの近傍に限られる。
	for (const double root : roots)
	{
		const bool nearTrueRoot = (IsClose(root, 1.0, 1e-3)
			|| IsClose(root, 2.0, 1e-3)
			|| IsClose(root, R, 1e-9));
		CHECK(nearTrueRoot);
	}
}

// [R3-FAIL] 符号バリエーション。
TEST_CASE("PolynomialSolver.Cubic.separatedRoots.signVariants")
{
	{
		// 大きな負の支配根 + 正の近接ペア: (x+1e10)(x-1)(x-2)
		const double R = 1e10;
		const PolynomialRoots roots = Math::SolveCubicEquation((R - 3.0), (2.0 - 3.0 * R), (2.0 * R));

		REQUIRE_EQ(roots.count, 3u);
		CHECK(IsClose(roots.roots[0], -R, 1e-12));
		CHECK(IsClose(roots.roots[1], 1.0, 1e-9));
		CHECK(IsClose(roots.roots[2], 2.0, 1e-9));
	}

	{
		// 全て負: (x+1)(x+2)(x+1e10)
		const double R = 1e10;
		const PolynomialRoots roots = Math::SolveCubicEquation((R + 3.0), (3.0 * R + 2.0), (2.0 * R));

		REQUIRE_EQ(roots.count, 3u);
		CHECK(IsClose(roots.roots[0], -R, 1e-12));
		CHECK(IsClose(roots.roots[1], -2.0, 1e-9));
		CHECK(IsClose(roots.roots[2], -1.0, 1e-9));
	}

	{
		// 小スケール正クラスタ（Round 2 由来の回帰ケース）。実測相対誤差 ~9e-16。
		const PolynomialRoots roots = Math::SolveCubicEquation(-0.06, 0.0011, -6e-6);

		REQUIRE_EQ(roots.count, 3u);
		CHECK(IsClose(roots.roots[0], 0.01, 1e-9));
		CHECK(IsClose(roots.roots[1], 0.02, 1e-9));
		CHECK(IsClose(roots.roots[2], 0.03, 1e-9));
	}

	{
		// 小スケール負クラスタ（鏡像）
		const PolynomialRoots roots = Math::SolveCubicEquation(0.06, 0.0011, 6e-6);

		REQUIRE_EQ(roots.count, 3u);
		CHECK(IsClose(roots.roots[0], -0.03, 1e-9));
		CHECK(IsClose(roots.roots[1], -0.02, 1e-9));
		CHECK(IsClose(roots.roots[2], -0.01, 1e-9));
	}
}

// 重根・多重根。
// 二重根は係数摂動に対し sqrt(eps) ~ 1.5e-8、三重根は eps^(1/3) ~ 6e-6 の
// 本質的感度を持つため、値の許容誤差はそれを尊重する（個数の検査は厳密）。
TEST_CASE("PolynomialSolver.Cubic.repeatedRoots")
{
	{
		// (x-1)^2 (x-5) = x^3 - 7x^2 + 11x - 5
		const PolynomialRoots roots = Math::SolveCubicEquation(-7.0, 11.0, -5.0);

		REQUIRE_EQ(roots.count, 2u);
		CHECK(IsClose(roots.roots[0], 1.0, 1e-6));
		CHECK(IsClose(roots.roots[1], 5.0, 1e-12));
	}

	{
		// (x-1)^3 = x^3 - 3x^2 + 3x - 1
		const PolynomialRoots roots = Math::SolveCubicEquation(-3.0, 3.0, -1.0);

		REQUIRE_EQ(roots.count, 1u);
		CHECK(IsClose(roots.roots[0], 1.0, 1e-4));
	}

	{
		// 近接ペア + 分離根（真の 3 実根だが数値的には二重根と区別困難な間隔）:
		// (x-1)(x-1-1e-7)(x-5)。2 根または 3 根のどちらも許容するが、
		// 近接ペアの代表値と分離根は正確であること。
		const double r2 = (1.0 + 1e-7);
		const PolynomialRoots roots = Math::SolveCubicEquation(
			-(1.0 + r2 + 5.0), (1.0 * r2 + 5.0 + 5.0 * r2), (-5.0 * r2));

		REQUIRE(2 <= roots.count);
		CHECK(roots.count <= 3);
		CHECK(HasRootNear(roots, 1.0, 1e-6));
		CHECK(HasRootNear(roots, 5.0, 1e-9));
	}
}

// [R3-FAIL: pq(1e300, 1.0)] 極端な係数の depressed cubic。
TEST_CASE("PolynomialSolver.CubicPQ.extreme")
{
	{
		// x^3 + x + 1e160 = 0: 実根 1 つ ~ -cbrt(1e160)
		const PolynomialRoots roots = Math::SolveCubicEquation(1.0, 1e160);

		REQUIRE_EQ(roots.count, 1u);
		CHECK(IsClose(roots.roots[0], -2.1544346900318838e+53, 1e-10));
	}

	{
		// x^3 + 1e-160 = 0: 実根 1 つ = cbrt(-1e-160)
		const PolynomialRoots roots = Math::SolveCubicEquation(0.0, 1e-160);

		REQUIRE_EQ(roots.count, 1u);
		CHECK(IsClose(roots.roots[0], -4.6415888336127785e-54, 1e-10));
	}

	{
		// x^3 + 1e300 x + 1 = 0: 実根 1 つ ~ -q/p = -1e-300（正規化数として表現可能）。
		// スケール後係数の underflow を backcheck の元空間 Newton が復元すること。
		// Round 3 は残差 |f| = 1 の根 0 を返していた。
		const PolynomialRoots roots = Math::SolveCubicEquation(1e300, 1.0);

		REQUIRE_EQ(roots.count, 1u);
		CHECK(AllFinite(roots));
		CHECK(IsClose(roots.roots[0], -1e-300, 1e-9));
	}

	{
		// x(x^2 - 1e-108) = 0: 3 根が実用マージ許容内 → {0}（文書化済みの挙動）
		const PolynomialRoots roots = Math::SolveCubicEquation(-1e-108, 0.0);

		REQUIRE_EQ(roots.count, 1u);
		CHECK_EQ(roots.roots[0], 0.0);
	}
}

// [R3-FAIL: 偽根 0 の混入防止は現状運任せ] 極端な monic / 一般 cubic。
TEST_CASE("PolynomialSolver.Cubic.extremeCoefficients")
{
	{
		// x^3 + 1e160 x^2 + 1 = 0: 実根はちょうど 1 つ (~ -1e160)。
		// x = 0 は f(0) = 1 なので根ではない。偽根 0（t=0 の幻ヒット相当）の
		// 混入を禁止する。
		const PolynomialRoots roots = Math::SolveCubicEquation(1e160, 0.0, 1.0);

		REQUIRE_EQ(roots.count, 1u);
		CHECK(AllFinite(roots));
		CHECK(IsClose(roots.roots[0], -1e160, 1e-10));
	}

	{
		// x^3 + 1e160 x^2 + x + 1 = 0: こちらも実根 1 つ。
		const PolynomialRoots roots = Math::SolveCubicEquation(1e160, 1.0, 1.0);

		REQUIRE_EQ(roots.count, 1u);
		CHECK(IsClose(roots.roots[0], -1e160, 1e-10));

		for (const double root : roots)
		{
			CHECK(1e100 < std::abs(root)); // 小さな偽根の禁止
		}
	}

	{
		// 4 引数版, 巨大な最高次係数: 1e160 x^3 - 1e160 x = 0 -> {-1, 0, 1}
		const PolynomialRoots roots = Math::SolveCubicEquation(1e160, 0.0, -1e160, 0.0);

		REQUIRE_EQ(roots.count, 3u);
		CHECK(IsClose(roots.roots[0], -1.0, 1e-12));
		CHECK_EQ(roots.roots[1], 0.0);
		CHECK(IsClose(roots.roots[2], 1.0, 1e-12));
	}
}

// [R3-FAIL] 有限性契約: いかなる入力でも inf / nan の根を返さない。
TEST_CASE("PolynomialSolver.finiteContract")
{
	{
		// -b/a ~ -1e320 は double で表現不能。Round 3 は count=1, root=-inf を
		// 返していた。表現不能な有限実根は「返さない」（count == 0）。
		const PolynomialRoots roots = Math::SolveLinearEquation(1e-160, 1e160);

		CHECK_EQ(roots.count, 0u);
		CHECK(not roots.hasInfiniteSolutions);
	}

	{
		// 境界の内側: -b/a = -1e308 は表現可能なので返す。
		const PolynomialRoots roots = Math::SolveLinearEquation(1e-154, 1e154);

		REQUIRE_EQ(roots.count, 1u);
		CHECK(std::isfinite(roots.roots[0]));
		CHECK(IsClose(roots.roots[0], -1e308, 1e-12));
	}

	{
		// quadratic の c == 0 分岐: -b/a が overflow するケース。
		// Round 3 は inf 根を生成後、UniqueSorted の inf <= inf 比較で偶然
		// 0 にマージしていた。明示的に {0} のみを返すこと。
		const PolynomialRoots roots = Math::SolveQuadraticEquation(1e-160, 1e160, 0.0);

		REQUIRE_EQ(roots.count, 1u);
		CHECK(AllFinite(roots));
		CHECK_EQ(roots.roots[0], 0.0);
	}

	{
		// 同分岐で表現可能な巨大根は保持する: {0, -1e200}
		const PolynomialRoots roots = Math::SolveQuadraticEquation(1e-200, 1.0, 0.0);

		REQUIRE_EQ(roots.count, 2u);
		CHECK(AllFinite(roots));
		CHECK(IsClose(roots.roots[0], -1e200, 1e-12));
		CHECK_EQ(roots.roots[1], 0.0);
	}

	{
		// 掃き出し: 極端な係数比の組に対して有限性のみを一括検査
		constexpr std::array<std::array<double, 3>, 6> QuadCases{ {
			{ 1e-160, 1e160, 1e160 },
			{ 1e-160, -1e160, 0.0 },
			{ 1e160, 1e-160, -1e160 },
			{ 1e-300, 1.0, 1.0 },
			{ 1.0, 1e300, -1e300 },
			{ 1e300, -1e300, -1e300 },
		} };

		for (const auto& q : QuadCases)
		{
			const PolynomialRoots roots = Math::SolveQuadraticEquation(q[0], q[1], q[2]);
			CHECK(AllFinite(roots));
		}

		constexpr std::array<std::array<double, 4>, 5> CubicCases{ {
			{ 1e-160, 1.0, 1.0, 1.0 },
			{ 1.0, 1e160, 1e160, 1e160 },
			{ 1e160, 1.0, 1.0, 1e-160 },
			{ 1e-300, 1e300, 0.0, -1e300 },
			{ 1.0, -3e150, 3e300, -1e160 },
		} };

		for (const auto& c : CubicCases)
		{
			const PolynomialRoots roots = Math::SolveCubicEquation(c[0], c[1], c[2], c[3]);
			CHECK(AllFinite(roots));
		}
	}
}

// 解の残差契約: 返された根は元の方程式を項スケール相対 1e-6 以下で満たす
// （overflow により元スケールで評価不能な巨大根は除く）。
TEST_CASE("PolynomialSolver.residualContract")
{
	constexpr std::array<std::array<double, 4>, 6> Cases{ {
		{ 1.0, -6.0, 11.0, -6.0 },
		{ 1.0, -(1e10 + 3.0), 3e10 + 2.0, -2e10 },
		{ 2.0, -12.0, 22.0, -12.0 },
		{ 1.0, 0.0, 1.0, 1.0 },
		{ 1.0, -0.06, 0.0011, -6e-6 },
		{ 5.0, 1.0, -3.0, 0.25 },
	} };

	for (const auto& c : Cases)
	{
		const PolynomialRoots roots = Math::SolveCubicEquation(c[0], c[1], c[2], c[3]);

		for (const double x : roots)
		{
			const double x2 = (x * x);
			const double t3 = ((c[0] * x2) * x);
			const double t2 = (c[1] * x2);
			const double t1 = (c[2] * x);

			if (not (std::isfinite(t3) && std::isfinite(t2) && std::isfinite(t1)))
			{
				continue; // 元スケールで評価不能な巨大根はスケール空間の検証に委ねる
			}

			const double f = (((t3 + t2) + t1) + c[3]);
			const double scale = std::max({ std::abs(t3), std::abs(t2), std::abs(t1), std::abs(c[3]) });

			if (scale == 0.0)
			{
				CHECK_EQ(f, 0.0);
			}
			else
			{
				CHECK((std::abs(f) / scale) <= 1e-6);
			}
		}
	}
}
