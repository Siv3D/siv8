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
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Shader/Metal/CShader_Metal.hpp>
# include <Siv3D/Renderer2D/Renderer2DCommon.hpp>
# include "MetalVertexBufferManager2D.hpp"

namespace s3d
{
	class CRenderer2D_Metal final : public ISiv3DRenderer2D
	{
	public:

		~CRenderer2D_Metal() override;

		void init() override;

		void beginFrame() override;

		void addTriangle(const Float2(&points)[3], const Float4& color) override;

		void addRect(const FloatRect& rect, const Float4& color) override;

		void flush() override;

		void flush(MTL::CommandBuffer* commandBuffer);

		dispatch_semaphore_t getSemaphore() const;

	private:

		MTL::Device* m_device			= nullptr;

		CRenderer_Metal* m_pRenderer	= nullptr;
		
		CShader_Metal* m_pShader		= nullptr;

		MetalVertexBufferManager2D m_vertexBufferManager;

		struct EngineShader
		{
			VertexShader::IDType vs;

			PixelShader::IDType psShape;

		} m_engineShader;
		
		Vertex2DBufferPointer createBuffer(uint16 vertexSize, uint32 indexSize);
	};
}
