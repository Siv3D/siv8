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
		: m_vertices{ other } {}

	constexpr LineString::LineString(container_type&& other)
		: m_vertices{ std::move(other) } {}

	constexpr LineString::LineString(const size_type count, const value_type& value)
		: m_vertices(count, value) {}

	constexpr LineString::LineString(const size_type count)
		: m_vertices(count) {}

	template <std::input_iterator Iterator>
	constexpr LineString::LineString(Iterator first, Iterator last)
		: m_vertices(first, last) {}

	constexpr LineString::LineString(const Array<Point>& vertices)
		: m_vertices(vertices.begin(), vertices.end()) {}

	constexpr LineString::LineString(const HasAsArray auto& a)
		: m_vertices{ a.asArray() } {}

	constexpr LineString::LineString(HasAsArray auto&& a)
		: m_vertices{ std::forward<decltype(a)>(a).asArray() } {}

	constexpr LineString::LineString(std::initializer_list<value_type> list)
		: m_vertices(list) {}

	template <Concept::ContainerCompatibleRange<Vec2> Range>
	constexpr LineString::LineString(std::from_range_t, Range&& range)
		: m_vertices(std::from_range, std::forward<Range>(range)) {}

	template <Concept::ContainerCompatibleRange<Point> Range>
	constexpr LineString::LineString(std::from_range_t, Range&& range)
		: m_vertices(std::from_range, std::forward<Range>(range)) {}

	constexpr LineString::LineString(const Arg::reserve_<size_type> size)
		: m_vertices{ size } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::operator =(const container_type& other)
	{
		m_vertices = other;
		return *this;
	}

	constexpr LineString& LineString::operator =(container_type&& other)
	{
		m_vertices = std::move(other);
		return *this;
	}

	constexpr LineString& LineString::operator =(const Array<Point>& other)
	{
		m_vertices.assign(other.begin(), other.end());
		return *this;
	}

	constexpr LineString& LineString::operator =(const HasAsArray auto& a)
	{
		m_vertices = a.asArray();
		return *this;
	}

	constexpr LineString& LineString::operator =(HasAsArray auto&& a)
	{
		m_vertices = std::forward<decltype(a)>(a).asArray();
		return *this;
	}

	constexpr LineString& LineString::operator =(std::initializer_list<value_type> list)
	{
		m_vertices = list;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	assign
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::assign(const size_type count, const value_type& value)
	{
		m_vertices.assign(count, value);
		return *this;
	}

	template <std::input_iterator Iterator>
	constexpr LineString& LineString::assign(Iterator first, Iterator last)
	{
		m_vertices.assign(first, last);
		return *this;
	}

	constexpr LineString& LineString::assign(std::initializer_list<value_type> list)
	{
		m_vertices.assign(list);
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
		m_vertices.assign_range(std::forward<Range>(range));
		return *this;
	}

	template <Concept::ContainerCompatibleRange<Point> Range>
	constexpr LineString& LineString::assign_range(Range&& range)
	{
		m_vertices.assign_range(std::forward<Range>(range));
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
		return m_vertices.get_allocator();
	}

	////////////////////////////////////////////////////////////////
	//
	//	asArray
	//
	////////////////////////////////////////////////////////////////

	constexpr const LineString::container_type& LineString::asArray() const& noexcept
	{
		return m_vertices;
	}

	constexpr LineString::container_type LineString::asArray() && noexcept
	{
		return std::move(m_vertices);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator container_type
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::operator container_type() const& noexcept
	{
		return m_vertices;
	}

	constexpr LineString::operator container_type() && noexcept
	{
		return std::move(m_vertices);
	}

	////////////////////////////////////////////////////////////////
	//
	//	at
	//
	////////////////////////////////////////////////////////////////

	constexpr const LineString::value_type& LineString::at(const size_type index) const&
	{
		return m_vertices.at(index);
	}

	constexpr LineString::value_type& LineString::at(const size_type index)&
	{
		return m_vertices.at(index);
	}

	constexpr LineString::value_type LineString::at(const size_type index)&&
	{
		return m_vertices.at(index);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	constexpr const LineString::value_type& LineString::operator [](const size_type index) const& noexcept
	{
		return m_vertices[index];
	}

	constexpr LineString::value_type& LineString::operator [](const size_type index) & noexcept
	{
		return m_vertices[index];
	}

	constexpr LineString::value_type LineString::operator [](const size_type index) && noexcept
	{
		return m_vertices[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	front
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::reference LineString::front() & noexcept
	{
		return m_vertices.front();
	}

	constexpr LineString::const_reference LineString::front() const& noexcept
	{
		return m_vertices.front();
	}

	constexpr LineString::value_type LineString::front() && noexcept
	{
		return m_vertices.front();
	}

	////////////////////////////////////////////////////////////////
	//
	//	back
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::reference LineString::back() & noexcept
	{
		return m_vertices.back();
	}

	constexpr LineString::const_reference LineString::back() const& noexcept
	{
		return m_vertices.back();
	}

	constexpr LineString::value_type LineString::back() && noexcept
	{
		return m_vertices.back();
	}

	////////////////////////////////////////////////////////////////
	//
	//	data
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::value_type* LineString::data() noexcept
	{
		return m_vertices.data();
	}

	constexpr const LineString::value_type* LineString::data() const noexcept
	{
		return m_vertices.data();
	}

	////////////////////////////////////////////////////////////////
	//
	//	begin, end
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::iterator LineString::begin() noexcept
	{
		return m_vertices.begin();
	}

	constexpr LineString::iterator LineString::end() noexcept
	{
		return m_vertices.end();
	}

	constexpr LineString::const_iterator LineString::begin() const noexcept
	{
		return m_vertices.begin();
	}

	constexpr LineString::const_iterator LineString::end() const noexcept
	{
		return m_vertices.end();
	}

	////////////////////////////////////////////////////////////////
	//
	//	cbegin, cend
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::const_iterator LineString::cbegin() const noexcept
	{
		return m_vertices.cbegin();
	}

	constexpr LineString::const_iterator LineString::cend() const noexcept
	{
		return m_vertices.cend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	rbegin, rend
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::reverse_iterator LineString::rbegin() noexcept
	{
		return m_vertices.rbegin();
	}

	constexpr LineString::reverse_iterator LineString::rend() noexcept
	{
		return m_vertices.rend();
	}

	constexpr LineString::const_reverse_iterator LineString::rbegin() const noexcept
	{
		return m_vertices.rbegin();
	}

	constexpr LineString::const_reverse_iterator LineString::rend() const noexcept
	{
		return m_vertices.rend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	crbegin, crend
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::const_reverse_iterator LineString::crbegin() const noexcept
	{
		return m_vertices.crbegin();
	}

	constexpr LineString::const_reverse_iterator LineString::crend() const noexcept
	{
		return m_vertices.crend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	empty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool LineString::empty() const noexcept
	{
		return m_vertices.isEmpty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool LineString::isEmpty() const noexcept
	{
		return m_vertices.isEmpty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	indexInBounds
	//
	////////////////////////////////////////////////////////////////

	constexpr bool LineString::indexInBounds(const size_type index) const noexcept
	{
		return (index < m_vertices.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::operator bool() const noexcept
	{
		return (not m_vertices.isEmpty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t LineString::size() const noexcept
	{
		return m_vertices.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	ssize
	//
	////////////////////////////////////////////////////////////////

	constexpr isize LineString::ssize() const noexcept
	{
		return m_vertices.ssize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	size_bytes
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t LineString::size_bytes() const noexcept
	{
		return m_vertices.size_bytes();
	}

	////////////////////////////////////////////////////////////////
	//
	//	max_size
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t LineString::max_size() const noexcept
	{
		return m_vertices.max_size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	reserve
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::reserve(const size_type n)
	{
		m_vertices.reserve(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	capacity
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t LineString::capacity() const noexcept
	{
		return m_vertices.capacity();
	}

	////////////////////////////////////////////////////////////////
	//
	//	shrink_to_fit
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::shrink_to_fit()
	{
		m_vertices.shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::clear() noexcept
	{
		m_vertices.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::release()
	{
		m_vertices.release();
	}

	////////////////////////////////////////////////////////////////
	//
	//	insert
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::iterator LineString::insert(const_iterator pos, const value_type& value)
	{
		return m_vertices.insert(pos, value);
	}

	constexpr LineString::iterator LineString::insert(const_iterator pos, const size_type count, const value_type& value)
	{
		return m_vertices.insert(pos, count, value);
	}

	template <std::input_iterator Iterator>
	constexpr LineString::iterator LineString::insert(const_iterator pos, Iterator first, Iterator last)
	{
		return m_vertices.insert(pos, first, last);
	}

	constexpr LineString::iterator LineString::insert(const_iterator pos, std::initializer_list<value_type> list)
	{
		return m_vertices.insert(pos, list);
	}

	////////////////////////////////////////////////////////////////
	//
	//	insert_range
	//
	////////////////////////////////////////////////////////////////

	template <Concept::ContainerCompatibleRange<Vec2> Range>
	constexpr LineString::iterator LineString::insert_range(const_iterator pos, Range&& range)
	{
		return m_vertices.insert_range(pos, std::forward<Range>(range));
	}

	////////////////////////////////////////////////////////////////
	//
	//	emplace
	//
	////////////////////////////////////////////////////////////////

	template <class... Args>
	constexpr LineString::iterator LineString::emplace(const_iterator pos, Args&&... args)
	{
		return m_vertices.emplace(pos, std::forward<Args>(args)...);
	}

	////////////////////////////////////////////////////////////////
	//
	//	append_range
	//
	////////////////////////////////////////////////////////////////

	template <Concept::ContainerCompatibleRange<Vec2> Range>
	constexpr void LineString::append_range(Range&& range)
	{
		m_vertices.append_range(std::forward<Range>(range));
	}

	template <Concept::ContainerCompatibleRange<Point> Range>
	constexpr void LineString::append_range(Range&& range)
	{
		m_vertices.append_range(std::forward<Range>(range));
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::iterator LineString::erase(const_iterator pos)
	{
		return m_vertices.erase(pos);
	}

	constexpr LineString::iterator LineString::erase(const_iterator first, const_iterator last)
	{
		return m_vertices.erase(first, last);
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_at
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::erase_at(const size_type index)&
	{
		m_vertices.erase_at(index);
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
		return m_vertices.erase_all(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_first
	//
	////////////////////////////////////////////////////////////////

	constexpr bool LineString::erase_first(const value_type& value)
	{
		return m_vertices.erase_first(value);
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
		return m_vertices.erase_all_if(std::forward<Fty>(f));
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
		return m_vertices.erase_first_if(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	push_back
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::push_back(const value_type& value)
	{
		m_vertices.push_back(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	emplace_back
	//
	////////////////////////////////////////////////////////////////

	template <class... Args>
	constexpr LineString::reference LineString::emplace_back(Args&&... args)
	{
		return m_vertices.emplace_back(std::forward<Args>(args)...);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::pop_back()
	{
		m_vertices.pop_back();
	}

	////////////////////////////////////////////////////////////////
	//
	//	resize
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::resize(const size_type count)
	{
		m_vertices.resize(count);
	}

	constexpr void LineString::resize(const size_type count, const value_type& value)
	{
		m_vertices.resize(count, value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::swap(LineString& other) noexcept
	{
		m_vertices.swap(other.m_vertices);
	}

	////////////////////////////////////////////////////////////////
	//
	//	push_front
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::push_front(const value_type& value)
	{
		m_vertices.push_front(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	emplace_front
	//
	////////////////////////////////////////////////////////////////

	template <class... Args>
	constexpr LineString::reference LineString::emplace_front(Args&&... args)
	{
		return m_vertices.emplace_front(std::forward<Args>(args)...);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_front
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::pop_front()
	{
		m_vertices.pop_front();
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_front_N
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::pop_front_N(const size_type n)
	{
		m_vertices.pop_front_N(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back_N
	//
	////////////////////////////////////////////////////////////////

	constexpr void LineString::pop_back_N(const size_type n)
	{
		m_vertices.pop_back_N(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::operator <<(const value_type& value)
	{
		m_vertices << value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	subspan
	//
	////////////////////////////////////////////////////////////////

	constexpr std::span<LineString::value_type> LineString::subspan(const size_type pos, const size_type count) noexcept
	{
		return m_vertices.subspan(pos, count);
	}

	constexpr std::span<const LineString::value_type> LineString::subspan(const size_type pos, const size_type count) const noexcept
	{
		return m_vertices.subspan(pos, count);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 LineString::hash() const noexcept
	{
		return BitwiseHash(m_vertices.data(), m_vertices.size_bytes());
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
		return m_vertices.all(std::forward<Fty>(f));
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
		return m_vertices.any(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	append
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::append(const LineString& other)
	{
		m_vertices.append(other.m_vertices);
		return *this;
	}

	constexpr LineString& LineString::append(const container_type& other)
	{
		m_vertices.append(other);
		return *this;
	}

	template <std::input_iterator Iterator>
	constexpr LineString& LineString::append(Iterator first, Iterator last)
	{
		m_vertices.append(first, last);
		return *this;
	}

	constexpr LineString& LineString::append(std::initializer_list<value_type> list)
	{
		m_vertices.append(list);
		return *this;
	}

	constexpr LineString& LineString::append(const size_type count, const value_type& value)
	{
		m_vertices.append(count, value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	choice
	//
	////////////////////////////////////////////////////////////////

	inline LineString::value_type& LineString::choice()
	{
		return m_vertices.choice();
	}

	inline const LineString::value_type& LineString::choice() const
	{
		return m_vertices.choice();
	}

	inline LineString::value_type& LineString::choice(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return m_vertices.choice(urbg);
	}

	inline const LineString::value_type& LineString::choice(Concept::UniformRandomBitGenerator auto&& urbg) const
	{
		return m_vertices.choice(urbg);
	}

	inline LineString LineString::choice(const size_t n) const
	{
		return LineString{ m_vertices.choice(n) };
	}

	inline LineString LineString::choice(const size_t n, Concept::UniformRandomBitGenerator auto&& urbg) const
	{
		return LineString{ m_vertices.choice(n, urbg) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	chunk
	//
	////////////////////////////////////////////////////////////////

	constexpr Array<LineString> LineString::chunk(const size_type n) const
	{
		Array<LineString> result;

		if (n == 0)
		{
			return result;
		}

		const size_type s = size();
		const size_type chunkCount = (s + n - 1) / n;
		result.reserve(chunkCount);

		for (size_type i = 0; i < chunkCount; ++i)
		{
			const size_type index = (i * n);
			const size_type length = Min((s - index), n);
			result.push_back(slice(index, length));
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains
	//
	////////////////////////////////////////////////////////////////

	constexpr bool LineString::contains(const value_type& value) const
	{
		return m_vertices.contains(value);
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
		return m_vertices.contains_if(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	count
	//
	////////////////////////////////////////////////////////////////

	constexpr isize LineString::count(const value_type& value) const
	{
		return m_vertices.count(value);
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
		return m_vertices.count_if(std::forward<Fty>(f));
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
		m_vertices.each(std::forward<Fty>(f));
	}

	template <class Fty>
	constexpr void LineString::each(Fty f) const
		requires std::invocable<Fty&, const value_type&>
	{
		m_vertices.each(std::forward<Fty>(f));
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
		m_vertices.each_index(std::forward<Fty>(f));
	}

	template <class Fty>
	constexpr void LineString::each_index(Fty f) const
		requires std::invocable<Fty&, size_t, const value_type&>
	{
		m_vertices.each_index(std::forward<Fty>(f));
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
		m_vertices.each_sindex(std::forward<Fty>(f));
	}

	template <class Fty>
	constexpr void LineString::each_sindex(Fty f) const
		requires std::invocable<Fty&, isize, const value_type&>
	{
		m_vertices.each_sindex(std::forward<Fty>(f));
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
		return m_vertices.fetch(index, std::forward<U>(defaultValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	fill
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::fill(const value_type& value)
	{
		m_vertices.fill(value);
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
		return LineString{ m_vertices.filter(std::forward<Fty>(f)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	in_groups
	//
	////////////////////////////////////////////////////////////////

	constexpr Array<LineString> LineString::in_groups(const size_type group) const
	{
		Array<LineString> result;

		if (group == 0)
		{
			return result;
		}

		const size_type s = size();

		if (s == 0)
		{
			return result;
		}

		const size_type g = Min(group, s);
		result.reserve(g);

		const size_type div = (s / g);
		const size_type mod = (s % g);

		size_type index = 0;

		for (size_type i = 0; i < g; ++i)
		{
			const size_type length = (div + (i < mod ? 1 : 0));
			result.push_back(slice(index, length));
			index += length;
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	indexOf
	//
	////////////////////////////////////////////////////////////////

	constexpr Optional<size_t> LineString::indexOf(const value_type& value) const
	{
		if (const auto it = std::ranges::find(m_vertices, value);
			it != m_vertices.end())
		{
			return std::ranges::distance(m_vertices.begin(), it);
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
		return m_vertices.join(sep);
	}

	inline String LineString::join(const StringView sep, const StringView begin, const StringView end) const
	{
		return m_vertices.join(sep, begin, end);
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
		return m_vertices.map(std::forward<Fty>(f));
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
		return m_vertices.none(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	slice
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::slice(const size_type index, const size_type length) const&
	{
		return LineString{ m_vertices.slice(index, length) };
	}

	constexpr LineString LineString::slice(const size_type index, const size_type length) &&
	{
		return LineString{ std::move(m_vertices).slice(index, length) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	head
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::head(const size_type n) const&
	{
		return LineString{ m_vertices.head(n) };
	}

	constexpr LineString LineString::head(const size_type n) &&
	{
		return LineString{ std::move(m_vertices).head(n) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	head_span
	//
	////////////////////////////////////////////////////////////////

	constexpr std::span<LineString::value_type> LineString::head_span(const size_type n) & noexcept
	{
		return m_vertices.head_span(n);
	}

	constexpr std::span<const LineString::value_type> LineString::head_span(const size_type n) const& noexcept
	{
		return m_vertices.head_span(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	head_view
	//
	////////////////////////////////////////////////////////////////

	constexpr auto LineString::head_view(const size_type n) & noexcept
	{
		return m_vertices.head_view(n);
	}

	constexpr auto LineString::head_view(const size_type n) const& noexcept
	{
		return m_vertices.head_view(n);
	}

	constexpr auto LineString::head_view(const size_type n) && noexcept
	{
		return std::move(m_vertices).head_view(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	tail
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::tail(const size_type n) const&
	{
		return LineString{ m_vertices.tail(n) };
	}

	constexpr LineString LineString::tail(const size_type n) &&
	{
		return LineString{ std::move(m_vertices).tail(n) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	tail_span
	//
	////////////////////////////////////////////////////////////////

	constexpr std::span<LineString::value_type> LineString::tail_span(const size_type n) & noexcept
	{
		return m_vertices.tail_span(n);
	}

	constexpr std::span<const LineString::value_type> LineString::tail_span(const size_type n) const& noexcept
	{
		return m_vertices.tail_span(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	tail_view
	//
	////////////////////////////////////////////////////////////////

	constexpr auto LineString::tail_view(const size_type n) & noexcept
	{
		return m_vertices.tail_view(n);
	}

	constexpr auto LineString::tail_view(const size_type n) const& noexcept
	{
		return m_vertices.tail_view(n);
	}

	constexpr auto LineString::tail_view(const size_type n) && noexcept
	{
		return std::move(m_vertices).tail_view(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	take
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::take(const size_type n) const&
	{
		return LineString{ m_vertices.take(n) };
	}

	constexpr LineString LineString::take(const size_type n) &&
	{
		return LineString{ std::move(m_vertices).take(n) };
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
		return LineString{ m_vertices.take_while(std::forward<Fty>(f)) };
	}

	template <class Fty>
	constexpr LineString LineString::take_while(Fty f) &&
		requires std::predicate<Fty&, const value_type&>
	{
		return LineString{ std::move(m_vertices).take_while(std::forward<Fty>(f)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	values_at
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::values_at(const std::initializer_list<size_type> indices) const
	{
		return LineString{ m_vertices.values_at(indices) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	without
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::without(const value_type& value) const&
	{
		return LineString{ m_vertices.without(value) };
	}

	constexpr LineString LineString::without(const value_type& value) &&
	{
		return LineString{ std::move(m_vertices).without(value) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	without_at
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::without_at(const size_type index) const&
	{
		return LineString{ m_vertices.without_at(index) };
	}

	constexpr LineString LineString::without_at(const size_type index) &&
	{
		return LineString{ std::move(m_vertices).without_at(index) };
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
		return LineString{ m_vertices.without_if(std::forward<Fty>(f)) };
	}

	template <class Fty>
	constexpr LineString LineString::without_if(Fty f) &&
		requires std::predicate<Fty&, const value_type&>
	{
		return LineString{ std::move(m_vertices).without_if(std::forward<Fty>(f)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	replace, replaced
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::replace(const value_type& oldValue, const value_type& newValue)&
	{
		m_vertices.replace(oldValue, newValue);
		return *this;
	}

	constexpr LineString LineString::replace(const value_type& oldValue, const value_type& newValue) &&
	{
		return LineString{ std::move(m_vertices).replace(oldValue, newValue) };
	}

	constexpr LineString LineString::replaced(const value_type& oldValue, const value_type& newValue) const&
	{
		return LineString{ m_vertices.replaced(oldValue, newValue) };
	}

	constexpr LineString LineString::replaced(const value_type& oldValue, const value_type& newValue) &&
	{
		return LineString{ std::move(m_vertices).replaced(oldValue, newValue) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	replace_if, replaced_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr LineString& LineString::replace_if(Fty f, const value_type& newValue)&
		requires std::predicate<Fty&, const value_type&>
	{
		m_vertices.replace_if(std::forward<Fty>(f), newValue);
		return *this;
	}

	template <class Fty>
	constexpr LineString LineString::replace_if(Fty f, const value_type& newValue) &&
		requires std::predicate<Fty&, const value_type&>
	{
		return LineString{ std::move(m_vertices).replace_if(std::forward<Fty>(f), newValue) };
	}

	template <class Fty>
	constexpr LineString LineString::replaced_if(Fty f, const value_type& newValue) const&
		requires std::predicate<Fty&, const value_type&>
	{
		return LineString{ m_vertices.replaced_if(std::forward<Fty>(f), newValue) };
	}

	template <class Fty>
	constexpr LineString LineString::replaced_if(Fty f, const value_type& newValue) &&
		requires std::predicate<Fty&, const value_type&>
	{
		return LineString{ std::move(m_vertices).replaced_if(std::forward<Fty>(f), newValue) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate, rotated
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::rotate(const size_type middle)&
	{
		m_vertices.rotate(middle);
		return *this;
	}

	constexpr LineString LineString::rotate(const size_type middle) &&
	{
		return LineString{ std::move(m_vertices).rotate(middle) };
	}

	constexpr LineString LineString::rotated(const size_type middle) const&
	{
		return LineString{ m_vertices.rotated(middle) };
	}

	constexpr LineString LineString::rotated(const size_type middle) &&
	{
		return LineString{ std::move(m_vertices).rotated(middle) };
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
		m_vertices.reverse_each(std::forward<Fty>(f));
	}

	template <class Fty>
	constexpr void LineString::reverse_each(Fty f) const
		requires std::invocable<Fty&, const value_type&>
	{
		m_vertices.reverse_each(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse_view
	//
	////////////////////////////////////////////////////////////////

	constexpr auto LineString::reverse_view() &
	{
		return m_vertices.reverse_view();
	}

	constexpr auto LineString::reverse_view() const&
	{
		return m_vertices.reverse_view();
	}

	constexpr auto LineString::reverse_view() &&
	{
		return std::move(m_vertices).reverse_view();
	}

	////////////////////////////////////////////////////////////////
	//
	//	shuffle, shuffled
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::shuffle()&
	{
		m_vertices.shuffle();
		return *this;
	}

	constexpr LineString LineString::shuffle() &&
	{
		return LineString{ std::move(m_vertices).shuffle() };
	}

	constexpr LineString LineString::shuffled() const&
	{
		return LineString{ m_vertices.shuffled() };
	}

	constexpr LineString LineString::shuffled() &&
	{
		return LineString{ std::move(m_vertices).shuffled() };
	}

	constexpr LineString& LineString::shuffle(Concept::UniformRandomBitGenerator auto&& urbg)&
	{
		m_vertices.shuffle(urbg);
		return *this;
	}

	constexpr LineString LineString::shuffle(Concept::UniformRandomBitGenerator auto&& urbg) &&
	{
		return LineString{ std::move(m_vertices).shuffle(urbg) };
	}

	constexpr LineString LineString::shuffled(Concept::UniformRandomBitGenerator auto&& urbg) const&
	{
		return LineString{ m_vertices.shuffled(urbg) };
	}

	constexpr LineString LineString::shuffled(Concept::UniformRandomBitGenerator auto&& urbg) &&
	{
		return LineString{ std::move(m_vertices).shuffled(urbg) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	sort_by, sorted_by
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr LineString& LineString::sort_by(Fty f)&
		requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
	{
		m_vertices.sort_by(std::forward<Fty>(f));
		return *this;
	}

	template <class Fty>
	constexpr LineString LineString::sort_by(Fty f) &&
		requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
	{
		return LineString{ std::move(m_vertices).sort_by(std::forward<Fty>(f)) };
	}

	template <class Fty>
	constexpr LineString LineString::sorted_by(Fty f) const&
		requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
	{
		return LineString{ m_vertices.sorted_by(std::forward<Fty>(f)) };
	}

	template <class Fty>
	constexpr LineString LineString::sorted_by(Fty f) &&
		requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
	{
		return LineString{ std::move(m_vertices).sorted_by(std::forward<Fty>(f)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	sum
	//
	////////////////////////////////////////////////////////////////

	constexpr auto LineString::sum() const
	{
		return m_vertices.sum();
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::reverse() & noexcept
	{
		m_vertices.reverse();
		return *this;
	}

	constexpr LineString LineString::reverse() && noexcept
	{
		m_vertices.reverse();
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	reversed
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::reversed() const&
	{
		return LineString{ m_vertices.rbegin(), m_vertices.rend() };
	}

	constexpr LineString LineString::reversed() && noexcept
	{
		std::ranges::reverse(m_vertices);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	unique_consecutive
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString& LineString::unique_consecutive() &
	{
		m_vertices.unique_consecutive();
		return *this;
	}

	constexpr LineString LineString::unique_consecutive() &&
	{
		m_vertices.unique_consecutive();
		return std::move(*this);
	}

	constexpr LineString LineString::uniqued_consecutive() const&
	{
		return LineString{ m_vertices.uniqued_consecutive() };
	}

	constexpr LineString LineString::uniqued_consecutive() && noexcept
	{
		return LineString{ std::move(m_vertices).uniqued_consecutive() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_points
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t LineString::num_vertices() const noexcept
	{
		return m_vertices.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_segments
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t LineString::num_segments(const CloseRing closeRing) const noexcept
	{
		if (m_vertices.size() < 2)
		{
			return 0;
		}
		
		return (closeRing ? m_vertices.size() : (m_vertices.size() - 1));
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString LineString::movedBy(const double x, const double y) const&
	{
		LineString result{ *this };
		
		for (auto& vertex : result.m_vertices)
		{
			vertex.moveBy(x, y);
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
	
		for (auto& vertex : result.m_vertices)
		{
			vertex.moveBy(v);
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
		for (auto& vertex : m_vertices)
		{
			vertex.moveBy(x, y);
		}

		return *this;
	}

	constexpr LineString& LineString::moveBy(const Vec2 v) noexcept
	{
		for (auto& vertex : m_vertices)
		{
			vertex.moveBy(v);
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
		
		for (auto& vertex : result.m_vertices)
		{
			vertex.moveBy(x, y);
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
	
		for (auto& vertex : result.m_vertices)
		{
			vertex.moveBy(v);
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
		
		for (auto& vertex : result.m_vertices)
		{
			vertex.x += x;
		}
		
		return result;
	}

	constexpr LineString LineString::withOffsetX(const double x) && noexcept
	{
		for (auto& vertex : m_vertices)
		{
			vertex.x += x;
		}
	
		return std::move(*this);
	}

	constexpr LineString LineString::withOffsetY(const double y) const&
	{
		LineString result{ *this };
		
		for (auto& vertex : result.m_vertices)
		{
			vertex.y += y;
		}
		
		return result;
	}

	constexpr LineString LineString::withOffsetY(const double y) && noexcept
	{
		for (auto& vertex : m_vertices)
		{
			vertex.y += y;
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

		for (auto& vertex : result.m_vertices)
		{
			vertex *= s;
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

		for (auto& vertex : result.m_vertices)
		{
			vertex.x *= sx;
			vertex.y *= sy;
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

		for (auto& vertex : result.m_vertices)
		{
			vertex *= s;
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
		for (auto& vertex : m_vertices)
		{
			vertex *= s;
		}

		return *this;
	}

	constexpr LineString& LineString::scaleFromOrigin(const double sx, const double sy)
	{
		for (auto& vertex : m_vertices)
		{
			vertex.x *= sx;
			vertex.y *= sy;
		}

		return *this;
	}

	constexpr LineString& LineString::scaleFromOrigin(const Vec2 s)
	{
		for (auto& vertex : m_vertices)
		{
			vertex *= s;
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
		
		for (auto& vertex : result.m_vertices)
		{
			vertex = pos + ((vertex - pos) * s);
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
		
		for (auto& vertex : result.m_vertices)
		{
			vertex.x = pos.x + ((vertex.x - pos.x) * sx);
			vertex.y = pos.y + ((vertex.y - pos.y) * sy);
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
		
		for (auto& vertex : result.m_vertices)
		{
			vertex.x = pos.x + ((vertex.x - pos.x) * s.x);
			vertex.y = pos.y + ((vertex.y - pos.y) * s.y);
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
		for (auto& vertex : m_vertices)
		{
			vertex = pos + ((vertex - pos) * s);
		}
	
		return *this;
	}

	constexpr LineString& LineString::scaleFrom(const Vec2 pos, const double sx, const double sy)
	{
		for (auto& vertex : m_vertices)
		{
			vertex.x = pos.x + ((vertex.x - pos.x) * sx);
			vertex.y = pos.y + ((vertex.y - pos.y) * sy);
		}
	
		return *this;
	}

	constexpr LineString& LineString::scaleFrom(const Vec2 pos, const Vec2 s)
	{
		for (auto& vertex : m_vertices)
		{
			vertex.x = pos.x + ((vertex.x - pos.x) * s.x);
			vertex.y = pos.y + ((vertex.y - pos.y) * s.y);
		}
	
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool LineString::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersects(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersectsAt
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	Optional<Array<Vec2>> LineString::intersectsAt(const Shape2DType& other) const
	{
		return Geometry2D::IntersectsAt(*this, other);
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
			result.m_vertices[i] = generator();
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
			result.m_vertices[i] = generator(i);
		}

		return result;
	}
}
