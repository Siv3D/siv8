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

# include <Siv3D/FileSystem.hpp>
# include "WindowsFileSystem.hpp"

namespace s3d
{
	namespace detail
	{
		namespace init
		{
			const static FilePathCache g_filePathCache{};
		}
	}

	namespace FileSystem
	{
		bool IsResourcePath(const FilePathView path) noexcept
		{
			return path.starts_with(U'/');
		}

		bool Exists(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return false;
			}

			if (IsResourcePath(path))
			{
				return detail::ResourceExists(path);
			}

			return (detail::GetStatus(path).type() != std::filesystem::file_type::not_found);
		}

		bool IsDirectory(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return false;
			}

			if (IsResourcePath(path))
			{
				return false;
			}

			return (detail::GetStatus(path).type() == std::filesystem::file_type::directory);
		}

		bool IsFile(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return false;
			}

			if (IsResourcePath(path))
			{
				return detail::ResourceExists(path);
			}

			return (detail::GetStatus(path).type() == std::filesystem::file_type::regular);
		}

		bool IsResource(const FilePathView path)
		{
			return (IsResourcePath(path)
				&& detail::ResourceExists(path));
		}

		FilePath FullPath(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return{};
			}

			if (IsResourcePath(path))
			{
				return path.toString();
			}

			const std::wstring nativeFullPath = detail::GetNativeFullPath(path);

			return Unicode::FromWstring(detail::NormalizePath(nativeFullPath, detail::PathType::Unknown));
		}

		NativeFilePath NativePath(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return{};
			}

			if (IsResourcePath(path))
			{
				return{};
			}

			return detail::GetNativeFullPath(path);
		}

		FilePath VolumePath(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return{};
			}

			if (IsResourcePath(path))
			{
				return{};
			}

			const std::wstring pathW = Unicode::ToWstring(path);
			wchar_t result[MAX_PATH];

			if (::GetVolumePathNameW(pathW.c_str(), result, _countof(result)) == 0)
			{
				return{};
			}

			return Unicode::FromWstring(result).replace(U'\\', U'/');
		}

		size_t Size(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return 0;
			}

			if (IsResourcePath(path))
			{
				return detail::ResourceSize(path);
			}

			const std::wstring pathW = Unicode::ToWstring(path);

			if (const auto type = detail::GetStatus(pathW).type(); 
				type == std::filesystem::file_type::regular)
			{
				if (const auto fad = detail::GetFileAttributeData(pathW))
				{
					return detail::GetFileSize(fad->nFileSizeHigh, fad->nFileSizeLow);
				}
				else
				{
					return 0;
				}
			}
			else if (type == std::filesystem::file_type::directory)
			{
				return detail::DirectorySizeRecursive(Unicode::ToWstring(FullPath(path)));
			}
			else
			{
				return 0;
			}
		}

		size_t FileSize(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return 0;
			}

			if (IsResourcePath(path))
			{
				return detail::ResourceSize(path);
			}

			if (const auto fad = detail::GetFileAttributeData(Unicode::ToWstring(path)))
			{
				// ディレクトリの場合は 0 を返す
				if (detail::IsDirectory(fad->dwFileAttributes))
				{
					return 0;
				}

				return detail::GetFileSize(fad->nFileSizeHigh, fad->nFileSizeLow);
			}
			else
			{
				return 0;
			}
		}

		Optional<DateTime> CreationTime(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return none;
			}

			if (IsResourcePath(path))
			{
				return CreationTime(ModulePath());
			}

			if (const auto fad = detail::GetFileAttributeData(Unicode::ToWstring(path)))
			{
				return detail::FileTimeToTime(fad->ftCreationTime);
			}
			else
			{
				return none;
			}
		}

		Optional<DateTime> WriteTime(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return none;
			}

			if (IsResourcePath(path))
			{
				return WriteTime(ModulePath());
			}

			if (const auto fad = detail::GetFileAttributeData(Unicode::ToWstring(path)))
			{
				return detail::FileTimeToTime(fad->ftLastWriteTime);
			}
			else
			{
				return none;
			}
		}

		Optional<DateTime> AccessTime(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return none;
			}

			if (IsResourcePath(path))
			{
				return AccessTime(ModulePath());
			}

			if (const auto fad = detail::GetFileAttributeData(Unicode::ToWstring(path)))
			{
				return detail::FileTimeToTime(fad->ftLastAccessTime);
			}
			else
			{
				return none;
			}
		}

		Array<FilePath> DirectoryContents(const FilePathView path, const Recursive recursive)
		{
			Array<FilePath> paths;

			if (path.isEmpty())
			{
				return paths;
			}

			if (IsResourcePath(path))
			{
				return paths;
			}

			if (detail::GetStatus(path).type() != std::filesystem::file_type::directory)
			{
				return paths;
			}

			detail::DirectoryContentsDetail(Unicode::ToWstring(FullPath(path)), paths, recursive);

			return paths;
		}

		const FilePath& InitialDirectory() noexcept
		{
			return detail::init::g_filePathCache.initialDirectory;
		}

		const FilePath& ModulePath() noexcept
		{
			return detail::init::g_filePathCache.modulePath;
		}

		FilePath CurrentDirectory()
		{
			wchar_t result[1024];
			const DWORD length = ::GetCurrentDirectoryW(_countof(result), result);

			if (length == 0)
			{
				return{};
			}
			else if (std::size(result) < length)
			{
				std::wstring result2((length - 1), L'\0');
				const DWORD length2 = ::GetCurrentDirectoryW(length, result2.data());

				if ((length2 == 0)
					|| (length < (length2 + 1)))
				{
					return{};
				}

				if (length2 < result2.size())
				{
					result2.resize(length2);
				}

				return Unicode::FromWstring(detail::NormalizePath(result2, detail::PathType::Directory));
			}

			return Unicode::FromWstring(detail::NormalizePath(std::wstring(result, length), detail::PathType::Directory));
		}

		bool ChangeCurrentDirectory(const FilePathView path)
		{
			if (not IsDirectory(path))
			{
				return false;
			}

			return (::SetCurrentDirectoryW(Unicode::ToWstring(path).c_str()) != 0);
		}

		const FilePath& GetFolderPath(const SpecialFolder folder)
		{
			return detail::init::g_filePathCache.specialFolderPaths[FromEnum(folder)];
		}

		bool Remove(const FilePathView path, const AllowUndo allowUndo)
		{
			if (path.isEmpty())
			{
				return false;
			}

			if (IsResourcePath(path))
			{
				return false;
			}

			const std::wstring from = Unicode::ToWstring((path + U'\0').replaced(U'/', U'\\'));

			SHFILEOPSTRUCTW fileOption =
			{
				.wFunc	= FO_DELETE,
				.pFrom	= from.c_str(), // must be double-null terminated
				.fFlags	= FILEOP_FLAGS(FOF_NOERRORUI | FOF_SILENT | FOF_NOCONFIRMATION | (allowUndo ? FOF_ALLOWUNDO : 0)),
			};

			return (::SHFileOperationW(&fileOption) == 0)
				&& (not fileOption.fAnyOperationsAborted);
		}

		bool RemoveContents(const FilePathView path, const AllowUndo allowUndo)
		{
			if (not IsDirectory(path))
			{
				return false;
			}

			return Remove((path + U"\\*"), allowUndo);
		}
	}
}
