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

# include <Siv3D/MemoryReader.hpp>
# include <Siv3D/Error.hpp>

namespace s3d
{
	namespace
	{
		[[noreturn]]
		static void ThrowReadDstError()
		{
			throw Error{ "MemoryReader::read(): A non-null destination pointer is required when readSize is greater than 0." };
		}

		[[noreturn]]
		static void ThrowReadPosError()
		{
			throw Error{ "MemoryReader::read(): `pos` must be a non-negative value." };
		}

		[[noreturn]]
		static void ThrowLookaheadDstError()
		{
			throw Error{ "MemoryReader::lookahead(): A non-null destination pointer is required when readSize is greater than 0." };
		}

		[[noreturn]]
		static void ThrowLookaheadPosError()
		{
			throw Error{ "MemoryReader::lookahead(): `pos` must be a non-negative value." };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	read
	//
	////////////////////////////////////////////////////////////////

	int64 MemoryReader::read(void* dst, const int64 size)
	{
		if (size <= 0)
		{
			return 0;
		}

		if (dst == nullptr)
		{
			ThrowReadDstError();
		}

		const int64 readSize = Min<int64>(size, (m_blob.ssize() - m_pos));

		if (readSize == 0)
		{
			return 0;
		}

		std::memcpy(dst, (m_blob.data() + m_pos), static_cast<size_t>(readSize));

		m_pos += readSize;

		return readSize;
	}

	int64 MemoryReader::read(void* dst, int64 pos, const int64 size)
	{
		if (size <= 0)
		{
			return 0;
		}

		if (dst == nullptr)
		{
			ThrowReadDstError();
		}

		if (pos < 0)
		{
			ThrowReadPosError();
		}

		pos = Min<int64>(pos, m_blob.ssize());

		const int64 readSize = Min<int64>(size, (m_blob.ssize() - pos));

		if (readSize == 0)
		{
			return 0;
		}

		std::memcpy(dst, (m_blob.data() + pos), static_cast<size_t>(readSize));

		m_pos = (pos + readSize);

		return readSize;
	}

	////////////////////////////////////////////////////////////////
	//
	//	lookahead
	//
	////////////////////////////////////////////////////////////////

	int64 MemoryReader::lookahead(void* dst, const int64 size) const
	{
		if (size <= 0)
		{
			return 0;
		}

		if (dst == nullptr)
		{
			ThrowLookaheadDstError();
		}

		const int64 readSize = Min<int64>(size, (m_blob.ssize() - m_pos));

		if (readSize == 0)
		{
			return 0;
		}

		std::memcpy(dst, (m_blob.data() + m_pos), static_cast<size_t>(readSize));

		return readSize;
	}

	int64 MemoryReader::lookahead(void* dst, int64 pos, const int64 size) const
	{
		if (size <= 0)
		{
			return 0;
		}

		if (dst == nullptr)
		{
			ThrowLookaheadDstError();
		}

		if (pos < 0)
		{
			ThrowLookaheadPosError();
		}

		pos = Min<int64>(pos, m_blob.ssize());

		const int64 readSize = Min<int64>(size, (m_blob.ssize() - pos));

		if (readSize == 0)
		{
			return 0;
		}

		std::memcpy(dst, (m_blob.data() + pos), static_cast<size_t>(readSize));

		return readSize;
	}
}
