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
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/FormatUtility.hpp>
# include <Siv3D/EngineLog.hpp>
# include "MemoryMappedFileViewDetail.hpp"

namespace s3d
{
	namespace detail
	{
		static const size_t g_granularity = ::sysconf(_SC_PAGE_SIZE);
	}

	MemoryMappedFileView::MemoryMappedFileViewDetail::~MemoryMappedFileViewDetail()
	{
		close();
	}

	bool MemoryMappedFileView::MemoryMappedFileViewDetail::open(const FilePathView path)
	{
		LOG_DEBUG(fmt::format("MemoryMappedFileView::MemoryMappedFileViewDetail::open(\"{0}\")", path));

		close();

		{
			const int32 fileHandle = ::open(Unicode::ToUTF8(path).c_str(), O_RDONLY);

			if (fileHandle == -1)
			{
				LOG_FAIL(fmt::format("❌ MemoryMappedFileView: Failed to open file `{0}`", path));
				return false;
			}

			struct stat s;
			::fstat(fileHandle, &s);
			const int64 fileSize = s.st_size;

			m_file =
			{
				.fileHandle		= fileHandle,
				.fileMapping	= nullptr,
				.baseAddress	= nullptr,
			};

			m_info =
			{
				.fullPath	= FileSystem::FullPath(path),
				.fileSize	= fileSize,
				.isOpen		= true,
			};

			LOG_INFO(fmt::format("📤 MemoryMappedFileView: Opened file `{0}` size: {1}", m_info.fullPath, FormatDataSize(m_info.fileSize)));
		}

		return true;
	}

	void MemoryMappedFileView::MemoryMappedFileViewDetail::close()
	{
		if (not m_info.isOpen)
		{
			return;
		}

		unmap();

		{
			::close(m_fileHandle);
			m_file = {};

			LOG_INFO(fmt::format("📥 MemoryMappedFileView: File `{0}` closed", m_info.fullPath));
		}

		m_info = {};
	}

	bool MemoryMappedFileView::MemoryMappedFileViewDetail::isOpen() const
	{
		return m_info.isOpen;
	}

	MappedMemory MemoryMappedFileView::MemoryMappedFileViewDetail::map(const size_t offset, const size_t requestSize)
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

		if (m_info.fileSize <= offset)
		{
			return{};
		}

		{
			const size_t mapSize = Min(requestSize, static_cast<size_t>(m_info.fileSize - offset));
			const size_t internalOffset = (offset / detail::g_granularity * detail::g_granularity);

			const void* baseAddress = ::mmap(0, (offset - internalOffset + mapSize), PROT_READ, MAP_SHARED, m_fileHandle, internalOffset);

			if (baseAddress == MAP_FAILED)
			{
				LOG_FAIL(fmt::format("❌ MemoryMappedFileView: mmap() failed. offset: {0}, size: {1}", offset, mapSize));
				return{};
			}

			m_file.baseAddress = static_cast<const Byte*>(baseAddress);
			m_file.mapLength = (offset - internalOffset + mapSize);

			return{ .data = (m_file.baseAddress + (offset - internalOffset)), .size = mapSize };
		}
	}

	void MemoryMappedFileView::MemoryMappedFileViewDetail::unmap()
	{
		// ファイルがマップされていなければ何もしない
		if (m_file.baseAddress == nullptr)
		{
			return;
		}

		if (::munmap(static_cast<void*>(m_file.baseAddress), m_file.mapLength) == -1)
		{
			LOG_FAIL("❌ MemoryMappedFileView: munmap() failed.");
		}
		m_file.baseAddress = nullptr;
		m_file.mapLength = 0;
	}

	int64 MemoryMappedFileView::MemoryMappedFileViewDetail::size() const
	{
		return m_info.fileSize;
	}

	const FilePath& MemoryMappedFileView::MemoryMappedFileViewDetail::path() const
	{
		return m_info.fullPath;
	}

	bool MemoryMappedFileView::MemoryMappedFileViewDetail::isResource() const noexcept
	{
		return (m_resource.pointer != nullptr);
	}
}
