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

# include <Siv3D/EngineLog.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/FormatUtility.hpp>
# include "BinaryFileWriterDetail.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr DWORD MakeCreationDisposition(const FileWriteMode writeMode) noexcept
		{
			DWORD creationDisposition = 0;

			if (writeMode == FileWriteMode::Append)
			{
				creationDisposition = OPEN_ALWAYS;
			}
			else if (writeMode == FileWriteMode::Trunc)
			{
				creationDisposition = CREATE_ALWAYS;
			}

			return creationDisposition;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	BinaryFileWriter::BinaryFileWriterDetail::~BinaryFileWriterDetail()
	{
		close();
	}

	////////////////////////////////////////////////////////////////
	//
	//	open
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileWriter::BinaryFileWriterDetail::open(const FilePathView path, const FileWriteMode writeMode)
	{
		LOG_DEBUG(fmt::format("BinaryFileWriter::BinaryFileWriterDetail::open(\"{0}\", {1})", path.toUTF8(), FromEnum(writeMode)));

		close();

		if (not path)
		{
			LOG_FAIL("❌ BinaryFileWriter: path is empty");
			return false;
		}

		if (FileSystem::IsResourcePath(path))
		{
			LOG_FAIL("❌ BinaryFileWriter: path is a resource path");
			return false;
		}

		FilePath fullPath;
		const FilePath parentDirectory = FileSystem::ParentPath(path, 0, fullPath);

		if (parentDirectory && (not FileSystem::Exists(parentDirectory)) && (not FileSystem::CreateDirectories(parentDirectory)))
		{
			LOG_FAIL(fmt::format("❌ BinaryFileWriter: Failed to create parent directories \"{0}\"", parentDirectory));
			return false;
		}

		// ファイルのオープン
		{
			const HANDLE handle = ::CreateFile2(path.toWstr().c_str(), GENERIC_WRITE, 0, MakeCreationDisposition(writeMode), nullptr);

			if (handle == INVALID_HANDLE_VALUE)
			{
				LOG_FAIL(fmt::format("❌ BinaryFileWriter: Failed to open the file `{0}`. {1}", path.toUTF8(), Platform::Windows::GetLastErrorMessage().toUTF8()));
				return false;
			}

			m_file =
			{
				.handle = handle
			};

			if (writeMode == FileWriteMode::Append)
			{
				LARGE_INTEGER distance{ 0, 0 };
				::SetFilePointerEx(m_file.handle, distance, nullptr, FILE_END);
			}

			m_info =
			{
				.fullPath = fullPath,
				.isOpen = true,
			};

			LOG_INFO(fmt::format("📤 BinaryFileWriter: File `{0}` opened", m_info.fullPath));
		}

		m_hasError = false;
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	close
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileWriter::BinaryFileWriterDetail::close()
	{
		if (not m_info.isOpen)
		{
			return (not m_hasError);
		}

		flush();

		m_buffer = {};

		if (not m_file.close())
		{
			m_hasError = true;
		}

		LOG_INFO(fmt::format("📥 BinaryFileWriter: File `{0}` closed", m_info.fullPath));

		m_info = {};
		return (not m_hasError);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isOpen
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileWriter::BinaryFileWriterDetail::isOpen() const noexcept
	{
		return m_info.isOpen;
	}

	////////////////////////////////////////////////////////////////
	//
	//	flush
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileWriter::BinaryFileWriterDetail::flush()
	{
		if ((m_buffer.writePos == 0) || m_hasError)
		{
			return (not m_hasError);
		}

		DWORD written = 0;
		const bool succeeded = (::WriteFile(m_file.handle, m_buffer.data.get(), static_cast<uint32>(m_buffer.writePos), &written, nullptr) != 0);
		m_hasError = ((not succeeded) || (written != m_buffer.writePos));

		m_buffer.writePos = 0;
		return (not m_hasError);
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void BinaryFileWriter::BinaryFileWriterDetail::clear()
	{
		if ((not m_info.isOpen) || m_hasError)
		{
			return;
		}

		m_buffer.writePos = 0;

		if ((setPos(0) != 0) || (not ::SetEndOfFile(m_file.handle)))
		{
			m_hasError = true;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileWriter::BinaryFileWriterDetail::size()
	{
		if (not m_info.isOpen)
		{
			return 0;
		}

		LARGE_INTEGER fileSize;
		::GetFileSizeEx(m_file.handle, &fileSize);

		return (fileSize.QuadPart + m_buffer.writePos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileWriter::BinaryFileWriterDetail::setPos(const int64 clampedPos)
	{
		if (not m_info.isOpen)
		{
			return 0;
		}

		if (not flush())
		{
			return -1;
		}

		const LARGE_INTEGER distance{ .QuadPart = clampedPos };
		LARGE_INTEGER newPos{};
		if (not ::SetFilePointerEx(m_file.handle, distance, &newPos, FILE_BEGIN))
		{
			return -1;
		}

		return newPos.QuadPart;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPos
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileWriter::BinaryFileWriterDetail::getPos()
	{
		if (not m_info.isOpen) [[unlikely]]
		{
			return 0;
		}

		const LARGE_INTEGER distance{ 0, 0 };
		LARGE_INTEGER currentPos;
		::SetFilePointerEx(m_file.handle, distance, &currentPos, FILE_CURRENT);

		return (currentPos.QuadPart + m_buffer.writePos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	write
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileWriter::BinaryFileWriterDetail::write(const NonNull<const void*> src, const size_t writeSize)
	{
		if ((not m_info.isOpen) || m_hasError)
		{
			return 0;
		}

		if (writeSize <= m_buffer.available())
		{
			return fillBuffer(src, writeSize);
		}

		if (not flush())
		{
			return 0;
		}

		DWORD writtenBytes = 0;
		const bool succeeded = (::WriteFile(m_file.handle, src.get(), static_cast<uint32>(writeSize), &writtenBytes, nullptr) != 0);
		m_hasError = ((not succeeded) || (writtenBytes != writeSize));

		return writtenBytes;
	}

	////////////////////////////////////////////////////////////////
	//
	//	path
	//
	////////////////////////////////////////////////////////////////

	const FilePath& BinaryFileWriter::BinaryFileWriterDetail::path() const noexcept
	{
		return m_info.fullPath;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileWriter::BinaryFileWriterDetail::fillBuffer(const NonNull<const void*> src, const size_t writeSize)
	{
		if (not m_buffer.data)
		{
			m_buffer.data = std::make_unique<Byte[]>(BufferSize);
		}

		std::memcpy((m_buffer.data.get() + m_buffer.writePos), src.get(), writeSize);

		m_buffer.writePos += writeSize;

		return writeSize;
	}

	bool BinaryFileWriter::BinaryFileWriterDetail::File::close()
	{
		const bool succeeded = (::CloseHandle(handle) != 0);
		handle = INVALID_HANDLE_VALUE;
		return succeeded;
	}

	size_t BinaryFileWriter::BinaryFileWriterDetail::Buffer::available() const noexcept
	{
		return (BufferSize - writePos);
	}
}
