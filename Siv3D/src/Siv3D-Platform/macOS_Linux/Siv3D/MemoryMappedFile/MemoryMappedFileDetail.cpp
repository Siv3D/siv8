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

# include <unistd.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/FormatUtility.hpp>
# include <Siv3D/EngineLog.hpp>
# include "MemoryMappedFileDetail.hpp"

namespace s3d
{
	namespace detail
	{
		static const size_t g_granularity = ::sysconf(_SC_PAGE_SIZE);
	}

	MemoryMappedFile::MemoryMappedFileDetail::~MemoryMappedFileDetail()
	{
		close();
	}

	bool MemoryMappedFile::MemoryMappedFileDetail::open(const FilePathView path, const OpenMode_if_Exists ifExists, const OpenMode_if_NotFound ifNotFound)
	{
		LOG_DEBUG(fmt::format("MemoryMappedFile::MemoryMappedFileDetail::open(\"{0}\")", path));

		close();
		
		if (FileSystem::IsResourcePath(path))
		{
			LOG_FAIL(fmt::format("❌ MemoryMappedFile: Resource `{0}` cannot be opened as writable", path));
			return false;
		}

		{
			int openMode = O_RDWR;

			switch (ifExists)
			{
			case OpenMode_if_Exists::JustOpen:
				{
					openMode |= ((ifNotFound == OpenMode_if_NotFound::Create) ? O_CREAT : 0);
					break;
				}
			case OpenMode_if_Exists::Truncate:
				{
					openMode |= ((ifNotFound == OpenMode_if_NotFound::Create) ? (O_TRUNC | O_CREAT) : O_TRUNC);
					break;
				}
			default:
				{
					if (ifNotFound == OpenMode_if_NotFound::Create)
					{
						openMode |= (O_EXCL | O_CREAT);
					}
					else
					{
						return false;
					}
				}
			}

			const int32 fileHandle = ::open(Unicode::ToUTF8(path).c_str(), openMode, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));

			if (fileHandle == -1)
			{
				LOG_FAIL(fmt::format("❌ MemoryMappedFile: Failed to open file `{0}`", path));
				return false;
			}

			struct stat s;
			::fstat(fileHandle, &s);
			const int64 fileSize = s.st_size;

			m_file =
			{
				.fileHandle = fileHandle,
				.baseAddress = nullptr,
				.mapLength = 0,
			};

			m_info =
			{
				.fullPath = FileSystem::FullPath(path),
				.fileSize = fileSize,
				.isOpen = true,
			};

			LOG_INFO(fmt::format("📤 MemoryMappedFile: Opened file `{0}` size: {1}", m_info.fullPath, FormatDataSize(m_info.fileSize)));
		}

		return true;
	}

	void MemoryMappedFile::MemoryMappedFileDetail::close()
	{
		if (not m_info.isOpen)
		{
			return;
		}

		unmap();

		{
			::close(m_file.fileHandle);
			m_file = {};

			LOG_INFO(fmt::format("📥 MemoryMappedFile: File `{0}` closed", m_info.fullPath));
		}

		m_info = {};
	}

	bool MemoryMappedFile::MemoryMappedFileDetail::isOpen() const
	{
		return m_info.isOpen;
	}

	MappedMemory MemoryMappedFile::MemoryMappedFileDetail::map(const size_t offset, const size_t requestSize)
	{
		if (not m_info.isOpen)
		{
			return{};
		}
		
		// すでにファイルがマップされている場合は何もしない
		if (m_file.baseAddress)
		{
			return{};
		}

		// ファイルサイズよりも大きいオフセットが指定された場合は失敗
		if (m_info.fileSize < static_cast<int64>(offset))
		{
			return{};
		}

		const size_t mapSize = requestSize;
		
		if (mapSize == 0)
		{
			return{};
		}
		
		if (static_cast<size_t>(m_info.fileSize) < (offset + mapSize))
		{
			if (-1 == ::ftruncate(m_file.fileHandle, (offset + mapSize)))
			{
				LOG_FAIL(fmt::format("❌ MemoryMappedFile: ftruncate() failed. offset: {0}, size: {1}", offset, mapSize));
				return{};
			}

			m_info.fileSize = (offset + mapSize);
		}

		const size_t internalOffset = (offset / detail::g_granularity * detail::g_granularity);

		void* baseAddress = ::mmap(0, (offset - internalOffset + mapSize), (PROT_READ | PROT_WRITE), MAP_SHARED, m_file.fileHandle, internalOffset);

		if (baseAddress == MAP_FAILED)
		{
			LOG_FAIL(fmt::format("❌ MemoryMappedFile: mmap() failed. offset: {0}, size: {1}", offset, mapSize));
			return{};
		}

		m_file.baseAddress = static_cast<Byte*>(baseAddress);
		m_file.mapLength = (offset - internalOffset + mapSize);

		return{ .data = (m_file.baseAddress + (offset - internalOffset)), .size = mapSize };
	}

	void MemoryMappedFile::MemoryMappedFileDetail::unmap()
	{
		// ファイルがマップされていなければ何もしない
		if (m_file.baseAddress == nullptr)
		{
			return;
		}

		if (::munmap(static_cast<void*>(m_file.baseAddress), m_file.mapLength) == -1)
		{
			LOG_FAIL("❌ MemoryMappedFile: munmap() failed.");
		}
		m_file.baseAddress = nullptr;
		m_file.mapLength = 0;
	}

	bool MemoryMappedFile::MemoryMappedFileDetail::flush() const
	{
		// ファイルがマップされていなければ何もしない
		if (m_file.baseAddress == nullptr)
		{
			return true;
		}

		return (::msync(m_file.baseAddress, m_file.mapLength, MS_SYNC) == 0);
	}

	int64 MemoryMappedFile::MemoryMappedFileDetail::size() const
	{
		return m_info.fileSize;
	}

	const FilePath& MemoryMappedFile::MemoryMappedFileDetail::path() const
	{
		return m_info.fullPath;
	}
}
