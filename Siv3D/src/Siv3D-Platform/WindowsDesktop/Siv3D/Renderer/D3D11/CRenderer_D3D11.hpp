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
# include "BlendState/D3D11BlendState.hpp"
# include "RasterizerState/D3D11RasterizerState.hpp"
# include "SamplerState/D3D11SamplerState.hpp"
# include "DepthStencilState/D3D11DepthStencilState.hpp"

namespace s3d
{
	class CRenderer_D3D11 final : public ISiv3DRenderer
	{
	public:

		~CRenderer_D3D11() override;

		[[nodiscard]]
		StringView getName() const override;

		void init() override;

		void clear() override;

		void flush() override;

		bool present() override;

		[[nodiscard]]
		SceneStyle& getSceneStyle() noexcept override;

		void setSceneResizeMode(ResizeMode resizeMode) override;

		[[nodiscard]]
		ResizeMode getSceneResizeMode() const noexcept override;

		void resizeSceneBuffer(Size size) override;

		[[nodiscard]]
		const Size& getSceneBufferSize() const noexcept override;

		[[nodiscard]]
		std::pair<double, RectF> getLetterboxComposition() const noexcept override;

		void updateSceneSize() override;

		void setVSyncEnabled(bool enabled) override;

		[[nodiscard]]
		bool isVSyncEnabled() const override;

		void setLowLatencyMode(bool enabled) override;

		[[nodiscard]]
		bool isLowLatencyMode() const override;

		[[nodiscard]]
		const D3D11Device& getDevice() const noexcept;

		[[nodiscard]]
		D3D11BlendState& getBlendState() noexcept;

		[[nodiscard]]
		D3D11RasterizerState& getRasterizerState() noexcept;

		[[nodiscard]]
		D3D11SamplerState& getSamplerState() noexcept;

		[[nodiscard]]
		D3D11DepthStencilState& getDepthStencilState() noexcept;

	private:

		D3D11Device m_device;

		D3D11SwapChain m_swapChain;

		D3D11BackBuffer m_backBuffer;

		D3D11BlendState m_blendState;

		D3D11RasterizerState m_rasterizerState;

		D3D11SamplerState m_samplerState;

		D3D11DepthStencilState m_depthStencilState;
	};
}
