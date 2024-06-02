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

# pragma once
# include <Siv3D/Renderer/IRenderer.hpp>
# include <Siv3D/ResizeMode.hpp>
# include <Siv3D/Scene/SceneStyle.hpp>
# include <Siv3D/GLFW/GLFW.hpp>
# include "Metal.hpp"
# include "BackBuffer/MetalInternalTexture2D.hpp"
# include <Siv3D/Shader/Metal/CShader_Metal.hpp>

namespace s3d
{
	class CRenderer_Metal final : public ISiv3DRenderer
	{
	public:

		~CRenderer_Metal() override;

		StringView getName() const override;

		void init() override;

		void beginFrame() override;

		void flush() override;

		bool present() override;

		SceneStyle& getSceneStyle() noexcept override;

		void setSceneResizeMode(ResizeMode resizeMode) override;

		ResizeMode getSceneResizeMode() const noexcept override;

		void resizeSceneBuffer(Size size) override;

		const Size& getSceneBufferSize() const noexcept override;

		std::pair<double, RectF> getLetterboxComposition() const noexcept override;

		void updateSceneSize() override;

		void setVSyncEnabled(bool enabled) override;

		bool isVSyncEnabled() const override;

		void setLowLatencyMode(bool enabled) override;

		bool isLowLatencyMode() const override;
		
		void updateSceneBufferSize();

		MTL::Device* getDevice() const;
		
	private:

		CShader_Metal* m_pShader = nullptr;
		
		NSWindow* m_metalWindow = nullptr;
		
		MTL::Device* m_device = nullptr;
		
		CAMetalLayer* m_metalLayer = nullptr;

		SceneStyle m_sceneStyle;
		
		ResizeMode m_sceneResizeMode = ResizeMode::Virtual;

		bool m_vSyncEnabled = true;
		
		CA::MetalDrawable* m_metalDrawable = nullptr;

		NS::SharedPtr<MTL::CommandQueue> m_commandQueue;

		MTL::CommandBuffer* m_commandBuffer = nullptr;
		
		struct SceneBuffer
		{
			uint32 sampleCount = FromEnum(Scene::DefaultMSAASampleCount);;

			MetalInternalTexture2D msaa;

			MetalInternalTexture2D nonMSAA;

			MetalInternalTexture2D& getSceneTexture() noexcept
			{
				return ((sampleCount == 1) ? nonMSAA : msaa);
			}
			
			const MetalInternalTexture2D& getSceneTexture() const noexcept
			{
				return ((sampleCount == 1) ? nonMSAA : msaa);
			}
			
		} m_sceneBuffers;
			
		NS::SharedPtr<MTL::RenderPipelineState> m_pipeLineStateFullScreenTriangle;
		NS::SharedPtr<MTL::RenderPipelineState> m_pipeLineTestNoAA;
		NS::SharedPtr<MTL::RenderPipelineState> m_pipeLineTestMSAAx4;

		class VertexBufferManager
		{
		public:
			
			static constexpr size_t MaxInflightBuffers = 3;
			
			dispatch_semaphore_t frameBoundarySemaphore = dispatch_semaphore_create(MaxInflightBuffers);
		
			std::array<NS::SharedPtr<MTL::Buffer>, MaxInflightBuffers> vertexBuffers;

			void updateContent()
			{
				dispatch_semaphore_wait(frameBoundarySemaphore, DISPATCH_TIME_FOREVER);
				
				++m_bufferIndex %= MaxInflightBuffers;
			}
			
			dispatch_semaphore_t getSemaphore() const
			{
				return frameBoundarySemaphore;
			}
			
			const NS::SharedPtr<MTL::Buffer>& getCurrentVertexBuffer() const
			{
				return vertexBuffers[m_bufferIndex];
			}
			
		private:
			
			size_t m_bufferIndex = 0;
			
		} m_vertexBufferManager;

		void resizeBackBuffer(Size backBufferSize);

		Size getBackBufferSize() const;
	};
}
