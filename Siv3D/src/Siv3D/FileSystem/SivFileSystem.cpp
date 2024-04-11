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

# include <filesystem>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/SpecialFolder.hpp>
# include <Siv3D/UUIDValue.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		inline static std::filesystem::path ToPath(const FilePathView path)
		{
			return std::filesystem::path{ Unicode::ToWstring(path) };
		}

		[[nodiscard]]
		inline static constexpr std::filesystem::copy_options ToCopyOptions(const CopyOption copyOption) noexcept
		{
			switch (copyOption)
			{
			case CopyOption::SkipExisting:
				return std::filesystem::copy_options::skip_existing;
			case CopyOption::OverwriteExisting:
				return std::filesystem::copy_options::overwrite_existing;
			case CopyOption::UpdateExisting:
				return std::filesystem::copy_options::update_existing;
			default:
				return std::filesystem::copy_options::none;
			}
		}
	}

	namespace FileSystem
	{		
		////////////////////////////////////////////////////////////////
		//
		//	Extension
		//
		////////////////////////////////////////////////////////////////

		String Extension(FilePathView path)
		{
			if (path.isEmpty())
			{
				return{};
			}

			while (path.starts_with(U'.'))
			{
				path.remove_prefix(1);
			}

			const size_t lastDotPos = path.rfind(U'.');

			if (lastDotPos == String::npos)
			{
				return{};
			}

			const size_t lastSeparatorPos = path.find_last_of(U"/\\");

			// aaa.bbb/ccc のようなケースを弾く
			if ((lastSeparatorPos != String::npos)
				&&  (lastDotPos < lastSeparatorPos))
			{
				return{};
			}

			return path.substr(lastDotPos + 1).toString().lowercase();
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	FileName
		//
		////////////////////////////////////////////////////////////////

		String FileName(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return{};
			}

			if (path.ends_with(U'/') || path.ends_with(U'\\'))
			{
				const size_t parentSeparatorPos = path.find_last_of(U"/\\", (path.size() - 2));

				if (parentSeparatorPos == String::npos)
				{
					return String(path.begin(), (path.end() - 1));
				}
				else
				{
					return String((path.begin() + parentSeparatorPos + 1), (path.end() - 1));
				}
			}
			else
			{
				const size_t lastSeparatorPos = path.find_last_of(U"/\\");

				if (lastSeparatorPos == String::npos)
				{
					return path.toString();
				}
				else
				{
					return String((path.begin() + lastSeparatorPos + 1), path.end());
				}
			}
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	BaseName
		//
		////////////////////////////////////////////////////////////////

		String BaseName(const FilePathView path)
		{
			const String fileName = FileName(path);

			if (fileName.isEmpty())
			{
				return{};
			}

			const size_t lastDotPos = fileName.rfind(U'.');

			if (lastDotPos == String::npos)
			{
				return fileName;
			}

			return String(fileName.begin(), (fileName.begin() + lastDotPos));
		}

		FilePath ParentPath(const FilePathView path, const size_t level)
		{
			FilePath unused;
			return ParentPath(path, level, unused);
		}

		FilePath ParentPath(const FilePathView path, size_t level, FilePath& baseFullPath)
		{
			if (path.isEmpty())
			{
				return{};
			}

			FilePath result = FullPath(path);

			baseFullPath = result;

			if (result.ends_with(U'/'))
			{
				result.pop_back();
			}

			while (not result.isEmpty())
			{
				do
				{
					result.pop_back();
				} while ((not result.isEmpty()) && (not result.ends_with(U'/')));

				if (level-- == 0)
				{
					break;
				}
			}

			return result;
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	PathAppend
		//
		////////////////////////////////////////////////////////////////

		FilePath PathAppend(const FilePathView lhs, const FilePathView rhs)
		{
			return FilePath{ (ToPath(lhs) / ToPath(rhs)).u32string() }.replace(U'\\', U'/');
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	IsEmptyDirectory
		//
		////////////////////////////////////////////////////////////////

		bool IsEmptyDirectory(const FilePathView path)
		{
			if (not IsDirectory(path))
			{
				return false;
			}

			return (std::filesystem::directory_iterator{ ToPath(path) } == std::filesystem::directory_iterator{});
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	TemporaryDirectoryPath
		//
		////////////////////////////////////////////////////////////////

		FilePath TemporaryDirectoryPath()
		{
			return (GetFolderPath(SpecialFolder::LocalAppData) + U"Temp/");
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	UniqueFilePath
		//
		////////////////////////////////////////////////////////////////

		FilePath UniqueFilePath(const FilePathView directory)
		{
			if (FileSystem::IsFile(directory))
			{
				return{};
			}

			FilePath directoryPath{ directory };

			if (directoryPath && (not directoryPath.ends_with(U'/')))
			{
				directoryPath.push_back(U'/');
			}

			for (;;)
			{
				const FilePath path = (directoryPath + UUIDValue::Generate().str() + U".tmp");

				if (not Exists(path))
				{
					return path;
				}
			}
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	RelativePath
		//
		////////////////////////////////////////////////////////////////

		FilePath RelativePath(const FilePathView _path, const FilePathView _start)
		{
			if (_path.isEmpty() || _start.isEmpty())
			{
				return FilePath();
			}

			const FilePath path = FullPath(_path);
			const FilePath start = FullPath(_start);

			if (not IsDirectory(start))
			{
				return path;
			}

			if (path == start)
			{
				return U"./";
			}

			const std::filesystem::path p(path.toUTF8());
			const std::filesystem::path	base(start.toUTF8());

			FilePath result = Unicode::FromUTF8(std::filesystem::proximate(p, base).string());

			result.replace(U'\\', U'/');

			if (IsDirectory(result) && (not result.ends_with(U'/')))
			{
				result.push_back(U'/');
			}

			return result;
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	CreateDirectories
		//
		////////////////////////////////////////////////////////////////

		bool CreateDirectories(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return true;
			}

			if (IsResourcePath(path))
			{
				return false;
			}

			try
			{
				std::filesystem::create_directories(ToPath(path));
				return true;
			}
			catch (const std::filesystem::filesystem_error&)
			{
				return false;
			}
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	CreateParentDirectories
		//
		////////////////////////////////////////////////////////////////

		bool CreateParentDirectories(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return false;
			}

			if (IsResourcePath(path))
			{
				return false;
			}

			const FilePath parentDirectory = ParentPath(path);

			if (not Exists(parentDirectory))
			{
				return CreateDirectories(parentDirectory);
			}

			return true;
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	Copy
		//
		////////////////////////////////////////////////////////////////

		bool Copy(const FilePathView from, const FilePathView to, const CopyOption copyOption)
		{
			if ((from.isEmpty()) || (to.isEmpty()))
			{
				return false;
			}

			if (IsResourcePath(from) || IsResourcePath(to))
			{
				return false;
			}

			CreateParentDirectories(to);

			const auto options = (ToCopyOptions(copyOption) | std::filesystem::copy_options::recursive);
			
			std::error_code error;
			std::filesystem::copy(ToPath(from), ToPath(to), options, error);

			return (error.value() == 0);
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	Rename
		//
		////////////////////////////////////////////////////////////////

		bool Rename(const FilePathView from, const FilePathView to)
		{
			if ((from.isEmpty()) || (to.isEmpty()))
			{
				return false;
			}

			if (IsResourcePath(from) || IsResourcePath(to))
			{
				return false;
			}

			std::error_code error;
			std::filesystem::rename(ToPath(from), ToPath(to), error);

			return (error.value() == 0);
		}
	}
}
