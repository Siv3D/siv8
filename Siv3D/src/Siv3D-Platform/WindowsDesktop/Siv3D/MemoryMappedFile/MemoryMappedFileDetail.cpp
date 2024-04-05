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

# include <Siv3D/FileSystem.hpp>
# include <Siv3D/FormatUtility.hpp>
# include <Siv3D/EngineLog.hpp>
# include "MemoryMappedFileDetail.hpp"

namespace s3d
{
	namespace
	{
		static const size_t g_granularity = []()
			{
				SYSTEM_INFO systemInfo{};
				::GetSystemInfo(&systemInfo);
				return systemInfo.dwAllocationGranularity;
			}();
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

		int32 openMode;

		switch (ifExists)
		{
		case OpenMode_if_Exists::JustOpen:
			{
				openMode = ((ifNotFound == OpenMode_if_NotFound::Create) ? OPEN_ALWAYS : OPEN_EXISTING);
				break;
			}
		case OpenMode_if_Exists::Truncate:
			{
				openMode = ((ifNotFound == OpenMode_if_NotFound::Create) ? CREATE_ALWAYS : TRUNCATE_EXISTING);
				break;
			}
		default: // OpenMode_if_Exists::Fail
			{
				if (ifNotFound == OpenMode_if_NotFound::Create)
				{
					openMode = CREATE_NEW;
				}
				else
				{
					return false;
				}
			}
		}

		const HANDLE fileHandle = ::CreateFileW(
			Unicode::ToWstring(path).c_str(), (GENERIC_READ | GENERIC_WRITE), (FILE_SHARE_READ | FILE_SHARE_WRITE),
			nullptr, openMode, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (fileHandle == INVALID_HANDLE_VALUE)
		{
			LOG_FAIL(fmt::format("❌ MemoryMappedFile: Failed to open file `{0}`", path));
			return false;
		}

		LARGE_INTEGER size{};
		::GetFileSizeEx(fileHandle, &size);
		const int64 fileSize = size.QuadPart;

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

		LOG_INFO(fmt::format("📤 MemoryMappedFile: Opened file `{0}` size: {1}", m_info.fullPath, FormatDataSize(m_info.fileSize)));

		return true;
	}

	void MemoryMappedFile::MemoryMappedFileDetail::close()
	{
		if (not m_info.isOpen)
		{
			return;
		}

		unmap();

		::CloseHandle(m_file.fileHandle);
		m_file = {};

		LOG_INFO(fmt::format("📥 MemoryMappedFile: File `{0}` closed", m_info.fullPath));

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
		if (m_file.fileMapping)
		{
			return{};
		}

		if (m_info.fileSize < static_cast<int64>(offset))
		{
			return{};
		}

		const size_t mapSize = (requestSize ? requestSize : Min(requestSize, static_cast<size_t>(m_info.fileSize - offset)));

		if (mapSize == 0)
		{
			return{};
		}

		const size_t internalOffset = (offset / g_granularity * g_granularity);

		m_file.fileMapping = ::CreateFileMappingW(
			m_file.fileHandle, 0, PAGE_READWRITE,
			(static_cast<uint64>(offset + mapSize) >> 32), ((offset + mapSize) & 0xffFFffFF), nullptr);

		if (m_file.fileMapping == nullptr)
		{
			LOG_FAIL(fmt::format("❌ MemoryMappedFile: CreateFileMappingW() failed. offset: {0}, size: {1}", offset, mapSize));
			return{};
		}

		m_file.baseAddress = static_cast<Byte*>(::MapViewOfFile(
			m_file.fileMapping, FILE_MAP_WRITE, (static_cast<uint64>(internalOffset) >> 32),
			(internalOffset & 0xffFFffFF), (offset - internalOffset + mapSize)));

		if (m_file.baseAddress == nullptr)
		{
			LOG_FAIL(fmt::format("❌ MemoryMappedFile: MapViewOfFile() failed. offset: {0}, size: {1}", offset, mapSize));
			return{};
		}

		if (static_cast<size_t>(m_info.fileSize) < (offset + mapSize))
		{
			m_info.fileSize = (offset + mapSize);
		}

		return{ .data = (m_file.baseAddress + (offset - internalOffset)), .size = mapSize };
	}

	void MemoryMappedFile::MemoryMappedFileDetail::unmap()
	{
		// ファイルがマップされていなければ何もしない
		if (not m_file.fileMapping)
		{
			return;
		}

		if (not ::UnmapViewOfFile(m_file.baseAddress))
		{
			LOG_FAIL("❌ MemoryMappedFile: UnmapViewOfFile() failed.");
		}
		m_file.baseAddress = nullptr;

		if (not ::CloseHandle(m_file.fileMapping))
		{
			LOG_FAIL("❌ MemoryMappedFile: CloseHandle() failed.");
		}
		m_file.fileMapping = nullptr;
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
