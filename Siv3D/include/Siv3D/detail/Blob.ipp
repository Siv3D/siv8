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

	constexpr Blob::Blob(const size_t sizeBytes)
		: m_data(sizeBytes) {}

	constexpr Blob::Blob(const Arg::reserve_<size_t> reserveSizeBytes)
		: m_data(reserveSizeBytes) {}

	inline Blob::Blob(const void* src, const size_t sizeBytes)
		: m_data(static_cast<const Byte*>(src), (static_cast<const Byte*>(src) + sizeBytes)) {}

	constexpr Blob::Blob(const Array<Byte>& data)
		: m_data{ data } {}

	constexpr Blob::Blob(Array<Byte>&& data)
		: m_data{ std::move(data) } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	constexpr Blob& Blob::operator =(const Array<Byte>& other)
	{
		m_data = other;
		return *this;
	}

	constexpr Blob& Blob::operator =(Array<Byte>&& other) noexcept
	{
		m_data = std::move(other);
		return *this;
	}

	constexpr Blob& Blob::operator =(const Blob& other)
	{
		m_data = other.m_data;
		return *this;
	}

	constexpr Blob& Blob::operator =(Blob&& other) noexcept
	{
		m_data = std::move(other.m_data);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	createFrom
	//
	////////////////////////////////////////////////////////////////

	constexpr void Blob::createFrom(const Blob& other)
	{
		m_data = other.m_data;
	}

	constexpr void Blob::createFrom(Blob&& other)
	{
		m_data = std::move(other.m_data);
	}

	inline void Blob::createFrom(const void* src, const size_t sizeBytes)
	{
		m_data.assign(static_cast<const Byte*>(src), (static_cast<const Byte*>(src) + sizeBytes));
	}

	constexpr void Blob::createFrom(const Array<Byte>& data)
	{
		m_data = data;
	}

	constexpr void Blob::createFrom(Array<Byte>&& data)
	{
		m_data = std::move(data);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	constexpr const Byte& Blob::operator[](const size_t index) const
	{
		return m_data[index];
	}

	constexpr Byte& Blob::operator[](const size_t index)
	{
		return m_data[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	data
	//
	////////////////////////////////////////////////////////////////

	constexpr const Byte* Blob::data() const noexcept
	{
		return m_data.data();
	}

	constexpr Byte* Blob::data() noexcept
	{
		return m_data.data();
	}

	////////////////////////////////////////////////////////////////
	//
	//	asArray
	//
	////////////////////////////////////////////////////////////////

	constexpr const Array<Byte>& Blob::asArray() const noexcept
	{
		return m_data;
	}

	////////////////////////////////////////////////////////////////
	//
	//	empty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Blob::empty() const noexcept
	{
		return m_data.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Blob::isEmpty() const noexcept
	{
		return m_data.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	constexpr Blob::operator bool() const noexcept
	{
		return (not m_data.empty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t Blob::size() const noexcept
	{
		return m_data.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	ssize
	//
	////////////////////////////////////////////////////////////////

	constexpr isize Blob::ssize() const noexcept
	{
		return m_data.ssize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	size_bytes
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t Blob::size_bytes() const noexcept
	{
		return m_data.size_bytes();
	}

	////////////////////////////////////////////////////////////////
	//
	//	capacity
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t Blob::capacity() const noexcept
	{
		return m_data.capacity();
	}

	////////////////////////////////////////////////////////////////
	//
	//	reserve
	//
	////////////////////////////////////////////////////////////////

	constexpr void Blob::reserve(const size_t sizeBytes)
	{
		m_data.reserve(sizeBytes);
	}

	////////////////////////////////////////////////////////////////
	//
	//	resize
	//
	////////////////////////////////////////////////////////////////

	constexpr void Blob::resize(const size_t sizeBytes)
	{
		m_data.resize(sizeBytes, Byte{ 0 });
	}

	////////////////////////////////////////////////////////////////
	//
	//	shrink_to_fit
	//
	////////////////////////////////////////////////////////////////

	constexpr void Blob::shrink_to_fit()
	{
		m_data.shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	constexpr void Blob::clear()
	{
		m_data.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	constexpr void Blob::release()
	{
		m_data.release();
	}

	////////////////////////////////////////////////////////////////
	//
	//	begin / end
	//
	////////////////////////////////////////////////////////////////

	constexpr Blob::iterator Blob::begin() noexcept
	{
		return m_data.begin();
	}

	constexpr Blob::iterator Blob::end() noexcept
	{
		return m_data.end();
	}

	constexpr Blob::const_iterator Blob::begin() const noexcept
	{
		return m_data.begin();
	}

	constexpr Blob::const_iterator Blob::end() const noexcept
	{
		return m_data.end();
	}

	////////////////////////////////////////////////////////////////
	//
	//	cbegin / cend
	//
	////////////////////////////////////////////////////////////////

	constexpr Blob::const_iterator Blob::cbegin() const noexcept
	{
		return m_data.cbegin();
	}

	constexpr Blob::const_iterator Blob::cend() const noexcept
	{
		return m_data.cend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	rbegin / rend
	//
	////////////////////////////////////////////////////////////////

	constexpr Blob::reverse_iterator Blob::rbegin() noexcept
	{
		return m_data.rbegin();
	}

	constexpr Blob::reverse_iterator Blob::rend() noexcept
	{
		return m_data.rend();
	}

	constexpr Blob::const_reverse_iterator Blob::rbegin() const noexcept
	{
		return m_data.rbegin();
	}

	constexpr Blob::const_reverse_iterator Blob::rend() const noexcept
	{
		return m_data.rend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	crbegin / crend
	//
	////////////////////////////////////////////////////////////////

	constexpr Blob::const_reverse_iterator Blob::crbegin() const noexcept
	{
		return m_data.crbegin();
	}

	constexpr Blob::const_reverse_iterator Blob::crend() const noexcept
	{
		return m_data.crend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	push_back
	//
	////////////////////////////////////////////////////////////////

	constexpr void Blob::push_back(const Byte value)
	{
		m_data.push_back(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back
	//
	////////////////////////////////////////////////////////////////

	constexpr void Blob::pop_back()
	{
		m_data.pop_back();
	}

	////////////////////////////////////////////////////////////////
	//
	//	append
	//
	////////////////////////////////////////////////////////////////

	constexpr void Blob::append(const Blob& other)
	{
		m_data.append(other.m_data);
	}

	inline void Blob::append(const void* src, const size_t sizeBytes)
	{
		m_data.insert(m_data.end(), static_cast<const Byte*>(src), (static_cast<const Byte*>(src) + sizeBytes));
	}

	////////////////////////////////////////////////////////////////
	//
	//	insert
	//
	////////////////////////////////////////////////////////////////

	constexpr Blob::iterator Blob::insert(const_iterator pos, const Byte value)
	{
		return m_data.insert(pos, value);
	}

	template <class Iterator>
	constexpr Blob::iterator Blob::insert(const_iterator pos, Iterator first, Iterator last)
	{
		return m_data.insert(pos, first, last);
	}
}
