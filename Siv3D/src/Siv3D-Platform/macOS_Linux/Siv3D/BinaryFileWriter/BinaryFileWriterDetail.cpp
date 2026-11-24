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
	BinaryFileWriter::BinaryFileWriterDetail::~BinaryFileWriterDetail()
	{
		close();
	}

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
		const FilePath parentFilePath = FileSystem::ParentPath(path, 0, fullPath);

		if (parentFilePath && (not FileSystem::Exists(parentFilePath)) && (not FileSystem::CreateDirectories(parentFilePath)))
		{
			LOG_FAIL(fmt::format("❌ BinaryFileWriter: Failed to create parent directories \"{0}\"", parentFilePath.toUTF8()));
			return false;
		}

		// ファイルのオープン
		{
			const bool append = (writeMode == FileWriteMode::Append && FileSystem::Exists(fullPath));
			
			std::FILE* file = std::fopen(Unicode::ToUTF8(fullPath).c_str(), (append ? "r+" : "w"));

			if (not file)
			{
				LOG_FAIL(fmt::format("❌ BinaryFileWriter: Failed to open the file `{0}`", path.toUTF8()));
				return false;
			}

			m_file =
			{
				.file = file
			};

			if (writeMode == FileWriteMode::Append)
			{
				std::fseek(m_file.file, 0, SEEK_END);
			}

			m_info =
			{
				.fullPath = fullPath,
				.isOpen = true,
			};

			LOG_INFO(fmt::format("📤 BinaryFileWriter: File `{0}` opened", m_info.fullPath.toUTF8()));
		}

		m_hasError = false;
		return true;
	}

	bool BinaryFileWriter::BinaryFileWriterDetail::close()
	{
		if (not m_info.isOpen)
		{
			return (not m_hasError);
		}

		flush();

		m_buffer = {};

		if (std::fclose(m_file.file) != 0)
		{
			m_hasError = true;
		}
		m_file = {};

		LOG_INFO(fmt::format("📥 BinaryFileWriter: File `{0}` closed", m_info.fullPath.toUTF8()));

		m_info = {};
		return (not m_hasError);
	}

	bool BinaryFileWriter::BinaryFileWriterDetail::isOpen() const noexcept
	{
		return m_info.isOpen;
	}

	bool BinaryFileWriter::BinaryFileWriterDetail::flush()
	{
		if ((not m_info.isOpen) || m_hasError)
		{
			return (not m_hasError);
		}

		if (m_buffer.currentWritePos > 0)
		{
			const size_t written = std::fwrite(m_buffer.data.get(), 1, m_buffer.currentWritePos, m_file.file);
			m_hasError = (written != m_buffer.currentWritePos);
			m_buffer.currentWritePos = 0;
		}

		// Direct writes may still be buffered by stdio, even when our buffer is empty.
		if (std::fflush(m_file.file) != 0)
		{
			m_hasError = true;
		}
		return (not m_hasError);
	}

	void BinaryFileWriter::BinaryFileWriterDetail::clear()
	{
		if ((not m_info.isOpen) || m_hasError)
		{
			return;
		}

		m_buffer.currentWritePos = 0;

		const int closed = std::fclose(m_file.file);
		m_file.file = nullptr;
		if (closed != 0)
		{
			m_hasError = true;
			m_info = {};
			return;
		}
		
		m_file.file = std::fopen(Unicode::ToUTF8(m_info.fullPath).c_str(), "w");
		
		if (not m_file.file)
		{
			m_hasError = true;
			m_info = {};
		}
	}

	int64 BinaryFileWriter::BinaryFileWriterDetail::size()
	{
		if (not m_info.isOpen)
		{
			return 0;
		}

		flush();

		return FileSystem::FileSize(m_info.fullPath);
	}

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
		
		if (std::fseek(m_file.file, clampedPos, SEEK_SET) != 0)
		{
			return -1;
		}
		
		return std::ftell(m_file.file);
	}

	int64 BinaryFileWriter::BinaryFileWriterDetail::getPos()
	{
		if (not m_info.isOpen)
		{
			return 0;
		}

		flush();

		return std::ftell(m_file.file);
	}

	int64 BinaryFileWriter::BinaryFileWriterDetail::write(const NonNull<const void*> src, const size_t size)
	{
		if ((not m_info.isOpen) || m_hasError)
		{
			return 0;
		}

		if (const size_t room = (BufferSize - m_buffer.currentWritePos);
			size <= room)
		{
			return fillBuffer(src, size);
		}
		
		if (not flush())
		{
			return 0;
		}

		const size_t written = std::fwrite(src.get(), 1, size, m_file.file);
		m_hasError = (written != size);
		return written;
	}

	const FilePath& BinaryFileWriter::BinaryFileWriterDetail::path() const noexcept
	{
		return m_info.fullPath;
	}

	int64 BinaryFileWriter::BinaryFileWriterDetail::fillBuffer(const NonNull<const void*> src, const size_t size)
	{
		if (not m_buffer.data)
		{
			m_buffer.data = std::make_unique<Byte[]>(BufferSize);
		}

		std::memcpy((m_buffer.data.get() + m_buffer.currentWritePos), src.get(), size);

		m_buffer.currentWritePos += size;

		return size;
	}
}
