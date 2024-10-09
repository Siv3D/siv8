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

	constexpr LineString::LineString(const container_type& other)
		: m_points{ other } {}

	constexpr LineString::LineString(container_type&& other)
		: m_points{ std::move(other) } {}

	constexpr LineString::LineString(const size_type count, const value_type& value)
		: m_points(count, value) {}

	constexpr LineString::LineString(const size_type count)
		: m_points(count) {}

	template <class Iterator>
	constexpr LineString::LineString(Iterator first, Iterator last)
		: m_points(first, last) {}

	LineString::LineString(const Array<Point>& points)
		: m_points(points.begin(), points.end()) {}

	constexpr LineString::LineString(const HasAsArray auto& a)
		: m_points{ a.asArray() } {}

	constexpr LineString::LineString(HasAsArray auto&& a)
		: m_points{ std::forward<decltype(a)>(a).asArray() } {}

	constexpr LineString::LineString(std::initializer_list<value_type> list)
		: m_points(list) {}

# ifdef __cpp_lib_containers_ranges

	template <Concept::ContainerCompatibleRange<Vec2> Range>
	constexpr LineString::LineString(std::from_range_t, Range&& range)
		: m_points(std::from_range, std::forward<Range>(range)) {}

	template <Concept::ContainerCompatibleRange<Point> Range>
	constexpr LineString::LineString(std::from_range_t, Range&& range)
		: m_points(std::from_range, std::forward<Range>(range)) {}

