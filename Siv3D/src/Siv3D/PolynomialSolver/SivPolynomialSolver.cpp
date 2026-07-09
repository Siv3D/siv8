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

# include <Siv3D/Utility.hpp>
# include <Siv3D/PolynomialSolver.hpp>
# include <Siv3D/MathConstants.hpp>
# include <cassert>
# include <cmath>
# include <limits>

namespace s3d
{
	namespace
	{
		// 分岐判定用。判別式はそれを構成する項に対して相対判定する。
		constexpr double DiscriminantRelEpsilon = 1e-12;

		// 最高次係数の縮退判定用。通常の許容誤差ではなく、overflow と極端な係数比を避けるための下限。
		constexpr double QuadraticDegreeRelEpsilon = 1e-150;
		constexpr double CubicDegreeRelEpsilon = 1e-100;

		// 重複根のマージ用。0 付近では実用上のスナップを許す。
		constexpr double RootMergeEpsilon = 1e-12;

		// cubic の近接多重根判定で生じうる偽の有限根を落とすための残差判定用。
		constexpr double CubicResidualRelEpsilon = 1e-8;

		[[nodiscard]]
		static constexpr double MaxAbs(const double a, const double b) noexcept
		{
			return Max(Abs(a), Abs(b));
		}

		[[nodiscard]]
		static constexpr double MaxAbs(const double a, const double b, const double c) noexcept
		{
			return Max(MaxAbs(a, b), Abs(c));
		}

		[[nodiscard]]
		static constexpr double MaxAbs(const double a, const double b, const double c, const double d) noexcept
		{
			return Max(MaxAbs(a, b, c), Abs(d));
		}

		[[nodiscard]]
		static constexpr bool IsRelZero(const double x, const double scale, const double relEps) noexcept
		{
			return (Abs(x) <= (relEps * scale));
		}

		[[nodiscard]]
		static constexpr double NormalizeZero(const double x) noexcept
		{
			return ((x == 0.0) ? 0.0 : x); // -0.0 を避ける
		}

		static void PushRoot(PolynomialRoots& result, const double x) noexcept
		{
			assert(result.count < 3);
			result.roots[result.count++] = NormalizeZero(x);
		}

		static void Sort(PolynomialRoots& result) noexcept
		{
			for (uint32 i = 1; i < result.count; ++i)
			{
				const double value = result.roots[i];
				uint32 j = i;

				while ((0 < j) && (value < result.roots[j - 1]))
				{
					result.roots[j] = result.roots[j - 1];
					--j;
				}

				result.roots[j] = value;
			}
		}

		[[nodiscard]]
		static bool IsSameRoot(const double a, const double b) noexcept
		{
			const double scale = Max(1.0, MaxAbs(a, b));
			return (Abs(a - b) <= (RootMergeEpsilon * scale));
		}

		static void UniqueSorted(PolynomialRoots& result) noexcept
		{
			if (result.count < 2)
			{
				return;
			}

			Sort(result);

			uint32 writeIndex = 0;

			for (uint32 readIndex = 1; readIndex < result.count; ++readIndex)
			{
				if (IsSameRoot(result.roots[writeIndex], result.roots[readIndex]))
				{
					// 0 近傍のマージでは、より 0 に近い代表値を残す。
					if (Abs(result.roots[readIndex]) < Abs(result.roots[writeIndex]))
					{
						result.roots[writeIndex] = result.roots[readIndex];
					}
				}
				else
				{
					++writeIndex;

					if (writeIndex != readIndex)
					{
						result.roots[writeIndex] = result.roots[readIndex];
					}
				}
			}

			result.count = (writeIndex + 1);
		}

		[[nodiscard]]
		static double CubicValue(const double x, const double a, const double b, const double c, const double d) noexcept
		{
			return std::fma(std::fma(std::fma(a, x, b), x, c), x, d);
		}

