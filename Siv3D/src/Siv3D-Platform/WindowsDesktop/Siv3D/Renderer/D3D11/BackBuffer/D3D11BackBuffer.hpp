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
# include <Siv3D/Common.hpp>
# include <Siv3D/Scene.hpp>
# include <Siv3D/EngineOption.hpp>
# include <Siv3D/Graphics.hpp>
# include <Siv3D/Scene/SceneStyle.hpp>
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

		void setSceneResizeMode(ResizeMode resizeMode);

		[[nodiscard]]
		ResizeMode getSceneResizeMode() const noexcept;

		void updateSceneBufferSize();

		void renderSceneToBackBuffer();

		void resizeBackBuffer(Size backBufferSize);

		[[nodiscard]]
		const Size& getBackBufferSize() const noexcept;

		void resizeSceneBuffer(Size size);

		[[nodiscard]]
		const Size& getSceneBufferSize() const noexcept;

		std::pair<double, RectF> getLetterboxComposition() const noexcept;

		void bindSceneTextureAsRenderTarget();

	private:
		
		ID3D11Device* m_device			= nullptr;

		ID3D11DeviceContext* m_context	= nullptr;

		IDXGISwapChain1* m_swapChain1	= nullptr;

		CRenderer_D3D11* m_pRenderer	= nullptr;

		SceneStyle m_sceneStyle;

		ResizeMode m_sceneResizeMode	= Scene::DefaultResizeMode;

		D3D11InternalTexture2D m_backBuffer;

		struct SceneBuffer
		{
			uint32 sampleCount = FromEnum(Scene::DefaultMSAASampleCount);

			D3D11InternalTexture2D msaa;
		
			D3D11InternalTexture2D nonMSAA;

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
	};
}
