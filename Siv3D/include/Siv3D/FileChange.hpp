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

# pragma once
# include "Common.hpp"
# include "String.hpp"
# include "FileAction.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	FileChange
	//
	////////////////////////////////////////////////////////////////

	/// @brief ファイルパスとファイルの操作 | File path and file action
	struct FileChange
	{
		/// @brief ファイルパス | File path
		FilePath path;

		/// @brief ファイルの操作 | File action
		FileAction action;
	};
}
