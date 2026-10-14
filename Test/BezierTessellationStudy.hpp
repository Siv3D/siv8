//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <Siv3D.hpp>
# include <array>
# include "../Siv3D/src/Siv3D/Geometry2D/BezierTessellation.hpp"

// Comparison baselines plus the production density kernel.
// Wang: n = ceil(sqrt(degree * (degree - 1) * max|second difference| / (8 * tolerance))).
// Density: Raph Levien, "Flattening quadratic Beziers" (2019), with a
// cubic-to-quadratic error budget and one count across all quadratic pieces.
// Density delegates to the production kernel; Current retains the legacy conversion.
// See the drawing guide for sources, measurement boundaries, and limitations.
namespace s3d::BezierStudy
{
	enum class Method { Current, CurrentDeep, Wang, Density };
	inline constexpr std::array Methods{ Method::Current, Method::CurrentDeep, Method::Wang, Method::Density };
	inline constexpr std::array Names{ "current", "current-deep", "wang", "density" };

	struct Curve : detail::BezierTessellation::Curve
	{
		using detail::BezierTessellation::Curve::Curve;
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

	using Workspace = detail::BezierTessellation::Workspace;

	inline void Flatten(LineString& out, Workspace& work, const Curve& curve, const Method method,
		const double tolerance = 0.5, const uint32 maxSegments = 256)
	{
		using namespace detail::BezierTessellation;
		work.pieceCount = 0;
		work.limited = false;
		if (method == Method::Current || method == Method::CurrentDeep)
		{
			const int32 depth = method == Method::Current ? 5 : 12;
			out = curve.degree == 2 ? curve.quadratic().toLineStringAdaptive(tolerance, depth) : curve.cubic().toLineStringAdaptive(tolerance, depth);
			return;
		}
		if (method == Method::Density)
		{
			detail::BezierTessellation::Flatten(out, work, curve, curve, tolerance, maxSegments);
			return;
		}
		const double needed = ChordSuffices(curve, tolerance) ? 1 : Max(1.0, std::ceil(WangCount(curve, tolerance)));
		work.limited = maxSegments < needed;
		const auto count = static_cast<uint32>(Min(needed, static_cast<double>(maxSegments)));
		out.clear();
		out.reserve(count + 1);
		out.push_back(curve.p[0]);
		Uniform(out, curve, count);
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
