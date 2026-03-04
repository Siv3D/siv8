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

# pragma once
# include "DriveType.hpp"
# include "Optional.hpp"
# include "String.hpp"
# include "Array.hpp"

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	DriveInfo
	//
	////////////////////////////////////////////////////////////////

	/// @brief ドライブの情報 | Represents information about a drive.
	struct DriveInfo
	{
		/// @brief ルートパス（マウントポイント） | The root path or mount point of the drive
		String rootPath;

		/// @brief デバイスパス（Windows では空の場合あり） | The device path (e.g., /dev/sda1). May be empty on Windows.
		String devicePath;

		/// @brief ボリューム名 | The name of the drive's volume
		String volumeName;

		/// @brief ボリュームの一意識別子（シリアルナンバーや UUID など） | Unique identifier for the volume (e.g., UUID or serial number)
		String volumeID;

		/// @brief ファイルシステム名（例: NTFS, APFS, ext4） | The name of the drive's file system
		String fileSystem;

		/// @brief ドライブの種類 | The type of the drive
		DriveType driveType = DriveType::Unknown;

		/// @brief 利用可能な空き容量（バイト） | The amount of free space available to the user, in bytes
		uint64 availableSpaceBytes = 0;

		/// @brief ファイルシステムの総空き容量（バイト） | The total amount of free space on the drive, in bytes
		uint64 freeSpaceBytes = 0;

		/// @brief 総容量（バイト） | The total size of the drive, in bytes
		uint64 totalSizeBytes = 0;

		/// @brief 読み取り専用であるか | Whether the drive is read-only
		bool isReadOnly = false;

		/// @brief 大文字と小文字を区別するか | Whether the file system is case-sensitive
		bool isCaseSensitive = false;

		/// @brief システムドライブであるか | Whether the drive is a system drive
		bool isSystem = false;

		/// @brief SSD（ソリッドステート）であるか。判定できない場合は none | Whether the drive is an SSD. Returns none if unknown.
		Optional<bool> isSSD = none;
	};

	namespace System
	{		
		////////////////////////////////////////////////////////////////
		//
		//	EnumerateDrives
		//
		////////////////////////////////////////////////////////////////

		/// @brief ドライブの情報を列挙します。 | Enumerates the drive information.
		/// @return ドライブの情報の配列 | Array of drive information
		[[nodiscard]]
		Array<DriveInfo> EnumerateDrives();
	}
}
