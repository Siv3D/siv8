//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <cstdio>
# include <ios>
# include <Siv3D/Common.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include "CConsole.hpp"

namespace s3d
{
	CConsole::CConsole()
		: m_defaultCodePage{ ::GetConsoleOutputCP() } {}

	CConsole::~CConsole()
	{
		close();
	}

	void CConsole::open()
	{
		if (m_isOpen)
		{
			return;
		}

		::AllocConsole();
		::freopen_s(&m_fp, "CONOUT$", "w", stdout);
		::freopen_s(&m_fp, "CONIN$", "r", stdin);
		::freopen_s(&m_fp, "CONOUT$", "w", stderr);

		std::ios_base::sync_with_stdio(false);

		m_isOpen = true;
	}

	void CConsole::close()
	{
		if (not m_isOpen)
		{
			return;
		}

		if (m_fp)
		{
			::fclose(m_fp);
			m_fp = nullptr;
		}

		::FreeConsole();
	}

	void CConsole::setSystemDefaultCodePage()
	{
		::SetConsoleOutputCP(m_defaultCodePage);
	}

	void CConsole::setUTF8CodePage()
	{
		::SetConsoleOutputCP(CP_UTF8);
	}
}
