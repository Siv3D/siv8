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
	MultiPolygon::MultiPolygon(Iterator first, Iterator last)
		: m_polygons(first, last) {}

	inline MultiPolygon::MultiPolygon(std::initializer_list<value_type> init)
		: m_polygons(init) {}

	template <Concept::ContainerCompatibleRange<Polygon> Range>
	MultiPolygon::MultiPolygon(std::from_range_t tag, Range&& range)
		: m_polygons(tag, std::forward<Range>(range)) {}

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

	inline MultiPolygon& MultiPolygon::operator =(std::initializer_list<value_type> list)
	{
		m_polygons = list;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	assign
	//
	////////////////////////////////////////////////////////////////

	template <std::input_iterator Iterator>
	MultiPolygon& MultiPolygon::assign(Iterator first, Iterator last)
	{
		m_polygons.assign(first, last);
		return *this;
	}

	inline MultiPolygon& MultiPolygon::assign(std::initializer_list<value_type> list)
	{
		m_polygons.assign(list);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	assign_range
	//
	////////////////////////////////////////////////////////////////

	template <Concept::ContainerCompatibleRange<Polygon> Range>
	MultiPolygon& MultiPolygon::assign_range(Range&& range)
	{
		m_polygons.assign_range(std::forward<Range>(range));
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	get_allocator
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon::allocator_type MultiPolygon::get_allocator() const noexcept
	{
		return m_polygons.get_allocator();
	}

	////////////////////////////////////////////////////////////////
	//
	//	asArray
	//
	////////////////////////////////////////////////////////////////

	inline const MultiPolygon::container_type& MultiPolygon::asArray() const& noexcept
	{
		return m_polygons;
	}

	inline MultiPolygon::container_type MultiPolygon::asArray() && noexcept
	{
		return std::move(m_polygons);
	}

	////////////////////////////////////////////////////////////////
	//
	//	at
	//
	////////////////////////////////////////////////////////////////

	inline const MultiPolygon::value_type& MultiPolygon::at(const size_type index) const&
	{
		return m_polygons.at(index);
	}

	inline MultiPolygon::value_type& MultiPolygon::at(const size_type index)&
	{
		return m_polygons.at(index);
	}

	inline MultiPolygon::value_type MultiPolygon::at(const size_type index)&&
	{
		return std::move(m_polygons.at(index));
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
	//	empty
	//
	////////////////////////////////////////////////////////////////

	inline bool MultiPolygon::empty() const noexcept
	{
		return m_polygons.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	inline bool MultiPolygon::isEmpty() const noexcept
	{
		return m_polygons.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	indexInBounds
	//
	////////////////////////////////////////////////////////////////

	inline bool MultiPolygon::indexInBounds(const size_type index) const noexcept
	{
		return (index < m_polygons.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon::operator bool() const noexcept
	{
		return (not m_polygons.empty());
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
	//	ssize
	//
	////////////////////////////////////////////////////////////////

	inline isize MultiPolygon::ssize() const noexcept
	{
		return m_polygons.ssize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	max_size
	//
	////////////////////////////////////////////////////////////////

	inline size_t MultiPolygon::max_size() const noexcept
	{
		return m_polygons.max_size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	reserve
	//
	////////////////////////////////////////////////////////////////

	inline void MultiPolygon::reserve(const size_type newCapacity)
	{
		m_polygons.reserve(newCapacity);
	}

	////////////////////////////////////////////////////////////////
	//
	//	capacity
	//
	////////////////////////////////////////////////////////////////

	inline size_t MultiPolygon::capacity() const noexcept
	{
		return m_polygons.capacity();
	}

	////////////////////////////////////////////////////////////////
	//
	//	shrink_to_fit
	//
	////////////////////////////////////////////////////////////////

	inline void MultiPolygon::shrink_to_fit()
	{
		m_polygons.shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	inline void MultiPolygon::clear() noexcept
	{
		m_polygons.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	inline void MultiPolygon::release()
	{
		m_polygons.release();
	}

	////////////////////////////////////////////////////////////////
	//
	//	insert
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon::iterator MultiPolygon::insert(const_iterator pos, const value_type& value)
	{
		return m_polygons.insert(pos, value);
	}

	inline MultiPolygon::iterator MultiPolygon::insert(const_iterator pos, value_type&& value)
	{
		return m_polygons.insert(pos, std::move(value));
	}

	template <std::input_iterator Iterator>
	MultiPolygon::iterator MultiPolygon::insert(const_iterator pos, Iterator first, Iterator last)
	{
		return m_polygons.insert(pos, first, last);
	}

	inline MultiPolygon::iterator MultiPolygon::insert(const_iterator pos, std::initializer_list<value_type> list)
	{
		return m_polygons.insert(pos, list);
	}

	////////////////////////////////////////////////////////////////
	//
	//	insert_range
	//
	////////////////////////////////////////////////////////////////

	template <Concept::ContainerCompatibleRange<Polygon> Range>
	MultiPolygon::iterator MultiPolygon::insert_range(const_iterator pos, Range&& range)
	{
		return m_polygons.insert_range(pos, std::forward<Range>(range));
	}

	////////////////////////////////////////////////////////////////
	//
	//	emplace
	//
	////////////////////////////////////////////////////////////////

	template <class... Args>
	MultiPolygon::iterator MultiPolygon::emplace(const_iterator pos, Args&&... args)
	{
		return m_polygons.emplace(pos, std::forward<Args>(args)...);
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

	////////////////////////////////////////////////////////////////
	//
	//	erase
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon::iterator MultiPolygon::erase(const_iterator pos)
	{
		return m_polygons.erase(pos);
	}

	inline MultiPolygon::iterator MultiPolygon::erase(const_iterator first, const_iterator last)
	{
		return m_polygons.erase(first, last);
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_at
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon& MultiPolygon::erase_at(const size_type index) &
	{
		m_polygons.erase_at(index);
		return *this;
	}

	inline MultiPolygon MultiPolygon::erase_at(const size_type index) &&
	{
		m_polygons.erase_at(index);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_at_unstable
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon& MultiPolygon::erase_at_unstable(const size_type index) &
	{
		m_polygons.erase_at_unstable(index);
		return *this;
	}

	inline MultiPolygon MultiPolygon::erase_at_unstable(const size_type index) &&
	{
		m_polygons.erase_at_unstable(index);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_all_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	MultiPolygon::size_type MultiPolygon::erase_all_if(Fty f)
		requires std::predicate<Fty&, const value_type&>
	{
		return m_polygons.erase_all_if(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_all_if_unstable
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	MultiPolygon::size_type MultiPolygon::erase_all_if_unstable(Fty f)
		requires std::predicate<Fty&, const value_type&>
	{
		return m_polygons.erase_all_if_unstable(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_first_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	bool MultiPolygon::erase_first_if(Fty f)
		requires std::predicate<Fty&, const value_type&>
	{
		return m_polygons.erase_first_if(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_first_if_unstable
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	bool MultiPolygon::erase_first_if_unstable(Fty f)
		requires std::predicate<Fty&, const value_type&>
	{
		return m_polygons.erase_first_if_unstable(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	push_back
	//
	////////////////////////////////////////////////////////////////

	inline void MultiPolygon::push_back(const value_type& value)
	{
		m_polygons.push_back(value);
	}

	inline void MultiPolygon::push_back(value_type&& value)
	{
		m_polygons.push_back(std::move(value));
	}

	////////////////////////////////////////////////////////////////
	//
	//	emplace_back
	//
	////////////////////////////////////////////////////////////////

	template <class... Args>
	MultiPolygon::reference MultiPolygon::emplace_back(Args&&... args)
	{
		return m_polygons.emplace_back(std::forward<Args>(args)...);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back
	//
	////////////////////////////////////////////////////////////////

	inline void MultiPolygon::pop_back()
	{
		m_polygons.pop_back();
	}

	////////////////////////////////////////////////////////////////
	//
	//	resize
	//
	////////////////////////////////////////////////////////////////

	inline void MultiPolygon::resize(const size_type newSize)
	{
		m_polygons.resize(newSize);
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	inline void MultiPolygon::swap(MultiPolygon& other) noexcept
	{
		m_polygons.swap(other.m_polygons);
	}

	////////////////////////////////////////////////////////////////
	//
	//	push_front
	//
	////////////////////////////////////////////////////////////////

	inline void MultiPolygon::push_front(const value_type& value)
	{
		m_polygons.push_front(value);
	}

	inline void MultiPolygon::push_front(value_type&& value)
	{
		m_polygons.push_front(std::move(value));
	}

	////////////////////////////////////////////////////////////////
	//
	//	emplace_front
	//
	////////////////////////////////////////////////////////////////

	template <class... Args>
	MultiPolygon::reference MultiPolygon::emplace_front(Args&&... args)
	{
		return m_polygons.emplace_front(std::forward<Args>(args)...);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_front
	//
	////////////////////////////////////////////////////////////////

	inline void MultiPolygon::pop_front()
	{
		m_polygons.pop_front();
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_front_N
	//
	////////////////////////////////////////////////////////////////

	inline void MultiPolygon::pop_front_N(const size_type count)
	{
		m_polygons.pop_front_N(count);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back_N
	//
	////////////////////////////////////////////////////////////////

	inline void MultiPolygon::pop_back_N(const size_type count)
	{
		m_polygons.pop_back_N(count);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon& MultiPolygon::operator <<(const value_type& value)
	{
		m_polygons.push_back(value);
		return *this;
	}

	inline MultiPolygon& MultiPolygon::operator <<(value_type&& value)
	{
		m_polygons.push_back(std::move(value));
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	subspan
	//
	////////////////////////////////////////////////////////////////

	inline std::span<MultiPolygon::value_type> MultiPolygon::subspan(const size_type offset, const size_type count) noexcept
	{
		return m_polygons.subspan(offset, count);
	}

	inline std::span<const MultiPolygon::value_type> MultiPolygon::subspan(const size_type offset, const size_type count) const noexcept
	{
		return m_polygons.subspan(offset, count);
	}

	////////////////////////////////////////////////////////////////
	//
	//	all
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	bool MultiPolygon::all(Fty f) const
		requires std::predicate<Fty&, const value_type&>
	{
		return m_polygons.all(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	any
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	bool MultiPolygon::any(Fty f) const
		requires std::predicate<Fty&, const value_type&>
	{
		return m_polygons.any(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	append
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon& MultiPolygon::append(const MultiPolygon& other)
	{
		m_polygons.append(other.m_polygons);
		return *this;
	}

	inline MultiPolygon& MultiPolygon::append(MultiPolygon&& other)
	{
		m_polygons.append(std::move(other.m_polygons));
		return *this;
	}

	inline MultiPolygon& MultiPolygon::append(const container_type& other)
	{
		m_polygons.append(other);
		return *this;
	}

	template <std::input_iterator Iterator>
	MultiPolygon& MultiPolygon::append(Iterator first, Iterator last)
	{
		m_polygons.append(first, last);
		return *this;
	}

	inline MultiPolygon& MultiPolygon::append(std::initializer_list<value_type> list)
	{
		m_polygons.append(list);
		return *this;
	}

	inline MultiPolygon& MultiPolygon::append(const size_type count, const value_type& value)
	{
		m_polygons.append(count, value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	choice
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon::value_type& MultiPolygon::choice()
	{
		return m_polygons.choice();
	}

	inline const MultiPolygon::value_type& MultiPolygon::choice() const
	{
		return m_polygons.choice();
	}

	inline MultiPolygon::value_type& MultiPolygon::choice(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return m_polygons.choice(urbg);
	}

	inline const MultiPolygon::value_type& MultiPolygon::choice(Concept::UniformRandomBitGenerator auto&& urbg) const
	{
		return m_polygons.choice(urbg);
	}

	inline MultiPolygon MultiPolygon::choice(const size_t n) const
	{
		return MultiPolygon{ m_polygons.choice(n) };
	}

	inline MultiPolygon MultiPolygon::choice(const size_t n, Concept::UniformRandomBitGenerator auto&& urbg) const
	{
		return MultiPolygon{ m_polygons.choice(n, urbg) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	chunk
	//
	////////////////////////////////////////////////////////////////

	inline Array<MultiPolygon> MultiPolygon::chunk(const size_type n) const
	{
		Array<MultiPolygon> result;

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
	//	contains_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	bool MultiPolygon::contains_if(Fty f) const
		requires std::predicate<Fty&, const value_type&>
	{
		return m_polygons.contains_if(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	count_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	isize MultiPolygon::count_if(Fty f) const
		requires std::predicate<Fty&, const value_type&>
	{
		return m_polygons.count_if(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	each
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	void MultiPolygon::each(Fty f)
		requires std::invocable<Fty&, value_type&>
	{
		m_polygons.each(std::forward<Fty>(f));
	}

	template <class Fty>
	void MultiPolygon::each(Fty f) const
		requires std::invocable<Fty&, const value_type&>
	{
		m_polygons.each(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	each_index
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	void MultiPolygon::each_index(Fty f)
		requires std::invocable<Fty&, size_t, value_type&>
	{
		m_polygons.each_index(std::forward<Fty>(f));
	}

	template <class Fty>
	void MultiPolygon::each_index(Fty f) const
		requires std::invocable<Fty&, size_t, const value_type&>
	{
		m_polygons.each_index(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	each_sindex
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	void MultiPolygon::each_sindex(Fty f)
		requires std::invocable<Fty&, isize, value_type&>
	{
		m_polygons.each_sindex(std::forward<Fty>(f));
	}

	template <class Fty>
	void MultiPolygon::each_sindex(Fty f) const
		requires std::invocable<Fty&, isize, const value_type&>
	{
		m_polygons.each_sindex(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	fetch
	//
	////////////////////////////////////////////////////////////////

	template <class U>
	MultiPolygon::value_type MultiPolygon::fetch(const size_type index, U&& defaultValue) const
		noexcept(std::is_nothrow_constructible_v<value_type, U> && std::is_nothrow_copy_constructible_v<value_type>)
		requires std::constructible_from<value_type, U>
	{
		return m_polygons.fetch(index, std::forward<U>(defaultValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	fill
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon& MultiPolygon::fill(const value_type& value)
	{
		m_polygons.fill(value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	filter
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	MultiPolygon MultiPolygon::filter(Fty f) const
		requires std::predicate<Fty&, const value_type&>
	{
		return MultiPolygon{ m_polygons.filter(std::forward<Fty>(f)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	fold_left
	//
	////////////////////////////////////////////////////////////////

	template <class R, class Fty>
	auto MultiPolygon::fold_left(R init, Fty f) const
	{
		return m_polygons.fold_left(std::move(init), std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	map
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	auto MultiPolygon::map(Fty f) const
		requires std::invocable<Fty&, const value_type&>
	{
		return m_polygons.map(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	none
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	bool MultiPolygon::none(Fty f) const
		requires std::predicate<Fty&, const value_type&>
	{
		return m_polygons.none(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	slice
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon MultiPolygon::slice(const size_type index, const size_type length) const&
	{
		return MultiPolygon{ m_polygons.slice(index, length) };
	}

	inline MultiPolygon MultiPolygon::slice(const size_type index, const size_type length) &&
	{
		return MultiPolygon{ std::move(m_polygons).slice(index, length) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	head
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon MultiPolygon::head(const size_type n) const&
	{
		return MultiPolygon{ m_polygons.head(n) };
	}

	inline MultiPolygon MultiPolygon::head(const size_type n) &&
	{
		return MultiPolygon{ std::move(m_polygons).head(n) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	head_span
	//
	////////////////////////////////////////////////////////////////

	inline std::span<MultiPolygon::value_type> MultiPolygon::head_span(const size_type n) & noexcept
	{
		return m_polygons.head_span(n);
	}

	inline std::span<const MultiPolygon::value_type> MultiPolygon::head_span(const size_type n) const& noexcept
	{
		return m_polygons.head_span(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	head_view
	//
	////////////////////////////////////////////////////////////////

	inline auto MultiPolygon::head_view(const size_type n) & noexcept
	{
		return m_polygons.head_view(n);
	}

	inline auto MultiPolygon::head_view(const size_type n) const& noexcept
	{
		return m_polygons.head_view(n);
	}

	inline auto MultiPolygon::head_view(const size_type n) && noexcept
	{
		return std::move(m_polygons).head_view(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	in_groups
	//
	////////////////////////////////////////////////////////////////

	inline Array<MultiPolygon> MultiPolygon::in_groups(const size_type group) const
	{
		Array<MultiPolygon> result;

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
	//	tail
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon MultiPolygon::tail(const size_type n) const&
	{
		return MultiPolygon{ m_polygons.tail(n) };
	}

	inline MultiPolygon MultiPolygon::tail(const size_type n) &&
	{
		return MultiPolygon{ std::move(m_polygons).tail(n) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	tail_span
	//
	////////////////////////////////////////////////////////////////

	inline std::span<MultiPolygon::value_type> MultiPolygon::tail_span(const size_type n) & noexcept
	{
		return m_polygons.tail_span(n);
	}

	inline std::span<const MultiPolygon::value_type> MultiPolygon::tail_span(const size_type n) const& noexcept
	{
		return m_polygons.tail_span(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	tail_view
	//
	////////////////////////////////////////////////////////////////

	inline auto MultiPolygon::tail_view(const size_type n) & noexcept
	{
		return m_polygons.tail_view(n);
	}

	inline auto MultiPolygon::tail_view(const size_type n) const& noexcept
	{
		return m_polygons.tail_view(n);
	}

	inline auto MultiPolygon::tail_view(const size_type n) && noexcept
	{
		return std::move(m_polygons).tail_view(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	take
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon MultiPolygon::take(const size_type n) const&
	{
		return MultiPolygon{ m_polygons.take(n) };
	}

	inline MultiPolygon MultiPolygon::take(const size_type n) &&
	{
		return MultiPolygon{ std::move(m_polygons).take(n) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	take_while
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	MultiPolygon MultiPolygon::take_while(Fty f) const&
		requires std::predicate<Fty&, const value_type&>
	{
		return MultiPolygon{ m_polygons.take_while(std::forward<Fty>(f)) };
	}

	template <class Fty>
	MultiPolygon MultiPolygon::take_while(Fty f) &&
		requires std::predicate<Fty&, const value_type&>
	{
		return MultiPolygon{ std::move(m_polygons).take_while(std::forward<Fty>(f)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	values_at
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon MultiPolygon::values_at(const std::initializer_list<size_type> indices) const
	{
		return MultiPolygon{ m_polygons.values_at(indices) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	without_at
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon MultiPolygon::without_at(const size_type index) const&
	{
		return MultiPolygon{ m_polygons.without_at(index) };
	}

	inline MultiPolygon MultiPolygon::without_at(const size_type index) &&
	{
		return MultiPolygon{ std::move(m_polygons).without_at(index) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	without_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	MultiPolygon MultiPolygon::without_if(Fty f) const&
		requires std::predicate<Fty&, const value_type&>
	{
		return MultiPolygon{ m_polygons.without_if(std::forward<Fty>(f)) };
	}

	template <class Fty>
	MultiPolygon MultiPolygon::without_if(Fty f) &&
		requires std::predicate<Fty&, const value_type&>
	{
		return MultiPolygon{ std::move(m_polygons).without_if(std::forward<Fty>(f)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	replace_if, replaced_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	MultiPolygon& MultiPolygon::replace_if(Fty f, const value_type& newValue) &
		requires std::predicate<Fty&, const value_type&>
	{
		m_polygons.replace_if(std::forward<Fty>(f), newValue);
		return *this;
	}

	template <class Fty>
	MultiPolygon MultiPolygon::replace_if(Fty f, const value_type& newValue) &&
		requires std::predicate<Fty&, const value_type&>
	{
		return MultiPolygon{ std::move(m_polygons).replace_if(std::forward<Fty>(f), newValue) };
	}

	template <class Fty>
	MultiPolygon MultiPolygon::replaced_if(Fty f, const value_type& newValue) const&
		requires std::predicate<Fty&, const value_type&>
	{
		return MultiPolygon{ m_polygons.replaced_if(std::forward<Fty>(f), newValue) };
	}

	template <class Fty>
	MultiPolygon MultiPolygon::replaced_if(Fty f, const value_type& newValue) &&
		requires std::predicate<Fty&, const value_type&>
	{
		return MultiPolygon{ std::move(m_polygons).replaced_if(std::forward<Fty>(f), newValue) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse, reversed
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon& MultiPolygon::reverse() &
	{
		m_polygons.reverse();
		return *this;
	}

	inline MultiPolygon MultiPolygon::reverse() &&
	{
		return MultiPolygon{ std::move(m_polygons).reverse() };
	}

	inline MultiPolygon MultiPolygon::reversed() const&
	{
		return MultiPolygon{ m_polygons.reversed() };
	}

	inline MultiPolygon MultiPolygon::reversed() &&
	{
		return MultiPolygon{ std::move(m_polygons).reversed() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse_each
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	void MultiPolygon::reverse_each(Fty f)
		requires std::invocable<Fty&, value_type&>
	{
		m_polygons.reverse_each(std::forward<Fty>(f));
	}

	template <class Fty>
	void MultiPolygon::reverse_each(Fty f) const
		requires std::invocable<Fty&, const value_type&>
	{
		m_polygons.reverse_each(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse_view
	//
	////////////////////////////////////////////////////////////////

	inline auto MultiPolygon::reverse_view() &
	{
		return m_polygons.reverse_view();
	}

	inline auto MultiPolygon::reverse_view() const&
	{
		return m_polygons.reverse_view();
	}

	inline auto MultiPolygon::reverse_view() &&
	{
		return std::move(m_polygons).reverse_view();
	}

	////////////////////////////////////////////////////////////////
	//
	//	shuffle, shuffled
	//
	////////////////////////////////////////////////////////////////

	inline MultiPolygon& MultiPolygon::shuffle() &
	{
		m_polygons.shuffle();
		return *this;
	}

	inline MultiPolygon MultiPolygon::shuffle() &&
	{
		return MultiPolygon{ std::move(m_polygons).shuffle() };
	}

	inline MultiPolygon MultiPolygon::shuffled() const&
	{
		return MultiPolygon{ m_polygons.shuffled() };
	}

	inline MultiPolygon MultiPolygon::shuffled() &&
	{
		return MultiPolygon{ std::move(m_polygons).shuffled() };
	}

	inline MultiPolygon& MultiPolygon::shuffle(Concept::UniformRandomBitGenerator auto&& urbg) &
	{
		m_polygons.shuffle(urbg);
		return *this;
	}

	inline MultiPolygon MultiPolygon::shuffle(Concept::UniformRandomBitGenerator auto&& urbg) &&
	{
		return MultiPolygon{ std::move(m_polygons).shuffle(urbg) };
	}

	inline MultiPolygon MultiPolygon::shuffled(Concept::UniformRandomBitGenerator auto&& urbg) const&
	{
		return MultiPolygon{ m_polygons.shuffled(urbg) };
	}

	inline MultiPolygon MultiPolygon::shuffled(Concept::UniformRandomBitGenerator auto&& urbg) &&
	{
		return MultiPolygon{ std::move(m_polygons).shuffled(urbg) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	sort_by, sorted_by
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	MultiPolygon& MultiPolygon::sort_by(Fty f) &
		requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
	{
		m_polygons.sort_by(std::forward<Fty>(f));
		return *this;
	}

	template <class Fty>
	MultiPolygon MultiPolygon::sort_by(Fty f) &&
		requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
	{
		return MultiPolygon{ std::move(m_polygons).sort_by(std::forward<Fty>(f)) };
	}

	template <class Fty>
	MultiPolygon MultiPolygon::sorted_by(Fty f) const&
		requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
	{
		return MultiPolygon{ m_polygons.sorted_by(std::forward<Fty>(f)) };
	}

	template <class Fty>
	MultiPolygon MultiPolygon::sorted_by(Fty f) &&
		requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
	{
		return MultiPolygon{ std::move(m_polygons).sorted_by(std::forward<Fty>(f)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	stable_sort_by, stable_sorted_by
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	MultiPolygon& MultiPolygon::stable_sort_by(Fty f) &
		requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
	{
		m_polygons.stable_sort_by(std::forward<Fty>(f));
		return *this;
	}

	template <class Fty>
	MultiPolygon MultiPolygon::stable_sort_by(Fty f) &&
		requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
	{
		return MultiPolygon{ std::move(m_polygons).stable_sort_by(std::forward<Fty>(f)) };
	}

	template <class Fty>
	MultiPolygon MultiPolygon::stable_sorted_by(Fty f) const&
		requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
	{
		return MultiPolygon{ m_polygons.stable_sorted_by(std::forward<Fty>(f)) };
	}

	template <class Fty>
	MultiPolygon MultiPolygon::stable_sorted_by(Fty f) &&
		requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
	{
		return MultiPolygon{ std::move(m_polygons).stable_sorted_by(std::forward<Fty>(f)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator >>
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	auto MultiPolygon::operator >>(Fty f) const
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
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool MultiPolygon::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersects(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	overlaps
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool MultiPolygon::overlaps(const Shape2DType& other) const
	{
		return Geometry2D::Overlaps(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool MultiPolygon::contains(const Shape2DType& other) const
	{
		return Geometry2D::Contains(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersectsAt
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	Optional<Array<Vec2>> MultiPolygon::intersectsAt(const Shape2DType& other) const
	{
		return Geometry2D::IntersectsAt(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	paint
	//
	////////////////////////////////////////////////////////////////

	inline const MultiPolygon& MultiPolygon::paint(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.paint(dst, color, enableAntialiasing);
		}

		return *this;
	}

	inline const MultiPolygon& MultiPolygon::paint(Image& dst, const Vec2& offset, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.paint(dst, offset, color, enableAntialiasing);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	inline const MultiPolygon& MultiPolygon::draw(const ColorF& color) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.draw(color);
		}

		return *this;
	}

	inline const MultiPolygon& MultiPolygon::draw(const Vec2& offset, const ColorF& color) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.draw(offset, color);
		}

		return *this;
	}

	inline const MultiPolygon& MultiPolygon::draw(const PatternParameters& pattern) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.draw(pattern);
		}

		return *this;
	}

	inline const MultiPolygon& MultiPolygon::draw(const Vec2& offset, const PatternParameters& pattern) const
	{
		for (const auto& polygon : m_polygons)
		{
			polygon.draw(offset, pattern);
		}

		return *this;
	}
}
