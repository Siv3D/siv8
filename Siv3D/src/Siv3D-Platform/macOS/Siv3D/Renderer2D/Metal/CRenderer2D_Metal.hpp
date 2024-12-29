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
# include <Siv3D/Optional.hpp>
# include <Siv3D/ConstantBuffer.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Renderer2D/Renderer2DCommon.hpp>
# include "MetalVertexBufferManager2D.hpp"
# include "MetalRenderer2DCommandManager.hpp"

namespace s3d
{
	enum class PatternType : uint8;
	class CRenderer_Metal;
	class CShader_Metal;
	class CTexture_Metal;

	class CRenderer2D_Metal final : public ISiv3DRenderer2D
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		~CRenderer2D_Metal() override;

		////////////////////////////////////////////////////////////////
		//
		//	init
		//
		////////////////////////////////////////////////////////////////

		void init() override;

		////////////////////////////////////////////////////////////////
		//
		//	addLine
		//
		////////////////////////////////////////////////////////////////

		void addLine(LineCap startCap, LineCap endCap, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2]) override;

		void addLine(const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2]) override;

		////////////////////////////////////////////////////////////////
		//
		//	addTriangle
		//
		////////////////////////////////////////////////////////////////

		void addTriangle(const Float2(&points)[3], const Float4& color) override;

		void addTriangle(const Float2(&points)[3], const Float4(&colors)[3]) override;

		void addTriangle(const Float2(&points)[3], const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addRect
		//
		////////////////////////////////////////////////////////////////

		void addRect(const FloatRect& rect, const Float4& color) override;

		void addRect(const FloatRect& rect, const Float4(&colors)[4]) override;

		void addRect(const FloatRect& rect, const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addRectFrame
		//
		////////////////////////////////////////////////////////////////

		void addRectFrame(const FloatRect& innerRect, float thickness, const Float4& innerColor, const Float4& outerColor, ColorFillDirection colorType) override;

		void addRectFrame(const FloatRect& innerRect, float thickness, const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addCircle
		//
		////////////////////////////////////////////////////////////////

		void addCircle(const Float2& center, float r, const Float4& color0, const Float4& color1, ColorFillDirection colorType) override;

		void addCircle(const Float2& center, float r, const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addCircleFrame
		//
		////////////////////////////////////////////////////////////////

		void addCircleFrame(const Float2& center, float rInner, float thickness, const Float4& innerColor, const Float4& outerColor) override;

		void addCircleFrame(const Float2& center, float rInner, float thickness, const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addCirclePie
		//
		////////////////////////////////////////////////////////////////

		void addCirclePie(const Float2& center, float r, float startAngle, float angle, const Float4& innerColor, const Float4& outerColor) override;

		void addCirclePie(const Float2& center, float r, float startAngle, float angle, const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addCircleArc
		//
		////////////////////////////////////////////////////////////////

		void addCircleArc(LineCap lineCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType) override;

		void addCircleArc(LineCap lineCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addCircleSegment
		//
		////////////////////////////////////////////////////////////////

		void addCircleSegment(const Float2& center, float r, float startAngle, float angle, const Float4& color) override;

		void addCircleSegment(const Float2& center, float r, float startAngle, float angle, const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addEllipse
		//
		////////////////////////////////////////////////////////////////

		void addEllipse(const Float2& center, float a, float b, const Float4& color0, const Float4& color1, ColorFillDirection colorType) override;

		void addEllipse(const Float2& center, float a, float b, const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addEllipseFrame
		//
		////////////////////////////////////////////////////////////////

		void addEllipseFrame(const Float2& center, float aInner, float bInner, float thickness, const Float4& innerColor, const Float4& outerColor) override;

		void addEllipseFrame(const Float2& center, float aInner, float bInner, float thickness, const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addQuad
		//
		////////////////////////////////////////////////////////////////

		void addQuad(const FloatQuad& quad, const Float4& color) override;

		void addQuad(const FloatQuad& quad, const Float4(&colors)[4]) override;

		void addQuad(const FloatQuad& quad, const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addRoundRect
		//
		////////////////////////////////////////////////////////////////

		void addRoundRect(const FloatRect& rect, float r, const Float4& color) override;

		void addRoundRect(const FloatRect& rect, float r, const Float4& color0, const Float4& color1, ColorFillDirection colorType) override;

		void addRoundRect(const FloatRect& rect, float r, const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addRoundRectFrame
		//
		////////////////////////////////////////////////////////////////

		void addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const Float4& color) override;

		void addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const Float4& color0, const Float4& color1, ColorFillDirection colorType) override;

		void addRoundRectFrame(const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addPolygon
		//
		////////////////////////////////////////////////////////////////

		void addPolygon(std::span<const Float2> vertices, std::span<const TriangleIndex> triangleIndices, const Optional<Float2>& offset, const Float4& color) override;

		void addPolygon(std::span<const Float2> vertices, std::span<const TriangleIndex> triangleIndices, const Optional<Float2>& offset, const PatternParameters& pattern) override;

		void addPolygon(std::span<const Float2> vertices, std::span<const Vertex2D::IndexType> indices, const Float4& color) override;

		void addPolygon(std::span<const Float2> vertices, std::span<const Vertex2D::IndexType> indices, const PatternParameters& pattern) override;

		////////////////////////////////////////////////////////////////
		//
		//	addLineString
		//
		////////////////////////////////////////////////////////////////

		void addLineString(LineCap startCap, LineCap endCap, std::span<const Vec2> points, const Optional<Float2>& offset, float thickness, bool inner, CloseRing closeRing, const Float4& color) override;

		void addLineString(LineCap startCap, LineCap endCap, std::span<const Vec2> points, const Optional<Float2>& offset, float thickness, bool inner, const Float4& colorStart, const Float4& colorEnd) override;

		void addLineString(LineCap startCap, LineCap endCap, std::span<const Vec2> points, const Optional<Float2>& offset, float thickness, bool inner, CloseRing closeRing, const PatternParameters& pattern) override;

		void addLineString(LineCap startCap, LineCap endCap, std::span<const Vec2> points, const Optional<Float2>& offset, float thickness, bool inner, CloseRing closeRing, std::span<const ColorF> colors) override;

		////////////////////////////////////////////////////////////////
		//
		//	addTexturedQuad
		//
		////////////////////////////////////////////////////////////////

		void addTexturedQuad(const Texture& texture, const FloatQuad& quad, const FloatRect& uv, const Float4& color) override;

		void addTexturedQuad(const Texture& texture, const FloatQuad& quad, const FloatRect& uv, const Float4(&colors)[4]) override;

		////////////////////////////////////////////////////////////////
		//
		//	addQuadWarp
		//
		////////////////////////////////////////////////////////////////

		void addQuadWarp(const Texture& texture, const FloatRect& uv, const FloatQuad& quad, const Float4& color) override;

		void addQuadWarp(const Texture& texture, const FloatRect& uv, const FloatQuad& quad, const Float4(&colors)[4]) override;
		
		////////////////////////////////////////////////////////////////
		//
		//	flush
		//
		////////////////////////////////////////////////////////////////

		void flush() override;

		////////////////////////////////////////////////////////////////
		//
		//	getColorMul, setColorMul
		//
		////////////////////////////////////////////////////////////////

		Float4 getColorMul() const override;

		void setColorMul(const Float4& color) override;

		////////////////////////////////////////////////////////////////
		//
		//	getColorAdd, setColorAdd
		//
		////////////////////////////////////////////////////////////////

		Float3 getColorAdd() const override;

		void setColorAdd(const Float3& color) override;

		////////////////////////////////////////////////////////////////
		//
		//	getBlendState, setBlendState
		//
		////////////////////////////////////////////////////////////////

		BlendState getBlendState() const override;

		void setBlendState(const BlendState& state) override;

		////////////////////////////////////////////////////////////////
		//
		//	getRasterizerState, setRasterizerState
		//
		////////////////////////////////////////////////////////////////

		RasterizerState getRasterizerState() const override;

		void setRasterizerState(const RasterizerState& state) override;

		////////////////////////////////////////////////////////////////
		//
		//	getVSSamplerState, setVSSamplerState
		//
		////////////////////////////////////////////////////////////////

		SamplerState getVSSamplerState(uint32 slot) const override;

		void setVSSamplerState(uint32 slot, const SamplerState& state) override;

		////////////////////////////////////////////////////////////////
		//
		//	getPSSamplerState, setPSSamplerState
		//
		////////////////////////////////////////////////////////////////

		SamplerState getPSSamplerState(uint32 slot) const override;

		void setPSSamplerState(uint32 slot, const SamplerState& state) override;

		////////////////////////////////////////////////////////////////
		//
		//	getScissorRect, setScissorRect
		//
		////////////////////////////////////////////////////////////////

		Optional<Rect> getScissorRect() const override;

		void setScissorRect(const Optional<Rect>& rect) override;

		////////////////////////////////////////////////////////////////
		//
		//	getViewport, setViewport
		//
		////////////////////////////////////////////////////////////////

		Optional<Rect> getViewport() const override;

		void setViewport(const Optional<Rect>& viewport) override;

		////////////////////////////////////////////////////////////////
		//
		//	getLocalTransform, setLocalTransform
		//
		////////////////////////////////////////////////////////////////

		const Mat3x2& getLocalTransform() const override;

		void setLocalTransform(const Mat3x2& matrix) override;

		////////////////////////////////////////////////////////////////
		//
		//	getCameraTransform, setCameraTransform
		//
		////////////////////////////////////////////////////////////////

		const Mat3x2& getCameraTransform() const override;

		void setCameraTransform(const Mat3x2& matrix) override;

		////////////////////////////////////////////////////////////////
		//
		//	getMaxScaling
		//
		////////////////////////////////////////////////////////////////

		float getMaxScaling() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	beginFrame
		//
		////////////////////////////////////////////////////////////////

		void beginFrame(MTL::CommandBuffer* commandBuffer);

		////////////////////////////////////////////////////////////////
		//
		//	getSemaphore
		//
		////////////////////////////////////////////////////////////////

		dispatch_semaphore_t getSemaphore() const;

	private:

		MTL::Device* m_device			= nullptr;

		CRenderer_Metal* m_pRenderer	= nullptr;
		
		CShader_Metal* m_pShader		= nullptr;

		CTexture_Metal* m_pTexture		= nullptr;

		MetalVertexBufferManager2D m_vertexBufferManager;

		MetalRenderer2DCommandManager m_commandManager;

		MTL::CommandBuffer* m_commandBuffer = nullptr;

		struct EngineShader
		{
			VertexShader::IDType vsShape;
			
			VertexShader::IDType vsQuadWarp;
			
			PixelShader::IDType psShape;

			PixelShader::IDType psTexture;
						
			PixelShader::IDType psQuadWarp;
			
			PixelShader::IDType psLineDot;
			
			PixelShader::IDType psLineDash;
			
			PixelShader::IDType psLineLongDash;
			
			PixelShader::IDType psLineDashDot;
			
			PixelShader::IDType psLineRoundDot;
			
			PixelShader::IDType psPatternPolkaDot;
			
			PixelShader::IDType psPatternStripe;

			PixelShader::IDType psPatternGrid;

			PixelShader::IDType psPatternChecker;

			PixelShader::IDType psPatternTriangle;

			PixelShader::IDType psPatternHexGrid;
			
			[[nodiscard]]
			PixelShader::IDType getPatternShader(PatternType pattern) const noexcept;

		} m_engineShader;
		
		struct CurrentCustomShader
		{
			Optional<VertexShader> vs;

			Optional<PixelShader> ps;
		
		} m_currentCustomShader;
		
		ConstantBuffer<VSConstants2D> m_vsConstants;

		ConstantBuffer<PSConstants2D> m_psConstants;

		ConstantBuffer<PSEffectConstants2D> m_psEffectConstants;
		
		Vertex2DBufferPointer createBuffer(uint16 vertexCount, uint32 indexCount);
	};
}
