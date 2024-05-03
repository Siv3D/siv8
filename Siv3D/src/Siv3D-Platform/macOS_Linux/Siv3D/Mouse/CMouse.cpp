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

# include "CMouse.hpp"
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	~CMouse
	//
	////////////////////////////////////////////////////////////////

	CMouse::~CMouse()
	{
		LOG_SCOPED_DEBUG("CMouse::~CMouse()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CMouse::init()
	{
		LOG_SCOPED_DEBUG("CMouse::init()");

		m_window = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	bool CMouse::update()
	{
		return true;
	}
}
