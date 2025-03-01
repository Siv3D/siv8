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
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Unicode.hpp>
# import  <Cocoa/Cocoa.h>


namespace s3d
{
	namespace detail
	{
		[[nodiscard]]
		static bool MacOS_OpenURLInBrowser(const char* _url)
		{
			@autoreleasepool
			{
				NSString* url = [NSString stringWithUTF8String:_url];
				NSURL* ns_url = [NSURL URLWithString:url];
				const bool result = [[NSWorkspace sharedWorkspace]
									openURLs:@[ns_url]
									withAppBundleIdentifier:nil
									options:NSWorkspaceLaunchDefault
									additionalEventParamDescriptor:nil
									launchIdentifiers:nil];
				return result;
			}
		}
	
		[[nodiscard]]
		static bool MacOS_OpenHTMLInBrowser(const char* _path)
		{
			@autoreleasepool
			{
				NSString* pathString = [NSString stringWithUTF8String:_path];
				NSURL* url = [NSURL fileURLWithPath:pathString];
				const bool result = [[NSWorkspace sharedWorkspace] openURL:url];
				return result;
			}
		}
	}

	namespace System
	{
		bool LaunchBrowser(const FilePathView url)
		{
			if (const bool isWebPage = (url.starts_with(U"http://") || url.starts_with(U"https://"))) // Web ページ
			{
                return detail::MacOS_OpenURLInBrowser(Unicode::ToUTF8(url).c_str());
			}
			else // ローカルファイル
			{
				const String extension = FileSystem::Extension(url);
				
				const bool isHTML = ((extension == U"html") || (extension == U"htm"));

				if (not isHTML)
				{
					return false;
				}

				return detail::MacOS_OpenHTMLInBrowser(Unicode::ToUTF8(FileSystem::FullPath(url)).c_str());
			}
		}
	}
}
