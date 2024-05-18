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
	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CRenderer_D3D11::~CRenderer_D3D11()
	{
		LOG_SCOPED_DEBUG("CRenderer_D3D11::~CRenderer_D3D11()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	getName
	//
	////////////////////////////////////////////////////////////////

	StringView CRenderer_D3D11::getName() const
	{
		static constexpr StringView name{ U"D3D11" };
		return name;
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	flush
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_D3D11::flush()
	{
		//pRenderer3D->flush();

		//pRenderer2D->flush();

		m_backBuffer.renderSceneToBackBuffer();
	}

	////////////////////////////////////////////////////////////////
	//
	//	present
	//
	////////////////////////////////////////////////////////////////

	bool CRenderer_D3D11::present()
	{
		return m_swapChain.present(m_vSyncEnabled);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneStyle
	//
	////////////////////////////////////////////////////////////////

	SceneStyle& CRenderer_D3D11::getSceneStyle() noexcept
	{
		return m_backBuffer.getSceneStyle();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSceneResizeMode
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_D3D11::setSceneResizeMode(const ResizeMode resizeMode)
	{
		m_backBuffer.setSceneResizeMode(resizeMode);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneResizeMode
	//
	////////////////////////////////////////////////////////////////

	ResizeMode CRenderer_D3D11::getSceneResizeMode() const noexcept
	{
		return m_backBuffer.getSceneResizeMode();
	}

	////////////////////////////////////////////////////////////////
	//
	//	resizeSceneBuffer
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_D3D11::resizeSceneBuffer(const Size size)
	{
		m_backBuffer.resizeSceneBuffer(size);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneBufferSize
	//
	////////////////////////////////////////////////////////////////

	const Size& CRenderer_D3D11::getSceneBufferSize() const noexcept
	{
		return m_backBuffer.getSceneBufferSize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLetterboxComposition
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, RectF> CRenderer_D3D11::getLetterboxComposition() const noexcept
	{
		return m_backBuffer.getLetterboxComposition();
	}

	////////////////////////////////////////////////////////////////
	//
	//	updateSceneSize
	//
	////////////////////////////////////////////////////////////////

	void CRenderer_D3D11::updateSceneSize()
	{
		m_backBuffer.updateSceneSize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getDevice
	//
	////////////////////////////////////////////////////////////////

	const D3D11Device& CRenderer_D3D11::getDevice() const noexcept
	{
		return m_device;
	}
}
 