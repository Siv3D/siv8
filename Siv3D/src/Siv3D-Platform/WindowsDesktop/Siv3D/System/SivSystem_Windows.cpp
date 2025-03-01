//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/System.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Windows/Windows.hpp>

namespace s3d
{
	namespace System
	{
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

		bool LaunchBrowser(const FilePathView url)
		{
			String target;

			if (const bool isWebPage = (url.starts_with(U"http://") || url.starts_with(U"https://"))) // Web ページ
			{
				target = url;
			}
			else // ローカルファイル
			{
				const String extension = FileSystem::Extension(url);
				
				const bool isHTML = ((extension == U"html") || (extension == U"htm"));

				if (not isHTML)
				{
					return false;
				}

				target = (U"file://" + FileSystem::FullPath(url));
			}

			return (32 < reinterpret_cast<size_t>(::ShellExecuteW(nullptr, L"open", Unicode::ToWstring(target).c_str(), nullptr, nullptr, SW_SHOWNORMAL)));
		}
	}
}