		[[nodiscard]]
		static double CubicRelativeResidual(const double x, const double a, const double b, const double c, const double d) noexcept
		{
			const double x2 = (x * x);

			// この評価順は、a が小さく x が大きいケースで ax^3 を不要に overflow させないために重要。
			const double ax3 = ((a * x2) * x);
			const double bx2 = (b * x2);
			const double cx = (c * x);
			const double f = (((ax3 + bx2) + cx) + d);
			const double scale = Max(MaxAbs(ax3, bx2, cx), Abs(d));

			if (scale == 0.0)
			{
				return 0.0;
			}

			return (Abs(f) / scale);
		}

		static void FilterCubicRoots(PolynomialRoots& result, const double a, const double b, const double c, const double d) noexcept
		{
			const uint32 oldCount = result.count;
			double bestRoot = 0.0;
			double bestResidual = std::numeric_limits<double>::infinity();
			uint32 writeIndex = 0;

			for (uint32 readIndex = 0; readIndex < oldCount; ++readIndex)
			{
				const double root = result.roots[readIndex];
				const double residual = CubicRelativeResidual(root, a, b, c, d);

				if (residual < bestResidual)
				{
					bestResidual = residual;
					bestRoot = root;
				}

				if (residual <= CubicResidualRelEpsilon)
				{
					result.roots[writeIndex++] = root;
				}
			}

			// cubic は少なくとも 1 つの実根を持つため、filter が全候補を落とす場合は最小残差の候補を残す。
			if ((writeIndex == 0) && (0 < oldCount))
			{
				result.roots[0] = NormalizeZero(bestRoot);
				result.count = 1;
			}
			else
			{
				result.count = writeIndex;
			}

			UniqueSorted(result);
		}

		static void PolishCubicRoots(PolynomialRoots& result, const double a, const double b, const double c, const double d) noexcept
		{
			for (uint32 i = 0; i < result.count; ++i)
			{
				const double x = result.roots[i];

				// f(x) = ax^3 + bx^2 + cx + d
				const double f = CubicValue(x, a, b, c, d);
				const double df = std::fma(std::fma((3.0 * a), x, (2.0 * b)), x, c);
				const double dfScale = MaxAbs((3.0 * a * x * x), (2.0 * b * x), c);

				// 多重根付近では Newton step が不安定になりやすいためスキップする。
				if ((dfScale != 0.0) && (not IsRelZero(df, dfScale, DiscriminantRelEpsilon)))
				{
					const double next = NormalizeZero(x - (f / df));
					const double fNext = CubicValue(next, a, b, c, d);

					// loose な残差 filter だけでは悪化 step を受け入れうるため、単調改善する場合のみ採用する。
					if (Abs(fNext) <= Abs(f))
					{
						result.roots[i] = next;
					}
				}
			}

			FilterCubicRoots(result, a, b, c, d);
		}

