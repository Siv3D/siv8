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

# include <Siv3D/Dialog.hpp>
# import <AppKit/AppKit.h>
# if __has_include(<UniformTypeIdentifiers/UniformTypeIdentifiers.h>)
#   import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#   define SIV3D_USE_ALLOWED_CONTENT_TYPES 1
# else
#   define SIV3D_USE_ALLOWED_CONTENT_TYPES 0
# endif

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static NSString* ToNSString(const String& s)
		{
			const std::string utf8 = Unicode::ToUTF8(s);
			return [NSString stringWithUTF8String:utf8.c_str()];
		}

		[[nodiscard]]
		static String FromNSString(NSString* s)
		{
			if (s == nil)
			{
				return{};
			}

			const char* utf8 = [s UTF8String];

			if (utf8 == nullptr)
			{
				return{};
			}

			return Unicode::FromUTF8(utf8);
		}

		[[nodiscard]]
		static String NormalizeExtension(String ext)
		{
			ext.trim();

			while (ext.starts_with(U"."))
			{
				ext.erase(ext.begin());
			}

			if (ext.starts_with(U"*"))
			{
				ext.erase(ext.begin());

				while (ext.starts_with(U"."))
				{
					ext.erase(ext.begin());
				}
			}

			ext = ext.lowercased();
			return ext;
		}

		[[nodiscard]]
		static Array<FileDialogFilter> NormalizeFilters(Array<FileDialogFilter> filters)
		{
			if (filters.isEmpty())
			{
				return{ FileDialogFilter::AllFiles() };
			}

			for (auto& filter : filters)
			{
				Array<String> extensions;
				extensions.reserve(filter.extensions.size());

				for (auto ext : filter.extensions)
				{
					ext = NormalizeExtension(std::move(ext));

					if (ext.isEmpty())
					{
						continue;
					}

					if (not extensions.contains(ext))
					{
						extensions.push_back(std::move(ext));
					}
				}

				filter.extensions = std::move(extensions);

				if (filter.extensions.isEmpty() && filter.description.isEmpty())
				{
					filter.description = U"All Files";
				}
			}

			return filters;
		}

		// macOS 標準パネルでは Windows のような「説明文ごとのフィルタ切替 UI」がないため、
		// 実際に適用する型は全フィルタの拡張子を union したものにする。
		[[nodiscard]]
		static Array<String> CollectUniqueExtensions(const Array<FileDialogFilter>& filters)
		{
			Array<String> extensions;

			for (const auto& filter : filters)
			{
				for (auto ext : filter.extensions)
				{
					ext = NormalizeExtension(std::move(ext));

					if (ext.isEmpty())
					{
						continue;
					}

					if (not extensions.contains(ext))
					{
						extensions.push_back(std::move(ext));
					}
				}
			}

			return extensions;
		}

		[[nodiscard]]
		static String GetDefaultExtension(const Array<FileDialogFilter>& filters)
		{
			for (const auto& filter : filters)
			{
				for (const auto& ext : filter.extensions)
				{
					const String normalized = NormalizeExtension(ext);

					if (not normalized.isEmpty())
					{
						return normalized;
					}
				}
			}

			return{};
		}

		[[nodiscard]]
		static NSURL* MakeDirectoryURL(const FilePath& path)
		{
			if (path.isEmpty())
			{
				return nil;
			}

			const FilePath fullPath = FileSystem::FullPath(path);

			if (fullPath.isEmpty())
			{
				return nil;
			}

			return [NSURL fileURLWithPath:ToNSString(fullPath) isDirectory:YES];
		}

		[[nodiscard]]
		static Optional<FilePath> GetFilePathFromURL(NSURL* url)
		{
			if ((url == nil) || (not [url isFileURL]))
			{
				return none;
			}

			return FileSystem::FullPath(FromNSString([url path]));
		}

		[[nodiscard]]
		static Optional<FilePath> GetSaveFilePathFromURL(NSURL* url, const String& defaultExtension)
		{
			if ((url == nil) || (not [url isFileURL]))
			{
				return none;
			}

			NSString* path = [url path];

			if ((path == nil) || ([path length] == 0))
			{
				return none;
			}

			// 保存ダイアログで型 UI を出していない場合、拡張子が自動付与されないケースがあるので補完する
			if (([path pathExtension].length == 0) && (not defaultExtension.isEmpty()))
			{
				path = [path stringByAppendingPathExtension:ToNSString(defaultExtension)];
			}

			return FileSystem::FullPath(FromNSString(path));
		}

		[[nodiscard]]
		static NSArray<NSString*>* MakeAllowedFileTypes(const Array<FileDialogFilter>& filters)
		{
			const Array<String> extensions = CollectUniqueExtensions(filters);

			if (extensions.isEmpty())
			{
				return nil;
			}

			NSMutableArray<NSString*>* types = [NSMutableArray arrayWithCapacity:extensions.size()];

			for (const auto& ext : extensions)
			{
				[types addObject:ToNSString(ext)];
			}

			return types;
		}

	# if SIV3D_USE_ALLOWED_CONTENT_TYPES
		[[nodiscard]]
		static NSArray<UTType*>* MakeAllowedContentTypes(const Array<FileDialogFilter>& filters)
		{
			const Array<String> extensions = CollectUniqueExtensions(filters);

			if (extensions.isEmpty())
			{
				return nil;
			}

			NSMutableArray<UTType*>* contentTypes = [NSMutableArray arrayWithCapacity:extensions.size()];

			for (const auto& ext : extensions)
			{
				NSString* nsExt = ToNSString(ext);

				if ((nsExt == nil) || ([nsExt length] == 0))
				{
					continue;
				}

				UTType* type = [UTType typeWithFilenameExtension:nsExt];

				if (type != nil)
				{
					[contentTypes addObject:type];
				}
			}

			return ([contentTypes count] ? contentTypes : nil);
		}
	# endif

		static void ApplyCommonPanelSettings(
			NSSavePanel* panel,
			const FilePath& initialDirectory,
			const String& defaultFileName,
			const String& title,
			const String& acceptButtonText,
			const bool showHiddenFiles)
		{
			if (panel == nil)
			{
				return;
			}

			if (not title.isEmpty())
			{
				[panel setTitle:ToNSString(title)];
			}

			if (not defaultFileName.isEmpty())
			{
				[panel setNameFieldStringValue:ToNSString(defaultFileName)];
			}

			if (not acceptButtonText.isEmpty())
			{
				[panel setPrompt:ToNSString(acceptButtonText)];
			}

			if (NSURL* directoryURL = MakeDirectoryURL(initialDirectory))
			{
				[panel setDirectoryURL:directoryURL];
			}

			[panel setShowsHiddenFiles:showHiddenFiles];
			[panel setCanCreateDirectories:YES];
		}

		static void ApplyFileFilters(NSSavePanel* panel, const Array<FileDialogFilter>& filters)
		{
			if (panel == nil)
			{
				return;
			}

		# if SIV3D_USE_ALLOWED_CONTENT_TYPES
			if (@available(macOS 11.0, *))
			{
				if (NSArray<UTType*>* contentTypes = MakeAllowedContentTypes(filters))
				{
					[panel setAllowedContentTypes:contentTypes];
				}

				return;
			}
		# endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
			if (NSArray<NSString*>* fileTypes = MakeAllowedFileTypes(filters))
			{
				[panel setAllowedFileTypes:fileTypes];
			}
#pragma clang diagnostic pop
		}

		[[nodiscard]]
		static NSInteger RunModal(NSSavePanel* panel)
		{
			if (panel == nil)
			{
				return NSModalResponseCancel;
			}

			return [panel runModal];
		}

		static void ApplyOpenDialogOptions(NSOpenPanel* panel, const OpenFileDialogOptions& options, const bool multiSelect)
		{
			if (panel == nil)
			{
				return;
			}

			[panel setCanChooseFiles:YES];
			[panel setCanChooseDirectories:NO];
			[panel setAllowsMultipleSelection:multiSelect];
			[panel setResolvesAliases:YES];

			ApplyCommonPanelSettings(
				panel,
				options.initialDirectory,
				options.defaultFileName,
				options.title,
				options.acceptButtonText,
				options.showHiddenFiles);

			ApplyFileFilters(panel, NormalizeFilters(options.filters));
		}

		static void ApplySaveDialogOptions(NSSavePanel* panel, const SaveFileDialogOptions& options)
		{
			if (panel == nil)
			{
				return;
			}

			ApplyCommonPanelSettings(
				panel,
				options.initialDirectory,
				options.defaultFileName,
				options.title,
				options.acceptButtonText,
				options.showHiddenFiles);

			ApplyFileFilters(panel, NormalizeFilters(options.filters));
		}

		static void ApplySelectFolderDialogOptions(NSOpenPanel* panel, const SelectFolderDialogOptions& options)
		{
			if (panel == nil)
			{
				return;
			}

			[panel setCanChooseFiles:NO];
			[panel setCanChooseDirectories:YES];
			[panel setAllowsMultipleSelection:NO];
			[panel setResolvesAliases:YES];

			ApplyCommonPanelSettings(
				panel,
				options.initialDirectory,
				U"",
				options.title,
				options.acceptButtonText,
				options.showHiddenFiles);
		}

		[[nodiscard]]
		static Array<FilePath> GetOpenDialogResults(NSOpenPanel* panel)
		{
			Array<FilePath> results;

			if (panel == nil)
			{
				return results;
			}

			NSArray<NSURL*>* urls = [panel URLs];

			if (urls == nil)
			{
				return results;
			}

			results.reserve([urls count]);

			for (NSURL* url in urls)
			{
				if (auto path = GetFilePathFromURL(url))
				{
					results.push_back(std::move(*path));
				}
			}

			return results;
		}
	}

	namespace Dialog
	{
		////////////////////////////////////////////////////////////////
		//
		//	OpenFile, OpenFiles
		//
		////////////////////////////////////////////////////////////////

		Optional<FilePath> OpenFile(const OpenFileDialogOptions& options)
		{
			@autoreleasepool
			{
				NSOpenPanel* panel = [NSOpenPanel openPanel];
				ApplyOpenDialogOptions(panel, options, false);

				const NSInteger result = RunModal(panel);

				if (result != NSModalResponseOK)
				{
					return none;
				}

				Array<FilePath> results = GetOpenDialogResults(panel);

				if (results.isEmpty())
				{
					return none;
				}

				return std::move(results.front());
			}
		}

		Array<FilePath> OpenFiles(const OpenFileDialogOptions& options)
		{
			@autoreleasepool
			{
				NSOpenPanel* panel = [NSOpenPanel openPanel];
				ApplyOpenDialogOptions(panel, options, true);

				const NSInteger result = RunModal(panel);

				if (result != NSModalResponseOK)
				{
					return{};
				}

				return GetOpenDialogResults(panel);
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	SaveFile
		//
		////////////////////////////////////////////////////////////////

		Optional<FilePath> SaveFile(const SaveFileDialogOptions& options)
		{
			@autoreleasepool
			{
				NSSavePanel* panel = [NSSavePanel savePanel];
				ApplySaveDialogOptions(panel, options);

				const NSInteger result = RunModal(panel);

				if (result != NSModalResponseOK)
				{
					return none;
				}

				const Array<FileDialogFilter> effectiveFilters = NormalizeFilters(options.filters);
				const String defaultExtension = GetDefaultExtension(effectiveFilters);

				return GetSaveFilePathFromURL([panel URL], defaultExtension);
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	SelectFolder
		//
		////////////////////////////////////////////////////////////////

		Optional<FilePath> SelectFolder(const SelectFolderDialogOptions& options)
		{
			@autoreleasepool
			{
				NSOpenPanel* panel = [NSOpenPanel openPanel];
				ApplySelectFolderDialogOptions(panel, options);

				const NSInteger result = RunModal(panel);

				if (result != NSModalResponseOK)
				{
					return none;
				}

				return GetFilePathFromURL([panel URL]);
			}
		}
	}
}
