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
# include "Common.hpp"
# include "String.hpp"
# include "FileChangeAction.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	FileChange
	//
	////////////////////////////////////////////////////////////////

	/// @brief ファイルパスとファイルの変更内容 | File path and file change action
	struct FileChange
	{
		/// @brief ファイルパス | File path
		/// @remark Windows 10 1703 およびそれ以前のバージョンでは、`FileChangeAction::Removed` の際、ディレクトリ名の末尾に '/' が付加されません。
		/// @remark On Windows 10 1703 and earlier, '/' is not appended to the end of the directory name when `FileChangeAction::Removed`.
		FilePath path;

		/// @brief ファイルの変更内容 | File change action
		FileChangeAction action;
	};
}
