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
# include <Siv3D/Shader/IShader.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
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

		m_sceneBuffers = {};

		if (m_device)
		{
			m_device->release();
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
		
		m_pShader = static_cast<CShader_Metal*>(SIV3D_ENGINE(Shader));

		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
		m_metalWindow = ::glfwGetCocoaWindow(glfwWindow);

		m_device = MTL::CreateSystemDefaultDevice();

		const Size& frameBufferSize = Window::GetState().frameBufferSize;
		m_metalLayer = [CAMetalLayer layer];
		m_metalLayer.device = (__bridge id<MTLDevice>)m_device;
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
			
			m_triangleVertexBuffer = NS::TransferPtr(m_device->newBuffer(&triangleVertices,
															  sizeof(triangleVertices),
															  MTL::ResourceStorageModeShared));
		}

		m_metalCommandQueue = NS::TransferPtr(m_device->newCommandQueue());
		
		{
			const Size sceneSize = Window::GetState().virtualSize;
			
			if (m_sceneBuffers.sampleCount == 1)
			{
				m_sceneBuffers.scene	= MetalInternalTexture2D::CreateRenderTexture(m_device, sceneSize);
			}
			else
			{
				m_sceneBuffers.scene	= MetalInternalTexture2D::CreateMSRenderTexture(m_device, sceneSize, m_sceneBuffers.sampleCount);
				m_sceneBuffers.resolved	= MetalInternalTexture2D::CreateRenderTexture(m_device, sceneSize);
			}
		}
		
		SIV3D_ENGINE(Shader)->init();
		SIV3D_ENGINE(EngineShader)->init();

		{
			const VertexShader& vs = SIV3D_ENGINE(EngineShader)->getVS(EngineVS::FullScreenTriangle);
			const PixelShader& ps = SIV3D_ENGINE(EngineShader)->getPS(EnginePS::FullScreenTriangle);

			NS::SharedPtr<MTL::RenderPipelineDescriptor> renderPipelineDescriptor = NS::TransferPtr(MTL::RenderPipelineDescriptor::alloc()->init());
			renderPipelineDescriptor->setLabel(NS::String::string("FullScreenTriangle Rendering Pipeline", NS::ASCIIStringEncoding));
			renderPipelineDescriptor->setVertexFunction(m_pShader->getShaderVS(vs.id()));
			renderPipelineDescriptor->setFragmentFunction(m_pShader->getShaderPS(ps.id()));
			renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat((MTL::PixelFormat)m_metalLayer.pixelFormat);
			
			NS::Error* error;
			m_pipeLineStateFullScreenTriangle = NS::TransferPtr(m_device->newRenderPipelineState(renderPipelineDescriptor.get(), &error));
		}
		
		clear();
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
		
		if (m_commandBuffer)
		{
			m_commandBuffer->release();
		}
		
		m_commandBuffer = m_metalCommandQueue->commandBuffer();
	}

	////////////////////////////////////////////////////////////////
	//
	//	flush
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_Metal::flush()
	{
		if (not m_pipeLineTest)
		{
			const VertexShader& vs = SIV3D_ENGINE(EngineShader)->getVS(EngineVS::TestTriangle);
			const PixelShader& ps = SIV3D_ENGINE(EngineShader)->getPS(EnginePS::TestTriangle);
			
			NS::SharedPtr<MTL::RenderPipelineDescriptor> renderPipelineDescriptor = NS::TransferPtr(MTL::RenderPipelineDescriptor::alloc()->init());
			renderPipelineDescriptor->setLabel(NS::String::string("Off-screen Rendering Pipeline", NS::ASCIIStringEncoding));
			renderPipelineDescriptor->setVertexFunction(m_pShader->getShaderVS(vs.id()));
			renderPipelineDescriptor->setFragmentFunction(m_pShader->getShaderPS(ps.id()));
			renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
			//renderPipelineDescriptor->setSampleCount(4);
			
			NS::Error* error;
			m_pipeLineTest = NS::TransferPtr(m_device->newRenderPipelineState(renderPipelineDescriptor.get(), &error));
		}

		// Draw2D
		@autoreleasepool
		{
			{
				NS::SharedPtr<MTL::RenderPassDescriptor> offscreenRenderPassDescriptor = NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init());
				MTL::RenderPassColorAttachmentDescriptor* cd = offscreenRenderPassDescriptor->colorAttachments()->object(0);
				cd->setTexture(m_sceneBuffers.scene.getTexture());
				cd->setLoadAction(MTL::LoadActionClear);
				cd->setClearColor(MTL::ClearColor(m_sceneStyle.backgroundColor.r, m_sceneStyle.backgroundColor.g, m_sceneStyle.backgroundColor.b, 1));
				cd->setStoreAction(MTL::StoreActionStore);
				
				MTL::RenderCommandEncoder* renderCommandEncoder = m_commandBuffer->renderCommandEncoder(offscreenRenderPassDescriptor.get());
				renderCommandEncoder->setRenderPipelineState(m_pipeLineTest.get());
				renderCommandEncoder->setVertexBuffer(m_triangleVertexBuffer.get(), 0, 0);
				MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
				NS::UInteger vertexStart = 0;
				NS::UInteger vertexCount = 3;
				renderCommandEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
				renderCommandEncoder->endEncoding();
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	present
	//
	////////////////////////////////////////////////////////////////

	bool CRenderer_Metal::present()
	{
		// SceneToBackBuffer
		@autoreleasepool
		{
			m_metalDrawable = (__bridge CA::MetalDrawable*)[m_metalLayer nextDrawable];
			
			if (m_sceneBuffers.sampleCount == 1)
			{
				NS::SharedPtr<MTL::RenderPassDescriptor> renderPassDescriptor = NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init());
				
				MTL::RenderPassColorAttachmentDescriptor* cd = renderPassDescriptor->colorAttachments()->object(0);
				cd->setTexture(m_metalDrawable->texture());
				cd->setLoadAction(MTL::LoadActionClear);
				cd->setClearColor(MTL::ClearColor{ m_sceneStyle.letterboxColor.r, m_sceneStyle.letterboxColor.g, m_sceneStyle.letterboxColor.b, 1.0 });
				cd->setStoreAction(MTL::StoreActionStore);
				
				MTL::RenderCommandEncoder* renderCommandEncoder = m_commandBuffer->renderCommandEncoder(renderPassDescriptor.get());
				renderCommandEncoder->setRenderPipelineState(m_pipeLineStateFullScreenTriangle.get());
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
				renderCommandEncoder->setFragmentTexture(m_sceneBuffers.scene.getTexture(), 0);
				renderCommandEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger{ 0 }, 3);
				renderCommandEncoder->endEncoding();
			}
			else
			{
				/*
				NS::SharedPtr<MTL::RenderPassDescriptor> renderPassDescriptor = NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init());
				
				MTL::RenderPassColorAttachmentDescriptor* cd = renderPassDescriptor->colorAttachments()->object(0);
				cd->setTexture(m_metalDrawable->texture());
				cd->setLoadAction(MTL::LoadActionClear);
				cd->setClearColor(MTL::ClearColor{ m_sceneStyle.letterboxColor.r, m_sceneStyle.letterboxColor.g, m_sceneStyle.letterboxColor.b, 1.0 });
				cd->setStoreAction(MTL::StoreActionStore);
				
				MTL::RenderCommandEncoder* renderCommandEncoder = m_commandBuffer->renderCommandEncoder(renderPassDescriptor.get());
				renderCommandEncoder->setRenderPipelineState(m_pipeLineStateFullScreenTriangle.get());
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
				renderCommandEncoder->setFragmentTexture(m_sceneBuffers.resolved.getTexture(), 0);
				renderCommandEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger{ 0 }, 3);
				renderCommandEncoder->endEncoding();
				*/
			}
			
			m_commandBuffer->presentDrawable(m_metalDrawable);
			m_commandBuffer->commit();
			m_commandBuffer->waitUntilCompleted();
		}

		return true;
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

		m_sceneBuffers = {};
		
		if (m_sceneBuffers.sampleCount == 1)
		{
			m_sceneBuffers.scene	= MetalInternalTexture2D::CreateRenderTexture(m_device, size);
		}
		else
		{
			m_sceneBuffers.scene	= MetalInternalTexture2D::CreateMSRenderTexture(m_device, size, m_sceneBuffers.sampleCount);
			m_sceneBuffers.resolved	= MetalInternalTexture2D::CreateRenderTexture(m_device, size);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneBufferSize
	//
	////////////////////////////////////////////////////////////////

	const Size& CRenderer_Metal::getSceneBufferSize() const noexcept
	{
		return m_sceneBuffers.scene.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLetterboxComposition
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, RectF> CRenderer_Metal::getLetterboxComposition() const noexcept
	{
		return SceneMisc::CalculateLetterboxComposition(getBackBufferSize(), m_sceneBuffers.scene.size());
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

	MTL::Device* CRenderer_Metal::getDevice() const
	{
		return m_device;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

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
 
