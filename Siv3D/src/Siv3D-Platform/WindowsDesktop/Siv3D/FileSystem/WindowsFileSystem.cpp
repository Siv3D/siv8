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

# include "WindowsFileSystem.hpp"
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Resource.hpp>
# include <Shlobj.h>

namespace s3d
{
	namespace detail
	{
		inline constexpr size_t DefaultMaxPathLength = 320;

		/// @brief ファイルパスが "." または ".." であるかを返します。
		/// @param data ファイル情報
		/// @return ファイルパスが "." または ".." である場合 true, それ以外の場合は false
		[[nodiscard]]
		static constexpr bool IsCurrentOrParentDirectory(const WIN32_FIND_DATAW& data)
		{
			return ((data.cFileName[0] == L'.') &&
				((data.cFileName[1] == L'\0') || ((data.cFileName[1] == L'.') && (data.cFileName[2] == L'\0'))));
		}

		FilePathCache::FilePathCache()
			: initialDirectory{ FileSystem::CurrentDirectory() }
		{
			modulePath = []() -> FilePath
			{
				wchar_t result[1024];
				const DWORD length = ::GetModuleFileNameW(nullptr, result, _countof(result));

				if ((length == 0) || (_countof(result) <= length))
				{
					return{};
				}

				return Unicode::FromWstring(NormalizePath(std::wstring(result, length), PathType::File));
			}();

			specialFolderPaths = []()
			{
				static constexpr int ids[SpecialFolderCount] = {
					CSIDL_DESKTOP,
					CSIDL_MYDOCUMENTS,
					CSIDL_LOCAL_APPDATA,
					CSIDL_MYPICTURES,
					CSIDL_MYMUSIC,
					CSIDL_MYVIDEO,
					CSIDL_FONTS,
					CSIDL_FONTS,
					CSIDL_FONTS,
					CSIDL_PROFILE,
					CSIDL_PROGRAM_FILES,
					CSIDL_PROFILE, // Downloads の親フォルダ
				};

				std::array<FilePath, SpecialFolderCount> paths;

				for (size_t i = 0; i < paths.size(); ++i)
				{
					wchar_t path[MAX_PATH];

					if (FAILED(::SHGetFolderPathW(nullptr, ids[i], nullptr, 0, path)))
					{
						continue;
					}

					paths[i] = Unicode::FromWstring(NormalizePath(path, PathType::Directory));
				}

				paths[11].append(U"Downloads/");

				return paths;
			}();
		}

		[[nodiscard]]
		std::filesystem::file_status GetStatus(const std::wstring_view path)
		{
			return std::filesystem::status(std::filesystem::path{ path });
		}

		[[nodiscard]]
		std::filesystem::file_status GetStatus(const FilePathView path)
		{
			return GetStatus(Unicode::ToWstring(path));
		}

		bool ResourceExists(const FilePathView path)
		{
			return (::FindResourceW(::GetModuleHandleW(nullptr), Platform::Windows::ToResourceName(path).c_str(), L"FILE") != nullptr);
		}

		size_t ResourceSize(const FilePathView path)
		{
			HMODULE hModule = ::GetModuleHandleW(nullptr);

			if (HRSRC hrs = ::FindResourceW(hModule, Platform::Windows::ToResourceName(path).c_str(), L"FILE"))
			{
				return ::SizeofResource(hModule, hrs);
			}
			else
			{
				return 0;
			}
		}

		std::wstring GetNativeFullPath(const FilePathView path)
		{
			const std::wstring pathW = Unicode::ToWstring(path);
			wchar_t result[DefaultMaxPathLength];
			wchar_t* pFilePart = nullptr;
			const DWORD length = ::GetFullPathNameW(pathW.c_str(), _countof(result), result, &pFilePart);

			if (length == 0)
			{
				return{};
			}

			if (std::size(result) < length)
			{
				std::wstring result2((length - 1), L'\0');
				const DWORD length2 = ::GetFullPathNameW(pathW.c_str(), length, result2.data(), &pFilePart);

				if ((length2 == 0)
					|| (length < (length2 + 1)))
				{
					return{};
				}

				if (length2 < result2.size())
				{
					result2.resize(length2);
				}

				return result2;
			}

			return std::wstring{ result, length };
		}

		std::wstring NormalizePath(std::wstring path, const PathType pathType)
		{
			std::replace(path.begin(), path.end(), L'\\', L'/');

			if ((not path.ends_with(L'/'))
				&& ((pathType == PathType::Directory) || (GetStatus(path).type() == std::filesystem::file_type::directory)))
			{
				path.push_back(L'/');
			}

			return path;
		}

		int64 DirectorySizeRecursive(const std::wstring& directoryPath)
		{
			assert(directoryPath.ends_with(L'/'));

			WIN32_FIND_DATAW data;
			HANDLE sh = ::FindFirstFileW((directoryPath + L'*').c_str(), &data);

			if (sh == INVALID_HANDLE_VALUE)
			{
				return 0;
			}

			int64 result = 0;

			do
			{
				// "." と ".." はスキップする
				if (IsCurrentOrParentDirectory(data))
				{
					continue;
				}

				if (IsDirectory(data.dwFileAttributes)) // ディレクトリ
				{
					result += DirectorySizeRecursive((directoryPath + data.cFileName) + L'/');
				}
				else // ファイル
				{
					result += detail::GetFileSize(data.nFileSizeHigh, data.nFileSizeLow);
				}

			} while (::FindNextFileW(sh, &data));

			::FindClose(sh);

			return result;
		}

		Optional<WIN32_FILE_ATTRIBUTE_DATA> GetFileAttributeData(const std::wstring& path)
		{
			WIN32_FILE_ATTRIBUTE_DATA fad{};

			if (::GetFileAttributesExW(path.c_str(), ::GetFileExInfoStandard, &fad) == 0)
			{
				return none;
			}

			return fad;
		}

		DateTime FileTimeToTime(FILETIME in)
		{
			SYSTEMTIME systemTime;
			::FileTimeToLocalFileTime(&in, &in);
			::FileTimeToSystemTime(&in, &systemTime);

			return{ systemTime.wYear, systemTime.wMonth, systemTime.wDay,
				systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds };
		}

		void DirectoryContentsDetail(const std::wstring& directoryPath, Array<FilePath>& paths, const Recursive recursive)
		{
			assert(directoryPath.ends_with(L'/'));

			WIN32_FIND_DATAW data;
			HANDLE hFind = ::FindFirstFileW((directoryPath + L'*').c_str(), &data);

			if (hFind == INVALID_HANDLE_VALUE)
			{
				return;
			}

			do
			{
				// "." と ".." はスキップする
				if (IsCurrentOrParentDirectory(data))
				{
					continue;
				}

				const bool isDirectory = IsDirectory(data.dwFileAttributes);

				std::wstring path = (directoryPath + data.cFileName);

				if (isDirectory)
				{
					// ディレクトリの場合は末尾に / を追加する
					path.push_back(L'/');
				}

				paths << Unicode::FromWstring(path);

				// 再帰的に検索する
				if (recursive && isDirectory)
				{
					DirectoryContentsDetail(path, paths, Recursive::Yes);
				}

			} while (::FindNextFileW(hFind, &data));

			::FindClose(hFind);
		}
	}
}
