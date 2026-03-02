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

# include <cwctype>
# include <Siv3D/Common.hpp>
# include <Siv3D/SerialPortInfo.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include <SetupAPI.h>
# include <devguid.h>

namespace s3d
{
	namespace
	{
		// レジストリからデバイスのプロパティを安全に取得するヘルパー
		[[nodiscard]]
		static std::wstring GetDeviceRegistryPropertyW(HDEVINFO deviceInfoSet, SP_DEVINFO_DATA* deviceInfoData, DWORD property)
		{
			DWORD dataType = 0;
			DWORD requiredSize = 0;

			// 最初に必要なバッファサイズ（バイト数）を取得
			::SetupDiGetDeviceRegistryPropertyW(deviceInfoSet, deviceInfoData, property, &dataType, nullptr, 0, &requiredSize);

			if (requiredSize == 0)
			{
				return L"";
			}

			std::wstring result;
			result.resize(requiredSize / sizeof(WCHAR));

			if (::SetupDiGetDeviceRegistryPropertyW(deviceInfoSet, deviceInfoData, property, &dataType, reinterpret_cast<PBYTE>(result.data()), requiredSize, nullptr))
			{
				// REG_MULTI_SZ（複数の文字列がヌル文字区切りで格納されている場合）への対応
				// 途中のヌル文字をスペースに置換し、文字列が途切れるのを防ぐ
				if (dataType == REG_MULTI_SZ)
				{
					for (auto& c : result)
					{
						if (c == L'\0') c = L' ';
					}
				}

				// 余分な末尾のヌル文字やスペースを削除
				while (!result.empty() && (result.back() == L'\0' || result.back() == L' '))
				{
					result.pop_back();
				}

				return result;
			}

			return L"";
		}

		// ハードウェアID文字列から VID または PID の 4桁16進数を抽出する
		// USBの場合は "VID_xxxx", Bluetooth仮想ポートの場合は "VID&xxxx" などになる
		[[nodiscard]]
		static Optional<uint16> ExtractHexID(const std::wstring& hwid, const std::wstring& prefix1, const std::wstring& prefix2)
		{
			// 大文字・小文字の揺らぎをなくすために大文字化
			std::wstring upperHwid = hwid;
			for (auto& c : upperHwid)
			{
				c = std::towupper(c);
			}

			size_t pos = upperHwid.find(prefix1);
			size_t prefixLen = prefix1.length();

			if (pos == std::wstring::npos)
			{
				pos = upperHwid.find(prefix2);
				prefixLen = prefix2.length();
			}

			if (pos != std::wstring::npos)
			{
				pos += prefixLen;
				if (pos + 4 <= upperHwid.length())
				{
					std::wstring hexStr = upperHwid.substr(pos, 4);
					try
					{
						// 16進数としてパース
						return static_cast<uint16>(std::stoul(hexStr, nullptr, 16));
					}
					catch (...)
					{
						return none;
					}
				}
			}

			return none;
		}
	}

	namespace System
	{
		////////////////////////////////////////////////////////////////
		//
		//	EnumerateSerialPorts
		//
		////////////////////////////////////////////////////////////////

		Array<SerialPortInfo> EnumerateSerialPorts()
		{
			Array<SerialPortInfo> ports;

			// ポートクラスのデバイス情報セットを取得
			HDEVINFO deviceInfoSet = ::SetupDiGetClassDevsW(&GUID_DEVCLASS_PORTS, nullptr, nullptr, DIGCF_PRESENT);
			if (deviceInfoSet == INVALID_HANDLE_VALUE)
			{
				return ports;
			}

			SP_DEVINFO_DATA deviceInfoData;
			deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

			DWORD deviceIndex = 0;

			// デバイスを列挙
			while (::SetupDiEnumDeviceInfo(deviceInfoSet, deviceIndex++, &deviceInfoData))
			{
				// デバイスのレジストリキーを開いてポート名("COM3" など)を取得
				HKEY hKey = ::SetupDiOpenDevRegKey(deviceInfoSet, &deviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
				if (hKey == INVALID_HANDLE_VALUE)
				{
					continue;
				}

				WCHAR portNameBuffer[256] = {};
				DWORD portNameSize = sizeof(portNameBuffer); // バイト数
				LSTATUS status = ::RegQueryValueExW(hKey, L"PortName", nullptr, nullptr, reinterpret_cast<LPBYTE>(portNameBuffer), &portNameSize);
				::RegCloseKey(hKey);

				if (status != ERROR_SUCCESS)
				{
					continue;
				}

				std::wstring portNameW{ portNameBuffer };

				// LPT (パラレルポート) を除外
				if (portNameW.find(L"LPT") != std::wstring::npos)
				{
					continue;
				}

				// デバイスプロパティの取得
				std::wstring descriptionW = GetDeviceRegistryPropertyW(deviceInfoSet, &deviceInfoData, SPDRP_FRIENDLYNAME);
				std::wstring hardwareIDW = GetDeviceRegistryPropertyW(deviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID);
				std::wstring manufacturerW = GetDeviceRegistryPropertyW(deviceInfoSet, &deviceInfoData, SPDRP_MFG); // 製造元

				SerialPortInfo info;

				// std::wstring から Siv3D の String への変換
				info.port = Unicode::FromWstring(portNameW);
				info.description = Unicode::FromWstring(descriptionW);
				info.hardwareID = Unicode::FromWstring(hardwareIDW);
				info.manufacturer = Unicode::FromWstring(manufacturerW);

				// ハードウェアIDから VID / PID を抽出 (例: "VID_04D8", "PID&000A")
				info.vendorID = ExtractHexID(hardwareIDW, L"VID_", L"VID&");
				info.productID = ExtractHexID(hardwareIDW, L"PID_", L"PID&");

				ports.push_back(info);
			}

			::SetupDiDestroyDeviceInfoList(deviceInfoSet);

			return ports;
		}
	}
}
