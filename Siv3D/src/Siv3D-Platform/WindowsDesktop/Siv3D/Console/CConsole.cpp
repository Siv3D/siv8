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

# include <cstdio>
# include <ios>
# include <Siv3D/Common.hpp>
# include <Siv3D/Windows/MinWindows.hpp>
# include "CConsole.hpp"

namespace s3d
{
	CConsole::CConsole() = default;

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
		
		m_oldCodePage = ::GetConsoleOutputCP();
		::SetConsoleOutputCP(CP_UTF8);
		
		::setlocale(LC_ALL, ".utf8");
		::freopen_s(&m_fp, "CONIN$",  "r", stdin);
		::freopen_s(&m_fp, "CONOUT$", "w", stdout);
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

		::SetConsoleOutputCP(m_oldCodePage);
		::FreeConsole();
	}
}
