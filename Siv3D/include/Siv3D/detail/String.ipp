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
		template <class Fty>
		constexpr auto PassFunction(Fty f)
		{
			if constexpr (std::is_member_pointer_v<std::remove_cvref_t<Fty>>)
			{
				return [f](auto& elem) { return std::invoke(f, elem); };
			}
			else
			{
				return f;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr String::String(const string_type& other)
		: m_string{ other } {}

	constexpr String::String(string_type&& other) noexcept
		: m_string{ std::move(other) } {}

	constexpr String::String(const String& other, const size_type pos)
		: m_string(other.m_string, pos) {}

	constexpr String::String(const String& other, const size_type pos, const size_type count)
		: m_string(other.m_string, pos, count) {}

	constexpr String::String(const value_type* s)
		: m_string(s) {}

	constexpr String::String(const value_type* s, const size_type count)
		: m_string(s, count) {}

	constexpr String::String(const size_type count, const value_type ch)
		: m_string(count, ch) {}

	constexpr String::String(const std::initializer_list<value_type> list)
		: m_string(list) {}

	template <class Iterator>
	constexpr String::String(Iterator first, Iterator last)
		: m_string(first, last) {}

	constexpr String::String(const StringViewLike auto& s)
		: m_string(s) {}

	constexpr String::String(const StringViewLike auto& s, const size_type pos, const size_type count)
		: m_string(s, pos, count) {}

# if __cpp_lib_containers_ranges >= 202202L

	template <class Range> requires Concept::ContainerCompatibleRange<String::value_type, Range>
	constexpr String::String(std::from_range_t, Range&& range)
		: m_string(std::from_range, std::forward<Range>(range)) {}

# endif

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	constexpr String& String::operator =(const String& other)
	{
		m_string = other.m_string;
		return *this;
	}

	constexpr String& String::operator =(const string_type& other)
	{
		m_string = other;
		return *this;
	}

	constexpr String& String::operator =(String&& other) noexcept
	{
		m_string = std::move(other.m_string);
		return *this;
	}

	constexpr String& String::operator =(string_type&& other) noexcept
	{
		m_string = std::move(other);
		return *this;
	}

	constexpr String& String::operator =(const value_type* s)
	{
		m_string = s;
		return *this;
	}

	constexpr String& String::operator =(const std::initializer_list<value_type> list)
	{
		m_string = list;
		return *this;
	}

	constexpr String& String::operator =(const StringViewLike auto& s)
	{
		m_string = s;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	assign
	//
	////////////////////////////////////////////////////////////////

	constexpr String& String::assign(const size_type count, const value_type ch)
	{
		m_string.assign(count, ch);
		return *this;
	}

	constexpr String& String::assign(const String& s)
	{
		m_string.assign(s.m_string);
		return *this;
	}

	constexpr String& String::assign(const string_type& s)
	{
		m_string.assign(s);
		return *this;
	}

	constexpr String& String::assign(const String& s, const size_type pos, const size_type count)
	{
		m_string.assign(s.m_string, pos, count);
		return *this;
	}

	constexpr String& String::assign(const string_type& s, const size_type pos, const size_type count)
	{
		m_string.assign(s, pos, count);
		return *this;
	}

	constexpr String& String::assign(String&& s) noexcept
	{
		m_string.assign(std::move(s.m_string));
		return *this;
	}

	constexpr String& String::assign(string_type&& s) noexcept
	{
		m_string.assign(std::move(s));
		return *this;
	}

	constexpr String& String::assign(const value_type* s, const size_type count)
	{
		m_string.assign(s, count);
		return *this;
	}

	constexpr String& String::assign(const value_type* s)
	{
		m_string.assign(s);
		return *this;
	}

	template <class Iterator>
	constexpr String& String::assign(Iterator first, Iterator last)
	{
		m_string.assign(first, last);
		return *this;
	}

	constexpr String& String::assign(const std::initializer_list<value_type> list)
	{
		m_string.assign(list);
		return *this;
	}

	constexpr String& String::assign(const StringViewLike auto& s)
	{
		m_string.assign(s);
		return *this;
	}

	constexpr String& String::assign(const StringViewLike auto& s, const size_type pos, const size_type count)
	{
		m_string.assign(s, pos, count);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	assign_range
	//
	////////////////////////////////////////////////////////////////

	template <class Range> requires Concept::ContainerCompatibleRange<String::value_type, Range>
	constexpr String& String::assign_range(Range&& range)
	{
	# if __cpp_lib_containers_ranges >= 202202L
		
		m_string.assign_range(std::forward<Range>(range));

	# else

		auto common_range = std::views::common(std::forward<Range>(range));
		m_string.assign(common_range.begin(), common_range.end());
		
	# endif

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	get_allocator
	//
	////////////////////////////////////////////////////////////////

	constexpr String::allocator_type String::get_allocator() const noexcept
	{
		return m_string.get_allocator();
	}

	////////////////////////////////////////////////////////////////
	//
	//	at
	//
	////////////////////////////////////////////////////////////////

	constexpr String::value_type& String::at(const size_type index)&
	{
		if (m_string.size() <= index)
		{
			ThrowAtOutOfRange();
		}

		return m_string[index];
	}

	constexpr const String::value_type& String::at(const size_type index) const&
	{
		if (m_string.size() <= index)
		{
			ThrowAtOutOfRange();
		}

		return m_string[index];
	}

	constexpr String::value_type String::at(const size_type index)&&
	{
		if (m_string.size() <= index)
		{
			ThrowAtOutOfRange();
		}

		return m_string[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	constexpr String::value_type& String::operator[](const size_type index) & noexcept
	{
		return m_string[index];
	}

	constexpr const String::value_type& String::operator[](const size_type index) const& noexcept
	{
		return m_string[index];
	}

	constexpr String::value_type String::operator[](const size_type index) && noexcept
	{
		return m_string[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	front
	//
	////////////////////////////////////////////////////////////////

	constexpr String::value_type& String::front()& noexcept
	{
		return m_string.front();
	}

	constexpr const String::value_type& String::front() const& noexcept
	{
		return m_string.front();
	}

	constexpr String::value_type String::front()&& noexcept
	{
		return m_string.front();
	}

	////////////////////////////////////////////////////////////////
	//
	//	back
	//
	////////////////////////////////////////////////////////////////

	constexpr String::value_type& String::back()& noexcept
	{
		return m_string.back();
	}

	constexpr const String::value_type& String::back() const& noexcept
	{
		return m_string.back();
	}

	constexpr String::value_type String::back()&& noexcept
	{
		return m_string.back();
	}

	////////////////////////////////////////////////////////////////
	//
	//	data
	//
	////////////////////////////////////////////////////////////////

	constexpr const String::value_type* String::data() const noexcept
	{
		return m_string.data();
	}

	constexpr String::value_type* String::data() noexcept
	{
		return m_string.data();
	}

	////////////////////////////////////////////////////////////////
	//
	//	c_str
	//
	////////////////////////////////////////////////////////////////

	constexpr const String::value_type* String::c_str() const noexcept
	{
		return m_string.c_str();
	}

	////////////////////////////////////////////////////////////////
	//
	//	str
	//
	////////////////////////////////////////////////////////////////

	constexpr String::string_type& String::str() noexcept
	{
		return m_string;
	}

	constexpr const String::string_type& String::str() const noexcept
	{
		return m_string;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator StringView
	//
	////////////////////////////////////////////////////////////////

	constexpr String::operator StringView() const noexcept
	{
		return StringView{ m_string.data(), m_string.size() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	begin / end
	//
	////////////////////////////////////////////////////////////////

	constexpr String::iterator String::begin() noexcept
	{
		return m_string.begin();
	}

	constexpr String::iterator String::end() noexcept
	{
		return m_string.end();
	}

	constexpr String::const_iterator String::begin() const noexcept
	{
		return m_string.begin();
	}

	constexpr String::const_iterator String::end() const noexcept
	{
		return m_string.end();
	}

	////////////////////////////////////////////////////////////////
	//
	//	cbegin / cend
	//
	////////////////////////////////////////////////////////////////

	constexpr String::const_iterator String::cbegin() const noexcept
	{
		return m_string.cbegin();
	}

	constexpr String::const_iterator String::cend() const noexcept
	{
		return m_string.cend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	rbegin / rend
	//
	////////////////////////////////////////////////////////////////

	constexpr String::reverse_iterator String::rbegin() noexcept
	{
		return m_string.rbegin();
	}

	constexpr String::reverse_iterator String::rend() noexcept
	{
		return m_string.rend();
	}

	constexpr String::const_reverse_iterator String::rbegin() const noexcept
	{
		return m_string.rbegin();
	}

	constexpr String::const_reverse_iterator String::rend() const noexcept
	{
		return m_string.rend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	crbegin / crend
	//
	////////////////////////////////////////////////////////////////

	constexpr String::const_reverse_iterator String::crbegin() const noexcept
	{
		return m_string.crbegin();
	}

	constexpr String::const_reverse_iterator String::crend() const noexcept
	{
		return m_string.crend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	empty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool String::empty() const noexcept
	{
		return m_string.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool String::isEmpty() const noexcept
	{
		return m_string.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	indexInBounds
	//
	////////////////////////////////////////////////////////////////

	constexpr bool String::indexInBounds(const size_type index) const noexcept
	{
		return (index < m_string.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	explicit operator bool
	//
	////////////////////////////////////////////////////////////////

	constexpr String::operator bool() const noexcept
	{
		return (not m_string.empty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	constexpr String::size_type String::size() const noexcept
	{
		return m_string.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	ssize
	//
	////////////////////////////////////////////////////////////////

	constexpr isize String::ssize() const noexcept
	{
		return static_cast<isize>(m_string.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	size_bytes
	//
	////////////////////////////////////////////////////////////////

	constexpr String::size_type String::size_bytes() const noexcept
	{
		return (m_string.size() * sizeof(value_type));
	}

	////////////////////////////////////////////////////////////////
	//
	//	length
	//
	////////////////////////////////////////////////////////////////

	constexpr String::size_type String::length() const noexcept
	{
		return m_string.length();
	}

	////////////////////////////////////////////////////////////////
	//
	//	max_size
	//
	////////////////////////////////////////////////////////////////

	constexpr String::size_type String::max_size() const noexcept
	{
		return m_string.max_size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	reserve
	//
	////////////////////////////////////////////////////////////////

	constexpr void String::reserve(const size_type newCapacity)
	{
		m_string.reserve(newCapacity);
	}

	////////////////////////////////////////////////////////////////
	//
	//	capacity
	//
	////////////////////////////////////////////////////////////////

	constexpr String::size_type String::capacity() const noexcept
	{
		return m_string.capacity();
	}

	////////////////////////////////////////////////////////////////
	//
	//	shrink_to_fit
	//
	////////////////////////////////////////////////////////////////

	constexpr void String::shrink_to_fit()
	{
		m_string.shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	constexpr void String::clear() noexcept
	{
		m_string.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	constexpr void String::release()
	{
		m_string.clear();
		m_string.shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////
	//
	//	insert
	//
	////////////////////////////////////////////////////////////////

	constexpr String& String::insert(const size_type offset, const size_type count, const value_type ch)
	{
		m_string.insert(offset, count, ch);
		return *this;
	}

	constexpr String& String::insert(const size_type offset, const value_type* s)
	{
		m_string.insert(offset, s);
		return *this;
	}

	constexpr String& String::insert(const size_type offset, const value_type* s, const size_type count)
	{
		m_string.insert(offset, s, count);
		return *this;
	}

	constexpr String& String::insert(const size_type offset, const String& s)
	{
		m_string.insert(offset, s.m_string);
		return *this;
	}

	constexpr String& String::insert(const size_type offset, const String& s, size_type offset2, size_type count)
	{
		m_string.insert(offset, s.m_string, offset2, count);
		return *this;
	}

	constexpr String::iterator String::insert(const_iterator pos, const value_type ch)
	{
		return m_string.insert(pos, ch);
	}

	constexpr String::iterator String::insert(const_iterator pos, const size_type count, const value_type ch)
	{
		return m_string.insert(pos, count, ch);
	}

	template <class Iterator>
	constexpr String::iterator String::insert(const_iterator pos, Iterator first, Iterator last)
	{
		return m_string.insert(pos, first, last);
	}

	constexpr String::iterator String::insert(const_iterator pos, const std::initializer_list<value_type> list)
	{
		return m_string.insert(pos, list);
	}

	constexpr String& String::insert(const size_type offset, const StringViewLike auto& s)
	{
		m_string.insert(offset, s);
		return *this;
	}

	constexpr String& String::insert(const size_type offset, const StringViewLike auto& s, const size_type offset2, const size_type count)
	{
		m_string.insert(offset, s, offset2, count);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	insert_range
	//
	////////////////////////////////////////////////////////////////

	template <class Range> requires Concept::ContainerCompatibleRange<String::value_type, Range>
	constexpr String::iterator String::insert_range(const_iterator pos, Range&& range)
	{
	# if __cpp_lib_containers_ranges >= 202202L
		
		return m_string.insert_range(pos, std::forward<Range>(range));
		
	# else

		auto common_range = std::views::common(std::forward<Range>(range));
		return m_string.insert(pos, common_range.begin(), common_range.end());
		
	# endif
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase
	//
	////////////////////////////////////////////////////////////////

	constexpr String& String::erase(const size_type offset, const size_type count)
	{
		m_string.erase(offset, count);
		return *this;
	}

	constexpr String::iterator String::erase(const_iterator pos) noexcept
	{
		return m_string.erase(pos);
	}

	constexpr String::iterator String::erase(const_iterator first, const_iterator last) noexcept
	{
		return m_string.erase(first, last);
	}

	////////////////////////////////////////////////////////////////
	//
	//	push_front
	//
	////////////////////////////////////////////////////////////////

	constexpr void String::push_front(const value_type ch)
	{
		m_string.insert(m_string.begin(), ch);
	}

	////////////////////////////////////////////////////////////////
	//
	//	push_back
	//
	////////////////////////////////////////////////////////////////

	constexpr void String::push_back(const value_type ch)
	{
		m_string.push_back(ch);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_front
	//
	////////////////////////////////////////////////////////////////

	constexpr void String::pop_front()
	{
		m_string.erase(m_string.begin());
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_front_N
	//
	////////////////////////////////////////////////////////////////

	constexpr void String::pop_front_N(const size_type n)
	{
		m_string.erase(m_string.begin(), (m_string.begin() + Min(n, m_string.size())));
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back
	//
	////////////////////////////////////////////////////////////////

	constexpr void String::pop_back() noexcept
	{
		m_string.pop_back();
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back_N
	//
	////////////////////////////////////////////////////////////////

	constexpr void String::pop_back_N(const size_type n) noexcept
	{
		m_string.erase((m_string.end() - Min(n, m_string.size())), m_string.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	constexpr String& String::operator <<(const value_type ch)
	{
		m_string.push_back(ch);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	append
	//
	////////////////////////////////////////////////////////////////

	constexpr String& String::append(const value_type ch)
	{
		m_string.push_back(ch);
		return *this;
	}

	constexpr String& String::append(const size_type count, const value_type ch)
	{
		m_string.append(count, ch);
		return *this;
	}

	constexpr String& String::append(const String& s)
	{
		m_string.append(s.m_string);
		return *this;
	}

	constexpr String& String::append(const string_type& s)
	{
		m_string.append(s);
		return *this;
	}

	constexpr String& String::append(const String& s, const size_type offset, const size_type count)
	{
		m_string.append(s.m_string, offset, count);
		return *this;
	}

	constexpr String& String::append(const value_type* s, const size_type count)
	{
		m_string.append(s, count);
		return *this;
	}

	constexpr String& String::append(const value_type* s)
	{
		m_string.append(s);
		return *this;
	}

	template <class Iterator>
	constexpr String& String::append(Iterator first, Iterator last)
	{
		m_string.append(first, last);
		return *this;
	}

	constexpr String& String::append(const std::initializer_list<value_type> list)
	{
		m_string.append(list);
		return *this;
	}

	constexpr String& String::append(const StringViewLike auto& s)
	{
		m_string.append(s);
		return *this;
	}

	constexpr String& String::append(const StringViewLike auto& s, const size_type pos, const size_type count)
	{
		m_string.append(s, pos, count);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	append_range
	//
	////////////////////////////////////////////////////////////////

	template <class Range> requires Concept::ContainerCompatibleRange<String::value_type, Range>
	constexpr String& String::append_range(Range&& range)
	{
	# if __cpp_lib_containers_ranges >= 202202L
	
		m_string.append_range(std::forward<Range>(range));
	
	# else

		auto common_range = std::views::common(std::forward<Range>(range));
		m_string.append(common_range.begin(), common_range.end());
		
	# endif
		
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	constexpr String& String::operator +=(const String& s)
	{
		m_string.append(s.m_string);
		return *this;
	}

	constexpr String& String::operator +=(const string_type& s)
	{
		m_string.append(s);
		return *this;
	}

	constexpr String& String::operator +=(const value_type ch)
	{
		m_string.push_back(ch);
		return *this;
	}

	constexpr String& String::operator +=(const value_type* s)
	{
		m_string.append(s);
		return *this;
	}

	constexpr String& String::operator +=(const std::initializer_list<value_type> list)
	{
		m_string.append(list);
		return *this;
	}

	constexpr String& String::operator +=(const StringViewLike auto& s)
	{
		m_string.append(s);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	replace
	//
	////////////////////////////////////////////////////////////////

	constexpr String& String::replace(const size_type pos, const size_type count, const String& s)
	{
		m_string.replace(pos, count, s.m_string);
		return *this;
	}

	constexpr String& String::replate(const_iterator first, const_iterator last, const String& s)
	{
		m_string.replace(first, last, s.m_string);
		return *this;
	}

	constexpr String& String::replace(const_iterator first, const_iterator last, const value_type* s)
	{
		m_string.replace(first, last, s);
		return *this;
	}

	constexpr String& String::replate(const_iterator first, const_iterator last, const StringViewLike auto& s)
	{
		m_string.replace(first, last, s);
		return *this;
	}

	constexpr String& String::replace(const size_type pos, const size_type count, const value_type* s)
	{
		m_string.replace(pos, count, s);
		return *this;
	}

	template <class Iterator>
	constexpr String& String::replace(const_iterator first, const_iterator last, Iterator newFirst, Iterator newLast)
	{
		m_string.replace(first, last, newFirst, newLast);
		return *this;
	}

	constexpr String& String::replace(const size_type pos, const size_type count, const StringViewLike auto& s)
	{
		m_string.replace(pos, count, s);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	resize
	//
	////////////////////////////////////////////////////////////////

	constexpr void String::resize(const size_type newSize)
	{
		m_string.resize(newSize);
	}

	constexpr void String::resize(const size_type newSize, const value_type ch)
	{
		m_string.resize(newSize, ch);
	}

	////////////////////////////////////////////////////////////////
	//
	//	resize_and_overwrite
	//
	////////////////////////////////////////////////////////////////

	template <class Operation>
	constexpr void String::resize_and_overwrite(const size_type count, Operation op)
	{
		m_string.resize_and_overwrite(count, std::move(op));
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	constexpr void String::swap(String& other) noexcept
	{
		m_string.swap(other.m_string);
	}

	////////////////////////////////////////////////////////////////
	//
	//	find
	//
	////////////////////////////////////////////////////////////////

	constexpr String::size_type String::find(const String& s, const size_type offset) const noexcept
	{
		return m_string.find(s.m_string, offset);
	}

	constexpr String::size_type String::find(const value_type* s, const size_type offset) const noexcept
	{
		return m_string.find(s, offset);
	}

	constexpr String::size_type String::find(const StringViewLike auto& s, const size_type offset) const noexcept
	{
		return m_string.find(s, offset);
	}

	constexpr String::size_type String::find(const value_type ch, const size_type offset) const noexcept
	{
		return m_string.find(ch, offset);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rfind
	//
	////////////////////////////////////////////////////////////////

	constexpr String::size_type String::rfind(const String& s, const size_type offset) const noexcept
	{
		return m_string.rfind(s.m_string, offset);
	}

	constexpr String::size_type String::rfind(const value_type* s, const size_type offset) const noexcept
	{
		return m_string.rfind(s, offset);
	}

	constexpr String::size_type String::rfind(const StringViewLike auto& s, const size_type offset) const noexcept
	{
		return m_string.rfind(s, offset);
	}

	constexpr String::size_type String::rfind(const value_type ch, const size_type offset) const noexcept
	{
		return m_string.rfind(ch, offset);
	}

	////////////////////////////////////////////////////////////////
	//
	//	find_first_of
	//
	////////////////////////////////////////////////////////////////

	constexpr String::size_type String::find_first_of(const String& s, const size_type offset) const noexcept
	{
		return m_string.find_first_of(s.m_string, offset);
	}

	constexpr String::size_type String::find_first_of(const value_type* s, const size_type offset) const noexcept
	{
		return m_string.find_first_of(s, offset);
	}

	constexpr String::size_type String::find_first_of(const StringViewLike auto& s, const size_type offset) const noexcept
	{
		return m_string.find_first_of(s, offset);
	}

	////////////////////////////////////////////////////////////////
	//
	//	find_first_not_of
	//
	////////////////////////////////////////////////////////////////

	constexpr String::size_type String::find_first_not_of(const String& s, const size_type offset) const noexcept
	{
		return m_string.find_first_not_of(s.m_string, offset);
	}

	constexpr String::size_type String::find_first_not_of(const value_type* s, const size_type offset) const noexcept
	{
		return m_string.find_first_not_of(s, offset);
	}

	constexpr String::size_type String::find_first_not_of(const StringViewLike auto& s, const size_type offset) const noexcept
	{
		return m_string.find_first_not_of(s, offset);
	}

	constexpr String::size_type String::find_first_not_of(const value_type ch, const size_type offset) const noexcept
	{
		return m_string.find_first_not_of(ch, offset);
	}

	////////////////////////////////////////////////////////////////
	//
	//	find_last_of
	//
	////////////////////////////////////////////////////////////////

	constexpr String::size_type String::find_last_of(const String& s, const size_type offset) const noexcept
	{
		return m_string.find_last_of(s.m_string, offset);
	}

	constexpr String::size_type String::find_last_of(const value_type* s, const size_type offset) const noexcept
	{
		return m_string.find_last_of(s, offset);
	}

	constexpr String::size_type String::find_last_of(const StringViewLike auto& s, const size_type offset) const noexcept
	{
		return m_string.find_last_of(s, offset);
	}

	////////////////////////////////////////////////////////////////
	//
	//	find_last_not_of
	//
	////////////////////////////////////////////////////////////////

	constexpr String::size_type String::find_last_not_of(const String& s, const size_type offset) const noexcept
	{
		return m_string.find_last_not_of(s.m_string, offset);
	}

	constexpr String::size_type String::find_last_not_of(const value_type* s, const size_type offset) const noexcept
	{
		return m_string.find_last_not_of(s, offset);
	}

	constexpr String::size_type String::find_last_not_of(const StringViewLike auto& s, size_type const offset) const noexcept
	{
		return m_string.find_last_not_of(s, offset);
	}

	constexpr String::size_type String::find_last_not_of(const value_type ch, const size_type offset) const noexcept
	{
		return m_string.find_last_not_of(ch, offset);
	}

	////////////////////////////////////////////////////////////////
	//
	//	compare
	//
	////////////////////////////////////////////////////////////////

	constexpr int32 String::compare(const String& s) const noexcept
	{
		return m_string.compare(s.m_string);
	}

	constexpr int32 String::compare(const value_type* s) const noexcept
	{
		return m_string.compare(s);
	}

	constexpr int32 String::compare(const StringViewLike auto& s) const noexcept
	{
		return m_string.compare(s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	starts_with
	//
	////////////////////////////////////////////////////////////////

	constexpr bool String::starts_with(const value_type ch) const noexcept
	{
		return m_string.starts_with(ch);
	}

	constexpr bool String::starts_with(const StringView s) const noexcept
	{
		return StringView{ m_string }.starts_with(s);
	}

	constexpr bool String::starts_with(const value_type* s) const noexcept
	{
		return m_string.starts_with(s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	ends_with
	//
	////////////////////////////////////////////////////////////////

	constexpr bool String::ends_with(const value_type ch) const noexcept
	{
		return m_string.ends_with(ch);
	}

	constexpr bool String::ends_with(const StringView s) const noexcept
	{
		return StringView{ m_string }.ends_with(s);
	}

	constexpr bool String::ends_with(const value_type* s) const noexcept
	{
		return m_string.ends_with(s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains
	//
	////////////////////////////////////////////////////////////////

	constexpr bool String::contains(const value_type ch) const noexcept
	{
		return m_string.contains(ch);
	}

	constexpr bool String::contains(const StringView s) const noexcept
	{
		return StringView{ m_string }.contains(s);
	}

	constexpr bool String::contains(const value_type* s) const noexcept
	{
		return m_string.contains(s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	substr
	//
	////////////////////////////////////////////////////////////////

	constexpr String String::substr(size_type offset, size_type count) const&
	{
		return String{ m_string.substr(offset, count) };
	}

	constexpr String String::substr(size_type offset, size_type count)&&
	{
		return String{ std::move(m_string).substr(offset, count) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	substrView
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView String::substrView(size_type offset, size_type count) const&
	{
		const size_t strSize = m_string.size();

		if (strSize < offset)
		{
			offset = strSize;
		}

		if ((count == npos) || (strSize < (offset + count)))
		{
			count = (strSize - offset);
		}

		return StringView{ (m_string.data() + offset), count };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 String::hash() const noexcept
	{
		return Hash(m_string.data(), (m_string.size() * sizeof(String::value_type)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	toUTF32
	//
	////////////////////////////////////////////////////////////////

	constexpr const std::u32string& String::toUTF32() const noexcept
	{
		return m_string;
	}

	////////////////////////////////////////////////////////////////
	//
	//	subspan
	//
	////////////////////////////////////////////////////////////////

	constexpr std::span<String::value_type> String::subspan(const size_type pos, const size_type count) noexcept
	{
		return std::span{ m_string }.subspan(pos, count);
	}

	constexpr std::span<const String::value_type> String::subspan(const size_type pos, const size_type count) const noexcept
	{
		return std::span{ m_string }.subspan(pos, count);
	}

	inline namespace Literals
	{
		inline namespace StringLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_s
			//
			////////////////////////////////////////////////////////////////

			constexpr String operator ""_s(const char32_t* s, const size_t length)
			{
				return String(s, length);
			}
		}
	}
}
