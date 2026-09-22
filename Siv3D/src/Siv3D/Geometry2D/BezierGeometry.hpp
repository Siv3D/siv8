//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# pragma once
# include <array>
# include <utility>
# include <Siv3D/Vector2D.hpp>

namespace s3d::detail
{
	// Roots in (0, 1) of a Bernstein polynomial. Derivative roots divide the
	// interval into monotone pieces; coefficients of one sign exclude roots.
	template <size_t Degree>
	struct UnitPolynomialRoots
	{
		std::array<double, Degree> values{};
		size_t count = 0;
	};

	template <size_t Count>
	[[nodiscard]]
	double EvaluateBernstein(const std::array<double, Count>& values, const double t) noexcept
	{
		if constexpr (Count == 1)
		{
			return values[0];
		}
		else
		{
			return [&]<size_t... I>(std::index_sequence<I...>) noexcept
			{
				return EvaluateBernstein(std::array{ ((1.0 - t) * values[I] + t * values[I + 1])... }, t);
			}(std::make_index_sequence<Count - 1>{});
		}
	}

	template <size_t Count>
	[[nodiscard]]
	UnitPolynomialRoots<Count - 1> BernsteinRoots(const std::array<double, Count>& values) noexcept
	{
		constexpr size_t Degree = (Count - 1);
		constexpr double ParameterTolerance = 2.0e-15;
		UnitPolynomialRoots<Degree> roots;
		size_t variations = 0;
		int32 previousSign = 0;
		for (const double value : values)
		{
			const int32 sign = ((0.0 < value) - (value < 0.0));
			if (sign != 0)
			{
				variations += ((previousSign != 0) && (previousSign != sign));
				previousSign = sign;
			}
		}
		if (variations == 0)
		{
			return roots;
		}
		if constexpr (Degree == 1)
		{
			roots.values[roots.count++] = (values[0] / (values[0] - values[1]));
		}
		else
		{
			// Endpoint roots do not affect interior candidates. Remove their
			// t or (1 - t) factor before solving repeated stationary endpoints.
			if ((values.front() == 0.0) || (values.back() == 0.0))
			{
				std::array<double, Degree> reduced;
				for (size_t i = 0; i < Degree; ++i)
				{
					reduced[i] = ((values.front() == 0.0)
						? (values[i + 1] * Degree / (i + 1)) : (values[i] * Degree / (Degree - i)));
				}
				const auto interior = BernsteinRoots(reduced);
				roots.count = interior.count;
				for (size_t i = 0; i < roots.count; ++i)
				{
					roots.values[i] = interior.values[i];
				}
				return roots;
			}
			std::array<double, Degree> derivative;
			for (size_t i = 0; i < Degree; ++i)
			{
				derivative[i] = (Degree * (values[i + 1] - values[i]));
			}
			UnitPolynomialRoots<Degree - 1> critical;
			// One coefficient sign change gives a single interior root, so no
			// derivative roots are needed to bracket it.
			if (1 < variations)
			{
				critical = BernsteinRoots(derivative);
			}
			double lower = 0.0, lowerValue = values.front();
			for (size_t i = 0; i <= critical.count; ++i)
			{
				const double upper = ((i < critical.count) ? critical.values[i] : 1.0);
				const double upperValue = EvaluateBernstein(values, upper);
				if ((upperValue == 0.0) && (upper < 1.0))
				{
					roots.values[roots.count++] = upper;
				}
				else if (((lowerValue < 0.0) && (0.0 < upperValue)) || ((upperValue < 0.0) && (0.0 < lowerValue)))
				{
					double lo = lower, hi = upper, flo = lowerValue;
					double t = ((lo + hi) * 0.5), previousStep = (hi - lo);
					for (int32 iteration = 0; iteration < 32; ++iteration)
					{
						const double f = EvaluateBernstein(values, t);
						if (f == 0.0)
						{
							break;
						}
						if ((f < 0.0) == (flo < 0.0))
						{
							lo = t;
							flo = f;
						}
						else
						{
							hi = t;
						}
						const double next = (t - f / EvaluateBernstein(derivative, t));
						if ((Abs(next - t) <= ParameterTolerance) && (lower <= next) && (next <= upper))
						{
							t = next;
							break;
						}
						const double candidate = (((lo < next) && (next < hi) && (Abs(next - t) < (previousStep * 0.5)))
							? next : ((lo + hi) * 0.5));
						previousStep = Abs(candidate - t);
						t = candidate;
						if (previousStep <= ParameterTolerance)
						{
							break;
						}
					}
					roots.values[roots.count++] = t;
				}
				lower = upper;
				lowerValue = upperValue;
			}
		}
		return roots;
	}

	// Stationary parameters of squared distance to a quadratic or cubic Bezier.
	// The product (B(t) - point).B'(t) has degree 3 or 5; the common positive
	// derivative factor is omitted. Endpoints are considered by the caller.
	template <size_t Count>
	[[nodiscard]]
	auto BezierPointStationaryParameters(const std::array<Vec2, Count>& controls, const Vec2& point) noexcept
	{
		static_assert((Count == 3) || (Count == 4));
		constexpr size_t Degree = (Count - 1);
		constexpr auto weights = []
		{
			if constexpr (Degree == 2)
			{
				return std::array{ std::array{ 1.0, (1.0 / 3) }, std::array{ (2.0 / 3), (2.0 / 3) }, std::array{ (1.0 / 3), 1.0 } };
			}
			else
			{
				return std::array{ std::array{ 1.0, 0.4, 0.1 }, std::array{ 0.6, 0.6, 0.3 },
					std::array{ 0.3, 0.6, 0.6 }, std::array{ 0.1, 0.4, 1.0 } };
			}
		}();
		std::array<double, 2 * Degree> coefficients{};
		for (size_t i = 0; i <= Degree; ++i)
		{
			for (size_t j = 0; j < Degree; ++j)
			{
				coefficients[i + j] += (weights[i][j] * (controls[i] - point).dot(controls[j + 1] - controls[j]));
			}
		}
		return BernsteinRoots(coefficients);
	}
}
