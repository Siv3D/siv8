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
			
			m_triangleVertexBuffer = m_metalDevice->newBuffer(&triangleVertices,
															  sizeof(triangleVertices),
															  MTL::ResourceStorageModeShared);
		}
		
		{
			VertexData sceneVertices[] {
				{{-1.0,  1.0,  0.0, 1.0f}, {0.0f, 0.0f}},
				{{ 1.0,  1.0,  0.0, 1.0f}, {1.0f, 0.0f}},
				{{-1.0, -1.0,  0.0, 1.0f}, {0.0f, 1.0f}},
				{{-1.0, -1.0,  0.0, 1.0f}, {0.0f, 1.0f}},
				{{ 1.0,  1.0,  0.0, 1.0f}, {1.0f, 0.0f}},
				{{ 1.0, -1.0,  0.0, 1.0f}, {1.0f, 1.0f}}
			};

			m_sceneVertexBuffer = m_metalDevice->newBuffer(&sceneVertices, sizeof(sceneVertices), MTL::ResourceStorageModeShared);
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

		m_metalCommandQueue = m_metalDevice->newCommandQueue();
		
		{
			MTL::Function* vertexShader = m_metalDefaultLibrary->newFunction(NS::String::string("vertexShader", NS::ASCIIStringEncoding));
			assert(vertexShader);
			MTL::Function* fragmentShader = m_metalDefaultLibrary->newFunction(NS::String::string("fragmentShader", NS::ASCIIStringEncoding));
			assert(fragmentShader);
			
			MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
			renderPipelineDescriptor->setLabel(NS::String::string("Off-screen Rendering Pipeline", NS::ASCIIStringEncoding));
			renderPipelineDescriptor->setVertexFunction(vertexShader);
			renderPipelineDescriptor->setFragmentFunction(fragmentShader);
			assert(renderPipelineDescriptor);
			MTL::PixelFormat pixelFormat = MTL::PixelFormatRGBA8Unorm;
			renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(pixelFormat);
			
			NS::Error* error;
			m_metalRenderPSO1 = m_metalDevice->newRenderPipelineState(renderPipelineDescriptor, &error);
			renderPipelineDescriptor->release();
		}
		
		{
			MTL::Function* vertexShader = m_metalDefaultLibrary->newFunction(NS::String::string("sceneVertexShader", NS::ASCIIStringEncoding));
			assert(vertexShader);
			MTL::Function* fragmentShader = m_metalDefaultLibrary->newFunction(NS::String::string("sceneFragmentShader", NS::ASCIIStringEncoding));
			assert(fragmentShader);
		
			MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
			renderPipelineDescriptor->setLabel(NS::String::string("Scene Rendering Pipeline", NS::ASCIIStringEncoding));
			
			renderPipelineDescriptor->setVertexFunction(vertexShader);
			renderPipelineDescriptor->setFragmentFunction(fragmentShader);
			assert(renderPipelineDescriptor);
			MTL::PixelFormat pixelFormat = (MTL::PixelFormat)m_metalLayer.pixelFormat;
			renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(pixelFormat);
			
			NS::Error* error;
			m_metalRenderPSO2 = m_metalDevice->newRenderPipelineState(renderPipelineDescriptor, &error);
			renderPipelineDescriptor->release();
		}

		/*
		{
			MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
			textureDescriptor->setTextureType(MTL::TextureType2D);
			textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
			textureDescriptor->setWidth(800);
			textureDescriptor->setHeight(600);
			textureDescriptor->setStorageMode(MTL::StorageModePrivate);  // GPU-private storage
			textureDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);

			m_sceneTexture = m_metalDevice->newTexture(textureDescriptor);
			textureDescriptor->release();
		}*/
		
		m_sceneBuffer = MetalInternalTexture2D::CreateRenderTexture(m_metalDevice, Size{ 800, 600 });
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

	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneBufferSize
	//
	////////////////////////////////////////////////////////////////

	const Size& CRenderer_Metal::getSceneBufferSize() const noexcept
	{
		return m_sceneBufferSize;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLetterboxComposition
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, RectF> CRenderer_Metal::getLetterboxComposition() const noexcept
	{
		const SizeF sceneSize = m_sceneBufferSize;
		const SizeF backBufferSize = getBackBufferSize();

		const double sx = (backBufferSize.x / sceneSize.x);
		const double sy = (backBufferSize.y / sceneSize.y);
		const double s = Min(sx, sy);

		if (sx <= sy)
		{
			const double offsetY = ((backBufferSize.y - sceneSize.y * s) * 0.5);
			const double width = backBufferSize.x;
			const double height = (backBufferSize.y - offsetY * 2.0);
			return{ s, RectF{ 0.0, offsetY, width, height } };
		}
		else
		{
			const double offsetX = ((backBufferSize.x - sceneSize.x * s) * 0.5);
			const double width = (backBufferSize.x - offsetX * 2.0);
			const double height = backBufferSize.y;
			return{ s, RectF{ offsetX, 0.0, width, height } };
		}
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

	}

	////////////////////////////////////////////////////////////////
	//
	//	isVSyncEnabled
	//
	////////////////////////////////////////////////////////////////

	bool CRenderer_Metal::isVSyncEnabled() const
	{
		return(true);
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

			renderCommandEncoder->setRenderPipelineState(m_metalRenderPSO1);
			renderCommandEncoder->setVertexBuffer(m_triangleVertexBuffer, 0, 0);
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
			
			renderCommandEncoder->setRenderPipelineState(m_metalRenderPSO2);
			renderCommandEncoder->setVertexBuffer(m_sceneVertexBuffer, 0, 0);
			MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
			NS::UInteger vertexStart = 0;
			NS::UInteger vertexCount = 6;
			renderCommandEncoder->setFragmentTexture(m_sceneBuffer.getTexture(), 0);
			renderCommandEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
			renderCommandEncoder->endEncoding();
		}
		
		m_metalCommandBuffer->presentDrawable(m_metalDrawable);
		m_metalCommandBuffer->commit();
		m_metalCommandBuffer->waitUntilCompleted();
	}

	void CRenderer_Metal::encodeRenderCommand(MTL::RenderCommandEncoder* renderCommandEncoder)
	{
		/*
		renderCommandEncoder->setRenderPipelineState(m_metalRenderPSO);
		renderCommandEncoder->setVertexBuffer(m_triangleVertexBuffer, 0, 0);
		MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
		NS::UInteger vertexStart = 0;
		NS::UInteger vertexCount = 3;
		renderCommandEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
		 */
	}

	void CRenderer_Metal::resizeBackBuffer(const Size backBufferSize)
	{
		assert((0 < backBufferSize.x) && (0 < backBufferSize.y));
	
		LOG_DEBUG(fmt::format("CRenderer_Metal::resizeBackBuffer({})", backBufferSize));

		m_metalLayer.drawableSize = CGSizeMake(backBufferSize.x, backBufferSize.y);
	}

	Size CRenderer_Metal::getBackBufferSize() const
	{
		const CGSize backBufferSizeCGSize = m_metalLayer.drawableSize;
		return{
			static_cast<int32>(backBufferSizeCGSize.width),
			static_cast<int32>(backBufferSizeCGSize.height) };
	}
}
 
