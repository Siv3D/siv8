//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include "PathArrowGeometry.hpp"

namespace s3d::detail
{
	template <class Head>
	struct DoubleArrowHeads
	{
		Head start, end;
	};

	// Reversed roots leave a common interval inside both requested head circles.
	// Balance the two chord lengths when that interval brackets the endpoint
	// bisector. Fixed bisection also handles unevenly parameterized straight paths.
	template <class PointAt>
	[[nodiscard]]
	double SharedArrowBase(double lo, double hi, const Vec2 start, const Vec2 end, PointAt&& pointAt)
	{
		const Vec2 direction = (end - start);
		const double halfDistanceSq = (direction.lengthSq() * 0.5);
		const auto side = [&](const double t) { return (pointAt(t) - start).dot(direction) - halfDistanceSq; };
		const double a = side(lo), b = side(hi);
		if (((a < 0.0) && (0.0 < b)) || ((b < 0.0) && (0.0 < a)))
		{
			for (int32 i = 0; i < 40; ++i)
			{
				const double mid = ((lo + hi) * 0.5);
				if ((side(mid) < 0.0) == (a < 0.0)) lo = mid;
				else hi = mid;
			}
		}
		return ((lo + hi) * 0.5);
	}

	template <class Bezier>
	[[nodiscard]]
	Optional<DoubleArrowHeads<BezierArrowHead>> FindBezierDoubleArrowHeads(const Bezier& curve, const double length)
	{
		const auto reverse = FindBezierArrowHead(curve.reversed(), length);
		const auto end = FindBezierArrowHead(curve, length);
		if (not reverse || not end) return none;
		DoubleArrowHeads<BezierArrowHead> result{ *reverse, *end };
		result.start.parameter = (1.0 - reverse->parameter);
		if (result.end.parameter <= result.start.parameter)
		{
			const double t = SharedArrowBase(result.end.parameter, result.start.parameter,
				result.start.tip, result.end.tip, [&](double u) { return curve.pointAt(u); });
			const Vec2 base = curve.pointAt(t);
			result.start = { t, base, result.start.tip, base.distanceFrom(result.start.tip) / length };
			result.end = { t, base, result.end.tip, base.distanceFrom(result.end.tip) / length };
		}
		return result;
	}

	template <class PointAt>
	void FitDoublePathArrowHeads(DoubleArrowHeads<PathArrowHead>& heads, const size_t segments,
		const double length, PointAt&& pointAt)
	{
		const double from = (static_cast<double>(heads.start.segment) + heads.start.parameter);
		const double to = (static_cast<double>(heads.end.segment) + heads.end.parameter);
		if (from < to) return;
		const double t = SharedArrowBase(to, from, heads.start.tip, heads.end.tip, pointAt);
		const size_t segment = Min(static_cast<size_t>(t), segments - 1);
		const Vec2 base = pointAt(t);
		heads.start = { segment, t - segment, base, heads.start.tip, base.distanceFrom(heads.start.tip) / length };
		heads.end = { segment, t - segment, base, heads.end.tip, base.distanceFrom(heads.end.tip) / length };
	}

	[[nodiscard]]
	inline Optional<DoubleArrowHeads<PathArrowHead>> FindLineStringDoubleArrowHeads(const std::span<const Vec2> points, const double length)
	{
		const auto reverse = FindLineStringArrowHead<true>(points, length);
		const auto end = FindLineStringArrowHead(points, length);
		if (not reverse || not end) return none;
		DoubleArrowHeads<PathArrowHead> result{ *reverse, *end };
		result.start.segment = (points.size() - 2 - reverse->segment);
		result.start.parameter = (1.0 - reverse->parameter);
		FitDoublePathArrowHeads(result, points.size() - 1, length, [&](const double t)
		{
			const size_t i = Min(static_cast<size_t>(t), points.size() - 2);
			return points[i].lerp(points[i + 1], t - i);
		});
		return result;
	}

	inline void WriteLineStringDoubleArrowShaft(Array<Vec2>& destination, const std::span<const Vec2> points,
		const DoubleArrowHeads<PathArrowHead>& heads)
	{
		destination.clear();
		destination.push_back(heads.start.base);
		for (size_t i = heads.start.segment + 1; i <= heads.end.segment; ++i)
		{
			if (destination.back() != points[i]) destination.push_back(points[i]);
		}
		if (destination.back() != heads.end.base) destination.push_back(heads.end.base);
	}

	[[nodiscard]]
	inline Optional<DoubleArrowHeads<PathArrowHead>> FindSplineDoubleArrowHeads(const std::span<const Bezier3> curves, const double length)
	{
		const auto reverse = FindSplineArrowHead<true>(curves, length);
		const auto end = FindSplineArrowHead(curves, length);
		if (not reverse || not end) return none;
		DoubleArrowHeads<PathArrowHead> result{ *reverse, *end };
		result.start.segment = (curves.size() - 1 - reverse->segment);
		result.start.parameter = (1.0 - reverse->parameter);
		FitDoublePathArrowHeads(result, curves.size(), length, [&](const double t)
		{
			const size_t i = Min(static_cast<size_t>(t), curves.size() - 1);
			return curves[i].pointAt(t - i);
		});
		return result;
	}
}
