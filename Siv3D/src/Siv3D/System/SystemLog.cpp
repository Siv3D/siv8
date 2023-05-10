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

# include "SystemLog.hpp"
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/DateTime.hpp>
# include <Siv3D/Version.hpp>

namespace s3d
{
	namespace SystemLog
	{
		void Launch()
		{
			LOG_INFO(fmt::format("ℹ️ {}", DateTime::Now().format()));

			if constexpr (SIV3D_BUILD(DEBUG))
			{
				LOG_INFO(U"ℹ️ Siv3D Engine (" SIV3D_VERSION_STRING U") " SIV3D_PLATFORM_NAME U" [Debug build]");
			}
			else
			{
				LOG_INFO(U"ℹ️ Siv3D Engine (" SIV3D_VERSION_STRING U") " SIV3D_PLATFORM_NAME U" [Release build]");
			}

			LOG_INFO("⌛ Preparing for setup...");
		}

		void Exit()
		{
			LOG_INFO("✅ Siv3D engine has terminated");

			LOG_INFO(fmt::format("ℹ️ {}", DateTime::Now().format()));
		}
	}
}
