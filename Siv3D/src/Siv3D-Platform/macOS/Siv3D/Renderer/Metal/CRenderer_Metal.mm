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
# include <Siv3D/Scene/SceneUtility.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	struct VertexData
	{
		simd::float4 position;
		simd::float2 textureCoordinate;
	};

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CRenderer_Metal::~CRenderer_Metal()
	{
		LOG_SCOPED_DEBUG("CRenderer_Metal::~CRenderer_Metal()");

		m_sceneBuffer.reset();

		if (m_metalDevice)
		{
			m_metalDevice->release();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getName
	//
	////////////////////////////////////////////////////////////////

	StringView CRenderer_Metal::getName() const
	{
		static constexpr StringView name{ U"Metal" };
		return name;
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_Metal::init()
	{
		LOG_SCOPED_DEBUG("CRenderer_Metal::init()");

		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
		m_metalWindow = ::glfwGetCocoaWindow(glfwWindow);
		
		const Size& frameBufferSize = Window::GetState().frameBufferSize;

		m_metalDevice = MTL::CreateSystemDefaultDevice();
		m_metalLayer = [CAMetalLayer layer];
		m_metalLayer.device = (__bridge id<MTLDevice>)m_metalDevice;
		m_metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
		m_metalLayer.drawableSize = CGSizeMake(frameBufferSize.x, frameBufferSize.y);
		m_metalWindow.contentView.layer = m_metalLayer;
		m_metalWindow.contentView.wantsLayer = YES;
		
		{
			simd::float3 triangleVertices[] = {
				{-0.5f, -0.5f, 0.0f},
				{ 0.5f, -0.5f, 0.0f},
				{ 0.0f,  0.5f, 0.0f}
			};
			
			m_triangleVertexBuffer = NS::TransferPtr(m_metalDevice->newBuffer(&triangleVertices,
															  sizeof(triangleVertices),
															  MTL::ResourceStorageModeShared));
		}
		
		m_metalDefaultLibrary = m_metalDevice->newDefaultLibrary();
		
		if(not m_metalDefaultLibrary)
		{
			throw InternalEngineError{ "Failed to create a default library" };
		}

		// list function names in the default library
		/*
		NS::Array* functionNames = m_metalDefaultLibrary->functionNames();

		for (NS::UInteger i = 0; i < functionNames->count(); ++i)
		{

		}
		 */

		m_metalCommandQueue = NS::TransferPtr(m_metalDevice->newCommandQueue());
		
		{
			NS::SharedPtr<MTL::Function> vertexShader = NS::TransferPtr(m_metalDefaultLibrary->newFunction(NS::String::string("vertexShader", NS::ASCIIStringEncoding)));
			assert(vertexShader);
			NS::SharedPtr<MTL::Function> fragmentShader = NS::TransferPtr(m_metalDefaultLibrary->newFunction(NS::String::string("fragmentShader", NS::ASCIIStringEncoding)));
			assert(fragmentShader);
			
			NS::SharedPtr<MTL::RenderPipelineDescriptor> renderPipelineDescriptor = NS::TransferPtr(MTL::RenderPipelineDescriptor::alloc()->init());
			renderPipelineDescriptor->setLabel(NS::String::string("Off-screen Rendering Pipeline", NS::ASCIIStringEncoding));
			renderPipelineDescriptor->setVertexFunction(vertexShader.get());
			renderPipelineDescriptor->setFragmentFunction(fragmentShader.get());
			assert(renderPipelineDescriptor);
			MTL::PixelFormat pixelFormat = MTL::PixelFormatRGBA8Unorm;
			renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(pixelFormat);
			
			NS::Error* error;
			m_metalRenderPSO1 = NS::TransferPtr(m_metalDevice->newRenderPipelineState(renderPipelineDescriptor.get(), &error));
		}
		
		{
			NS::SharedPtr<MTL::Function> vertexShader = NS::TransferPtr(m_metalDefaultLibrary->newFunction(NS::String::string("sceneVertexShader", NS::ASCIIStringEncoding)));
			assert(vertexShader);
			NS::SharedPtr<MTL::Function> fragmentShader = NS::TransferPtr(m_metalDefaultLibrary->newFunction(NS::String::string("sceneFragmentShader", NS::ASCIIStringEncoding)));
			assert(fragmentShader);
		
			MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
			renderPipelineDescriptor->setLabel(NS::String::string("Scene Rendering Pipeline", NS::ASCIIStringEncoding));
			
			renderPipelineDescriptor->setVertexFunction(vertexShader.get());
			renderPipelineDescriptor->setFragmentFunction(fragmentShader.get());
			assert(renderPipelineDescriptor);
			MTL::PixelFormat pixelFormat = (MTL::PixelFormat)m_metalLayer.pixelFormat;
			renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(pixelFormat);
			
			NS::Error* error;
			m_metalRenderPSO2 = NS::TransferPtr(m_metalDevice->newRenderPipelineState(renderPipelineDescriptor, &error));
		}

		{
			const Size sceneSize = Window::GetState().virtualSize;
			m_sceneBuffer = MetalInternalTexture2D::CreateRenderTexture(m_metalDevice, sceneSize);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_Metal::clear()
	{
		if (const Size windowFrameBufferSize = SIV3D_ENGINE(Window)->getState().frameBufferSize;
			windowFrameBufferSize != getBackBufferSize())
		{
			resizeBackBuffer(windowFrameBufferSize);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	flush
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_Metal::flush()
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	present
	//
	////////////////////////////////////////////////////////////////

	bool CRenderer_Metal::present()
	{
		@autoreleasepool
		{
			m_metalDrawable = (__bridge CA::MetalDrawable*)[m_metalLayer nextDrawable];
			
			sendRenderCommand();
		}

		return(true);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneStyle
	//
	////////////////////////////////////////////////////////////////

	SceneStyle& CRenderer_Metal::getSceneStyle() noexcept
	{
		return m_sceneStyle;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSceneResizeMode
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_Metal::setSceneResizeMode(const ResizeMode resizeMode)
	{
		m_sceneResizeMode = resizeMode;

		updateSceneBufferSize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneResizeMode
	//
	////////////////////////////////////////////////////////////////

	ResizeMode CRenderer_Metal::getSceneResizeMode() const noexcept
	{
		return m_sceneResizeMode;
	}

	////////////////////////////////////////////////////////////////
	//
	//	resizeSceneBuffer
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_Metal::resizeSceneBuffer(const Size size)
	{
		assert((0 < size.x) && (0 < size.y));

		if (getSceneBufferSize() == size)
		{
			return;
		}

		LOG_DEBUG(fmt::format("CRenderer_Metal::resizeSceneBuffer({})", size));

		m_sceneBuffer = {};
		
		//if (m_sceneSampleCount == 1)
		{
			m_sceneBuffer = MetalInternalTexture2D::CreateRenderTexture(m_metalDevice, size);
		}
		//else
		//{
		//	m_sceneBuffers.scene = D3D11InternalTexture2D::CreateMSRenderTexture(m_device, size, m_sceneSampleCount);
		//}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneBufferSize
	//
	////////////////////////////////////////////////////////////////

	const Size& CRenderer_Metal::getSceneBufferSize() const noexcept
	{
		return m_sceneBuffer.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLetterboxComposition
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, RectF> CRenderer_Metal::getLetterboxComposition() const noexcept
	{
		return SceneMisc::CalculateLetterboxComposition(getBackBufferSize(), m_sceneBuffer.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	updateSceneSize
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_Metal::updateSceneSize()
	{
		if (m_sceneResizeMode == ResizeMode::Actual)
		{
			resizeSceneBuffer(Window::GetState().frameBufferSize);
		}
		else if (m_sceneResizeMode == ResizeMode::Virtual)
		{
			resizeSceneBuffer(Window::GetState().virtualSize);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setVSyncEnabled
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_Metal::setVSyncEnabled(const bool enabled)
	{
		if (m_vSyncEnabled == enabled)
		{
			return;
		}
		
		m_vSyncEnabled = enabled;
		m_metalLayer.displaySyncEnabled = enabled;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isVSyncEnabled
	//
	////////////////////////////////////////////////////////////////

	bool CRenderer_Metal::isVSyncEnabled() const
	{
		return m_vSyncEnabled;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setLowLatencyMode
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_Metal::setLowLatencyMode(const bool) {}

	////////////////////////////////////////////////////////////////
	//
	//	isLowLatencyMode
	//
	////////////////////////////////////////////////////////////////

	bool CRenderer_Metal::isLowLatencyMode() const
	{
		return false;
	}

	void CRenderer_Metal::updateSceneBufferSize()
	{
		if (m_sceneResizeMode == ResizeMode::Actual)
		{
			resizeSceneBuffer(Window::GetState().frameBufferSize);
		}
		else if (m_sceneResizeMode == ResizeMode::Virtual)
		{
			resizeSceneBuffer(Window::GetState().virtualSize);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_Metal::sendRenderCommand()
	{
		m_metalCommandBuffer = m_metalCommandQueue->commandBuffer();

		{
			MTL::RenderPassDescriptor* offscreenRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
			MTL::RenderPassColorAttachmentDescriptor* cd = offscreenRenderPassDescriptor->colorAttachments()->object(0);
			cd->setTexture(m_sceneBuffer.getTexture());
			cd->setLoadAction(MTL::LoadActionClear);
			cd->setClearColor(MTL::ClearColor(m_sceneStyle.backgroundColor.r, m_sceneStyle.backgroundColor.g, m_sceneStyle.backgroundColor.b, 1));
			cd->setStoreAction(MTL::StoreActionStore);

			MTL::RenderCommandEncoder* renderCommandEncoder = m_metalCommandBuffer->renderCommandEncoder(offscreenRenderPassDescriptor);
			offscreenRenderPassDescriptor->release();

			renderCommandEncoder->setRenderPipelineState(m_metalRenderPSO1.get());
			renderCommandEncoder->setVertexBuffer(m_triangleVertexBuffer.get(), 0, 0);
			MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
			NS::UInteger vertexStart = 0;
			NS::UInteger vertexCount = 3;
			renderCommandEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
			renderCommandEncoder->endEncoding();
		}
		
		{
			MTL::RenderPassDescriptor* renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
			MTL::RenderPassColorAttachmentDescriptor* cd = renderPassDescriptor->colorAttachments()->object(0);
			cd->setTexture(m_metalDrawable->texture());
			cd->setLoadAction(MTL::LoadActionClear);
			cd->setClearColor(MTL::ClearColor{ m_sceneStyle.letterboxColor.r, m_sceneStyle.letterboxColor.g, m_sceneStyle.letterboxColor.b, 1.0 });
			cd->setStoreAction(MTL::StoreActionStore);
			
			MTL::RenderCommandEncoder* renderCommandEncoder = m_metalCommandBuffer->renderCommandEncoder(renderPassDescriptor);
			renderPassDescriptor->release();
			
			renderCommandEncoder->setRenderPipelineState(m_metalRenderPSO2.get());
			
			const auto [s, viewRect] = getLetterboxComposition();
			const MTL::Viewport viewport = {
				.originX = viewRect.x,
				.originY = viewRect.y,
				.width = viewRect.w,
				.height = viewRect.h,
				.znear = 0.0,
				.zfar = 1.0
			};
			renderCommandEncoder->setViewport(viewport);
			
			MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
			NS::UInteger vertexStart = 0;
			NS::UInteger vertexCount = 3;
			renderCommandEncoder->setFragmentTexture(m_sceneBuffer.getTexture(), 0);
			renderCommandEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
			renderCommandEncoder->endEncoding();
		}
		
		m_metalCommandBuffer->presentDrawable(m_metalDrawable);
		m_metalCommandBuffer->commit();
		m_metalCommandBuffer->waitUntilCompleted();
	}

	void CRenderer_Metal::resizeBackBuffer(const Size backBufferSize)
	{
		assert((0 < backBufferSize.x) && (0 < backBufferSize.y));
	
		LOG_DEBUG(fmt::format("CRenderer_Metal::resizeBackBuffer({})", backBufferSize));

		m_metalLayer.drawableSize = CGSizeMake(backBufferSize.x, backBufferSize.y);

		updateSceneBufferSize();
	}

	Size CRenderer_Metal::getBackBufferSize() const
	{
		const CGSize backBufferSizeCGSize = m_metalLayer.drawableSize;
		return{
			static_cast<int32>(backBufferSizeCGSize.width),
			static_cast<int32>(backBufferSizeCGSize.height) };
	}
}
 
