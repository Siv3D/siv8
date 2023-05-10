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

# pragma once
# include <atomic>
# include <Siv3D/System/ISystem.hpp>

namespace s3d
{
	class CSystem final : public ISiv3DSystem
	{
	public:

		CSystem();

		~CSystem() override;

		void init() override;

		bool update() override;

		void onDeviceChange();

	private:

		enum class SetupState
		{
			Uninitialized,
			Initialized,
			WindowDisplayed,
		} m_setupState = SetupState::Uninitialized;

		std::atomic_flag m_onDeviceChange;

		bool m_shouldTerminate = false;
	};
}
