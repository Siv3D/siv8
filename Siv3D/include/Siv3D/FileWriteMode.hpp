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
	////////////////////////////////////////////////////////////////
	//
	//	FileWriteMode
	//
	////////////////////////////////////////////////////////////////

	/// @brief ファイルの書き込みモード | File write mode
	enum class FileWriteMode : uint8
	{
		/// @brief ファイルを新規作成するか、既存の内容を消去します。 | Create a new file or truncate an existing file
		Trunc,

		/// @brief 既存の内容の末尾に追記します。 | Append to the end of an existing file
		Append,
	};
}
