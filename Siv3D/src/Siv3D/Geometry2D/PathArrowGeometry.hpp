//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include "BezierArrowGeometry.hpp"

namespace s3d::detail
{
	struct PathArrowHead
	{
		size_t segment;
		double parameter;
		Vec2 base, tip;
		double sizeScale;
	};

	// Positive length and finite, representable geometry are preconditions.
	// Convexity of squared distance on a segment makes vertices sufficient for
	// both rejecting a segment and finding the maximum over the entire polyline.
	template <bool Reverse = false>
	[[nodiscard]]
	inline Optional<PathArrowHead> FindLineStringArrowHead(const std::span<const Vec2> points, const double length) noexcept
	{
		if (points.size() < 2) return none;
		const auto point = [&](const size_t i) -> const Vec2&
		{
			if constexpr (Reverse) return points[points.size() - 1 - i];
			else return points[i];
		};
		const Vec2 tip = point(points.size() - 1);
		const double requestedSq = (length * length);
		double maximumSq = 0.0, endSq = 0.0;
		size_t maximumIndex = 0;
		for (size_t i = points.size() - 1; i-- > 0;)
		{
			const double startSq = point(i).distanceFromSq(tip);
			if (requestedSq <= startSq)
			{
				if (requestedSq == startSq) return PathArrowHead{ i, 0.0, point(i), tip, 1.0 };
				const Vec2 q = (point(i + 1) - tip), d = (point(i) - point(i + 1));
				const double a = d.lengthSq(), b = q.dot(d), c = (requestedSq - endSq);
				const double root = std::sqrt(b * b + a * c);
				const double u = ((0.0 < b) ? (c / (root + b)) : ((root - b) / a));
				return PathArrowHead{ i, (1.0 - u), (point(i + 1) + d * u), tip, 1.0 };
			}
			if (maximumSq < startSq) { maximumSq = startSq; maximumIndex = i; }
			endSq = startSq;
		}
		if (maximumSq == 0.0) return none;
		return PathArrowHead{ maximumIndex, 0.0, point(maximumIndex), tip, (std::sqrt(maximumSq) / length) };
	}

	inline void WriteLineStringArrowShaft(Array<Vec2>& destination, const std::span<const Vec2> points, const PathArrowHead& head)
	{
		destination.assign(points.begin(), points.begin() + head.segment + 1);
		if (destination.back() != head.base) destination.push_back(head.base);
	}

	template <bool Reverse = false>
	[[nodiscard]]
	inline Optional<PathArrowHead> FindSplineArrowHead(const std::span<const Bezier3> curves, const double length) noexcept
	{
		if (curves.empty()) return none;
		const Vec2 tip = (Reverse ? curves.front().p0 : curves.back().p3);
		Optional<PathArrowHead> best;
		for (size_t i = curves.size(); i-- > 0;)
		{
			const auto head = [&]
			{
				if constexpr (Reverse) return FindBezierArrowHead(curves[curves.size() - 1 - i].reversed(), tip, length);
				else return FindBezierArrowHead(curves[i], tip, length);
			}();
			if (not head) continue;
			const PathArrowHead candidate{ i, head->parameter, head->base, tip, head->sizeScale };
			if (head->sizeScale == 1.0) return candidate;
			// Reverse traversal and strict improvement keep the last maximum.
			if (not best || (best->sizeScale < head->sizeScale)) best = candidate;
		}
		return best;
	}
}
