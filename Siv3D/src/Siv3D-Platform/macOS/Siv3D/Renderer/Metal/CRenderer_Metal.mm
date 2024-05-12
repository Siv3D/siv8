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

# include "CRenderer_Metal.hpp"
# include <Siv3D/WindowState.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	CRenderer_Metal::~CRenderer_Metal()
	{
		LOG_SCOPED_DEBUG("CRenderer_Metal::~CRenderer_Metal()");
		
		if (m_metalDevice)
		{
			m_metalDevice->release();
		}
	}

	void CRenderer_Metal::init()
	{
		LOG_SCOPED_DEBUG("CRenderer_Metal::init()");

		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
		m_metalWindow = ::glfwGetCocoaWindow(glfwWindow);

		m_metalDevice = MTL::CreateSystemDefaultDevice();
		m_metalLayer = [CAMetalLayer layer];
		m_metalLayer.device = (__bridge id<MTLDevice>)m_metalDevice;
		m_metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
		m_metalWindow.contentView.layer = m_metalLayer;
		m_metalWindow.contentView.wantsLayer = YES;
	}

	void CRenderer_Metal::clear()
	{

	}

	void CRenderer_Metal::flush()
	{

	}

	bool CRenderer_Metal::present()
	{
		return(true);
	}

	SceneStyle& CRenderer_Metal::getSceneStyle() noexcept
	{
		return(m_sceneStyle);
	}

	void CRenderer_Metal::setSceneResizeMode(const ResizeMode resizeMode)
	{

	}

	ResizeMode CRenderer_Metal::getSceneResizeMode() const noexcept
	{
		return m_sceneResizeMode;
	}

	void CRenderer_Metal::resizeSceneBuffer(const Size size)
	{

	}

	const Size& CRenderer_Metal::getSceneBufferSize() const noexcept
	{
		return(m_sceneBufferSize);
	}
}
 
