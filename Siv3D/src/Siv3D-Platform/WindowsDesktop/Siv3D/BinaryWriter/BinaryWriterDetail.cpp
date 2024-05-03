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

# include <Siv3D/EngineLog.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/FormatUtility.hpp>
# include "BinaryWriterDetail.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr DWORD MakeCreationDisposition(const OpenMode openMode) noexcept
		{
			DWORD creationDisposition = 0;

			if (openMode & OpenMode::Append)
			{
				creationDisposition = OPEN_ALWAYS;
			}
			else if (openMode & OpenMode::Trunc)
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

	BinaryWriter::BinaryWriterDetail::~BinaryWriterDetail()
	{
		close();
	}

	////////////////////////////////////////////////////////////////
	//
	//	open
	//
	////////////////////////////////////////////////////////////////

	bool BinaryWriter::BinaryWriterDetail::open(const FilePathView path, const OpenMode openMode)
	{
		LOG_DEBUG(fmt::format("BinaryWriter::BinaryWriterDetail::open(\"{0}\", {1})", path, FromEnum(openMode)));

		close();

		if (not path)
		{
			LOG_FAIL("❌ BinaryWriter: path is empty");
			return false;
		}

		if (FileSystem::IsResourcePath(path))
		{
			LOG_FAIL("❌ BinaryWriter: path is a resource path");
			return false;
		}

		FilePath fullPath;
		const FilePath parentDirectory = FileSystem::ParentPath(path, 0, fullPath);

		if (parentDirectory && (not FileSystem::Exists(parentDirectory)) && (not FileSystem::CreateDirectories(parentDirectory)))
		{
			LOG_FAIL(fmt::format("❌ BinaryWriter: Failed to create parent directories \"{0}\"", parentDirectory));
			return false;
		}

		// ファイルのオープン
		{
			const HANDLE handle = ::CreateFile2(path.toWstr().c_str(), GENERIC_WRITE, 0, MakeCreationDisposition(openMode), nullptr);

			if (handle == INVALID_HANDLE_VALUE)
			{
				LOG_FAIL(fmt::format("❌ BinaryWriter: Failed to open the file `{0}`. {1}", path, Platform::Windows::GetLastErrorMessage()));
				return false;
			}

			m_file =
			{
				.handle = handle
			};

			if (openMode & OpenMode::Append)
			{
				LARGE_INTEGER distance{ 0, 0 };
				::SetFilePointerEx(m_file.handle, distance, nullptr, FILE_END);
			}

			m_info =
			{
				.fullPath = fullPath,
				.isOpen = true,
			};

			LOG_INFO(fmt::format("📤 BinaryWriter: File `{0}` opened", m_info.fullPath));
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	close
	//
	////////////////////////////////////////////////////////////////

	void BinaryWriter::BinaryWriterDetail::close()
	{
		if (not m_info.isOpen)
		{
			return;
		}

		flush();

		m_buffer = {};

		m_file.close();

		LOG_INFO(fmt::format("📥 BinaryWriter: File `{0}` closed", m_info.fullPath));

		m_info = {};
	}

	////////////////////////////////////////////////////////////////
	//
	//	isOpen
	//
	////////////////////////////////////////////////////////////////

	bool BinaryWriter::BinaryWriterDetail::isOpen() const noexcept
	{
		return m_info.isOpen;
	}

	////////////////////////////////////////////////////////////////
	//
	//	flush
	//
	////////////////////////////////////////////////////////////////

	void BinaryWriter::BinaryWriterDetail::flush()
	{
		if (m_buffer.writePos == 0)
		{
			return;
		}

		DWORD written = 0;
		::WriteFile(m_file.handle, m_buffer.data.get(), static_cast<uint32>(m_buffer.writePos), &written, nullptr);

		m_buffer.writePos = 0;
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void BinaryWriter::BinaryWriterDetail::clear()
	{
		if (not m_info.isOpen)
		{
			return;
		}

		m_buffer.writePos = 0;

		setPos(0);

		::SetEndOfFile(m_file.handle);
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryWriter::BinaryWriterDetail::size()
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

	int64 BinaryWriter::BinaryWriterDetail::setPos(const int64 clampedPos)
	{
		if (not m_info.isOpen)
		{
			return 0;
		}

		flush();

		const LARGE_INTEGER distance{ .QuadPart = clampedPos };
		LARGE_INTEGER newPos;
		::SetFilePointerEx(m_file.handle, distance, &newPos, FILE_BEGIN);

		return newPos.QuadPart;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPos
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryWriter::BinaryWriterDetail::getPos()
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

	int64 BinaryWriter::BinaryWriterDetail::write(const NonNull<const void*> src, const size_t writeSize)
	{
		if (not m_info.isOpen)
		{
			return 0;
		}

		if (writeSize <= m_buffer.available())
		{
			return fillBuffer(src, writeSize);
		}

		flush();

		DWORD writtenBytes;
		::WriteFile(m_file.handle, src.get(), static_cast<uint32>(writeSize), &writtenBytes, nullptr);

		return writtenBytes;
	}

	////////////////////////////////////////////////////////////////
	//
	//	path
	//
	////////////////////////////////////////////////////////////////

	const FilePath& BinaryWriter::BinaryWriterDetail::path() const noexcept
	{
		return m_info.fullPath;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryWriter::BinaryWriterDetail::fillBuffer(const NonNull<const void*> src, const size_t writeSize)
	{
		if (not m_buffer.data)
		{
			m_buffer.data = std::make_unique<Byte[]>(BufferSize);
		}

		std::memcpy((m_buffer.data.get() + m_buffer.writePos), src.get(), writeSize);

		m_buffer.writePos += writeSize;

		return writeSize;
	}

	void BinaryWriter::BinaryWriterDetail::File::close()
	{
		::CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
	}

	size_t BinaryWriter::BinaryWriterDetail::Buffer::available() const noexcept
	{
		return (BufferSize - writePos);
	}
}
