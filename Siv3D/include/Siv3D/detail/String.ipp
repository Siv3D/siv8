//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	//////////////////////////////////////////////////
	//
	//	constructor
	//
	//////////////////////////////////////////////////

	inline String::String() noexcept
		: m_string() {}

	inline String::String(const String& other)
		: m_string{ other.m_string } {}

	inline String::String(const string_type& s)
		: m_string{ s } {}

	inline String::String(const String& other, const size_type pos)
		: m_string{ other.m_string, pos } {}

	inline String::String(const string_type& other, const size_type pos)
		: m_string{ other, pos } {}

	inline String::String(const String& other, const size_type pos, const size_type count)
		: m_string{ other.m_string, pos, count } {}

	inline String::String(const string_type& other, const size_type pos, const size_type count)
		: m_string{ other, pos, count } {}

	inline String::String(const value_type* s)
		: m_string(s) {}

	inline String::String(const value_type* s, const size_type count)
		: m_string(s, count) {}

	inline String::String(std::initializer_list<value_type> ilist)
		: m_string{ ilist } {}

	inline String::String(const size_t count, const value_type ch)
		: m_string(count, ch) {}

	template <class Iterator>
	inline String::String(Iterator first, Iterator last)
		: m_string(first, last) {}

	inline String::String(String&& other) noexcept
		: m_string{ std::move(other.m_string) } {}

	inline String::String(string_type&& s) noexcept
		: m_string{ std::move(s) } {}

	inline String::String(const StringViewIsh auto& s)
		: m_string(s) {}

	inline String::String(const StringViewIsh auto s, const size_type pos, const size_type count)
		: m_string(s, pos, count) {}

	inline String::operator StringView() const noexcept
	{
		return{ m_string.data(), m_string.size() };
	}

	//////////////////////////////////////////////////
	//
	//	operator =
	//
	//////////////////////////////////////////////////

	inline String& String::operator =(const String& other)
	{
		m_string = other.m_string;
		return *this;
	}

	inline String& String::operator =(const string_type& s)
	{
		m_string = s;
		return *this;
	}

	inline String& String::operator =(String&& other) noexcept
	{
		m_string = std::move(other.m_string);
		return *this;
	}

	inline String& String::operator =(string_type&& s) noexcept
	{
		m_string = std::move(s);
		return *this;
	}

	inline String& String::operator =(const value_type* s)
	{
		m_string = s;
		return *this;
	}

	inline String& String::operator =(std::initializer_list<value_type> ilist)
	{
		m_string = ilist;
		return *this;
	}

	inline String& String::operator =(const StringViewIsh auto& s)
	{
		const StringView sv = s;
		m_string.assign(sv.data(), sv.size());
		return *this;
	}

	//////////////////////////////////////////////////
	//
	//	assign
	//
	//////////////////////////////////////////////////

	inline String& String::assign(const size_t count, const value_type ch)
	{
		m_string.assign(count, ch);
		return *this;
	}

	inline String& String::assign(const String& s)
	{
		m_string.assign(s.m_string);
		return *this;
	}

	inline String& String::assign(const string_type& s)
	{
		m_string.assign(s);
		return *this;
	}

	inline String& String::assign(String&& s) noexcept
	{
		m_string.assign(std::move(s.m_string));
		return *this;
	}

	inline String& String::assign(string_type&& s) noexcept
	{
		m_string.assign(std::move(s));
		return *this;
	}

	inline String& String::assign(const String& s, const size_type pos, const size_type count)
	{
		m_string.assign(s.m_string, pos, count);
		return *this;
	}

	inline String& String::assign(const string_type& s, const size_type pos, const size_type count)
	{
		m_string.assign(s, pos, count);
		return *this;
	}

	inline String& String::assign(const value_type* s)
	{
		m_string.assign(s);
		return *this;
	}

	inline String& String::assign(const value_type* s, const size_type count)
	{
		m_string.assign(s, count);
		return *this;
	}

	template <class Iterator>
	inline String& String::assign(Iterator first, Iterator last)
	{
		m_string.assign(first, last);
		return *this;
	}

	inline String& String::assign(std::initializer_list<value_type> ilist)
	{
		m_string.assign(ilist);
		return *this;
	}

	inline String& String::assign(const StringViewIsh auto& s)
	{
		const StringView sv = s;
		m_string.assign(sv.data(), sv.size());
		return *this;
	}

	inline String& String::assign(const StringViewIsh auto& s, const size_type pos, const size_type count)
	{
		const StringView sv = s;
		return assign(sv.substr(pos, count));
	}

	//////////////////////////////////////////////////
	//
	//	get_allocator
	//
	//////////////////////////////////////////////////

	inline String::allocator_type String::get_allocator() const noexcept
	{
		return m_string.get_allocator();
	}

	//////////////////////////////////////////////////
	//
	//	operator <<
	//
	//////////////////////////////////////////////////

	inline String& String::operator <<(const value_type ch)
	{
		m_string.push_back(ch);
		return *this;
	}

	//////////////////////////////////////////////////
	//
	//	operator +=
	//
	//////////////////////////////////////////////////

	inline String& String::operator +=(const String& s)
	{
		return append(s);
	}

	inline String& String::operator +=(const string_type& s)
	{
		return append(s);
	}

	inline String& String::operator +=(const value_type ch)
	{
		return append(ch);
	}

	inline String& String::operator +=(const value_type* s)
	{
		return append(s);
	}

	inline String& String::operator +=(std::initializer_list<value_type> ilist)
	{
		return append(ilist);
	}

	inline String& String::operator +=(const StringViewIsh auto& s)
	{
		return append(s);
	}

	//////////////////////////////////////////////////
	//
	//	append
	//
	//////////////////////////////////////////////////

	inline String& String::append(const size_t count, const value_type ch)
	{
		m_string.append(count, ch);
		return *this;
	}

	inline String& String::append(const String& s)
	{
		m_string.append(s.m_string);
		return *this;
	}

	inline String& String::append(const string_type& s)
	{
		m_string.append(s);
		return *this;
	}

	inline String& String::append(const String& s, const size_type pos, const size_type count)
	{
		m_string.append(s.m_string, pos, count);
		return *this;
	}

	inline String& String::append(const string_type& s, const size_type pos, const size_type count)
	{
		m_string.append(s, pos, count);
		return *this;
	}

	inline String& String::append(const value_type ch)
	{
		m_string.push_back(ch);
		return *this;
	}

	inline String& String::append(const value_type* s)
	{
		m_string.append(s);
		return *this;
	}

	inline String& String::append(const value_type* s, const size_t count)
	{
		m_string.append(s, count);
		return *this;
	}

	template <class Iterator>
	inline String& String::append(Iterator first, Iterator last)
	{
		m_string.append(first, last);
		return *this;
	}

	inline String& String::append(std::initializer_list<value_type> ilist)
	{
		m_string.append(ilist);
		return *this;
	}

	inline String& String::append(const StringViewIsh auto& s)
	{
		const StringView sv = s;
		return append(sv.data(), sv.size());
	}

	inline String& String::append(const StringViewIsh auto& s, const size_type pos, const size_type count)
	{
		const StringView sv = s;
		return append(sv.substr(pos, count));
	}

	//////////////////////////////////////////////////
	//
	//	insert
	//
	//////////////////////////////////////////////////

	inline String& String::insert(const size_t offset, const size_t count, const value_type ch)
	{
		m_string.insert(offset, count, ch);
		return *this;
	}

	inline String& String::insert(const size_t offset, const value_type* s)
	{
		m_string.insert(offset, s);
		return *this;
	}

	inline String& String::insert(const size_t offset, const value_type* s, const size_t count)
	{
		m_string.insert(offset, s, count);
		return *this;
	}

	inline String& String::insert(const size_t offset, const String& s)
	{
		m_string.insert(offset, s.m_string);
		return *this;
	}

	inline String& String::insert(const size_t offset, const string_type& s)
	{
		m_string.insert(offset, s);
		return *this;
	}

	inline String& String::insert(const size_t offset, const String& s, const size_type pos, const size_type count)
	{
		m_string.insert(offset, s.m_string, pos, count);
		return *this;
	}

	inline String& String::insert(const size_t offset, const string_type& s, const size_type pos, const size_type count)
	{
		m_string.insert(offset, s, pos, count);
		return *this;
	}

	inline String::iterator String::insert(const_iterator where, const value_type ch)
	{
		return m_string.insert(where, ch);
	}

	inline String::iterator String::insert(const_iterator where, const size_t count, const value_type ch)
	{
		return m_string.insert(where, count, ch);
	}

	template <class Iterator>
	inline String::iterator String::insert(const_iterator where, Iterator first, Iterator last)
	{
		return m_string.insert(where, first, last);
	}

	inline String::iterator String::insert(const_iterator where, std::initializer_list<value_type> ilist)
	{
		return m_string.insert(where, ilist);
	}

	inline String& String::insert(const size_t offset, const StringViewIsh auto& s)
	{
		const StringView sv = s;
		return insert(offset, sv.data(), sv.size());
	}

	inline String& String::insert(const size_t offset, const StringViewIsh auto& s, const size_type pos, const size_type count)
	{
		const StringView sv = s;
		return insert(offset, sv.substr(pos, count));
	}

	//////////////////////////////////////////////////
	//
	//	push / pop
	//
	//////////////////////////////////////////////////

	inline void String::push_front(const value_type ch)
	{
		m_string.insert(begin(), ch);
	}

	inline void String::push_back(const value_type ch)
	{
		m_string.push_back(ch);
	}

	inline void String::pop_front()
	{
		m_string.erase(m_string.begin());
	}

	inline void String::pop_front_N(const size_t n)
	{
		m_string.erase(m_string.begin(), (m_string.begin() + Min(n, m_string.size())));
	}

	inline void String::pop_back() noexcept
	{
		m_string.pop_back();
	}

	inline void String::pop_back_N(const size_t n) noexcept
	{
		m_string.erase((m_string.end() - Min(n, m_string.size())), m_string.end());
	}

	//////////////////////////////////////////////////
	//
	//	erase
	//
	//////////////////////////////////////////////////

	inline String& String::erase(const size_t offset, const size_t count)
	{
		m_string.erase(offset, count);
		return *this;
	}

	inline String::iterator String::erase(const_iterator where) noexcept
	{
		return m_string.erase(where);
	}

	inline String::iterator String::erase(const_iterator first, const_iterator last) noexcept
	{
		return m_string.erase(first, last);
	}

	//////////////////////////////////////////////////
	//
	//	clear
	//
	//////////////////////////////////////////////////

	inline void String::clear() noexcept
	{
		m_string.clear();
	}

	//////////////////////////////////////////////////
	//
	//	begin / end
	//
	//////////////////////////////////////////////////

	inline String::iterator String::begin() noexcept
	{
		return m_string.begin();
	}

	inline String::iterator String::end() noexcept
	{
		return m_string.end();
	}

	inline String::const_iterator String::begin() const noexcept
	{
		return m_string.begin();
	}

	inline String::const_iterator String::end() const noexcept
	{
		return m_string.end();
	}

	inline String::const_iterator String::cbegin() const noexcept
	{
		return m_string.cbegin();
	}

	inline String::const_iterator String::cend() const noexcept
	{
		return m_string.cend();
	}

	inline String::reverse_iterator String::rbegin() noexcept
	{
		return m_string.rbegin();
	}

	inline String::reverse_iterator String::rend() noexcept
	{
		return m_string.rend();
	}

	inline String::const_reverse_iterator String::rbegin() const noexcept
	{
		return m_string.rbegin();
	}

	inline String::const_reverse_iterator String::rend() const noexcept
	{
		return m_string.rend();
	}

	inline String::const_reverse_iterator String::crbegin() const noexcept
	{
		return m_string.crbegin();
	}

	inline String::const_reverse_iterator String::crend() const noexcept
	{
		return m_string.crend();
	}

	//////////////////////////////////////////////////
	//
	//	shrink_to_fit
	//
	//////////////////////////////////////////////////

	inline void String::shrink_to_fit()
	{
		m_string.shrink_to_fit();
	}

	//////////////////////////////////////////////////
	//
	//	release
	//
	//////////////////////////////////////////////////

	inline void String::release()
	{
		m_string.clear();
		m_string.shrink_to_fit();
	}

	//////////////////////////////////////////////////
	//
	//	at
	//
	//////////////////////////////////////////////////

	inline String::value_type& String::at(const size_t offset)&
	{
		return m_string.at(offset);
	}

	inline const String::value_type& String::at(const size_t offset) const&
	{
		return m_string.at(offset);
	}

	inline String::value_type String::at(const size_t offset)&&
	{
		return m_string.at(offset);
	}

	//////////////////////////////////////////////////
	//
	//	operator[]
	//
	//////////////////////////////////////////////////

	inline String::value_type& String::operator[](const size_t offset) & noexcept
	{
		return m_string[offset];
	}

	inline const String::value_type& String::operator[](const size_t offset) const& noexcept
	{
		return m_string[offset];
	}

	inline String::value_type String::operator[](const size_t offset) && noexcept
	{
		return m_string[offset];
	}

	//////////////////////////////////////////////////
	//
	//	front / back
	//
	//////////////////////////////////////////////////

	inline String::value_type& String::front() noexcept
	{
		return m_string.front();
	}

	inline const String::value_type& String::front() const noexcept
	{
		return m_string.front();
	}

	inline String::value_type& String::back() noexcept
	{
		return m_string.back();
	}

	inline const String::value_type& String::back() const noexcept
	{
		return m_string.back();
	}

	//////////////////////////////////////////////////
	//
	//	c_str / data
	//
	//////////////////////////////////////////////////

	inline const String::value_type* String::c_str() const noexcept
	{
		return m_string.c_str();
	}

	inline const String::value_type* String::data() const noexcept
	{
		return m_string.data();
	}

	inline String::value_type* String::data() noexcept
	{
		return m_string.data();
	}

	//////////////////////////////////////////////////
	//
	//	str
	//
	//////////////////////////////////////////////////

	inline String::string_type& String::str() noexcept
	{
		return m_string;
	}

	inline const String::string_type& String::str() const noexcept
	{
		return m_string;
	}

	//////////////////////////////////////////////////
	//
	//	length / size / size_bytes
	//
	//////////////////////////////////////////////////

	inline size_t String::length() const noexcept
	{
		return m_string.length();
	}

	inline size_t String::size() const noexcept
	{
		return m_string.size();
	}

	inline size_t String::size_bytes() const noexcept
	{
		return (m_string.size() * sizeof(value_type));
	}

	//////////////////////////////////////////////////
	//
	//	empty / isEmpty / operator bool
	//
	//////////////////////////////////////////////////

	inline bool String::empty() const noexcept
	{
		return m_string.empty();
	}

	inline bool String::isEmpty() const noexcept
	{
		return m_string.empty();
	}

	inline String::operator bool() const noexcept
	{
		return (not m_string.empty());
	}

	//////////////////////////////////////////////////
	//
	//	max_size / capacity
	//
	//////////////////////////////////////////////////

	inline size_t String::max_size() const noexcept
	{
		return m_string.max_size();
	}

	inline size_t String::capacity() const noexcept
	{
		return m_string.capacity();
	}

	//////////////////////////////////////////////////
	//
	//	resize
	//
	//////////////////////////////////////////////////

	inline void String::resize(const size_t newSize)
	{
		m_string.resize(newSize);
	}

	inline void String::resize(const size_t newSize, const value_type ch)
	{
		m_string.resize(newSize, ch);
	}

	//////////////////////////////////////////////////
	//
	//	resize_and_overwrite
	//
	//////////////////////////////////////////////////

