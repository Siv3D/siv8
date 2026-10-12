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
# include <Siv3D/Common.hpp>
# include <Siv3D/Scene.hpp>
# include <Siv3D/EngineOption.hpp>
# include <Siv3D/Graphics.hpp>
# include <Siv3D/Scene/SceneStyle.hpp>
# include <Siv3D/Scene/SceneUtility.hpp>
# include "../Device/D3D11Device.hpp"
# include "D3D11ClearTarget.hpp"
# include "D3D11InternalTexture2D.hpp"

namespace s3d
{
	class CRenderer_D3D11;

	class D3D11BackBuffer
	{
	public:

		void init(const D3D11Device& device, IDXGISwapChain1* swapChain, CRenderer_D3D11* pRenderer);

		void clear(D3D11ClearTarget clearTarget);

		[[nodiscard]]
		SceneStyle& getSceneStyle() noexcept;

		void configureScene(const SceneConfig& config);

		[[nodiscard]]
		SceneConfig getSceneConfig() const noexcept;

		void setSceneSampleCount(uint32 sampleCount);

		[[nodiscard]]
		uint32 getSceneSampleCount() const noexcept;

		void updateSceneBufferSize();

		void renderSceneToBackBuffer();

		void resizeBackBuffer(Size backBufferSize);

		[[nodiscard]]
		const Size& getBackBufferSize() const noexcept;

		const Size& getSceneSize() const noexcept;

		[[nodiscard]]
		const Size& getSceneBufferSize() const noexcept;

		std::pair<double, RectF> getLetterboxComposition() const noexcept;

		void bindSceneTextureAsRenderTarget();

		std::pair<ID3D11Texture2D*, Size> getCaptureTexture();

		[[nodiscard]]
		ID3D11Texture2D* getBackBufferTexture() const noexcept
		{
			return m_backBuffer.getTexture();
		}

		[[nodiscard]]
		ID3D11Texture2D* getSceneTexture() const noexcept
		{
			return m_sceneBuffers.getSceneTexture().getTexture();
		}

	private:
		
		ID3D11Device* m_device			= nullptr;

		ID3D11DeviceContext* m_context	= nullptr;

		IDXGISwapChain1* m_swapChain1	= nullptr;

		CRenderer_D3D11* m_pRenderer	= nullptr;

		SceneStyle m_sceneStyle;

		SceneMisc::RenderState m_sceneState;

		D3D11InternalTexture2D m_backBuffer;

		struct SceneBuffer
		{
			uint32 sampleCount = FromEnum(Scene::DefaultMSAASampleCount);

			D3D11InternalTexture2D msaa;
		
			D3D11InternalTexture2D nonMSAA;

			// Valid until the scene is cleared or bound for drawing.
			bool resolved = false;

			D3D11InternalTexture2D& getSceneTexture() noexcept
			{
				return ((sampleCount == 1) ? nonMSAA : msaa);
			}
			
			const D3D11InternalTexture2D& getSceneTexture() const noexcept
			{
				return ((sampleCount == 1) ? nonMSAA : msaa);
			}
		
		} m_sceneBuffers;

		/// @brief レンダーターゲットをバインドします。
		/// @param rtv バインドするレンダーターゲット
		void bindRenderTarget(ID3D11RenderTargetView* const rtv);

		/// @brief すべてのレンダーターゲットのバインドを解除します。
		void unbindAllRenderTargets();

		void bindTextureAsPSResource(ID3D11ShaderResourceView* const srv);

		void unbindAllPSTextures();

		void drawFullScreenTriangle();

		void resolveScene();

		[[nodiscard]]
		SceneBuffer createSceneBuffers(Size size, uint32 sampleCount) const;

		void applySceneState(const SceneMisc::RenderState& state, uint32 sampleCount);
	};
}
