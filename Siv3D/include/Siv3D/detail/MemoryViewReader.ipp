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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr MemoryViewReader::MemoryViewReader(const void* data, const size_t size_bytes) noexcept
		: m_base{ static_cast<const Byte*>(data) }
		, m_size{ static_cast<int64>(size_bytes) }
		, m_pos{ 0 } {}

	////////////////////////////////////////////////////////////////
	//
	//	supportsLookahead
	//
	////////////////////////////////////////////////////////////////

	constexpr bool MemoryViewReader::supportsLookahead() const noexcept
	{
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isOpen
	//
	////////////////////////////////////////////////////////////////

	constexpr bool MemoryViewReader::isOpen() const noexcept
	{
		return (m_base != nullptr);
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	constexpr int64 MemoryViewReader::size() const
	{
		return m_size;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPos
	//
	////////////////////////////////////////////////////////////////

	constexpr int64 MemoryViewReader::getPos() const
	{
		return m_pos;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	constexpr int64 MemoryViewReader::setPos(const int64 pos)
	{
		return (m_pos = Clamp<int64>(pos, 0, m_size));
	}

	////////////////////////////////////////////////////////////////
	//
	//	skip
	//
	////////////////////////////////////////////////////////////////

	constexpr int64 MemoryViewReader::skip(const int64 offset)
	{
		return (m_pos = Clamp<int64>((m_pos + offset), 0, m_size));
	}

	////////////////////////////////////////////////////////////////
	//
	//	read
	//
	////////////////////////////////////////////////////////////////

	inline int64 MemoryViewReader::read(void* dst, const int64 size)
	{
		if (size <= 0)
		{
			return 0;
		}

		if (dst == nullptr)
		{
			ThrowReadDstError();
		}

		const int64 readSize = Min<int64>(size, (m_size - m_pos));

		if (readSize <= 0)
		{
			return 0;
		}

		std::memcpy(dst, (m_base + m_pos), static_cast<size_t>(readSize));

		m_pos += readSize;

		return readSize;
	}

	inline int64 MemoryViewReader::read(void* dst, int64 pos, const int64 size)
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

		pos = Min<int64>(pos, m_size);

		const int64 readSize = Min<int64>(size, (m_size - pos));

		if (readSize <= 0)
		{
			return 0;
		}

		std::memcpy(dst, (m_base + pos), static_cast<size_t>(readSize));

		m_pos = (pos + readSize);

		return readSize;
	}

	bool MemoryViewReader::read(Concept::TriviallyCopyable auto& dst)
	{
		return (read(std::addressof(dst), sizeof(dst)) == sizeof(dst));
	}

	////////////////////////////////////////////////////////////////
	//
	//	lookahead
	//
	////////////////////////////////////////////////////////////////

	inline int64 MemoryViewReader::lookahead(void* dst, const int64 size) const
	{
		if (size <= 0)
		{
			return 0;
		}

		if (dst == nullptr)
		{
			ThrowLookaheadDstError();
		}

		const int64 readSize = Min<int64>(size, (m_size - m_pos));

		if (readSize <= 0)
		{
			return 0;
		}

		std::memcpy(dst, (m_base + m_pos), static_cast<size_t>(readSize));

		return readSize;
	}

	inline int64 MemoryViewReader::lookahead(void* dst, int64 pos, const int64 size) const
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

		pos = Min<int64>(pos, m_size);

		const int64 readSize = Min<int64>(size, (m_size - pos));

		if (readSize <= 0)
		{
			return 0;
		}

		std::memcpy(dst, (m_base + pos), static_cast<size_t>(readSize));

		return readSize;
	}

	bool MemoryViewReader::lookahead(Concept::TriviallyCopyable auto& dst) const
	{
		return (lookahead(std::addressof(dst), sizeof(dst)) == sizeof(dst));
	}
}