# if defined(__cpp_lib_string_resize_and_overwrite)

	template <class Operation>
	inline void String::resize_and_overwrite(const size_type count, Operation op)
	{
		m_string.resize_and_overwrite(count, std::move(op));
	}

# endif

	//////////////////////////////////////////////////
	//
	//	reserve
	//
	//////////////////////////////////////////////////

	inline void String::reserve(const size_t newCapacity)
	{
		m_string.reserve(newCapacity);
	}

	//////////////////////////////////////////////////
	//
	//	swap
	//
	//////////////////////////////////////////////////

	inline void String::swap(String& other) noexcept
	{
		m_string.swap(other.m_string);
	}

	//////////////////////////////////////////////////
	//
	//	starts_with
	//
	//////////////////////////////////////////////////

	inline bool String::starts_with(const value_type ch) const noexcept
	{
		return m_string.starts_with(ch);
	}

	inline bool String::starts_with(const value_type* s) const noexcept
	{
		return m_string.starts_with(s);
	}

	inline bool String::starts_with(const StringView s) const noexcept
	{
		return m_string.starts_with(s.view());
	}

	//////////////////////////////////////////////////
	//
	//	ends_with
	//
	//////////////////////////////////////////////////

	inline bool String::ends_with(const value_type ch) const noexcept
	{
		return m_string.ends_with(ch);
	}

	inline bool String::ends_with(const value_type* s) const noexcept
	{
		return m_string.ends_with(s);
	}

	inline bool String::ends_with(const StringView s) const noexcept
	{
		return m_string.ends_with(s.view());
	}

	//////////////////////////////////////////////////
	//
	//	contains
	//
	//////////////////////////////////////////////////

	inline bool String::contains(const value_type ch) const noexcept
	{
		return StringView{ m_string.data(), m_string.size() }.contains(ch);
	}

	inline bool String::contains(const value_type* s) const noexcept
	{
		return StringView{ m_string.data(), m_string.size() }.contains(s);
	}

	inline bool String::contains(const StringView s) const noexcept
	{ 
		return StringView{ m_string.data(), m_string.size() }.contains(s);
	}

	//////////////////////////////////////////////////
	//
	//	swap
	//
	//////////////////////////////////////////////////

	inline void swap(String& a, String& b) noexcept
	{
		a.swap(b);
	}

	//////////////////////////////////////////////////
	//
	//	operator ""_s
	//
	//////////////////////////////////////////////////

	inline namespace Literals
	{
		inline namespace StringLiterals
		{
			inline String operator ""_s(const char32_t* s, const size_t length)
			{
				return String(s, length);
			}
		}
	}
}
