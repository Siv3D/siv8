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
	constexpr StringView::StringView(const value_type* s, const size_type length) noexcept
		: m_view{ s, length } {}

	constexpr StringView::StringView(const value_type* s) noexcept
		: m_view{ s } {}

	template <std::contiguous_iterator Iterator, std::sized_sentinel_for<Iterator> Sentinel>
		requires (std::is_same_v<std::iter_value_t<Iterator>, StringView::value_type> && (not std::is_convertible_v<Sentinel, StringView::size_type>))
	constexpr StringView::StringView(Iterator first, Sentinel last)
		: m_view{ first, last } {}

	inline StringView::StringView(const std::u32string& s) noexcept
		: m_view{ s } {}

	constexpr StringView::StringView(const std::u32string_view s) noexcept
		: m_view{ s } {}

	constexpr StringView::const_iterator StringView::begin() const noexcept
	{
		return m_view.begin();
	}

	constexpr StringView::const_iterator StringView::end() const noexcept
	{
		return m_view.end();
	}

	constexpr StringView::const_iterator StringView::cbegin() const noexcept
	{
		return m_view.cbegin();
	}

	constexpr StringView::const_iterator StringView::cend() const noexcept
	{
		return m_view.cend();
	}

	constexpr StringView::const_reverse_iterator StringView::rbegin() const noexcept
	{
		return m_view.rbegin();
	}

	constexpr StringView::const_reverse_iterator StringView::rend() const noexcept
	{
		return m_view.rend();
	}

	constexpr StringView::const_reverse_iterator StringView::crbegin() const noexcept
	{
		return m_view.crbegin();
	}

	constexpr StringView::const_reverse_iterator StringView::crend() const noexcept
	{
		return m_view.crend();
	}

	constexpr StringView::const_reference StringView::operator[](const size_type index) const noexcept
	{
		return m_view[index];
	}

	constexpr StringView::const_reference StringView::at(const size_type index) const
	{
		return m_view.at(index);
	}

	constexpr StringView::const_reference StringView::front() const noexcept
	{
		return m_view.front();
	}

	constexpr StringView::const_reference StringView::back() const noexcept
	{
		return m_view.back();
	}

	constexpr StringView::const_pointer StringView::data() const noexcept
	{
		return m_view.data();
	}

	constexpr StringView::size_type StringView::size() const noexcept
	{
		return m_view.size();
	}

	constexpr StringView::size_type StringView::size_bytes() const noexcept
	{
		return (m_view.size() * sizeof(value_type));
	}

	constexpr StringView::size_type StringView::length() const noexcept
	{
		return m_view.length();
	}

	constexpr StringView::size_type StringView::max_size() const noexcept
	{
		return m_view.max_size();
	}

	constexpr bool StringView::empty() const noexcept
	{
		return m_view.empty();
	}

	constexpr bool StringView::isEmpty() const noexcept
	{
		return m_view.empty();
	}

	constexpr StringView::operator bool() const noexcept
	{
		return (not m_view.empty());
	}

	constexpr void StringView::remove_prefix(const size_type n) noexcept
	{
		m_view.remove_prefix(n);
	}

	constexpr void StringView::remove_suffix(const size_type n) noexcept
	{
		m_view.remove_suffix(n);
	}

	constexpr void StringView::clear() noexcept
	{
		remove_suffix(size());
	}

	constexpr void StringView::swap(StringView& other) noexcept
	{
		m_view.swap(other.m_view);
	}

	constexpr StringView::size_type StringView::copy(value_type* dst, const size_type n, const size_type pos) const
	{
		return m_view.copy(dst, n, pos);
	}

	constexpr StringView StringView::substr(const size_type pos, const size_type n) const
	{
		return m_view.substr(pos, n);
	}
	
	constexpr int32 StringView::compare(const StringView s) const noexcept
	{
		return m_view.compare(s.m_view);
	}

	constexpr int32 StringView::compare(const size_type pos1, const size_type n1, const StringView s) const noexcept
	{
		return m_view.compare(pos1, n1, s.m_view);
	}

	constexpr int32 StringView::compare(const size_type pos1, const size_type n1, const StringView s, const size_type pos2, const size_type n2) const noexcept
	{
		return m_view.compare(pos1, n1, s.m_view, pos2, n2);
	}

	constexpr int32 StringView::compare(const value_type* s) const noexcept
	{
		return m_view.compare(s);
	}

	constexpr int32 StringView::compare(const size_type pos1, const size_type n1, const value_type* s) const noexcept
	{
		return m_view.compare(pos1, n1, s);
	}

	constexpr int32 StringView::compare(const size_type pos1, const size_type n1, const value_type* s, const size_type n2) const noexcept
	{
		return m_view.compare(pos1, n1, s, n2);
	}

	constexpr bool StringView::starts_with(const value_type ch) const noexcept
	{
		return m_view.starts_with(ch);
	}

	constexpr bool StringView::starts_with(const value_type* s) const noexcept
	{
		return m_view.starts_with(s);
	}

	constexpr bool StringView::starts_with(const StringView s) const noexcept
	{
		return m_view.starts_with(s.m_view);
	}

	constexpr bool StringView::ends_with(const value_type ch) const noexcept
	{
		return m_view.starts_with(ch);
	}

	constexpr bool StringView::ends_with(const value_type* s) const noexcept
	{
		return m_view.starts_with(s);
	}

	constexpr bool StringView::ends_with(const StringView s) const noexcept
	{
		return m_view.starts_with(s.m_view);
	}

	constexpr bool StringView::contains(const value_type ch) const noexcept
	{
		return m_view.contains(ch);
	}

	constexpr bool StringView::contains(const value_type* s) const noexcept
	{
		return m_view.contains(s);
	}

	constexpr bool StringView::contains(const StringView s) const noexcept
	{
		return m_view.contains(s.m_view);
	}

	constexpr StringView::size_type StringView::indexOf(const StringView s, const size_type pos) const noexcept
	{
		return m_view.find(s.m_view, pos);
	}

	constexpr StringView::size_type StringView::indexOf(const value_type ch, const size_type pos) const noexcept
	{
		return m_view.find(ch, pos);
	}

	constexpr StringView::size_type StringView::indexOf(const value_type* s, const size_type pos, const size_type count) const noexcept
	{
		return m_view.find(s, pos, count);
	}

	constexpr StringView::size_type StringView::indexOf(const value_type* s, const size_type pos) const noexcept
	{
		return m_view.find(s, pos);
	}

	constexpr size_t StringView::indexOfNot(const value_type ch, const size_t pos) const noexcept
	{
		return m_view.find_first_not_of(ch, pos);
	}

	constexpr StringView::size_type StringView::lastIndexOf(const StringView s, const size_type pos) const noexcept
	{
		return m_view.rfind(s.m_view, pos);
	}

	constexpr StringView::size_type StringView::lastIndexOf(const value_type ch, const size_type pos) const noexcept
	{
		return m_view.rfind(ch, pos);
	}

	constexpr StringView::size_type StringView::lastIndexOf(const value_type* s, const size_type pos, const size_type count) const noexcept
	{
		return m_view.rfind(s, pos, count);
	}

	constexpr StringView::size_type StringView::lastIndexOf(const value_type* s, const size_type pos) const noexcept
	{
		return m_view.rfind(s, pos);
	}

	constexpr size_t StringView::lastIndexNotOf(const value_type ch, const size_t pos) const noexcept
	{
		return m_view.find_last_not_of(ch, pos);
	}

	constexpr size_t StringView::indexOfAny(const StringView anyof, const size_t pos) const noexcept
	{
		return m_view.find_first_of(anyof.m_view, pos);
	}

	constexpr size_t StringView::indexOfAny(const value_type* anyof, const size_t pos) const noexcept
	{
		return m_view.find_first_of(anyof, pos);
	}

	constexpr size_t StringView::lastIndexOfAny(const StringView anyof, const size_t pos) const noexcept
	{
		return m_view.find_last_of(anyof.m_view, pos);
	}

	constexpr size_t StringView::lastIndexOfAny(const value_type* anyof, const size_t pos) const noexcept
	{
		return m_view.find_last_of(anyof, pos);
	}

	constexpr size_t StringView::indexNotOfAny(const StringView anyof, const size_t pos) const noexcept
	{
		return m_view.find_first_not_of(anyof.m_view, pos);
	}

	constexpr size_t StringView::indexNotOfAny(const value_type* anyof, const size_t pos) const noexcept
	{
		return m_view.find_first_not_of(anyof, pos);
	}

	constexpr size_t StringView::lastIndexNotOfAny(const StringView anyof, const size_t pos) const noexcept
	{
		return m_view.find_last_not_of(anyof.m_view, pos);
	}

	constexpr size_t StringView::lastIndexNotOfAny(const value_type* anyof, const size_t pos) const noexcept
	{
		return m_view.find_last_not_of(anyof, pos);
	}

	constexpr void swap(StringView& a, StringView& b) noexcept
	{
		a.swap(b);
	}

	inline namespace Literals
	{
		inline namespace StringViewLiterals
		{
			constexpr StringView operator ""_sv(const char32_t* s, const size_t length) noexcept
			{
				return StringView{ s, length };
			}
		}
	}
}
