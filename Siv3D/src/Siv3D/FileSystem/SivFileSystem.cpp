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

# include <filesystem>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/SpecialFolder.hpp>
# include <Siv3D/UUIDValue.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static FilePathView FileNameView(FilePathView path) noexcept
		{
		# if SIV3D_PLATFORM(WINDOWS)
			if (FileSystem::IsResourcePath(path))
			{
				path.remove_prefix(1);
			}
		# endif

			if (const size_t separatorPos = path.find_last_of(U"/\\");
				separatorPos != String::npos)
			{
				path.remove_prefix(separatorPos + 1);
			}

			return path;
		}

		static void TrimToParentPath(FilePath& path, size_t level)
		{
			if (path.ends_with(U'/'))
			{
				path.pop_back();
			}

			while (not path.isEmpty())
			{
				do
				{
					path.pop_back();
				} while ((not path.isEmpty()) && (not path.ends_with(U'/')));

				if (level-- == 0)
				{
					break;
				}
			}
		}

		[[nodiscard]]
		inline static std::filesystem::path ToPath(const FilePathView path)
		{
			return std::filesystem::path{ Unicode::ToWstring(path) };
		}

		[[nodiscard]]
		inline static constexpr std::filesystem::copy_options ToCopyOptions(const FileCopyOption copyOption) noexcept
		{
			switch (copyOption)
			{
			case FileCopyOption::SkipExisting:
				return std::filesystem::copy_options::skip_existing;
			case FileCopyOption::OverwriteExisting:
				return std::filesystem::copy_options::overwrite_existing;
			case FileCopyOption::UpdateExisting:
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
			return Extension(path, PreserveCase::No);
		}

		String Extension(FilePathView path, const PreserveCase preserveCase)
		{
			if (path.isEmpty())
			{
				return{};
			}

		# if SIV3D_PLATFORM(WINDOWS)

			if (IsResourcePath(path))
			{
				path.remove_prefix(1);
			}

		# endif

			if (const size_t lastSeparatorPos = path.find_last_of(U"/\\");
				lastSeparatorPos != String::npos)
			{
				path.remove_prefix(lastSeparatorPos + 1);
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

			String result = path.substr(lastDotPos + 1).toString();

			if (preserveCase == PreserveCase::No)
			{
				result.lowercase();
			}

			return result;
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	FileName
		//
		////////////////////////////////////////////////////////////////

		String FileName(const FilePathView path)
		{
			return FileNameView(path).toString();
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	BaseName
		//
		////////////////////////////////////////////////////////////////

		String BaseName(const FilePathView path)
		{
			const FilePathView fileName = FileNameView(path);

			if (fileName.isEmpty())
			{
				return{};
			}

			const size_t dotPos = fileName.rfind(U'.');

			if (dotPos == String::npos)
			{
				return fileName.toString();
			}

			if ((dotPos == 0) || (dotPos == (fileName.size() - 1)))
			{
				return fileName.toString();
			}

			return fileName.substr(0, dotPos).toString();
		}

		////////////////////////////////////////////////////////////////
		//
		//	ParentPath
		//
		////////////////////////////////////////////////////////////////

		FilePath ParentPath(const FilePathView path, const size_t level)
		{
			FilePath result = FullPath(path);
			TrimToParentPath(result, level);
			return result;
		}

		FilePath ParentPath(const FilePathView path, const size_t level, FilePath& baseFullPath)
		{
			FilePath result = FullPath(path);
			baseFullPath = result;
			TrimToParentPath(result, level);
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
			if (path.isEmpty())
			{
				return false;
			}

		# if SIV3D_PLATFORM(WINDOWS)

			if (IsResourcePath(path))
			{
				return false;
			}

		# endif

			std::error_code error;
			const std::filesystem::directory_iterator it{ ToPath(path), error };
			return ((not error) && (it == std::filesystem::directory_iterator{}));
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
			if (path.isEmpty() || start.isEmpty())
			{
				return{};
			}

			if (not IsDirectory(start))
			{
				return path;
			}

			if (path == start)
			{
				return U"./";
			}

		# if SIV3D_PLATFORM(WINDOWS)
			const std::filesystem::path p = ToPath(path);
			const std::filesystem::path base = ToPath(start);
		# else
			const std::filesystem::path p(path.toUTF8());
			const std::filesystem::path base(start.toUTF8());
		# endif

			std::error_code error;
			const std::filesystem::path relativePath = std::filesystem::proximate(p, base, error);
			if (error)
			{
				return{};
			}
		# if SIV3D_PLATFORM(WINDOWS)
			FilePath result = Unicode::FromWstring(relativePath.native());
		# else
			FilePath result = Unicode::FromUTF8(relativePath.native());
		# endif

			result.replace(U'\\', U'/');

			if ((not result.ends_with(U'/')) && IsDirectory(path))
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

			FilePath fullPath;
			const FilePath parentDirectory = ParentPath(path, 0, fullPath);

			if (parentDirectory.isEmpty())
			{
				return ((not fullPath.isEmpty()) && IsDirectory(fullPath));
			}

			return (IsDirectory(parentDirectory) || CreateDirectories(parentDirectory));
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	Copy
		//
		////////////////////////////////////////////////////////////////

		bool Copy(const FilePathView from, const FilePathView to, const FileCopyOption copyOption)
		{
			if ((from.isEmpty()) || (to.isEmpty()))
			{
				return false;
			}

			if (IsResourcePath(from) || IsResourcePath(to))
			{
				return false;
			}

			if (not CreateParentDirectories(to))
			{
				return false;
			}

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
