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
# include <cassert>
# include <string_view>
# include <string>
# include "Hash.hpp"
# include "Utility.hpp"

namespace s3d
{
	struct FormatData;
	class String;

	////////////////////////////////////////////////////////////////
	//
	//	StringView
	//
	////////////////////////////////////////////////////////////////

	/// @brief 所有権を持たない文字列クラス | Non-owning string class
	class StringView
	{
	public:

		using string_view_type			= std::u32string_view;
		using traits_type				= typename string_view_type::traits_type;
		using value_type				= typename string_view_type::value_type;
		using pointer					= typename string_view_type::pointer;
		using const_pointer				= typename string_view_type::const_pointer;
		using reference					= typename string_view_type::reference;
		using const_reference			= typename string_view_type::const_reference;
		using const_iterator			= typename string_view_type::const_iterator;
		using iterator					= typename string_view_type::iterator;
		using const_reverse_iterator	= typename string_view_type::const_reverse_iterator;
		using reverse_iterator			= typename string_view_type::reverse_iterator;
		using size_type					= typename string_view_type::size_type;
		using difference_type			= typename string_view_type::difference_type;

		////////////////////////////////////////////////////////////////
		//
		//	npos
		//
		////////////////////////////////////////////////////////////////

		static constexpr size_type npos{ static_cast<size_type>(-1) };

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr StringView() = default;

		[[nodiscard]] 
		constexpr StringView(const StringView&) = default;

		[[nodiscard]] 
		constexpr StringView(const std::u32string& s) noexcept;

		[[nodiscard]] 
		constexpr StringView(std::u32string_view s) noexcept;

		[[nodiscard]] 
		constexpr StringView(const value_type* s, size_type length) noexcept;

		[[nodiscard]] 
		constexpr StringView(const value_type* s) noexcept;

		template <class Range>
		explicit constexpr StringView(Range&& range);

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		constexpr StringView(std::nullptr_t) = delete;

		constexpr StringView& operator =(const StringView&) = default;

		////////////////////////////////////////////////////////////////
		//
		//	begin, end
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr const_iterator begin() const noexcept;

		[[nodiscard]]
		constexpr const_iterator end() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	cbegin, cend
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr const_iterator cbegin() const noexcept;

		[[nodiscard]]
		constexpr const_iterator cend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rbegin, rend
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr const_reverse_iterator rbegin() const noexcept;

		[[nodiscard]]
		constexpr const_reverse_iterator rend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	crbegin, crend
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr const_reverse_iterator crbegin() const noexcept;

