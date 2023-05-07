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

# include <Siv3D/Windows/MinWindows.hpp>
# include <array>
# include <Siv3D/Utility.hpp>
# include <Siv3D/FormatUtility.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Time.hpp>
# include "CLogger.hpp"

namespace s3d
{
	namespace detail
	{
		static const std::array<std::wstring, 6> LogTypeNames =
		{
			L": [error] ",
			L": [fail] ",
			L": [warning] ",
			L": [log] ",
			L": [info] ",
			L": [trace] ",
		};
	}

	CLogger::CLogger()
		: m_outputLevel{ SIV3D_BUILD(DEBUG) ? LogType::Trace : LogType::User } {}

	CLogger::~CLogger() = default;

	void CLogger::write(const std::string_view s)
	{
		write((Unicode::ToWstring(s) + L'\n').c_str());
	}

	void CLogger::write(const StringView s)
	{
		write((s.toWstr() + L'\n').c_str());
	}

	void CLogger::write(const LogType type, const std::string_view s)
	{
		if (m_outputLevel < type)
		{
			return;
		}

		const std::wstring output = std::to_wstring(Time::GetMillisec())
			+ (detail::LogTypeNames[FromEnum(type)]
			+ Unicode::ToWstring(s) + L'\n');

		write(output.c_str());
	}

	void CLogger::write(const LogType type, const StringView s)
	{
		if (m_outputLevel < type)
		{
			return;
		}

		const std::wstring output = std::to_wstring(Time::GetMillisec())
			+ (detail::LogTypeNames[FromEnum(type)]
			+ Unicode::ToWstring(s) + L'\n');

		write(output.c_str());
	}

	void CLogger::setOutputLevel(const LogType outputLevel)
	{
		m_outputLevel.store(outputLevel);
	}

	LogType CLogger::getOutputLevel() const noexcept
	{
		return m_outputLevel.load();
	}

	void CLogger::write(const wchar_t* s)
	{
		std::lock_guard lock{ m_mutex };

		::OutputDebugStringW(s);
	}
}
