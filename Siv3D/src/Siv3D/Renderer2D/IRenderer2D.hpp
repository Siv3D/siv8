//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Optional.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/2DShapes.hpp>
# include "ColorFillDirection.hpp"

namespace s3d
{
	struct FloatRect;
	struct FloatQuad;
	struct LineStyle;
	struct BlendState;
	struct RasterizerState;
	struct SamplerState;
	struct Mat3x2;
	enum class LineCap : uint8;
	struct PatternParameters;

	class SIV3D_NOVTABLE ISiv3DRenderer2D
	{
	public:

		[[nodiscard]]
		static ISiv3DRenderer2D* Create();

		virtual ~ISiv3DRenderer2D() = default;

		virtual void init() = 0;

		virtual void addLine(LineCap startCap, LineCap endCap, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2]) = 0;

		virtual void addLine(const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2]) = 0;

		virtual void addTriangle(const Float2(&points)[3], const Float4& color) = 0;

		virtual void addTriangle(const Float2(&points)[3], const Float4(&colors)[3]) = 0;

		virtual void addTriangle(const Float2(&points)[3], const PatternParameters& pattern) = 0;

		virtual void addRect(const FloatRect& rect, const Float4& color) = 0;

		virtual void addRect(const FloatRect& rect, const Float4(&colors)[4]) = 0;

		virtual void addRect(const FloatRect& rect, const PatternParameters& pattern) = 0;

		virtual void addRectFrame(const FloatRect& innerRect, float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addRectFrame(const FloatRect& innerRect, float thickness, const PatternParameters& pattern) = 0;

		virtual void addCircle(const Float2& center, float r, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addCircle(const Float2& center, float r, const PatternParameters& pattern) = 0;

		virtual void addCircleFrame(const Float2& center, float rInner, float thickness, const Float4& innerColor, const Float4& outerColor) = 0;

		virtual void addCircleFrame(const Float2& center, float rInner, float thickness, const PatternParameters& pattern) = 0;

		virtual void addCirclePie(const Float2& center, float r, float startAngle, float angle, const Float4& innerColor, const Float4& outerColor) = 0;

		virtual void addCirclePie(const Float2& center, float r, float startAngle, float angle, const PatternParameters& pattern) = 0;

		virtual void addCircleArc(LineCap lineCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addCircleArc(LineCap lineCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, const PatternParameters& pattern) = 0;

		virtual void addCircleSegment(const Float2& center, float r, float startAngle, float angle, const Float4& color) = 0;

		virtual void addCircleSegment(const Float2& center, float r, float startAngle, float angle, const PatternParameters& pattern) = 0;

		virtual void addEllipse(const Float2& center, float a, float b, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addEllipse(const Float2& center, float a, float b, const PatternParameters& pattern) = 0;

		virtual void addEllipseFrame(const Float2& center, float aInner, float bInner, float thickness, const Float4& innerColor, const Float4& outerColor) = 0;

		virtual void addEllipseFrame(const Float2& center, float aInner, float bInner, float thickness, const PatternParameters& pattern) = 0;

		virtual void addQuad(const FloatQuad& quad, const Float4& color) = 0;

		virtual void addQuad(const FloatQuad& quad, const Float4(&colors)[4]) = 0;

		virtual void addQuad(const FloatQuad& quad, const PatternParameters& pattern) = 0;

		virtual void addRoundRect(const FloatRect& rect, float r, const Float4& color) = 0;

		virtual void addRoundRect(const FloatRect& rect, float r, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addRoundRect(const FloatRect& rect, float r, const PatternParameters& pattern) = 0;

		virtual void addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const Float4& color) = 0;

		virtual void addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const Float4& color0, const Float4& color1, ColorFillDirection colorType) = 0;

		virtual void addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const PatternParameters& pattern) = 0;




		virtual void flush() = 0;


		virtual Float4 getColorMul() const = 0;

		virtual void setColorMul(const Float4& color) = 0;

		virtual Float3 getColorAdd() const = 0;

		virtual void setColorAdd(const Float3& color) = 0;

		virtual BlendState getBlendState() const = 0;

		virtual void setBlendState(const BlendState& state) = 0;

		virtual RasterizerState getRasterizerState() const = 0;

		virtual void setRasterizerState(const RasterizerState& state) = 0;

		virtual SamplerState getVSSamplerState(uint32 slot) const = 0;

		virtual void setVSSamplerState(uint32 slot, const SamplerState& state) = 0;

		virtual SamplerState getPSSamplerState(uint32 slot) const = 0;

		virtual void setPSSamplerState(uint32 slot, const SamplerState& state) = 0;

		virtual Optional<Rect> getScissorRect() const = 0;

		virtual void setScissorRect(const Optional<Rect>& rect) = 0;

		virtual Optional<Rect> getViewport() const = 0;

		virtual void setViewport(const Optional<Rect>& viewport) = 0;

		virtual const Mat3x2& getLocalTransform() const = 0;

		virtual void setLocalTransform(const Mat3x2& matrix) = 0;

		virtual const Mat3x2& getCameraTransform() const = 0;

		virtual void setCameraTransform(const Mat3x2& matrix) = 0;

		virtual float getMaxScaling() const noexcept = 0;
	};
}
