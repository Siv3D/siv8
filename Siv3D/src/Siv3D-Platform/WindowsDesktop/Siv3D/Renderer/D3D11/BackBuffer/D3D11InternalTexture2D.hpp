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
# include <Siv3D/ColorHSV.hpp>
# include <Siv3D/Renderer/D3D11/D3D11.hpp>

namespace s3d
{
	class D3D11InternalTexture2D
	{
	public:

		void clear(ID3D11DeviceContext* context, const ColorF& color);

		void reset();

		void copyTo(ID3D11DeviceContext* context, D3D11InternalTexture2D& dst);

		void resolveTo(ID3D11DeviceContext* context, D3D11InternalTexture2D& dst);

		[[nodiscard]]
		bool isEmpty() const noexcept
		{
			return m_size.isZero();
		}

		[[nodiscard]]
		operator bool() const noexcept
		{
			return (not isEmpty());
		}

		[[nodiscard]]
		const Size& size() const noexcept
		{
			return m_size;
		}

		[[nodiscard]]
		ID3D11Texture2D* getTexture() const noexcept
		{
			return m_texture.Get();
		}

		[[nodiscard]]
		ID3D11ShaderResourceView* getSRV() const noexcept
		{
			return m_shaderResourceView.Get();
		}

		[[nodiscard]]
		ID3D11RenderTargetView* getRTV() const noexcept
		{
			return m_renderTargetView.Get();
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

		DXGI_SAMPLE_DESC m_sampleDesc{ 1, 0 };
	};
}
