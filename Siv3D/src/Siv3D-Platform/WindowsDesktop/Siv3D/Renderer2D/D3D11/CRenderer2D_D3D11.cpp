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
# include <Siv3D/Renderer2D/Vertex2DBuilder.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

/*
#	define LOG_COMMAND(...) LOG_TRACE(__VA_ARGS__)
/*/
#	define LOG_COMMAND(...) ((void)0)
//*/

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr D3D11_VIEWPORT MakeViewport(const Size size) noexcept
		{
			return{
				.TopLeftX	= 0.0f,
				.TopLeftY	= 0.0f,
				.Width		= static_cast<float>(size.x),
				.Height		= static_cast<float>(size.y),
				.MinDepth	= D3D11_MIN_DEPTH,
				.MaxDepth	= D3D11_MAX_DEPTH,
			};
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CRenderer2D_D3D11::~CRenderer2D_D3D11()
	{
		LOG_SCOPED_DEBUG("CRenderer2D_D3D11::~CRenderer2D_D3D11()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::init()
	{
		LOG_SCOPED_DEBUG("CRenderer2D_D3D11::init()");

		// 各種ポインタを保存
		{
			m_pRenderer	= static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer));
			m_pShader	= static_cast<CShader_D3D11*>(SIV3D_ENGINE(Shader));
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
				const Blob& bytecode = SIV3D_ENGINE(EngineShader)->getVS(EngineVS::Shape2D).getBytecode();

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

		m_engineShader.vs = SIV3D_ENGINE(EngineShader)->getVS(EngineVS::Shape2D).id();
		m_engineShader.psShape = SIV3D_ENGINE(EngineShader)->getPS(EnginePS::Shape2D).id();
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTriangle
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addTriangle(const Float2(&points)[3], const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), points, color))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRect
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addRect(const FloatRect& rect, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, color))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				m_commandManager.pushEnginePS(m_engineShader.psShape);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	flush
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::flush()
	{
		ScopeExit cleanUp = [this]()
		{
			m_vertexBufferManager2D.reset();
			m_commandManager.reset();
			m_currentCustomShader.vs.reset();
			m_currentCustomShader.ps.reset();
		};

		m_commandManager.flush();
		m_context->IASetInputLayout(m_inputLayout.Get());
		m_pShader->setConstantBufferVS(0, m_vsConstants._base());
		//pShader->setConstantBufferPS(0, m_psConstants2D.base());

		const Size currentRenderTargetSize = SIV3D_ENGINE(Renderer)->getSceneBufferSize();
		{
			const D3D11_VIEWPORT viewport = MakeViewport(currentRenderTargetSize);
			m_context->RSSetViewports(1, &viewport);
		}

		Mat3x2 transform = Mat3x2::Identity();
		Mat3x2 screenMat = Mat3x2::Screen(currentRenderTargetSize);

		m_pRenderer->getBackBuffer().bindSceneTextureAsRenderTarget();
		m_pRenderer->getDepthStencilState().set(DepthStencilState::Default2D);

		LOG_COMMAND("----");

		// (仮)
		{
			m_vsConstants->transform[0].set(screenMat._11, screenMat._12, screenMat._31, screenMat._32);
			m_vsConstants->transform[1].set(screenMat._21, screenMat._22, 0.0f, 1.0f);
		}

		BatchInfo2D batchInfo;

		for (const auto& command : m_commandManager.getCommands())
		{
			switch (command.type)
			{
			case D3D11Renderer2DCommandType::Null:
				{
					LOG_COMMAND("Null");
					break;
				}
			case D3D11Renderer2DCommandType::SetBuffers:
				{
					m_vertexBufferManager2D.setBuffers();
					LOG_COMMAND(fmt::format("SetBuffers[{}]", command.index));
					break;
				}
			case D3D11Renderer2DCommandType::UpdateBuffers:
				{
					batchInfo = m_vertexBufferManager2D.commitBuffers(command.index);				
					LOG_COMMAND(fmt::format("UpdateBuffers[{}] BatchInfo(indexCount = {}, startIndexLocation = {}, baseVertexLocation = {})",
						command.index, batchInfo.indexCount, batchInfo.startIndexLocation, batchInfo.baseVertexLocation));
					break;
				}
			case D3D11Renderer2DCommandType::Draw:
				{
					m_vsConstants._update_if_dirty();
					//m_psConstants2D._update_if_dirty();

					const D3D11DrawCommand& draw = m_commandManager.getDraw(command.index);
					const uint32 indexCount = draw.indexCount;
					const uint32 startIndexLocation = batchInfo.startIndexLocation;
					const uint32 baseVertexLocation = batchInfo.baseVertexLocation;

					m_context->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
					batchInfo.startIndexLocation += indexCount;
					
					//++m_stat.drawCalls;
					//m_stat.triangleCount += (indexCount / 3);
					LOG_COMMAND(fmt::format("Draw[{}] indexCount = {}, startIndexLocation = {}", command.index, indexCount, startIndexLocation));
					break;
				}
			case D3D11Renderer2DCommandType::SetVS:
				{
					const auto vsID = m_commandManager.getVS(command.index);

					if (vsID == VertexShader::IDType::Invalid())
					{
						m_pShader->setVSNull();
						LOG_COMMAND(fmt::format("SetVS[{}]: null", command.index));
					}
					else
					{
						m_pShader->setVS(vsID);
						LOG_COMMAND(fmt::format("SetVS[{}]: {}", command.index, vsID.value()));
					}

					break;
				}
			case D3D11Renderer2DCommandType::SetPS:
				{
					const auto psID = m_commandManager.getPS(command.index);

					if (psID == PixelShader::IDType::Invalid())
					{
						m_pShader->setPSNull();
						LOG_COMMAND(fmt::format("SetPS[{}]: null", command.index));
					}
					else
					{
						m_pShader->setPS(psID);
						LOG_COMMAND(fmt::format("SetPS[{}]: {}", command.index, psID.value()));
					}

					break;
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::beginFrame()
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	Vertex2DBufferPointer CRenderer2D_D3D11::createBuffer(const uint16 vertexSize, const uint32 indexSize)
	{
		return m_vertexBufferManager2D.requestBuffer(vertexSize, indexSize, m_commandManager);
	}
}
