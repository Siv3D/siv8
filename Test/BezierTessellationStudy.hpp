//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <Siv3D.hpp>
# include <array>

// Experimental comparison only. No production caller uses these algorithms.
// Wang: n = ceil(sqrt(degree * (degree - 1) * max|second difference| / (8 * tolerance))).
// Density: Raph Levien, "Flattening quadratic Beziers" (2019), with a
// cubic-to-quadratic error budget and one count across all quadratic pieces.
// The equations are implemented here independently; see the study for sources,
// measurement boundaries, geometric fallbacks, and approximation limitations.
namespace s3d::BezierStudy
{
	enum class Method { Current, CurrentDeep, Wang, Density };
	inline constexpr std::array Methods{ Method::Current, Method::CurrentDeep, Method::Wang, Method::Density };
	inline constexpr std::array Names{ "current", "current-deep", "wang", "density" };

	struct Curve
	{
		std::array<Vec2, 4> p;
		int32 degree;

		Curve(const Bezier2& q) : p{ q.p0, q.p1, q.p2, q.p2 }, degree{ 2 } {}
		Curve(const Bezier3& c) : p{ c.p0, c.p1, c.p2, c.p3 }, degree{ 3 } {}

		Bezier2 quadratic() const { return { p[0], p[1], p[2] }; }
		Bezier3 cubic() const { return { p[0], p[1], p[2], p[3] }; }
		Vec2 point(const double t) const { return degree == 2 ? quadratic().pointAt(t) : cubic().pointAt(t); }
		Vec2 closest(const Vec2 v) const { return degree == 2 ? quadratic().computeClosestPoint(v) : cubic().computeClosestPoint(v); }
		Curve transformed(const double sx, const double sy, const Vec2 offset = { 0, 0 }) const
		{
			Curve result = *this;
			for (auto& v : result.p) v = v * Vec2{ sx, sy } + offset;
			return result;
		}
	};

	struct Case
	{
		const char* name;
		Curve curve;
		bool ordinary;
	};

