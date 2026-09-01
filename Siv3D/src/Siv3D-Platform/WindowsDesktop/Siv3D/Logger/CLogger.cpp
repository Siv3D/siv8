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

# include <array>
# include <Siv3D/Windows/MinWindows.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Time.hpp>
# include "CLogger.hpp"

namespace s3d
{
	namespace
	{
		static constexpr std::array LogLevelStrings =
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

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CLogger::CLogger()
		: m_outputLevel{ SIV3D_BUILD(DEBUG) ? LogLevel::Trace : LogLevel::Info } {}

	////////////////////////////////////////////////////////////////
	//
	//	writeln
	//
	////////////////////////////////////////////////////////////////

	void CLogger::writeln(const std::string_view s)
	{
		writeImpl((Unicode::ToWstring(s) + L'\n').c_str());
	}

	void CLogger::writeln(const StringView s)
	{
		writeImpl((s.toWstr() + L'\n').c_str());
	}

	void CLogger::writeln(const LogLevel logLevel, const std::string_view s)
	{
		if (m_outputLevel < logLevel)
		{
			return;
		}

		std::wstring output = std::to_wstring(Time::GetMillisec());
		output.append(LogLevelStrings[FromEnum(logLevel)]);
		output.append(Unicode::ToWstring(s));
		output.push_back(L'\n');

		writeImpl(output.c_str());
	}

	void CLogger::writeln(const LogLevel logLevel, const StringView s)
	{
		if (m_outputLevel < logLevel)
		{
			return;
		}

		std::wstring output = std::to_wstring(Time::GetMillisec());
		output.append(LogLevelStrings[FromEnum(logLevel)]);
		output.append(Unicode::ToWstring(s));
		output.push_back(L'\n');

		writeImpl(output.c_str());
	}

	////////////////////////////////////////////////////////////////
	//
	//	setOutputLevel
	//
	////////////////////////////////////////////////////////////////

	void CLogger::setOutputLevel(const LogLevel logLevel) noexcept
	{
		m_outputLevel.store(logLevel);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getOutputLevel
	//
	////////////////////////////////////////////////////////////////

	LogLevel CLogger::getOutputLevel() const noexcept
	{
		return m_outputLevel.load();
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void CLogger::writeImpl(const wchar_t* s)
	{
		std::lock_guard lock{ m_mutex };

		::OutputDebugStringW(s);
	}
}
