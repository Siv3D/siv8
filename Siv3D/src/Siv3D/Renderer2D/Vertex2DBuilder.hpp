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
# include <array>
# include <Siv3D/Common.hpp>
# include <Siv3D/Vertex2D.hpp>
# include <Siv3D/FloatRect.hpp>
# include <Siv3D/TriangleIndex.hpp>
# include <Siv3D/FunctionRef.hpp>
# include "Vertex2DBufferPointer.hpp"
# include "ColorFillDirection.hpp"

namespace s3d
{
	namespace detail::RoundRectBalloon { struct Placement; }
	using BufferCreatorFunc = FunctionRef<Vertex2DBufferPointer(uint16, uint32)>;
	struct LineStyle;
	struct FloatQuad;
	enum class LineCap : uint8;
	enum class JoinStyle : uint8;

	namespace Vertex2DBuilder
	{
		////////////////////////////////////////////////////////////////
		//
		//	CalculateCircleArcQuality
		//
		////////////////////////////////////////////////////////////////

		// Points including both endpoints (2..253); r is in raster pixels, angle is in [0, 2*pi].
		[[nodiscard]]
		Vertex2D::IndexType CalculateCircleArcQuality(float r, float angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	BuildLine
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildLine(const BufferCreatorFunc& bufferCreator, LineCap startCap, LineCap endCap, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2], float scale);

		[[nodiscard]]
		Vertex2D::IndexType BuildLine(const BufferCreatorFunc& bufferCreator, const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2], float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildArrow
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildArrow(const BufferCreatorFunc& bufferCreator, LineCap startCap, const Float2& start, const Float2& end, float thickness, const Float2& headSize, const Float4(&colors)[2], float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildTriangle
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildTriangle(const BufferCreatorFunc& bufferCreator, const Float2(&points)[3], const Float4& color);

		[[nodiscard]]
		Vertex2D::IndexType BuildTriangle(const BufferCreatorFunc& bufferCreator, const Float2(&points)[3], const Float4(&colors)[3]);

		////////////////////////////////////////////////////////////////
		//
		//	BuildRect
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const Float4& color);

		[[nodiscard]]
		Vertex2D::IndexType BuildRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const Float4(&colors)[4]);

		////////////////////////////////////////////////////////////////
		//
		//	BuildRectFrame
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildRectFrame(const BufferCreatorFunc& bufferCreator, const FloatRect& innerRect, float thickness, ColorFillDirection colorType, const Float4& color0, const Float4& color1);

		////////////////////////////////////////////////////////////////
		//
		//	BuildRectDashedFrame
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildRectDashedFrame(const BufferCreatorFunc& bufferCreator, const FloatRect& innerRect, float offset, float thickness, float dashRatio, uint32 dashCount, const Float4& color);

		////////////////////////////////////////////////////////////////
		//
		//	BuildQuad
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildQuad(const BufferCreatorFunc& bufferCreator, const FloatQuad& quad, const Float4 color);

