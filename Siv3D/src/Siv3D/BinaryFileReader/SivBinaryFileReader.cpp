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

# include <Siv3D/BinaryFileReader.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/Blob.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/BinaryFileReader/BinaryFileReaderDetail.hpp>

namespace s3d
{
	namespace
	{
		[[noreturn]]
		static void ThrowReadDstError()
		{
			throw Error{ "BinaryFileReader::read(): A non-null destination pointer is required when readSize is greater than 0." };
		}

		[[noreturn]]
		static void ThrowReadRangeError(const int64 pos, const int64 fileSize)
		{
			throw Error{ fmt::format("BinaryFileReader::read(): Position ({0}) is out of the valid range. The file size is {1} bytes.", pos, fileSize) };
		}

		[[noreturn]]
		static void ThrowLookaheadDstError()
		{
			throw Error{ "BinaryFileReader::lookahead(): A non-null destination pointer is required when readSize is greater than 0." };
		}

		[[noreturn]]
		static void ThrowLookaheadRangeError(const int64 pos, const int64 fileSize)
		{
			throw Error{ fmt::format("BinaryFileReader::lookahead(): Position ({0}) is out of the valid range. The file size is {1} bytes.", pos, fileSize) };
		}
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	BinaryFileReader::BinaryFileReader()
		: pImpl{ std::make_shared<BinaryFileReaderDetail>() } {}

	BinaryFileReader::BinaryFileReader(const FilePathView path)
		: BinaryFileReader{}
	{
		open(path);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	open
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::open(const FilePathView path)
	{
		return pImpl->open(path);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	close
	//
	////////////////////////////////////////////////////////////////

	void BinaryFileReader::close()
	{
		pImpl->close();
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	isOpen
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::isOpen() const noexcept
	{
		return pImpl->isOpen();
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	BinaryFileReader::operator bool() const noexcept
	{
		return pImpl->isOpen();
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::size() const
	{
		return pImpl->size();
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	getPos
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::getPos() const
	{
		return pImpl->getPos();
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::setPos(const int64 pos)
	{
		return pImpl->setPos(pos);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	skip
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::skip(const int64 offset)
	{
		return pImpl->skip(offset);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	read
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::read(void* const dst, const int64 readSize)
	{
		if (readSize <= 0)
		{
			return 0;
		}

		if (dst == nullptr)
		{
			ThrowReadDstError();
		}

		return pImpl->read(NonNull{ dst }, readSize);
	}

	int64 BinaryFileReader::read(void* const dst, const int64 pos, const int64 readSize)
	{
		if (readSize <= 0)
		{
			return 0;
		}

		if (dst == nullptr)
		{
			ThrowReadDstError();
		}

		if (const int64 fileSize = pImpl->size();
			not InRange<int64>(pos, 0, fileSize))
		{
			ThrowReadRangeError(pos, fileSize);
		}

		return pImpl->read(NonNull{ dst }, pos, readSize);
	}

	////////////////////////////////////////////////////////////////
	//
	//	readBlob
	//
	////////////////////////////////////////////////////////////////

	Blob BinaryFileReader::readBlob()
	{
		const int64 toReadBytes = (size() - getPos());
		Blob blob(toReadBytes);

		const int64 readBytes = read(blob.data(), toReadBytes);
		blob.resize(readBytes);

		return blob;
	}

	Blob BinaryFileReader::readBlob(const int64 _size)
	{
		const int64 toReadBytes = Min(_size, (size() - getPos()));
		Blob blob(toReadBytes);

		const int64 readBytes = read(blob.data(), toReadBytes);
		blob.resize(readBytes);

		return blob;
	}

	Blob BinaryFileReader::readBlob(const int64 pos, const int64 _size)
	{
		const int64 fileSize = pImpl->size();

		if (not InRange<int64>(pos, 0, fileSize))
		{
			ThrowReadRangeError(pos, fileSize);
		}

		const int64 toReadBytes = Min(_size, (fileSize - pos));
		Blob blob(toReadBytes);
		
		const int64 readBytes = read(blob.data(), pos, toReadBytes);
		blob.resize(readBytes);
		
		return blob;
	}

	////////////////////////////////////////////////////////////////
	//
	//	lookahead
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::lookahead(void* const dst, const int64 readSize) const
	{
		if (readSize <= 0)
		{
			return 0;
		}

		if (dst == nullptr)
		{
			ThrowLookaheadDstError();
		}

		return pImpl->lookahead(NonNull{ dst }, readSize);
	}

	int64 BinaryFileReader::lookahead(void* const dst, const int64 pos, const int64 readSize) const
	{
		if (readSize <= 0)
		{
			return 0;
		}

		if (dst == nullptr)
		{
			ThrowLookaheadDstError();
		}

		if (const int64 fileSize = pImpl->size();
			not InRange<int64>(pos, 0, fileSize))
		{
			ThrowLookaheadRangeError(pos, fileSize);
		}

		return pImpl->lookahead(NonNull{ dst }, pos, readSize);
	}

	////////////////////////////////////////////////////////////////
	//
	//	lookaheadBlob
	//
	////////////////////////////////////////////////////////////////

	Blob BinaryFileReader::lookaheadBlob(const int64 _size)
	{
		const int64 toReadBytes = Min(_size, (size() - getPos()));
		Blob blob(toReadBytes);

		const int64 readBytes = lookahead(blob.data(), toReadBytes);
		blob.resize(readBytes);

		return blob;
	}

	Blob BinaryFileReader::lookaheadBlob(const int64 pos, const int64 _size)
	{
		const int64 fileSize = pImpl->size();
		
		if (not InRange<int64>(pos, 0, fileSize))
		{
			ThrowLookaheadRangeError(pos, fileSize);
		}
		
		const int64 toReadBytes = Min(_size, (fileSize - pos));
		Blob blob(toReadBytes);
		
		const int64 readBytes = lookahead(blob.data(), pos, toReadBytes);
		blob.resize(readBytes);
		
		return blob;
	}

	////////////////////////////////////////////////////////////////
	//
	//	path
	//
	////////////////////////////////////////////////////////////////

	const FilePath& BinaryFileReader::path() const noexcept
	{
		return pImpl->path();
	}
}
