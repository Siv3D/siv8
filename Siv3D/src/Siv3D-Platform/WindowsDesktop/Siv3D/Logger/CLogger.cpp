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

# include <Siv3D/Windows/MinWindows.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Time.hpp>
# include "CLogger.hpp"

namespace s3d
{
	namespace
	{
		static constexpr std::array LogTypeStrings =
		{
			L": [error] ",
			L": [fail] ",
			L": [warning] ",
			L": [log] ",
			L": [info] ",
			L": [debug] ",
			L": [trace] ",
		};
	}

	CLogger::CLogger()
		: m_outputLevel{ SIV3D_BUILD(DEBUG) ? LogType::Trace : LogType::Info } {}

	void CLogger::write(const std::string_view s)
	{
		writeImpl((Unicode::ToWstring(s) + L'\n').c_str());
	}

	void CLogger::write(const StringView s)
	{
		writeImpl((s.toWstr() + L'\n').c_str());
	}

	void CLogger::write(const LogType type, const std::string_view s)
	{
		if (m_outputLevel < type)
		{
			return;
		}

		std::wstring output = std::to_wstring(Time::GetMillisec());
		output.append(LogTypeStrings[FromEnum(type)]);
		output.append(Unicode::ToWstring(s));
		output.push_back(L'\n');

		writeImpl(output.c_str());
	}

	void CLogger::write(const LogType type, const StringView s)
	{
		if (m_outputLevel < type)
		{
			return;
		}

		std::wstring output = std::to_wstring(Time::GetMillisec());
		output.append(LogTypeStrings[FromEnum(type)]);
		output.append(Unicode::ToWstring(s));
		output.push_back(L'\n');

		writeImpl(output.c_str());
	}

	void CLogger::setOutputLevel(const LogType logType) noexcept
	{
		m_outputLevel.store(logType);
	}

	LogType CLogger::getOutputLevel() const noexcept
	{
		return m_outputLevel.load();
	}

	void CLogger::writeImpl(const wchar_t* s)
	{
		std::lock_guard lock{ m_mutex };

		::OutputDebugStringW(s);
	}
}
