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

namespace s3d
{
	class CRenderer_Metal final : public ISiv3DRenderer
	{
	public:

		~CRenderer_Metal() override;

		StringView getName() const override;

		void init() override;

		void clear() override;

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
		
	private:
		
		NSWindow* m_metalWindow = nullptr;
		
		MTL::Device* m_metalDevice = nullptr;
		
		CAMetalLayer* m_metalLayer = nullptr;

		SceneStyle m_sceneStyle;
		
		ResizeMode m_sceneResizeMode = ResizeMode::Virtual;
		
		CA::MetalDrawable* m_metalDrawable = nullptr;
		MTL::Library* m_metalDefaultLibrary = nullptr;
		MTL::CommandQueue* m_metalCommandQueue = nullptr;
		MTL::CommandBuffer* m_metalCommandBuffer = nullptr;
		MTL::Buffer* m_triangleVertexBuffer = nullptr;
		
		MetalInternalTexture2D m_sceneBuffer;
		MTL::Buffer* m_sceneVertexBuffer = nullptr;
		MTL::RenderPipelineState* m_metalRenderPSO1 = nullptr;
		MTL::RenderPipelineState* m_metalRenderPSO2 = nullptr;
		
		void sendRenderCommand();
		
		void resizeBackBuffer(Size backBufferSize);
		Size getBackBufferSize() const;
		
	};
}
