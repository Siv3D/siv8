//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <filesystem>
# include <Siv3D/FileSystem.hpp>

namespace s3d
{
	namespace FileSystem
	{
		FilePath ParentPath(const FilePathView path, size_t level, FilePath& baseFullPath)
		{
			if (not path)
			{
				return{};
			}

			if constexpr (Platform::HasEmbeddedResource)
			{
				if (IsResourcePath(path))
				{
					baseFullPath.clear();
					return{};
				}
			}

			baseFullPath = FullPath(path);

			FilePath result = baseFullPath;

			if (result.ends_with(U'/'))
			{
				result.pop_back();
			}

			while (result)
			{
				do
				{
					result.pop_back();
				}
				while (result && (not result.ends_with(U'/')));

				if (level-- == 0)
				{
					break;
				}
			}

			return result;
		}

		bool CreateDirectories(const FilePathView path)
		{
			if (not path)
			{
				return true;
			}

			if (IsResourcePath(path))
			{
				return false;
			}

			try
			{
				std::filesystem::create_directories(std::filesystem::path{ path.toWstr() });
			}
			catch (const std::filesystem::filesystem_error&)
			{
				return false;
			}

			return true;
		}
	}
}
