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

# include <Siv3D/UserInfo.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Windows/Windows.hpp>
# define SECURITY_WIN32
# include <Security.h>
# include <lmcons.h>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static String ComputerName()
		{
			wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 1];
			DWORD size = static_cast<DWORD>(std::ssize(buffer));

			if (::GetComputerNameExW(ComputerNamePhysicalDnsHostname, buffer, &size))
			{
				return Unicode::FromWstring(buffer);
			}
			else
			{
				return{};
			}
		}

		[[nodiscard]]
		static String UserName()
		{
			wchar_t buffer[UNLEN + 1];
			DWORD size = static_cast<DWORD>(std::ssize(buffer));

			if (GetUserNameW(buffer, &size))
			{
				return Unicode::FromWstring(buffer);
			}
			else
			{
				return{};
			}
		}

		[[nodiscard]]
		static String FullUserName()
		{
			wchar_t buffer[UNLEN + 1];
			ULONG size = static_cast<ULONG>(std::ssize(buffer));

			if (::GetUserNameExW(NameDisplay, buffer, &size))
			{
				return Unicode::FromWstring(buffer);
			}
			else
			{
				return{};
			}
		}

		[[nodiscard]]
		static String DefaultLocale()
		{
			wchar_t languageBuffer[LOCALE_NAME_MAX_LENGTH];

			if (const size_t length = ::GetUserDefaultLocaleName(languageBuffer, LOCALE_NAME_MAX_LENGTH))
			{
				return Unicode::FromWstring(std::wstring_view{ languageBuffer, (length - 1) });
			}
			else
			{
				return U"en-US";
			}
		}

		[[nodiscard]]
		static String DefaultLanguage()
		{
			ULONG numLanguages = 0;
			ULONG languageBufferSize = 0;

			if (::GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, nullptr, &languageBufferSize))
			{
				std::wstring languages(languageBufferSize, '\0');

				if (::GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, languages.data(), &languageBufferSize))
				{
					// languages はヌル区切りされた文字列。
					// ヌル区切りされたうち、最初の言語が使用言語
					return Unicode::FromWstring(languages.data());
				}
			}

			return DefaultLocale();
		}

		[[nodiscard]]
		static bool IsRunningInVisualStudio_impl()
		{
			wchar_t* pValue;
			size_t len;
			errno_t err = ::_wdupenv_s(&pValue, &len, L"SIV3D_APP_LAUNCHED_FROM_IDE");

			if (err || (not pValue))
			{
				return false;
			}

			std::free(pValue);

			return true;
		}
	}

	namespace System
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetUserInfo
		//
		////////////////////////////////////////////////////////////////

		const UserInfo& GetUserInfo()
		{
			static const UserInfo userInfo = []()
			{
				UserInfo info;
				info.computerName		= ComputerName();
				info.userName			= UserName();
				info.fullUserName		= FullUserName();
				info.defaultLocale		= DefaultLocale();
				info.defaultLanguage	= DefaultLanguage();
				return info;
			}();

			return userInfo;
		}

		////////////////////////////////////////////////////////////////
		//
		//	IsRunningInVisualStudio
		//
		////////////////////////////////////////////////////////////////

		bool IsRunningInVisualStudio()
		{
			static const bool result = IsRunningInVisualStudio_impl();

			return result;
		}

		////////////////////////////////////////////////////////////////
		//
		//	IsRunningInXcode
		//
		////////////////////////////////////////////////////////////////

		bool IsRunningInXcode()
		{
			return false;
		}
	}
}
