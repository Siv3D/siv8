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
//	Authors (OpenSiv3D challenge participants)
//	- Ebishu
//	- fal_rnd
//	- きつねび
//-----------------------------------------------

# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Bezier2.hpp>
# include <Siv3D/Geometry2D/Distance.hpp>
# include <Siv3D/Polygon/GeometryCommon.hpp>

namespace s3d
{
	namespace
	{
		constexpr double Epsilon = 1e-12;

		[[nodiscard]]
		static constexpr bool IsZero(const double x, const double eps = Epsilon) noexcept
		{
			return (Abs(x) <= eps);
		}

		[[nodiscard]]
		static Array<double> UniqueSorted(Array<double> v, const double eps = Epsilon)
		{
			if (v.size() < 2)
			{
				return v;
			}

			// 重複排除
			{
				v.sort();

				size_t writeIdx = 0;

				for (size_t readIdx = 1; readIdx < v.size(); ++readIdx)
				{
					if (eps < Abs(v[readIdx] - v[writeIdx]))
					{
						++writeIdx;

						if (writeIdx != readIdx)
						{
							v[writeIdx] = v[readIdx];
						}
					}
				}

				v.resize(writeIdx + 1);
			}

			return v;
		}

		// ax + b = 0
		[[nodiscard]]
		static Optional<Array<double>> SolveLinearEquation(const double a, const double b)
		{
			if (IsZero(a))
			{
				return none;
			}

			return Array<double>{ (-b / a) };
		}

		// ax^2 + bx + c = 0
		[[nodiscard]]
		static Optional<Array<double>> SolveQuadraticEquation(const double a, const double b, const double c)
		{
			if (IsZero(a))
			{
				return SolveLinearEquation(b, c);
			}

			if (IsZero(c))
			{
				return UniqueSorted({ 0.0, (-b / a) });
			}

			const double d = (b * b - 4 * a * c);

			if (d < -Epsilon)
			{
				return none; // 実数解なし
			}

			if (IsZero(d))
			{
				return Array<double>{ (-0.5 * b / a) };
			}

			const double s = std::sqrt(d);
			const double t = ((0.0 < b) ? (-b - s) : (-b + s));

			double x1 = ((2.0 * c) / t);
			double x2 = ((0.5 * t) / a);

			if (x2 < x1)
			{
				std::swap(x1, x2);
			}

			return UniqueSorted({ x1, x2 });
		}

		// x^3 + px + q = 0
		[[nodiscard]]
		static Optional<Array<double>> SolveCubicEquation(const double p, const double q)
		{
			Array<double> result;

			if (IsZero(q)) // x(x^2 + p) = 0
			{
				result.push_back(0.0);

				if (-Epsilon <= -p)
				{
					const double r = std::sqrt(std::max(0.0, -p));
					result.push_back(-r);
					result.push_back(r);
				}

				return UniqueSorted(std::move(result));
			}

			if (IsZero(p)) // x^3 + q = 0
			{ 
				result.push_back(std::cbrt(-q));
				return result;
			}

			const double p3 = (p / 3.0);
			const double q2 = (q / 2.0);
			const double d = (q2 * q2 + p3 * p3 * p3); // Δ = (q/2)^2 + (p/3)^3

			if (IsZero(d))
			{
				// 多重根
				const double u = std::cbrt(-q2);
				result.push_back(2.0 * u);
				result.push_back(-u);
				return UniqueSorted(std::move(result));
			}

			if (0.0 < d)
			{
				// 実根 1 つ
				const double s = std::sqrt(d);
				const double u = std::cbrt(-q2 + s);
				const double v = std::cbrt(-q2 - s);

				result.push_back(u + v);
				return result;
			}

			// d < 0 : 実根 3 つ（三角関数）
			const double r = (2.0 * std::sqrt(-p3));

			// cos(phi) = -q/2 / sqrt(-(p/3)^3)
			const double denom = std::sqrt(-(p3 * p3 * p3));
			double cosphi = ((-q2) / denom);
			cosphi = std::max(-1.0, std::min(1.0, cosphi));
			const double phi = std::acos(cosphi);

			result.push_back(r * std::cos(phi / 3.0));
			result.push_back(r * std::cos((phi + 2.0 * Math::Pi) / 3.0));
			result.push_back(r * std::cos((phi + 4.0 * Math::Pi) / 3.0));
			return UniqueSorted(std::move(result));
		}

		// x^3 + ax^2 + bx + c = 0
		[[nodiscard]]
		static Optional<Array<double>> SolveCubicEquation(const double a, const double b, const double c)
		{
			// c ≒ 0 なら因数分解で早期 return（重複を避ける）
			if (IsZero(c))
			{
				Array<double> result = { 0.0 };

				if (const auto opt = SolveQuadraticEquation(1.0, a, b))
				{
					result.append(opt->begin(), opt->end());
					return UniqueSorted(std::move(result));
				}

				return result;
			}

			const double a_third = a / 3.0;
			const double p = std::fma(-a_third, a, b);
			const double q = std::fma(a, std::fma(((2.0 / 27.0) * a), a, (-b / 3.0)), c);

			if (const auto opt = SolveCubicEquation(p, q))
			{
				Array<double> result = *opt;
				
				for (double& y : result)
				{
					y -= a_third;
				}
				
				return result;
			}

			return none;
		}

		// ax^3 + bx^2 + cx + d = 0
		[[nodiscard]]
		static Optional<Array<double>> SolveCubicEquation(const double a, const double b, const double c, const double d)
		{
			if (IsZero(a))
			{
				return SolveQuadraticEquation(b, c, d);
			}

			return SolveCubicEquation((b / a), (c / a), (d / a));
		}
	}

	namespace Geometry2D
	{
		//////////////////////////////////////////////////
		//
		//	Distance(Vec2, _)
		//
		//////////////////////////////////////////////////

		double Distance(const Vec2& a, const Line& b)
		{
			return boost::geometry::distance(GVec2{ a.x, a.y }, GSegment{ b.start, b.end });
		}

		double Distance(const Vec2& a, const Bezier2& b)
		{
			constexpr double Eps = 1e-12;
			constexpr double DegenerateRel = 1e-24; // for M.dot(M) relative check (squared magnitude)

			const Vec2 M = (b.p0 - 2 * b.p1 + b.p2);
			const Vec2 N = (b.p1 - b.p0);
			const Vec2 K = (a - b.p0);

			const double t1 = M.dot(M);
			const double t2 = (3 * M.dot(N));
			const double t3 = (2 * N.dot(N) - M.dot(K));
			const double t4 = -N.dot(K);

			// 退化: (ほぼ)直線 → 線分 [p0, p2] への距離に落とす
			{
				const Vec2 chord = (b.p2 - b.p0);
				const double scale = Max(1.0, chord.dot(chord)); // length^2 scale
				
				if (t1 <= (DegenerateRel * scale))
				{
					return Line{ b.p0, b.p2 }.distanceFrom(a);
				}
			}

			double minDistSq = Min(a.distanceFromSq(b.p0), a.distanceFromSq(b.p2));
			
			if (const auto opt = SolveCubicEquation(t1, t2, t3, t4))
			{
				for (const double x : *opt)
				{
					if (InRange(x, -Eps, (1.0 + Eps)))
					{
						const double t = Clamp(x, 0.0, 1.0);
						minDistSq = Min(minDistSq, a.distanceFromSq(b.getPos(t)));
					}
				}
			}

			return std::sqrt(minDistSq);
		}
	}
}
