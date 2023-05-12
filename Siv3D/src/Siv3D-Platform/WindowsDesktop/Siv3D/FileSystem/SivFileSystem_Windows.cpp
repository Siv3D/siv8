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
# include <array>
# include <Siv3D/Windows/Windows.hpp>
# include <Shlobj.h>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Array.hpp>

namespace s3d
{
	inline constexpr size_t DefaultMaxPathLength = 320;

	namespace detail
	{
		[[nodiscard]]
		inline static std::filesystem::file_status GetStatus(const std::wstring_view path)
		{
			return std::filesystem::status(std::filesystem::path{ path });
		}

		[[nodiscard]]
		inline static std::filesystem::file_status GetStatus(const FilePathView path)
		{
			return GetStatus(path.toWstr());
		}

		[[nodiscard]]
		static FilePath NormalizePath(std::wstring path, const bool skipDirectoryCheck = false)
		{
			std::replace(path.begin(), path.end(), L'\\', L'/');

			if ((not path.ends_with(L'/'))
				&& (skipDirectoryCheck || (GetStatus(path).type() == std::filesystem::file_type::directory)))
			{
				path.push_back(L'/');
			}

			return Unicode::FromWstring(path);
		}

		[[nodiscard]]
		HRSRC GetResource(const HMODULE hModule, const FilePathView path)
		{
			return ::FindResourceW(hModule, path.substr(1).toWstr().c_str(), L"FILE");
		}

		[[nodiscard]]
		static bool ResourceExists(const FilePathView path)
		{
			const HMODULE hModule = ::GetModuleHandleW(nullptr);

			return (GetResource(hModule, path) != nullptr);
		}

		[[nodiscard]]
		static int64 ResourceSize(const FilePathView path)
		{
			const HMODULE hModule = ::GetModuleHandleW(nullptr);

			if (HRSRC hrs = GetResource(hModule, path))
			{
				return ::SizeofResource(hModule, hrs);
			}
			else
			{
				return 0;
			}
		}

		struct FilePathCache
		{
			FilePath initialDirectory;

			FilePath modulePath;

			std::array<FilePath, 11> specialFolderPaths;

			Array<FilePath> resourceFilePaths;

			void init()
			{
				modulePath = []()->FilePath
				{
					wchar_t result[1024];
					const DWORD length = ::GetModuleFileNameW(nullptr, result, _countof(result));

					if ((length == 0) || (_countof(result) <= length))
					{
						return{};
					}

					return NormalizePath(std::wstring{ result, length });
				}();

				specialFolderPaths = []()
				{
					static constexpr int ids[] = {
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
					};

					std::array<FilePath, 11> paths;

					for (size_t i = 0; i < paths.size(); ++i)
					{
						wchar_t path[MAX_PATH];

						if (FAILED(::SHGetFolderPathW(nullptr, ids[i], nullptr, 0, path)))
						{
							continue;
						}

						paths[i] = NormalizePath(path, true);
					}

					return paths;
				}();

				resourceFilePaths = []()
				{
					Array<FilePath> paths;

					HMODULE hModule = ::GetModuleHandleW(nullptr);

					::EnumResourceNamesW(hModule, L"FILE", EnumResourceNameCallback, (LONG_PTR)&paths);

					std::ranges::sort(paths);

					return paths;
				}();
			}

		private:

			static BOOL CALLBACK EnumResourceNameCallback(HMODULE, LPCWSTR, LPWSTR lpName, LONG_PTR lParam)
			{
				Array<FilePath>& paths = *reinterpret_cast<Array<FilePath>*>(lParam);

				paths.push_back(U'/' + Unicode::FromWstring(lpName));

				return true;
			}
		};

		namespace init
		{
			static FilePathCache g_filePathCache;

			void InitFileSystem()
			{
				g_filePathCache.init();
			}
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
			if (not path) [[unlikely]]
			{
				return false;
			}

			if (IsResourcePath(path))
			{
				return detail::ResourceExists(path);
			}

			return (detail::GetStatus(path).type() != std::filesystem::file_type::not_found);
		}


		FilePath FullPath(const FilePathView path)
		{
			if (not path) [[unlikely]]
			{
				return{};
			}

			if (IsResourcePath(path)) [[unlikely]]
			{
				return FilePath(path);
			}

			const std::wstring pathW = path.toWstr();
			wchar_t result[DefaultMaxPathLength];
			wchar_t* pFilePart = nullptr;
			const DWORD length = ::GetFullPathNameW(pathW.c_str(), _countof(result), result, &pFilePart);

			if (length == 0) [[unlikely]]
			{
				return{};
			}
			
			if (std::size(result) < length) [[unlikely]]
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

				const bool isDirectory = (pFilePart == nullptr);
				return detail::NormalizePath(result2, isDirectory);
			}

			const bool isDirectory = (pFilePart == nullptr);
			return detail::NormalizePath(std::wstring{ result, length }, isDirectory);
		}







		int64 FileSize(const FilePathView path)
		{
			if (not path) [[unlikely]]
			{
				return 0;
			}

			if (IsResourcePath(path))
			{
				return detail::ResourceSize(path);
			}

			WIN32_FILE_ATTRIBUTE_DATA fad{};

			if (::GetFileAttributesExW(path.toWstr().c_str(), ::GetFileExInfoStandard, &fad) == 0) [[unlikely]]
			{
				return 0;
			}

			// ディレクトリの場合は 0
			if (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				return 0;
			}

			return ((static_cast<uint64>(fad.nFileSizeHigh) << 32) + fad.nFileSizeLow);
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
			wchar_t result[DefaultMaxPathLength];
			const DWORD length = ::GetCurrentDirectoryW(_countof(result), result);

			if (length == 0)
			{
				return{};
			}
			else if (std::size(result) < length)
			{
				std::wstring result2(length - 1, L'\0');
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

				return detail::NormalizePath(result2, true);
			}

			return detail::NormalizePath(std::wstring{ result, length }, true);
		}

		bool Remove(const FilePathView path, const AllowUndo allowUndo)
		{
			if (not path)
			{
				return false;
			}

			if (IsResourcePath(path))
			{
				return false;
			}

			std::wstring pathW = path.toWstr();
			std::replace(pathW.begin(), pathW.end(), L'/', L'\\');
			pathW.push_back(L'\0');

			SHFILEOPSTRUCTW fileOption =
			{
				.wFunc = FO_DELETE,
				.pFrom = pathW.c_str(), // must be double-null terminated
				.fFlags = FILEOP_FLAGS(FOF_NOERRORUI | FOF_SILENT | FOF_NOCONFIRMATION | (allowUndo ? FOF_ALLOWUNDO : 0)),
			};

			return ((::SHFileOperationW(&fileOption) == 0)
				&& (not fileOption.fAnyOperationsAborted));
		}
	}
}
