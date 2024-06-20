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
			//if (not m_currentCustomShader.vs)
			//{
			//	m_commandManager.pushEngineVS(m_engineShader.vs);
			//}

			//if (not m_currentCustomShader.ps)
			//{
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
					//m_commandManager.pushEnginePS(m_engineShader.psShape);
				}
			//}

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
			//if (not m_currentCustomShader.vs)
			//{
			//	m_commandManager.pushEngineVS(m_engineShader.vs);
			//}

			//if (not m_currentCustomShader.ps)
			//{
			//	m_commandManager.pushEnginePS(m_engineShader.psShape);
			//}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_Metal::addTriangle(const Float2(&points)[3], const Float4(&colors)[3])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(std::bind_front(&CRenderer2D_Metal::createBuffer, this), points, colors))
		{
			//if (not m_currentCustomShader.vs)
			//{
			//	m_commandManager.pushEngineVS(m_engineShader.vs);
			//}

			//if (not m_currentCustomShader.ps)
			//{
			//	m_commandManager.pushEnginePS(m_engineShader.psShape);
			//}

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
			//if (not m_currentCustomShader.vs)
			//{
			//	m_commandManager.pushEngineVS(m_engineShader.vs);
			//}

			//if (not m_currentCustomShader.ps)
			//{
			//	m_commandManager.pushEnginePS(m_engineShader.psShape);
			//}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_Metal::addRect(const FloatRect& rect, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(std::bind_front(&CRenderer2D_Metal::createBuffer, this), rect, colors))
		{
			//if (not m_currentCustomShader.vs)
			//{
			//	m_commandManager.pushEngineVS(m_engineShader.vs);
			//}

			//if (not m_currentCustomShader.ps)
			//{
			//	m_commandManager.pushEnginePS(m_engineShader.psShape);
			//}

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
			//if (not m_currentCustomShader.vs)
			//{
			//	m_commandManager.pushEngineVS(m_engineShader.vs);
			//}

			//if (not m_currentCustomShader.ps)
			//{
			//	m_commandManager.pushEnginePS(m_engineShader.psShape);
			//}

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
			//m_currentCustomShader.vs.reset();
			//m_currentCustomShader.ps.reset();
		};
		
		m_commandManager.flush();
		
		const Size currentRenderTargetSize = SIV3D_ENGINE(Renderer)->getSceneBufferSize();
		const Mat3x2 screenMat = Mat3x2::Screen(currentRenderTargetSize);
		const Float4 transform[2] =
		{
			{ screenMat._11, screenMat._12, screenMat._31, screenMat._32 },
			{ screenMat._21, screenMat._22, 0.0f, 1.0f }
		};

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
		
		const PipelineStateDesc pipelineStateDesc
		{
			.vs = m_engineShader.vs,
			.ps = m_engineShader.psShape,
			.pixelFormat = static_cast<uint16>(MTL::PixelFormatRGBA8Unorm),
			.sampleCount = static_cast<uint16>(m_pRenderer->getSceneSampleCount()),
			.blendState = BlendState::Default2D,
		};

		const auto pipeline = m_pRenderer->getRenderPipelineState().get(pipelineStateDesc);
		
		@autoreleasepool
		{
			MTL::RenderCommandEncoder* renderCommandEncoder = m_commandBuffer->renderCommandEncoder(offscreenRenderPassDescriptor.get());
			renderCommandEncoder->setRenderPipelineState(pipeline);
			
			if (const uint32 indexCount = m_vertexBufferManager.indexCount())
			{
				renderCommandEncoder->setVertexBuffer(m_vertexBufferManager.getVertexBuffer(), 0, 0);
				renderCommandEncoder->setVertexBytes(transform, sizeof(transform), 1);
				renderCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, indexCount, MTL::IndexTypeUInt16, m_vertexBufferManager.getIndexBuffer(), 0);
			}
			
			renderCommandEncoder->endEncoding();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getColorMul
	//
	////////////////////////////////////////////////////////////////

	Float4 CRenderer2D_Metal::getColorMul() const
	{
		return m_commandManager.getCurrentColorMul();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getColorAdd
	//
	////////////////////////////////////////////////////////////////

	Float3 CRenderer2D_Metal::getColorAdd() const
	{
		return m_commandManager.getCurrentColorAdd();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setColorMul
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::setColorMul(const Float4& color)
	{
		m_commandManager.pushColorMul(color);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setColorAdd
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::setColorAdd(const Float3& color)
	{
		m_commandManager.pushColorAdd(color);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBlendState
	//
	////////////////////////////////////////////////////////////////

	BlendState CRenderer2D_Metal::getBlendState() const
	{
		return m_commandManager.getCurrentBlendState();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setBlendState
	//
	////////////////////////////////////////////////////////////////

	void CRenderer2D_Metal::setBlendState(const BlendState& state)
	{
		m_commandManager.pushBlendState(state);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMaxScaling
	//
	////////////////////////////////////////////////////////////////

	float CRenderer2D_Metal::getMaxScaling() const noexcept
	{
		return(1.0f); // [Siv3D ToDo]
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
