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

# include <unistd.h>
# include <mach-o/dyld.h>
# include <filesystem>
# include <Foundation/Foundation.h>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Unicode.hpp>

namespace s3d
{
	namespace detail
	{
		[[nodiscard]]
		inline static std::filesystem::path ToPath(const FilePathView path)
		{
			return std::filesystem::path{ Unicode::ToUTF8(path) };
		}
	
		[[nodiscard]]
		inline static std::filesystem::file_status GetStatus(const FilePathView path)
		{
			return std::filesystem::status(ToPath(path));
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

			return (detail::GetStatus(path).type() != std::filesystem::file_type::not_found);
		}
	
		bool IsDirectory(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return false;
			}

			return (detail::GetStatus(path).type() == std::filesystem::file_type::directory);
		}
	
		bool IsFile(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return false;
			}

			return (detail::GetStatus(path).type() == std::filesystem::file_type::regular);
		}
	
		bool IsResource(const FilePathView path)
		{
			return (IsResourcePath(path)
					&& (detail::GetStatus(path).type() != std::filesystem::file_type::not_found));
		}
	
		FilePath FullPath(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return{};
			}
			
			FilePath fullpath = Unicode::FromUTF8(std::filesystem::weakly_canonical(detail::ToPath(path)).string());
			
			//if (IsDirectory(fullpath) && (not fullpath.ends_with(U'/')))
			{
				fullpath.push_back(U'/');
			}
			
			return fullpath;
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
	}
}
