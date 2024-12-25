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
# include <Siv3D/Image.hpp>
# include <Siv3D/Scene/SceneStyle.hpp>
# include <Siv3D/GLFW/GLFW.hpp>
# include "Metal.hpp"
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
	
		void captureScreenshot() override;

		const Image& getScreenCapture() const override;
		
		void updateSceneBufferSize();

		[[nodiscard]]
		MTL::Device* getDevice() const noexcept;

		[[nodiscard]]		
		uint32 getSceneSampleCount() const noexcept;

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
		
		Image m_screenCapture;
		
		MetalRenderPipelineState m_renderPipelineState;
		
		MetalSamplerState m_samplerState;

		const MTL::RenderPipelineState* m_fullscreenTriangleRenderPipelineState = nullptr;
		
		void resizeBackBuffer(Size backBufferSize);

		Size getBackBufferSize() const;
	};
}