		[[nodiscard]]
		constexpr const_reverse_iterator crend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr const_reference operator [](size_type index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	at
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr const_reference at(size_type index) const;

		////////////////////////////////////////////////////////////////
		//
		//	front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の要素への参照を返します。 | Returns a reference to the first element.
		/// @return 先頭の要素への参照 | Reference to the first element
		/// @remark `[0]` と同じです。 | Same as `[0]`.
		[[nodiscard]]
		constexpr const_reference front() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の要素への参照を返します。 | Returns a reference to the last element.
		/// @return 末尾の要素への参照 | Reference to the last element
		/// @remark `[size() - 1]` と同じです。 | Same as `[size() - 1]`.
		[[nodiscard]]
		constexpr const_reference back() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	data
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr const_pointer data() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の長さ（要素数）を返します。 | Returns the length of the string (number of elements).
		/// @return 文字列の長さ（要素数） | Length of the string (number of elements)
		[[nodiscard]]
		constexpr size_type size() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	ssize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の長さ（要素数）を符号付き整数型で返します。 | Returns the length of the string (number of elements) as a signed integer type.
		/// @return 文字列の長さ（要素数） | Length of the string (number of elements)
		[[nodiscard]]
		constexpr isize ssize() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	size_bytes
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列のバイト数を返します。 | Returns the number of bytes in the string.
		/// @remark `size() * sizeof(value_type)` と同じです。 | Same as `size() * sizeof(value_type)`.
		/// @return　文字列のバイト数 | Number of bytes in the string
		[[nodiscard]]
		constexpr size_type size_bytes() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	length
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の長さ（要素数）を返します。 | Returns the length of the string (number of elements).
		/// @remark `.size()` と同じです。 | Same as `size()`.
		/// @return 文字列の長さ（要素数） | Length of the string (number of elements)
		[[nodiscard]]
		constexpr size_type length() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	max_size
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr size_type max_size() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	empty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列が空であるかを返します。 | Returns whether the string is empty.
		/// @return 文字列が空である場合 true, それ以外の場合は false | If the string is empty, true, otherwise false
		[[nodiscard]]
		constexpr bool empty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列が空であるかを返します。 | Returns whether the string is empty.
		/// @remark `empty()` と同じです。 | Same as `empty()`.
		/// @return 文字列が空である場合 true, それ以外の場合は false | If the string is empty, true, otherwise false
		[[nodiscard]]
		constexpr bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列が空でないかを返します。 | Returns whether the string is not empty.
		/// @return 文字列が空でない場合 true, それ以外の場合は false | If the string is not empty, true, otherwise false
		[[nodiscard]]
		explicit constexpr operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator std::u32string_view
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr operator std::u32string_view() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	remove_prefix
		//
		////////////////////////////////////////////////////////////////

		constexpr void remove_prefix(size_type n) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	remove_suffix
		//
		////////////////////////////////////////////////////////////////

		constexpr void remove_suffix(size_type n) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		constexpr void clear() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		constexpr void swap(StringView& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	copy
		//
		////////////////////////////////////////////////////////////////

		constexpr size_type copy(value_type* dst, size_type n, size_type pos = 0) const;

		////////////////////////////////////////////////////////////////
		//
		//	substr
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr StringView substr(size_type pos = 0, size_type n = npos) const;

		////////////////////////////////////////////////////////////////
		//
		//	compare
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr int32 compare(StringView s) const noexcept;

		[[nodiscard]]
		constexpr int32 compare(size_type pos1, size_type n1, StringView s) const noexcept;

		[[nodiscard]]
		constexpr int32 compare(size_type pos1, size_type n1, StringView s, size_type pos2, size_type n2) const noexcept;

		[[nodiscard]]
		constexpr int32 compare(const value_type* s) const noexcept;

		[[nodiscard]]
		constexpr int32 compare(size_type pos1, size_type n1, const value_type* s) const noexcept;

		[[nodiscard]]
		constexpr int32 compare(size_type pos1, size_type n1, const value_type* s, size_type n2) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	starts_with
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列が指定した文字から始まるかを返します。 | Returns whether the string starts with the specified character.
		/// @param ch 検索する文字 | Character to search for
		/// @return 指定した文字から始まる場合 true, それ以外の場合は false | If the string starts with the specified character, true, otherwise false
		[[nodiscard]]
		constexpr bool starts_with(value_type ch) const noexcept;

		/// @brief 文字列が指定した文字列から始まるかを返します。 | Returns whether the string starts with the specified string.
		/// @param s 検索する文字列 | String to search for
		/// @return 指定した文字列から始まる場合 true, それ以外の場合は false | If the string starts with the specified string, true, otherwise false
		[[nodiscard]]
		constexpr bool starts_with(const value_type* s) const noexcept;

		/// @brief 文字列が指定した文字列から始まるかを返します。 | Returns whether the string starts with the specified string.
		/// @param s 検索する文字列 | String to search for
		/// @return 指定した文字列から始まる場合 true, それ以外の場合は false | If the string starts with the specified string, true, otherwise false
		[[nodiscard]]
		constexpr bool starts_with(StringView s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	ends_with
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列が指定した文字で終わるかを返します。 | Returns whether the string ends with the specified character.
		/// @param ch 検索する文字 | Character to search for
		/// @return 指定した文字で終わる場合 true, それ以外の場合は false | If the string ends with the specified character, true, otherwise false
		[[nodiscard]]
		constexpr bool ends_with(value_type ch) const noexcept;

		/// @brief 文字列が指定した文字列で終わるかを返します。 | Returns whether the string ends with the specified string.
		/// @param s 検索する文字列 | String to search for
		/// @return 指定した文字列で終わる場合 true, それ以外の場合は false | If the string ends with the specified string, true, otherwise false
		[[nodiscard]]
		constexpr bool ends_with(const value_type* s) const noexcept;

		/// @brief 文字列が指定した文字列で終わるかを返します。 | Returns whether the string ends with the specified string.
		/// @param s 検索する文字列 | String to search for
		/// @return 指定した文字列で終わる場合 true, それ以外の場合は false | If the string ends with the specified string, true, otherwise false
		[[nodiscard]]
		constexpr bool ends_with(StringView s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	contains
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列が指定した文字を含むかを返します。 | Returns whether the string contains the specified character.
		/// @param ch 検索する文字 | Character to search for
		/// @return 指定した文字を含む場合 true, それ以外の場合は false | If the string contains the specified character, true, otherwise false
		[[nodiscard]]
		constexpr bool contains(value_type ch) const noexcept;

		/// @brief 文字列が指定した文字列を含むかを返します。 | Returns whether the string contains the specified string.
		/// @param ch 検索する文字列 | String to search for
		/// @return 指定した文字列を含む場合 true, それ以外の場合は false | If the string contains the specified string, true, otherwise false
		[[nodiscard]]
		constexpr bool contains(const value_type* s) const noexcept;

		/// @brief 文字列が指定した文字列を含むかを返します。 | Returns whether the string contains the specified string.
		/// @param ch 検索する文字列 | String to search for
		/// @return 指定した文字列を含む場合 true, それ以外の場合は false | If the string contains the specified string, true, otherwise false
		[[nodiscard]]
		constexpr bool contains(StringView s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	find
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr size_type find(StringView s, size_type pos = 0) const noexcept;

		[[nodiscard]]
		constexpr size_type find(value_type ch, size_type pos = 0) const noexcept;

		[[nodiscard]]
		constexpr size_type find(const value_type* s, size_type pos, size_type count) const noexcept;

		[[nodiscard]]
		constexpr size_type find(const value_type* s, size_type pos = 0) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rfind
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr size_type rfind(StringView s, size_type pos = npos) const noexcept;

		[[nodiscard]]
		constexpr size_type rfind(value_type ch, size_type pos = npos) const noexcept;

		[[nodiscard]]
		constexpr size_type rfind(const value_type* s, size_type pos, size_type count) const noexcept;

		[[nodiscard]]
		constexpr size_type rfind(const value_type* s, size_type pos = npos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	find_first_of
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr size_type find_first_of(StringView s, size_type pos = 0) const noexcept;
		
		[[nodiscard]]
		constexpr size_type find_first_of(value_type ch, size_type pos = 0) const noexcept;

		[[nodiscard]]
		constexpr size_type find_first_of(const value_type* s, size_type pos, size_type count) const noexcept;

		[[nodiscard]]
		constexpr size_type find_first_of(const value_type* s, size_type pos = 0) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	find_last_of
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr size_type find_last_of(StringView s, size_type pos = npos) const noexcept;

		[[nodiscard]]
		constexpr size_type find_last_of(value_type ch, size_type pos = npos) const noexcept;

		[[nodiscard]]
		constexpr size_type find_last_of(const value_type* s, size_type pos, size_type count) const noexcept;

		[[nodiscard]]
		constexpr size_type find_last_of(const value_type* s, size_type pos = npos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	find_first_not_of
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr size_type find_first_not_of(StringView s, size_type pos = 0) const noexcept;

		[[nodiscard]]
		constexpr size_type find_first_not_of(value_type ch, size_type pos = 0) const noexcept;

		[[nodiscard]]
		constexpr size_type find_first_not_of(const value_type* s, size_type pos, size_type count) const;

		[[nodiscard]]
		constexpr size_type find_first_not_of(const value_type* s, size_type pos = 0) const;

		////////////////////////////////////////////////////////////////
		//
		//	find_last_not_of
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr size_type find_last_not_of(StringView s, size_type pos = npos) const noexcept;

		[[nodiscard]]
		constexpr size_type find_last_not_of(value_type ch, size_type pos = npos) const noexcept;

		[[nodiscard]]
		constexpr size_type find_last_not_of(const value_type* s, size_type pos, size_type count) const noexcept;

		[[nodiscard]]
		constexpr size_type find_last_not_of(const value_type* s, size_type pos = npos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列のハッシュを返します。 | Returns the hash of the string.
		/// @return 文字列のハッシュ | Hash of the string
		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を UTF-8 文字列に変換します。 | Converts the string to a UTF-8 string.
		/// @return 変換された文字列 | Converted string
		[[nodiscard]]
		std::string toUTF8() const;

		////////////////////////////////////////////////////////////////
		//
		//	toUTF16
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を UTF-16 文字列に変換します。 | Converts the string to a UTF-16 string.
		/// @return 変換された文字列 | Converted string
		[[nodiscard]]
		std::u16string toUTF16() const;

		////////////////////////////////////////////////////////////////
		//
		//	toWstr
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を std::wstring に変換します。 | Converts the string to a std::wstring.
		/// @return 変換された文字列 | Converted string
		[[nodiscard]]
		std::wstring toWstr() const;

		////////////////////////////////////////////////////////////////
		//
		//	toUTF32
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を UTF-32 文字列に変換します。 | Converts the string to a UTF-32 string.
		/// @return 変換された文字列 | Converted string
		[[nodiscard]]
		constexpr std::u32string toUTF32() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	view
		//
		////////////////////////////////////////////////////////////////

		/// @brief std::u32string_view を返します。 | Returns std::u32string_view.
		/// @return std::u32string_view 
		constexpr std::u32string_view view() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toString
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を String 型に変換します。 | Converts the string to a String.
		/// @return 変換された文字列 | Converted string
		[[nodiscard]]
		String toString() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==, operator <=>
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(StringView, StringView) noexcept = default;

		[[nodiscard]]
		friend constexpr auto operator <=>(StringView, StringView) noexcept = default;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		friend void swap(StringView& lhs, StringView& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		friend String operator +(StringView lhs, StringView rhs);

		friend String operator +(StringView lhs, const value_type* rhs);

		friend String operator +(StringView lhs, value_type rhs);

		friend String operator +(const value_type* lhs, StringView rhs);

		friend String operator +(value_type lhs, StringView rhs);

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		friend std::ostream& operator <<(std::ostream& os, const StringView& value);

		friend std::wostream& operator <<(std::wostream& os, const StringView& value);

		friend std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const StringView& value);

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, StringView s);

	private:

		[[noreturn]]
		static void ThrowAtOutOfRange();

		[[noreturn]]
		static void ThrowSubstrOutOfRange();

		string_view_type m_view;
	};

	inline namespace Literals
	{
		inline namespace StringViewLiterals
		{
			[[nodiscard]]
			inline constexpr StringView operator ""_sv(const char32_t* s, size_t length) noexcept;
		}
	}

	using FilePathView = StringView;
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::StringView>
{
	[[nodiscard]]
	size_t operator ()(const s3d::StringView& value) const noexcept
	{
		return value.hash();
	}
};

template <>
inline constexpr bool std::ranges::enable_view<s3d::StringView> = true;

template <>
inline constexpr bool std::ranges::enable_borrowed_range<s3d::StringView> = true;

# include "detail/StringView.ipp"
