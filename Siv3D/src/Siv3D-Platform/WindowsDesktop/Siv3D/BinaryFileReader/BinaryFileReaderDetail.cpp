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
			// ファイルのオープン
			{
				m_file.file.open(path.toWstr(), std::ios_base::binary);
				m_file.readPos = 0;

				if (not m_file.file)
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
			m_file.file.close();
			m_file.readPos = 0;
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

		const auto RestoreReadPos = [&]()
		{
			readPos = previousReadPos;

			file.clear();
			file.seekg(previousReadPos);

			if (not file)
			{
				LOG_FAIL(fmt::format("❌ BinaryFileReader `{0}`: Failed to restore the read position", fullPath));
				file.clear();
			}
		};

		if (pos != previousReadPos)
		{
			file.clear();
			file.seekg(pos);

			if (not file)
			{
				LOG_FAIL(fmt::format("❌ BinaryFileReader `{0}`: seekg() failed", fullPath));
				file.clear();
				RestoreReadPos();
				return 0;
			}

			readPos = pos;
		}

		const int64 readBytes = read(dst, readSize, fileSize, fullPath);

		RestoreReadPos();

		return readBytes;
	}
}
