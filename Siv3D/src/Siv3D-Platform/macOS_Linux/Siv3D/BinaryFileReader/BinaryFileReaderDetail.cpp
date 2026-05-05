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

		{
			// ファイルのオープン
			{
				m_file.file.open(path.toWstr(), std::ios_base::binary);
				m_file.readPos = 0;

				if (not m_file.file) [[unlikely]]
					{
						LOG_FAIL(fmt::format("❌ BinaryFileReader: Failed to open file `{0}`", path.toUTF8()));
						return false;
					}
			}

			m_info =
			{
				.fullPath = FileSystem::FullPath(path),
				.fileSize = static_cast<int64>(FileSystem::FileSize(path)),
				.isOpen = true,
			};

			LOG_INFO(fmt::format("📤 BinaryFileReader: File `{0}` opened (size: {1})", m_info.fullPath, FormatDataSize(m_info.fileSize).toUTF8()));
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

		{
			m_file.file.close();
			m_file.readPos = 0;
			LOG_INFO(fmt::format("📥 BinaryFileReader: File `{0}` closed", m_info.fullPath.toUTF8()));
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

		if (m_file.readPos == clampedPos)
		{
			return m_file.readPos;
		}

		m_file.file.clear();
		m_file.file.seekg(clampedPos);

		if (not m_file.file)
		{
			m_file.file.clear();
			return m_file.readPos;
		}

		return (m_file.readPos = clampedPos);
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

		const int64 currentPos = getPos();

		int64 clampedPos;

		if (offset >= 0)
		{
			const int64 forwardLimit = (m_info.fileSize - currentPos);
			clampedPos = (currentPos + Min(offset, forwardLimit));
		}
		else
		{
			const int64 backwardLimit = currentPos;

			const int64 backward = (offset == INT64_MIN)
				? backwardLimit
				: Min<int64>(-offset, backwardLimit);

			clampedPos = (currentPos - backward);
		}

		m_file.file.clear();
		m_file.file.seekg(clampedPos);

		if (not m_file.file)
		{
			m_file.file.clear();
			return m_file.readPos;
		}

		return (m_file.readPos = clampedPos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPos
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::BinaryFileReaderDetail::getPos() const
	{
		return m_file.readPos;
	}

	////////////////////////////////////////////////////////////////
	//
	//	read
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::BinaryFileReaderDetail::read(const NonNull<void*> dst, const int64 readSize)
	{
		return m_file.read(dst, readSize, m_info.fileSize, m_info.fullPath);
	}

	int64 BinaryFileReader::BinaryFileReaderDetail::read(const NonNull<void*> dst, const int64 pos, const int64 readSize)
	{
		if (pos != setPos(pos))
		{
			return 0;
		}

		return m_file.read(dst, readSize, m_info.fileSize, m_info.fullPath);
	}

	////////////////////////////////////////////////////////////////
	//
	//	lookahead
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::BinaryFileReaderDetail::lookahead(const NonNull<void*> dst, const int64 readSize)
	{
		return m_file.lookahead(dst, readSize, m_info.fileSize, m_info.fullPath);
	}

	int64 BinaryFileReader::BinaryFileReaderDetail::lookahead(const NonNull<void*> dst, const int64 pos, const int64 readSize)
	{
		return m_file.lookaheadAt(dst, pos, readSize, m_info.fileSize, m_info.fullPath);
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
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::BinaryFileReaderDetail::File::read(const NonNull<void*> dst, const int64 readSize, const int64 fileSize, const FilePath& fullPath)
	{
		const int64 readBytes = Clamp<int64>(readSize, 0, (fileSize - readPos));

		if (readBytes <= 0)
		{
			return 0;
		}

		file.read(static_cast<char*>(dst.get()), static_cast<std::streamsize>(readBytes));

		const int64 actual = static_cast<int64>(file.gcount());
		readPos += actual;

		if (actual == readBytes)
		{
			return actual;
		}

		if (file.eof())
		{
			file.clear();
			return actual;
		}

		LOG_FAIL(fmt::format("❌ BinaryFileReader `{0}`: read() failed", fullPath));
		file.clear();

		return actual;
	}

	int64 BinaryFileReader::BinaryFileReaderDetail::File::lookahead(const NonNull<void*> dst, const int64 readSize, const int64 fileSize, const FilePath& fullPath)
	{
		const int64 previousReadPos = readPos;
		const int64 readBytes = read(dst, readSize, fileSize, fullPath);
		readPos = previousReadPos;
		file.clear();
		file.seekg(previousReadPos);
		return readBytes;
	}

	int64 BinaryFileReader::BinaryFileReaderDetail::File::lookaheadAt(const NonNull<void*> dst, const int64 pos, const int64 readSize, const int64 fileSize, const FilePath& fullPath)
	{
		const int64 previousReadPos = readPos;

		if (pos != previousReadPos)
		{
			file.clear();
			file.seekg(pos);

			if (not file)
			{
				file.clear();
				return 0;
			}

			readPos = pos;
		}

		const int64 readBytes = read(dst, readSize, fileSize, fullPath);

		readPos = previousReadPos;

		file.clear();
		file.seekg(previousReadPos);

		if (not file)
		{
			file.clear();
		}

		return readBytes;
	}
}
