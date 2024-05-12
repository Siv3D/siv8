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
	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::init(const D3D11Device& device, IDXGISwapChain1* swapChain)
	{
		LOG_SCOPED_DEBUG("D3D11BackBuffer::init()");

		m_device		= device.getDevice();
		m_context		= device.getContext();
		m_swapChain1	= swapChain;

		m_backBuffer	= D3D11InternalTexture2D::CreateFromSwapChain(m_device, m_swapChain1);
		
		{
			const Size sceneSize = Window::GetState().virtualSize;

			if (m_sceneSampleCount == 1)
			{
				m_sceneBuffers.scene = D3D11InternalTexture2D::CreateRenderTexture(m_device, sceneSize);
			}
			else
			{
				m_sceneBuffers.scene = D3D11InternalTexture2D::CreateMSRenderTexture(m_device, sceneSize, m_sceneSampleCount);
			}
		}

		clear(D3D11ClearTarget::Scene | D3D11ClearTarget::BackBuffer);
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::clear(const D3D11ClearTarget clearTarget)
	{
		if (clearTarget & D3D11ClearTarget::Scene)
		{
			m_sceneBuffers.scene.clear(m_context, m_sceneStyle.backgroundColor);
		}

		if (clearTarget & D3D11ClearTarget::BackBuffer)
		{
			m_backBuffer.clear(m_context, m_sceneStyle.letterboxColor);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneStyle
	//
	////////////////////////////////////////////////////////////////

	SceneStyle& D3D11BackBuffer::getSceneStyle() noexcept
	{
		return m_sceneStyle;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSceneResizeMode
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::setSceneResizeMode(const ResizeMode resizeMode)
	{
		m_sceneResizeMode = resizeMode;

		updateSceneBufferSize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneResizeMode
	//
	////////////////////////////////////////////////////////////////

	ResizeMode D3D11BackBuffer::getSceneResizeMode() const noexcept
	{
		return m_sceneResizeMode;
	}

	////////////////////////////////////////////////////////////////
	//
	//	updateSceneBufferSize
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::updateSceneBufferSize()
	{
		if (m_sceneResizeMode == ResizeMode::Actual)
		{
			resizeSceneBuffer(Window::GetState().frameBufferSize);
		}
		else if (m_sceneResizeMode == ResizeMode::Virtual)
		{
			resizeSceneBuffer(Window::GetState().virtualSize);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderSceneToBackBuffer
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::renderSceneToBackBuffer()
	{
		unbindAllRenderTargets();

		if (m_sceneSampleCount == 1)
		{
			if (m_backBuffer.size() == m_sceneBuffers.scene.size())
			{
				m_sceneBuffers.scene.copyTo(m_context, m_backBuffer);
			}
			else
			{
			//	setRenderTarget(m_backBuffer);
			//	m_context->PSSetShaderResources(0, 1, m_sceneBuffers.scene.getSRVPtr());
			//	pRenderer2D->drawFullScreenTriangle(m_sceneTextureFilter);

			//	D3D11::ResetPSShaderResources(m_context);
			}
		}
		else
		{
			if (m_backBuffer.size() == m_sceneBuffers.scene.size())
			{
				m_sceneBuffers.scene.resolveTo(m_context, m_backBuffer);
			}
			else
			{
			//	if (m_sceneBuffers.resolved.size() != m_sceneBuffers.scene.size())
			//	{
			//		m_sceneBuffers.resolved = D3D11InternalTexture2D::CreateRenderTargetTexture2D(m_device, m_sceneSize);
			//	}
			//	m_sceneBuffers.scene.resolveTo(m_context, m_sceneBuffers.resolved);

			//	setRenderTarget(m_backBuffer);
			//	m_context->PSSetShaderResources(0, 1, m_sceneBuffers.resolved.getSRVPtr());
			//	pRenderer2D->drawFullScreenTriangle(m_sceneTextureFilter);

			//	D3D11::ResetPSShaderResources(m_context);
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	resizeBackBuffer
	//
	////////////////////////////////////////////////////////////////

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

		updateSceneBufferSize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBackBufferSize
	//
	////////////////////////////////////////////////////////////////

	const Size& D3D11BackBuffer::getBackBufferSize() const noexcept
	{
		return m_backBuffer.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	resizeSceneBuffer
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::resizeSceneBuffer(const Size size)
	{
		assert((0 < size.x) && (0 < size.y));

		if (getSceneBufferSize() == size)
		{
			return;
		}

		LOG_DEBUG(fmt::format("D3D11BackBuffer::resizeSceneBuffer({})", size));

		unbindAllRenderTargets();

		m_sceneBuffers = {};

		if (m_sceneSampleCount == 1)
		{
			m_sceneBuffers.scene = D3D11InternalTexture2D::CreateRenderTexture(m_device, size);
		}
		else
		{
			m_sceneBuffers.scene = D3D11InternalTexture2D::CreateMSRenderTexture(m_device, size, m_sceneSampleCount);
		}

		clear(D3D11ClearTarget::Scene | D3D11ClearTarget::BackBuffer);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneBufferSize
	//
	////////////////////////////////////////////////////////////////

	const Size& D3D11BackBuffer::getSceneBufferSize() const noexcept
	{
		return m_sceneBuffers.scene.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::unbindAllRenderTargets()
	{
		m_context->OMSetRenderTargets(0, nullptr, nullptr);
	}
}
