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
# include <Siv3D/PointVector.hpp>
# include <Siv3D/Renderer/D3D11/D3D11.hpp>

namespace s3d
{
	class D3D11InternalTexture2D
	{
	public:

		[[nodiscard]]
		const Size& size() const noexcept
		{
			return m_size;
		}

		[[nodiscard]]
		static D3D11InternalTexture2D CreateFromSwapChain(ID3D11Device* device, IDXGISwapChain1* swapChain1);

		[[nodiscard]]
		static D3D11InternalTexture2D CreateRenderTexture(ID3D11Device* device, const Size& size);

		[[nodiscard]]
		static D3D11InternalTexture2D CreateMSRenderTexture(ID3D11Device* device, const Size& size, uint32 sampleCount);

	private:

		ComPtr<ID3D11Texture2D> m_texture;

		ComPtr<ID3D11RenderTargetView> m_renderTargetView;

		ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

		Size m_size{ 0, 0 };
	};
}