# endif

	constexpr LineString::LineString(const Arg::reserve_<size_type> size)
		: m_points{ size } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::operator =(const container_type& other)
	{
		m_points = other;
		return *this;
	}

	constexpr LineString& LineString::operator =(container_type&& other)
	{
		m_points = std::move(other);
		return *this;
	}

	constexpr LineString& LineString::operator =(const Array<Point>& other)
	{
		m_points.assign(other.begin(), other.end());
		return *this;
	}

	constexpr LineString& LineString::operator =(const HasAsArray auto& a)
	{
		m_points = a.asArray();
		return *this;
	}

	constexpr LineString& LineString::operator =(HasAsArray auto&& a)
	{
		m_points = std::forward<decltype(a)>(a).asArray();
		return *this;
	}

	constexpr LineString& LineString::operator =(std::initializer_list<value_type> list)
	{
		m_points = list;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	assign
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::assign(const size_type count, const value_type& value)
	{
		m_points.assign(count, value);
		return *this;
	}

	template <class Iterator>
	constexpr LineString& LineString::assign(Iterator first, Iterator last)
	{
		m_points.assign(first, last);
		return *this;
	}

	constexpr LineString& LineString::assign(std::initializer_list<value_type> list)
	{
		m_points.assign(list);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	assign_range
	//
	////////////////////////////////////////////////////////////////

	template <Concept::ContainerCompatibleRange<Vec2> Range>
	constexpr LineString& LineString::assign_range(Range&& range)
	{
		m_points.assign_range(std::forward<Range>(range));
		return *this;
	}

	template <Concept::ContainerCompatibleRange<Point> Range>
	constexpr LineString& LineString::assign_range(Range&& range)
	{
		m_points.assign_range(std::forward<Range>(range));
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	get_allocator
	//
	////////////////////////////////////////////////////////////////

	/// @brief アロケータを返します。
	/// @return アロケータ
	[[nodiscard]]
	constexpr LineString::allocator_type LineString::get_allocator() const noexcept
	{
		return m_points.get_allocator();
	}

	////////////////////////////////////////////////////////////////
	//
	//	asArray
	//
	////////////////////////////////////////////////////////////////

	constexpr const LineString::container_type& LineString::asArray() const& noexcept
	{
		return m_points;
	}

	constexpr LineString::container_type LineString::asArray() && noexcept
	{
		return std::move(m_points);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator container_type
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::operator container_type() const& noexcept
	{
		return m_points;
	}

	constexpr LineString::operator container_type() && noexcept
	{
		return std::move(m_points);
	}

	////////////////////////////////////////////////////////////////
	//
	//	at
	//
	////////////////////////////////////////////////////////////////

	constexpr const LineString::value_type& LineString::at(const size_type index) const&
	{
		return m_points.at(index);
	}

	constexpr LineString::value_type& LineString::at(const size_type index)&
	{
		return m_points.at(index);
	}

	constexpr LineString::value_type LineString::at(const size_type index)&&
	{
		return m_points.at(index);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	constexpr const LineString::value_type& LineString::operator [](const size_type index) const& noexcept
	{
		return m_points[index];
	}

	constexpr LineString::value_type& LineString::operator [](const size_type index) & noexcept
	{
		return m_points[index];
	}

	constexpr LineString::value_type LineString::operator [](const size_type index) && noexcept
	{
		return m_points[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	front
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::reference LineString::front() & noexcept
	{
		return m_points.front();
	}

	constexpr LineString::const_reference LineString::front() const& noexcept
	{
		return m_points.front();
	}

	constexpr LineString::value_type LineString::front() && noexcept
	{
		return m_points.front();
	}

	////////////////////////////////////////////////////////////////
	//
	//	back
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::reference LineString::back() & noexcept
	{
		return m_points.back();
	}

	constexpr LineString::const_reference LineString::back() const& noexcept
	{
		return m_points.back();
	}

	constexpr LineString::value_type LineString::back() && noexcept
	{
		return m_points.back();
	}

	////////////////////////////////////////////////////////////////
	//
	//	data
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::value_type* LineString::data() noexcept
	{
		return m_points.data();
	}

	constexpr const LineString::value_type* LineString::data() const noexcept
	{
		return m_points.data();
	}

	////////////////////////////////////////////////////////////////
	//
	//	begin, end
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::iterator LineString::begin() noexcept
	{
		return m_points.begin();
	}

	constexpr LineString::iterator LineString::end() noexcept
	{
		return m_points.end();
	}

	constexpr LineString::const_iterator LineString::begin() const noexcept
	{
		return m_points.begin();
	}

	constexpr LineString::const_iterator LineString::end() const noexcept
	{
		return m_points.end();
	}

	////////////////////////////////////////////////////////////////
	//
	//	cbegin, cend
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::const_iterator LineString::cbegin() const noexcept
	{
		return m_points.cbegin();
	}

	constexpr LineString::const_iterator LineString::cend() const noexcept
	{
		return m_points.cend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	rbegin, rend
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::reverse_iterator LineString::rbegin() noexcept
	{
		return m_points.rbegin();
	}

	constexpr LineString::reverse_iterator LineString::rend() noexcept
	{
		return m_points.rend();
	}

	constexpr LineString::const_reverse_iterator LineString::rbegin() const noexcept
	{
		return m_points.rbegin();
	}

	constexpr LineString::const_reverse_iterator LineString::rend() const noexcept
	{
		return m_points.rend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	crbegin, crend
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::const_reverse_iterator LineString::crbegin() const noexcept
	{
		return m_points.crbegin();
	}

	constexpr LineString::const_reverse_iterator LineString::crend() const noexcept
	{
		return m_points.crend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	empty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool LineString::empty() const noexcept
	{
		return m_points.isEmpty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool LineString::isEmpty() const noexcept
	{
		return m_points.isEmpty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	indexInBounds
	//
	////////////////////////////////////////////////////////////////

	constexpr bool LineString::indexInBounds(const size_type index) const noexcept
	{
		return (index < m_points.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::operator bool() const noexcept
	{
		return (not m_points.isEmpty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t LineString::size() const noexcept
	{
		return m_points.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	ssize
	//
	////////////////////////////////////////////////////////////////

	constexpr isize LineString::ssize() const noexcept
	{
		return m_points.ssize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	size_bytes
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t LineString::size_bytes() const noexcept
	{
		return m_points.size_bytes();
	}

	////////////////////////////////////////////////////////////////
	//
	//	max_size
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t LineString::max_size() const noexcept
	{
		return m_points.max_size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	reserve
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::reserve(const size_type n)
	{
		m_points.reserve(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	capacity
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t LineString::capacity() const noexcept
	{
		return m_points.capacity();
	}

	////////////////////////////////////////////////////////////////
	//
	//	shrink_to_fit
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::shrink_to_fit()
	{
		m_points.shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::clear() noexcept
	{
		m_points.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::release()
	{
		m_points.release();
	}

	////////////////////////////////////////////////////////////////
	//
	//	insert
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::iterator LineString::insert(const_iterator pos, const value_type& value)
	{
		return m_points.insert(pos, value);
	}

	constexpr LineString::iterator LineString::insert(const_iterator pos, const size_type count, const value_type& value)
	{
		return m_points.insert(pos, count, value);
	}

	template <class Iterator>
	constexpr LineString::iterator LineString::insert(const_iterator pos, Iterator first, Iterator last)
	{
		return m_points.insert(pos, first, last);
	}

	constexpr LineString::iterator LineString::insert(const_iterator pos, std::initializer_list<value_type> list)
	{
		return m_points.insert(pos, list);
	}

	////////////////////////////////////////////////////////////////
	//
	//	emplace
	//
	////////////////////////////////////////////////////////////////

	template <class... Args>
	constexpr LineString::iterator LineString::emplace(const_iterator pos, Args&&... args)
	{
		return m_points.emplace(pos, std::forward<Args>(args)...);
	}

	////////////////////////////////////////////////////////////////
	//
	//	append_range
	//
	////////////////////////////////////////////////////////////////

	template <Concept::ContainerCompatibleRange<Vec2> Range>
	constexpr void LineString::append_range(Range&& range)
	{
		m_points.append_range(std::forward<Range>(range));
	}

	template <Concept::ContainerCompatibleRange<Point> Range>
	constexpr void LineString::append_range(Range&& range)
	{
		m_points.append_range(std::forward<Range>(range));
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::iterator LineString::erase(const_iterator pos)
	{
		return m_points.erase(pos);
	}

	constexpr LineString::iterator LineString::erase(const_iterator first, const_iterator last)
	{
		return m_points.erase(first, last);
	}

	////////////////////////////////////////////////////////////////
	//
	//	push_back
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::push_back(const value_type& value)
	{
		m_points.push_back(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	emplace_back
	//
	////////////////////////////////////////////////////////////////

	template <class... Args>
	constexpr LineString::reference LineString::emplace_back(Args&&... args)
	{
		return m_points.emplace_back(std::forward<Args>(args)...);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::pop_back()
	{
		m_points.pop_back();
	}

	////////////////////////////////////////////////////////////////
	//
	//	resize
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::resize(const size_type count)
	{
		m_points.resize(count);
	}

	constexpr void LineString::resize(const size_type count, const value_type& value)
	{
		m_points.resize(count, value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::swap(LineString& other) noexcept
	{
		m_points.swap(other.m_points);
	}

	////////////////////////////////////////////////////////////////
	//
	//	push_front
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::push_front(const value_type& value)
	{
		m_points.push_front(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	emplace_front
	//
	////////////////////////////////////////////////////////////////

	template <class... Args>
	constexpr LineString::reference LineString::emplace_front(Args&&... args)
	{
		return m_points.emplace_front(std::forward<Args>(args)...);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_front
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::pop_front()
	{
		m_points.pop_front();
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_front_N
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::pop_front_N(const size_type n)
	{
		m_points.pop_front_N(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back_N
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::pop_back_N(const size_type n)
	{
		m_points.pop_back_N(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::operator <<(const value_type& value)
	{
		m_points << value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	subspan
	//
	////////////////////////////////////////////////////////////////

	constexpr std::span<LineString::value_type> LineString::subspan(const size_type pos, const size_type count) noexcept
	{
		return m_points.subspan(pos, count);
	}

	constexpr std::span<const LineString::value_type> LineString::subspan(const size_type pos, const size_type count) const noexcept
	{
		return m_points.subspan(pos, count);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 LineString::hash() const noexcept
	{
		return Hash(m_points.data(), m_points.size_bytes());
	}




	////////////////////////////////////////////////////////////////
	//
	//	operator >>
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr auto LineString::operator >>(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		using result_value_type = std::decay_t<std::invoke_result_t<Fty&, const value_type&>>;

		if constexpr (std::is_same_v<result_value_type, void>)
		{
			each(std::forward<Fty>(f));
		}
		else
		{
			return map(std::forward<Fty>(f));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	Generate
	//
	////////////////////////////////////////////////////////////////

	inline LineString LineString::Generate(const size_type size, FunctionRef<value_type()> generator)
	{
		LineString result(size);

		for (size_type i = 0; i < size; ++i)
		{
			result.m_points[i] = generator();
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	IndexedGenerate
	//
	////////////////////////////////////////////////////////////////

	inline LineString LineString::IndexedGenerate(const size_type size, FunctionRef<value_type(size_t)> generator)
	{
		LineString result(size);

		for (size_type i = 0; i < size; ++i)
		{
			result.m_points[i] = generator(i);
		}

		return result;
	}
}
