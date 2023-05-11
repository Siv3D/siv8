//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "CSystem.hpp"
# include <Siv3D/Window/CWindow.hpp>
# include <Siv3D/UserAction/IUserAction.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/System/SystemLog.hpp>

namespace s3d
{
	CSystem::CSystem() = default;

	CSystem::~CSystem()
	{
		LOG_SCOPED_TRACE("CSystem::~CSystem()");

		//SystemMisc::Destroy();
		SystemLog::Exit();
	}

	void CSystem::init()
	{
		LOG_SCOPED_TRACE("CSystem::init()");



		m_setupStep = SetupStep::Initialized;
		LOG_INFO("✅ Siv3D engine has initialized");
	}

	bool CSystem::update()
	{
		if (m_shouldTerminate)
		{
			return false;
		}

		if (m_setupStep == SetupStep::Initialized)
		{
			if (auto pWindow = static_cast<CWindow*>(SIV3D_ENGINE(Window)))
			{
				pWindow->show();
			}

			m_setupStep = SetupStep::WindowDisplayed;
		}

		if (SIV3D_ENGINE(UserAction)->shouldTerminate())
		{
			m_shouldTerminate = true;
			return false;
		}





		return true;
	}

	void CSystem::preInit()
	{
		LOG_SCOPED_TRACE("CSystem::preInit()");

		SystemLog::Launch();
		//SystemMisc::Init();

		//SIV3D_ENGINE(Resource)->init();
		//SIV3D_ENGINE(Profiler)->init();
	}

	void CSystem::onDeviceChange()
	{
		LOG_TRACE("CSystem::onDeviceChange()");
		m_onDeviceChange.test_and_set();
	}
}