		[[nodiscard]]
		Vertex2D::IndexType BuildQuad(const BufferCreatorFunc& bufferCreator, const FloatQuad& quad, const Float4(&colors)[4]);

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircle
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildCircle(const BufferCreatorFunc& bufferCreator, const Float2& center, float r, ColorFillDirection colorType, const Float4& color0, const Float4& color1, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircleFrame
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildCircleFrame(const BufferCreatorFunc& bufferCreator, const Float2& center, float rInner, float thickness, const Float4& innerColor, const Float4& outerColor, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircleDashedFrame
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildCircleDashedFrame(const BufferCreatorFunc& bufferCreator, const Float2& center, float rInner, float startAngle, float thickness, float dashRatio, uint32 dashCount, const Float4& innerColor, const Float4& outerColor, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildCirclePie
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildCirclePie(const BufferCreatorFunc& bufferCreator, const Float2& center, float r, float startAngle, float angle, const Float4& innerColor, const Float4& outerColor, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircleArc
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildCircleArc(const BufferCreatorFunc& bufferCreator, const LineCap lineCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, ColorFillDirection colorType, const Float4& color0, const Float4& color1, float scale);

		[[nodiscard]]
		Vertex2D::IndexType BuildCircleArc(const BufferCreatorFunc& bufferCreator, LineCap startCap, LineCap endCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, ColorFillDirection colorType, const Float4& color0, const Float4& color1, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircleSegment
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildCircleSegment(const BufferCreatorFunc& bufferCreator, const Float2& center, float r, float startAngle, float angle, const Float4& color, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildEllipse
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildEllipse(const BufferCreatorFunc& bufferCreator, const Float2& center, float a, float b, ColorFillDirection colorType, const Float4& color0, const Float4& color1, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildEllipseFrame
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildEllipseFrame(const BufferCreatorFunc& bufferCreator, const Float2& center, float a, float b, float innerThickness, float outerThickness, const Float4& innerColor, const Float4& outerColor, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildEllipseDashedFrame
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildEllipseDashedFrame(const BufferCreatorFunc& bufferCreator, const Float2& center, float a, float b, float innerThickness, float outerThickness, float offset, float dashRatio, uint32 dashCount, const Float4& innerColor, const Float4& outerColor, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildEllipseRotated
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildEllipseRotated(const BufferCreatorFunc& bufferCreator, const Float2& center, float a, float b, float rotation, ColorFillDirection colorType, const Float4& color0, const Float4& color1, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildEllipseFrameRotated
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildEllipseFrameRotated(const BufferCreatorFunc& bufferCreator, const Float2& center, float a, float b, float rotation, float innerThickness, float outerThickness, const Float4& innerColor, const Float4& outerColor, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildEllipsePie
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildEllipsePie(const BufferCreatorFunc& bufferCreator, const Float2& center, float rx, float ry, float startAngle, float angle, const Float4& innerColor, const Float4& outerColor, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildEllipseArc
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildEllipseArc(const BufferCreatorFunc& bufferCreator, LineCap startCap, LineCap endCap, const Float2& center, float a, float b, float startAngle, float angle, float innerThickness, float outerThickness, ColorFillDirection colorType, const Float4& color0, const Float4& color1, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildSuperEllipse
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildSuperEllipse(const BufferCreatorFunc& bufferCreator, const Float2& center, float a, float b, float n, ColorFillDirection colorType, const Float4& color0, const Float4& color1, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildRoundRect
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, float r, const Float4& color, float scale);

		[[nodiscard]]
		Vertex2D::IndexType BuildRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, float r, ColorFillDirection colorType, const Float4& color0, const Float4& color1, float scale);

		// Radii are normalized, nonnegative, in TL, TR, BR, BL order.
		[[nodiscard]]
		Vertex2D::IndexType BuildRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const std::array<float, 4>& radii, const Float4& color, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildRoundRectFrame
		//
		////////////////////////////////////////////////////////////////

	
		[[nodiscard]]
		Vertex2D::IndexType BuildRoundRectFrame(const BufferCreatorFunc& bufferCreator, const FloatRect& innerRect, float innerR, const FloatRect& outerRect, float outerR, const Float4& color, float scale);

		[[nodiscard]]
		Vertex2D::IndexType BuildRoundRectFrame(const BufferCreatorFunc& bufferCreator, const FloatRect& innerRect, float innerR, const FloatRect& outerRect, float outerR, ColorFillDirection colorType, const Float4& color0, const Float4& color1, float scale);

		// Corresponding inner/outer rounded rectangles use the drawRoundedFrame offset contract.
		[[nodiscard]]
		Vertex2D::IndexType BuildRoundRectFrame(const BufferCreatorFunc& bufferCreator, const FloatRect& innerRect, const std::array<float, 4>& innerRadii, const FloatRect& outerRect, const std::array<float, 4>& outerRadii, const Float4& color, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildRoundRectDashedFrame
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildRoundRectDashedFrame(const BufferCreatorFunc& bufferCreator, const FloatRect& innerRect, float innerR, const FloatRect& outerRect, float outerR, float offset, float dashRatio, uint32 dashCount, const Float4& color, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildRoundRectBalloon
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint32 BuildRoundRectBalloon(const BufferCreatorFunc& bufferCreator, const detail::RoundRectBalloon::Placement& balloon, const Float4& color, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildRoundRectBalloonFrame
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint32 BuildRoundRectBalloonFrame(const BufferCreatorFunc& bufferCreator, const detail::RoundRectBalloon::Placement& balloon, float thickness, const Float4& color, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildPolygon
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint32 BuildPolygon(const BufferCreatorFunc& bufferCreator, std::span<const Float2> vertices, std::span<const TriangleIndex> triangleIndices, const Optional<Float2>& offset, const Float4& color);

		[[nodiscard]]
		uint32 BuildPolygon(const BufferCreatorFunc& bufferCreator, std::span<const Float2> vertices, std::span<const Vertex2D::IndexType> indices, const Float4& color);

		////////////////////////////////////////////////////////////////
		//
		//	BuildPolygonTransformed
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint32 BuildPolygonTransformed(const BufferCreatorFunc& bufferCreator, std::span<const Float2> vertices, std::span<const TriangleIndex> triangleIndices, float s, float c, const Float2& offset, const Float4& color);

		////////////////////////////////////////////////////////////////
		//
		//	BuildShape2DFrame
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildShape2DFrame(const BufferCreatorFunc& bufferCreator, std::span<const Float2> vertices, const Optional<Float2>& offset, float thickness, const Float4& color, const float scale);

		////////////////////////////////////////////////////////////////
		//
		//	TryBuildSmallConvexFrame
		//
		////////////////////////////////////////////////////////////////

		// none: unsupported topology, no allocation; 0: handled but allocation failed.
		// points is an open ring with 3 or 4 vertices (no repeated closure points).
		[[nodiscard]]
		Optional<uint32> TryBuildSmallConvexFrame(const BufferCreatorFunc& bufferCreator, std::span<const Vec2> points, JoinStyle joinStyle, double thickness, const Float4& color, float scale);

		[[nodiscard]]
		Optional<uint32> TryBuildSmallConvexFrame(const BufferCreatorFunc& bufferCreator, std::span<const Vec2> points, JoinStyle joinStyle, double innerThickness, double outerThickness, const Float4& color, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildLineString
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint32 BuildLineString(const BufferCreatorFunc& bufferCreator, LineCap startCap, LineCap endCap, std::span<const Vec2> points, const Optional<Float2>& offset, float thickness, bool inner, CloseRing closeRing, const Float4& color, float scale);

		[[nodiscard]]
		uint32 BuildLineString(const BufferCreatorFunc& bufferCreator, LineCap startCap, LineCap endCap, std::span<const Vec2> points, const Optional<Float2>& offset, float thickness, bool inner, const Float4& colorStart, const Float4& colorEnd, float scale);

		[[nodiscard]]
		uint32 BuildLineString(const BufferCreatorFunc& bufferCreator, LineCap startCap, LineCap endCap, std::span<const Vec2> points, const Optional<Float2>& offset, float thickness, bool inner, CloseRing closeRing, std::span<const ColorF> colors, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildLineStringArrow
		//
		////////////////////////////////////////////////////////////////

		// none requests ordinary-curve fallback; 0 means allocation failed or empty.
		[[nodiscard]]
		Optional<uint32> BuildLineStringArrow(const BufferCreatorFunc& bufferCreator, LineCap startCap, std::span<const Vec2> points, const Float2& tip, float thickness, float headWidth, const Float4& color, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildLineStringDoubleHeadedArrow
		//
		////////////////////////////////////////////////////////////////

		// Same handled/fallback contract as BuildLineStringArrow.
		[[nodiscard]]
		Optional<uint32> BuildLineStringDoubleHeadedArrow(const BufferCreatorFunc& bufferCreator, std::span<const Vec2> points,
			const Float2& startTip, const Float2& endTip, float thickness, float startHeadWidth, float endHeadWidth, const Float4& color, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildTexturedQuad
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildTexturedQuad(const BufferCreatorFunc& bufferCreator, const FloatQuad& quad, const FloatRect& uv, const Float4& color);

		
		[[nodiscard]]
		Vertex2D::IndexType BuildTexturedQuad(const BufferCreatorFunc& bufferCreator, const FloatQuad& rect, const FloatRect& uv, const Float4(&colors)[4]);

		////////////////////////////////////////////////////////////////
		//
		//	BuildTexturedCircle
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildTexturedCircle(const BufferCreatorFunc& bufferCreator, const Circle& circle, const FloatRect& uv, const Float4& color, float scale);

		[[nodiscard]]
		Vertex2D::IndexType BuildTexturedCircle(const BufferCreatorFunc& bufferCreator, const Circle& circle, const FloatRect& uv, ColorFillDirection colorType, const Float4& color0, const Float4& color1, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildTexturedCirclePie
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildTexturedCirclePie(const BufferCreatorFunc& bufferCreator, const Float2& center, float r, float startAngle, float angle, const FloatRect& uv, const Float4& innerColor, const Float4& outerColor, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildTexturedRoundRect
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildTexturedRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, float w, float h, float r, const FloatRect& uvRect, const Float4& color, float scale);

		[[nodiscard]]
		Vertex2D::IndexType BuildTexturedRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, float w, float h, float r, const FloatRect& uvRect, ColorFillDirection colorType, const Float4& color0, const Float4& color1, float scale);

		////////////////////////////////////////////////////////////////
		//
		//	BuildRectShadow
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildRectShadow(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, float blur, const Float4& color, bool fill);

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircleShadow
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildCircleShadow(const BufferCreatorFunc& bufferCreator, const Circle& circle, float blur, const Float4& color, float scale, bool fill);

		////////////////////////////////////////////////////////////////
		//
		//	BuildRoundRectShadow
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vertex2D::IndexType BuildRoundRectShadow(const BufferCreatorFunc& bufferCreator, const RoundRect& roundRect, float blur, const Float4& color, float scale, bool fill);

		////////////////////////////////////////////////////////////////
		//
		//	BuildMesh2D
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint32 BuildMesh2D(const BufferCreatorFunc& bufferCreator, std::span<const Vertex2D> vertices, std::span<const TriangleIndex> triangleIndices, const Optional<Float2>& offset);
	}
}
