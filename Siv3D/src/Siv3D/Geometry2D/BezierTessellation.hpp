//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <Siv3D/Bezier.hpp>
# include <Siv3D/Mat3x2.hpp>
# include <array>
# include <algorithm>
# include <cassert>

// Density integral and inverse: Raph Levien, Flattening quadratic Beziers (2019).
// See docs/geometry2d/bezier-drawing.md for derivation, measured tradeoffs, and sources.
// The metric curve is an affine image of source. Output vertices remain in source coordinates.
namespace s3d::detail::BezierTessellation
{
	struct Curve
	{
		std::array<Vec2, 4> p;
		int32 degree;

		Curve(const Bezier2& q) : p{ q.p0, q.p1, q.p2, q.p2 }, degree{ 2 } {}
		Curve(const Bezier3& c) : p{ c.p0, c.p1, c.p2, c.p3 }, degree{ 3 } {}

		Bezier2 quadratic() const { return { p[0], p[1], p[2] }; }
		Bezier3 cubic() const { return { p[0], p[1], p[2], p[3] }; }
		Vec2 point(const double t) const { return degree == 2 ? quadratic().pointAt(t) : cubic().pointAt(t); }
	};

	inline Curve TransformMetric(const Curve& curve, const Mat3x2& affine, const Vec2 sceneScale)
	{
		Curve result = curve;
		for (int32 i = 0; i <= curve.degree; ++i)
		{
			const Vec2 p = curve.p[i] - curve.p[0];
			result.p[i] = Vec2{ p.x * affine._11 + p.y * affine._21,
				p.x * affine._12 + p.y * affine._22 } * sceneScale;
		}
		return result;
	}

	inline Vec2 ClosestOnSegment(const Vec2 point, const Vec2 a, const Vec2 b)
	{
		const Vec2 d = b - a;
		const double lengthSq = d.lengthSq();
		const double t = lengthSq == 0 ? 0 : Clamp((point - a).dot(d) / lengthSq, 0.0, 1.0);
		return a + t * d;
	}

	template <class Output>
	inline void PushUnique(Output& out, const Vec2 point)
	{
		if (out.isEmpty() || out.back() != point) out.push_back(point);
	}

	inline double WangCount(const Curve& curve, const double tolerance)
	{
		double differenceSq = 0;
		for (int32 i = 0; i < curve.degree - 1; ++i)
		{
			differenceSq = Max(differenceSq, (curve.p[i] - 2 * curve.p[i + 1] + curve.p[i + 2]).lengthSq());
		}
		return std::sqrt((curve.degree * (curve.degree - 1) / (8.0 * tolerance)) * std::sqrt(differenceSq));
	}

	inline bool ChordSuffices(const Curve& curve, const double tolerance)
	{
		for (int32 i = 1; i < curve.degree; ++i)
		{
			if (curve.p[i].distanceFromSq(ClosestOnSegment(curve.p[i], curve.p[0], curve.p[curve.degree])) > tolerance * tolerance) return false;
		}
		return true;
	}

	template <class Output>
	inline void Uniform(Output& out, const Curve& curve, const uint32 n)
	{
		for (uint32 i = 1; i < n; ++i) PushUnique(out, curve.point(static_cast<double>(i) / n));
		PushUnique(out, curve.p[curve.degree]);
	}

	// A continuous segment count and its normalized inverse-density map.
	struct DensityPiece
	{
		double begin, end;
		double weight;
		double integralBegin, integralDelta;
		double inverseBegin, inverseScale;
		bool uniform;
		bool collinear;

		static double Integral(const double x)
		{
			return x / (0.33 + std::sqrt(std::sqrt(0.20151121 + 0.25 * x * x)));
		}
		static double Inverse(const double x)
		{
			return x * (0.61 + std::sqrt(0.1521 + 0.25 * x * x));
		}
		double parameter(const double fraction) const
		{
			if (uniform) return begin + (end - begin) * fraction;
			const double integral = integralBegin + fraction * integralDelta;
			const double inverse = collinear ? (integral * Abs(integral)) : Inverse(integral);
			const double local = (inverse - inverseBegin) * inverseScale;
			return begin + (end - begin) * Clamp(local, 0.0, 1.0);
		}
	};

