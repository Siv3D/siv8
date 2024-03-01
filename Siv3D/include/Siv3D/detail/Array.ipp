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

# ifdef __cpp_lib_containers_ranges
	   
	template <class Type, class Allocator>
	template <Concept::ContainerCompatibleRange<Type> Range>
	constexpr Array<Type, Allocator>::Array(std::from_range_t, Range&& range, const Allocator& alloc)
		: m_container(std::from_range, std::forward<Range>(range), alloc) {}

# endif

	template <class Type, class Allocator>
	template <class Fty> requires (std::invocable<Fty> && std::same_as<std::invoke_result_t<Fty>, Type>)
	Array<Type, Allocator>::Array(const size_type size, Arg::generator_<Fty> generator)
		: Array(Generate<Fty>(size, *generator)) {}

	template <class Type, class Allocator>
	template <class Fty>
	Array<Type, Allocator>::Array(const size_type size, Arg::indexedGenerator_<Fty> indexedGenerator)
		: Array(IndexedGenerate<Fty>(size, *indexedGenerator)) {}

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

	template <class Type, class Allocator>
	template <Concept::ContainerCompatibleRange<Type> Range>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::assign_range(Range&& range)
	{
		m_container.assign_range(std::forward<Range>(range));
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
	constexpr const typename Array<Type, Allocator>::container_type& Array<Type, Allocator>::getContainer() const noexcept
	{
		return m_container;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator container_type
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>::operator typename Array<Type, Allocator>::container_type() const noexcept
	{
		return m_container;
	}

	////////////////////////////////////////////////////////////////
	//
	//	at
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr const typename Array<Type, Allocator>::value_type& Array<Type, Allocator>::at(const size_type index) const
	{
		return m_container.at(index);
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::value_type& Array<Type, Allocator>::at(const size_type index)
	{
		return m_container.at(index);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr const typename Array<Type, Allocator>::value_type& Array<Type, Allocator>::operator[](const size_type index) const noexcept
	{
		return m_container[index];
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::value_type& Array<Type, Allocator>::operator[](const size_type index) noexcept
	{
		return m_container[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	front
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::reference Array<Type, Allocator>::front() noexcept
	{
		return m_container.front();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::const_reference Array<Type, Allocator>::front() const noexcept
	{
		return m_container.front();
	}

	////////////////////////////////////////////////////////////////
	//
	//	back
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::reference Array<Type, Allocator>::back() noexcept
	{
		return m_container.back();
	}

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::const_reference Array<Type, Allocator>::back() const noexcept
	{
		return m_container.back();
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
	//	begin / end
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
	//	cbegin / cend
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
	//	rbegin / rend
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
	//	crbegin / crend
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
	constexpr size_t Array<Type, Allocator>::size_bytes() const noexcept requires (Concept::TriviallyCopyable<Type>)
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
		m_container.append_range(std::forward<Range>(range));
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
	//	all
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr bool Array<Type, Allocator>::all(Fty f) const requires std::predicate<Fty, value_type>
	{
		return std::all_of(m_container.begin(), m_container.end(), f);
	}

	////////////////////////////////////////////////////////////////
	//
	//	any
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr bool Array<Type, Allocator>::any(Fty f) const requires std::predicate<Fty, value_type>
	{
		return std::any_of(m_container.begin(), m_container.end(), f);
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




	////////////////////////////////////////////////////////////////
	//
	//	sort / sorted
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::sort()& requires Concept::LessThanComparable<Type>
	{
		std::ranges::sort(m_container);
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::sort() && requires Concept::LessThanComparable<Type>
	{
		return std::move(sort());
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::sorted() const& requires Concept::LessThanComparable<Type>
	{
		Array result(*this);
		result.sort();
		return result;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::sorted() && requires Concept::LessThanComparable<Type>
	{
		return std::move(sort());
	}





	template <class Type, class Allocator>
	template <class Fty>  requires (std::invocable<Fty>&& std::same_as<std::invoke_result_t<Fty>, Type>)
	Array<Type, Allocator> Array<Type, Allocator>::Generate(const size_type size, Fty generator)
	{
		Array new_array(Arg::reserve = size);

		for (size_type i = 0; i < size; ++i)
		{
			new_array.m_container.push_back(generator());
		}

		return new_array;
	}

	template <class Type, class Allocator>
	template <class Fty>
	Array<Type, Allocator> Array<Type, Allocator>::IndexedGenerate(const size_type size, Fty indexedGenerator)
	{
		Array new_array(Arg::reserve = size);

		for (size_type i = 0; i < size; ++i)
		{
			new_array.m_container.push_back(indexedGenerator(i));
		}

		return new_array;
	}

	template <class Range, class Elem>
	constexpr Array<Elem> ToArray(Range&& range) requires Concept::ContainerCompatibleRange<Range, Elem>
	{
	# ifdef __cpp_lib_containers_ranges
	   
		return Array<Elem>(std::from_range, std::forward<Range>(range));
	
	# else
		
		 return Array<Elem>(range.begin(), range.end());
	 
	# endif
	}
}
