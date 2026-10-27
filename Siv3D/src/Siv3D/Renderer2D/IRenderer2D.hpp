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
# include <memory>
# include <Siv3D/Common.hpp>
# include <Siv3D/Optional.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/TriangleIndex.hpp>
# include <Siv3D/Texture.hpp>
# include <Siv3D/VertexShader.hpp>
# include <Siv3D/PixelShader.hpp>
# include "ColorFillDirection.hpp"
# include <Siv3D/ShaderStage.hpp>

namespace s3d
{
	namespace detail::RoundRectBalloon { struct Placement; }
	struct FloatRect;
	struct FloatQuad;
	struct LineStyle;
	struct BlendState;
	struct RasterizerState;
	struct SamplerState;
	struct Mat3x2;
	struct Mat3x3;
	enum class LineCap : uint8;
	struct PatternParameters;
	struct Bezier2;
	struct Bezier3;

	class SIV3D_NOVTABLE ISiv3DRenderer2D
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	Create
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static ISiv3DRenderer2D* Create();

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		virtual ~ISiv3DRenderer2D() = default;

		////////////////////////////////////////////////////////////////
		//
		//	init
		//
		////////////////////////////////////////////////////////////////

		virtual void init() = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addLine
		//
		////////////////////////////////////////////////////////////////

