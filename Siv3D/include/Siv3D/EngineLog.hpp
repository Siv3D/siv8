//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "LogType.hpp"
# include "String.hpp"
# include "FormatLiteral.hpp"
# include "FmtExtension.hpp"

namespace s3d
{
	namespace Internal
	{
		void OutputEngineLog(LogType type, std::string_view s);

		void OutputEngineLog(LogType type, StringView s);

		class ScopedEngineLog
		{
		public:

			ScopedEngineLog(LogType type, std::string message);

			~ScopedEngineLog();

		private:

			std::string m_message;

			LogType m_type;
		};
	}
}

# if SIV3D_BUILD(DEBUG)

#	define LOG_ERROR(S)			s3d::Internal::OutputEngineLog(s3d::LogType::Error,   S)
#	define LOG_FAIL(S)			s3d::Internal::OutputEngineLog(s3d::LogType::Fail,    S)
#	define LOG_WARN(S)			s3d::Internal::OutputEngineLog(s3d::LogType::Warning, S)
#	define LOG_INFO(S)			s3d::Internal::OutputEngineLog(s3d::LogType::Info,    S)
#	define LOG_DEBUG(S)			s3d::Internal::OutputEngineLog(s3d::LogType::Debug,   S)
#	define LOG_SCOPED_DEBUG(S)	const s3d::Internal::ScopedEngineLog s3d_scoped_trace{ s3d::LogType::Debug, S }
#	define LOG_TRACE(S)			s3d::Internal::OutputEngineLog(s3d::LogType::Trace,	  S)
#	define LOG_SCOPED_TRACE(S)	const s3d::Internal::ScopedEngineLog s3d_scoped_trace{ s3d::LogType::Trace, S }
#	define LOG_TEST(S)			s3d::Internal::OutputEngineLog(s3d::LogType::App,	  S)

# else

#	define LOG_ERROR(S)			s3d::Internal::OutputEngineLog(s3d::LogType::Error,		S)
#	define LOG_FAIL(S)			s3d::Internal::OutputEngineLog(s3d::LogType::Fail,		S)
#	define LOG_WARN(S)			s3d::Internal::OutputEngineLog(s3d::LogType::Warning,	S)
#	define LOG_INFO(S)			s3d::Internal::OutputEngineLog(s3d::LogType::Info,		S)
#	define LOG_DEBUG(S)			((void)0)
#	define LOG_SCOPED_DEBUG(S)	((void)0)
#	define LOG_TRACE(S)			((void)0)
#	define LOG_SCOPED_TRACE(S)	((void)0)

# endif
