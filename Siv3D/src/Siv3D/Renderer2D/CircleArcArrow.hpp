//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <Siv3D/Circle.hpp>
# include <Siv3D/FastMath.hpp>
# include <Siv3D/Optional.hpp>
# include "Vertex2DBuilder.hpp"

namespace s3d::detail::CircleArcArrow
{
	struct Placement
	{
		Vec2 center, start, base, tip;
		double radius, startAngle, sweep, shaftSweep, sizeScale;
	};

	// Positive head length and finite, representable geometry are preconditions.
	// The last point reaching the requested chord distance is at most pi behind
	// the tip, even when the source arc extends beyond a semicircle.
	[[nodiscard]]
	inline Optional<Placement> Place(const Circle& circle, const double startAngle, const double angle, const double headLength) noexcept
	{
		const double radius = Abs(circle.r);
		const double sweep = Clamp(angle, -Math::TwoPi, Math::TwoPi);
		const double magnitude = Abs(sweep);
		if ((radius == 0.0) || (magnitude == 0.0)) return none;
		const double maximumAngle = Min(magnitude, Math::Pi);
		const double maximumLength = ((magnitude < Math::Pi) ? (2 * radius * std::sin(magnitude * 0.5)) : (2 * radius));
		const bool shrink = (maximumLength <= headLength);
		const double headAngle = (shrink ? maximumAngle : Min(2 * std::asin(headLength / (2 * radius)), magnitude));
		const double shaftSweep = std::copysign((magnitude - headAngle), sweep);
		const auto pointAt = [&](const double a)
		{
			return circle.center + Vec2{ std::sin(a) * radius, -std::cos(a) * radius };
		};
		const Vec2 start = pointAt(startAngle);
		// Preserve exact endpoint identity for both directions and clamped sweeps.
		const Vec2 tip = ((magnitude == Math::TwoPi) ? start : pointAt(startAngle + sweep));
		const Vec2 base = ((shaftSweep == 0.0) ? start : pointAt(startAngle + shaftSweep));
		return Placement{ circle.center, start, base, tip, radius, startAngle, sweep, shaftSweep,
			(shrink ? (maximumLength / headLength) : 1.0) };
	}

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

	struct DoublePlacement
	{
		Placement arc;
		Vec2 startBase, endBase;
		double headSweep, sizeScale;
	};

	[[nodiscard]]
	inline Optional<DoublePlacement> PlaceDouble(const Circle& circle, const double startAngle, const double angle, const double headLength) noexcept
	{
		const auto arc = Place(circle, startAngle, angle, headLength);
		if (not arc) return none;
		const double headAngle = Min(Abs(arc->sweep - arc->shaftSweep), Abs(arc->sweep) * 0.5);
		const double headSweep = std::copysign(headAngle, arc->sweep);
		const auto pointAt = [&](const double a)
		{
			return circle.center + Vec2{ std::sin(a) * arc->radius, -std::cos(a) * arc->radius };
		};
		const Vec2 startBase = pointAt(startAngle + headSweep);
		const Vec2 endBase = ((headAngle == Abs(arc->sweep) * 0.5) ? startBase : arc->base);
		return DoublePlacement{ *arc, startBase, endBase, headSweep,
			Min(1.0, (2 * arc->radius * std::sin(headAngle * 0.5)) / headLength) };
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
