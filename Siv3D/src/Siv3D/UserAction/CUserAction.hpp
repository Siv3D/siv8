﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <atomic>
# include <Siv3D/UserAction.hpp>
# include "IUserAction.hpp"

namespace s3d
{
	class CUserAction final : public ISiv3DUserAction
	{
	public:

		void setTerminationTriggers(uint32 userActionFlags) noexcept override;

		uint32 getTerminationTriggers() noexcept override;

		uint32 getPreviousUserActions() noexcept override;

		void reportUserActions(uint32 userActions) noexcept override;

		bool shouldTerminate() noexcept override;

	private:

		std::atomic<uint32> m_currentUserActions{ UserAction::NoAction };

		uint32 m_previousUserActions = 0;

		uint32 m_terminationTriggers = UserAction::Default;
	};
}
