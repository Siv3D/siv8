//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <array>
# include <Siv3D/Array.hpp>
# include <Siv3D/PointVector.hpp>

namespace s3d::detail::SuperEllipseTessellation
{
	inline constexpr double TargetError = 0.25;
	inline constexpr size_t MaxOctantSegments = 32;
	inline constexpr size_t MaxPoints = (MaxOctantSegments * 8);

	struct Quarter
	{
		std::array<Vec2, (MaxOctantSegments * 2 + 1)> points;
		size_t count;
		bool limited;
	};

	// Positive axes and exponent. The scalar metric matches other rounded
	// primitives; the stroke margin refines the centerline for wide frames.
	// Source coordinates are never multiplied by the raster scale.
	void Tessellate(Quarter& output, double a, double b, double n, double scale, double strokeMargin = 0.0);

	template <class Visitor>
	void VisitContour(const Quarter& quarter, Visitor&& visitor)
	{
		const size_t segments = (quarter.count - 1);
		for (size_t i = 0; i < segments; ++i) visitor(quarter.points[i]);
		for (size_t i = segments; i > 0; --i) visitor(Vec2{ -quarter.points[i].x, quarter.points[i].y });
		for (size_t i = 0; i < segments; ++i) visitor(-quarter.points[i]);
		for (size_t i = segments; i > 0; --i) visitor(Vec2{ quarter.points[i].x, -quarter.points[i].y });
	}

	void WriteContour(Array<Vec2>& destination, const Quarter& quarter, Vec2 center);
}