	inline Array<Case> Cases()
	{
		return {
			{ "q-shallow", Bezier2{ { 0, 0 }, { 300, 16 }, { 600, 0 } }, true },
			{ "q-arch", Bezier2{ { 0, 0 }, { 300, 200 }, { 600, 0 } }, true },
			{ "q-asymmetric", Bezier2{ { 0, 0 }, { 25, 280 }, { 600, 30 } }, true },
			{ "q-tight", Bezier2{ { 0, 0 }, { -100, 300 }, { 60, 0 } }, true },
			{ "q-small", Bezier2{ { 0, 0 }, { 10, 20 }, { 40, 0 } }, true },
			{ "q-straight", Bezier2{ { 0, 0 }, { 0, 0 }, { 600, 0 } }, true },
			{ "q-reverse", Bezier2{ { 0, 0 }, { 600, 0 }, { 40, 0 } }, false },
			{ "q-near-reverse", Bezier2{ { 0, 0 }, { 600, 0.01 }, { 40, 0 } }, false },
			{ "q-return", Bezier2{ { 0, 0 }, { 300, 200 }, { 0, 0 } }, false },
			{ "q-point", Bezier2{ { 3, 7 }, { 3, 7 }, { 3, 7 } }, false },
			{ "c-arch", Bezier3{ { 0, 0 }, { 0, 400 }, { 600, 400 }, { 600, 0 } }, true },
			{ "c-s", Bezier3{ { 0, 0 }, { 200, 400 }, { 400, -400 }, { 600, 0 } }, true },
			{ "c-asymmetric", Bezier3{ { 0, 0 }, { -100, 400 }, { 150, 0 }, { 600, 0 } }, true },
			{ "c-quarter", Bezier3{ { 0, 200 }, { 0, 89.54305 }, { 89.54305, 0 }, { 200, 0 } }, true },
			{ "c-small", Bezier3{ { 0, 0 }, { 10, 30 }, { 20, -20 }, { 40, 0 } }, true },
			{ "c-straight", Bezier3{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 600, 0 } }, true },
			{ "c-loop", Bezier3{ { 0, 0 }, { 400, 400 }, { -400, 400 }, { 0, 0 } }, false },
			{ "c-near-loop", Bezier3{ { 0, 0 }, { 400, 400 }, { -400, 400 }, { 0.01, 0 } }, false },
			{ "c-cusp", Bezier3{ { 0, 0 }, { 200, 200 }, { -200, 200 }, { 400, 0 } }, false },
			{ "c-reverse", Bezier3{ { 0, 0 }, { 400, 0 }, { -400, 0 }, { 0, 0 } }, false },
			{ "c-end-stationary", Bezier3{ { 0, 0 }, { 0, 0 }, { 0, 400 }, { 600, 0 } }, false },
			{ "c-point", Bezier3{ { 3, 7 }, { 3, 7 }, { 3, 7 }, { 3, 7 } }, false },
			{ "c-large", Bezier3{ { 0, 0 }, { 0, 4000 }, { 6000, 4000 }, { 6000, 0 } }, false },
		};
	}

	inline Vec2 ClosestOnSegment(const Vec2 point, const Vec2 a, const Vec2 b)
	{
		const Vec2 d = b - a;
		const double lengthSq = d.lengthSq();
		const double t = lengthSq == 0 ? 0 : Clamp((point - a).dot(d) / lengthSq, 0.0, 1.0);
		return a + t * d;
	}

	inline Vec2 ClosestOnPolyline(const LineString& line, const Vec2 point)
	{
		Vec2 result = line.front();
		double distanceSq = point.distanceFromSq(result);
		for (size_t i = 1; i < line.size(); ++i)
		{
			const Vec2 v = ClosestOnSegment(point, line[i - 1], line[i]);
			if (const double d = point.distanceFromSq(v); d < distanceSq)
			{
				distanceSq = d;
				result = v;
			}
		}
		return result;
	}

	inline void PushUnique(LineString& out, const Vec2 point)
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

	inline void Uniform(LineString& out, const Curve& curve, const uint32 n)
	{
		out.reserve(n + 1);
		for (uint32 i = 1; i < n; ++i) PushUnique(out, curve.point(static_cast<double>(i) / n));
		PushUnique(out, curve.p[curve.degree]);
	}

	// A continuous segment count and its normalized inverse-density map.
	struct DensityPiece
	{
		double begin = 0, end = 1;
		double weight = 0;
		double integralBegin = 0, integralDelta = 0;
		double inverseBegin = 0, inverseScale = 0;
		bool uniform = true;
		bool collinear = false;

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
			const double integral = integralBegin + fraction * integralDelta;
			const double inverse = collinear ? (integral * Abs(integral)) : Inverse(integral);
			const double local = uniform ? fraction : ((inverse - inverseBegin) * inverseScale);
			return begin + (end - begin) * Clamp(local, 0.0, 1.0);
		}
	};

	inline DensityPiece MakeDensityPiece(const Bezier2& q, const double tolerance, const double begin, const double end)
	{
		DensityPiece piece;
		piece.begin = begin;
		piece.end = end;
		const Vec2 a = q.p1 - q.p0, b = q.p2 - q.p1, difference = a - b;
		const double cross = (q.p2 - q.p0).cross(difference);
		const double dd = difference.lengthSq();
		// Nearly constant derivatives use a parametric bound.
		// These are geometric limits for finite curves, not input sanitization.
		if (dd <= 1e-12 * (a.lengthSq() + b.lengthSq()))
		{
			piece.weight = WangCount(Curve{ q }, tolerance);
			return piece;
		}
		if (cross * cross <= 1e-12 * dd * (q.p2 - q.p0).lengthSq())
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

	inline uint32 InflectionBoundaries(const Bezier3& c, std::array<double, 4>& boundaries)
	{
		boundaries[0] = 0;
		uint32 count = 1;
		const Vec2 a = -c.p0 + 3 * c.p1 - 3 * c.p2 + c.p3;
		const Vec2 b = c.p0 - 2 * c.p1 + c.p2, d = c.p1 - c.p0;
		const double aa = a.cross(b), bb = a.cross(d), cc = b.cross(d);
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
		boundaries[count++] = 1;
		std::sort(boundaries.begin(), boundaries.begin() + count);
		return count;
	}

	inline void Flatten(LineString& out, Workspace& work, const Curve& curve, const Method method,
		const double tolerance = 0.5, const uint32 maxSegments = 256)
	{
		work.pieceCount = 0;
		work.limited = false;
		if (method == Method::Current || method == Method::CurrentDeep)
		{
			const int32 depth = method == Method::Current ? 5 : 12;
			out = curve.degree == 2 ? curve.quadratic().toLineStringAdaptive(tolerance, depth) : curve.cubic().toLineStringAdaptive(tolerance, depth);
			return;
		}
		out.clear();
		if (ChordSuffices(curve, tolerance))
		{
			out.reserve(2);
			out.push_back(curve.p[0]);
			PushUnique(out, curve.p[curve.degree]);
			return;
		}
		if (method == Method::Wang)
		{
			const double needed = Max(1.0, std::ceil(WangCount(curve, tolerance)));
			work.limited = maxSegments < needed;
			out.reserve(static_cast<uint32>(Min(needed, static_cast<double>(maxSegments))) + 1);
			out.push_back(curve.p[0]);
			Uniform(out, curve, static_cast<uint32>(Min(needed, static_cast<double>(maxSegments))));
			return;
		}
		if (curve.degree == 2)
		{
			work.pieces[0] = MakeDensityPiece(curve.quadratic(), tolerance, 0, 1);
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
					Flatten(out, work, curve, Method::Wang, tolerance, maxSegments);
					return;
				}
				const uint32 count = static_cast<uint32>(needed);
				const double step = (end - begin) / count;
				for (uint32 i = 0; i < count; ++i)
				{
					const double a = begin + i * step, b = begin + (i + 1) * step;
					const Vec2 start = c.pointAt(a), finish = c.pointAt(b);
					const Vec2 control = 0.5 * (start + finish) + (0.25 * step) * (c.derivativeAt(a) - c.derivativeAt(b));
					work.pieces[work.pieceCount++] = MakeDensityPiece({ start, control, finish }, tolerance * 0.9, a, b);
				}
			}
		}
		double total = 0;
		for (uint32 i = 0; i < work.pieceCount; ++i) total += work.pieces[i].weight;
		const double needed = Max(1.0, std::ceil(total));
		work.limited = maxSegments < needed;
		const uint32 n = static_cast<uint32>(Min(needed, static_cast<double>(maxSegments)));
		out.reserve(n + 1);
		out.push_back(curve.p[0]);
		uint32 pieceIndex = 0;
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
			PushUnique(out, curve.point(piece.parameter(fraction)));
		}
		PushUnique(out, curve.p[curve.degree]);
	}

	inline double SampledError(const Curve& curve, const LineString& line, const int32 samples = 4096)
	{
		double errorSq = 0;
		for (int32 i = 0; i <= samples; ++i)
		{
			const Vec2 point = curve.point(static_cast<double>(i) / samples);
			errorSq = Max(errorSq, point.distanceFromSq(ClosestOnPolyline(line, point)));
		}
		for (size_t i = 1; i < line.size(); ++i)
		{
			for (int32 j = 1; j <= 7; ++j)
			{
				const Vec2 point = line[i - 1].lerp(line[i], j / 8.0);
				errorSq = Max(errorSq, point.distanceFromSq(curve.closest(point)));
			}
		}
		return std::sqrt(errorSq);
	}
}
