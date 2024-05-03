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
# include <filesystem>
# include <Siv3D/Common.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/Optional.hpp>
# include <Siv3D/DateTime.hpp>
# include <Siv3D/PredefinedYesNo.hpp>
# include <Siv3D/Windows/Windows.hpp>

namespace s3d
{
	namespace detail
	{
		/// @brief 引数のファイルパスの種類
		/// @remark ファイルやディレクトリであることが分かっている場合、一部の処理を省略できます。
		enum class PathType : uint8
		{
			/// @brief ファイルであるかディレクトリであるか不明
			Unknown,
			
			/// @brief ファイル
			File,
			
			/// @brief ディレクトリ
			Directory,
		};

		struct FilePathCache
		{
			/// @brief 特殊フォルダの数
			static constexpr size_t SpecialFolderCount = 12;

			/// @brief 起動ディレクトリ
			FilePath initialDirectory;

			/// @brief 実行ファイルのパス
			FilePath modulePath;

			/// @brief 特殊フォルダのパス
			std::array<FilePath, SpecialFolderCount> specialFolderPaths;

			FilePathCache();
		};

		[[nodiscard]]
		std::filesystem::file_status GetStatus(std::wstring_view path);

		[[nodiscard]]
		std::filesystem::file_status GetStatus(FilePathView path);

		[[nodiscard]]
		bool ResourceExists(FilePathView path);

		[[nodiscard]]
		size_t ResourceSize(FilePathView path);

		[[nodiscard]]
		std::wstring GetNativeFullPath(FilePathView path);

		[[nodiscard]]
		std::wstring NormalizePath(std::wstring path, PathType pathType);
		
		[[nodiscard]]
		int64 DirectorySizeRecursive(const std::wstring& directoryPath);

		[[nodiscard]]
		Optional<WIN32_FILE_ATTRIBUTE_DATA> GetFileAttributeData(const std::wstring& path);

		[[nodiscard]]
		DateTime FileTimeToTime(FILETIME in);

		void DirectoryContentsDetail(const std::wstring& directoryPath, Array<FilePath>& paths, Recursive recursive);

		/// @brief ファイルがディレクトリであるかを返します。
		/// @param data ファイル情報
		/// @return ファイルがディレクトリである場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool IsDirectory(const DWORD dwFileAttributes)
		{
			return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}

		[[nodiscard]]
		constexpr uint64 GetFileSize(const DWORD high, const DWORD low)
		{
			return ((static_cast<uint64>(high) << 32) | low);
		}
	}
}
