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
		/// @remark Windows 10 1703 およびそれ以前のバージョンでは、`FileAction::Removed` の際、ディレクトリ名の末尾に '/' が付加されません。
		/// @remark On Windows 10 1703 and earlier, '/' is not appended to the end of the directory name when `FileAction::Removed`.
		FilePath path;

		/// @brief ファイルの操作 | File action
		FileAction action;
	};
}
