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

# include <sys/stat.h>
# include <filesystem>
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
		static bool GetStat(const FilePathView _path, struct stat& s)
		{
			const std::string path = Unicode::ToUTF8(FilePath{ _path }.replaced(U'\\', U'/'));
			return (::stat(path.c_str(), &s) == 0);
		}
	
		[[nodiscard]]
		static DateTime ToDateTime(const ::timespec& tv)
		{
			::tm lt;
			::localtime_r(&tv.tv_sec, &lt);
			return{ (1900 + lt.tm_year),
					(1 + lt.tm_mon),
					(lt.tm_mday),
					lt.tm_hour,
					lt.tm_min,
					lt.tm_sec,
					static_cast<int32>(tv.tv_nsec / (1'000'000))};
		}
	}

	namespace FileSystem
	{			
		////////////////////////////////////////////////////////////////
		//
		//	FullPath
		//
		////////////////////////////////////////////////////////////////

		FilePath FullPath(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return{};
			}

			FilePath fullpath = Unicode::FromUTF8(std::filesystem::weakly_canonical(detail::ToPath(path)).string());
			
			if (IsDirectory(fullpath) && (not fullpath.ends_with(U'/')))
			{
				fullpath.push_back(U'/');
			}
			
			return fullpath;
		}

		////////////////////////////////////////////////////////////////
		//
		//	CreationTime
		//
		////////////////////////////////////////////////////////////////

		Optional<DateTime> CreationTime(const FilePathView path)
		{
			struct stat s;
			if (not detail::GetStat(path, s))
			{
				return none;
			}
			
		# if SIV3D_PLATFORM(MACOS)
			return detail::ToDateTime(s.st_birthtimespec);
		# else
			return detail::ToDateTime(s.st_ctim);
		# endif
		}

		////////////////////////////////////////////////////////////////
		//
		//	WriteTime
		//
		////////////////////////////////////////////////////////////////

		Optional<DateTime> WriteTime(const FilePathView path)
		{
			struct stat s;
			if (not detail::GetStat(path, s))
			{
				return none;
			}
			
		# if SIV3D_PLATFORM(MACOS)
			return detail::ToDateTime(s.st_mtimespec);
		# else
			return detail::ToDateTime(s.st_mtim);
		# endif
		}

		////////////////////////////////////////////////////////////////
		//
		//	AccessTime
		//
		////////////////////////////////////////////////////////////////

		Optional<DateTime> AccessTime(const FilePathView path)
		{
			struct stat s;
			if (not detail::GetStat(path, s))
			{
				return none;
			}
			
		# if SIV3D_PLATFORM(MACOS)
			return detail::ToDateTime(s.st_atimespec);
		# else
			return detail::ToDateTime(s.st_atim);
		# endif
		}

		////////////////////////////////////////////////////////////////
		//
		//	CurrentDirectory
		//
		////////////////////////////////////////////////////////////////
	
		FilePath CurrentDirectory()
		{
			FilePath currentDirectory = Unicode::FromUTF8(std::filesystem::current_path().string());
			
			if (not currentDirectory.ends_with(U'/'))
			{
				currentDirectory.push_back(U'/');
			}
			
			return currentDirectory;
		}
	
		////////////////////////////////////////////////////////////////
		//
		//	Size
		//
		////////////////////////////////////////////////////////////////

		size_t Size(const FilePathView path)
		{
			if (not path)
			{
				return 0;
			}

			const FilePath fullPath = FullPath(path);
			
			struct stat s;
			if (not detail::GetStat(fullPath, s))
			{
				return 0;
			}

			if (S_ISREG(s.st_mode))
			{
				return s.st_size;
			}
			else if (S_ISDIR(s.st_mode))
			{
				size_t result = 0;

				for (const auto& v : std::filesystem::recursive_directory_iterator(Unicode::ToUTF8(path)))
				{
					struct stat s;
					
					if ((::stat(v.path().c_str(), &s) != 0) || S_ISDIR(s.st_mode))
					{
						continue;
					}

					result += s.st_size;
				}
				
				return result;
			}
			else
			{
				return 0;
			}
		}
	
		////////////////////////////////////////////////////////////////
		//
		//	FileSize
		//
		////////////////////////////////////////////////////////////////
	
		size_t FileSize(const FilePathView path)
		{
			if (path.isEmpty())
			{
				return 0;
			}
			
			struct stat s;
			if (not detail::GetStat(path, s))
			{
				return 0;
			}
			
			if (not S_ISREG(s.st_mode))
			{
				return 0;
			}
			
			return s.st_size;
		}
	
		////////////////////////////////////////////////////////////////
		//
		//	RemoveContents
		//
		////////////////////////////////////////////////////////////////
	
		bool RemoveContents(const FilePathView path, const AllowUndo allowUndo)
		{
			if (not IsDirectory(path))
			{
				return false;
			}

			if (not Remove(path, allowUndo))
			{
				return false;
			}

			return CreateDirectories(path);
		}
	}
}
