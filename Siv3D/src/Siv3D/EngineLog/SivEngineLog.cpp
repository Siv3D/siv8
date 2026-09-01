//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Logger/ILogger.hpp>

namespace s3d
{
	namespace Internal
	{
		void OutputEngineLog(const LogLevel logLevel, const std::string_view s)
		{
			if (Siv3DEngine::isNull())
			{
				return;
			}

			if (const auto pLogger = SIV3D_ENGINE(Logger))
			{
				pLogger->writeln(logLevel, s);
			}
		}

		void OutputEngineLog(const LogLevel logLevel, const StringView s)
		{
			if (Siv3DEngine::isNull())
			{
				return;
			}

			if (const auto pLogger = SIV3D_ENGINE(Logger))
			{
				pLogger->writeln(logLevel, s);
			}
		}

		ScopedEngineLog::ScopedEngineLog(const LogLevel logLevel, std::string message)
			: m_message{ std::move(message) }
			, m_logLevel{ logLevel }
		{
			if (const auto pLogger = SIV3D_ENGINE(Logger))
			{
				pLogger->writeln(m_logLevel, (m_message + " ---"));
			}
		}

		ScopedEngineLog::~ScopedEngineLog()
		{
			if (const auto pLogger = SIV3D_ENGINE(Logger))
			{
				pLogger->writeln(m_logLevel, ("--- " + m_message));
			}
		}
	}
}
