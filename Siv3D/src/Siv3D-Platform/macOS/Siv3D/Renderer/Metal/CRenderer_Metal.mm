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
# include <Siv3D/Shader/Metal/CShader_Metal.hpp>
# include <Siv3D/Renderer2D/Metal/CRenderer2D_Metal.hpp>
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
		m_pRenderer2D = static_cast<CRenderer2D_Metal*>(SIV3D_ENGINE(Renderer2D));

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

		m_commandQueue = NS::TransferPtr(m_device->newCommandQueue());
		
		{
			const Size sceneSize = Window::GetState().virtualSize;
			
			if (m_sceneBuffers.sampleCount == 1)
			{
				m_sceneBuffers.nonMSAA	= MetalInternalTexture2D::CreateRenderTexture(m_device, sceneSize);
			}
			else
			{
				m_sceneBuffers.msaa		= MetalInternalTexture2D::CreateMSRenderTexture(m_device, sceneSize, m_sceneBuffers.sampleCount);
				m_sceneBuffers.nonMSAA	= MetalInternalTexture2D::CreateRenderTexture(m_device, sceneSize);
			}
		}
		
		m_renderPipelineState.init(m_device, m_pShader);
		
		SIV3D_ENGINE(Shader)->init();
		SIV3D_ENGINE(EngineShader)->init();

		{
			const PipelineStateDesc pipelineStateDesc
			{
				.vs = SIV3D_ENGINE(EngineShader)->getVS(EngineVS::FullScreenTriangle).id(),
				.ps = SIV3D_ENGINE(EngineShader)->getPS(EnginePS::FullScreenTriangle).id(),
				.pixelFormat = static_cast<uint16>((MTL::PixelFormat)m_metalLayer.pixelFormat),
				.sampleCount = 1,
			};
			
			m_fullscreenTriangleRenderPipelineState = getRenderPipelineState().get(pipelineStateDesc);
		}

		beginFrame();
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginFrame
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_Metal::beginFrame()
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
		
		m_commandBuffer = m_commandQueue->commandBuffer();
	}

	////////////////////////////////////////////////////////////////
	//
	//	flush
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_Metal::flush()
	{
		m_pRenderer2D->flush(m_commandBuffer);
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

			{
				NS::SharedPtr<MTL::RenderPassDescriptor> renderPassDescriptor = NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init());
				
				MTL::RenderPassColorAttachmentDescriptor* cd = renderPassDescriptor->colorAttachments()->object(0);
				cd->setTexture(m_metalDrawable->texture());
				cd->setLoadAction(MTL::LoadActionClear);
				cd->setClearColor(MTL::ClearColor{ m_sceneStyle.letterboxColor.r, m_sceneStyle.letterboxColor.g, m_sceneStyle.letterboxColor.b, 1.0 });
				cd->setStoreAction(MTL::StoreActionStore);
				
				MTL::RenderCommandEncoder* renderCommandEncoder = m_commandBuffer->renderCommandEncoder(renderPassDescriptor.get());
				renderCommandEncoder->setRenderPipelineState(m_fullscreenTriangleRenderPipelineState);
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
				renderCommandEncoder->setFragmentTexture(m_sceneBuffers.nonMSAA.getTexture(), 0);
				renderCommandEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger{ 0 }, 3);
				renderCommandEncoder->endEncoding();
			}
			
			m_commandBuffer->presentDrawable(m_metalDrawable);
			
			__weak dispatch_semaphore_t semaphore = m_pRenderer2D->getSemaphore();
			m_commandBuffer->addCompletedHandler(^(MTL::CommandBuffer*) {
				dispatch_semaphore_signal(semaphore);
			});
			m_commandBuffer->commit();
			//m_commandBuffer->waitUntilCompleted();
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
			m_sceneBuffers.nonMSAA	= MetalInternalTexture2D::CreateRenderTexture(m_device, size);
		}
		else
		{
			m_sceneBuffers.msaa		= MetalInternalTexture2D::CreateMSRenderTexture(m_device, size, m_sceneBuffers.sampleCount);
			m_sceneBuffers.nonMSAA	= MetalInternalTexture2D::CreateRenderTexture(m_device, size);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneBufferSize
	//
	////////////////////////////////////////////////////////////////

	const Size& CRenderer_Metal::getSceneBufferSize() const noexcept
	{
		return m_sceneBuffers.getSceneTexture().size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLetterboxComposition
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, RectF> CRenderer_Metal::getLetterboxComposition() const noexcept
	{
		return SceneMisc::CalculateLetterboxComposition(getBackBufferSize(), getSceneBufferSize());
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

	////////////////////////////////////////////////////////////////
	//
	//	updateSceneBufferSize
	//
	////////////////////////////////////////////////////////////////

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
	//	getDevice
	//
	////////////////////////////////////////////////////////////////

	MTL::Device* CRenderer_Metal::getDevice() const
	{
		return m_device;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneSampleCount
	//
	////////////////////////////////////////////////////////////////

	uint32 CRenderer_Metal::getSceneSampleCount() const
	{
		return m_sceneBuffers.sampleCount;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneTextureMSAA
	//
	////////////////////////////////////////////////////////////////

	const MetalInternalTexture2D& CRenderer_Metal::getSceneTextureMSAA() const
	{
		return m_sceneBuffers.msaa;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneTextureNonMSAA
	//
	////////////////////////////////////////////////////////////////

	const MetalInternalTexture2D& CRenderer_Metal::getSceneTextureNonMSAA() const
	{
		return m_sceneBuffers.nonMSAA;
	}

	MetalRenderPipelineState& CRenderer_Metal::getRenderPipelineState()
	{
		return m_renderPipelineState;
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
 
