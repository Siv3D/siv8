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

	}

	void D3D11BackBuffer::updateFromSceneBuffer()
	{

	}

	const Size& D3D11BackBuffer::getBackBufferSize() const noexcept
	{
		return m_backBuffer.size();
	}

	void D3D11BackBuffer::resizeBackBuffer(const Size backBufferSize)
	{

	}
}
