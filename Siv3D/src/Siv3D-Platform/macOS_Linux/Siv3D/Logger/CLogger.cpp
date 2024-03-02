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

# include <Siv3D/Unicode.hpp>
# include <Siv3D/Time.hpp>
# include "CLogger.hpp"

namespace s3d
{
	namespace detail
	{
		static constexpr std::array LogTypeStrings =
		{
			": [error] ",
			": [fail] ",
			": [warning] ",
			": [log] ",
			": [info] ",
			": [trace] ",
			": [verbose] ",
		};
	}

	CLogger::CLogger()
		: m_outputLevel{ SIV3D_BUILD(DEBUG) ? LogType::Trace : LogType::Info } {}

	void CLogger::write(const std::string_view s)
	{
		std::string output;
		output.reserve(s.size() + 1);
		output.append(s);
		output.push_back('\n');
		
		writeImpl(output);
	}

	void CLogger::write(const StringView s)
	{
		std::string output = Unicode::ToUTF8(s);
		output.push_back('\n');
		
		writeImpl(output);
	}

	void CLogger::write(const LogType type, const std::string_view s)
	{
		if (m_outputLevel < type)
		{
			return;
		}

		std::string output;
		output.append(std::to_string(Time::GetMillisec()));
		output.append(detail::LogTypeStrings[FromEnum(type)]);
		output.append(s);
		output.push_back('\n');

		writeImpl(output);
	}

	void CLogger::write(const LogType type, const StringView s)
	{
		if (m_outputLevel < type)
		{
			return;
		}
			
		std::string output;
		output.append(std::to_string(Time::GetMillisec()));
		output.append(detail::LogTypeStrings[FromEnum(type)]);
		output.append(Unicode::ToUTF8(s));
		output.push_back('\n');

		writeImpl(output);
	}

	void CLogger::setOutputLevel(const LogType logType) noexcept
	{
		m_outputLevel.store(logType);
	}

	LogType CLogger::getOutputLevel() const noexcept
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
