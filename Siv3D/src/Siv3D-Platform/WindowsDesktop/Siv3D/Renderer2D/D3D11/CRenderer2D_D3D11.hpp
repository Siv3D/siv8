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
# include <Siv3D/ConstantBuffer.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# include <Siv3D/Shader/D3D11/CShader_D3D11.hpp>
# include <Siv3D/Renderer2D/Renderer2DCommon.hpp>
# include "D3D11VertexBufferManager2D.hpp"
# include "D3D11Renderer2DCommandManager.hpp"

namespace s3d
{
	enum class PatternType : uint8;

	class CRenderer2D_D3D11 final : public ISiv3DRenderer2D
	{
	public:

		~CRenderer2D_D3D11() override;

		void init() override;

		void addLine(LineCap startCap, LineCap endCap, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2]) override;

		void addLine(const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2]) override;

		void addTriangle(const Float2(&points)[3], const Float4& color) override;

		void addTriangle(const Float2(&points)[3], const Float4(&colors)[3]) override;

		void addTriangle(const Float2(&points)[3], const PatternParameters& pattern) override;

		void addRect(const FloatRect& rect, const Float4& color) override;

		void addRect(const FloatRect& rect, const Float4(&colors)[4]) override;

		void addRect(const FloatRect& rect, const PatternParameters& pattern) override;

		void addRectFrame(const FloatRect& innerRect, float thickness, const Float4& innerColor, const Float4& outerColor, ColorFillDirection colorType);

		void addRectFrame(const FloatRect& innerRect, float thickness, const PatternParameters& pattern) override;

		void addCircle(const Float2& center, float r, const Float4& color0, const Float4& color1, ColorFillDirection colorType) override;

		void addCircle(const Float2& center, float r, const PatternParameters& pattern) override;

		void addCircleFrame(const Float2& center, float rInner, float thickness, const Float4& innerColor, const Float4& outerColor) override;

		void addCircleFrame(const Float2& center, float rInner, float thickness, const PatternParameters& pattern) override;

		void addCirclePie(const Float2& center, float r, float startAngle, float angle, const Float4& innerColor, const Float4& outerColor) override;

		void addCirclePie(const Float2& center, float r, float startAngle, float angle, const PatternParameters& pattern) override;

		void addCircleArc(LineCap lineCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, const Float4& color0, const Float4& color1, ColorFillDirection colorType) override;

		void addCircleArc(LineCap lineCap, const Float2& center, float rInner, float startAngle, float angle, float thickness, const PatternParameters& pattern) override;

		void addCircleSegment(const Float2& center, float r, float startAngle, float angle, const Float4& color) override;

		void addCircleSegment(const Float2& center, float r, float startAngle, float angle, const PatternParameters& pattern) override;

		void addEllipse(const Float2& center, float a, float b, const Float4& color0, const Float4& color1, ColorFillDirection colorType) override;

		void addEllipse(const Float2& center, float a, float b, const PatternParameters& pattern) override;

		void addEllipseFrame(const Float2& center, float aInner, float bInner, float thickness, const Float4& innerColor, const Float4& outerColor) override;

		void addEllipseFrame(const Float2& center, float aInner, float bInner, float thickness, const PatternParameters& pattern) override;

		void addQuad(const FloatQuad& quad, const Float4& color) override;

		void addQuad(const FloatQuad& quad, const Float4(&colors)[4]) override;

		void addQuad(const FloatQuad& quad, const PatternParameters& pattern) override;

		void addRoundRect(const FloatRect& rect, float r, const Float4& color) override;

		void addRoundRect(const FloatRect& rect, float r, const Float4& color0, const Float4& color1, ColorFillDirection colorType) override;

		void addRoundRect(const FloatRect& rect, float r, const PatternParameters& pattern) override;

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


		void beginFrame();

	private:

		CRenderer_D3D11* m_pRenderer = nullptr;

		CShader_D3D11* m_pShader = nullptr;

		//CTexture_D3D11* m_pTexture	= nullptr;

		ID3D11Device* m_device = nullptr;

		ID3D11DeviceContext* m_context = nullptr;

		ComPtr<ID3D11InputLayout> m_inputLayout;

		D3D11VertexBufferManager2D m_vertexBufferManager2D;

		D3D11Renderer2DCommandManager m_commandManager;

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