	inline DensityPiece MakeDensityPiece(const Vec2 a, const Vec2 b, const double tolerance, const double begin, const double end)
	{
		DensityPiece piece{};
		piece.uniform = true;
		piece.begin = begin;
		piece.end = end;
		const Vec2 difference = a - b, chord = a + b;
		const double cross = chord.cross(difference);
		const double dd = difference.lengthSq();
		// Nearly constant derivatives use a parametric bound.
		// These are geometric limits for finite curves, not input sanitization.
		if (dd <= 1e-12 * (a.lengthSq() + b.lengthSq()))
		{
			piece.weight = std::sqrt(std::sqrt(dd) / (4 * tolerance));
			return piece;
		}
		if (cross * cross <= 1e-12 * dd * chord.lengthSq())
		{
			// The cross -> 0 limit of the cusp branch below. Cancelling cross
			// before evaluation avoids a discontinuous fallback to Wang's count.
			// Integral coordinates are proportional to sign(A) * sqrt(abs(A));
			// their inverse is A, up to a common scale that cancels in parameter().
			const double aa = a.dot(difference), bb = b.dot(difference);
			const double start = std::copysign(std::sqrt(Abs(aa)), aa);
			const double finish = std::copysign(std::sqrt(Abs(bb)), bb);
			piece.weight = ((aa < 0) == (bb < 0)) ? 0
				: Abs(finish - start) / (2 * std::sqrt(std::sqrt(tolerance * dd * std::sqrt(dd))));
			piece.integralBegin = start;
			piece.integralDelta = finish - start;
			piece.inverseBegin = aa;
			piece.inverseScale = 1 / (bb - aa);
			piece.uniform = false;
			piece.collinear = true;
			return piece;
		}
		const double x0 = a.dot(difference) / cross, x1 = b.dot(difference) / cross;
		const double scale = Abs(cross / (std::sqrt(dd) * (x1 - x0)));
		const double sqrtScale = std::sqrt(scale), sqrtTolerance = std::sqrt(tolerance);
		piece.integralBegin = DensityPiece::Integral(x0);
		piece.integralDelta = DensityPiece::Integral(x1) - piece.integralBegin;
		const double span = Abs(piece.integralDelta);
		piece.weight = ((x0 < 0) == (x1 < 0))
			? (span * sqrtScale / (2 * sqrtTolerance))
			: (span / (2 * DensityPiece::Integral(sqrtTolerance / sqrtScale)));
		piece.inverseBegin = DensityPiece::Inverse(piece.integralBegin);
		piece.inverseScale = 1 / (DensityPiece::Inverse(piece.integralBegin + piece.integralDelta) - piece.inverseBegin);
		piece.uniform = false;
		return piece;
	}

	struct Workspace
	{
		// Caps both approximation work and storage independently of output count.
		std::array<DensityPiece, 64> pieces;
		uint32 pieceCount = 0;
		bool limited = false;
	};

	inline void AddUnitRoots(const double aa, const double bb, const double cc,
		std::array<double, 4>& boundaries, uint32& count)
	{
		const auto add = [&](const double t)
		{
			if (0 < t && t < 1) boundaries[count++] = t;
		};
		if (aa == 0)
		{
			if (bb != 0) add(-cc / bb);
		}
		else
		{
			const double discriminant = bb * bb - 4 * aa * cc;
			if (discriminant == 0) add(-bb / (2 * aa));
			else if (0 < discriminant)
			{
				const double q = -0.5 * (bb + std::copysign(std::sqrt(discriminant), bb));
				add(q / aa);
				add(cc / q);
			}
		}
	}

