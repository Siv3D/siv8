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

# include <Siv3D/EngineLog.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/FormatUtility.hpp>
# include "BinaryWriterDetail.hpp"

namespace s3d
{
	BinaryWriter::BinaryWriterDetail::~BinaryWriterDetail()
	{
		close();
	}

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
		const FilePath parentFilePath = FileSystem::ParentPath(path, 0, fullPath);

		if (parentFilePath && (not FileSystem::Exists(parentFilePath)) && (not FileSystem::CreateDirectories(parentFilePath)))
		{
			LOG_FAIL(fmt::format("❌ BinaryWriter: Failed to create parent directories \"{0}\"", parentFilePath));
			return false;
		}

		// ファイルのオープン
		{
			const bool append = (openMode == OpenMode::Append && FileSystem::Exists(fullPath));
			
			std::FILE* file = std::fopen(Unicode::ToUTF8(fullPath).c_str(), (append ? "r+" : "w"));

			if (not file)
			{
				LOG_FAIL(fmt::format("❌ BinaryWriter: Failed to open the file `{0}`", path));
				return false;
			}

			m_file =
			{
				.file = file
			};

			if (openMode & OpenMode::Append)
			{
				std::fseek(m_file.file, 0, SEEK_END);
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

	void BinaryWriter::BinaryWriterDetail::close()
	{
		if (not m_info.isOpen)
		{
			return;
		}

		flush();

		m_buffer = {};

		std::fclose(m_file.file);
		m_file = {};

		LOG_INFO(fmt::format("📥 BinaryWriter: File `{0}` closed", m_info.fullPath));

		m_info = {};
	}

	bool BinaryWriter::BinaryWriterDetail::isOpen() const noexcept
	{
		return m_info.isOpen;
	}

	void BinaryWriter::BinaryWriterDetail::flush()
	{
		if (m_buffer.currentWritePos == 0)
		{
			return;
		}

		std::fwrite(m_buffer.data.get(), 1, m_buffer.currentWritePos, m_file.file);

		std::fflush(m_file.file);

		m_buffer.currentWritePos = 0;
	}

	void BinaryWriter::BinaryWriterDetail::clear()
	{
		if (not m_info.isOpen)
		{
			return;
		}

		m_buffer.currentWritePos = 0;

		std::fclose(m_file.file);
		
		m_file.file = std::fopen(Unicode::ToUTF8(m_info.fullPath).c_str(), "w");
		
		if (not m_file.file)
		{
			close();
		}
	}

	int64 BinaryWriter::BinaryWriterDetail::size()
	{
		if (not m_info.isOpen)
		{
			return 0;
		}

		flush();

		return FileSystem::FileSize(m_info.fullPath);
	}

	int64 BinaryWriter::BinaryWriterDetail::setPos(const int64 clampedPos)
	{
		if (not m_info.isOpen)
		{
			return 0;
		}
		
		flush();
		
		std::fseek(m_file.file, clampedPos, SEEK_SET);
		
		return std::ftell(m_file.file);
	}

	int64 BinaryWriter::BinaryWriterDetail::getPos()
	{
		if (not m_info.isOpen)
		{
			return 0;
		}

		flush();

		return std::ftell(m_file.file);
	}

	int64 BinaryWriter::BinaryWriterDetail::write(const NonNull<const void*> src, const size_t size)
	{
		if (not m_info.isOpen)
		{
			return 0;
		}

		if (const size_t room = (BufferSize - m_buffer.currentWritePos);
			size <= room)
		{
			return fillBuffer(src, size);
		}
		
		flush();

		return std::fwrite(src.get(), 1, size, m_file.file);
	}

	const FilePath& BinaryWriter::BinaryWriterDetail::path() const noexcept
	{
		return m_info.fullPath;
	}

	int64 BinaryWriter::BinaryWriterDetail::fillBuffer(const NonNull<const void*> src, const size_t size)
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
