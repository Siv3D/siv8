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

# include <sys/stat.h>
# include <unistd.h>
# include <mach-o/dyld.h>
# include <filesystem>
# include <Foundation/Foundation.h>
# include <Siv3D/String.hpp>
# include <Siv3D/StringAlgorithm.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/SpecialFolder.hpp>
# include "macOSFileSystem.hpp"

namespace s3d
{
	namespace detail
	{
		namespace init
		{
			const static FilePathCache g_filePathCache{};
		
			const Array<FilePath>& GetResourceFiles() noexcept
			{
				return g_filePathCache.resourceFilePaths;
			}
		}
	
		[[nodiscard]]
		static std::filesystem::path ToPath(const FilePathView path)
		{
			return std::filesystem::path(Unicode::ToUTF8(path));
		}

		[[nodiscard]]
		static bool GetStat(const FilePathView path, struct stat& s)
		{
			return (::stat(Unicode::ToUTF8(FilePath{ path }.replaced(U'\\', U'/')).c_str(), &s) == 0);
		}
	
		[[nodiscard]]
		static bool Exists(const FilePathView path)
		{
			struct stat s;
			return GetStat(path, s);
		}

		[[nodiscard]]
		static bool IsRegular(const FilePathView path)
		{
			struct stat s;
			if (!GetStat(path, s))
			{
				return false;
			}

			return S_ISREG(s.st_mode);
		}
	
		[[nodiscard]]
		static bool IsDirectory(const FilePathView path)
		{
			struct stat s;
			if (!GetStat(path, s))
			{
				return false;
			}
			
			return S_ISDIR(s.st_mode);
		}
		
