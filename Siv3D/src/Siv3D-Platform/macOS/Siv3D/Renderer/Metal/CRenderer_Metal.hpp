//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Renderer/IRenderer.hpp>
# include <Siv3D/SceneConfig.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/Scene/SceneStyle.hpp>
# include <Siv3D/Scene/SceneUtility.hpp>
# include <Siv3D/GLFW/GLFW.hpp>
# include "Metal.hpp"
# include "MetalFrameContext.hpp"
# include "BackBuffer/MetalInternalTexture2D.hpp"
# include "RenderPipelineState/MetalRenderPipelineState.hpp"
# include "SamplerState/MetalSamplerState.hpp"

namespace s3d
{
	class CTexture_Metal;
	class CShader_Metal;
	class CRenderer2D_Metal;

	class CRenderer_Metal final : public ISiv3DRenderer
	{
	public:

		~CRenderer_Metal() override;

		[[nodiscard]]
		StringView getName() const override;

		[[nodiscard]]
		EngineOption::Renderer getRendererType() const noexcept override;

		void init() override;

		void waitForFrame() override;

		void beginFrame() override;

		void flush() override;

		bool present() override;

		[[nodiscard]]
		SceneStyle& getSceneStyle() noexcept override;

		void configureScene(const SceneConfig& config) override;

		[[nodiscard]]
		SceneConfig getSceneConfig() const noexcept override;

		void setSceneSampleCount(uint32 sampleCount) override;

		const Size& getSceneSize() const noexcept override;

		[[nodiscard]]
		const Size& getSceneBufferSize() const noexcept override;

		[[nodiscard]]
		std::pair<double, RectF> getLetterboxComposition() const noexcept override;

		void updateSceneSize() override;

		void setVSyncEnabled(bool enabled) override;

		[[nodiscard]]
		bool isVSyncEnabled() const override;

		[[nodiscard]]
		bool captureScreenshot() override;

		[[nodiscard]]
		const Image& getScreenCapture() const override;
		
		void updateSceneBufferSize();

		void renderSceneToBackBuffer(MTL::CommandBuffer* commandBuffer, MTL::Texture* target);

		[[nodiscard]]
		MTL::Device* getDevice() const noexcept;
		
		[[nodiscard]]
		MTL::CommandQueue* getCommandQueue() const noexcept;

		[[nodiscard]]
		const MetalFrameContext& getFrameContext() const noexcept;

		[[nodiscard]]		
		uint32 getSceneSampleCount() const noexcept override;

		[[nodiscard]]
		const MetalInternalTexture2D& getSceneTextureMSAA() const noexcept;

		[[nodiscard]]
		const MetalInternalTexture2D& getSceneTextureNonMSAA() const noexcept;
		
		[[nodiscard]]
		MetalRenderPipelineState& getRenderPipelineState() noexcept;
		
		[[nodiscard]]
		MetalSamplerState& getSamplerState() noexcept;
		
	private:

		CTexture_Metal* m_pTexture = nullptr;
		
		CShader_Metal* m_pShader = nullptr;
		
		CRenderer2D_Metal* m_pRenderer2D = nullptr;
		
		NSWindow* m_metalWindow = nullptr;
		
		MTL::Device* m_device = nullptr;
		
		CAMetalLayer* m_metalLayer = nullptr;

		SceneStyle m_sceneStyle;
		
		SceneMisc::RenderState m_sceneState;

		bool m_vSyncEnabled = true;
		
		NS::SharedPtr<MTL::CommandQueue> m_commandQueue;

		MetalFrameContext m_frameContext;
		
		struct SceneBuffer
		{
			uint32 sampleCount = FromEnum(Scene::DefaultMSAASampleCount);

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
		
		Image m_screenCapture;
		
		MetalRenderPipelineState m_renderPipelineState;
		
		MetalSamplerState m_samplerState;

		const MTL::RenderPipelineState* m_fullscreenTriangleRenderPipelineState = nullptr;
		
		[[nodiscard]]
		SceneBuffer createSceneBuffers(Size size, uint32 sampleCount) const;

		void applySceneState(const SceneMisc::RenderState& state, uint32 sampleCount);

		void resizeBackBuffer(Size backBufferSize);

		Size getBackBufferSize() const;
	};
}
