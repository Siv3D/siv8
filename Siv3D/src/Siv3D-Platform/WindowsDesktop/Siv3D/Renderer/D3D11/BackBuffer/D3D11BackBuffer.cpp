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

# include "D3D11BackBuffer.hpp"
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	void D3D11BackBuffer::init(const D3D11Device& device, IDXGISwapChain1* swapChain)
	{
		LOG_SCOPED_DEBUG("D3D11BackBuffer::init()");

		m_device		= device.getDevice();
		m_context		= device.getContext();
		m_swapChain1	= swapChain;

		m_backBuffer	= D3D11InternalTexture2D::CreateFromSwapChain(m_device, m_swapChain1);
		
		{
			const Size sceneSize = Window::GetState().virtualSize;

			if (m_sampleCount == 1)
			{
				m_sceneBuffers.scene = D3D11InternalTexture2D::CreateRenderTexture(m_device, sceneSize);
			}
			else
			{
				m_sceneBuffers.scene = D3D11InternalTexture2D::CreateMSRenderTexture(m_device, sceneSize, m_sampleCount);
			}
		}

		clear(D3D11ClearTarget::Scene | D3D11ClearTarget::BackBuffer);
	}

	void D3D11BackBuffer::clear(const D3D11ClearTarget clearTarget)
	{
		if (clearTarget & D3D11ClearTarget::Scene)
		{
			m_sceneBuffers.scene.clear(m_context, m_backgroundColor);
		}

		if (clearTarget & D3D11ClearTarget::BackBuffer)
		{
			m_backBuffer.clear(m_context, ColorF{0.3,0.4,0.5});
		}
	}

	void D3D11BackBuffer::updateFromSceneBuffer()
	{
		unbindAllRenderTargets();

	}

	const Size& D3D11BackBuffer::getBackBufferSize() const noexcept
	{
		return m_backBuffer.size();
	}

	void D3D11BackBuffer::resizeBackBuffer(const Size backBufferSize)
	{
		assert((0 < backBufferSize.x) && (0 < backBufferSize.y));

		LOG_DEBUG(fmt::format("D3D11BackBuffer::resizeBackBuffer({})", backBufferSize));

		unbindAllRenderTargets();

		m_backBuffer.reset();
		{
			DXGI_SWAP_CHAIN_DESC1 desc{};
			m_swapChain1->GetDesc1(&desc);
			
			if (FAILED(m_swapChain1->ResizeBuffers(desc.BufferCount, backBufferSize.x, backBufferSize.y, desc.Format, desc.Flags)))
			{
				throw InternalEngineError{ "IDXGISwapChain->ResizeBuffers() failed" };
			}

			m_backBuffer = D3D11InternalTexture2D::CreateFromSwapChain(m_device, m_swapChain1);
			
			clear(D3D11ClearTarget::BackBuffer);
		}

		//updateSceneSize();




		//assert((0 < backBufferSize.x) && (0 < backBufferSize.y));

		//if (m_backBuffer.size() == backBufferSize)
		//{
		//	return;
		//}

		//LOG_DEBUG(fmt::format("D3D11BackBuffer::resizeBackBuffer({})", backBufferSize));

		////unbindAllRenderTargets();

		//m_sceneBuffers = {};

		//if (m_sampleCount == 1)
		//{
		//	m_sceneBuffers.scene = D3D11InternalTexture2D::CreateRenderTexture(m_device, backBufferSize);
		//}
		//else
		//{
		//	m_sceneBuffers.scene = D3D11InternalTexture2D::CreateMSRenderTexture(m_device, backBufferSize, m_sampleCount);
		//}

		//clear(D3D11ClearTarget::Scene | D3D11ClearTarget::BackBuffer);
	}

	void D3D11BackBuffer::unbindAllRenderTargets()
	{
		m_context->OMSetRenderTargets(0, nullptr, nullptr);
	}
}
