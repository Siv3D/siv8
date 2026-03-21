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

# include <Siv3D/System.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Windows/Windows.hpp>

namespace s3d
{
	namespace System
	{
		////////////////////////////////////////////////////////////////
		//
		//	Sleep
		//
		////////////////////////////////////////////////////////////////

		void Sleep(const int32 milliseconds)
		{
			if (milliseconds < 0)
			{
				return;
			}

			// システムクロックの精度を上げる
			::timeBeginPeriod(1);
			{
				::Sleep(milliseconds);
			}
			::timeEndPeriod(1);
		}

		////////////////////////////////////////////////////////////////
		//
		//	OpenInBrowser
		//
		////////////////////////////////////////////////////////////////

		bool OpenInBrowser(const URLView url)
		{
			String target;

			if (const bool isWebPage = (url.starts_with(U"http://") || url.starts_with(U"https://"))) // Web ページ
			{
				target = url;
			}
			else // ローカルファイル
			{
				const String extension = FileSystem::Extension(url); // 小文字の拡張子を取得
				
				const bool isHTML = ((extension == U"html") || (extension == U"htm"));

				if (not isHTML)
				{
					return false;
				}

				target = (U"file:///" + FileSystem::FullPath(url));
			}

			return (32 < reinterpret_cast<INT_PTR>(
				::ShellExecuteW(nullptr, L"open", Unicode::ToWstring(target).c_str(), nullptr, nullptr, SW_SHOWNORMAL)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	ShowInFileManager
		//
		////////////////////////////////////////////////////////////////

		bool ShowInFileManager(const FilePathView path)
		{
			const FilePath fullPath = FileSystem::FullPath(path);

			if (FileSystem::IsFile(fullPath))
			{
				const auto native = fullPath.replaced(U'/', U'\\');
				const String param = U"/select, \"{}\""_fmt(native);
				const std::wstring wParam = Unicode::ToWstring(param);

				return (32 < reinterpret_cast<INT_PTR>(
					::ShellExecuteW(nullptr, L"open", L"explorer.exe", wParam.c_str(), nullptr, SW_SHOWNORMAL)));
			}
			else if (FileSystem::IsDirectory(fullPath))
			{
				const std::wstring wFullPath = Unicode::ToWstring(fullPath);

				return (32 < reinterpret_cast<INT_PTR>(
					::ShellExecuteW(nullptr, L"open", wFullPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL)));
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	LaunchFile
		//
		////////////////////////////////////////////////////////////////

		bool LaunchFile(const FilePathView fileName)
		{
			const NativeFilePath fullpath = FileSystem::NativePath(fileName);

			return (32 < reinterpret_cast<INT_PTR>(
				::ShellExecuteW(nullptr, nullptr, fullpath.c_str(), nullptr, nullptr, SW_SHOWNORMAL)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	LaunchFileWithTextEditor
		//
		////////////////////////////////////////////////////////////////

		bool LaunchFileWithTextEditor(const FilePathView fileName)
		{
			const auto params = (L"\"" + FileSystem::NativePath(fileName) + L"\"");

			return (32 < reinterpret_cast<INT_PTR>(
				::ShellExecuteW(nullptr, L"open", L"notepad.exe", params.c_str(), nullptr, SW_SHOWNORMAL)));
		}
	}
}