		[[nodiscard]]
		static PolynomialRoots SolveDepressedCubicUnit(const double p, const double q)
		{
			PolynomialRoots result;

			if (q == 0.0) // x(x^2 + p) = 0
			{
				PushRoot(result, 0.0);

				if (p < 0.0)
				{
					const double r = std::sqrt(-p);
					PushRoot(result, -r);
					PushRoot(result, r);
				}

				UniqueSorted(result);
				return result;
			}

			if (p == 0.0) // x^3 + q = 0
			{
				PushRoot(result, std::cbrt(-q));
				return result;
			}

			const double p3 = (p / 3.0);
			const double q2 = (q / 2.0);
			const double p3Cube = (p3 * p3 * p3);
			const double q2Square = (q2 * q2);
			const double discriminant = (q2Square + p3Cube); // Δ = (q/2)^2 + (p/3)^3
			const double discriminantScale = Max(q2Square, Abs(p3Cube));
			const double discriminantEpsilon = (DiscriminantRelEpsilon * discriminantScale);

			// exact fast paths の後では discriminantScale == 0.0 は通常到達しないが、防御的に多重根側へ倒す。
			if ((discriminantScale == 0.0) || (Abs(discriminant) <= discriminantEpsilon))
			{
				// 多重根
				const double u = std::cbrt(-q2);
				PushRoot(result, (2.0 * u));
				PushRoot(result, -u);
				UniqueSorted(result);
				return result;
			}

			if (0.0 < discriminant)
			{
				// 実根 1 つ。片側の cbrt は u * v = -p/3 から復元して cancellation を避ける。
				const double s = std::sqrt(discriminant);
				const double w = (-q2 - std::copysign(s, q2));
				const double u = std::cbrt(w);
				const double v = ((u == 0.0) ? 0.0 : (-p3 / u));

				PushRoot(result, (u + v));
				return result;
			}

			// discriminant < 0 : 実根 3 つ（三角関数）
			const double r = (2.0 * std::sqrt(-p3));

			// cos(phi) = -q/2 / sqrt(-(p/3)^3)
			const double denom = std::sqrt(-p3Cube);
			double cosphi = ((-q2) / denom);
			cosphi = Max(-1.0, Min(1.0, cosphi));
			const double phi = std::acos(cosphi);

			PushRoot(result, (r * std::cos(phi / 3.0)));
			PushRoot(result, (r * std::cos((phi + 2.0 * Math::Pi) / 3.0)));
			PushRoot(result, (r * std::cos((phi + 4.0 * Math::Pi) / 3.0)));
			UniqueSorted(result);
			return result;
		}
	}

	namespace Math
	{
		////////////////////////////////////////////////////////////////
		//
		//	SolveLinearEquation
		//
		////////////////////////////////////////////////////////////////

		PolynomialRoots SolveLinearEquation(double a, double b)
		{
			const double scale = MaxAbs(a, b);

			if (scale == 0.0)
			{
				return PolynomialRoots::Infinite();
			}

			// 方程式全体をスケールして、極端な係数比の判定を相対化する。
			a /= scale;
			b /= scale;

			if (a == 0.0)
			{
				return ((b == 0.0) ? PolynomialRoots::Infinite() : PolynomialRoots{});
			}

			PolynomialRoots result;
			PushRoot(result, (-b / a));
			return result;
		}

		////////////////////////////////////////////////////////////////
		//
		//	SolveQuadraticEquation
		//
		////////////////////////////////////////////////////////////////

		PolynomialRoots SolveQuadraticEquation(double a, double b, double c)
		{
			const double scale = MaxAbs(a, b, c);

			if (scale == 0.0)
			{
				return PolynomialRoots::Infinite();
			}

			// 方程式全体をスケールして、判別式の overflow / underflow を抑える。
			a /= scale;
			b /= scale;
			c /= scale;

			if (a == 0.0)
			{
				return SolveLinearEquation(b, c);
			}

			if (c == 0.0)
			{
				PolynomialRoots result;
				PushRoot(result, 0.0);
				PushRoot(result, (-b / a));
				UniqueSorted(result);
				return result;
			}

			if (IsRelZero(a, 1.0, QuadraticDegreeRelEpsilon))
			{
				return SolveLinearEquation(b, c);
			}

			const double ac4 = (4.0 * a * c);
			const double discriminant = (b * b - ac4);
			const double discriminantScale = (b * b + Abs(ac4));
			const double discriminantEpsilon = (DiscriminantRelEpsilon * discriminantScale);

			if (discriminant < -discriminantEpsilon)
			{
				return PolynomialRoots{}; // 実数解なし
			}

			if (Abs(discriminant) <= discriminantEpsilon)
			{
				PolynomialRoots result;
				PushRoot(result, (-0.5 * b / a));
				return result;
			}

			const double s = std::sqrt(Max(0.0, discriminant));
			const double t = ((0.0 < b) ? (-b - s) : (-b + s));

			PolynomialRoots result;
			PushRoot(result, ((2.0 * c) / t));
			PushRoot(result, ((0.5 * t) / a));
			UniqueSorted(result);
			return result;
		}

