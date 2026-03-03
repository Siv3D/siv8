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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	DriveType
	//
	////////////////////////////////////////////////////////////////

	/// @brief ドライブの種類 | Represents the type of a drive.
	enum class DriveType : uint8
	{
		/// @brief 不明 | Unknown
		Unknown,

		/// @brief リムーバブルメディア | Removable media
		Removable,

		/// @brief 固定ディスク | Fixed disk
		Fixed,

		/// @brief ネットワークドライブ | Network drive
		Network,

		/// @brief CD-ROM ドライブ | CD-ROM drive
		CDROM,

		/// @brief RAM ディスク | RAM disk
		RAMDisk,

		/// @brief 仮想ドライブ | Virtual drive
		Virtual,
	};

	/// @brief ドライブの種類を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value ドライブの種類
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, DriveType value);
}
