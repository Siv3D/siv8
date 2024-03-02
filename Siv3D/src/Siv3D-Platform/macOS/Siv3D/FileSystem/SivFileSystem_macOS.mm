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
# include <Foundation/Foundation.h>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/SpecialFolder.hpp>

namespace s3d
{
	namespace detail
	{
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
					NSLibraryDirectory, // (dummy)
					NSLibraryDirectory, // (dummy)
					NSLibraryDirectory, // (dummy)
					NSLibraryDirectory, // (dummy)
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
	
		namespace init
		{
			[[nodiscard]]
			static FilePath ParentPath(FilePath path, isize n)
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
		
			const static FilePath g_initialPath = []() -> FilePath
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

			const static FilePath g_modulePath = []() -> FilePath
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
		
			const static std::array<FilePath, 12> g_specialFolderPaths = []()
			{
				std::array<FilePath, 12> specialFolderPaths;

				for (int32 i = 0; i < static_cast<int32>(specialFolderPaths.size()); ++i)
				{
					specialFolderPaths[i] = Unicode::FromUTF8(detail::MacOS_SpecialFolder(ToEnum<SpecialFolder>(i))) << U'/';
				}

				return specialFolderPaths;
			}();
		}
	}

	namespace FileSystem
	{
		bool IsResourcePath(const FilePathView path) noexcept
		{
			const FilePath resourceDirectory = (FileSystem::ModulePath() + U"/Contents/Resources/");
			return FullPath(path).starts_with(resourceDirectory);
		}
	
		bool Exists(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return false;
			}

			return detail::Exists(path);
		}
	
		bool IsDirectory(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return false;
			}

			return detail::IsDirectory(path);
		}
	
		bool IsFile(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return false;
			}

			return detail::IsRegular(path);
		}
	
		bool IsResource(const FilePathView path)
		{
			return (IsResourcePath(path) && detail::Exists(path));
		}
	

	
	
	
	
		const FilePath& InitialDirectory() noexcept
		{
			return detail::init::g_initialPath;
		}

		const FilePath& ModulePath() noexcept
		{
			return detail::init::g_modulePath;
		}
	
		bool ChangeCurrentDirectory(const FilePathView path)
		{
			if (not IsDirectory(path))
			{
				return false;
			}
			
			return detail::MacOS_ChangeCurrentDirectory(Unicode::ToUTF8(path).c_str());
		}
	
		const FilePath& GetFolderPath(const SpecialFolder folder)
		{
			assert(FromEnum(folder) < static_cast<int32>(std::size(detail::init::g_specialFolderPaths)));

			return detail::init::g_specialFolderPaths[FromEnum(folder)];
		}
	}
}
