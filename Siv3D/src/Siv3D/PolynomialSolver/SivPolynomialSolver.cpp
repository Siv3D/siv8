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
		constexpr double CubicDegreeRelEpsilon = 1e-100;

		// 重複根のマージ用。0 付近では実用上のスナップを許す。
		constexpr double RootMergeEpsilon = 1e-12;

		// cubic の候補根を落とすための残差判定用。ゲーム・交差判定用途向けの実用閾値。
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

		static bool PushRoot(PolynomialRoots& result, const double x) noexcept
		{
			if (not std::isfinite(x))
			{
				return false;
			}

			assert(result.count < 3);

			if (result.count == 3)
			{
				return false;
			}

			result.roots[result.count++] = NormalizeZero(x);
			return true;
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

		struct CubicEval
		{
			double f = 0.0;
			double scale = 0.0;
			bool finite = false;
		};

		[[nodiscard]]
		static CubicEval EvaluateCubicTermwise(const double x, const double a, const double b, const double c, const double d) noexcept
		{
			const double x2 = (x * x);

			// この評価順は、a が小さく x が大きいケースで ax^3 を不要に overflow させないために重要。
			const double ax3 = ((a * x2) * x);
			const double bx2 = (b * x2);
			const double cx = (c * x);

			if (not (std::isfinite(ax3) && std::isfinite(bx2) && std::isfinite(cx)))
			{
				return CubicEval{ 0.0, 0.0, false };
			}

			const double f = (((ax3 + bx2) + cx) + d);
			const double scale = Max(MaxAbs(ax3, bx2, cx), Abs(d));

			if (not (std::isfinite(f) && std::isfinite(scale)))
			{
				return CubicEval{ 0.0, 0.0, false };
			}

			return CubicEval{ f, scale, true };
		}

		[[nodiscard]]
		static double CubicRelativeResidual(const double x, const double a, const double b, const double c, const double d) noexcept
		{
			const CubicEval eval = EvaluateCubicTermwise(x, a, b, c, d);

			if (not eval.finite)
			{
				return std::numeric_limits<double>::infinity();
			}

			if (eval.scale == 0.0)
			{
				return 0.0;
			}

			return (Abs(eval.f) / eval.scale);
		}

		static void PolishCubicRoot(double& x, const double a, const double b, const double c, const double d) noexcept
		{
			const double f = CubicValue(x, a, b, c, d);
			const double df = std::fma(std::fma((3.0 * a), x, (2.0 * b)), x, c);

			if (not (std::isfinite(f) && std::isfinite(df) && (df != 0.0)))
			{
				return;
			}

			const double next = NormalizeZero(x - (f / df));

			if (not std::isfinite(next))
			{
				return;
			}

			const double fNext = CubicValue(next, a, b, c, d);

			// loose な残差 filter だけでは悪化 step を受け入れうるため、単調改善する場合のみ採用する。
			if (std::isfinite(fNext) && (Abs(fNext) <= Abs(f)))
			{
				x = next;
			}
		}

		static void PolishCubicRoots(PolynomialRoots& result, const double a, const double b, const double c, const double d, const uint32 iterations = 1) noexcept
		{
			for (uint32 i = 0; i < result.count; ++i)
			{
				for (uint32 step = 0; step < iterations; ++step)
				{
					PolishCubicRoot(result.roots[i], a, b, c, d);
				}
			}
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

				if (not std::isfinite(root))
				{
					continue;
				}

				const double residual = CubicRelativeResidual(root, a, b, c, d);

				if (residual < bestResidual)
				{
					bestResidual = residual;
					bestRoot = root;
				}

				if (residual <= CubicResidualRelEpsilon)
				{
					result.roots[writeIndex++] = NormalizeZero(root);
				}
			}

			// cubic は少なくとも 1 つの実根を持つため、filter が全候補を落とす場合は最小残差の候補を残す。
			if ((writeIndex == 0) && (0 < oldCount) && (bestResidual < std::numeric_limits<double>::infinity()))
			{
				result.roots[0] = NormalizeZero(bestRoot);
				result.count = 1;
			}
			else
			{
				result.count = writeIndex;
			}

			Sort(result);
		}

		static void BackcheckCubicRootsInOriginalScale(PolynomialRoots& result, const double a, const double b, const double c, const double d) noexcept
		{
			const uint32 oldCount = result.count;
			double bestRoot = 0.0;
			double bestResidual = std::numeric_limits<double>::infinity();
			uint32 writeIndex = 0;

			for (uint32 readIndex = 0; readIndex < oldCount; ++readIndex)
			{
				double root = result.roots[readIndex];

				if (not std::isfinite(root))
				{
					continue;
				}

				CubicEval eval = EvaluateCubicTermwise(root, a, b, c, d);

				if (not eval.finite)
				{
					// 元スケールで評価不能な巨大根は、スケール空間の verdict に委ねる。
					result.roots[writeIndex++] = NormalizeZero(root);
					continue;
				}

				const double df = std::fma(std::fma((3.0 * a), root, (2.0 * b)), root, c);

				if (std::isfinite(df) && (df != 0.0))
				{
					const double next = NormalizeZero(root - (eval.f / df));

					if (std::isfinite(next))
					{
						const CubicEval nextEval = EvaluateCubicTermwise(next, a, b, c, d);

						if (nextEval.finite && (Abs(nextEval.f) <= Abs(eval.f)))
						{
							root = next;
							eval = nextEval;
						}
					}
				}

				const double residual = ((eval.scale == 0.0) ? 0.0 : (Abs(eval.f) / eval.scale));

				if (residual < bestResidual)
				{
					bestResidual = residual;
					bestRoot = root;
				}

				if (residual <= CubicResidualRelEpsilon)
				{
					result.roots[writeIndex++] = NormalizeZero(root);
				}
			}

			// genuine cubic の invariant: 表現可能な候補がすべて落ちる場合は最小残差の候補を残す。
			if ((writeIndex == 0) && (0 < oldCount) && (bestResidual < std::numeric_limits<double>::infinity()))
			{
				result.roots[0] = NormalizeZero(bestRoot);
				result.count = 1;
			}
			else
			{
				result.count = writeIndex;
			}

			Sort(result);
		}

		[[nodiscard]]
		static PolynomialRoots SolveQuadraticEquationRaw(double a, double b, double c, const bool mergeRoots)
		{
			const double scale = MaxAbs(a, b, c);

			if (scale == 0.0)
			{
				return PolynomialRoots::Infinite();
			}

			a /= scale;
			b /= scale;
			c /= scale;

			if (a == 0.0)
			{
				PolynomialRoots result;

				if (b == 0.0)
				{
					return ((c == 0.0) ? PolynomialRoots::Infinite() : PolynomialRoots{});
				}

				PushRoot(result, (-c / b));
				return result;
			}

			if (c == 0.0)
			{
				PolynomialRoots result;
				PushRoot(result, 0.0);
				PushRoot(result, (-b / a));

				if (mergeRoots)
				{
					UniqueSorted(result);
				}
				else
				{
					Sort(result);
				}

				return result;
			}

			const double ac4 = (4.0 * a * c);
			const double discriminant = (b * b - ac4);
			const double discriminantScale = (b * b + Abs(ac4));
			const double discriminantEpsilon = (DiscriminantRelEpsilon * discriminantScale);

			if (discriminant < -discriminantEpsilon)
			{
				return PolynomialRoots{}; // 実数解なし
			}

			PolynomialRoots result;

			if (Abs(discriminant) <= discriminantEpsilon)
			{
				PushRoot(result, (-0.5 * b / a));
				return result;
			}

			const double s = std::sqrt(Max(0.0, discriminant));
			const double t = ((0.0 < b) ? (-b - s) : (-b + s));

			if (t == 0.0)
			{
				PushRoot(result, (-0.5 * b / a));
				return result;
			}

			PushRoot(result, ((2.0 * c) / t));
			PushRoot(result, ((0.5 * t) / a));

			if (mergeRoots)
			{
				UniqueSorted(result);
			}
			else
			{
				Sort(result);
			}

			return result;
		}

		[[nodiscard]]
		static PolynomialRoots SolveCubicUnitMonic(const double a, const double b, const double c)
		{
			if (c == 0.0)
			{
				PolynomialRoots result;
				PushRoot(result, 0.0);

				const PolynomialRoots quadraticRoots = SolveQuadraticEquationRaw(1.0, a, b, false);

				for (const double root : quadraticRoots)
				{
					PushRoot(result, root);
				}

				Sort(result);
				return result;
			}

			const double aThird = (a / 3.0);
			const double p = std::fma(-aThird, a, b);
			const double q = std::fma(a, std::fma(((2.0 / 27.0) * a), a, (-b / 3.0)), c);

			const double p3 = (p / 3.0);
			const double q2 = (q / 2.0);
			const double p3Cube = (p3 * p3 * p3);
			const double q2Square = (q2 * q2);
			const double discriminant = (q2Square + p3Cube); // Δ = (q/2)^2 + (p/3)^3
			const double discriminantScale = Max(q2Square, Abs(p3Cube));
			const double discriminantEpsilon = (DiscriminantRelEpsilon * discriminantScale);

			PolynomialRoots result;

			if (discriminant < -discriminantEpsilon)
			{
				// clean な 3 実根領域では三角関数法を維持する。
				const double r = (2.0 * std::sqrt(-p3));
				const double denom = std::sqrt(-p3Cube);
				double cosphi = ((-q2) / denom);
				cosphi = Max(-1.0, Min(1.0, cosphi));
				const double phi = std::acos(cosphi);

				double y0 = ((r * std::cos(phi / 3.0)) - aThird);
				double y1 = ((r * std::cos((phi + 2.0 * Math::Pi) / 3.0)) - aThird);
				double y2 = ((r * std::cos((phi + 4.0 * Math::Pi) / 3.0)) - aThird);

				// 三角関数法の根は絶対誤差 ~eps * (根スケール) を持つため、支配根に対して
				// 極端に小さい根は自身のスケールで相対精度を失い、後段の filter に落とされうる
				// （例: 根 {1e-8, 5e3, 9e7} で最小根が消失する）。
				// 最小絶対値の根を根の積の恒等式 y0 * y1 * y2 = -c から再計算し、
				// 残差が改善する場合のみ採用する。
				{
					double* ys[3] = { &y0, &y1, &y2 };
					int minIndex = 0;

					for (int i = 1; i < 3; ++i)
					{
						if (Abs(*ys[i]) < Abs(*ys[minIndex]))
						{
							minIndex = i;
						}
					}

					const double others = ((*ys[(minIndex + 1) % 3]) * (*ys[(minIndex + 2) % 3]));

					if ((others != 0.0) && std::isfinite(others))
					{
						const double candidate = (-c / others);

						if (std::isfinite(candidate))
						{
							const double fOld = CubicValue(*ys[minIndex], 1.0, a, b, c);
							const double fNew = CubicValue(candidate, 1.0, a, b, c);

							if (std::isfinite(fNew) && (Abs(fNew) <= Abs(fOld)))
							{
								*ys[minIndex] = candidate;
							}
						}
					}
				}

				PushRoot(result, y0);
				PushRoot(result, y1);
				PushRoot(result, y2);
				Sort(result);
				return result;
			}

			// Δ >= 0 と near-zero Δ は、単一根を先に求めて scaled monic 空間で deflate する。
			double x = 0.0;

			if ((discriminantScale == 0.0) || (Abs(discriminant) <= discriminantEpsilon))
			{
				// Δ = 0 形の well-conditioned な単純根。三重根の場合もこの値が代表根になる。
				const double u = std::cbrt(-q2);
				x = ((2.0 * u) - aThird);
			}
			else
			{
				// 実根 1 つ。片側の cbrt は u * v = -p/3 から復元して cancellation を避ける。
				const double s = std::sqrt(discriminant);
				const double w = (-q2 - std::copysign(s, q2));
				const double u = std::cbrt(w);
				const double v = ((u == 0.0) ? 0.0 : (-p3 / u));
				x = ((u + v) - aThird);
			}

			if (std::isfinite(x))
			{
				PolishCubicRoot(x, 1.0, a, b, c);
				PolishCubicRoot(x, 1.0, a, b, c);
				PushRoot(result, x);

				if (x != 0.0)
				{
					const double B = (a + x);
					const double C = (-c / x);

					if (std::isfinite(B) && std::isfinite(C))
					{
						const PolynomialRoots quadraticRoots = SolveQuadraticEquationRaw(1.0, B, C, false);

						for (const double root : quadraticRoots)
						{
							PushRoot(result, root);
						}
					}
				}
			}

			Sort(result);
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
			PushRoot(result, (-b / a)); // overflow して非有限になる有限根は返さない。
			return result;
		}

		////////////////////////////////////////////////////////////////
		//
		//	SolveQuadraticEquation
		//
		////////////////////////////////////////////////////////////////

		PolynomialRoots SolveQuadraticEquation(double a, double b, double c)
		{
			return SolveQuadraticEquationRaw(a, b, c, true);
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

			PolynomialRoots result = SolveCubicUnitMonic(0.0, scaledP, scaledQ);
			PolishCubicRoots(result, 1.0, 0.0, scaledP, scaledQ, 1);
			FilterCubicRoots(result, 1.0, 0.0, scaledP, scaledQ);

			for (double& y : result)
			{
				y = NormalizeZero(y * xScale);
			}

			BackcheckCubicRootsInOriginalScale(result, 1.0, 0.0, p, q);
			UniqueSorted(result);
			return result;
		}

		PolynomialRoots SolveCubicEquation(const double a, const double b, const double c)
		{
			if (c == 0.0)
			{
				PolynomialRoots result;
				PushRoot(result, 0.0);

				const PolynomialRoots quadraticRoots = SolveQuadraticEquation(1.0, a, b);

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

			PolynomialRoots result = SolveCubicUnitMonic(scaledA, scaledB, scaledC);
			PolishCubicRoots(result, 1.0, scaledA, scaledB, scaledC, 1);
			FilterCubicRoots(result, 1.0, scaledA, scaledB, scaledC);

			for (double& y : result)
			{
				y = NormalizeZero(y * xScale);
			}

			BackcheckCubicRootsInOriginalScale(result, 1.0, a, b, c);
			UniqueSorted(result);
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