		////////////////////////////////////////////////////////////////
		//
		//	SolveCubicEquation
		//
		////////////////////////////////////////////////////////////////

		PolynomialRoots SolveCubicEquation(const double p, const double q)
		{
			if (q == 0.0) // x(x^2 + p) = 0
			{
				PolynomialRoots result;
				PushRoot(result, 0.0);

				if (p < 0.0)
				{
					const double r = std::sqrt(-p);
					PushRoot(result, -r);
					PushRoot(result, r);
				}

				UniqueSorted(result);
				return result;
			}

			if (p == 0.0) // x^3 + q = 0
			{
				PolynomialRoots result;
				PushRoot(result, std::cbrt(-q));
				return result;
			}

			// x = y * xScale として、p, q の極端な大小による判別式の overflow / underflow を避ける。
			const double xScale = Max(std::sqrt(Abs(p)), std::cbrt(Abs(q)));

			if (xScale == 0.0)
			{
				PolynomialRoots result;
				PushRoot(result, 0.0);
				return result;
			}

			const double scaledP = ((p / xScale) / xScale);
			const double scaledQ = (((q / xScale) / xScale) / xScale);

			PolynomialRoots result = SolveDepressedCubicUnit(scaledP, scaledQ);
			PolishCubicRoots(result, 1.0, 0.0, scaledP, scaledQ);

			for (double& y : result)
			{
				y = NormalizeZero(y * xScale);
			}

			return result;
		}

		PolynomialRoots SolveCubicEquation(const double a, const double b, const double c)
		{
			if (c == 0.0)
			{
				PolynomialRoots result;
				PushRoot(result, 0.0);

				PolynomialRoots quadraticRoots = SolveQuadraticEquation(1.0, a, b);

				for (const double root : quadraticRoots)
				{
					PushRoot(result, root);
				}

				UniqueSorted(result);
				return result;
			}

			// x = y * xScale として、depressed-cubic 変換前に monic 係数を unit scale へ寄せる。
			const double xScale = Max(Abs(a), Max(std::sqrt(Abs(b)), std::cbrt(Abs(c))));

			if (xScale == 0.0)
			{
				PolynomialRoots result;
				PushRoot(result, 0.0);
				return result;
			}

			const double scaledA = (a / xScale);
			const double scaledB = ((b / xScale) / xScale);
			const double scaledC = (((c / xScale) / xScale) / xScale);

			const double aThird = (scaledA / 3.0);
			const double p = std::fma(-aThird, scaledA, scaledB);
			const double q = std::fma(scaledA, std::fma(((2.0 / 27.0) * scaledA), scaledA, (-scaledB / 3.0)), scaledC);

			PolynomialRoots result = SolveCubicEquation(p, q);

			for (double& z : result)
			{
				z = NormalizeZero(z - aThird);
			}

			// polish / filter はスケーリング後の y 空間で行い、巨大根評価時の overflow を避ける。
			PolishCubicRoots(result, 1.0, scaledA, scaledB, scaledC);

			for (double& y : result)
			{
				y = NormalizeZero(y * xScale);
			}

			return result;
		}

		PolynomialRoots SolveCubicEquation(double a, double b, double c, double d)
		{
			const double scale = MaxAbs(a, b, c, d);

			if (scale == 0.0)
			{
				return PolynomialRoots::Infinite();
			}

			// 方程式全体をスケールして、monic 化前の overflow / underflow を抑える。
			a /= scale;
			b /= scale;
			c /= scale;
			d /= scale;

			if (IsRelZero(a, 1.0, CubicDegreeRelEpsilon))
			{
				return SolveQuadraticEquation(b, c, d);
			}

			const double invA = (1.0 / a);
			return SolveCubicEquation((b * invA), (c * invA), (d * invA));
		}
	}
}
