﻿//-----------------------------------------------
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
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Logger/ILogger.hpp>

namespace s3d
{
	namespace Internal
	{
		void OutputEngineLog(const LogType type, const StringView s)
		{
			if (Siv3DEngine::isActive())
			{
				SIV3D_ENGINE(Logger)->write(type, s);
			}
		}

		ScopedEngineLog::ScopedEngineLog(const LogType type, String message)
			: m_message{ std::move(message) }
			, m_type{ type }
		{
			if (Siv3DEngine::isActive())
			{
				SIV3D_ENGINE(Logger)->write(m_type, (m_message + U" ---"_s));
			}
		}

		ScopedEngineLog::~ScopedEngineLog()
		{
			if (Siv3DEngine::isActive())
			{
				SIV3D_ENGINE(Logger)->write(m_type, (U"--- "_s + m_message));
			}
		}
	}
}