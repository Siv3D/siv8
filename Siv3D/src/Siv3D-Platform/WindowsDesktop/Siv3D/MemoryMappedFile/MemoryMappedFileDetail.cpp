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
	//namespace
	//{
	//	static const size_t g_granularity = []()
	//		{
	//			SYSTEM_INFO systemInfo{};
	//			::GetSystemInfo(&systemInfo);
	//			return systemInfo.dwAllocationGranularity;
	//		}();
	//}

	//MemoryMappedFileView::MemoryMappedFileViewDetail::~MemoryMappedFileViewDetail()
	//{
	//	close();
	//}

	//bool MemoryMappedFileView::MemoryMappedFileViewDetail::open(const FilePathView path)
	//{
	//	LOG_DEBUG(fmt::format("MemoryMappedFileView::MemoryMappedFileViewDetail::open(\"{0}\")", path));

	//	close();

	//	if (FileSystem::IsResourcePath(path))
	//	{
	//		HMODULE hModule = ::GetModuleHandleW(nullptr);

	//		if (not hModule)
	//		{
	//			LOG_FAIL("GetModuleHandleW() failed.");
	//			return false;
	//		}

	//		HRSRC hrs = ::FindResourceW(hModule, path.substr(1).toWstr().c_str(), L"FILE");

	//		if (not hrs)
	//		{
	//			LOG_FAIL(fmt::format("❌ MemoryMappedFileView: Failed to open resource \"{0}\"", path));
	//			return false;
	//		}

	//		HGLOBAL pResource = ::LoadResource(hModule, hrs);

	//		if (not pResource)
	//		{
	//			LOG_FAIL(fmt::format("❌ MemoryMappedFileView: Failed to load resource \"{0}\"", path));
	//			return false;
	//		}

	//		m_resource =
	//		{
	//			.pointer	= static_cast<const Byte*>(::LockResource(pResource)),
	//		};

	//		m_info =
	//		{
	//			.fullPath	= FilePath{ path },
	//			.fileSize	= ::SizeofResource(hModule, hrs),
	//			.isOpen		= true,
	//		};

	//		LOG_INFO(fmt::format("📤 MemoryMappedFileView: Opened resource \"{0}\" size: {1}", m_info.fullPath, FormatDataSize(m_info.fileSize)));
	//	}
	//	else
	//	{
	//		const HANDLE fileHandle = ::CreateFileW(
	//			Unicode::ToWstring(path).c_str(), GENERIC_READ, (FILE_SHARE_READ | FILE_SHARE_WRITE),
	//			nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	//		if (fileHandle == INVALID_HANDLE_VALUE)
	//		{
	//			LOG_FAIL(fmt::format("❌ MemoryMappedFileView: Failed to open file `{0}`", path));
	//			return false;
	//		}

	//		LARGE_INTEGER size{};
	//		::GetFileSizeEx(fileHandle, &size);
	//		const int64 fileSize = size.QuadPart;

	//		m_file =
	//		{
	//			.fileHandle		= fileHandle,
	//			.fileMapping	= nullptr,
	//			.baseAddress	= nullptr,
	//		};

	//		m_info =
	//		{
	//			.fullPath	= FileSystem::FullPath(path),
	//			.fileSize	= fileSize,
	//			.isOpen		= true,
	//		};

	//		LOG_INFO(fmt::format("📤 MemoryMappedFileView: Opened file `{0}` size: {1}", m_info.fullPath, FormatDataSize(m_info.fileSize)));
	//	}

	//	return true;
	//}

	//void MemoryMappedFileView::MemoryMappedFileViewDetail::close()
	//{
	//	if (not m_info.isOpen)
	//	{
	//		return;
	//	}

	//	unmap();

	//	if (isResource())
	//	{
	//		m_resource = {};

	//		LOG_INFO(fmt::format("📥 MemoryMappedFileView: Resource `{0}` closed", m_info.fullPath));
	//	}
	//	else
	//	{
	//		::CloseHandle(m_file.fileHandle);
	//		m_file = {};

	//		LOG_INFO(fmt::format("📥 MemoryMappedFileView: File `{0}` closed", m_info.fullPath));
	//	}

	//	m_info = {};
	//}

	//bool MemoryMappedFileView::MemoryMappedFileViewDetail::isOpen() const
	//{
	//	return m_info.isOpen;
	//}

	//MappedMemory MemoryMappedFileView::MemoryMappedFileViewDetail::map(const size_t offset, const size_t requestSize)
	//{
	//	if (not m_info.isOpen)
	//	{
	//		return{};
	//	}

	//	// すでにファイルがマップされている場合は何もしない
	//	if (m_file.fileMapping)
	//	{
	//		return{};
	//	}

	//	if (m_info.fileSize <= static_cast<int64>(offset))
	//	{
	//		return{};
	//	}

	//	const size_t mapSize = Min(requestSize, static_cast<size_t>(m_info.fileSize - offset));

	//	if (mapSize == 0)
	//	{
	//		return{};
	//	}

	//	if (isResource())
	//	{
	//		return{ .data = (m_resource.pointer + offset), .size = mapSize };
	//	}
	//	else
	//	{
	//		const size_t internalOffset = (offset / g_granularity * g_granularity);

	//		m_file.fileMapping = ::CreateFileMappingW(
	//			m_file.fileHandle, 0, PAGE_READONLY,
	//			(static_cast<uint64>(offset + mapSize) >> 32), ((offset + mapSize) & 0xffFFffFF), nullptr);

	//		if (m_file.fileMapping == NULL)
	//		{
	//			LOG_FAIL(fmt::format("❌ MemoryMappedFileView: CreateFileMappingW() failed. offset: {0}, size: {1}", offset, mapSize));
	//			return{};
	//		}

	//		m_file.baseAddress = static_cast<const Byte*>(::MapViewOfFile(
	//			m_file.fileMapping, FILE_MAP_READ, (static_cast<uint64>(internalOffset) >> 32),
	//			(internalOffset & 0xffFFffFF), (offset - internalOffset + mapSize)));

	//		if (m_file.baseAddress == nullptr)
	//		{
	//			LOG_FAIL(fmt::format("❌ MemoryMappedFileView: MapViewOfFile() failed. offset: {0}, size: {1}", offset, mapSize));
	//			return{};
	//		}

	//		return{ .data = (m_file.baseAddress + (offset - internalOffset)), .size = mapSize };
	//	}
	//}

	//void MemoryMappedFileView::MemoryMappedFileViewDetail::unmap()
	//{
	//	// ファイルがマップされていなければ何もしない
	//	if (not m_file.fileMapping)
	//	{
	//		return;
	//	}

	//	if (not ::UnmapViewOfFile(m_file.baseAddress))
	//	{
	//		LOG_FAIL("❌ MemoryMappedFileView: UnmapViewOfFile() failed.");
	//	}
	//	m_file.baseAddress = nullptr;

	//	if (not ::CloseHandle(m_file.fileMapping))
	//	{
	//		LOG_FAIL("❌ MemoryMappedFileView: CloseHandle() failed.");
	//	}
	//	m_file.fileMapping = nullptr;
	//}

	//int64 MemoryMappedFileView::MemoryMappedFileViewDetail::size() const
	//{
	//	return m_info.fileSize;
	//}

	//const FilePath& MemoryMappedFileView::MemoryMappedFileViewDetail::path() const
	//{
	//	return m_info.fullPath;
	//}

	//bool MemoryMappedFileView::MemoryMappedFileViewDetail::isResource() const noexcept
	//{
	//	return (m_resource.pointer != nullptr);
	//}
}
