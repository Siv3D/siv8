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

# include <cstring>
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/FormatUtility.hpp>
# include <Siv3D/Resource.hpp>
# include <Siv3D/EngineLog.hpp>
# include "BinaryFileReaderDetail.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	BinaryFileReader::BinaryFileReaderDetail::~BinaryFileReaderDetail()
	{
		close();
	}

	////////////////////////////////////////////////////////////////
	//
	//	open
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::BinaryFileReaderDetail::open(const FilePathView path)
	{
		LOG_DEBUG(fmt::format("BinaryFileReader::BinaryFileReaderDetail::open(\"{0}\")", path.toUTF8()));

		close();

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
				LOG_FAIL(fmt::format("❌ BinaryFileReader: Failed to open resource \"{0}\"", path.toUTF8()));
				return false;
			}

			HGLOBAL pResource = ::LoadResource(hModule, hrs);

			if (not pResource)
			{
				LOG_FAIL(fmt::format("❌ BinaryFileReader: Failed to load resource \"{0}\"", path.toUTF8()));
				return false;
			}

			const Byte* pData = static_cast<const Byte*>(::LockResource(pResource));

			if (not pData)
			{
				LOG_FAIL(fmt::format("❌ BinaryFileReader: Failed to lock resource \"{0}\"", path.toUTF8()));
				return false;
			}

			m_resource =
			{
				.pointer = pData,
				.readPos = 0,
			};

			m_info =
			{
				.fullPath = FilePath{ path },
				.fileSize = ::SizeofResource(hModule, hrs),
				.isOpen = true,
			};

			LOG_INFO(fmt::format("📤 BinaryFileReader: Opened resource \"{0}\" size: {1}", m_info.fullPath.toUTF8(), FormatDataSize(m_info.fileSize).toUTF8()));
		}
		else
		{
			FilePath fullPath = FileSystem::FullPath(path);
			// Match the CRT's read/write sharing and handle inheritance.
			SECURITY_ATTRIBUTES security{ sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
			const HANDLE handle = ::CreateFileW(path.toWstr().c_str(), GENERIC_READ,
				(FILE_SHARE_READ | FILE_SHARE_WRITE), &security, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (handle == INVALID_HANDLE_VALUE)
			{
				LOG_FAIL(fmt::format("❌ BinaryFileReader: Failed to open file `{0}`", path.toUTF8()));
				return false;
			}

			LARGE_INTEGER fileSize;
			if (not ::GetFileSizeEx(handle, &fileSize))
			{
				::CloseHandle(handle);
				LOG_FAIL(fmt::format("❌ BinaryFileReader: Failed to get the size of file `{0}`", path.toUTF8()));
				return false;
			}

			m_file.handle = handle;
			m_info =
			{
				.fullPath = std::move(fullPath),
				.fileSize = fileSize.QuadPart,
				.isOpen = true,
			};

			LOG_INFO(fmt::format("📤 BinaryFileReader: File `{0}` opened (size: {1})", m_info.fullPath.toUTF8(), FormatDataSize(m_info.fileSize).toUTF8()));
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	close
	//
	////////////////////////////////////////////////////////////////

	void BinaryFileReader::BinaryFileReaderDetail::close()
	{
		if (not m_info.isOpen)
		{
			return;
		}

		if (isResource())
		{
			m_resource = {};
			LOG_INFO(fmt::format("📥 BinaryFileReader: Resource `{0}` closed", m_info.fullPath));
		}
		else
		{
			m_file.close();
			LOG_INFO(fmt::format("📥 BinaryFileReader: File `{0}` closed", m_info.fullPath));
		}

		m_info = {};
	}

	////////////////////////////////////////////////////////////////
	//
	//	isOpen
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::BinaryFileReaderDetail::isOpen() const noexcept
	{
		return m_info.isOpen;
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::BinaryFileReaderDetail::size() const noexcept
	{
		return m_info.fileSize;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::BinaryFileReaderDetail::setPos(const int64 pos)
	{
		if (not m_info.isOpen)
		{
			return 0;
		}

		const int64 clampedPos = Clamp<int64>(pos, 0, m_info.fileSize);

		if (isResource())
		{
			return (m_resource.readPos = clampedPos);
		}
		else
		{
			return m_file.setPos(clampedPos);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	skip
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::BinaryFileReaderDetail::skip(const int64 offset)
	{
		if (not m_info.isOpen)
		{
			return 0;
		}

		const int64 current = getPos();

		int64 target;

		if (0 <= offset)
		{
			const int64 room = (m_info.fileSize - current);
			target = current + Min(offset, room);
		}
		else
		{
			// INT64_MIN の符号反転を避ける
			const int64 backward = (offset == INT64_MIN) ? current : Min(current, -offset);
			target = current - backward;
		}

		return setPos(target);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPos
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::BinaryFileReaderDetail::getPos() const
	{
		return (isResource() ? m_resource.readPos : m_file.readPos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	read
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::BinaryFileReaderDetail::read(const NonNull<void*> dst, const int64 readSize)
	{
		if (isResource())
		{
			return m_resource.read(dst, m_resource.readPos, readSize, m_info.fileSize);
		}
		else
		{
			return m_file.read(dst, readSize, m_info.fileSize, m_info.fullPath);
		}
	}

	int64 BinaryFileReader::BinaryFileReaderDetail::read(const NonNull<void*> dst, const int64 pos, const int64 readSize)
	{
		if (isResource())
		{
			return m_resource.read(dst, pos, readSize, m_info.fileSize);
		}
		else
		{
			if (pos != setPos(pos))
			{
				return 0;
			}

			return m_file.read(dst, readSize, m_info.fileSize, m_info.fullPath);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	readExact
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::BinaryFileReaderDetail::readExact(const NonNull<void*> dst, const int64 pos, const int64 readSize)
	{
		if (isResource())
		{
			return (m_resource.read(dst, pos, readSize, m_info.fileSize) == readSize);
		}

		// Commit the logical position only after a complete read. Failure needs no
		// rollback seek, which could itself fail after an I/O error.
		if (m_file.lookaheadAt(dst, pos, readSize, m_info.fileSize, m_info.fullPath) != readSize)
		{
			return false;
		}

		m_file.readPos = (pos + readSize);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	lookahead
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::BinaryFileReaderDetail::lookahead(const NonNull<void*> dst, const int64 readSize)
	{
		if (isResource())
		{
			return m_resource.lookahead(dst, m_resource.readPos, readSize, m_info.fileSize);
		}
		else
		{
			return m_file.lookahead(dst, readSize, m_info.fileSize, m_info.fullPath);
		}
	}

	int64 BinaryFileReader::BinaryFileReaderDetail::lookahead(const NonNull<void*> dst, const int64 pos, const int64 readSize)
	{
		if (isResource())
		{
			return m_resource.lookahead(dst, pos, readSize, m_info.fileSize);
		}
		else
		{
			return m_file.lookaheadAt(dst, pos, readSize, m_info.fileSize, m_info.fullPath);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	path
	//
	////////////////////////////////////////////////////////////////

	const FilePath& BinaryFileReader::BinaryFileReaderDetail::path() const noexcept
	{
		return m_info.fullPath;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isResource
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::BinaryFileReaderDetail::isResource() const noexcept
	{
		return (m_resource.pointer != nullptr);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::BinaryFileReaderDetail::Resource::read(NonNull<void*> dst, const int64 pos, const int64 readSize, const int64 fileSize)
	{
		const int64 readBytes = lookahead(dst, pos, readSize, fileSize);
		readPos = (pos + readBytes);
		return readBytes;
	}

	int64 BinaryFileReader::BinaryFileReaderDetail::Resource::lookahead(NonNull<void*> dst, const int64 pos, const int64 readSize, const int64 fileSize)
	{
		const int64 readBytes = Clamp<int64>(readSize, 0, (fileSize - pos));

		if (0 < readBytes)
		{
			std::memcpy(dst.get(), (pointer + pos), static_cast<size_t>(readBytes));
		}

		return readBytes;
	}

	BinaryFileReader::BinaryFileReaderDetail::File::~File()
	{
		close();
	}

	void BinaryFileReader::BinaryFileReaderDetail::File::close()
	{
		if (handle != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(handle);
			handle = INVALID_HANDLE_VALUE;
		}

		readPos = nativePos = bufferBegin = 0;
		bufferLength = 0;
		// Reuse allocated storage across open()/close(); cached bytes never survive close().
	}

	int64 BinaryFileReader::BinaryFileReaderDetail::File::setPos(const int64 pos)
	{
		if (readPos == pos)
		{
			return readPos;
		}

		const uint64 offset = static_cast<uint64>(pos - bufferBegin);
		if ((offset < bufferLength) || seekNative(pos))
		{
			readPos = pos;
		}

		return readPos;
	}

	bool BinaryFileReader::BinaryFileReaderDetail::File::seekNative(const int64 pos)
	{
		if (nativePos == pos)
		{
			return true;
		}

		LARGE_INTEGER distance;
		distance.QuadPart = pos;
		if (not ::SetFilePointerEx(handle, distance, nullptr, FILE_BEGIN))
		{
			nativePos = -1;
			return false;
		}

		nativePos = pos;
		return true;
	}

	bool BinaryFileReader::BinaryFileReaderDetail::File::readNative(void* const dst, const DWORD size, DWORD& actual)
	{
		actual = 0;
		if (not ::ReadFile(handle, dst, size, &actual, nullptr))
		{
			// Do not rely on the native file pointer after a failed I/O operation.
			nativePos = -1;
			actual = 0;
			return false;
		}

		nativePos += actual;
		return true;
	}

	int64 BinaryFileReader::BinaryFileReaderDetail::File::read(const NonNull<void*> dst, const int64 readSize, const int64 fileSize, const FilePath& fullPath)
	{
		const int64 actual = readAt(dst, readPos, readSize, fileSize, fullPath);
		readPos += actual;
		return actual;
	}

	int64 BinaryFileReader::BinaryFileReaderDetail::File::lookahead(const NonNull<void*> dst, const int64 readSize, const int64 fileSize, const FilePath& fullPath)
	{
		return readAt(dst, readPos, readSize, fileSize, fullPath);
	}

	int64 BinaryFileReader::BinaryFileReaderDetail::File::lookaheadAt(const NonNull<void*> dst, const int64 pos, const int64 readSize, const int64 fileSize, const FilePath& fullPath)
	{
		return readAt(dst, pos, readSize, fileSize, fullPath);
	}

	int64 BinaryFileReader::BinaryFileReaderDetail::File::readAt(const NonNull<void*> dst, int64 pos, const int64 readSize, const int64 fileSize, const FilePath& fullPath)
	{
		const int64 requested = Clamp<int64>(readSize, 0, (fileSize - pos));
		Byte* const destination = static_cast<Byte*>(dst.get());
		int64 total = 0;

		while (total < requested)
		{
			const int64 remaining = (requested - total);
			const uint64 offset = static_cast<uint64>(pos - bufferBegin);
			if (offset < bufferLength)
			{
				const int64 count = Min<int64>(remaining, (bufferLength - offset));
				std::memcpy((destination + total), (buffer.get() + offset), static_cast<size_t>(count));
				pos += count;
				total += count;
				continue;
			}

			if (not seekNative(pos))
			{
				LOG_FAIL(fmt::format("❌ BinaryFileReader `{0}`: seek failed (error: {1})", fullPath, ::GetLastError()));
				break;
			}

			if (remaining < BufferSize)
			{
				if (not buffer)
				{
					buffer = std::make_unique_for_overwrite<Byte[]>(BufferSize);
				}

				bufferBegin = pos;
				bufferLength = 0;
				const DWORD prefetchSize = static_cast<DWORD>(Min<int64>(BufferSize, (fileSize - pos)));
				if (readNative(buffer.get(), prefetchSize, bufferLength))
				{
					if (bufferLength == 0)
					{
						break;
					}

					// A short prefetch is not a failed caller request. Consume its valid
					// bytes first; EOF requires no persistent stream error flags.
					continue;
				}

				// Read-ahead can overlap a locked range beyond the caller's request.
				// Retry only the requested bytes, after restoring the native position.
				if (not seekNative(pos))
				{
					LOG_FAIL(fmt::format("❌ BinaryFileReader `{0}`: seek failed (error: {1})", fullPath, ::GetLastError()));
					break;
				}
			}

			const DWORD count = static_cast<DWORD>(Min<int64>(remaining, MAXDWORD));
			DWORD actual = 0;
			if (not readNative((destination + total), count, actual))
			{
				LOG_FAIL(fmt::format("❌ BinaryFileReader `{0}`: read failed (error: {1})", fullPath, ::GetLastError()));
				break;
			}

			pos += actual;
			total += actual;
			if (actual < count)
			{
				break;
			}
		}

		return total;
	}
}
