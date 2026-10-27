// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.

# pragma once
# include <Siv3D/ImageDraw.hpp>
# include <array>

namespace s3d::detail::ImageShape
{
	void Rounded(Image&, const RectF&, std::array<double, 4> radii, Color, ImagePixel::BlendMode, EnableAntialiasing);
	void RoundedFrame(Image&, const RectF&, std::array<double, 4> radii, double inner, double outer, Color, ImagePixel::BlendMode, EnableAntialiasing);
	void Balloon(Image&, const RoundRect&, const Vec2& target, double tailWidth, Color, ImagePixel::BlendMode, EnableAntialiasing);
	void BalloonFrame(Image&, const RoundRect&, const Vec2& target, double tailWidth, double thickness, Color, ImagePixel::BlendMode, EnableAntialiasing);
	void Crescent(Image&, const Circle&, double openingAngle, double width, Color, ImagePixel::BlendMode, EnableAntialiasing);
	void Teardrop(Image&, const Circle&, const Vec2& tip, Color, ImagePixel::BlendMode, EnableAntialiasing);
	void CircleArc(Image&, const Circle&, LineCap startCap, LineCap endCap, double startAngle, double angle, double inner, double outer, Color, ImagePixel::BlendMode, EnableAntialiasing, ImageDraw::DstAlpha = ImageDraw::DstAlpha::UseStored);
	void EllipseRotated(Image&, const Ellipse&, double rotation, Color, ImagePixel::BlendMode, EnableAntialiasing);
	void EllipseFrameRotated(Image&, const Ellipse&, double rotation, double innerThickness, double outerThickness, Color, ImagePixel::BlendMode, EnableAntialiasing);
	void EllipseArc(Image&, const Ellipse&, LineCap startCap, LineCap endCap, double startAngle, double angle, double inner, double outer, Color, ImagePixel::BlendMode, EnableAntialiasing);
	void BezierArrow(Image&, const Bezier2&, LineCap startCap, double thickness, const SizeF& headSize, Color, ImagePixel::BlendMode, EnableAntialiasing, double targetError, int32 maxSegments);
	void BezierDoubleArrow(Image&, const Bezier2&, double thickness, const SizeF& headSize, Color, ImagePixel::BlendMode, EnableAntialiasing, double targetError, int32 maxSegments);
	void BezierArrow(Image&, const Bezier3&, LineCap startCap, double thickness, const SizeF& headSize, Color, ImagePixel::BlendMode, EnableAntialiasing, double targetError, int32 maxSegments);
	void BezierDoubleArrow(Image&, const Bezier3&, double thickness, const SizeF& headSize, Color, ImagePixel::BlendMode, EnableAntialiasing, double targetError, int32 maxSegments);
	void PolylineArrow(Image&, std::span<const Vec2>, LineCap startCap, double thickness, const SizeF& headSize, Color, ImagePixel::BlendMode, EnableAntialiasing);
	void PolylineDoubleArrow(Image&, std::span<const Vec2>, double thickness, const SizeF& headSize, Color, ImagePixel::BlendMode, EnableAntialiasing);
	void SplineArrow(Image&, std::span<const Bezier3>, LineCap startCap, double thickness, const SizeF& headSize, Color, ImagePixel::BlendMode, EnableAntialiasing, double targetError, int32 maxSegmentsPerCurve);
	void SplineDoubleArrow(Image&, std::span<const Bezier3>, double thickness, const SizeF& headSize, Color, ImagePixel::BlendMode, EnableAntialiasing, double targetError, int32 maxSegmentsPerCurve);
	void CircleArcArrow(Image&, const Circle&, LineCap startCap, double startAngle, double angle, double thickness, const SizeF& headSize, Color, ImagePixel::BlendMode, EnableAntialiasing);
	void CircleArcDoubleArrow(Image&, const Circle&, double startAngle, double angle, double thickness, const SizeF& headSize, Color, ImagePixel::BlendMode, EnableAntialiasing);
}
