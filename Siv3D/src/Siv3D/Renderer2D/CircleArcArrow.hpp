//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <Siv3D/Circle.hpp>
# include <Siv3D/FastMath.hpp>
# include "../Geometry2D/CircleArcArrowGeometry.hpp"
# include "Vertex2DBuilder.hpp"

namespace s3d::detail::CircleArcArrow
{
	// Circle's bounded angular density; ownership stays with the renderer.
	// The head and endpoints keep double-precision placement independently of
	// the float trigonometry used by ordinary circle drawing for interior points.
	inline void WritePoints(Array<Vec2>& destination, const Placement& placement,
		const float thickness, const float scale, const bool fullArc = false)
	{
		const double sweep = (fullArc ? placement.sweep : placement.shaftSweep);
		if (sweep == 0.0)
		{
			destination.resize(1);
			destination.front() = placement.start;
			return;
		}
		const uint16 count = Vertex2DBuilder::CalculateCircleArcQuality(
			static_cast<float>(placement.radius + thickness * 0.5) * scale, static_cast<float>(Abs(sweep)));
		destination.resize(count);
		destination.front() = placement.start;
		const double step = (sweep / (count - 1));
		for (uint16 i = 1; i < (count - 1); ++i)
		{
			const auto [s, c] = FastMath::SinCos(placement.startAngle + step * i);
			destination[i] = placement.center + Vec2{ s * placement.radius, -c * placement.radius };
		}
		destination.back() = (fullArc ? placement.tip : placement.base);
	}

	inline void WriteDoublePoints(Array<Vec2>& destination, const DoublePlacement& placement, const float thickness, const float scale)
	{
		Placement shaft = placement.arc;
		shaft.start = placement.startBase;
		shaft.base = placement.endBase;
		shaft.startAngle += placement.headSweep;
		shaft.shaftSweep = (shaft.sweep - 2 * placement.headSweep);
		WritePoints(destination, shaft, thickness, scale);
	}

}
