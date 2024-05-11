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
# include <Siv3D/Graphics.hpp>
# include "../Device/D3D11Device.hpp"
# include "D3D11ClearTarget.hpp"
# include "D3D11InternalTexture2D.hpp"

namespace s3d
{
	class D3D11BackBuffer
	{
	public:

		void init(const D3D11Device& device, IDXGISwapChain1* swapChain);

		void clear(D3D11ClearTarget clearTarget);

		void updateFromSceneBuffer();

		[[nodiscard]]
		const Size& getBackBufferSize() const noexcept;

		void resizeBackBuffer(Size backBufferSize);

	private:
		
		ID3D11Device* m_device			= nullptr;

		ID3D11DeviceContext* m_context	= nullptr;

		IDXGISwapChain1* m_swapChain1	= nullptr;

		uint32 m_sampleCount			= Graphics::DefaultSampleCount;

		D3D11InternalTexture2D m_backBuffer;

		struct SceneBuffer
		{
			D3D11InternalTexture2D scene;
		
			D3D11InternalTexture2D resolved;
		
		} m_sceneBuffers;
	};
}
