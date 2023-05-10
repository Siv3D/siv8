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

		SystemLog::Launch();
		//SystemMisc::Init();

		//SIV3D_ENGINE(Resource)->init();
		//SIV3D_ENGINE(Profiler)->init();
	}

	bool CSystem::update()
	{
		if (m_shouldTerminate)
		{
			return false;
		}

		return true;
	}

	void CSystem::onDeviceChange()
	{
		LOG_TRACE("CSystem::onDeviceChange()");
		m_onDeviceChange.test_and_set();
	}
}
