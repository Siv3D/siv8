//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include "BezierGeometry.hpp"

namespace s3d::detail
{
	struct BezierArrowHead
	{
		double parameter;
		Vec2 base;
		Vec2 tip;
		double sizeScale;
	};

	// With the last control at the origin, these are the interior Bernstein
	// coefficients of |B(t)|^2 (the final two coefficients are zero). If B(0)
	// is outside the circle and the coefficients minus radius^2 change sign
	// only once, variation diminution certifies a unique crossing. Ordinary
	// arrows can then skip solving for distance extrema altogether.
	template <size_t Count>
	[[nodiscard]]
	bool BezierArrowRootIsUnique(const std::array<Vec2, Count>& p, const double radiusSq) noexcept
	{
		static_assert((Count == 3) || (Count == 4));
		const auto coefficients = [&]
		{
			if constexpr (Count == 3)
				return std::array{ p[0].dot(p[1]), (2.0 / 3.0) * p[1].lengthSq() };
			else
				return std::array{ p[0].dot(p[1]), 0.4 * p[0].dot(p[2]) + 0.6 * p[1].lengthSq(),
					0.9 * p[1].dot(p[2]), 0.6 * p[2].lengthSq() };
		}();
		bool negative = false;
		for (const double coefficient : coefficients)
		{
			if (coefficient < radiusSq) negative = true;
			else if (negative && (radiusSq < coefficient)) return false;
		}
		return true;
	}

	// Geometry is independent of the rendering metric and subdivision budget.
	// Distance extrema partition the curve into at most six monotone intervals.
	// The existing Bernstein solver uses fixed storage and at most 32 iterations
	// per bracket.
	template <class Bezier>
	[[nodiscard]]
	Optional<BezierArrowHead> FindBezierArrowHead(const Bezier& curve, const Vec2 tip, const double length) noexcept
	{
		// The segment's end must be inside the requested circle. A path searches
		// backwards and stops at the first crossing, preserving this invariant.
		// Work relative to the tip, avoiding cancellation in the distance queries.
		const Bezier relative = curve.movedBy(-tip);
		const auto controls = BezierControlPoints(relative);
		const double requestedSq = (length * length);
		const auto refine = [&](double lo, double hi)
		{
			for (int32 iteration = 0; iteration < 40; ++iteration)
			{
				const double mid = ((lo + hi) * 0.5);
				if (relative.pointAt(mid).lengthSq() < requestedSq) hi = mid;
				else lo = mid;
			}
			const double t = ((lo + hi) * 0.5);
			return BezierArrowHead{ t, curve.pointAt(t), tip, 1.0 };
		};
		if ((controls.back() == Vec2{ 0, 0 }) && (requestedSq < controls.front().lengthSq())
			&& BezierArrowRootIsUnique(controls, requestedSq))
		{
			return refine(0.0, 1.0);
		}
		const auto stationary = BezierPointStationaryParameters(controls, Vec2{ 0, 0 });
		double maximumSq = controls.back().lengthSq(), maximumT = 1.0, upper = 1.0;
		for (size_t i = stationary.count + 1; i > 0; --i)
		{
			const double lower = ((i == 1) ? 0.0 : stationary.values[i - 2]);
			const double distanceSq = relative.pointAt(lower).lengthSq();
			if (maximumSq < distanceSq)
			{
				maximumSq = distanceSq;
				maximumT = lower;
			}
			if (requestedSq <= distanceSq)
			{
				if (requestedSq == distanceSq) return BezierArrowHead{ lower, curve.pointAt(lower), tip, 1.0 };
				// Search the last interval reaching the requested radius. Keeping
				// a bracket also handles stationary endpoints and near-tangent roots.
				return refine(lower, upper);
			}
			upper = lower;
		}
		if (maximumSq == 0.0) return none;
		return BezierArrowHead{ maximumT, curve.pointAt(maximumT), tip, (std::sqrt(maximumSq) / length) };
	}

	template <class Bezier>
	[[nodiscard]]
	Optional<BezierArrowHead> FindBezierArrowHead(const Bezier& curve, const double length) noexcept
	{
		return FindBezierArrowHead(curve, BezierControlPoints(curve).back(), length);
	}
}
