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

	inline MultiPolygon::MultiPolygon(const container_type& other)
		: m_polygons(other) {}

	inline MultiPolygon::MultiPolygon(container_type&& other)
		: m_polygons(std::move(other)) {}

	inline MultiPolygon::MultiPolygon(size_type count)
		: m_polygons(count) {}

	template <std::input_iterator Iterator>
	inline MultiPolygon::MultiPolygon(Iterator first, Iterator last)
		: m_polygons(first, last) {}

	inline MultiPolygon::MultiPolygon(std::initializer_list<value_type> init)
		: m_polygons(init) {}

	inline MultiPolygon::MultiPolygon(Arg::reserve_<size_type> size)
		: m_polygons(size) {}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon& MultiPolygon::operator =(const MultiPolygon& other)
	{
		m_polygons = other.m_polygons;
		return *this;
	}

	inline MultiPolygon& MultiPolygon::operator =(MultiPolygon&& other) noexcept
	{
		m_polygons = std::move(other.m_polygons);
		return *this;
	}

	inline MultiPolygon& MultiPolygon::operator =(const container_type& other)
	{
		m_polygons = other;
		return *this;
	}

	inline MultiPolygon& MultiPolygon::operator =(container_type&& other) noexcept
	{
		m_polygons = std::move(other);
		return *this;
	}





	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	inline const MultiPolygon::value_type& MultiPolygon::operator [](const size_type index) const& noexcept
	{
		return m_polygons[index];
	}

	inline MultiPolygon::value_type& MultiPolygon::operator [](const size_type index) & noexcept
	{
		return m_polygons[index];
	}

	inline MultiPolygon::value_type MultiPolygon::operator [](const size_type index) && noexcept
	{
		return std::move(m_polygons[index]);
	}

	////////////////////////////////////////////////////////////////
	//
	//	front
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon::reference MultiPolygon::front() & noexcept
	{
		return m_polygons.front();
	}

	inline MultiPolygon::const_reference MultiPolygon::front() const& noexcept
	{
		return m_polygons.front();
	}

	inline MultiPolygon::value_type MultiPolygon::front() && noexcept
	{
		return std::move(m_polygons.front());
	}

	////////////////////////////////////////////////////////////////
	//
	//	back
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon::reference MultiPolygon::back() & noexcept
	{
		return m_polygons.back();
	}

	inline MultiPolygon::const_reference MultiPolygon::back() const& noexcept
	{
		return m_polygons.back();
	}

	inline MultiPolygon::value_type MultiPolygon::back() && noexcept
	{
		return std::move(m_polygons.back());
	}

	////////////////////////////////////////////////////////////////
	//
	//	data
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon::value_type* MultiPolygon::data() noexcept
	{
		return m_polygons.data();
	}

	inline const MultiPolygon::value_type* MultiPolygon::data() const noexcept
	{
		return m_polygons.data();
	}

	////////////////////////////////////////////////////////////////
	//
	//	begin, end
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon::iterator MultiPolygon::begin() noexcept
	{
		return m_polygons.begin();
	}

	inline MultiPolygon::iterator MultiPolygon::end() noexcept
	{
		return m_polygons.end();
	}

	inline MultiPolygon::const_iterator MultiPolygon::begin() const noexcept
	{
		return m_polygons.begin();
	}

	inline MultiPolygon::const_iterator MultiPolygon::end() const noexcept
	{
		return m_polygons.end();
	}

	////////////////////////////////////////////////////////////////
	//
	//	cbegin, cend
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon::const_iterator MultiPolygon::cbegin() const noexcept
	{
		return m_polygons.cbegin();
	}

	inline MultiPolygon::const_iterator MultiPolygon::cend() const noexcept
	{
		return m_polygons.cend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	rbegin, rend
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon::reverse_iterator MultiPolygon::rbegin() noexcept
	{
		return m_polygons.rbegin();
	}

	inline MultiPolygon::reverse_iterator MultiPolygon::rend() noexcept
	{
		return m_polygons.rend();
	}

	inline MultiPolygon::const_reverse_iterator MultiPolygon::rbegin() const noexcept
	{
		return m_polygons.rbegin();
	}

	inline MultiPolygon::const_reverse_iterator MultiPolygon::rend() const noexcept
	{
		return m_polygons.rend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	crbegin, crend
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon::const_reverse_iterator MultiPolygon::crbegin() const noexcept
	{
		return m_polygons.crbegin();
	}

	inline MultiPolygon::const_reverse_iterator MultiPolygon::crend() const noexcept
	{
		return m_polygons.crend();
	}



	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	inline size_t MultiPolygon::size() const noexcept
	{
		return m_polygons.size();
	}






	////////////////////////////////////////////////////////////////
	//
	//	append_range
	//
	////////////////////////////////////////////////////////////////

	template <Concept::ContainerCompatibleRange<Polygon> Range>
	void MultiPolygon::append_range(Range&& range)
	{
		m_polygons.append_range(std::forward<Range>(range));
	}
}
