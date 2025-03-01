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

# include <array>
# include <Siv3D/WebBrowser.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Windows/MinWindows.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static String FindBrowserInRegistry(const char* registryKey)
		{
			HKEY hKey;
			
			if (::RegOpenKeyExA(HKEY_LOCAL_MACHINE, registryKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
			{
				char path[1024]{};

				DWORD pathSize = sizeof(path);
				
				if (::RegQueryValueExA(hKey, nullptr, nullptr, nullptr, (LPBYTE)path, &pathSize) == ERROR_SUCCESS)
				{
					::RegCloseKey(hKey);
					
					return Unicode::FromUTF8(path);
				}

				::RegCloseKey(hKey);
			}

			return{};
		}

		constexpr std::array<const char*, 7> BrowserRegistryKeys =
		{
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe",
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\firefox.exe",
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\msedge.exe",
			"",
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\opera.exe",
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\brave.exe",
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\vivaldi.exe",
		};
	}

	namespace System
	{
		FilePath GetWebBrowserPath(const WebBrowser webBrowser)
		{
			// Windows では Safari はサポートされない
			if (webBrowser == WebBrowser::Safari)
			{
				return{};
			}

			return FindBrowserInRegistry(BrowserRegistryKeys[FromEnum(webBrowser)]);
		}
	}
}
