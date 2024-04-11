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

# include <Siv3D/Resource.hpp>
# include <Siv3D/Windows/Windows.hpp>

namespace s3d
{
	namespace
	{
		static BOOL CALLBACK EnumResourceFilePathCallback(HMODULE, LPCWSTR, LPWSTR lpName, LONG_PTR lParam)
		{
			Array<std::wstring>& files = *reinterpret_cast<Array<std::wstring>*>(lParam);

			files.push_back(lpName);

			return true;
		}
	}

	static const Array<FilePath> g_resourceFilePaths = []()
		{
			Array<std::wstring> files;

			HMODULE hModule = ::GetModuleHandleW(nullptr);

			::EnumResourceNamesW(hModule, L"FILE", EnumResourceFilePathCallback, (LONG_PTR)&files);

			Array<FilePath> paths;

			for (const auto& file : files)
			{
				HRSRC hResource = ::FindResourceW(hModule, file.c_str(), L"PATH");
				
				if (not hResource)
				{
					continue;
				}

				HGLOBAL hData = ::LoadResource(hModule, hResource);

				if (not hData)
				{
					continue;
				}

				const DWORD dataSize = SizeofResource(hModule, hResource);

				if (dataSize == 0)
				{
					continue;
				}

				const void* pData = ::LockResource(hData);

				if (not pData)
				{
					continue;
				}
				
				const std::wstring_view pathView{ static_cast<const wchar_t*>(pData), (dataSize / sizeof(wchar_t)) };

				paths.push_back(L'/' + Unicode::FromWstring(pathView));
			}

			paths.sort();

			return paths;
		}();

	const Array<FilePath>& EnumResourceFiles() noexcept
	{
		return g_resourceFilePaths;
	}

	FilePath Resource(const FilePathView path)
	{
		if (path.starts_with(U'/'))
		{
			return FilePath{ path };
		}

		return (U'/' + path);
	}

	namespace Platform::Windows
	{
		std::wstring ToResourceName(const FilePathView resourcePath)
		{
			if (not resourcePath.starts_with(U'/'))
			{
				return{};
			}

			return (L'\"' + Unicode::ToWstring(resourcePath.substr(1)) + L'\"');
		}
	}
}
