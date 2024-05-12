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

# include "CRenderer_Metal.hpp"
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/WindowState.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	CRenderer_Metal::~CRenderer_Metal()
	{
		LOG_SCOPED_DEBUG("CRenderer_Metal::~CRenderer_Metal()");
		
		if (m_metalDevice)
		{
			m_metalDevice->release();
		}
	}

	void CRenderer_Metal::init()
	{
		LOG_SCOPED_DEBUG("CRenderer_Metal::init()");

		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
		m_metalWindow = ::glfwGetCocoaWindow(glfwWindow);

		m_metalDevice = MTL::CreateSystemDefaultDevice();
		m_metalLayer = [CAMetalLayer layer];
		m_metalLayer.device = (__bridge id<MTLDevice>)m_metalDevice;
		m_metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
		m_metalWindow.contentView.layer = m_metalLayer;
		m_metalWindow.contentView.wantsLayer = YES;
		
		simd::float3 triangleVertices[] = {
			{-0.5f, -0.5f, 0.0f},
			{ 0.5f, -0.5f, 0.0f},
			{ 0.0f,  0.5f, 0.0f}
		};

		m_triangleVertexBuffer = m_metalDevice->newBuffer(&triangleVertices,
													  sizeof(triangleVertices),
													  MTL::ResourceStorageModeShared);

		m_metalDefaultLibrary = m_metalDevice->newDefaultLibrary();
		
		if(not m_metalDefaultLibrary)
		{
			throw InternalEngineError{ "Failed to create a default library" };
		}
		
		m_metalCommandQueue = m_metalDevice->newCommandQueue();
		
		
		MTL::Function* vertexShader = m_metalDefaultLibrary->newFunction(NS::String::string("vertexShader", NS::ASCIIStringEncoding));
		assert(vertexShader);
		MTL::Function* fragmentShader = m_metalDefaultLibrary->newFunction(NS::String::string("fragmentShader", NS::ASCIIStringEncoding));
		assert(fragmentShader);

		{
			MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
			renderPipelineDescriptor->setLabel(NS::String::string("Triangle Rendering Pipeline", NS::ASCIIStringEncoding));
			renderPipelineDescriptor->setVertexFunction(vertexShader);
			renderPipelineDescriptor->setFragmentFunction(fragmentShader);
			assert(renderPipelineDescriptor);
			MTL::PixelFormat pixelFormat = (MTL::PixelFormat)m_metalLayer.pixelFormat;
			renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(pixelFormat);
			
			NS::Error* error;
			m_metalRenderPSO = m_metalDevice->newRenderPipelineState(renderPipelineDescriptor, &error);
			
			renderPipelineDescriptor->release();
		}
	}

	void CRenderer_Metal::clear()
	{

	}

	void CRenderer_Metal::flush()
	{

	}

	bool CRenderer_Metal::present()
	{
		@autoreleasepool
		{
			m_metalDrawable = (__bridge CA::MetalDrawable*)[m_metalLayer nextDrawable];
			
			sendRenderCommand();
		}

		return(true);
	}

	SceneStyle& CRenderer_Metal::getSceneStyle() noexcept
	{
		return(m_sceneStyle);
	}

	void CRenderer_Metal::setSceneResizeMode(const ResizeMode resizeMode)
	{

	}

	ResizeMode CRenderer_Metal::getSceneResizeMode() const noexcept
	{
		return m_sceneResizeMode;
	}

	void CRenderer_Metal::resizeSceneBuffer(const Size size)
	{

	}

	const Size& CRenderer_Metal::getSceneBufferSize() const noexcept
	{
		return(m_sceneBufferSize);
	}

	void CRenderer_Metal::sendRenderCommand()
	{
		m_metalCommandBuffer = m_metalCommandQueue->commandBuffer();

		MTL::RenderPassDescriptor* renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
		MTL::RenderPassColorAttachmentDescriptor* cd = renderPassDescriptor->colorAttachments()->object(0);
		cd->setTexture(m_metalDrawable->texture());
		cd->setLoadAction(MTL::LoadActionClear);
		cd->setClearColor(MTL::ClearColor{ m_sceneStyle.backgroundColor.r, m_sceneStyle.backgroundColor.g, m_sceneStyle.backgroundColor.b, 1.0 });
		cd->setStoreAction(MTL::StoreActionStore);

		MTL::RenderCommandEncoder* renderCommandEncoder = m_metalCommandBuffer->renderCommandEncoder(renderPassDescriptor);
		encodeRenderCommand(renderCommandEncoder);
		renderCommandEncoder->endEncoding();

		m_metalCommandBuffer->presentDrawable(m_metalDrawable);
		m_metalCommandBuffer->commit();
		m_metalCommandBuffer->waitUntilCompleted();

		renderPassDescriptor->release();
	}

	void CRenderer_Metal::encodeRenderCommand(MTL::RenderCommandEncoder* renderCommandEncoder)
	{
		renderCommandEncoder->setRenderPipelineState(m_metalRenderPSO);
		renderCommandEncoder->setVertexBuffer(m_triangleVertexBuffer, 0, 0);
		MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
		NS::UInteger vertexStart = 0;
		NS::UInteger vertexCount = 3;
		renderCommandEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
	}
}
 
