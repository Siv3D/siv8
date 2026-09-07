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

# include <iostream>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Time.hpp>
# include "CLogger.hpp"

namespace s3d
{
	namespace detail
	{
		static constexpr std::array LogLevelStrings =
		{
			": [error] ",
			": [fail] ",
			": [warning] ",
			": [log] ",
			": [info] ",
			": [debug] ",
			": [trace] ",
		};
	}

	CLogger::CLogger()
		: m_outputLevel{ SIV3D_BUILD(DEBUG) ? LogLevel::Trace : LogLevel::Info } {}

	void CLogger::writeln(const std::string_view s)
	{
		std::string output;
		output.reserve(s.size() + 1);
		output.append(s);
		
		writeImpl(output);
	}

	void CLogger::writeln(const StringView s)
	{
		writeImpl(Unicode::ToUTF8(s));
	}

	void CLogger::writeln(const LogLevel logLevel, const std::string_view s)
	{
		if (m_outputLevel < logLevel)
		{
			return;
		}

		std::string output;
		output.append(std::to_string(Time::GetMillisec()));
		output.append(detail::LogLevelStrings[FromEnum(logLevel)]);
		output.append(s);

		writeImpl(output);
	}

	void CLogger::writeln(const LogLevel logLevel, const StringView s)
	{
		if (m_outputLevel < logLevel)
		{
			return;
		}
			
		std::string output;
		output.append(std::to_string(Time::GetMillisec()));
		output.append(detail::LogLevelStrings[FromEnum(logLevel)]);
		output.append(Unicode::ToUTF8(s));

		writeImpl(output);
	}

	void CLogger::setOutputLevel(const LogLevel logLevel) noexcept
	{
		m_outputLevel.store(logLevel);
	}

	LogLevel CLogger::getOutputLevel() const noexcept
	{
		return m_outputLevel.load();
	}

	void CLogger::writeImpl(const std::string& s)
	{
		std::lock_guard lock{ m_mutex };
		{
		# if SIV3D_PLATFORM(WEB)
		
			std::cout << s << std::endl;
		
		# else
		
			std::clog << s << std::endl;
		
		# endif
		}
	}
}
