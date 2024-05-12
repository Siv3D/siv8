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

# include "CRenderer_D3D11.hpp"
# include <Siv3D/WindowState.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	CRenderer_D3D11::~CRenderer_D3D11()
	{
		LOG_SCOPED_DEBUG("CRenderer_D3D11::~CRenderer_D3D11()");
	}

	void CRenderer_D3D11::init()
	{
		LOG_SCOPED_DEBUG("CRenderer_D3D11::init()");

		m_device.init();

		const HWND hWnd = static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());
		const Size windowFrameBufferSize = SIV3D_ENGINE(Window)->getState().frameBufferSize;

		m_swapChain.init(hWnd, m_device, windowFrameBufferSize);
		m_backBuffer.init(m_device, m_swapChain.getSwapChain1());


		m_device.getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void CRenderer_D3D11::clear()
	{
		m_backBuffer.clear(D3D11ClearTarget::Scene | D3D11ClearTarget::BackBuffer);

		if (const Size windowFrameBufferSize = SIV3D_ENGINE(Window)->getState().frameBufferSize;
			windowFrameBufferSize != m_backBuffer.getBackBufferSize())
		{
			m_backBuffer.resizeBackBuffer(windowFrameBufferSize);
		}

		//pRenderer2D->update();
	}

	void CRenderer_D3D11::flush()
	{
		//pRenderer3D->flush();

		//pRenderer2D->flush();

		m_backBuffer.renderSceneToBackBuffer();
	}

	bool CRenderer_D3D11::present()
	{
		return m_swapChain.present(m_vSyncEnabled);
	}

	SceneStyle& CRenderer_D3D11::getSceneStyle() noexcept
	{
		return m_backBuffer.getSceneStyle();
	}
}
 