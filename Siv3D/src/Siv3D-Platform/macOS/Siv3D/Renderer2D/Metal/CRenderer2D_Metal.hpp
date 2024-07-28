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
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Shader/Metal/CShader_Metal.hpp>
# include <Siv3D/Renderer2D/Renderer2DCommon.hpp>
# include "MetalVertexBufferManager2D.hpp"
# include "MetalRenderer2DCommandManager.hpp"

namespace s3d
{
	enum class PatternType : uint8;

	class CRenderer2D_Metal final : public ISiv3DRenderer2D
	{
	public:

		~CRenderer2D_Metal() override;

		void init() override;

		void addLine(LineCap startCap, LineCap endCap, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2]) override;

		void addLine(const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2]) override;

		void addTriangle(const Float2(&points)[3], const Float4& color) override;

		void addTriangle(const Float2(&points)[3], const Float4(&colors)[3]) override;

		void addTriangle(const Float2(&points)[3], const PatternParameters& pattern) override;

		void addRect(const FloatRect& rect, const Float4& color) override;

		void addRect(const FloatRect& rect, const Float4(&colors)[4]) override;

		void addRect(const FloatRect& rect, const PatternParameters& pattern) override;

		void addRectFrame(const FloatRect& innerRect, float thickness, const Float4& innerColor, const Float4& outerColor, ColorFillDirection colorType) override;

		void addRectFrame(const FloatRect& innerRect, float thickness, const PatternParameters& pattern) override;

		void addCircle(const Float2& center, float r, const Float4& color0, const Float4& color1, ColorFillDirection colorType) override;

		void addCircle(const Float2& center, float r, const PatternParameters& pattern) override;

		void addCircleFrame(const Float2& center, float rInner, float thickness, const Float4& innerColor, const Float4& outerColor) override;

		void addCircleFrame(const Float2& center, float rInner, float thickness, const PatternParameters& pattern) override;

		void addCirclePie(const Float2& center, float r, float startAngle, float angle, const Float4& innerColor, const Float4& outerColor) override;

		void addCirclePie(const Float2& center, float r, float startAngle, float angle, const PatternParameters& pattern) override;

		void addCircleArc(LineCap lineCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType) override;
		
		void addCircleArc(LineCap lineCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, const PatternParameters& pattern) override;

		void addQuad(const FloatQuad& quad, const Float4& color) override;

		void addQuad(const FloatQuad& quad, const Float4(&colors)[4]) override;

		void addQuad(const FloatQuad& quad, const PatternParameters& pattern) override;

		void flush() override;


		Float4 getColorMul() const override;

		void setColorMul(const Float4& color) override;

		Float3 getColorAdd() const override;

		void setColorAdd(const Float3& color) override;

		BlendState getBlendState() const override;

		void setBlendState(const BlendState& state) override;

		RasterizerState getRasterizerState() const override;

		void setRasterizerState(const RasterizerState& state) override;

		SamplerState getVSSamplerState(uint32 slot) const override;

		void setVSSamplerState(uint32 slot, const SamplerState& state) override;

		SamplerState getPSSamplerState(uint32 slot) const override;

		void setPSSamplerState(uint32 slot, const SamplerState& state) override;

		Optional<Rect> getScissorRect() const override;

		void setScissorRect(const Optional<Rect>& rect) override;

		Optional<Rect> getViewport() const override;

		void setViewport(const Optional<Rect>& viewport) override;

		const Mat3x2& getLocalTransform() const override;

		void setLocalTransform(const Mat3x2& matrix) override;

		const Mat3x2& getCameraTransform() const override;

		void setCameraTransform(const Mat3x2& matrix) override;

		float getMaxScaling() const noexcept override;


		void beginFrame(MTL::CommandBuffer* commandBuffer);

		dispatch_semaphore_t getSemaphore() const;

	private:

		MTL::Device* m_device			= nullptr;

		CRenderer_Metal* m_pRenderer	= nullptr;
		
		CShader_Metal* m_pShader		= nullptr;

		MetalVertexBufferManager2D m_vertexBufferManager;

		MetalRenderer2DCommandManager m_commandManager;

		MTL::CommandBuffer* m_commandBuffer = nullptr;

		struct EngineShader
		{
			VertexShader::IDType vs;

			PixelShader::IDType psShape;
			
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

		ConstantBuffer<PSPatternConstants2D> m_psPatternConstants;
		
		Vertex2DBufferPointer createBuffer(uint16 vertexCount, uint32 indexCount);
	};
}
