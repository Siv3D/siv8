//-----------------------------------------------
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
# include <Siv3D/System/ISystem.hpp>

namespace s3d
{
	class CSystem final : public ISiv3DSystem
	{
	public:

		CSystem() = default;

		~CSystem() override;

		void preInit();

		void init() override;

		bool update() override;

	private:

		enum class SetupProgress : uint8
		{
			Uninitialized,
		
			EngineInitialized,
			
			WindowDisplayed,
		
		} m_setupProgress = SetupProgress::Uninitialized;

		std::atomic<bool> m_onDeviceChange{ false };

		bool m_shouldTerminate = false;
	};
}