		[[nodiscard]]
		static std::string MacOS_FullPath(const char* _path)
		{
			@autoreleasepool
			{
				NSString* path = [NSString stringWithUTF8String:_path];
				NSURL* file = [NSURL fileURLWithPath:path];
				NSURL* absolutePath = [file absoluteURL];
				NSString* str = [absolutePath path];
				return std::string([str UTF8String], [str lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
			}
		}
		
		[[nodiscard]]
		static bool MacOS_ChangeCurrentDirectory(const char* _path)
		{
			@autoreleasepool
			{
				NSString* path = [NSString stringWithUTF8String:_path];
				return ([[NSFileManager defaultManager] changeCurrentDirectoryPath:path] == YES);
			}
		}
	
		[[nodiscard]]
		static std::string MacOS_SpecialFolder(const SpecialFolder folder)
		{
			@autoreleasepool
			{
				const NSSearchPathDirectory folders[] = {
					NSDesktopDirectory,
					NSDocumentDirectory,
					NSCachesDirectory,
					NSPicturesDirectory,
					NSMusicDirectory,
					NSMoviesDirectory,
					NSLibraryDirectory, // (placeholder)
					NSLibraryDirectory, // (placeholder)
					NSLibraryDirectory, // (placeholder)
					NSLibraryDirectory, // (placeholder)
					NSApplicationDirectory,
					NSDownloadsDirectory,
				};
				
				NSArray* paths = NSSearchPathForDirectoriesInDomains(folders[FromEnum(folder)], NSUserDomainMask, YES);
				
				NSString* directory = [paths objectAtIndex:0];
				
				// NSCachesDirectory
				if (folder == SpecialFolder::LocalAppData)
				{
					NSString* bundleID = [[NSBundle mainBundle] bundleIdentifier];
					directory = [directory stringByAppendingString:@"/Siv3DApp/"];
					directory = [directory stringByAppendingString:bundleID];
				}
				else if (folder == SpecialFolder::SystemFonts)
				{
					directory = @"/System/Library/Fonts";
				}
				else if (folder == SpecialFolder::LocalFonts)
				{
					directory = @"/Library/Fonts";
				}
				else if (folder == SpecialFolder::UserFonts)
				{
					directory = [directory stringByAppendingString:@"/Fonts"];
				}
				else if (folder == SpecialFolder::UserProfile)
				{
					directory = NSHomeDirectory();
				}
				
				return [directory UTF8String];
			}
		}

		[[nodiscard]]
		static bool MacOS_TrashFile(const std::string_view path, const bool isDirectory)
		{
			@autoreleasepool
			{
				NSURL* url = (CFBridgingRelease(CFURLCreateFromFileSystemRepresentation(NULL, (const UInt8*)path.data(), path.size(), isDirectory)));
				
				const bool result = [[NSFileManager defaultManager] trashItemAtURL: url
																  resultingItemURL: nil
																			 error: nil];
				
				return result;
			}
		}
	
		FilePathCache::FilePathCache()
		{
			initialDirectory = []() -> FilePath
			{
				char path_str[4096];
				uint32_t bufferSize = sizeof(path_str);

				if (_NSGetExecutablePath(path_str, &bufferSize) != 0)
				{
					return{};
				}

				const String path = Unicode::FromUTF8(path_str);

				FilePath modulePath = ParentPath(path, 2);

				if (modulePath.ends_with(U'/'))
				{
					modulePath.pop_back();
				}
				
				FilePath initialPath = ParentPath(path, 3);
				
				::chdir(Unicode::ToUTF8(initialPath).c_str());

				return initialPath;
			}();
			
			modulePath = []() -> FilePath
			{
				char path_str[4096];
				uint32_t bufferSize = sizeof(path_str);

				if (_NSGetExecutablePath(path_str, &bufferSize) != 0)
				{
					return FilePath{};
				}

				const String path = Unicode::FromUTF8(path_str);

				FilePath modulePath = ParentPath(path, 2);

				if (modulePath.ends_with(U'/'))
				{
					modulePath.pop_back();
				}

				return modulePath;
			}();
			
			specialFolderPaths = []()
			{
				std::array<FilePath, 12> specialFolderPaths;

				for (int32 i = 0; i < static_cast<int32>(specialFolderPaths.size()); ++i)
				{
					specialFolderPaths[i] = Unicode::FromUTF8(detail::MacOS_SpecialFolder(ToEnum<SpecialFolder>(i))) << U'/';
				}

				return specialFolderPaths;
			}();
			
			resourceFilePaths = [modulePath = modulePath]()
			{
				const FilePath resourcePath = (modulePath + U"/Contents/Resources/");

				Array<FilePath> paths = FileSystem::DirectoryContents(resourcePath, Recursive::Yes);

				paths.remove_if(FileSystem::IsDirectory);
				
				paths.remove(resourcePath + U"icon.icns");

				paths.sort();
				
				return paths;
			}();
		}
	
		FilePath FilePathCache::ParentPath(FilePath path, isize n)
		{
			if (path.count(U'/') <= n)
			{
				return{};
			}

			while (path)
			{
				if (path.back() == U'/')
				{
					if (n == 0)
					{
						break;
					}
					
					--n;
				}
				
				path.pop_back();
			}
			
			return path;
		}
	}

	namespace FileSystem
	{
		////////////////////////////////////////////////////////////////
		//
		//	IsResourcePath
		//
		////////////////////////////////////////////////////////////////

		bool IsResourcePath(const FilePathView path) noexcept
		{
			const FilePath resourceDirectory = (FileSystem::ModulePath() + U"/Contents/Resources/");
			return FullPath(path).starts_with(resourceDirectory);
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	Exists
		//
		////////////////////////////////////////////////////////////////

		bool Exists(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return false;
			}

			return detail::Exists(path);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IsDirectory
		//
		////////////////////////////////////////////////////////////////

		bool IsDirectory(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return false;
			}

			return detail::IsDirectory(path);
		}
	
		////////////////////////////////////////////////////////////////
		//
		//	IsFile
		//
		////////////////////////////////////////////////////////////////

		bool IsFile(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return false;
			}

			return detail::IsRegular(path);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IsResource
		//
		////////////////////////////////////////////////////////////////
	
		bool IsResource(const FilePathView path)
		{
			return (IsResourcePath(path) && detail::Exists(path));
		}

		////////////////////////////////////////////////////////////////
		//
		//	NativePath
		//
		////////////////////////////////////////////////////////////////

		NativeFilePath NativePath(const FilePathView path)
		{
			if (not path)
			{
				return{};
			}
			
			const FilePath fullpath = FullPath(path);
			
			return detail::MacOS_FullPath(Unicode::ToUTF8(fullpath).c_str());			
		}

		////////////////////////////////////////////////////////////////
		//
		//	VolumePath
		//
		////////////////////////////////////////////////////////////////

		FilePath VolumePath(const FilePathView)
		{
			return U"/";
		}

		////////////////////////////////////////////////////////////////
		//
		//	DirectoryContents
		//
		////////////////////////////////////////////////////////////////

		Array<FilePath> DirectoryContents(const FilePathView path, const Recursive recursive)
		{
			Array<FilePath> paths;
			
			if (path.isEmpty() || !IsDirectory(path))
			{
				return paths;
			}
			
			if (recursive)
			{
				for (const auto& v : std::filesystem::recursive_directory_iterator{ Unicode::ToUTF8(path) })
				{
					paths.push_back(FullPath(Unicode::FromUTF8(v.path().string())));
				}
			}
			else
			{
				for (const auto& v : std::filesystem::directory_iterator{ Unicode::ToUTF8(path) })
				{
					paths.push_back(FullPath(Unicode::FromUTF8(v.path().string())));
				}
			}
			
			return paths;
		}
	
		////////////////////////////////////////////////////////////////
		//
		//	InitialDirectory
		//
		////////////////////////////////////////////////////////////////
	
		const FilePath& InitialDirectory() noexcept
		{
			return detail::init::g_filePathCache.initialDirectory;
		}

		////////////////////////////////////////////////////////////////
		//
		//	ModulePath
		//
		////////////////////////////////////////////////////////////////

		const FilePath& ModulePath() noexcept
		{
			return detail::init::g_filePathCache.modulePath;
		}

		////////////////////////////////////////////////////////////////
		//
		//	ChangeCurrentDirectory
		//
		////////////////////////////////////////////////////////////////
	
		bool ChangeCurrentDirectory(const FilePathView path)
		{
			if (not IsDirectory(path))
			{
				return false;
			}
			
			return detail::MacOS_ChangeCurrentDirectory(Unicode::ToUTF8(path).c_str());
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetFolderPath
		//
		////////////////////////////////////////////////////////////////
	
		const FilePath& GetFolderPath(const SpecialFolder folder)
		{
			assert(FromEnum(folder) < static_cast<int32>(std::size(detail::init::g_filePathCache.specialFolderPaths)));

			return detail::init::g_filePathCache.specialFolderPaths[FromEnum(folder)];
		}

		////////////////////////////////////////////////////////////////
		//
		//	Remove
		//
		////////////////////////////////////////////////////////////////
	
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
			
			if (allowUndo)
			{
				return detail::MacOS_TrashFile(Unicode::ToUTF8(path), IsDirectory(path));
			}
			else
			{
				try
				{
					std::filesystem::remove_all(detail::ToPath(path));
					return true;
				}
				catch (const std::filesystem::filesystem_error&)
				{
					return false;
				}
			}
		}
	}
}
