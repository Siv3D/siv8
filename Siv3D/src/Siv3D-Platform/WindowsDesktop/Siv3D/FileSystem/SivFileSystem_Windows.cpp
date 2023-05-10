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
# include <Shlobj.h>
# include <Siv3D/Windows/MinWindows.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Unicode.hpp>

namespace s3d
{
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

			if ((not path.ends_with(U'/'))
				&& (skipDirectoryCheck || (GetStatus(path).type() == std::filesystem::file_type::directory)))
			{
				path.push_back(U'/');
			}

			return Unicode::FromWstring(path);
		}

		[[nodiscard]]
		static int64 ResourceSize(const FilePathView path)
		{
			HMODULE hModule = ::GetModuleHandleW(nullptr);

			if (HRSRC hrs = ::FindResourceW(hModule, path.substr(1).toWstr().c_str(), L"FILE"))
			{
				return ::SizeofResource(hModule, hrs);
			}
			else
			{
				return 0;
			}
		}
	}

	namespace FileSystem
	{
		bool IsResourcePath(const FilePathView path) noexcept
		{
			return path.starts_with(U'/');
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
			wchar_t result[320];
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
