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

# include <Siv3D/EngineLog.hpp>
# include "CUserAction.hpp"

namespace s3d
{
	namespace detail
	{
		static void LogTermination(const uint32 triggeredActions)
		{
			if (triggeredActions & UserAction::CloseButtonClicked)
			{
				LOG_INFO("🚪 System::Update(): `UserAction::CloseButtonClicked` has occurred");
			}
			else if (triggeredActions & UserAction::EscapeKeyDown)
			{
				LOG_INFO("🚪 System::Update(): `UserAction::EscapeKeyDown` has occurred");
			}
			else if (triggeredActions & UserAction::WindowDeactivated)
			{
				LOG_INFO("🚪 System::Update(): `UserAction::WindowDeactivated` has occurred");
			}
			else if (triggeredActions & UserAction::AnyKeyDown)
			{
				LOG_INFO("🚪 System::Update(): `UserAction::AnyKeyDown` has occurred");
			}
			else if (triggeredActions & UserAction::MouseButtonDown)
			{
				LOG_INFO("🚪 System::Update(): `UserAction::MouseButtonDown` has occurred");
			}
			else if (triggeredActions & UserAction::SystemExitCalled)
			{
				LOG_INFO("🚪 System::Update(): System::Exit() has called");
			}
		}
	}

	void CUserAction::setTerminationTriggers(const uint32 userActionFlags) noexcept
	{
		m_terminationTriggers = userActionFlags;
	}

	uint32 CUserAction::getTerminationTriggers() noexcept
	{
		return m_terminationTriggers;
	}

	uint32 CUserAction::getPreviousUserActions() noexcept
	{
		return m_previousUserActions;
	}

	void CUserAction::reportUserActions(const uint32 userActions) noexcept
	{
		m_currentUserActions.fetch_or(userActions);
	}

	bool CUserAction::shouldTerminate() noexcept
	{
		m_previousUserActions = m_currentUserActions.exchange(UserAction::NoAction);

		if (const uint32 triggeredActions = (m_previousUserActions & (UserAction::SystemExitCalled | m_terminationTriggers)))
		{
			detail::LogTermination(triggeredActions);
			return true;
		}
		else
		{
			return false;
		}
	}
}