		virtual void addLine(LineCap startCap, LineCap endCap, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2]) = 0;

		virtual void addLine(const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2]) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addArrow
		//
		////////////////////////////////////////////////////////////////

		virtual void addArrow(LineCap startCap, const Float2& start, const Float2& end, float thickness, const Float2& headSize, const Float4(&colors)[2]) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addTriangle
		//
		////////////////////////////////////////////////////////////////

		virtual void addTriangle(const Float2(&points)[3], const Float4& color) = 0;

		virtual void addTriangle(const Float2(&points)[3], const Float4(&colors)[3]) = 0;

		virtual void addTriangle(const Float2(&points)[3], const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addRect
		//
		////////////////////////////////////////////////////////////////

		virtual void addRect(const FloatRect& rect, const Float4& color) = 0;

		virtual void addRect(const FloatRect& rect, const Float4(&colors)[4]) = 0;

		virtual void addRect(const FloatRect& rect, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addRectFrame
		//
		////////////////////////////////////////////////////////////////

		virtual void addRectFrame(const FloatRect& innerRect, float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addRectFrame(const FloatRect& innerRect, float thickness, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addRectDashedFrame
		//
		////////////////////////////////////////////////////////////////

		virtual void addRectDashedFrame(const FloatRect& innerRect, float offset, float thickness, float dashRatio, uint32 dashCount, const Float4& color) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addQuad
		//
		////////////////////////////////////////////////////////////////

		virtual void addQuad(const FloatQuad& quad, const Float4& color) = 0;

		virtual void addQuad(const FloatQuad& quad, const Float4(&colors)[4]) = 0;

		virtual void addQuad(const FloatQuad& quad, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addCircle
		//
		////////////////////////////////////////////////////////////////

		virtual void addCircle(const Float2& center, float r, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addCircle(const Float2& center, float r, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addCircleFrame
		//
		////////////////////////////////////////////////////////////////

		virtual void addCircleFrame(const Float2& center, float rInner, float thickness, const Float4& innerColor, const Float4& outerColor) = 0;

		virtual void addCircleFrame(const Float2& center, float rInner, float thickness, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addCircleDashedFrame
		//
		////////////////////////////////////////////////////////////////

		virtual void addCircleDashedFrame(const Float2& center, float rInner, float startAngle, float thickness, float dashRatio, uint32 dashCount, const Float4& innerColor, const Float4& outerColor) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addCirclePie
		//
		////////////////////////////////////////////////////////////////

		virtual void addCirclePie(const Float2& center, float r, float startAngle, float angle, const Float4& innerColor, const Float4& outerColor) = 0;

		virtual void addCirclePie(const Float2& center, float r, float startAngle, float angle, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addCircleArc
		//
		////////////////////////////////////////////////////////////////

		virtual void addCircleArc(LineCap lineCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addCircleArc(LineCap lineCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, const PatternParameters& pattern) = 0;

		virtual void addCircleArc(LineCap startCap, LineCap endCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addCircleArc(LineCap startCap, LineCap endCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addCircleSegment
		//
		////////////////////////////////////////////////////////////////

		virtual void addCircleSegment(const Float2& center, float r, float startAngle, float angle, const Float4& color) = 0;

		virtual void addCircleSegment(const Float2& center, float r, float startAngle, float angle, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addCircleArcArrow
		//
		////////////////////////////////////////////////////////////////

		void addCircleArcArrow(const Circle& circle, LineCap startCap, double startAngle, double angle, float thickness, const SizeF& headSize, const Float4& color);

		////////////////////////////////////////////////////////////////
		//
		//	addEllipse
		//
		////////////////////////////////////////////////////////////////

		virtual void addEllipse(const Float2& center, float a, float b, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addEllipse(const Float2& center, float a, float b, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addEllipseFrame
		//
		////////////////////////////////////////////////////////////////

		virtual void addEllipseFrame(const Float2& center, float a, float b, float innerThickness, float outerThickness, const Float4& innerColor, const Float4& outerColor) = 0;

		virtual void addEllipseFrame(const Float2& center, float a, float b, float innerThickness, float outerThickness, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addEllipseDashedFrame
		//
		////////////////////////////////////////////////////////////////

		virtual void addEllipseDashedFrame(const Float2& center, float a, float b, float innerThickness, float outerThickness, float offset, float dashRatio, uint32 dashCount, const Float4& innerColor, const Float4& outerColor) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addEllipseRotated
		//
		////////////////////////////////////////////////////////////////

		virtual void addEllipseRotated(const Float2& center, float a, float b, float rotation, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addEllipseRotated(const Float2& center, float a, float b, float rotation, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addEllipseFrameRotated
		//
		////////////////////////////////////////////////////////////////

		virtual void addEllipseFrameRotated(const Float2& center, float a, float b, float rotation, float innerThickness, float outerThickness, const Float4& innerColor, const Float4& outerColor) = 0;

		virtual void addEllipseFrameRotated(const Float2& center, float a, float b, float rotation, float innerThickness, float outerThickness, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addEllipsePie
		//
		////////////////////////////////////////////////////////////////

		virtual void addEllipsePie(const Float2& center, float rx, float ry, float startAngle, float angle, const Float4& innerColor, const Float4& outerColor) = 0;

		virtual void addEllipsePie(const Float2& center, float rx, float ry, float startAngle, float angle, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addEllipseArc
		//
		////////////////////////////////////////////////////////////////

		virtual void addEllipseArc(LineCap startCap, LineCap endCap, const Float2& center, float a, float b, float startAngle, float angle, float innerThickness, float outerThickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addEllipseArc(LineCap startCap, LineCap endCap, const Float2& center, float a, float b, float startAngle, float angle, float innerThickness, float outerThickness, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addSuperEllipse
		//
		////////////////////////////////////////////////////////////////

		virtual void addSuperEllipse(const Float2& center, float a, float b, float n, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addSuperEllipse(const Float2& center, float a, float b, float n, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addSuperEllipseFrame
		//
		////////////////////////////////////////////////////////////////

		void addSuperEllipseFrame(const SuperEllipse& shape, float thickness, const Float4& color);

		void addSuperEllipseFrame(const SuperEllipse& shape, float thickness, const PatternParameters& pattern);

		////////////////////////////////////////////////////////////////
		//
		//	addRoundRect
		//
		////////////////////////////////////////////////////////////////

		virtual void addRoundRect(const FloatRect& rect, float r, const Float4& color) = 0;

		virtual void addRoundRect(const FloatRect& rect, float r, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addRoundRect(const FloatRect& rect, float r, const PatternParameters& pattern) = 0;

		virtual void addRoundRect(const FloatRect& rect, const std::array<float, 4>& radii, const Float4& color) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addRoundRectFrame
		//
		////////////////////////////////////////////////////////////////

		virtual void addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const Float4& color) = 0;

		virtual void addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const PatternParameters& pattern) = 0;

		virtual void addRoundRectFrame(const FloatRect& innerRect, const std::array<float, 4>& innerRadii, const FloatRect& outerRect, const std::array<float, 4>& outerRadii, const Float4& color) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addRoundRectDashedFrame
		//
		////////////////////////////////////////////////////////////////

		virtual void addRoundRectDashedFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, float offset, float dashRatio, uint32 dashCount, const Float4& color) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addRoundRectBalloon
		//
		////////////////////////////////////////////////////////////////

		virtual void addRoundRectBalloon(const detail::RoundRectBalloon::Placement& balloon, const Float4& color) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addRoundRectBalloonFrame
		//
		////////////////////////////////////////////////////////////////

		virtual void addRoundRectBalloonFrame(const detail::RoundRectBalloon::Placement& balloon, float thickness, const Float4& color) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addPolygon
		//
		////////////////////////////////////////////////////////////////

		virtual void addPolygon(std::span<const Float2> vertices, std::span<const TriangleIndex> triangleIndices, const Optional<Float2>& offset, const Float4& color) = 0;

		virtual void addPolygon(std::span<const Float2> vertices, std::span<const TriangleIndex> triangleIndices, const Optional<Float2>& offset, const PatternParameters& pattern) = 0;

		virtual void addPolygon(std::span<const Float2> vertices, std::span<const Vertex2D::IndexType> indices, const Float4& color) = 0;

		virtual void addPolygon(std::span<const Float2> vertices, std::span<const Vertex2D::IndexType> indices, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addPolygonTransformed
		//
		////////////////////////////////////////////////////////////////

		virtual void addPolygonTransformed(std::span<const Float2> vertices, std::span<const TriangleIndex> triangleIndices, float s, float c, const Float2& offset, const Float4& color) = 0;

		virtual void addPolygonTransformed(std::span<const Float2> vertices, std::span<const TriangleIndex> triangleIndices, float s, float c, const Float2& offset, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addShape2DFrame
		//
		////////////////////////////////////////////////////////////////

		virtual void addShape2DFrame(std::span<const Float2> vertices, const Optional<Float2>& offset, float thickness, const Float4& color) = 0;

		virtual void addShape2DFrame(std::span<const Float2> vertices, const Optional<Float2>& offset, float thickness, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	tryAddSmallConvexFrame
		//
		////////////////////////////////////////////////////////////////

		// Returns false only for unsupported topology, before reserving a buffer.
		virtual bool tryAddSmallConvexFrame(std::span<const Vec2> points, JoinStyle joinStyle, double innerThickness, double outerThickness, const Float4& color) = 0;

		virtual bool tryAddSmallConvexFrame(std::span<const Vec2> points, JoinStyle joinStyle, double innerThickness, double outerThickness, const PatternParameters& pattern) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addLineString
		//
		////////////////////////////////////////////////////////////////

		virtual void addLineString(LineCap startCap, LineCap endCap, std::span<const Vec2> points, const Optional<Float2>& offset, float thickness, bool inner, CloseRing closeRing, const Float4& color) = 0;

		virtual void addLineString(LineCap startCap, LineCap endCap, std::span<const Vec2> points, const Optional<Float2>& offset, float thickness, bool inner, const Float4& colorStart, const Float4& colorEnd) = 0;

		virtual void addLineString(LineCap startCap, LineCap endCap, std::span<const Vec2> points, const Optional<Float2>& offset, float thickness, bool inner, CloseRing closeRing, const PatternParameters& pattern) = 0;

		virtual void addLineString(LineCap startCap, LineCap endCap, std::span<const Vec2> points, const Optional<Float2>& offset, float thickness, bool inner, CloseRing closeRing, std::span<const ColorF> colors) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addLineStringArrow
		//
		////////////////////////////////////////////////////////////////

		// false requests ordinary-curve fallback; allocation failure is handled (true).
		virtual bool addLineStringArrow(LineCap startCap, std::span<const Vec2> points, const Float2& tip, float thickness, float headWidth, const Float4& color) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addPolylineArrow
		//
		////////////////////////////////////////////////////////////////

		void addPolylineArrow(std::span<const Vec2> points, LineCap startCap, float thickness, const SizeF& headSize, const Float4& color);

		////////////////////////////////////////////////////////////////
		//
		//	addBezier
		//
		////////////////////////////////////////////////////////////////

		// Shared CPU tessellation and reusable storage, independent of the graphics backend.
		void addBezier(const Bezier2& curve, LineCap startCap, LineCap endCap, float thickness, const Float4& color, double targetError, int32 maxSegments);

		void addBezier(const Bezier3& curve, LineCap startCap, LineCap endCap, float thickness, const Float4& color, double targetError, int32 maxSegments);

		void addBezier(const Bezier2& curve, LineCap startCap, LineCap endCap, float thickness, const Float4& startColor, const Float4& endColor, double targetError, int32 maxSegments);

		void addBezier(const Bezier3& curve, LineCap startCap, LineCap endCap, float thickness, const Float4& startColor, const Float4& endColor, double targetError, int32 maxSegments);

		////////////////////////////////////////////////////////////////
		//
		//	addBezierArrow
		//
		////////////////////////////////////////////////////////////////

		void addBezierArrow(const Bezier2& curve, LineCap startCap, float thickness, const SizeF& headSize, const Float4& color, double targetError, int32 maxSegments);

		void addBezierArrow(const Bezier3& curve, LineCap startCap, float thickness, const SizeF& headSize, const Float4& color, double targetError, int32 maxSegments);

		////////////////////////////////////////////////////////////////
		//
		//	addSpline
		//
		////////////////////////////////////////////////////////////////

		void addSpline(std::span<const Bezier3> curves, CloseRing closeRing, float thickness, const Float4& color, double targetError, int32 maxSegmentsPerCurve);

		////////////////////////////////////////////////////////////////
		//
		//	addSplineArrow
		//
		////////////////////////////////////////////////////////////////

		void addSplineArrow(std::span<const Bezier3> curves, LineCap startCap, float thickness, const SizeF& headSize, const Float4& color, double targetError, int32 maxSegmentsPerCurve);

		////////////////////////////////////////////////////////////////
		//
		//	addTexturedQuad
		//
		////////////////////////////////////////////////////////////////

		virtual void addTexturedQuad(const Texture& texture, const FloatQuad& quad, const FloatRect& uv, const Float4& color) = 0;

		virtual void addTexturedQuad(const Texture& texture, const FloatQuad& quad, const FloatRect& uv, const Float4(&colors)[4]) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addTexturedCircle
		//
		////////////////////////////////////////////////////////////////

		virtual void addTexturedCircle(const Texture& texture, const Circle& circle, const FloatRect& uv, const Float4& color) = 0;

		virtual void addTexturedCircle(const Texture& texture, const Circle& circle, const FloatRect& uv, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addTexturedCirclePie
		//
		////////////////////////////////////////////////////////////////

		virtual void addTexturedCirclePie(const Texture& texture, const Float2& center, float r, float startAngle, float angle, const FloatRect& uv, const Float4& innerColor, const Float4& outerColor) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addTexturedRoundRect
		//
		////////////////////////////////////////////////////////////////

		virtual void addTexturedRoundRect(const Texture& texture, const FloatRect& rect, float w, float h, float r, const FloatRect& uvRect, const Float4& color) = 0;

		virtual void addTexturedRoundRect(const Texture& texture, const FloatRect& rect, float w, float h, float r, const FloatRect& uvRect, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addRectShadow
		//
		////////////////////////////////////////////////////////////////

		virtual void addRectShadow(const FloatRect& rect, float blur, const Float4& color, bool fill) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addCircleShadow
		//
		////////////////////////////////////////////////////////////////

		virtual void addCircleShadow(const Circle& circle, float blur, const Float4& color, bool fill) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addRoundRectShadow
		//
		////////////////////////////////////////////////////////////////

		virtual void addRoundRectShadow(const RoundRect& roundRect, float blur, const Float4& color, bool fill) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	addMesh2D
		//
		////////////////////////////////////////////////////////////////

		virtual void addMesh2D(std::span<const Vertex2D> vertices, std::span<const TriangleIndex> indices, const Optional<Float2>& offset) = 0;

		virtual void addMesh2D(const Texture& texture, std::span<const Vertex2D> vertices, std::span<const TriangleIndex> indices, const Optional<Float2>& offset) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	setConstantBuffer
		//
		////////////////////////////////////////////////////////////////

		virtual void setConstantBuffer(ShaderStage stage, uint32 slot, const void* data, size_t size) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	beginConstantBufferScope, endConstantBufferScope
		//
		////////////////////////////////////////////////////////////////

		virtual uint32 beginConstantBufferScope(ShaderStage stage, uint32 slot, const void* data, size_t size) = 0;

		virtual void endConstantBufferScope(ShaderStage stage, uint32 slot, uint32 previous) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	flush
		//
		////////////////////////////////////////////////////////////////

		virtual void flush() = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getSceneScale, setSceneScale
		//
		////////////////////////////////////////////////////////////////

		virtual Float2 getSceneScale() const noexcept = 0;

		virtual void setSceneScale(Float2 scale) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getColorMul, setColorMul
		//
		////////////////////////////////////////////////////////////////

		virtual Float4 getColorMul() const = 0;

		virtual void setColorMul(const Float4& color) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getColorAdd, setColorAdd
		//
		////////////////////////////////////////////////////////////////

		virtual Float3 getColorAdd() const = 0;

		virtual void setColorAdd(const Float3& color) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getBlendState, setBlendState
		//
		////////////////////////////////////////////////////////////////

		virtual BlendState getBlendState() const = 0;

		virtual void setBlendState(const BlendState& state) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getRasterizerState, setRasterizerState
		//
		////////////////////////////////////////////////////////////////

		virtual RasterizerState getRasterizerState() const = 0;

		virtual void setRasterizerState(const RasterizerState& state) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getVSSamplerState, setVSSamplerState
		//
		////////////////////////////////////////////////////////////////

		virtual SamplerState getVSSamplerState(uint32 slot) const = 0;

		virtual void setVSSamplerState(uint32 slot, const SamplerState& state) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getPSSamplerState, setPSSamplerState
		//
		////////////////////////////////////////////////////////////////

		virtual SamplerState getPSSamplerState(uint32 slot) const = 0;

		virtual void setPSSamplerState(uint32 slot, const SamplerState& state) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getScissorRect, setScissorRect
		//
		////////////////////////////////////////////////////////////////

		virtual Optional<Rect> getScissorRect() const = 0;

		virtual void setScissorRect(const Optional<Rect>& rect) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getViewport, setViewport
		//
		////////////////////////////////////////////////////////////////

		virtual Optional<Rect> getViewport() const = 0;

		virtual void setViewport(const Optional<Rect>& viewport) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	setSDFParameters
		//
		////////////////////////////////////////////////////////////////

		virtual void setSDFParameters(const std::array<Float4, 3>& params) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getCustomVS, setCustomVS
		//
		////////////////////////////////////////////////////////////////

		virtual Optional<VertexShader> getCustomVS() const = 0;

		virtual void setCustomVS(const Optional<VertexShader>& vs) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getCustomPS, setCustomPS
		//
		////////////////////////////////////////////////////////////////

		virtual Optional<PixelShader> getCustomPS() const = 0;

		virtual void setCustomPS(const Optional<PixelShader>& ps) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getLocalTransform, setLocalTransform
		//
		////////////////////////////////////////////////////////////////

		virtual const Mat3x2& getLocalTransform() const = 0;

		virtual void setLocalTransform(const Mat3x2& matrix) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getCameraTransform, setCameraTransform
		//
		////////////////////////////////////////////////////////////////

		virtual const Mat3x2& getCameraTransform() const = 0;

		virtual void setCameraTransform(const Mat3x2& matrix) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getQuadWarpTransform, setQuadWarpTransform
		//
		////////////////////////////////////////////////////////////////

		virtual const Mat3x3& getQuadWarpTransform() const = 0;

		virtual void setQuadWarpTransform(const Mat3x3& matrix) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getRMSScaling
		//
		////////////////////////////////////////////////////////////////

		virtual float getRMSScaling() const noexcept = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getRasterScaling
		//
		////////////////////////////////////////////////////////////////

		virtual float getRasterScaling() const noexcept = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getShadowTexture
		//
		////////////////////////////////////////////////////////////////

		virtual const Texture& getShadowTexture() const noexcept = 0;

	protected:

		////////////////////////////////////////////////////////////////
		//
		//	CreateShadowTexture
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static std::unique_ptr<Texture> CreateShadowTexture();

	private:

		Array<Vec2> m_curvePoints;
	};
}
