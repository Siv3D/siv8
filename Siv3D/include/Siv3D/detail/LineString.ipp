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

	constexpr LineString::LineString(const container_type& other)
		: m_points{ other } {}

	constexpr LineString::LineString(container_type&& other)
		: m_points{ std::move(other) } {}

	constexpr LineString::LineString(const size_type count, const value_type& value)
		: m_points(count, value) {}

	constexpr LineString::LineString(const size_type count)
		: m_points(count) {}

	template <std::input_iterator Iterator>
	constexpr LineString::LineString(Iterator first, Iterator last)
		: m_points(first, last) {}

	constexpr LineString::LineString(const Array<Point>& points)
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

	template <std::input_iterator Iterator>
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

	template <std::input_iterator Iterator>
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
	//	insert_range
	//
	////////////////////////////////////////////////////////////////

	template <Concept::ContainerCompatibleRange<Vec2> Range>
	constexpr LineString::iterator LineString::insert_range(const_iterator pos, Range&& range)
	{
		return m_points.insert_range(pos, std::forward<Range>(range));
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
	//	erase_at
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::erase_at(const size_type index)&
	{
		m_points.erase_at(index);
		return *this;
	}

	constexpr LineString LineString::erase_at(const size_type index)&&
	{
		erase_at(index);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_all
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::size_type LineString::erase_all(const value_type& value)
	{
		return m_points.erase_all(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_first
	//
	////////////////////////////////////////////////////////////////

	constexpr bool LineString::erase_first(const value_type& value)
	{
		return m_points.erase_first(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_all_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr LineString::size_type LineString::erase_all_if(Fty f)
		requires std::predicate<Fty&, const value_type&>
	{
		return m_points.erase_all_if(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_first_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr bool LineString::erase_first_if(Fty f)
		requires std::predicate<Fty&, const value_type&>
	{
		return m_points.erase_first_if(std::forward<Fty>(f));
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
	//	all
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr bool LineString::all(Fty f) const
		requires std::predicate<Fty&, const value_type&>
	{
		return m_points.all(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	any
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr bool LineString::any(Fty f) const
		requires std::predicate<Fty&, const value_type&>
	{
		return m_points.any(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	append
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::append(const LineString& other)
	{
		m_points.append(other.m_points);
		return *this;
	}

	constexpr LineString& LineString::append(const container_type& other)
	{
		m_points.append(other);
		return *this;
	}

	template <std::input_iterator Iterator>
	constexpr LineString& LineString::append(Iterator first, Iterator last)
	{
		m_points.append(first, last);
		return *this;
	}

	constexpr LineString& LineString::append(std::initializer_list<value_type> list)
	{
		m_points.append(list);
		return *this;
	}

	constexpr LineString& LineString::append(const size_type count, const value_type& value)
	{
		m_points.append(count, value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains
	//
	////////////////////////////////////////////////////////////////

	constexpr bool LineString::contains(const value_type& value) const
	{
		return m_points.contains(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr bool LineString::contains_if(Fty f) const
		requires std::predicate<Fty&, const value_type&>
	{
		return m_points.contains_if(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	count
	//
	////////////////////////////////////////////////////////////////

	constexpr isize LineString::count(const value_type& value) const
	{
		return m_points.count(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	count_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr isize LineString::count_if(Fty f) const
		requires std::predicate<Fty&, const value_type&>
	{
		return m_points.count_if(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	each
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr void LineString::each(Fty f)
		requires std::invocable<Fty&, value_type&>
	{
		m_points.each(std::forward<Fty>(f));
	}

	template <class Fty>
	constexpr void LineString::each(Fty f) const
		requires std::invocable<Fty&, const value_type&>
	{
		m_points.each(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	each_index
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr void LineString::each_index(Fty f)
		requires std::invocable<Fty&, size_t, value_type&>
	{
		m_points.each_index(std::forward<Fty>(f));
	}

	template <class Fty>
	constexpr void LineString::each_index(Fty f) const
		requires std::invocable<Fty&, size_t, const value_type&>
	{
		m_points.each_index(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	each_sindex
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr void LineString::each_sindex(Fty f)
		requires std::invocable<Fty&, isize, value_type&>
	{
		m_points.each_sindex(std::forward<Fty>(f));
	}

	template <class Fty>
	constexpr void LineString::each_sindex(Fty f) const
		requires std::invocable<Fty&, isize, const value_type&>
	{
		m_points.each_sindex(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	fetch
	//
	////////////////////////////////////////////////////////////////

	template <class U>
	constexpr LineString::value_type LineString::fetch(const size_type index, U&& defaultValue) const
		noexcept(std::is_nothrow_constructible_v<value_type, U> && std::is_nothrow_copy_constructible_v<value_type>)
		requires std::constructible_from<value_type, U>
	{
		return m_points.fetch(index, std::forward<U>(defaultValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	fill
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::fill(const value_type& value)
	{
		m_points.fill(value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	filter
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr LineString LineString::filter(Fty f) const
		requires std::predicate<Fty&, const value_type&>
	{
		return LineString{ m_points.filter(std::forward<Fty>(f)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	indexOf
	//
	////////////////////////////////////////////////////////////////

	constexpr Optional<size_t> LineString::indexOf(const value_type& value) const
	{
		if (const auto it = std::ranges::find(m_points, value);
			it != m_points.end())
		{
			return std::ranges::distance(m_points.begin(), it);
		}
		else
		{
			return s3d::none;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	join
	//
	////////////////////////////////////////////////////////////////

	inline String LineString::join(const StringView sep) const
	{
		return m_points.join(sep);
	}

	inline String LineString::join(const StringView sep, const StringView begin, const StringView end) const
	{
		return m_points.join(sep, begin, end);
	}

	////////////////////////////////////////////////////////////////
	//
	//	map
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr auto LineString::map(Fty f) const
		requires std::invocable<Fty&, const value_type&>
	{
		return m_points.map(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	none
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr bool LineString::none(Fty f) const
		requires std::predicate<Fty&, const value_type&>
	{
		return m_points.none(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	slice
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::slice(const size_type index, const size_type length) const&
	{
		return LineString{ m_points.slice(index, length) };
	}

	constexpr LineString LineString::slice(const size_type index, const size_type length) &&
	{
		return LineString{ std::move(m_points).slice(index, length) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	head
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::head(const size_type n) const&
	{
		return LineString{ m_points.head(n) };
	}

	constexpr LineString LineString::head(const size_type n) &&
	{
		return LineString{ std::move(m_points).head(n) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	tail
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::tail(const size_type n) const&
	{
		return LineString{ m_points.tail(n) };
	}

	constexpr LineString LineString::tail(const size_type n) &&
	{
		return LineString{ std::move(m_points).tail(n) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	take
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::take(const size_type n) const&
	{
		return LineString{ m_points.take(n) };
	}

	constexpr LineString LineString::take(const size_type n) &&
	{
		return LineString{ std::move(m_points).take(n) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	take_while
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr LineString LineString::take_while(Fty f) const&
		requires std::predicate<Fty&, const value_type&>
	{
		return LineString{ m_points.take_while(std::forward<Fty>(f)) };
	}

	template <class Fty>
	constexpr LineString LineString::take_while(Fty f) &&
		requires std::predicate<Fty&, const value_type&>
	{
		return LineString{ std::move(m_points).take_while(std::forward<Fty>(f)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	values_at
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::values_at(const std::initializer_list<size_type> indices) const
	{
		return LineString{ m_points.values_at(indices) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	without
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::without(const value_type& value) const&
	{
		return LineString{ m_points.without(value) };
	}

	constexpr LineString LineString::without(const value_type& value) &&
	{
		return LineString{ std::move(m_points).without(value) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	without_at
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::without_at(const size_type index) const&
	{
		return LineString{ m_points.without_at(index) };
	}

	constexpr LineString LineString::without_at(const size_type index) &&
	{
		return LineString{ std::move(m_points).without_at(index) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	without_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr LineString LineString::without_if(Fty f) const&
		requires std::predicate<Fty&, const value_type&>
	{
		return LineString{ m_points.without_if(std::forward<Fty>(f)) };
	}

	template <class Fty>
	constexpr LineString LineString::without_if(Fty f) &&
		requires std::predicate<Fty&, const value_type&>
	{
		return LineString{ std::move(m_points).without_if(std::forward<Fty>(f)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate, rotated
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::rotate(const size_type middle)&
	{
		m_points.rotate(middle);
		return *this;
	}

	constexpr LineString LineString::rotate(const size_type middle) &&
	{
		return LineString{ std::move(m_points).rotate(middle) };
	}

	constexpr LineString LineString::rotated(const size_type middle) const&
	{
		return LineString{ m_points.rotated(middle) };
	}

	constexpr LineString LineString::rotated(const size_type middle) &&
	{
		return LineString{ std::move(m_points).rotated(middle) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse_each
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr void LineString::reverse_each(Fty f)
		requires std::invocable<Fty&, value_type&>
	{
		m_points.reverse_each(std::forward<Fty>(f));
	}

	template <class Fty>
	constexpr void LineString::reverse_each(Fty f) const
		requires std::invocable<Fty&, const value_type&>
	{
		m_points.reverse_each(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	shuffle, shuffled
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::shuffle()&
	{
		m_points.shuffle();
		return *this;
	}

	constexpr LineString LineString::shuffle() &&
	{
		return LineString{ std::move(m_points).shuffle() };
	}

	constexpr LineString LineString::shuffled() const&
	{
		return LineString{ m_points.shuffled() };
	}

	constexpr LineString LineString::shuffled() &&
	{
		return LineString{ std::move(m_points).shuffled() };
	}

	constexpr LineString& LineString::shuffle(Concept::UniformRandomBitGenerator auto&& rbg)&
	{
		m_points.shuffle(std::forward<decltype(rbg)>(rbg));
		return *this;
	}

	constexpr LineString LineString::shuffle(Concept::UniformRandomBitGenerator auto&& rbg) &&
	{
		return LineString{ std::move(m_points).shuffle(std::forward<decltype(rbg)>(rbg)) };
	}

	constexpr LineString LineString::shuffled(Concept::UniformRandomBitGenerator auto&& rbg) const&
	{
		return LineString{ m_points.shuffled(std::forward<decltype(rbg)>(rbg)) };
	}

	constexpr LineString LineString::shuffled(Concept::UniformRandomBitGenerator auto&& rbg) &&
	{
		return LineString{ std::move(m_points).shuffled(std::forward<decltype(rbg)>(rbg)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 LineString::hash() const noexcept
	{
		return BitwiseHash(m_points.data(), m_points.size_bytes());
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::reverse() noexcept
	{
		m_points.reverse();
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	reversed
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::reversed() const&
	{
		return LineString{ m_points.rbegin(), m_points.rend() };
	}

	constexpr LineString LineString::reversed() && noexcept
	{
		std::ranges::reverse(m_points);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	unique_consecutive
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::unique_consecutive()
	{
		m_points.unique_consecutive();
		return *this;
	}

	constexpr LineString LineString::uniqued_consecutive() const&
	{
		return LineString{ m_points.uniqued_consecutive() };
	}

	constexpr LineString LineString::uniqued_consecutive() && noexcept
	{
		return LineString{ std::move(m_points).uniqued_consecutive() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_points
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t LineString::num_points() const noexcept
	{
		return m_points.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_segments
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t LineString::num_segments(const CloseRing closeRing) const noexcept
	{
		if (m_points.size() < 2)
		{
			return 0;
		}
		
		return (closeRing ? m_points.size() : (m_points.size() - 1));
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::movedBy(const double x, const double y) const&
	{
		LineString result{ *this };
		
		for (auto& point : result.m_points)
		{
			point.moveBy(x, y);
		}
		
		return result;
	}

	constexpr LineString LineString::movedBy(const double x, const double y) && noexcept
	{
		moveBy(x, y);
		return std::move(*this);
	}

	constexpr LineString LineString::movedBy(const Vec2 v) const&
	{
		LineString result{ *this };
	
		for (auto& point : result.m_points)
		{
			point.moveBy(v);
		}
		
		return result;
	}

	constexpr LineString LineString::movedBy(const Vec2 v) && noexcept
	{
		moveBy(v);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::moveBy(const double x, const double y) noexcept
	{
		for (auto& point : m_points)
		{
			point.moveBy(x, y);
		}

		return *this;
	}

	constexpr LineString& LineString::moveBy(const Vec2 v) noexcept
	{
		for (auto& point : m_points)
		{
			point.moveBy(v);
		}
	
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffset
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::withOffset(const double x, const double y) const&
	{
		LineString result{ *this };
		
		for (auto& point : result.m_points)
		{
			point.moveBy(x, y);
		}
		
		return result;
	}

	constexpr LineString LineString::withOffset(const double x, const double y) && noexcept
	{
		moveBy(x, y);
		return std::move(*this);
	}

	constexpr LineString LineString::withOffset(const Vec2 v) const&
	{
		LineString result{ *this };
	
		for (auto& point : result.m_points)
		{
			point.moveBy(v);
		}
		
		return result;
	}

	constexpr LineString LineString::withOffset(const Vec2 v) && noexcept
	{
		moveBy(v);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffsetX, withOffsetY
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::withOffsetX(const double x) const&
	{
		LineString result{ *this };
		
		for (auto& point : result.m_points)
		{
			point.x += x;
		}
		
		return result;
	}

	constexpr LineString LineString::withOffsetX(const double x) && noexcept
	{
		for (auto& point : m_points)
		{
			point.x += x;
		}
	
		return std::move(*this);
	}

	constexpr LineString LineString::withOffsetY(const double y) const&
	{
		LineString result{ *this };
		
		for (auto& point : result.m_points)
		{
			point.y += y;
		}
		
		return result;
	}

	constexpr LineString LineString::withOffsetY(const double y) && noexcept
	{
		for (auto& point : m_points)
		{
			point.y += y;
		}
	
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromOrigin
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::scaledFromOrigin(const double s) const&
	{
		LineString result{ *this };

		for (auto& point : result.m_points)
		{
			point *= s;
		}

		return result;
	}

	constexpr LineString LineString::scaledFromOrigin(const double s) && noexcept
	{
		scaleFromOrigin(s);
		return std::move(*this);
	}

	constexpr LineString LineString::scaledFromOrigin(const double sx, const double sy) const&
	{
		LineString result{ *this };

		for (auto& point : result.m_points)
		{
			point.x *= sx;
			point.y *= sy;
		}

		return result;
	}

	constexpr LineString LineString::scaledFromOrigin(const double sx, const double sy) && noexcept
	{
		scaleFromOrigin(sx, sy);
		return std::move(*this);
	}

	constexpr LineString LineString::scaledFromOrigin(const Vec2 s) const&
	{
		LineString result{ *this };

		for (auto& point : result.m_points)
		{
			point *= s;
		}

		return result;
	}

	constexpr LineString LineString::scaledFromOrigin(const Vec2 s) && noexcept
	{
		scaleFromOrigin(s);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFromOrigin
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::scaleFromOrigin(const double s)
	{
		for (auto& point : m_points)
		{
			point *= s;
		}

		return *this;
	}

	constexpr LineString& LineString::scaleFromOrigin(const double sx, const double sy)
	{
		for (auto& point : m_points)
		{
			point.x *= sx;
			point.y *= sy;
		}

		return *this;
	}

	constexpr LineString& LineString::scaleFromOrigin(const Vec2 s)
	{
		for (auto& point : m_points)
		{
			point *= s;
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFrom
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::scaledFrom(const Vec2 pos, const double s) const&
	{
		LineString result{ *this };
		
		for (auto& point : result.m_points)
		{
			point = pos + ((point - pos) * s);
		}
		
		return result;
	}

	constexpr LineString LineString::scaledFrom(const Vec2 pos, const double s) && noexcept
	{
		scaleFrom(pos, s);
		return std::move(*this);
	}

	constexpr LineString LineString::scaledFrom(const Vec2 pos, const double sx, const double sy) const&
	{
		LineString result{ *this };
		
		for (auto& point : result.m_points)
		{
			point.x = pos.x + ((point.x - pos.x) * sx);
			point.y = pos.y + ((point.y - pos.y) * sy);
		}
		
		return result;
	}

	constexpr LineString LineString::scaledFrom(const Vec2 pos, const double sx, const double sy) && noexcept
	{
		scaleFrom(pos, sx, sy);
		return std::move(*this);
	}

	constexpr LineString LineString::scaledFrom(const Vec2 pos, const Vec2 s) const&
	{
		LineString result{ *this };
		
		for (auto& point : result.m_points)
		{
			point.x = pos.x + ((point.x - pos.x) * s.x);
			point.y = pos.y + ((point.y - pos.y) * s.y);
		}
		
		return result;
	}

	constexpr LineString LineString::scaledFrom(const Vec2 pos, const Vec2 s) && noexcept
	{
		scaleFrom(pos, s);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFrom
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::scaleFrom(const Vec2 pos, const double s)
	{
		for (auto& point : m_points)
		{
			point = pos + ((point - pos) * s);
		}
	
		return *this;
	}

	constexpr LineString& LineString::scaleFrom(const Vec2 pos, const double sx, const double sy)
	{
		for (auto& point : m_points)
		{
			point.x = pos.x + ((point.x - pos.x) * sx);
			point.y = pos.y + ((point.y - pos.y) * sy);
		}
	
		return *this;
	}

	constexpr LineString& LineString::scaleFrom(const Vec2 pos, const Vec2 s)
	{
		for (auto& point : m_points)
		{
			point.x = pos.x + ((point.x - pos.x) * s.x);
			point.y = pos.y + ((point.y - pos.y) * s.y);
		}
	
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator >>
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr auto LineString::operator >>(Fty f) const
		requires std::invocable<Fty&, const value_type&>
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
