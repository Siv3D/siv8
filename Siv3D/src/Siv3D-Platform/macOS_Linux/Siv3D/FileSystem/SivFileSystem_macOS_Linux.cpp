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

# include <sys/stat.h>
# include <filesystem>
# include <limits>
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
		static Optional<DateTime> ToDateTime(const ::timespec& tv)
		{
			::tm lt;
			if ((not ::localtime_r(&tv.tv_sec, &lt))
				|| (lt.tm_year > (std::numeric_limits<int32>::max() - 1900)))
			{
				return none;
			}
			return DateTime{ (1900 + lt.tm_year),
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

			std::error_code error;
			const std::filesystem::path nativeFullPath = std::filesystem::weakly_canonical(detail::ToPath(path), error);
			if (error)
			{
				return{};
			}

			FilePath fullpath = Unicode::FromUTF8(nativeFullPath.native());
			
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
			std::error_code error;
			const std::filesystem::path nativeCurrentDirectory = std::filesystem::current_path(error);
			if (error)
			{
				return{};
			}

			FilePath currentDirectory = Unicode::FromUTF8(nativeCurrentDirectory.native());
			
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

		uint64 Size(const FilePathView path)
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
				uint64 result = 0;
				std::error_code error;
				std::filesystem::recursive_directory_iterator it{ detail::ToPath(fullPath), error };
				if (error)
				{
					return 0;
				}

				const std::filesystem::recursive_directory_iterator end;
				while (it != end)
				{
					struct stat entryStat;
					if (::stat(it->path().c_str(), &entryStat) != 0)
					{
						return 0;
					}
					if (not S_ISDIR(entryStat.st_mode))
					{
						result += entryStat.st_size;
					}

					it.increment(error);
					if (error)
					{
						return 0;
					}
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
	
		uint64 FileSize(const FilePathView path)
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
	
		bool RemoveContents(const FilePathView path, const MoveToTrash moveToTrash)
		{
			if (path.isEmpty() || IsResourcePath(path))
			{
				return false;
			}

			std::error_code error;
			std::filesystem::directory_iterator it{ detail::ToPath(path), error };
			if (error)
			{
				return false;
			}

			const std::filesystem::directory_iterator end;
			while (it != end)
			{
				if (moveToTrash)
				{
					if (not Remove(Unicode::FromUTF8(it->path().native()), MoveToTrash::Yes))
					{
						return false;
					}
				}
				else
				{
					std::filesystem::remove_all(it->path(), error);
					if (error)
					{
						return false;
					}
				}

				it.increment(error);
				if (error)
				{
					return false;
				}
			}

			return true;
		}
	}
}
