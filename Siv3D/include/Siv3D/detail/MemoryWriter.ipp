﻿//-----------------------------------------------
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

	constexpr MemoryWriter::MemoryWriter(const Arg::reserve_<size_t> reserveSizeBytes)
		: m_blob{ reserveSizeBytes } {}

	////////////////////////////////////////////////////////////////
	//
	//	isOpen
	//
	////////////////////////////////////////////////////////////////

	constexpr bool MemoryWriter::isOpen() const noexcept
	{
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	constexpr MemoryWriter::operator bool() const noexcept
	{
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	constexpr int64 MemoryWriter::size() const
	{
		return m_blob.ssize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPos
	//
	////////////////////////////////////////////////////////////////

	constexpr int64 MemoryWriter::getPos() const
	{
		return m_writePos;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	constexpr bool MemoryWriter::setPos(const int64 pos)
	{
		if (pos < 0)
		{
			return false;
		}

		m_writePos = pos;

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	seekToEnd
	//
	////////////////////////////////////////////////////////////////

	constexpr int64 MemoryWriter::seekToEnd()
	{
		m_writePos = m_blob.ssize();

		return m_writePos;
	}

	////////////////////////////////////////////////////////////////
	//
	//	write
	//
	////////////////////////////////////////////////////////////////

	inline int64 MemoryWriter::write(const void* src, const int64 sizeBytes)
	{
		assert(src || (sizeBytes == 0));

		if (sizeBytes == 0)
		{
			return 0;
		}

		if (m_writePos == m_blob.ssize()) // append
		{
			m_blob.append(src, sizeBytes);
		}
		else // overwrite
		{
			const size_t newSize = (static_cast<size_t>(m_writePos) + sizeBytes);

			m_blob.resize(newSize);

			std::memcpy((m_blob.data() + m_writePos), src, sizeBytes);
		}

		m_writePos += sizeBytes;

		return sizeBytes;
	}

	bool MemoryWriter::write(const Concept::TriviallyCopyable auto& src)
	{
		return (write(std::addressof(src), sizeof(src)) == sizeof(src));
	}

	////////////////////////////////////////////////////////////////
	//
	//	retrieve
	//
	////////////////////////////////////////////////////////////////

	constexpr Blob MemoryWriter::retrieve() noexcept
	{
		m_writePos = 0;
		return std::move(m_blob);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBlob
	//
	////////////////////////////////////////////////////////////////

	constexpr const Blob& MemoryWriter::getBlob() const noexcept
	{
		return m_blob;
	}

	////////////////////////////////////////////////////////////////
	//
	//	reserve
	//
	////////////////////////////////////////////////////////////////

	constexpr void MemoryWriter::reserve(const size_t sizeBytes)
	{
		m_blob.reserve(sizeBytes);
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	constexpr void MemoryWriter::clear()
	{
		m_blob.clear();
		m_writePos = 0;
	}

	////////////////////////////////////////////////////////////////
	//
	//	shrink_to_fit
	//
	////////////////////////////////////////////////////////////////

	constexpr void MemoryWriter::shrink_to_fit()
	{
		m_blob.shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	constexpr void MemoryWriter::release()
	{
		clear();
		shrink_to_fit();
	}
}