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

	constexpr StringView::StringView(const std::u32string& s SIV3D_LIFETIMEBOUND) noexcept
		: m_view{ s } {}

	constexpr StringView::StringView(const std::u32string_view s) noexcept
		: m_view{ s } {}

	constexpr StringView::StringView(const value_type* s, const size_type length) noexcept
		: m_view{ s, length } {}

	constexpr StringView::StringView(const value_type* s) noexcept
		: m_view{ s } {}

	template <class Range>
		requires(
		!std::same_as<std::remove_cvref_t<Range>, StringView>
		&& std::ranges::contiguous_range<Range>
		&& std::ranges::sized_range<Range>
		&& std::same_as<std::ranges::range_value_t<Range>, char32>
		&& !std::is_convertible_v<Range, const char32*>
		&& !requires(std::remove_cvref_t<Range>& range) { range.operator StringView; })
	constexpr StringView::StringView(Range&& range SIV3D_LIFETIMEBOUND)
		: m_view{ std::forward<Range>(range) } {}

	////////////////////////////////////////////////////////////////
	//
	//	begin, end
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::const_iterator StringView::begin() const noexcept
	{
		return m_view.begin();
	}

	constexpr StringView::const_iterator StringView::end() const noexcept
	{
		return m_view.end();
	}

	////////////////////////////////////////////////////////////////
	//
	//	cbegin, cend
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::const_iterator StringView::cbegin() const noexcept
	{
		return m_view.cbegin();
	}

	constexpr StringView::const_iterator StringView::cend() const noexcept
	{
		return m_view.cend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	rbegin, rend
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::const_reverse_iterator StringView::rbegin() const noexcept
	{
		return m_view.rbegin();
	}

	constexpr StringView::const_reverse_iterator StringView::rend() const noexcept
	{
		return m_view.rend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	crbegin, crend
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::const_reverse_iterator StringView::crbegin() const noexcept
	{
		return m_view.crbegin();
	}

	constexpr StringView::const_reverse_iterator StringView::crend() const noexcept
	{
		return m_view.crend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::const_reference StringView::operator [](const size_type index) const noexcept
	{
		return m_view[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	at
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::const_reference StringView::at(const size_type index) const
	{
		if (m_view.size() <= index)
		{
			ThrowAtOutOfRange();
		}

		return m_view[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	front
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::const_reference StringView::front() const noexcept
	{
		assert((not m_view.empty()) && "cannot call StringView::front(): on empty StringView");

		return m_view[0];
	}

	////////////////////////////////////////////////////////////////
	//
	//	back
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::const_reference StringView::back() const noexcept
	{
		assert((not m_view.empty()) && "cannot call StringView::back() on empty StringView");

		return m_view[m_view.size() - 1];
	}

	////////////////////////////////////////////////////////////////
	//
	//	data
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::const_pointer StringView::data() const noexcept
	{
		return m_view.data();
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::size_type StringView::size() const noexcept
	{
		return m_view.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	ssize
	//
	////////////////////////////////////////////////////////////////

	constexpr isize StringView::ssize() const noexcept
	{
		return static_cast<isize>(m_view.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	size_bytes
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::size_type StringView::size_bytes() const noexcept
	{
		return (m_view.size() * sizeof(value_type));
	}

	////////////////////////////////////////////////////////////////
	//
	//	length
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::size_type StringView::length() const noexcept
	{
		return m_view.length();
	}

	////////////////////////////////////////////////////////////////
	//
	//	max_size
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::size_type StringView::max_size() const noexcept
	{
		return m_view.max_size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	empty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool StringView::empty() const noexcept
	{
		return m_view.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool StringView::isEmpty() const noexcept
	{
		return m_view.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	indexInBounds
	//
	////////////////////////////////////////////////////////////////

	constexpr bool StringView::indexInBounds(const size_type index) const noexcept
	{
		return (index < m_view.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::operator bool() const noexcept
	{
		return (not m_view.empty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator std::u32string_view
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::operator std::u32string_view() const noexcept
	{
		return m_view;
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove_prefix
	//
	////////////////////////////////////////////////////////////////

	constexpr void StringView::remove_prefix(const size_type n) noexcept
	{
		m_view.remove_prefix(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove_suffix
	//
	////////////////////////////////////////////////////////////////

	constexpr void StringView::remove_suffix(const size_type n) noexcept
	{
		m_view.remove_suffix(n);
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	constexpr void StringView::clear() noexcept
	{
		m_view = string_view_type{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	constexpr void StringView::swap(StringView& other) noexcept
	{
		m_view.swap(other.m_view);
	}

	////////////////////////////////////////////////////////////////
	//
	//	copy
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::size_type StringView::copy(value_type* dst, const size_type n, const size_type pos) const
	{
		return m_view.copy(dst, n, pos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	substr
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView StringView::substr(const size_type pos, const size_type n) const
	{
		if (m_view.size() < pos)
		{
			ThrowSubstrOutOfRange();
		}

		return StringView{ (m_view.data() + pos), Min(n, (m_view.size() - pos)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	compare
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	starts_with
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	ends_with
	//
	////////////////////////////////////////////////////////////////

	constexpr bool StringView::ends_with(const value_type ch) const noexcept
	{
		return m_view.ends_with(ch);
	}

	constexpr bool StringView::ends_with(const value_type* s) const noexcept
	{
		return m_view.ends_with(s);
	}

	constexpr bool StringView::ends_with(const StringView s) const noexcept
	{
		return m_view.ends_with(s.m_view);
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	find
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::size_type StringView::find(const StringView s, const size_type pos) const noexcept
	{
		return m_view.find(s.m_view, pos);
	}

	constexpr StringView::size_type StringView::find(const value_type ch, const size_type pos) const noexcept
	{
		return m_view.find(ch, pos);
	}

	constexpr StringView::size_type StringView::find(const value_type* s, const size_type pos, const size_type count) const noexcept
	{
		return m_view.find(s, pos, count);
	}

	constexpr StringView::size_type StringView::find(const value_type* s, const size_type pos) const noexcept
	{
		return m_view.find(s, pos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rfind
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::size_type StringView::rfind(const StringView s, const size_type pos) const noexcept
	{
		return m_view.rfind(s.m_view, pos);
	}

	constexpr StringView::size_type StringView::rfind(const value_type ch, const size_type pos) const noexcept
	{
		return m_view.rfind(ch, pos);
	}

	constexpr StringView::size_type StringView::rfind(const value_type* s, const size_type pos, const size_type count) const noexcept
	{
		return m_view.rfind(s, pos, count);
	}

	constexpr StringView::size_type StringView::rfind(const value_type* s, const size_type pos) const noexcept
	{
		return m_view.rfind(s, pos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	find_first_of
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t StringView::find_first_of(const StringView s, const size_type pos) const noexcept
	{
		return m_view.find_first_of(s.m_view, pos);
	}

	constexpr StringView::size_type StringView::find_first_of(const value_type ch, const size_type pos) const noexcept
	{
		return m_view.find_first_of(ch, pos);
	}

	constexpr StringView::size_type StringView::find_first_of(const value_type* s, const size_type pos, const size_type count) const noexcept
	{
		return m_view.find_first_of(s, pos, count);
	}

	constexpr StringView::size_type StringView::find_first_of(const value_type* s, const size_type pos) const noexcept
	{
		return m_view.find_first_of(s, pos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	find_last_of
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::size_type StringView::find_last_of(const StringView s, const size_type pos) const noexcept
	{
		return m_view.find_last_of(s.m_view, pos);
	}

	constexpr StringView::size_type StringView::find_last_of(const value_type ch, const size_type pos) const noexcept
	{
		return m_view.find_last_of(ch, pos);
	}

	constexpr StringView::size_type StringView::find_last_of(const value_type* s, const size_type pos, const size_type count) const noexcept
	{
		return m_view.find_last_of(s, pos, count);
	}

	constexpr StringView::size_type StringView::find_last_of(const value_type* s, const size_type pos) const noexcept
	{
		return m_view.find_last_of(s, pos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	find_first_not_of
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::size_type StringView::find_first_not_of(const StringView s, const size_type pos) const noexcept
	{
		return m_view.find_first_not_of(s.m_view, pos);
	}

	constexpr StringView::size_type StringView::find_first_not_of(const value_type ch, const size_type pos) const noexcept
	{
		return m_view.find_first_not_of(ch, pos);
	}

	constexpr StringView::size_type StringView::find_first_not_of(const value_type* s, const size_type pos, const size_type count) const
	{
		return m_view.find_first_not_of(s, pos, count);
	}

	constexpr StringView::size_type StringView::find_first_not_of(const value_type* s, const size_type pos) const
	{
		return m_view.find_first_not_of(s, pos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	find_last_not_of
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::size_type StringView::find_last_not_of(const StringView s, const size_type pos) const noexcept
	{
		return m_view.find_last_not_of(s.m_view, pos);
	}

	constexpr StringView::size_type StringView::find_last_not_of(const value_type ch, const size_type pos) const noexcept
	{
		return m_view.find_last_not_of(ch, pos);
	}

	constexpr StringView::size_type StringView::find_last_not_of(const value_type* s, const size_type pos, const size_type count) const noexcept
	{
		return m_view.find_last_not_of(s, pos, count);
	}

	constexpr StringView::size_type StringView::find_last_not_of(const value_type* s, const size_type pos) const noexcept
	{
		return m_view.find_last_not_of(s, pos);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 StringView::hash() const noexcept
	{
		return Hash(m_view.data(), (m_view.size() * sizeof(StringView::value_type)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	toUTF32
	//
	////////////////////////////////////////////////////////////////

	constexpr std::u32string StringView::toUTF32() const noexcept
	{
		return std::u32string{ m_view };
	}

	////////////////////////////////////////////////////////////////
	//
	//	view
	//
	////////////////////////////////////////////////////////////////

	constexpr std::u32string_view StringView::view() const noexcept
	{
		return m_view;
	}

	inline namespace Literals
	{
		inline namespace StringViewLiterals
		{
			inline constexpr StringView operator ""_sv(const char32_t* s, const size_t length) noexcept
			{
				return StringView{ s, length };
			}
		}
	}
}
