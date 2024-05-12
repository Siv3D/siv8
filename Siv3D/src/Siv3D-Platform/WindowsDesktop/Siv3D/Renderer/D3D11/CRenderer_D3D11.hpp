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
# include "Device/D3D11Device.hpp"
# include "SwapChain/D3D11SwapChain.hpp"
# include "BackBuffer/D3D11BackBuffer.hpp"

namespace s3d
{
	class CRenderer_D3D11 final : public ISiv3DRenderer
	{
	public:

		~CRenderer_D3D11() override;

		void init() override;

		void clear() override;

		void flush() override;

		bool present() override;

		SceneStyle& getSceneStyle() noexcept override;

		void setSceneResizeMode(ResizeMode resizeMode) override;

		ResizeMode getSceneResizeMode() const noexcept override;

		void resizeSceneBuffer(Size size) override;

		const Size& getSceneBufferSize() const noexcept override;

	private:

		D3D11Device m_device;

		D3D11SwapChain m_swapChain;

		D3D11BackBuffer m_backBuffer;

		bool m_vSyncEnabled = true;
	};
}
