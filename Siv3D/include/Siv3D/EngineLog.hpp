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

# pragma once
# include "LogLevel.hpp"
# include "String.hpp"
# include "FormatLiteral.hpp"

namespace s3d
{
	namespace Internal
	{
		void OutputEngineLog(LogLevel logLevel, std::string_view s);

		void OutputEngineLog(LogLevel logLevel, StringView s);

		class ScopedEngineLog
		{
		public:

			ScopedEngineLog(LogLevel logLevel, std::string message);

			~ScopedEngineLog();

		private:

			std::string m_message;

			LogLevel m_logLevel;
		};
	}
}

# if SIV3D_BUILD(DEBUG)

#	define LOG_ERROR(S)			s3d::Internal::OutputEngineLog(s3d::LogLevel::Error,   S)
#	define LOG_FAIL(S)			s3d::Internal::OutputEngineLog(s3d::LogLevel::Fail,    S)
#	define LOG_WARN(S)			s3d::Internal::OutputEngineLog(s3d::LogLevel::Warning, S)
#	define LOG_INFO(S)			s3d::Internal::OutputEngineLog(s3d::LogLevel::Info,    S)
#	define LOG_DEBUG(S)			s3d::Internal::OutputEngineLog(s3d::LogLevel::Debug,   S)
#	define LOG_SCOPED_DEBUG(S)	const s3d::Internal::ScopedEngineLog s3d_scoped_trace{ s3d::LogLevel::Debug, S }
#	define LOG_TRACE(S)			s3d::Internal::OutputEngineLog(s3d::LogLevel::Trace,	  S)
#	define LOG_SCOPED_TRACE(S)	const s3d::Internal::ScopedEngineLog s3d_scoped_trace{ s3d::LogLevel::Trace, S }
#	define LOG_TEST(S)			s3d::Internal::OutputEngineLog(s3d::LogLevel::App,	  S)

# else

#	define LOG_ERROR(S)			s3d::Internal::OutputEngineLog(s3d::LogLevel::Error,		S)
#	define LOG_FAIL(S)			s3d::Internal::OutputEngineLog(s3d::LogLevel::Fail,		S)
#	define LOG_WARN(S)			s3d::Internal::OutputEngineLog(s3d::LogLevel::Warning,	S)
#	define LOG_INFO(S)			s3d::Internal::OutputEngineLog(s3d::LogLevel::Info,		S)
#	define LOG_DEBUG(S)			((void)0)
#	define LOG_SCOPED_DEBUG(S)	((void)0)
#	define LOG_TRACE(S)			((void)0)
#	define LOG_SCOPED_TRACE(S)	((void)0)

# endif
