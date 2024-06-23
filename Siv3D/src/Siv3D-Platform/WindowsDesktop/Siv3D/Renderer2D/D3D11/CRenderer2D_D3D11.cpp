﻿//-----------------------------------------------
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
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/Renderer2D/Vertex2DBuilder.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/FmtOptional.hpp>
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
		static constexpr D3D11_VIEWPORT MakeViewport(const Point pos, const Size size) noexcept
		{
			return{
				.TopLeftX	= static_cast<float>(pos.x),
				.TopLeftY	= static_cast<float>(pos.y),
				.Width		= static_cast<float>(size.x),
				.Height		= static_cast<float>(size.y),
				.MinDepth	= D3D11_MIN_DEPTH,
				.MaxDepth	= D3D11_MAX_DEPTH,
			};
		}
	}

	struct CommandState
	{
		BatchInfo2D batchInfo;
		
		RasterizerState rasterizerState = RasterizerState::Default2D;
		
		Optional<Rect> scissorRect;

		Mat3x2 transform = Mat3x2::Identity();

		Mat3x2 screenMat = Mat3x2::Identity();
	};

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
	//	addLine
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addLine(const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLine(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), style, start, end, thickness, colors, getMaxScaling()))
		{
			if (not m_currentCustomShader.vs)
			{
				m_commandManager.pushEngineVS(m_engineShader.vs);
			}

			if (not m_currentCustomShader.ps)
			{
				//if (style.hasSquareDot())
				//{
				//	m_commandManager.pushEnginePS(m_engineShader.);
				//}
				//else if (style.hasRoundDot())
				//{
				//	m_commandManager.pushEnginePS(m_engineShader.);
				//}
				//else
				{
					m_commandManager.pushEnginePS(m_engineShader.psShape);
				}
			}

			m_commandManager.pushDraw(indexCount);
		}
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

	void CRenderer2D_D3D11::addTriangle(const Float2(&points)[3], const Float4(&colors)[3])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), points, colors))
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

	void CRenderer2D_D3D11::addRect(const FloatRect& rect, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), rect, colors))
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
	//	addCircle
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_D3D11::addCircle(const Float2& center, float r, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircle(std::bind_front(&CRenderer2D_D3D11::createBuffer, this), center, r, innerColor, outerColor, getMaxScaling()))
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
		m_pShader->setConstantBufferPS(0, m_psConstants._base());

		const Size currentRenderTargetSize = SIV3D_ENGINE(Renderer)->getSceneBufferSize();
		{
			const D3D11_VIEWPORT viewport = MakeViewport(Point{ 0, 0 }, currentRenderTargetSize);
			m_context->RSSetViewports(1, &viewport);
		}

		m_pRenderer->getBackBuffer().bindSceneTextureAsRenderTarget();
		m_pRenderer->getDepthStencilState().set(DepthStencilState::Default2D);

		LOG_COMMAND("----");

		CommandState commandState;
		commandState.screenMat = Mat3x2::Screen(currentRenderTargetSize);

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
					commandState.batchInfo = m_vertexBufferManager2D.commitBuffers(command.index);
					LOG_COMMAND(fmt::format("UpdateBuffers[{}] BatchInfo(indexCount = {}, startIndexLocation = {}, baseVertexLocation = {})",
						command.index, commandState.batchInfo.indexCount, commandState.batchInfo.startIndexLocation, commandState.batchInfo.baseVertexLocation));
					break;
				}
			case D3D11Renderer2DCommandType::Draw:
				{
					m_vsConstants._update_if_dirty();
					m_psConstants._update_if_dirty();

					const D3D11DrawCommand& draw = m_commandManager.getDraw(command.index);
					const uint32 indexCount = draw.indexCount;
					const uint32 startIndexLocation = commandState.batchInfo.startIndexLocation;
					const uint32 baseVertexLocation = commandState.batchInfo.baseVertexLocation;

					m_context->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
					commandState.batchInfo.startIndexLocation += indexCount;
					
					//++m_stat.drawCalls;
					//m_stat.triangleCount += (indexCount / 3);
					LOG_COMMAND(fmt::format("Draw[{}] indexCount = {}, startIndexLocation = {}", command.index, indexCount, startIndexLocation));
					break;
				}
			case D3D11Renderer2DCommandType::ColorMul:
				{
					const Float4 colorMul = m_commandManager.getColorMul(command.index);
					m_vsConstants->colorMul = colorMul;
					LOG_COMMAND(fmt::format("ColorMul[{}] {}", command.index, colorMul));
					break;
				}
			case D3D11Renderer2DCommandType::ColorAdd:
				{
					const Float3 colorAdd = m_commandManager.getColorAdd(command.index);
					m_psConstants->colorAdd.set(colorAdd, 0.0f);
					LOG_COMMAND(fmt::format("ColorAdd[{}] {}", command.index, colorAdd));
					break;
				}
			case D3D11Renderer2DCommandType::BlendState:
				{
					const auto& blendState = m_commandManager.getBlendState(command.index);
					m_pRenderer->getBlendState().set(blendState);
					LOG_COMMAND(fmt::format("BlendState[{}]", command.index));
					break;
				}
			case D3D11Renderer2DCommandType::RasterizerState:
				{
					const auto& rasterizerState = m_commandManager.getRasterizerState(command.index);
					commandState.rasterizerState = rasterizerState;

					if (commandState.scissorRect)
					{
						m_pRenderer->getRasterizerState().set(rasterizerState, true);
					}
					else
					{
						m_pRenderer->getRasterizerState().set(rasterizerState, false);
					}

					LOG_COMMAND(fmt::format("RasterizerState[{}]", command.index));
					break;
				}
			case D3D11Renderer2DCommandType::ScissorRect:
				{
					const auto& scissorRect = m_commandManager.getScissorRect(command.index);
					commandState.scissorRect = scissorRect;

					if (scissorRect)
					{
						m_pRenderer->getRasterizerState().set(commandState.rasterizerState, true);
						m_pRenderer->getRasterizerState().setScissorRect(*scissorRect);
					}
					else
					{
						m_pRenderer->getRasterizerState().set(commandState.rasterizerState, false);
					}

					LOG_COMMAND(fmt::format("ScissorRect[{}] {}", command.index, scissorRect));
					break;
				}
			case D3D11Renderer2DCommandType::Viewport:
				{
					const auto& viewport = m_commandManager.getViewport(command.index);
					
					const D3D11_VIEWPORT vp = (viewport ? MakeViewport(viewport->pos, viewport->size) : MakeViewport(Point{ 0, 0 }, currentRenderTargetSize));
					m_context->RSSetViewports(1, &vp);

					commandState.screenMat = Mat3x2::Screen(vp.Width, vp.Height);
					const Mat3x2 matrix = (commandState.transform * commandState.screenMat);
					m_vsConstants->transform[0].set(matrix._11, matrix._12, matrix._31, matrix._32);
					m_vsConstants->transform[1].set(matrix._21, matrix._22, 0.0f, 1.0f);

					LOG_COMMAND(fmt::format("Viewport[{}] ({}, {}, {}, {})", command.index, vp.TopLeftX, vp.TopLeftY, vp.Width, vp.Height));
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
			case D3D11Renderer2DCommandType::Transform:
				{
					commandState.transform = m_commandManager.getCombinedTransform(command.index);
					const Mat3x2 matrix = (commandState.transform * commandState.screenMat);
					m_vsConstants->transform[0].set(matrix._11, matrix._12, matrix._31, matrix._32);
					m_vsConstants->transform[1].set(matrix._21, matrix._22, 0.0f, 1.0f);

					LOG_COMMAND(U"Transform[{}] {}"_fmt(command.index, matrix));
					break;
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getColorMul, setColorMul
	//
	////////////////////////////////////////////////////////////////

	Float4 CRenderer2D_D3D11::getColorMul() const
	{
		return m_commandManager.getCurrentColorMul();
	}

	void CRenderer2D_D3D11::setColorMul(const Float4& color)
	{
		m_commandManager.pushColorMul(color);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getColorAdd, setColorAdd
	//
	////////////////////////////////////////////////////////////////

	Float3 CRenderer2D_D3D11::getColorAdd() const
	{
		return m_commandManager.getCurrentColorAdd();
	}

	void CRenderer2D_D3D11::setColorAdd(const Float3& color)
	{
		m_commandManager.pushColorAdd(color);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBlendState, setBlendState
	//
	////////////////////////////////////////////////////////////////

	BlendState CRenderer2D_D3D11::getBlendState() const
	{
		return m_commandManager.getCurrentBlendState();
	}

	void CRenderer2D_D3D11::setBlendState(const BlendState& state)
	{
		m_commandManager.pushBlendState(state);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getRasterizerState, setRasterizerState
	//
	////////////////////////////////////////////////////////////////

	RasterizerState CRenderer2D_D3D11::getRasterizerState() const
	{
		return m_commandManager.getCurrentRasterizerState();
	}

	void CRenderer2D_D3D11::setRasterizerState(const RasterizerState& state)
	{
		m_commandManager.pushRasterizerState(state);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getVSSamplerState, setVSSamplerState
	//
	////////////////////////////////////////////////////////////////

	SamplerState CRenderer2D_D3D11::getVSSamplerState(const uint32 slot) const
	{
		// [Siv3D ToDo]
		return SamplerState{};
	}

	void CRenderer2D_D3D11::setVSSamplerState(const uint32 slot, const SamplerState& state)
	{
		// [Siv3D ToDo]
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPSSamplerState, setPSSamplerState
	//
	////////////////////////////////////////////////////////////////

	SamplerState CRenderer2D_D3D11::getPSSamplerState(const uint32 slot) const
	{
		// [Siv3D ToDo]
		return SamplerState{};
	}

	void CRenderer2D_D3D11::setPSSamplerState(const uint32 slot, const SamplerState& state)
	{
		// [Siv3D ToDo]
	}

	////////////////////////////////////////////////////////////////
	//
	//	getScissorRect, setScissorRect
	//
	////////////////////////////////////////////////////////////////

	Optional<Rect> CRenderer2D_D3D11::getScissorRect() const
	{
		return m_commandManager.getCurrentScissorRect();
	}

	void CRenderer2D_D3D11::setScissorRect(const Optional<Rect>& rect)
	{
		m_commandManager.pushScissorRect(rect);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getViewport, setViewport
	//
	////////////////////////////////////////////////////////////////

	Optional<Rect> CRenderer2D_D3D11::getViewport() const
	{
		return m_commandManager.getCurrentViewport();
	}

	void CRenderer2D_D3D11::setViewport(const Optional<Rect>& viewport)
	{
		m_commandManager.pushViewport(viewport);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLocalTransform, setLocalTransform
	//
	////////////////////////////////////////////////////////////////

	const Mat3x2& CRenderer2D_D3D11::getLocalTransform() const
	{
		return m_commandManager.getCurrentLocalTransform();
	}

	void CRenderer2D_D3D11::setLocalTransform(const Mat3x2& matrix)
	{
		m_commandManager.pushLocalTransform(matrix);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCameraTransform, setCameraTransform
	//
	////////////////////////////////////////////////////////////////

	const Mat3x2& CRenderer2D_D3D11::getCameraTransform() const
	{
		return m_commandManager.getCurrentCameraTransform();
	}

	void CRenderer2D_D3D11::setCameraTransform(const Mat3x2& matrix)
	{
		m_commandManager.pushCameraTransform(matrix);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMaxScaling
	//
	////////////////////////////////////////////////////////////////

	float CRenderer2D_D3D11::getMaxScaling() const noexcept
	{
		return m_commandManager.getCurrentMaxScaling();
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
