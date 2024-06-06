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
	class CRenderer2D_D3D11 final : public ISiv3DRenderer2D
	{
	public:

		~CRenderer2D_D3D11() override;

		void init() override;

		void addTriangle(const Float2(&points)[3], const Float4& color) override;

		void addRect(const FloatRect& rect, const Float4& color) override;

		void flush() override;

		void beginFrame();

	private:

		CRenderer_D3D11* m_pRenderer	= nullptr;
		
		CShader_D3D11* m_pShader		= nullptr;
		
		//CTexture_D3D11* m_pTexture	= nullptr;

		ID3D11Device* m_device			= nullptr;

		ID3D11DeviceContext* m_context	= nullptr;

		ComPtr<ID3D11InputLayout> m_inputLayout;

		D3D11VertexBufferManager2D m_vertexBufferManager2D;

		D3D11Renderer2DCommandManager m_commandManager;

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

		ConstantBuffer<VSConstants2D> m_vsConstants;

		Vertex2DBufferPointer createBuffer(uint16 vertexSize, uint32 indexSize);
	};
}
