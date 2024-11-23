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
	namespace detail
	{
		[[noreturn]]
		void ThrowArrayRemoveAtIndexOutOfRange();

		[[noreturn]]
		void ThrowArrayRemovedAtIndexOutOfRange();

		[[noreturn]]
		void ThrowArrayChoiceOutOfRange();

		[[noreturn]]
		void ThrowArrayValuesAtIndexOutOfRange();
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(const container_type& other)
		: m_container(other) {}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(container_type&& other)
		: m_container(std::move(other)) {}

	template<class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(const Allocator& alloc) noexcept
		: m_container(alloc) {}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(const size_type count, const value_type& value, const Allocator& alloc)
		: m_container(count, value, alloc) {}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(const size_type count, const Allocator& alloc)
		: m_container(count, alloc) {}

	template <class Type, class Allocator>
	template <class Iterator>
	constexpr Array<Type, Allocator>::Array(Iterator first, Iterator last, const Allocator& alloc)
		: m_container(first, last, alloc) {}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(const Array& other, const Allocator& alloc)
		: m_container(other.m_container, alloc) {}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(Array&& other, const Allocator& alloc)
		: m_container(std::move(other.m_container), alloc) {}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(const container_type& other, const Allocator& alloc)
		: m_container(other, alloc) {}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(container_type&& other, const Allocator& alloc)
		: m_container(std::move(other), alloc) {}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(const HasAsArray auto& a)
		: Array{ a.asArray() } {}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(HasAsArray auto&& a)
		: Array{ std::forward<decltype(a)>(a).asArray() } {}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(const std::initializer_list<value_type> list, const Allocator& alloc)
		: m_container(list, alloc) {}

# if __cpp_lib_containers_ranges >= 202202L
	   
	template <class Type, class Allocator>
	template <Concept::ContainerCompatibleRange<Type> Range>
	constexpr Array<Type, Allocator>::Array(std::from_range_t, Range&& range, const Allocator& alloc)
		: m_container(std::from_range, std::forward<Range>(range), alloc) {}

# endif

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(const size_type size, Arg::generator_<FunctionRef<value_type()>> generator)
		: Array(Generate(size, *generator)) {}

		template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(const size_type size, Arg::generator_<FunctionRef<value_type(size_t)>> generator)
		: Array(IndexedGenerate(size, *generator)) {}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::Array(const Arg::reserve_<size_type> size)
	{
		m_container.reserve(*size);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::operator =(const container_type& other)
	{
		m_container = other;
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::operator =(container_type&& other)
	{
		m_container = std::move(other);
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::operator =(const HasAsArray auto& a)
	{
		m_container = a.asArray();
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::operator =(HasAsArray auto&& a)
	{
		m_container = std::forward<decltype(a)>(a).asArray();
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::operator =(const std::initializer_list<value_type> list)
	{
		m_container = list;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	assign
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::assign(const size_type count, const value_type& value)
	{
		m_container.assign(count, value);
		return *this;
	}

	template <class Type, class Allocator>
	template <class Iterator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::assign(Iterator first, Iterator last)
	{
		m_container.assign(first, last);
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::assign(const std::initializer_list<value_type> list)
	{
		m_container.assign(list);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	assign_range
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <Concept::ContainerCompatibleRange<Type> Range>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::assign_range(Range&& range)
	{
	# if __cpp_lib_containers_ranges >= 202202L
		m_container.assign_range(std::forward<Range>(range));
	# else
		auto common_range = std::views::common(std::forward<Range>(range));
		m_container.assign(std::ranges::begin(common_range), std::ranges::end(common_range));
	# endif
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	get_allocator
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::allocator_type Array<Type, Allocator>::get_allocator() const noexcept
	{
		return m_container.get_allocator();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getContainer
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr const typename Array<Type, Allocator>::container_type& Array<Type, Allocator>::getContainer() const& noexcept
	{
		return m_container;
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::container_type Array<Type, Allocator>::getContainer() && noexcept
	{
		return std::move(m_container);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator container_type
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::operator typename Array<Type, Allocator>::container_type() const& noexcept
	{
		return m_container;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::operator typename Array<Type, Allocator>::container_type() && noexcept
	{
		return std::move(m_container);
	}

	////////////////////////////////////////////////////////////////
	//
	//	at
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr const typename Array<Type, Allocator>::value_type& Array<Type, Allocator>::at(const size_type index) const&
	{
		return m_container.at(index);
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::value_type& Array<Type, Allocator>::at(const size_type index) &
	{
		return m_container.at(index);
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::value_type Array<Type, Allocator>::at(const size_type index) &&
	{
		return std::move(m_container.at(index));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr const typename Array<Type, Allocator>::value_type& Array<Type, Allocator>::operator[](const size_type index) const& noexcept
	{
		return m_container[index];
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::value_type& Array<Type, Allocator>::operator[](const size_type index) & noexcept
	{
		return m_container[index];
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::value_type Array<Type, Allocator>::operator[](const size_type index) && noexcept
	{
		return std::move(m_container[index]);
	}

	////////////////////////////////////////////////////////////////
	//
	//	front
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::reference Array<Type, Allocator>::front()& noexcept
	{
		return m_container.front();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::const_reference Array<Type, Allocator>::front() const& noexcept
	{
		return m_container.front();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::value_type Array<Type, Allocator>::front()&& noexcept
	{
		return std::move(m_container.front());
	}

	////////////////////////////////////////////////////////////////
	//
	//	back
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::reference Array<Type, Allocator>::back()& noexcept
	{
		return m_container.back();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::const_reference Array<Type, Allocator>::back() const& noexcept
	{
		return m_container.back();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::value_type Array<Type, Allocator>::back() && noexcept
	{
		return std::move(m_container.back());
	}

	////////////////////////////////////////////////////////////////
	//
	//	data
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::value_type* Array<Type, Allocator>::data() noexcept
	{
		return m_container.data();
	}

	template <class Type, class Allocator>
	constexpr const typename Array<Type, Allocator>::value_type* Array<Type, Allocator>::data() const noexcept
	{
		return m_container.data();
	}

	////////////////////////////////////////////////////////////////
	//
	//	begin, end
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::iterator Array<Type, Allocator>::begin() noexcept
	{
		return m_container.begin();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::iterator Array<Type, Allocator>::end() noexcept
	{
		return m_container.end();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::const_iterator Array<Type, Allocator>::begin() const noexcept
	{
		return m_container.begin();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::const_iterator Array<Type, Allocator>::end() const noexcept
	{
		return m_container.end();
	}

	////////////////////////////////////////////////////////////////
	//
	//	cbegin, cend
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::const_iterator Array<Type, Allocator>::cbegin() const noexcept
	{
		return m_container.cbegin();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::const_iterator Array<Type, Allocator>::cend() const noexcept
	{
		return m_container.cend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	rbegin, rend
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::reverse_iterator Array<Type, Allocator>::rbegin() noexcept
	{
		return m_container.rbegin();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::reverse_iterator Array<Type, Allocator>::rend() noexcept
	{
		return m_container.rend();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::const_reverse_iterator Array<Type, Allocator>::rbegin() const noexcept
	{
		return m_container.rbegin();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::const_reverse_iterator Array<Type, Allocator>::rend() const noexcept
	{
		return m_container.rend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	crbegin, crend
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::const_reverse_iterator Array<Type, Allocator>::crbegin() const noexcept
	{
		return m_container.crbegin();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::const_reverse_iterator Array<Type, Allocator>::crend() const noexcept
	{
		return m_container.crend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	empty
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr bool Array<Type, Allocator>::empty() const noexcept
	{
		return m_container.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr bool Array<Type, Allocator>::isEmpty() const noexcept
	{
		return m_container.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	indexInBounds
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr bool Array<Type, Allocator>::indexInBounds(const size_type index) const noexcept
	{
		return (index < m_container.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	explicit operator bool
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::operator bool() const noexcept
	{
		return (not m_container.empty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr size_t Array<Type, Allocator>::size() const noexcept
	{
		return m_container.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	ssize
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr isize Array<Type, Allocator>::ssize() const noexcept
	{
		return static_cast<isize>(m_container.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	size_bytes
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr size_t Array<Type, Allocator>::size_bytes() const noexcept requires (Concept::TriviallyCopyable<value_type>)
	{
		return (m_container.size() * sizeof(value_type));
	}

	////////////////////////////////////////////////////////////////
	//
	//	max_size
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr size_t Array<Type, Allocator>::max_size() const noexcept
	{
		return m_container.max_size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	reserve
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::reserve(const size_type n)
	{
		m_container.reserve(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	capacity
	//
	////////////////////////////////////////////////////////////////

	template<class Type, class Allocator>
	constexpr size_t Array<Type, Allocator>::capacity() const noexcept
	{
		return m_container.capacity();
	}

	////////////////////////////////////////////////////////////////
	//
	//	shrink_to_fit
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::shrink_to_fit()
	{
		m_container.shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::clear() noexcept
	{
		m_container.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::release()
	{
		m_container.clear();
		m_container.shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////
	//
	//	insert
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::iterator Array<Type, Allocator>::insert(const const_iterator pos, const value_type& value)
	{
		return m_container.insert(pos, value);
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::iterator Array<Type, Allocator>::insert(const const_iterator pos, value_type&& value)
	{
		return m_container.insert(pos, std::move(value));
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::iterator Array<Type, Allocator>::insert(const const_iterator pos, const size_type count, const value_type& value)
	{
		return m_container.insert(pos, count, value);
	}

	template <class Type, class Allocator>
	template <class Iterator>
	constexpr typename Array<Type, Allocator>::iterator Array<Type, Allocator>::insert(const const_iterator pos, Iterator first, Iterator last)
	{
		return m_container.insert(pos, first, last);
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::iterator Array<Type, Allocator>::insert(const const_iterator pos, const std::initializer_list<value_type> list)
	{
		return m_container.insert(pos, list);
	}

	////////////////////////////////////////////////////////////////
	//
	//	emplace
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class... Args>
	constexpr typename Array<Type, Allocator>::iterator Array<Type, Allocator>::emplace(const const_iterator pos, Args&&... args)
	{
		return m_container.emplace(pos, std::forward<Args>(args)...);
	}

	////////////////////////////////////////////////////////////////
	//
	//	append_range
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <Concept::ContainerCompatibleRange<Type> Range>
	constexpr void Array<Type, Allocator>::append_range(Range&& range)
	{
	# if __cpp_lib_containers_ranges >= 202202L
		m_container.append_range(std::forward<Range>(range));
	# else
		auto common_range = std::views::common(std::forward<Range>(range));
		m_container.insert(m_container.end(), std::ranges::begin(common_range), std::ranges::end(common_range));
	# endif
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::iterator Array<Type, Allocator>::erase(const_iterator pos)
	{
		return m_container.erase(pos);
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::iterator Array<Type, Allocator>::erase(const_iterator first, const_iterator last)
	{
		return m_container.erase(first, last);
	}

	////////////////////////////////////////////////////////////////
	//
	//	push_back
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::push_back(const value_type& value)
	{
		m_container.push_back(value);
	}

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::push_back(value_type&& value)
	{
		m_container.push_back(std::move(value));
	}

	////////////////////////////////////////////////////////////////
	//
	//	emplace_back
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class... Args>
	constexpr typename Array<Type, Allocator>::reference Array<Type, Allocator>::emplace_back(Args&&... args)
	{
		return m_container.emplace_back(std::forward<Args>(args)...);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::pop_back()
	{
		m_container.pop_back();
	}

	////////////////////////////////////////////////////////////////
	//
	//	resize
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::resize(const size_type count)
	{
		m_container.resize(count);
	}

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::resize(const size_type count, const value_type& value)
	{
		m_container.resize(count, value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::swap(Array& other) noexcept
	{
		m_container.swap(other.m_container);
	}

	////////////////////////////////////////////////////////////////
	//
	//	push_front
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::push_front(const value_type& value)
	{
		m_container.insert(m_container.begin(), value);
	}

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::push_front(value_type&& value)
	{
		m_container.insert(m_container.begin(), std::move(value));
	}

	////////////////////////////////////////////////////////////////
	//
	//	emplace_front
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class... Args>
	constexpr typename Array<Type, Allocator>::reference Array<Type, Allocator>::emplace_front(Args&&... args)
	{
		return m_container.emplace(m_container.begin(), std::forward<Args>(args)...);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_front
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::pop_front()
	{
		m_container.erase(m_container.begin());
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_front_N
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::pop_front_N(const size_type n)
	{
		m_container.erase(m_container.begin(), (m_container.begin() + Min(n, m_container.size())));
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back_N
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Array<Type, Allocator>::pop_back_N(const size_type n)
	{
		m_container.erase((m_container.end() - Min(n, m_container.size())), m_container.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::operator <<(const value_type& value)
	{
		m_container.push_back(value);
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::operator <<(value_type&& value)
	{
		m_container.push_back(std::move(value));
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	subspan
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr std::span<typename Array<Type, Allocator>::value_type> Array<Type, Allocator>::subspan(const size_type pos, const size_type count) noexcept
	{
		return std::span{ m_container }.subspan(pos, count);
	}

	template <class Type, class Allocator>
	constexpr std::span<const typename Array<Type, Allocator>::value_type> Array<Type, Allocator>::subspan(const size_type pos, const size_type count) const noexcept
	{
		return std::span{ m_container }.subspan(pos, count);
	}

	////////////////////////////////////////////////////////////////
	//
	//	all
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr bool Array<Type, Allocator>::all(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return std::all_of(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	any
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr bool Array<Type, Allocator>::any(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return std::any_of(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	append
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::append(const Array& other)
	{
		m_container.insert(m_container.end(), other.m_container.begin(), other.m_container.end());
		return *this;
	}

	template <class Type, class Allocator>
	template <class Iterator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::append(Iterator first, Iterator last)
	{
		m_container.insert(m_container.end(), first, last);
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::append(std::initializer_list<value_type> list)
	{
		m_container.insert(m_container.end(), list);
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::append(const size_type count, const value_type& value)
	{
		m_container.insert(m_container.end(), count, value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	chunk
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Array<typename Array<Type, Allocator>::value_type>> Array<Type, Allocator>::chunk(const size_type n) const
	{
		Array<Array<value_type>> result;

		if (n == 0)
		{
			return result;
		}

		for (size_t i = 0; i < (size() + n - 1) / n; ++i)
		{
			result.push_back(slice((i * n), n));
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr bool Array<Type, Allocator>::contains(const value_type& value) const
	{
		return (std::find(m_container.begin(), m_container.end(), value) != m_container.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains_if
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr bool Array<Type, Allocator>::contains_if(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return std::any_of(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	count
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr isize Array<Type, Allocator>::count(const value_type& value) const
	{
		return std::count(m_container.begin(), m_container.end(), value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	count_if
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr isize Array<Type, Allocator>::count_if(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return std::count_if(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	each
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr void Array<Type, Allocator>::each(Fty f) requires std::invocable<Fty&, value_type&>
	{
		std::for_each(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr void Array<Type, Allocator>::each(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		std::for_each(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	each_index
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr void Array<Type, Allocator>::each_index(Fty f) requires std::invocable<Fty&, size_t, value_type&>
	{
		for (size_t i = 0; auto& elem : m_container)
		{
			f(i++, elem);
		}
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr void Array<Type, Allocator>::each_index(Fty f) const requires std::invocable<Fty&, size_t, const value_type&>
	{
		for (size_t i = 0; const auto& elem : m_container)
		{
			f(i++, elem);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	each_sindex
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr void Array<Type, Allocator>::each_sindex(Fty f) requires std::invocable<Fty&, isize, value_type&>
	{
		for (isize i = 0; auto& elem : m_container)
		{
			f(i++, elem);
		}
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr void Array<Type, Allocator>::each_sindex(Fty f) const requires std::invocable<Fty&, isize, const value_type&>
	{
		for (isize i = 0; const auto& elem : m_container)
		{
			f(i++, elem);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	fetch
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class U>
	constexpr typename Array<Type, Allocator>::value_type Array<Type, Allocator>::fetch(const size_type index, U&& defaultValue) const noexcept(std::is_nothrow_constructible_v<value_type, U>) requires std::constructible_from<value_type, U>
	{
		if (m_container.size() <= index)
		{
			return value_type(std::forward<U>(defaultValue));
		}

		return m_container[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	fill
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::fill(const value_type& value)
	{
		std::fill(m_container.begin(), m_container.end(), value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	filter
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::filter(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		Array result;

		for (const auto& value : m_container)
		{
			if (f(value))
			{
				result.push_back(value);
			}
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	in_groups
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Array<typename Array<Type, Allocator>::value_type>> Array<Type, Allocator>::in_groups(const size_type group) const
	{
		Array<Array<value_type>> result;

		if (group == 0)
		{
			return result;
		}

		const size_t div = (m_container.size() / group);
		const size_t mod = (m_container.size() % group);
		size_t index = 0;

		for (size_t i = 0; i < group; ++i)
		{
			const size_t length = (div + ((0 < mod) && (i < mod)));

			result.push_back(slice(index, length));

			index += length;
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isSorted
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr bool Array<Type, Allocator>::isSorted() const requires Concept::LessThanComparable<value_type>
	{
		return std::is_sorted(m_container.begin(), m_container.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	join
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr String Array<Type, Allocator>::join(const StringView sep, const StringView begin, const StringView end) const
	{
		String result;

		result.append(begin);

		bool isFirst = true;

		for (const auto& value : m_container)
		{
			if (isFirst)
			{
				isFirst = false;
			}
			else
			{
				result.append(sep);
			}

			result.append(Format(value));
		}

		result.append(end);

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	map
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr auto Array<Type, Allocator>::map(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		using result_value_type = std::decay_t<std::invoke_result_t<Fty&, const value_type&>>;

		Array<result_value_type> result;

		result.reserve(m_container.size());

		for (const auto& value : m_container)
		{
			result.push_back(f(value));
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	none
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr bool Array<Type, Allocator>::none(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return std::none_of(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	partition
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr auto Array<Type, Allocator>::partition(Fty f) requires std::predicate<Fty&, const value_type&>
	{
		return std::partition(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove, removed
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::remove(const value_type& value)&
	{
		m_container.erase(std::remove(m_container.begin(), m_container.end(), value), m_container.end());
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::remove(const value_type& value)&&
	{
		return std::move(remove(value));
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::removed(const value_type& value) const&
	{
		Array result(Arg::reserve = m_container.size());

		for (const auto& v : m_container)
		{
			if (v != value)
			{
				result.push_back(v);
			}
		}

		return result;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::removed(const value_type& value)&&
	{
		return std::move(remove(value));
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove_at, removed_at
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::remove_at(const size_type index)&
	{
		if (m_container.size() <= index)
		{
			detail::ThrowArrayRemoveAtIndexOutOfRange();
		}

		erase(m_container.begin() + index);

		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::remove_at(const size_type index)&&
	{
		return std::move(remove_at(index));
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::removed_at(const size_type index) const&
	{
		if (m_container.size() <= index)
		{
			detail::ThrowArrayRemovedAtIndexOutOfRange();
		}

		Array result(Arg::reserve = m_container.size() - 1);
		result.insert(result.end(), m_container.begin(), (m_container.begin() + index));
		result.insert(result.end(), (m_container.begin() + index + 1), m_container.end());

		return result;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::removed_at(const size_type index)&&
	{
		return std::move(remove_at(index));
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove_if, removed_if
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::remove_if(Fty f)& requires std::predicate<Fty&, const value_type&>
	{
		erase(std::remove_if(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f))), m_container.end());
		return *this;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::remove_if(Fty f) && requires std::predicate<Fty&, const value_type&>
	{
		return std::move(remove_if(std::forward<Fty>(f)));
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::removed_if(Fty f) const& requires std::predicate<Fty&, const value_type&>
	{
		Array result;

		for (const auto& v : m_container)
		{
			if (not f(v))
			{
				result.push_back(v);
			}
		}

		return result;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::removed_if(Fty f) && requires std::predicate<Fty&, const value_type&>
	{
		return std::move(remove_if(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	replace, replaced
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::replace(const value_type& oldValue, const value_type& newValue)&
	{
		std::replace(m_container.begin(), m_container.end(), oldValue, newValue);
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::replace(const value_type& oldValue, const value_type& newValue)&&
	{
		return std::move(replace(oldValue, newValue));
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::replaced(const value_type& oldValue, const value_type& newValue) const&
	{
		Array result(Arg::reserve = m_container.size());

		for (const auto& v : m_container)
		{
			result.push_back((v == oldValue) ? newValue : v);
		}

		return result;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::replaced(const value_type& oldValue, const value_type& newValue)&&
	{
		return std::move(replace(oldValue, newValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	replace_if, replaced_if
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::replace_if(Fty f, const value_type& newValue)& requires std::predicate<Fty&, const value_type&>
	{
		std::replace_if(m_container.begin(), m_container.end(), f, newValue);
		return *this;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::replace_if(Fty f, const value_type& newValue) && requires std::predicate<Fty&, const value_type&>
	{
		return std::move(replace_if(f, newValue));
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::replaced_if(Fty f, const value_type& newValue) const& requires std::predicate<Fty&, const value_type&>
	{
		Array result(Arg::reserve = m_container.size());

		for (const auto& v : m_container)
		{
			result.push_back(f(v) ? newValue : v);
		}

		return result;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::replaced_if(Fty f, const value_type& newValue) && requires std::predicate<Fty&, const value_type&>
	{
		return std::move(replace_if(f, newValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse, reversed
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::reverse()&
	{
		std::reverse(m_container.begin(), m_container.end());
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::reverse()&&
	{
		return std::move(reverse());
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::reversed() const&
	{
		return Array(m_container.rbegin(), m_container.rend());
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::reversed()&&
	{
		return std::move(reverse());
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse_each
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr void Array<Type, Allocator>::reverse_each(Fty f) requires std::invocable<Fty&, value_type&>
	{
		std::for_each(m_container.rbegin(), m_container.rend(), detail::PassFunction(std::forward<Fty>(f)));
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr void Array<Type, Allocator>::reverse_each(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		std::for_each(m_container.rbegin(), m_container.rend(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate, rotated
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::rotate(const size_type middle)&
	{
		std::rotate(m_container.begin(), (m_container.begin() + middle), m_container.end());
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::rotate(const size_type middle)&&
	{
		return std::move(rotate(middle));
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::rotated(const size_type middle) const&
	{
		Array result(Arg::reserve = m_container.size());

		result.insert(result.end(), (m_container.begin() + middle), m_container.end());

		result.insert(result.end(), m_container.begin(), (m_container.begin() + middle));

		return result;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::rotated(const size_type middle)&&
	{
		return std::move(rotate(middle));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rsort, rsorted
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::rsort()& requires Concept::LessThanComparable<value_type>
	{
		std::sort(m_container.begin(), m_container.end(), [](const Type& a, const Type& b) { return b < a; });
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::rsort() && requires Concept::LessThanComparable<value_type>
	{
		return std::move(rsort());
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::rsorted() const& requires Concept::LessThanComparable<value_type>
	{
		Array result(*this);
		result.rsort();
		return result;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::rsorted() && requires Concept::LessThanComparable<value_type>
	{
		return std::move(rsort());
	}

	////////////////////////////////////////////////////////////////
	//
	//	slice
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::slice(const size_type index, const size_type length) const&
	{
		return Array((m_container.begin() + index), (m_container.begin() + index + length));
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::slice(const size_type index, const size_type length)&&
	{
		return Array(std::make_move_iterator(m_container.begin() + index), std::make_move_iterator(m_container.begin() + index + length));
	}

	////////////////////////////////////////////////////////////////
	//
	//	sort, sorted
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::sort()& requires Concept::LessThanComparable<value_type>
	{
		std::sort(m_container.begin(), m_container.end());
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::sort() && requires Concept::LessThanComparable<value_type>
	{
		return std::move(sort());
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::sorted() const& requires Concept::LessThanComparable<value_type>
	{
		Array result(*this);
		result.sort();
		return result;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::sorted() && requires Concept::LessThanComparable<value_type>
	{
		return std::move(sort());
	}

	////////////////////////////////////////////////////////////////
	//
	//	sort_and_unique, sorted_and_uniqued
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::sort_and_unique() & noexcept
	{
		std::sort(m_container.begin(), m_container.end());
		m_container.erase(std::unique(m_container.begin(), m_container.end()), m_container.end());
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::sort_and_unique() && noexcept
	{
		return std::move(sort_and_unique());
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::sorted_and_uniqued() const&
	{
		Array result(*this);
		result.sort_and_unique();
		return result;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::sorted_and_uniqued() && noexcept
	{
		return std::move(sort_and_unique());
	}

	////////////////////////////////////////////////////////////////
	//
	//	sort_by, sorted_by
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::sort_by(Fty f)& requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		std::sort(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
		return *this;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::sort_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		return std::move(sort_by(std::forward<Fty>(f)));
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::sorted_by(Fty f) const& requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		Array result(*this);
		result.sort_by(std::forward<Fty>(f));
		return result;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::sorted_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		return std::move(sort_by(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	stable_partition
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr auto Array<Type, Allocator>::stable_partition(Fty f) requires std::predicate<Fty&, const value_type&>
	{
		return std::stable_partition(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	stable_sort, stable_sorted
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::stable_sort()& requires Concept::LessThanComparable<value_type>
	{
		std::stable_sort(m_container.begin(), m_container.end());
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::stable_sort() && requires Concept::LessThanComparable<value_type>
	{
		return std::move(stable_sort());
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::stable_sorted() const& requires Concept::LessThanComparable<value_type>
	{
		Array result(*this);
		result.stable_sort();
		return result;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::stable_sorted() && requires Concept::LessThanComparable<value_type>
	{
		return std::move(stable_sort());
	}

	////////////////////////////////////////////////////////////////
	//
	//	stable_sort_by, stable_sorted_by
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::stable_sort_by(Fty f)& requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		std::stable_sort(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
		return *this;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::stable_sort_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		return std::move(stable_sort_by(std::forward<Fty>(f)));
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::stable_sorted_by(Fty f) const& requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		Array result(*this);
		result.stable_sort_by(std::forward<Fty>(f));
		return result;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::stable_sorted_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		return std::move(stable_sort_by(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	sum
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr auto Array<Type, Allocator>::sum() const requires (Concept::Addable<value_type> || Concept::AddAssignable<value_type>)
	{
		decltype(std::declval<Type>() + std::declval<Type>()) result{};

		if constexpr (Concept::AddAssignable<Type>)
		{
			for (const auto& elem : m_container)
			{
				result += elem;
			}
		}
		else
		{
			for (const auto& elem : m_container)
			{
				result = (result + elem);
			}
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	sumF
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr auto Array<Type, Allocator>::sumF() const requires Concept::FloatingPoint<value_type>
	{
		KahanSummation<Type> sum;

		for (const auto& elem : m_container)
		{
			sum += elem;
		}

		return sum.value();
	}

	////////////////////////////////////////////////////////////////
	//
	//	take
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::take(const size_type n) const&
	{
		return Array(m_container.begin(), (m_container.begin() + Min(n, m_container.size())));
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::take(const size_type n)&&
	{
		return Array(std::make_move_iterator(m_container.begin()), std::make_move_iterator(m_container.begin() + Min(n, m_container.size())));
	}

	////////////////////////////////////////////////////////////////
	//
	//	take_while
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::take_while(Fty f) const& requires std::predicate<Fty&, const value_type&>
	{
		return Array(m_container.begin(), std::find_if_not(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f))));
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::take_while(Fty f) && requires std::predicate<Fty&, const value_type&>
	{
		return Array(std::make_move_iterator(m_container.begin()), std::make_move_iterator(std::find_if_not(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)))));
	}

	////////////////////////////////////////////////////////////////
	//
	//	values_at
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::values_at(const std::initializer_list<size_type> indices) const
	{
		Array result;
		result.reserve(indices.size());

		for (auto index : indices)
		{
			if (index < m_container.size())
			{
				result.push_back(m_container[index]);
			}
			else
			{
				detail::ThrowArrayValuesAtIndexOutOfRange();
			}
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator >>
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr auto Array<Type, Allocator>::operator >>(Fty f) const requires std::invocable<Fty&, const value_type&>
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

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::Generate(const size_type size, FunctionRef<value_type()> generator)
	{
		Array result(Arg::reserve = size);

		for (size_type i = 0; i < size; ++i)
		{
			result.m_container.push_back(generator());
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	IndexedGenerate
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::IndexedGenerate(const size_type size, FunctionRef<value_type(size_t)> generator)
	{
		Array result(Arg::reserve = size);

		for (size_type i = 0; i < size; ++i)
		{
			result.m_container.push_back(generator(i));
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	ToArray
	//
	////////////////////////////////////////////////////////////////

	template <class Range, class Elem>
	constexpr Array<Elem> ToArray(Range&& range) requires Concept::ContainerCompatibleRange<Range, Elem>
	{
	# if __cpp_lib_containers_ranges >= 202202L
	   
		return Array<Elem>(std::from_range, std::forward<Range>(range));
	
	# else
		
		 return Array<Elem>(range.begin(), range.end());
	 
	# endif
	}
}