	inline uint32 InflectionBoundaries(const Bezier3& c, std::array<double, 4>& boundaries)
	{
		boundaries[0] = 0;
		uint32 count = 1;
		const Vec2 a = -c.p0 + 3 * c.p1 - 3 * c.p2 + c.p3;
		const Vec2 b = c.p0 - 2 * c.p1 + c.p2, d = c.p1 - c.p0;
		AddUnitRoots(a.cross(b), a.cross(d), b.cross(d), boundaries, count);
		boundaries[count++] = 1;
		std::sort(boundaries.begin(), boundaries.begin() + count);
		return count;
	}

	inline uint32 ProjectionExtrema(const Curve& metric, std::array<double, 4>& parameters)
	{
		Vec2 axis = metric.p[3] - metric.p[0];
		if (axis.lengthSq() == 0)
		{
			axis = metric.p[1] - metric.p[0];
			if (axis.lengthSq() == 0) axis = metric.p[2] - metric.p[0];
		}
		const double d0 = (metric.p[1] - metric.p[0]).dot(axis);
		const double d1 = (metric.p[2] - metric.p[1]).dot(axis);
		const double d2 = (metric.p[3] - metric.p[2]).dot(axis);
		uint32 count = 0;
		AddUnitRoots(d0 - 2 * d1 + d2, 2 * (d1 - d0), d0, parameters, count);
		std::sort(parameters.begin(), parameters.begin() + count);
		return count;
	}

	// Grow geometrically when appending many curves; exact reserves per interval
	// would repeatedly copy the whole path. Existing capacity is retained.
	template <class Output>
	inline void ReserveAdditional(Output& out, const uint32 count)
	{
		const size_t needed = out.size() + count;
		if (out.capacity() < needed) out.reserve(Max(needed, out.capacity() * 2));
	}

