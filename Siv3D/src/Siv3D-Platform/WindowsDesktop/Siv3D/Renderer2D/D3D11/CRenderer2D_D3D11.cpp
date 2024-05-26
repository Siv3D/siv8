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

# include "CRenderer2D_D3D11.hpp"
# include <Siv3D/Blob.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/Mat3x2.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	CRenderer2D_D3D11::~CRenderer2D_D3D11()
	{
		LOG_SCOPED_DEBUG("CRenderer2D_D3D11::~CRenderer2D_D3D11()");
	}

	void CRenderer2D_D3D11::init()
	{
		LOG_SCOPED_DEBUG("CRenderer2D_D3D11::init()");

		// 各種ポインタを保存
		{
			m_pRenderer = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer));
			//m_pShader = static_cast<CShader_D3D11*>(SIV3D_ENGINE(Shader));
			//m_pTexture = static_cast<CTexture_D3D11*>(SIV3D_ENGINE(Texture));
			m_device	= m_pRenderer->getDevice().getDevice();
			m_context	= m_pRenderer->getDevice().getContext();
		}
		
		// 標準 InputLayout を作成
		{
			const D3D11_INPUT_ELEMENT_DESC layout[3] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			{
				const Blob& bytecode = SIV3D_ENGINE(EngineShader)->getVS(EngineVS::TestTriangle).getBytecode();

				if (FAILED(m_device->CreateInputLayout(layout, 3, bytecode.data(), bytecode.size(), &m_inputLayout)))
				{
					throw InternalEngineError{ "ID3D11Device::CreateInputLayout() failed" };
				}
			}

			if (not m_vertexBufferManager2D.init(m_device, m_context))
			{
				throw InternalEngineError{ "D3D11VertexBufferManager2D::init() failed" };
			}
		}
	}

	void CRenderer2D_D3D11::beginFrame()
	{

	}

	void CRenderer2D_D3D11::addTriangle(const Float2(&points)[3], const Float4& color)
	{

	}

	void CRenderer2D_D3D11::flush()
	{
		ScopeExit cleanUp = [this]()
		{
			m_vertexBufferManager2D.reset();
			//m_commandManager.reset();
			//m_currentCustomVS.reset();
			//m_currentCustomPS.reset();
		};

		//m_commandManager.flush();

		m_context->IASetInputLayout(m_inputLayout.Get());
		//pShader->setConstantBufferVS(0, m_vsConstants2D.base());
		//pShader->setConstantBufferPS(0, m_psConstants2D.base());

		const Size currentRenderTargetSize = SIV3D_ENGINE(Renderer)->getSceneBufferSize();

		{
			const D3D11_VIEWPORT viewport
			{
				.TopLeftX	= 0.0f,
				.TopLeftY	= 0.0f,
				.Width		= static_cast<float>(currentRenderTargetSize.x),
				.Height		= static_cast<float>(currentRenderTargetSize.y),
				.MinDepth	= D3D11_MIN_DEPTH,
				.MaxDepth	= D3D11_MAX_DEPTH,
			};
			m_context->RSSetViewports(1, &viewport);
		}

		Mat3x2 transform = Mat3x2::Identity();
		Mat3x2 screenMat = Mat3x2::Screen(currentRenderTargetSize);

		//m_pRenderer->getBackBuffer().bindSceneToContext();
		//m_pRenderer->getDepthStencilState().set(DepthStencilState::Default2D);

		BatchInfo2D batchInfo;
	}
}
