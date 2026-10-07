//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <Siv3D/Bezier.hpp>
# include <Siv3D/LineString.hpp>

namespace s3d::detail
{
	inline double Bezier3ControlDistanceSq(const Vec2 point, const Vec2 start, const Vec2 chord, const double chordSq) noexcept
	{
		const double t = ((chordSq == 0.0) ? 0.0 : Clamp((point - start).dot(chord) / chordSq, 0.0, 1.0));
		return point.distanceFromSq(start + t * chord);
	}

	// Append endpoints without allocating a temporary polyline for each curve.
	inline void AppendBezier3Polyline(LineString& destination, const Bezier3& curve, const double errorSq, const int32 depth)
	{
		const Vec2 chord = (curve.p3 - curve.p0);
		const double chordSq = chord.lengthSq();
		if ((depth == 0)
			|| ((Bezier3ControlDistanceSq(curve.p1, curve.p0, chord, chordSq) <= errorSq)
				&& (Bezier3ControlDistanceSq(curve.p2, curve.p0, chord, chordSq) <= errorSq)))
		{
			if (destination.back() != curve.p3)
			{
				destination.push_back(curve.p3);
			}
			return;
		}
		const auto [left, right] = curve.split(0.5);
		AppendBezier3Polyline(destination, left, errorSq, (depth - 1));
		AppendBezier3Polyline(destination, right, errorSq, (depth - 1));
	}
}
