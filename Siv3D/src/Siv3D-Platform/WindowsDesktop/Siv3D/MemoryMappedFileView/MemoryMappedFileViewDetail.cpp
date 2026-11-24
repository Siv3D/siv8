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

# include <Siv3D/FileSystem.hpp>
# include <Siv3D/FormatUtility.hpp>
# include <Siv3D/Resource.hpp>
# include <Siv3D/EngineLog.hpp>
# include "MemoryMappedFileViewDetail.hpp"

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

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	MemoryMappedFileView::MemoryMappedFileViewDetail::~MemoryMappedFileViewDetail()
	{
		close();
	}

	////////////////////////////////////////////////////////////////
	//
	//	open
	//
	////////////////////////////////////////////////////////////////

	bool MemoryMappedFileView::MemoryMappedFileViewDetail::open(FilePath path)
	{
		LOG_DEBUG(fmt::format("MemoryMappedFileView::MemoryMappedFileViewDetail::open(\"{0}\")", path.toUTF8()));

		close();

		if (path.isEmpty())
		{
			return false;
		}

		if (FileSystem::IsResourcePath(path))
		{
			HMODULE hModule = ::GetModuleHandleW(nullptr);

			if (not hModule)
			{
				LOG_FAIL("GetModuleHandleW() failed.");
				return false;
			}

			HRSRC hrs = ::FindResourceW(hModule, Platform::Windows::ToResourceName(path).c_str(), L"FILE");

			if (not hrs)
			{
				LOG_FAIL(fmt::format("❌ MemoryMappedFileView: Failed to open resource \"{0}\"", path.toUTF8()));
				return false;
			}

			HGLOBAL pResource = ::LoadResource(hModule, hrs);

			if (not pResource)
			{
				LOG_FAIL(fmt::format("❌ MemoryMappedFileView: Failed to load resource \"{0}\"", path.toUTF8()));
				return false;
			}

			const auto pointer = static_cast<const Byte*>(::LockResource(pResource));
			if (pointer == nullptr)
			{
				return false;
			}

			m_resource =
			{
				.pointer	= pointer,
			};

			m_info =
			{
				.fullPath	= std::move(path),
				.fileSize	= ::SizeofResource(hModule, hrs),
				.isOpen		= true,
			};

			LOG_INFO(fmt::format("📤 MemoryMappedFileView: Opened resource \"{0}\" size: {1}", m_info.fullPath, FormatDataSize(m_info.fileSize)));
		}
		else
		{
			const HANDLE fileHandle = ::CreateFileW(
				Unicode::ToWstring(path).c_str(), GENERIC_READ, (FILE_SHARE_READ | FILE_SHARE_WRITE),
				nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (fileHandle == INVALID_HANDLE_VALUE)
			{
				LOG_FAIL(fmt::format("❌ MemoryMappedFileView: Failed to open file `{0}`", path.toUTF8()));
				return false;
			}

			LARGE_INTEGER size{};
			if ((::GetFileType(fileHandle) != FILE_TYPE_DISK)
				|| (not ::GetFileSizeEx(fileHandle, &size)) || (size.QuadPart < 0))
			{
				::CloseHandle(fileHandle);
				return false;
			}
			const int64 fileSize = size.QuadPart;

			m_file =
			{
				.fileHandle		= fileHandle,
				.fileMapping	= nullptr,
				.baseAddress	= nullptr,
			};

			m_info =
			{
				.fullPath	= std::move(path),
				.fileSize	= fileSize,
				.isOpen		= true,
			};

			LOG_INFO(fmt::format("📤 MemoryMappedFileView: Opened file `{0}` size: {1}", m_info.fullPath, FormatDataSize(m_info.fileSize)));
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	close
	//
	////////////////////////////////////////////////////////////////

	void MemoryMappedFileView::MemoryMappedFileViewDetail::close()
	{
		if (not m_info.isOpen)
		{
			return;
		}

		unmap();

		if (isResource())
		{
			m_resource = {};

			LOG_INFO(fmt::format("📥 MemoryMappedFileView: Resource `{0}` closed", m_info.fullPath));
		}
		else
		{
			::CloseHandle(m_file.fileHandle);
			m_file = {};

			LOG_INFO(fmt::format("📥 MemoryMappedFileView: File `{0}` closed", m_info.fullPath));
		}

		m_info = {};
	}

	////////////////////////////////////////////////////////////////
	//
	//	isOpen
	//
	////////////////////////////////////////////////////////////////

	bool MemoryMappedFileView::MemoryMappedFileViewDetail::isOpen() const
	{
		return m_info.isOpen;
	}

	////////////////////////////////////////////////////////////////
	//
	//	map
	//
	////////////////////////////////////////////////////////////////

	MappedMemoryView MemoryMappedFileView::MemoryMappedFileViewDetail::map(const size_t offset, const size_t requestSize)
	{
		if (not m_info.isOpen)
		{
			return{};
		}

		// すでにファイルがマップされている場合は何もしない
		if (m_file.fileMapping || m_resource.isMapped)
		{
			return{};
		}

		// ファイルサイズよりも大きいオフセットが指定された場合は失敗
		if (static_cast<uint64>(m_info.fileSize) <= offset)
		{
			return{};
		}

		const size_t mapSize = Min(requestSize, (static_cast<size_t>(m_info.fileSize) - offset));

		if (mapSize == 0)
		{
			return{};
		}

		if (isResource())
		{
			m_resource.isMapped = true;
			return{ .data = (m_resource.pointer + offset), .size = mapSize };
		}
		else
		{
			const size_t internalOffset = (offset / g_granularity * g_granularity);

			const HANDLE fileMapping = ::CreateFileMappingW(
				m_file.fileHandle, 0, PAGE_READONLY,
				static_cast<DWORD>(static_cast<uint64>(offset + mapSize) >> 32), static_cast<DWORD>(offset + mapSize), nullptr);

			if (fileMapping == nullptr)
			{
				LOG_FAIL(fmt::format("❌ MemoryMappedFileView: CreateFileMappingW() failed. offset: {0}, size: {1}", offset, mapSize));
				return{};
			}

			m_file.baseAddress = static_cast<const Byte*>(::MapViewOfFile(
				fileMapping, FILE_MAP_READ, static_cast<DWORD>(static_cast<uint64>(internalOffset) >> 32),
				static_cast<DWORD>(internalOffset), (offset - internalOffset + mapSize)));

			if (m_file.baseAddress == nullptr)
			{
				::CloseHandle(fileMapping);
				LOG_FAIL(fmt::format("❌ MemoryMappedFileView: MapViewOfFile() failed. offset: {0}, size: {1}", offset, mapSize));
				return{};
			}

			m_file.fileMapping = fileMapping;

			return{ .data = (m_file.baseAddress + (offset - internalOffset)), .size = mapSize };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	unmap
	//
	////////////////////////////////////////////////////////////////

	void MemoryMappedFileView::MemoryMappedFileViewDetail::unmap()
	{
		m_resource.isMapped = false;

		// ファイルがマップされていなければ何もしない
		if (not m_file.fileMapping)
		{
			return;
		}

		if (not ::UnmapViewOfFile(m_file.baseAddress))
		{
			LOG_FAIL("❌ MemoryMappedFileView: UnmapViewOfFile() failed.");
		}
		m_file.baseAddress = nullptr;

		if (not ::CloseHandle(m_file.fileMapping))
		{
			LOG_FAIL("❌ MemoryMappedFileView: CloseHandle() failed.");
		}
		m_file.fileMapping = nullptr;
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	int64 MemoryMappedFileView::MemoryMappedFileViewDetail::size() const
	{
		return m_info.fileSize;
	}

	////////////////////////////////////////////////////////////////
	//
	//	path
	//
	////////////////////////////////////////////////////////////////

	const FilePath& MemoryMappedFileView::MemoryMappedFileViewDetail::path() const
	{
		return m_info.fullPath;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	bool MemoryMappedFileView::MemoryMappedFileViewDetail::isResource() const noexcept
	{
		return (m_resource.pointer != nullptr);
	}
}
