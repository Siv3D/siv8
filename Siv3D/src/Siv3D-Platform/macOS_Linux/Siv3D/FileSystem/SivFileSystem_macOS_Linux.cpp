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
# include <filesystem>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Unicode.hpp>

namespace s3d
{
	namespace detail
	{
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
	
		FilePath CurrentDirectory()
		{
			FilePath currentDirectory = Unicode::FromUTF8(std::filesystem::current_path().string());
			
			if (not currentDirectory.ends_with(U'/'))
			{
				currentDirectory.push_back(U'/');
			}
			
			return currentDirectory;
		}
	
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
	}
}
