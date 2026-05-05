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

		static int64 ClampReadSize(const int64 requested, const int64 available) noexcept
		{
			if ((requested <= 0) || (available <= 0))
			{
				return 0;
			}

			return Min(requested, available);
		}

		static const FilePath EmptyFilePath;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	BinaryFileReader::BinaryFileReader()
		: pImpl{ std::make_unique<BinaryFileReaderDetail>() } {}

	BinaryFileReader::BinaryFileReader(const FilePathView path)
		: BinaryFileReader{}
	{
		open(path);
	}

	BinaryFileReader::BinaryFileReader(BinaryFileReader&& other) noexcept = default;

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	BinaryFileReader::~BinaryFileReader() = default;

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	BinaryFileReader& BinaryFileReader::operator =(BinaryFileReader&& other) noexcept = default;

	////////////////////////////////////////////////////////////////
	//
	//	open
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::open(const FilePathView path)
	{
		if (not pImpl)
		{
			pImpl = std::make_unique<BinaryFileReaderDetail>();
		}

		return pImpl->open(path);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	close
	//
	////////////////////////////////////////////////////////////////

	void BinaryFileReader::close()
	{
		if (pImpl)
		{
			pImpl->close();
		}
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	isOpen
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::isOpen() const noexcept
	{
		return (pImpl && pImpl->isOpen());
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	BinaryFileReader::operator bool() const noexcept
	{
		return isOpen();
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::size() const
	{
		return (pImpl ? pImpl->size() : 0);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	getPos
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::getPos() const
	{
		return (pImpl ? pImpl->getPos() : 0);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	remaining
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::remaining() const noexcept
	{
		return Max<int64>((size() - getPos()), 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEOF
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::isEOF() const noexcept
	{
		return (remaining() == 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::setPos(const int64 pos)
	{
		return (pImpl ? pImpl->setPos(pos) : 0);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	skip
	//
	////////////////////////////////////////////////////////////////

	int64 BinaryFileReader::skip(const int64 offset)
	{
		return (pImpl ? pImpl->skip(offset) : 0);
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

		if (not pImpl)
		{
			return 0;
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

		const int64 fileSize = size();

		if (not InRange<int64>(pos, 0, fileSize))
		{
			ThrowReadRangeError(pos, fileSize);
		}

		if (not pImpl)
		{
			return 0;
		}

		return pImpl->read(NonNull{ dst }, pos, readSize);
	}

	////////////////////////////////////////////////////////////////
	//
	//	readExact
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::readExact(void* const dst, const int64 size)
	{
		return (read(dst, size) == size);
	}

	bool BinaryFileReader::readExact(void* const dst, const int64 pos, const int64 size)
	{
		return (read(dst, pos, size) == size);
	}

	////////////////////////////////////////////////////////////////
	//
	//	readBlob
	//
	////////////////////////////////////////////////////////////////

	Blob BinaryFileReader::readBlob()
	{
		const int64 toReadBytes = remaining();

		if (toReadBytes == 0)
		{
			return Blob{};
		}

		Blob blob(toReadBytes);

		const int64 readBytes = pImpl->read(NonNull{ static_cast<void*>(blob.data()) }, toReadBytes);
		blob.resize(readBytes);

		return blob;
	}

	Blob BinaryFileReader::readBlob(const int64 requestedSize)
	{
		const int64 toReadBytes = ClampReadSize(requestedSize, remaining());

		if (toReadBytes == 0)
		{
			return Blob{};
		}

		Blob blob(toReadBytes);

		const int64 readBytes = pImpl->read(NonNull{ static_cast<void*>(blob.data()) }, toReadBytes);
		blob.resize(readBytes);

		return blob;
	}

	Blob BinaryFileReader::readBlob(const int64 pos, const int64 requestedSize)
	{
		const int64 fileSize = size();

		if (not InRange<int64>(pos, 0, fileSize))
		{
			ThrowReadRangeError(pos, fileSize);
		}

		const int64 toReadBytes = ClampReadSize(requestedSize, (fileSize - pos));

		if (toReadBytes == 0)
		{
			return Blob{};
		}

		Blob blob(toReadBytes);

		const int64 readBytes = pImpl->read(NonNull{ static_cast<void*>(blob.data()) }, pos, toReadBytes);
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

		if (not pImpl)
		{
			return 0;
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

		const int64 fileSize = size();

		if (not InRange<int64>(pos, 0, fileSize))
		{
			ThrowLookaheadRangeError(pos, fileSize);
		}

		if (not pImpl)
		{
			return 0;
		}

		return pImpl->lookahead(NonNull{ dst }, pos, readSize);
	}

	////////////////////////////////////////////////////////////////
	//
	//	lookaheadExact
	//
	////////////////////////////////////////////////////////////////

	bool BinaryFileReader::lookaheadExact(void* const dst, const int64 size) const
	{
		return (lookahead(dst, size) == size);
	}

	bool BinaryFileReader::lookaheadExact(void* const dst, const int64 pos, const int64 size) const
	{
		return (lookahead(dst, pos, size) == size);
	}

	////////////////////////////////////////////////////////////////
	//
	//	lookaheadBlob
	//
	////////////////////////////////////////////////////////////////

	Blob BinaryFileReader::lookaheadBlob(const int64 requestedSize) const
	{
		const int64 toReadBytes = ClampReadSize(requestedSize, remaining());

		if (toReadBytes == 0)
		{
			return Blob{};
		}

		Blob blob(toReadBytes);

		const int64 readBytes = pImpl->lookahead(NonNull{ static_cast<void*>(blob.data()) }, toReadBytes);
		blob.resize(readBytes);

		return blob;
	}

	Blob BinaryFileReader::lookaheadBlob(const int64 pos, const int64 requestedSize) const
	{
		const int64 fileSize = size();

		if (not InRange<int64>(pos, 0, fileSize))
		{
			ThrowLookaheadRangeError(pos, fileSize);
		}

		const int64 toReadBytes = ClampReadSize(requestedSize, (fileSize - pos));

		if (toReadBytes == 0)
		{
			return Blob{};
		}

		Blob blob(toReadBytes);

		const int64 readBytes = pImpl->lookahead(NonNull{ static_cast<void*>(blob.data()) }, pos, toReadBytes);
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
		return (pImpl ? pImpl->path() : EmptyFilePath);
	}
}
