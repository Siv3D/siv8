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

# include "CRenderer2D_Metal.hpp"
# include <Siv3D/Blob.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/Mat3x2.hpp>
# include <Siv3D/Vertex2D.hpp>
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
		static constexpr MTL::ScissorRect MakeScissorRect(const Rect& rect) noexcept
		{
			return{
				.x		= static_cast<uint32>(rect.x),
				.y		= static_cast<uint32>(rect.y),
				.width	= static_cast<uint32>(rect.w),
				.height	= static_cast<uint32>(rect.h)
			};
		}
	
		[[nodiscard]]
		static constexpr MTL::Viewport MakeViewport(const Point pos, const Size size) noexcept
		{
			return{
				.originX	= static_cast<double>(pos.x),
				.originY	= static_cast<double>(pos.y),
				.width		= static_cast<double>(size.x),
				.height		= static_cast<double>(size.y),
				.znear		= 0.0,
				.zfar		= 1.0
			};
		}
	}

	struct CommandState
	{
		Mat3x2 transform = Mat3x2::Identity();

		Mat3x2 screenMat = Mat3x2::Identity();
		
		uint32 startIndexLocation = 0;
	};

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CRenderer2D_Metal::~CRenderer2D_Metal()
	{
		LOG_SCOPED_DEBUG("CRenderer2D_Metal::~CRenderer2D_Metal()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::init()
	{
		LOG_SCOPED_DEBUG("CRenderer2D_Metal::init()");

		// 各種ポインタを保存
		{
			m_pRenderer	= static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
			m_pShader	= static_cast<CShader_Metal*>(SIV3D_ENGINE(Shader));
			m_device	= m_pRenderer->getDevice();
		}

		m_engineShader.vs		= SIV3D_ENGINE(EngineShader)->getVS(EngineVS::Shape2D).id();
		m_engineShader.psShape	= SIV3D_ENGINE(EngineShader)->getPS(EnginePS::Shape2D).id();
		
		m_vertexBufferManager.init(m_device);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addLine
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addLine(const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildLine(std::bind_front(&CRenderer2D_Metal::createBuffer, this), style, start, end, thickness, colors, getMaxScaling()))
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

	void CRenderer2D_Metal::addTriangle(const Float2(&points)[3], const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), points, color))
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

	void CRenderer2D_Metal::addTriangle(const Float2(&points)[3], const Float4(&colors)[3])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), points, colors))
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

	void CRenderer2D_Metal::addRect(const FloatRect& rect, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, color))
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

	void CRenderer2D_Metal::addRect(const FloatRect& rect, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, colors))
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
	//	addRectFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addRectFrame(const FloatRect& innerRect, const float thickness, const Float4& color0, const Float4& color1, const RectFrameColorType colorType)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRectFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), innerRect, thickness, colorType, color0, color1))
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

	void CRenderer2D_Metal::addCircle(const Float2& center, float r, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, r, innerColor, outerColor, getMaxScaling()))
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
	//	addCircleFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addCircleFrame(const Float2& center, const float rInner, const float thickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleFrame(std::bind_front(&CRenderer2D_Metal::createBuffer, this), center, rInner, thickness, innerColor, outerColor, getMaxScaling()))
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
	//	addQuad
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addQuad(const FloatQuad& quad, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(std::bind_front(&CRenderer2D_Metal::createBuffer, this), quad, color))
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

	void CRenderer2D_Metal::addQuad(const FloatQuad& quad, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(std::bind_front(&CRenderer2D_Metal::createBuffer, this), quad, colors))
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

	void CRenderer2D_Metal::flush()
	{
		ScopeExit cleanUp = [this]()
		{
			m_commandManager.reset();
			m_currentCustomShader.vs.reset();
			m_currentCustomShader.ps.reset();
		};
		
		m_commandManager.flush();
		
		const Size currentRenderTargetSize = SIV3D_ENGINE(Renderer)->getSceneBufferSize();

		// Draw2D
		NS::SharedPtr<MTL::RenderPassDescriptor> offscreenRenderPassDescriptor = NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init());
		MTL::RenderPassColorAttachmentDescriptor* cd = offscreenRenderPassDescriptor->colorAttachments()->object(0);
		
		if (m_pRenderer->getSceneSampleCount() == 1)
		{
			cd->setTexture(m_pRenderer->getSceneTextureNonMSAA().getTexture());
		}
		else
		{
			cd->setTexture(m_pRenderer->getSceneTextureMSAA().getTexture());
			cd->setResolveTexture(m_pRenderer->getSceneTextureNonMSAA().getTexture());
		}

		cd->setLoadAction(MTL::LoadActionClear);
		const ColorF& backgroundColor = m_pRenderer->getSceneStyle().backgroundColor;
		cd->setClearColor(MTL::ClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1));
		cd->setStoreAction((m_pRenderer->getSceneSampleCount() == 1) ? MTL::StoreActionStore : MTL::StoreActionMultisampleResolve);
		
		LOG_COMMAND("----");

		@autoreleasepool
		{
			MTL::RenderCommandEncoder* renderCommandEncoder = m_commandBuffer->renderCommandEncoder(offscreenRenderPassDescriptor.get());
			
			PipelineStateDesc pipelineStateDesc
			{
				.vs = m_engineShader.vs,
				.ps = m_engineShader.psShape,
				.pixelFormat = static_cast<uint16>(MTL::PixelFormatRGBA8Unorm),
				.sampleCount = static_cast<uint16>(m_pRenderer->getSceneSampleCount()),
				.blendState = BlendState::Default2D,
			};
			
			CommandState commandState;
			commandState.screenMat = Mat3x2::Screen(currentRenderTargetSize);
			
			renderCommandEncoder->setVertexBuffer(m_vertexBufferManager.getVertexBuffer(), 0, 0);
			
			for (const auto& command : m_commandManager.getCommands())
			{
				switch (command.type)
				{
				case MetalRenderer2DCommandType::Null:
					{
						LOG_COMMAND("Null");
						break;
					}
				case MetalRenderer2DCommandType::Draw:
					{
						const auto pipeline = m_pRenderer->getRenderPipelineState().get(pipelineStateDesc);
						renderCommandEncoder->setRenderPipelineState(pipeline);
						
						if (m_vsConstants.isDirty())
						{
							m_vsConstants._update_if_dirty();
							renderCommandEncoder->setVertexBytes(m_vsConstants.data(), m_vsConstants.size(), 1);
						}

						if (m_psConstants.isDirty())
						{
							m_psConstants._update_if_dirty();
							renderCommandEncoder->setFragmentBytes(m_psConstants.data(), m_psConstants.size(), 1);
						}

						const MetalDrawCommand& draw = m_commandManager.getDraw(command.index);
						const uint32 indexCount = draw.indexCount;

						// indexBufferOffset, 4 の倍数でなくても大丈夫？
						renderCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, indexCount, MTL::IndexTypeUInt16, m_vertexBufferManager.getIndexBuffer(), (sizeof(Vertex2D::IndexType) * commandState.startIndexLocation));
						commandState.startIndexLocation += indexCount;
						
						//++m_stat.drawCalls;
						//m_stat.triangleCount += (indexCount / 3);
						LOG_COMMAND(fmt::format("Draw[{}] indexCount = {}, startIndexLocation = {}", command.index, indexCount, commandState.startIndexLocation));
						break;
					}
				case MetalRenderer2DCommandType::ColorMul:
					{
						const Float4 colorMul = m_commandManager.getColorMul(command.index);
						m_vsConstants->colorMul = colorMul;
						LOG_COMMAND(fmt::format("ColorMul[{}] {}", command.index, colorMul));
						break;
					}
				case MetalRenderer2DCommandType::ColorAdd:
					{
						const Float3 colorAdd = m_commandManager.getColorAdd(command.index);
						m_psConstants->colorAdd.set(colorAdd, 0.0f);
						LOG_COMMAND(fmt::format("ColorAdd[{}] {}", command.index, colorAdd));
						break;
					}
				case MetalRenderer2DCommandType::BlendState:
					{
						pipelineStateDesc.blendState = m_commandManager.getBlendState(command.index);
						LOG_COMMAND(fmt::format("BlendState[{}]", command.index));
						break;
					}
				case MetalRenderer2DCommandType::RasterizerState:
					{
						const auto& rasterizerState = m_commandManager.getRasterizerState(command.index);
						
						if (rasterizerState.triangleFillMode == TriangleFillMode::Solid)
						{
							renderCommandEncoder->setTriangleFillMode(MTL::TriangleFillModeFill);
						}
						else
						{
							renderCommandEncoder->setTriangleFillMode(MTL::TriangleFillModeLines);
						}
						
						if (rasterizerState.cullMode == CullMode::None)
						{
							renderCommandEncoder->setCullMode(MTL::CullModeNone);
						}
						else if (rasterizerState.cullMode == CullMode::Front)
						{
							renderCommandEncoder->setCullMode(MTL::CullModeFront);
						}
						else
						{
							renderCommandEncoder->setCullMode(MTL::CullModeBack);
						}

						LOG_COMMAND(fmt::format("RasterizerState[{}]", command.index));
						break;
					}
				case MetalRenderer2DCommandType::ScissorRect:
					{
						const auto& scissorRect = m_commandManager.getScissorRect(command.index);
						const Rect rect = (scissorRect ? scissorRect->clamped(Rect{ 0, 0, currentRenderTargetSize }) : Rect{ currentRenderTargetSize });
						renderCommandEncoder->setScissorRect(MakeScissorRect(rect));
						
						LOG_COMMAND(fmt::format("ScissorRect[{}] {}", command.index, rect));
						break;
					}
				case MetalRenderer2DCommandType::Viewport:
					{
						const auto& viewport = m_commandManager.getViewport(command.index);
						
						const MTL::Viewport vp = (viewport ? MakeViewport(viewport->pos, viewport->size) : MakeViewport(Point{ 0, 0 }, currentRenderTargetSize));
						renderCommandEncoder->setViewport(vp);
	
						commandState.screenMat = Mat3x2::Screen(vp.width, vp.height);
						const Mat3x2 matrix = (commandState.transform * commandState.screenMat);
						m_vsConstants->transform[0].set(matrix._11, matrix._12, matrix._31, matrix._32);
						m_vsConstants->transform[1].set(matrix._21, matrix._22, 0.0f, 1.0f);
						
						LOG_COMMAND(fmt::format("Viewport[{}] ({}, {}, {}, {})", command.index, vp.originX, vp.originY, vp.width, vp.height));
						break;
					}
				case MetalRenderer2DCommandType::SetVS:
					{
						const auto vsID = m_commandManager.getVS(command.index);

						if (vsID == VertexShader::IDType::Invalid())
						{
							//m_pShader->setVSNull();
							LOG_COMMAND(fmt::format("SetVS[{}]: null", command.index));
						}
						else
						{
							pipelineStateDesc.vs = vsID;
							LOG_COMMAND(fmt::format("SetVS[{}]: {}", command.index, vsID.value()));
						}

						break;
					}
				case MetalRenderer2DCommandType::SetPS:
					{
						const auto psID = m_commandManager.getPS(command.index);

						if (psID == PixelShader::IDType::Invalid())
						{
							//m_pShader->setPSNull();
							LOG_COMMAND(fmt::format("SetPS[{}]: null", command.index));
						}
						else
						{
							pipelineStateDesc.ps = psID;
							LOG_COMMAND(fmt::format("SetPS[{}]: {}", command.index, psID.value()));
						}

						break;
					}
				case MetalRenderer2DCommandType::Transform:
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
						
			renderCommandEncoder->endEncoding();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getColorMul, setColorMul
	//
	////////////////////////////////////////////////////////////////

	Float4 CRenderer2D_Metal::getColorMul() const
	{
		return m_commandManager.getCurrentColorMul();
	}

	void CRenderer2D_Metal::setColorMul(const Float4& color)
	{
		m_commandManager.pushColorMul(color);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getColorAdd, setColorAdd
	//
	////////////////////////////////////////////////////////////////

	Float3 CRenderer2D_Metal::getColorAdd() const
	{
		return m_commandManager.getCurrentColorAdd();
	}

	void CRenderer2D_Metal::setColorAdd(const Float3& color)
	{
		m_commandManager.pushColorAdd(color);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBlendState, setBlendState
	//
	////////////////////////////////////////////////////////////////

	BlendState CRenderer2D_Metal::getBlendState() const
	{
		return m_commandManager.getCurrentBlendState();
	}

	void CRenderer2D_Metal::setBlendState(const BlendState& state)
	{
		m_commandManager.pushBlendState(state);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getRasterizerState, setRasterizerState
	//
	////////////////////////////////////////////////////////////////

	RasterizerState CRenderer2D_Metal::getRasterizerState() const
	{
		return m_commandManager.getCurrentRasterizerState();
	}

	void CRenderer2D_Metal::setRasterizerState(const RasterizerState& state)
	{
		m_commandManager.pushRasterizerState(state);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getVSSamplerState, setVSSamplerState
	//
	////////////////////////////////////////////////////////////////

	SamplerState CRenderer2D_Metal::getVSSamplerState(const uint32 slot) const
	{
		// [Siv3D ToDo]
		return SamplerState{};
	}

	void CRenderer2D_Metal::setVSSamplerState(const uint32 slot, const SamplerState& state)
	{
		// [Siv3D ToDo]
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPSSamplerState, setPSSamplerState
	//
	////////////////////////////////////////////////////////////////

	SamplerState CRenderer2D_Metal::getPSSamplerState(const uint32 slot) const
	{
		// [Siv3D ToDo]
		return SamplerState{};
	}

	void CRenderer2D_Metal::setPSSamplerState(const uint32 slot, const SamplerState& state)
	{
		// [Siv3D ToDo]
	}

	////////////////////////////////////////////////////////////////
	//
	//	getScissorRect, setScissorRect
	//
	////////////////////////////////////////////////////////////////

	Optional<Rect> CRenderer2D_Metal::getScissorRect() const
	{
		return m_commandManager.getCurrentScissorRect();
	}

	void CRenderer2D_Metal::setScissorRect(const Optional<Rect>& rect)
	{
		m_commandManager.pushScissorRect(rect);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getViewport, setViewport
	//
	////////////////////////////////////////////////////////////////

	Optional<Rect> CRenderer2D_Metal::getViewport() const
	{
		return m_commandManager.getCurrentViewport();
	}

	void CRenderer2D_Metal::setViewport(const Optional<Rect>& viewport)
	{
		m_commandManager.pushViewport(viewport);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLocalTransform, setLocalTransform
	//
	////////////////////////////////////////////////////////////////

	const Mat3x2& CRenderer2D_Metal::getLocalTransform() const
	{
		return m_commandManager.getCurrentLocalTransform();
	}

	void CRenderer2D_Metal::setLocalTransform(const Mat3x2& matrix)
	{
		m_commandManager.pushLocalTransform(matrix);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCameraTransform, setCameraTransform
	//
	////////////////////////////////////////////////////////////////

	const Mat3x2& CRenderer2D_Metal::getCameraTransform() const
	{
		return m_commandManager.getCurrentCameraTransform();
	}

	void CRenderer2D_Metal::setCameraTransform(const Mat3x2& matrix)
	{
		m_commandManager.pushCameraTransform(matrix);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMaxScaling
	//
	////////////////////////////////////////////////////////////////

	float CRenderer2D_Metal::getMaxScaling() const noexcept
	{
		return m_commandManager.getCurrentMaxScaling();
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::beginFrame(MTL::CommandBuffer* commandBuffer)
	{
		m_commandBuffer = commandBuffer;

		m_vertexBufferManager.updateContent();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSemaphore
	//
	////////////////////////////////////////////////////////////////

	dispatch_semaphore_t CRenderer2D_Metal::getSemaphore() const
	{
		return m_vertexBufferManager.getSemaphore();
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	Vertex2DBufferPointer CRenderer2D_Metal::createBuffer(const uint16 vertexSize, const uint32 indexSize)
	{
		return m_vertexBufferManager.requestBuffer(vertexSize, indexSize);
	}
}
