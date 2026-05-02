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
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Unicode.hpp>
# import  <Cocoa/Cocoa.h>


namespace s3d
{
	namespace
	{
		static NSString* ToNSString(const StringView s)
		{
			const std::string utf8 = Unicode::ToUTF8(s);
			return [[[NSString alloc] initWithBytes:utf8.data()
											 length:utf8.size()
										   encoding:NSUTF8StringEncoding] autorelease];
		}

		static NSURL* ToFileURL(const FilePathView path, const bool isDirectory = false)
		{
			const FilePath fullPath = FileSystem::FullPath(path);
			NSString* nsPath = ToNSString(fullPath);
			return [NSURL fileURLWithPath:nsPath isDirectory:isDirectory];
		}
	}

	namespace System
	{
		bool OpenInBrowser(const URLView url)
		{
			@autoreleasepool
			{
				if (url.starts_with(U"http://") || url.starts_with(U"https://"))
				{
					NSString* nsURLString = ToNSString(url);
					NSURL* nsURL = [NSURL URLWithString:nsURLString];

					if (nsURL == nil)
					{
						return false;
					}

					return ([[NSWorkspace sharedWorkspace] openURL:nsURL] == YES);
				}
				else
				{
					const String extension = FileSystem::Extension(url);

					const bool isHTML = ((extension == U"html") || (extension == U"htm"));

					if (not isHTML)
					{
						return false;
					}

					NSURL* fileURL = ToFileURL(url, false);

					return ([[NSWorkspace sharedWorkspace] openURL:fileURL] == YES);
				}
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//  ShowInFileManager
		//
		////////////////////////////////////////////////////////////////

		bool ShowInFileManager(const FilePathView path)
		{
			@autoreleasepool
			{
				const FilePath fullPath = FileSystem::FullPath(path);

				if (FileSystem::IsFile(fullPath))
				{
					NSURL* fileURL = ToFileURL(fullPath, false);

					// activateFileViewerSelectingURLs: は void なので、
					// 存在確認済みなら true とする
					[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[ fileURL ]];

					return true;
				}
				else if (FileSystem::IsDirectory(fullPath))
				{
					NSURL* dirURL = ToFileURL(fullPath, true);

					return ([[NSWorkspace sharedWorkspace] openURL:dirURL] == YES);
				}

				return false;
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//  LaunchFile
		//
		////////////////////////////////////////////////////////////////

		bool LaunchFile(const FilePathView fileName)
		{
			@autoreleasepool
			{
				const FilePath fullPath = FileSystem::FullPath(fileName);

				if ((not FileSystem::IsFile(fullPath)) && (not FileSystem::IsDirectory(fullPath)))
				{
					return false;
				}

				const bool isDirectory = FileSystem::IsDirectory(fullPath);
				NSURL* fileURL = ToFileURL(fullPath, isDirectory);

				return ([[NSWorkspace sharedWorkspace] openURL:fileURL] == YES);
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//  LaunchFileWithTextEditor
		//
		////////////////////////////////////////////////////////////////

		bool LaunchFileWithTextEditor(const FilePathView fileName)
		{
			@autoreleasepool
			{
				const FilePath fullPath = FileSystem::FullPath(fileName);

				if (not FileSystem::IsFile(fullPath))
				{
					return false;
				}

				NSURL* fileURL = ToFileURL(fullPath, false);

				NSURL* textEditURL =
					[[NSWorkspace sharedWorkspace] URLForApplicationWithBundleIdentifier:@"com.apple.TextEdit"];

				if (textEditURL == nil)
				{
					return false;
				}

				NSWorkspaceOpenConfiguration* config = [NSWorkspaceOpenConfiguration configuration];
				config.activates = YES;

				[[NSWorkspace sharedWorkspace] openURLs:@[ fileURL ]
								withApplicationAtURL:textEditURL
										configuration:config
									completionHandler:nil];

				return true;
			}
		}
	}
}
