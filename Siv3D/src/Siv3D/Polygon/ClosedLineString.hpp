//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/JoinStyle.hpp>

namespace s3d
{
	// Input includes the first two points again at the end, as required by the
	// closed buffer path.
	void DrawClosedLineStringFallback(std::span<const Vec2> points, JoinStyle joinStyle, double thickness, const ColorF& color);

	void DrawClosedLineStringFallback(std::span<const Vec2> points, JoinStyle joinStyle, double thickness, const PatternParameters& pattern);

	void DrawClosedLineString(std::span<const Vec2> points, JoinStyle joinStyle, double thickness, const ColorF& color);

	void DrawClosedLineString(std::span<const Vec2> points, JoinStyle joinStyle, double thickness, const PatternParameters& pattern);
	// Unique Triangle / Quad vertices; widths refer to the original filled region.
	void DrawSmallPolygonFrame(std::span<const Vec2> points, JoinStyle joinStyle, double innerThickness, double outerThickness, const ColorF& color);

	void DrawSmallPolygonFrame(std::span<const Vec2> points, JoinStyle joinStyle, double innerThickness, double outerThickness, const PatternParameters& pattern);

	void DrawSmallPolygonFrameFallback(std::span<const Vec2> points, JoinStyle joinStyle, double innerThickness, double outerThickness, const ColorF& color);

	void DrawSmallPolygonFrameFallback(std::span<const Vec2> points, JoinStyle joinStyle, double innerThickness, double outerThickness, const PatternParameters& pattern);
}
