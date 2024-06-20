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
	class CRenderer2D_Metal final : public ISiv3DRenderer2D
	{
	public:

		~CRenderer2D_Metal() override;

		void init() override;

		void addLine(const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2]) override;

		void addTriangle(const Float2(&points)[3], const Float4& color) override;

		void addTriangle(const Float2(&points)[3], const Float4(&colors)[3]) override;

		void addRect(const FloatRect& rect, const Float4& color) override;

		void addRect(const FloatRect& rect, const Float4(&colors)[4]) override;

		void addCircle(const Float2& center, float r, const Float4& innerColor, const Float4& outerColor) override;

		void flush() override;


		Float4 getColorMul() const override;

		Float3 getColorAdd() const override;

		void setColorMul(const Float4& color) override;

		void setColorAdd(const Float3& color) override;

		BlendState getBlendState() const override;

		void setBlendState(const BlendState& state) override;

	
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

		} m_engineShader;
		
		struct CurrentCustomShader
		{
			Optional<VertexShader> vs;

			Optional<PixelShader> ps;
		
		} m_currentCustomShader;
		
		Vertex2DBufferPointer createBuffer(uint16 vertexSize, uint32 indexSize);
	};
}
