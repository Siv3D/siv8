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

# include "CWindow.hpp"
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	CWindow::~CWindow()
	{
		LOG_SCOPED_DEBUG("CWindow::~CWindow()");

	}

	void CWindow::init()
	{
		LOG_SCOPED_DEBUG("CWindow::init()");

	}

	void CWindow::update()
	{


	}

	void CWindow::setWindowTitle(const String& title)
	{

	}

	const String& CWindow::getWindowTitle() const noexcept
	{
		static String windowTitle;
		return(windowTitle);
	}

	void* CWindow::getHandle() const noexcept
	{
		return nullptr;
	}
}
