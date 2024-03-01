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
# include "BinaryReaderDetail.hpp"

namespace s3d
{
	BinaryReader::BinaryReaderDetail::~BinaryReaderDetail()
	{
		close();
	}

	bool BinaryReader::BinaryReaderDetail::open(const FilePathView path)
	{
		LOG_TRACE(fmt::format("BinaryReader::BinaryReaderDetail::open(\"{0}\")", path));

		close();

		if (FileSystem::IsResourcePath(path))
		{
			HMODULE hModule = ::GetModuleHandleW(nullptr);

			if (not hModule)
			{
				LOG_FAIL("GetModuleHandleW() failed.");
				return false;
			}

			HRSRC hrs = ::FindResourceW(hModule, path.substr(1).toWstr().c_str(), L"FILE");

			if (not hrs)
			{
				LOG_FAIL(fmt::format("❌ BinaryReader: Failed to open resource \"{0}\"", path));
				return false;
			}

			HGLOBAL pResource = ::LoadResource(hModule, hrs);

			if (not pResource) [[unlikely]]
			{
				LOG_FAIL("LoadResource() failed.");
				return false;
			}

			m_resource =
			{
				.pointer = static_cast<const Byte*>(::LockResource(pResource)),
				.readPos = 0,
			};

			m_info =
			{
				.fullPath = FilePath{ path },
				.fileSize = ::SizeofResource(hModule, hrs),
				.isOpen = true,
			};

			LOG_INFO(fmt::format("📤 BinaryReader: Opened resource \"{0}\" size: {1}", m_info.fullPath, FormatDataSize(m_info.fileSize)));
		}
		else
		{
			// ファイルのオープン
			{
				m_file.file.open(path.toWstr(), std::ios_base::binary);
				m_file.readPos = 0;

				if (not m_file.file) [[unlikely]]
					{
						LOG_FAIL(fmt::format("❌ BinaryReader: Failed to open file `{0}`", path));
						return false;
					}
			}

			m_info =
			{
				.fullPath = FileSystem::FullPath(path),
				.fileSize = static_cast<int64>(FileSystem::FileSize(path)),
				.isOpen = true,
			};

			LOG_INFO(fmt::format("📤 BinaryReader: File `{0}` opened (size: {1})", m_info.fullPath, FormatDataSize(m_info.fileSize)));
		}

		return true;
	}


	void BinaryReader::BinaryReaderDetail::close()
	{
		if (not m_info.isOpen)
		{
			return;
		}

		if (isResource())
		{
			m_resource = {};
			LOG_INFO(fmt::format("📥 BinaryReader: Resource `{0}` closed", m_info.fullPath));
		}
		else
		{
			m_file.file.close();
			m_file.readPos = 0;
			LOG_INFO(fmt::format("📥 BinaryReader: File `{0}` closed", m_info.fullPath));
		}

		m_info = {};
	}

	bool BinaryReader::BinaryReaderDetail::isOpen() const noexcept
	{
		return m_info.isOpen;
	}

	int64 BinaryReader::BinaryReaderDetail::size() const noexcept
	{
		return m_info.fileSize;
	}

	int64 BinaryReader::BinaryReaderDetail::setPos(const int64 pos)
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
			m_file.file.seekg(clampedPos);
			return (m_file.readPos = clampedPos);
		}
	}

	int64 BinaryReader::BinaryReaderDetail::skip(const int64 offset)
	{
		if (not m_info.isOpen)
		{
			return 0;
		}

		const int64 clampedPos = Clamp<int64>((getPos() + offset), 0, m_info.fileSize);

		if (isResource())
		{
			return (m_resource.readPos = clampedPos);
		}
		else
		{
			m_file.file.seekg(clampedPos);
			return (m_file.readPos = clampedPos);
		}
	}

	int64 BinaryReader::BinaryReaderDetail::getPos()
	{
		return (isResource() ? m_resource.readPos : m_file.readPos);
	}

	int64 BinaryReader::BinaryReaderDetail::read(const NonNull<void*> dst, const int64 readSize)
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

	int64 BinaryReader::BinaryReaderDetail::read(const NonNull<void*> dst, const int64 pos, const int64 readSize)
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

	int64 BinaryReader::BinaryReaderDetail::lookahead(const NonNull<void*> dst, const int64 readSize)
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

	int64 BinaryReader::BinaryReaderDetail::lookahead(const NonNull<void*> dst, const int64 pos, const int64 readSize)
	{
		if (isResource())
		{
			return m_resource.lookahead(dst, pos, readSize, m_info.fileSize);
		}
		else
		{
			if (pos != setPos(pos))
			{
				return 0;
			}

			return m_file.lookahead(dst, readSize, m_info.fileSize, m_info.fullPath);
		}
	}

	const FilePath& BinaryReader::BinaryReaderDetail::path() const noexcept
	{
		return m_info.fullPath;
	}

	bool BinaryReader::BinaryReaderDetail::isResource() const noexcept
	{
		return (m_resource.pointer != nullptr);
	}

	int64 BinaryReader::BinaryReaderDetail::Resource::read(NonNull<void*> dst, const int64 pos, const int64 readSize, const int64 fileSize)
	{
		const int64 readBytes = lookahead(dst, pos, readSize, fileSize);
		readPos = (pos + readBytes);
		return readBytes;
	}

	int64 BinaryReader::BinaryReaderDetail::Resource::lookahead(NonNull<void*> dst, const int64 pos, const int64 readSize, const int64 fileSize)
	{
		const int64 readBytes = Clamp<int64>(readSize, 0, (fileSize - pos));
		std::memcpy(dst.get(), (pointer + pos), static_cast<size_t>(readBytes));
		return readBytes;
	}

	int64 BinaryReader::BinaryReaderDetail::File::read(const NonNull<void*> dst, const int64 readSize, const int64 fileSize, const FilePath& fullPath)
	{
		const int64 readBytes = Clamp<int64>(readSize, 0, (fileSize - readPos));

		if (readBytes == 0)
		{
			return 0;
		}

		if (not file.read(static_cast<char*>(dst.get()), readBytes))
		{
			readPos = file.tellg();

			if (file.eof())
			{
				return readBytes;
			}

			LOG_FAIL(fmt::format("❌ BinaryReader `{0}`: read() failed", fullPath));
			return 0;
		}

		readPos += readBytes;
		return readBytes;
	}

	int64 BinaryReader::BinaryReaderDetail::File::lookahead(const NonNull<void*> dst, const int64 readSize, const int64 fileSize, const FilePath& fullPath)
	{
		const int64 previousReadPos = readPos;
		const int64 readBytes = read(dst, readSize, fileSize, fullPath);
		readPos = previousReadPos;
		file.seekg(previousReadPos);
		return readBytes;
	}
}
