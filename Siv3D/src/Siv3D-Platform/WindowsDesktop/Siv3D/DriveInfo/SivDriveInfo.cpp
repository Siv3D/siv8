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

# include <Siv3D/DriveInfo.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Windows/Windows.hpp>

namespace s3d
{
	namespace
	{
		/// @brief Windows の DriveType を Siv3D の DriveType に変換します。
		[[nodiscard]]
		constexpr DriveType ToDriveType(const UINT type) noexcept
		{
			switch (type)
			{
			case DRIVE_REMOVABLE: return DriveType::Removable;
			case DRIVE_FIXED:     return DriveType::Fixed;
			case DRIVE_REMOTE:    return DriveType::Network;
			case DRIVE_CDROM:     return DriveType::CDROM;
			case DRIVE_RAMDISK:   return DriveType::RAMDisk;
			default:              return DriveType::Unknown;
			}
		}

		/// @brief 仮想ドライブ（VHD 等）であるかを判定します。
		[[nodiscard]]
		static bool IsVirtualDrive(const std::wstring& rootPathName)
		{
			const std::wstring devicePath = (L"\\\\.\\" + rootPathName.substr(0, 2));

			HANDLE hDevice = ::CreateFileW(devicePath.c_str(),
				0, (FILE_SHARE_READ | FILE_SHARE_WRITE), nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (hDevice == INVALID_HANDLE_VALUE)
			{
				return false;
			}

			STORAGE_PROPERTY_QUERY query{};
			query.PropertyId = StorageDeviceProperty;
			query.QueryType = PropertyStandardQuery;

			BYTE buffer[1024]{};
			DWORD bytesRead = 0;

			if (::DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
				&query, sizeof(query),
				buffer, sizeof(buffer),
				&bytesRead, nullptr))
			{
				::CloseHandle(hDevice);
				const auto pDesc = reinterpret_cast<const STORAGE_DEVICE_DESCRIPTOR*>(buffer);
				return (pDesc->BusType == BusTypeVirtual) || (pDesc->BusType == BusTypeFileBackedVirtual);
			}

			::CloseHandle(hDevice);
			return false;
		}

		/// @brief SSDであるかを判定します。判定できない場合は none を返します。
		[[nodiscard]]
		static Optional<bool> IsSSD(const std::wstring& rootPathName)
		{
			const std::wstring devicePath = (L"\\\\.\\" + rootPathName.substr(0, 2));

			HANDLE hDevice = ::CreateFileW(devicePath.c_str(),
				0, (FILE_SHARE_READ | FILE_SHARE_WRITE), nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (hDevice == INVALID_HANDLE_VALUE)
			{
				return none;
			}

			STORAGE_PROPERTY_QUERY query{};
			query.PropertyId = StorageDeviceSeekPenaltyProperty;
			query.QueryType = PropertyStandardQuery;

			DEVICE_SEEK_PENALTY_DESCRIPTOR desc{};
			DWORD bytesRead = 0;

			if (::DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
				&query, sizeof(query),
				&desc, sizeof(desc),
				&bytesRead, nullptr))
			{
				::CloseHandle(hDevice);
				return (not desc.IncursSeekPenalty);
			}

			::CloseHandle(hDevice);
			return none;
		}
	}

	namespace System
	{
		Array<DriveInfo> EnumerateDrives()
		{
			Array<DriveInfo> driveInfos;

			// メディアが挿入されていないカードリーダーや光学ドライブをクエリした際、
			// 「ディスクを挿入してください」という Windows エラーダイアログが出現するのを防ぐ。
			const UINT oldErrorMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);

			const DWORD driveMask = ::GetLogicalDrives();

			// システムドライブ（Windows がインストールされているドライブ）のドライブレターを取得
			wchar_t systemDriveLetter = L'\0';
			{
				wchar_t winDir[MAX_PATH] = {};
				if (::GetWindowsDirectoryW(winDir, static_cast<UINT>(std::size(winDir))))
				{
					systemDriveLetter = winDir[0];
					// 小文字で返ってきた場合を考慮して大文字に揃える
					if (systemDriveLetter >= L'a' && systemDriveLetter <= L'z')
					{
						systemDriveLetter = (systemDriveLetter - L'a' + L'A');
					}
				}
			}

			for (int32 i = 0; i < 26; ++i)
			{
				if (driveMask & (1 << i))
				{
					const wchar_t currentDriveLetter = wchar_t(L'A' + i);
					const wchar_t rootPathName[4] = { currentDriveLetter, L':', L'\\', L'\0' };
					wchar_t volumeName[MAX_PATH + 1] = {};
					wchar_t fileSystemName[MAX_PATH + 1] = {};

					DWORD serialNumber = 0;
					DWORD maxComponentLength = 0;
					DWORD fileSystemFlags = 0;

					// メディアが存在しない場合等は失敗しスキップされる
					if (::GetVolumeInformationW(rootPathName, volumeName, static_cast<DWORD>(std::size(volumeName)),
						&serialNumber, &maxComponentLength, &fileSystemFlags, fileSystemName, static_cast<DWORD>(std::size(fileSystemName))))
					{
						DriveType driveType = ToDriveType(::GetDriveTypeW(rootPathName));

						// 固定ディスクとして認識されている場合でも、バスタイプから仮想ドライブか判定して上書きする
						if ((driveType == DriveType::Fixed) && IsVirtualDrive(rootPathName))
						{
							driveType = DriveType::Virtual;
						}

						DriveInfo driveInfo
						{
							.rootPath = { char32(U'A' + i), U':', U'/' },
							.devicePath = { U'\\', U'\\', U'.', U'\\', char32(U'A' + i), U':' },
							.volumeName = Unicode::FromWstring(volumeName),
							// シリアルナンバーを一般的に見慣れた XAAA-BBBB の形式（16進数）に変換
							.volumeID = U"{:04X}-{:04X}"_fmt((serialNumber >> 16) & 0xFFFF, serialNumber & 0xFFFF),
							.fileSystem = Unicode::FromWstring(fileSystemName),
							.driveType = driveType,
							.availableSpaceBytes = 0,
							.freeSpaceBytes = 0,
							.totalSizeBytes = 0,
							// fileSystemFlags から読み取り専用フラグを抽出
							.isReadOnly = ((fileSystemFlags & FILE_READ_ONLY_VOLUME) != 0),
							// fileSystemFlags から大文字と小文字を区別するフラグを抽出
							.isCaseSensitive = ((fileSystemFlags & FILE_CASE_SENSITIVE_SEARCH) != 0),
							// システムドライブ判定
							.isSystem = (currentDriveLetter == systemDriveLetter),
							.isSSD = IsSSD(rootPathName)
						};

						ULARGE_INTEGER freeBytesAvailableToCaller;
						ULARGE_INTEGER totalNumberOfBytes;
						ULARGE_INTEGER totalNumberOfFreeBytes;

						if (::GetDiskFreeSpaceExW(rootPathName, &freeBytesAvailableToCaller, &totalNumberOfBytes, &totalNumberOfFreeBytes))
						{
							driveInfo.availableSpaceBytes = freeBytesAvailableToCaller.QuadPart;
							driveInfo.totalSizeBytes = totalNumberOfBytes.QuadPart;
							driveInfo.freeSpaceBytes = totalNumberOfFreeBytes.QuadPart;
						}

						driveInfos.push_back(std::move(driveInfo));
					}
				}
			}

			::SetErrorMode(oldErrorMode); // エラーモードを復元

			return driveInfos;
		}
	}
}
