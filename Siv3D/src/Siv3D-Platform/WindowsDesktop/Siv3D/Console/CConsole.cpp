//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <cstdio>
# include <iostream>
# include <Siv3D/Common.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include "CConsole.hpp"
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Error.hpp>

namespace s3d
{
	namespace
	{
		/// @brief 標準出力がファイルにリダイレクトされているかを返します。
		/// @return 標準出力がファイルにリダイレクトされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool IsStdoutRedirected()
		{
			const DWORD fileType = ::GetFileType(::GetStdHandle(STD_OUTPUT_HANDLE));
			
			return (fileType == FILE_TYPE_DISK); // FILE_TYPE_DISK = ファイルにリダイレクトされている
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CConsole::CConsole() = default;

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CConsole::~CConsole()
	{
		LOG_SCOPED_DEBUG("CConsole::~CConsole()");

		close();
	}

	////////////////////////////////////////////////////////////////
	//
	//	open
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	attach
	//
	////////////////////////////////////////////////////////////////

	bool CConsole::attach()
	{
		if (m_isOpen)
		{
			return false;
		}

		// 標準入力がファイルにリダイレクトされている場合は新しいコンソールを作成しない
		if (IsStdoutRedirected())
		{
			::setlocale(LC_ALL, ".utf8");

			std::ios_base::sync_with_stdio(false);
			m_isOpen = true;

			return true;
		}

		// 呼び出し元のコンソールがある場合は新しいコンソールを作成しない
		if (::AttachConsole(ATTACH_PARENT_PROCESS))
		{
			m_oldCodePage = ::GetConsoleOutputCP();
			::SetConsoleOutputCP(CP_UTF8);

			::setlocale(LC_ALL, ".utf8");
			::freopen_s(&m_fp, "CONIN$", "r", stdin);
			::freopen_s(&m_fp, "CONOUT$", "w", stdout);
			::freopen_s(&m_fp, "CONOUT$", "w", stderr);

			std::ios_base::sync_with_stdio(false);
			m_isOpen = true;

			return true;
		}
		else
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	clearSelection
	//
	////////////////////////////////////////////////////////////////

	void CConsole::clearSelection()
	{
		if (not m_isOpen)
		{
			return;
		}

		if (HWND hConsoleWindow = ::GetConsoleWindow())
		{
			// ESC キーを送信
			::SendMessage(hConsoleWindow, WM_KEYDOWN, VK_ESCAPE, 0);
			::SendMessage(hConsoleWindow, WM_KEYUP, VK_ESCAPE, 0);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	close
	//
	////////////////////////////////////////////////////////////////

	void CConsole::close()
	{
		if (not m_isOpen)
		{
			return;
		}

		std::cout.flush();
		std::cerr.flush();
		std::clog.flush();
		std::cin.clear();

		if (m_fp)
		{
			::fflush(m_fp);
			::fclose(m_fp);
			m_fp = nullptr;
		}

		if (m_oldCodePage)
		{
			::SetConsoleOutputCP(m_oldCodePage);
		}

		::FreeConsole();
	}
}
