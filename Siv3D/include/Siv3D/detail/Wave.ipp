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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr Wave::Wave(const Arg::sampleRate_<uint32> sampleRate)
		: m_sampleRate{ Clamp(*sampleRate, MinSampleRate, MaxSampleRate) } {}

	constexpr Wave::Wave(const container_type& other, const Arg::sampleRate_<uint32> sampleRate)
		: m_container{ other }
		, m_sampleRate{ Clamp(*sampleRate, MinSampleRate, MaxSampleRate) } {}

	constexpr Wave::Wave(container_type&& other, const Arg::sampleRate_<uint32> sampleRate)
		: m_container{ std::move(other) }
		, m_sampleRate{ Clamp(*sampleRate, MinSampleRate, MaxSampleRate) } {}

	constexpr Wave::Wave(const size_type count, const Arg::sampleRate_<uint32> sampleRate)
		: m_container(count)
		, m_sampleRate{ Clamp(*sampleRate, MinSampleRate, MaxSampleRate) } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	constexpr Wave& Wave::operator =(Wave&& other) noexcept
	{
		if (&other != this)
		{
			m_container		= std::move(other.m_container);
			m_sampleRate	= other.m_sampleRate;
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	constexpr const Wave::value_type& Wave::operator [](const size_type index) const& noexcept
	{
		return m_container[index];
	}

	constexpr Wave::value_type& Wave::operator [](const size_type index) & noexcept
	{
		return m_container[index];
	}

	constexpr Wave::value_type Wave::operator [](const size_type index) && noexcept
	{
		return m_container[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	front
	//
	////////////////////////////////////////////////////////////////

	constexpr Wave::reference Wave::front() & noexcept
	{
		return m_container.front();
	}

	constexpr Wave::const_reference Wave::front() const& noexcept
	{
		return m_container.front();
	}

	constexpr Wave::value_type Wave::front() && noexcept
	{
		return m_container.front();
	}

	////////////////////////////////////////////////////////////////
	//
	//	back
	//
	////////////////////////////////////////////////////////////////

	constexpr Wave::reference Wave::back() & noexcept
	{
		return m_container.back();
	}

	constexpr Wave::const_reference Wave::back() const& noexcept
	{
		return m_container.back();
	}

	constexpr Wave::value_type Wave::back() && noexcept
	{
		return m_container.back();
	}

	////////////////////////////////////////////////////////////////
	//
	//	data
	//
	////////////////////////////////////////////////////////////////

	constexpr Wave::value_type* Wave::data() noexcept
	{
		return m_container.data();
	}

	constexpr const Wave::value_type* Wave::data() const noexcept
	{
		return m_container.data();
	}

	////////////////////////////////////////////////////////////////
	//
	//	begin, end
	//
	////////////////////////////////////////////////////////////////

	constexpr Wave::iterator Wave::begin() noexcept
	{
		return m_container.begin();
	}

	constexpr Wave::iterator Wave::end() noexcept
	{
		return m_container.end();
	}

	constexpr Wave::const_iterator Wave::begin() const noexcept
	{
		return m_container.begin();
	}

	constexpr Wave::const_iterator Wave::end() const noexcept
	{
		return m_container.end();
	}

	////////////////////////////////////////////////////////////////
	//
	//	cbegin, cend
	//
	////////////////////////////////////////////////////////////////

	constexpr Wave::const_iterator Wave::cbegin() const noexcept
	{
		return m_container.cbegin();
	}

	constexpr Wave::const_iterator Wave::cend() const noexcept
	{
		return m_container.cend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	rbegin, rend
	//
	////////////////////////////////////////////////////////////////

	constexpr Wave::reverse_iterator Wave::rbegin() noexcept
	{
		return m_container.rbegin();
	}

	constexpr Wave::reverse_iterator Wave::rend() noexcept
	{
		return m_container.rend();
	}

	constexpr Wave::const_reverse_iterator Wave::rbegin() const noexcept
	{
		return m_container.rbegin();
	}

	constexpr Wave::const_reverse_iterator Wave::rend() const noexcept
	{
		return m_container.rend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	crbegin, crend
	//
	////////////////////////////////////////////////////////////////

	constexpr Wave::const_reverse_iterator Wave::crbegin() const noexcept
	{
		return m_container.crbegin();
	}

	constexpr Wave::const_reverse_iterator Wave::crend() const noexcept
	{
		return m_container.crend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	empty, isEmpty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Wave::empty() const noexcept
	{
		return m_container.empty();
	}

	constexpr bool Wave::isEmpty() const noexcept
	{
		return m_container.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	constexpr Wave::operator bool() const noexcept
	{
		return (not m_container.empty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	samples
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t Wave::samples() const noexcept
	{
		return m_container.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	ssamples
	//
	////////////////////////////////////////////////////////////////

	constexpr isize Wave::ssamples() const noexcept
	{
		return m_container.ssize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t Wave::size() const noexcept
	{
		return m_container.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	ssize
	//
	////////////////////////////////////////////////////////////////

	constexpr isize Wave::ssize() const noexcept
	{
		return m_container.ssize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	size_bytes
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t Wave::size_bytes() const noexcept
	{
		return m_container.size_bytes();
	}

	////////////////////////////////////////////////////////////////
	//
	//	duration
	//
	////////////////////////////////////////////////////////////////

	constexpr Duration Wave::duration() const noexcept
	{
		return SecondsF{ static_cast<double>(m_container.size()) / m_sampleRate };
	}

	////////////////////////////////////////////////////////////////
	//
	//	sampleRate
	//
	////////////////////////////////////////////////////////////////

	constexpr uint32 Wave::sampleRate() const noexcept
	{
		return m_sampleRate;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSampleRate
	//
	////////////////////////////////////////////////////////////////

	constexpr void Wave::setSampleRate(const uint32 sampleRate) noexcept
	{
		m_sampleRate = Clamp(sampleRate, MinSampleRate, MaxSampleRate);
	}

	////////////////////////////////////////////////////////////////
	//
	//	reserve
	//
	////////////////////////////////////////////////////////////////

	constexpr void Wave::reserve(const size_type n)
	{
		m_container.reserve(n);
	}




	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	constexpr void Wave::clear() noexcept
	{
		m_container.clear();
	}



	////////////////////////////////////////////////////////////////
	//
	//	resize
	//
	////////////////////////////////////////////////////////////////

	constexpr void Wave::resize(const size_type count)
	{
		m_container.resize(count, value_type::Zero());
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	constexpr void Wave::swap(Wave& other) noexcept
	{
		m_container.swap(other.m_container);
		std::ranges::swap(m_sampleRate, other.m_sampleRate);
	}
}
