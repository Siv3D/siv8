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

# include <Siv3D/String.hpp>
# include <Siv3D/Array.hpp>

namespace s3d
{
	namespace detail
	{
		struct FilePathCache
		{
			/// @brief 起動ディレクトリ
			FilePath initialDirectory;
			
			/// @brief 実行ファイルのパス
			FilePath modulePath;
			
			/// @brief 特殊フォルダのパス
			std::array<FilePath, 12> specialFolderPaths;
			
			/// @brief リソースファイルのパス一覧
			Array<FilePath> resourceFilePaths;
			
			FilePathCache();
			
			static FilePath ParentPath(FilePath path, isize n);
		};
	}
}
