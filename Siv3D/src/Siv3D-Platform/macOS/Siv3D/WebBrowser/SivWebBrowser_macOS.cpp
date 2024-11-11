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

# include <array>
# include <CoreFoundation/CoreFoundation.h>
# include <ApplicationServices/ApplicationServices.h>
# include <Siv3D/WebBrowser.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/FileSystem.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static FilePath GetAppPath(const char* bundleID)
		{
			CFStringRef cfBundleId = CFStringCreateWithCString(NULL, bundleID, kCFStringEncodingUTF8);
			
			if (cfBundleId == NULL)
			{
				return{};
			}
			
			CFArrayRef appURLs = LSCopyApplicationURLsForBundleIdentifier(cfBundleId, NULL);
			
			if (appURLs == NULL)
			{
				CFRelease(cfBundleId);
				
				return{};
			}

			FilePath appPath;
			
			if ((appURLs != NULL) && (0 < CFArrayGetCount(appURLs)))
			{
				CFURLRef appURL = (CFURLRef)CFArrayGetValueAtIndex(appURLs, 0);
				
				char path[PATH_MAX];
				
				if (CFURLGetFileSystemRepresentation(appURL, true, (UInt8*)path, PATH_MAX))
				{
					appPath = Unicode::FromUTF8(path);
				}
				
				CFRelease(appURLs);
			}
			
			CFRelease(cfBundleId);

			return appPath;
		}
	
		constexpr std::array<const char*, 7> BrowserBundleIDs =
		{
			"com.google.Chrome",
			"org.mozilla.firefox",
			"com.microsoft.Edge",
			"com.apple.Safari",
			"com.operasoftware.Opera",
			"com.brave.Browser",
			"com.vivaldi.Vivaldi",
		};
	
		constexpr std::array<const char32*, 7> BrowserPaths =
		{
			U"/Applications/Google Chrome.app",
			U"/Applications/Firefox.app",
			U"/Applications/Microsoft Edge.app",
			U"/Applications/Safari.app",
			U"/Applications/Opera.app",
			U"/Applications/Brave Browser.app",
			U"/Applications/Vivaldi.app",
		};
	}

	namespace System
	{
		FilePath GetWebBrowserPath(const WebBrowser webBrowser)
		{
			if (FileSystem::Exists(BrowserPaths[FromEnum(webBrowser)]))
			{
				return BrowserPaths[FromEnum(webBrowser)];
			}
			
			return GetAppPath(BrowserBundleIDs[FromEnum(webBrowser)]);
		}
	}
}
