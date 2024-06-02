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
# include <Siv3D/Renderer2D/Vertex2DBuilder.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

///*
#	define LOG_COMMAND(...) LOG_TRACE(__VA_ARGS__)
/*/
#	define LOG_COMMAND(...) ((void)0)
//*/

namespace s3d
{
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

		//m_engineShader.vs = SIV3D_ENGINE(EngineShader)->getVS(EngineVS::TestTriangle).id();
		//m_engineShader.psShape = SIV3D_ENGINE(EngineShader)->getPS(EnginePS::TestTriangle).id();

		for (int32 i = 0; i < 3; ++i)
		{
			simd::float3 triangleVertices[] = {
				{-0.5f, -0.5f, 0.0f},
				{ 0.5f, -0.5f, 0.0f},
				{ (-0.01f + i * 0.01f),  0.5f, 0.0f}
			};
			
			m_vertexBufferManager.vertexBuffers[i] = NS::TransferPtr(m_device->newBuffer(&triangleVertices,
															  sizeof(triangleVertices),
															  MTL::ResourceStorageModeShared));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::beginFrame()
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	addTriangle
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addTriangle(const Float2(&points)[3], const Float4& color)
	{
		/*
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
		*/
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRect
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::addRect(const FloatRect& rect, const Float4& color)
	{
		/*
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
		*/
	}

	////////////////////////////////////////////////////////////////
	//
	//	flush
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::flush()
	{

	}

	void CRenderer2D_Metal::flush(MTL::CommandBuffer* commandBuffer)
	{
		m_vertexBufferManager.updateContent();

		if (m_pRenderer->getSceneSampleCount() == 1)
		{
			if (not m_pipeLineTestNoAA)
			{
				const VertexShader& vs = SIV3D_ENGINE(EngineShader)->getVS(EngineVS::TestTriangle);
				const PixelShader& ps = SIV3D_ENGINE(EngineShader)->getPS(EnginePS::TestTriangle);
				
				NS::SharedPtr<MTL::RenderPipelineDescriptor> renderPipelineDescriptor = NS::TransferPtr(MTL::RenderPipelineDescriptor::alloc()->init());
				renderPipelineDescriptor->setLabel(NS::String::string("Off-screen Rendering Pipeline", NS::ASCIIStringEncoding));
				renderPipelineDescriptor->setVertexFunction(m_pShader->getShaderVS(vs.id()));
				renderPipelineDescriptor->setFragmentFunction(m_pShader->getShaderPS(ps.id()));
				renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
		
				NS::Error* error;
				m_pipeLineTestNoAA = NS::TransferPtr(m_device->newRenderPipelineState(renderPipelineDescriptor.get(), &error));
			}
		}
		else
		{
			if (not m_pipeLineTestMSAAx4)
			{
				const VertexShader& vs = SIV3D_ENGINE(EngineShader)->getVS(EngineVS::TestTriangle);
				const PixelShader& ps = SIV3D_ENGINE(EngineShader)->getPS(EnginePS::TestTriangle);
				
				NS::SharedPtr<MTL::RenderPipelineDescriptor> renderPipelineDescriptor = NS::TransferPtr(MTL::RenderPipelineDescriptor::alloc()->init());
				renderPipelineDescriptor->setLabel(NS::String::string("Off-screen Rendering Pipeline", NS::ASCIIStringEncoding));
				renderPipelineDescriptor->setVertexFunction(m_pShader->getShaderVS(vs.id()));
				renderPipelineDescriptor->setFragmentFunction(m_pShader->getShaderPS(ps.id()));
				renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
				renderPipelineDescriptor->setSampleCount(4);
				
				NS::Error* error;
				m_pipeLineTestMSAAx4 = NS::TransferPtr(m_device->newRenderPipelineState(renderPipelineDescriptor.get(), &error));
			}
		}

		// Draw2D
		@autoreleasepool
		{
			{
				NS::SharedPtr<MTL::RenderPassDescriptor> offscreenRenderPassDescriptor = NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init());
				MTL::RenderPassColorAttachmentDescriptor* cd = offscreenRenderPassDescriptor->colorAttachments()->object(0);
				
				if (m_pRenderer->getSceneSampleCount() == 1)
				{
					cd->setTexture(m_pRenderer->getSceneTextureNonMSAA().getTexture());
					//cd->setTexture(m_sceneBuffers.nonMSAA.getTexture());
				}
				else
				{
					cd->setTexture(m_pRenderer->getSceneTextureMSAA().getTexture());
					cd->setResolveTexture(m_pRenderer->getSceneTextureNonMSAA().getTexture());
					
					//cd->setTexture(m_sceneBuffers.msaa.getTexture());
					//cd->setResolveTexture(m_sceneBuffers.nonMSAA.getTexture());
				}

				cd->setLoadAction(MTL::LoadActionClear);
				const ColorF& backgroundColor = m_pRenderer->getSceneStyle().backgroundColor;
				cd->setClearColor(MTL::ClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1));
				
				if (m_pRenderer->getSceneSampleCount() == 1)
				{
					cd->setStoreAction(MTL::StoreActionStore);
				}
				else
				{
					cd->setStoreAction(MTL::StoreActionMultisampleResolve);
				}
				
				MTL::RenderCommandEncoder* renderCommandEncoder = commandBuffer->renderCommandEncoder(offscreenRenderPassDescriptor.get());
				
				if (m_pRenderer->getSceneSampleCount() == 1)
				{
					renderCommandEncoder->setRenderPipelineState(m_pipeLineTestNoAA.get());
				}
				else
				{
					renderCommandEncoder->setRenderPipelineState(m_pipeLineTestMSAAx4.get());
				}
					
				renderCommandEncoder->setVertexBuffer(m_vertexBufferManager.getCurrentVertexBuffer().get(), 0, 0);
				MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
				NS::UInteger vertexStart = 0;
				NS::UInteger vertexCount = 3;
				renderCommandEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
				renderCommandEncoder->endEncoding();
			}
		}

		/*
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
		*/
	}

	dispatch_semaphore_t CRenderer2D_Metal::getSemaphore() const
	{
		return m_vertexBufferManager.getSemaphore();
	}
}