	template <class Output>
	inline void Append(Output& out, Workspace& work, const Curve& source, const Curve& curve,
		const double tolerance, const uint32 maxSegments)
	{
		assert(0 < tolerance);
		assert(1 <= maxSegments && maxSegments <= 4096);
		work.pieceCount = 0;
		work.limited = false;
		const uint32 startCount = (out.isEmpty() || out.back() != source.p[0]) ? 1 : 0;
		if (ChordSuffices(curve, tolerance))
		{
			ReserveAdditional(out, startCount + (source.p[0] != source.p[source.degree]));
			PushUnique(out, source.p[0]);
			PushUnique(out, source.p[source.degree]);
			return;
		}
		// Density can vanish along a straight interval leading into a reversal.
		// Its endpoint is then far from the turn despite a tiny density distance.
		// Include projection extrema explicitly; at most two extra cubic vertices.
		std::array<double, 4> extrema;
		const uint32 extremaCount = curve.degree == 3 ? ProjectionExtrema(curve, extrema) : 0;
		if (maxSegments <= extremaCount)
		{
			work.limited = true;
			ReserveAdditional(out, maxSegments + startCount);
			PushUnique(out, source.p[0]);
			Uniform(out, source, maxSegments);
			return;
		}
		if (curve.degree == 2)
		{
			work.pieces[0] = MakeDensityPiece(curve.p[1] - curve.p[0], curve.p[2] - curve.p[1], tolerance, 0, 1);
			work.pieceCount = 1;
		}
		else
		{
			// A quadratic matching both endpoints: |cubic - quadratic| <= |third difference| / sqrt(432).
			// Reserve 10% of the tolerance for this intermediate approximation.
			const Vec2 third = curve.p[3] - 3 * curve.p[2] + 3 * curve.p[1] - curve.p[0];
			const double rate = std::cbrt(third.length() / (std::sqrt(432.0) * tolerance * 0.1));
			const Bezier3 c = curve.cubic();
			std::array<double, 4> boundaries;
			const uint32 boundaryCount = InflectionBoundaries(c, boundaries);
			// Do not allow a quadratic piece to straddle an inflection and become
			// spuriously straight: a zero-density interval makes inverse placement jump.
			for (uint32 part = 1; part < boundaryCount; ++part)
			{
				const double begin = boundaries[part - 1], end = boundaries[part];
				const double needed = Max(1.0, std::ceil(rate * (end - begin)));
				if (work.pieces.size() < work.pieceCount + needed)
				{
					const double needed = Max(1.0, std::ceil(WangCount(curve, tolerance)));
					const auto n = static_cast<uint32>(Min(needed, static_cast<double>(maxSegments)));
					work.limited = maxSegments < needed;
					ReserveAdditional(out, n + startCount);
					PushUnique(out, source.p[0]);
					Uniform(out, source, n);
					return;
				}
				const uint32 count = static_cast<uint32>(needed);
				const double step = (end - begin) / count;
				const Vec2 correction = (0.25 * step * step * step) * third;
				Vec2 derivative = c.derivativeAt(begin);
				for (uint32 i = 0; i < count; ++i)
				{
					const double a = begin + i * step, b = begin + (i + 1) * step;
					const Vec2 nextDerivative = c.derivativeAt(b);
					// The endpoint-matching quadratic has legs h/2 * B'(a or b) - h^3/4 * third.
					// Compute these directly, and share each derivative with the next interval.
					work.pieces[work.pieceCount++] = MakeDensityPiece((0.5 * step) * derivative - correction,
						(0.5 * step) * nextDerivative - correction, tolerance * 0.9, a, b);
					derivative = nextDerivative;
				}
			}
		}
		double total = 0;
		for (uint32 i = 0; i < work.pieceCount; ++i) total += work.pieces[i].weight;
		const double needed = Max(1.0, std::ceil(total));
		work.limited = maxSegments < needed + extremaCount;
		const uint32 n = static_cast<uint32>(Min(needed, static_cast<double>(maxSegments - extremaCount)));
		ReserveAdditional(out, n + extremaCount + startCount);
		PushUnique(out, source.p[0]);
		uint32 pieceIndex = 0, extremumIndex = 0;
		double prefix = 0;
		for (uint32 i = 1; i < n; ++i)
		{
			const double target = total * i / n;
			while (pieceIndex + 1 < work.pieceCount && prefix + work.pieces[pieceIndex].weight < target)
			{
				prefix += work.pieces[pieceIndex++].weight;
			}
			const auto& piece = work.pieces[pieceIndex];
			const double fraction = piece.weight == 0 ? 0 : (target - prefix) / piece.weight;
			// Keep output vertices on the original curve. Error is measured against it,
			// including intervals that cross intermediate quadratic boundaries.
			const double t = piece.parameter(fraction);
			while (extremumIndex < extremaCount && extrema[extremumIndex] < t)
			{
				PushUnique(out, source.point(extrema[extremumIndex++]));
			}
			PushUnique(out, source.point(t));
		}
		while (extremumIndex < extremaCount) PushUnique(out, source.point(extrema[extremumIndex++]));
		PushUnique(out, source.p[source.degree]);
	}

	template <class Output>
	inline void Flatten(Output& out, Workspace& work, const Curve& source, const Curve& metric,
		const double tolerance, const uint32 maxSegments)
	{
		out.clear();
		Append(out, work, source, metric, tolerance, maxSegments);
	}

	// Shared path composition for geometry conversion and renderer-space quality.
	// MetricFor is evaluated once per curve and does not own output storage.
	template <class Output, class MetricFor>
	inline void FlattenPath(Output& out, Workspace& work, const std::span<const Bezier3> curves,
		const bool closed, const double tolerance, const uint32 maxSegmentsPerCurve, MetricFor&& metricFor)
	{
		out.clear();
		for (const Bezier3& curve : curves)
		{
			const Curve source{ curve };
			Append(out, work, source, metricFor(source), tolerance, maxSegmentsPerCurve);
		}
		if (closed && (1 < out.size()) && (out.front() == out.back())) out.pop_back();
	}
}
