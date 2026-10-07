// Experimental synchronous Win32 backend for the BinaryFileReader comparison.
// Only the subset of std::ifstream used by BinaryFileReaderDetail is provided.
# pragma once
# include <Siv3D/Windows/Windows.hpp>
# include <algorithm>
# include <cstdint>
# include <cstring>
# include <fstream>
# include <memory>

# ifndef READER_BUFFER_SIZE
# define READER_BUFFER_SIZE 16384
# endif
# ifndef READER_SEQUENTIAL_SCAN
# define READER_SEQUENTIAL_SCAN 0
# endif

namespace benchmark
{
	class Win32InputStream
	{
	public:

		Win32InputStream() = default;
		Win32InputStream(const Win32InputStream&) = delete;
		Win32InputStream& operator =(const Win32InputStream&) = delete;

		~Win32InputStream()
		{
			close();
		}

		void open(const std::wstring& path, std::ios_base::openmode)
		{
			close();
			// Match MSVC CRT sharing and default handle inheritance (no _O_NOINHERIT).
			SECURITY_ATTRIBUTES security{ sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
			m_handle = ::CreateFileW(path.c_str(), GENERIC_READ,
				(FILE_SHARE_READ | FILE_SHARE_WRITE), &security, OPEN_EXISTING,
				(FILE_ATTRIBUTE_NORMAL | (READER_SEQUENTIAL_SCAN ? FILE_FLAG_SEQUENTIAL_SCAN : 0)), nullptr);
			m_failed = (m_handle == INVALID_HANDLE_VALUE);
			m_eof = false;
		}

		void close()
		{
			if (m_handle != INVALID_HANDLE_VALUE)
			{
				::CloseHandle(m_handle);
				m_handle = INVALID_HANDLE_VALUE;
			}
			m_pos = m_nativePos = m_bufferBegin = 0;
			m_bufferLength = 0;
			m_count = 0;
			// Retain an allocated buffer for open()/close() reuse, as a candidate design choice.
		}

		explicit operator bool() const noexcept
		{
			return (not m_failed);
		}

		bool eof() const noexcept
		{
			return m_eof;
		}

		void clear() noexcept
		{
			m_failed = m_eof = false;
		}

		std::streamsize gcount() const noexcept
		{
			return m_count;
		}

		void seekg(const std::int64_t pos)
		{
			if constexpr (BufferSize == 0)
			{
				// Raw control: conventional ReadFile + SetFilePointerEx, no user buffer.
				if (not seekNative(pos))
				{
					return;
				}
			}
			// Buffered candidate: logical seeking preserves reusable bytes. The native
			// handle is synchronized only when an actual OS read is necessary.
			m_pos = pos;
		}

		void read(char* dst, const std::streamsize size)
		{
			m_count = 0;
			while (m_count < size)
			{
				const auto remaining = static_cast<std::uint64_t>(size - m_count);
				if constexpr (BufferSize != 0)
				{
					const auto offset = static_cast<std::uint64_t>(m_pos - m_bufferBegin);
					if (offset < m_bufferLength)
					{
						const auto count = static_cast<std::size_t>((std::min)(remaining, m_bufferLength - offset));
						std::memcpy(dst + m_count, m_buffer.get() + offset, count);
						m_count += count;
						m_pos += count;
						continue;
					}
				}

				if (not seekNative(m_pos))
				{
					return;
				}

				if constexpr (BufferSize != 0)
				{
					if (remaining < BufferSize)
					{
						if (not m_buffer)
						{
							// Allocate on the first small read; large one-shot reads need no buffer.
							m_buffer = std::make_unique_for_overwrite<char[]>(BufferSize);
						}
						m_bufferBegin = m_pos;
						m_bufferLength = readNative(m_buffer.get(), BufferSize);
						if (m_bufferLength == 0)
						{
							return;
						}
						continue;
					}
				}

				// Bypass the user buffer for bulk transfers. Split the 64-bit API size
				// into DWORD-sized OS requests without narrowing the entire request.
				const DWORD requested = static_cast<DWORD>((std::min)(remaining, std::uint64_t{ MAXDWORD }));
				const DWORD count = readNative(dst + m_count, requested);
				m_count += count;
				m_pos += count;
				if (count < requested)
				{
					return;
				}
			}
		}

	private:

		static constexpr DWORD BufferSize = READER_BUFFER_SIZE;
		HANDLE m_handle = INVALID_HANDLE_VALUE;
		std::unique_ptr<char[]> m_buffer;
		std::int64_t m_pos = 0;
		std::int64_t m_nativePos = 0;
		std::int64_t m_bufferBegin = 0;
		std::uint64_t m_bufferLength = 0;
		std::streamsize m_count = 0;
		bool m_failed = false;
		bool m_eof = false;

		bool seekNative(const std::int64_t pos)
		{
			if (m_nativePos == pos)
			{
				return true;
			}
			LARGE_INTEGER distance;
			distance.QuadPart = pos;
			if (not ::SetFilePointerEx(m_handle, distance, nullptr, FILE_BEGIN))
			{
				m_failed = true;
				return false;
			}
			m_nativePos = pos;
			return true;
		}

		DWORD readNative(char* dst, const DWORD requested)
		{
			DWORD count = 0;
			if (not ::ReadFile(m_handle, dst, requested, &count, nullptr))
			{
				m_failed = true;
				return 0;
			}
			m_nativePos += count;
			if (count < requested)
			{
				m_eof = m_failed = true;
			}
			return count;
		}
	};
}
