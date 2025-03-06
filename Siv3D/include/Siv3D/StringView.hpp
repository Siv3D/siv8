//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
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
	template <class Type, class Allocator>
	class Array;

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
		constexpr StringView(const std::u32string& s SIV3D_LIFETIMEBOUND) noexcept;

		[[nodiscard]] 
		constexpr StringView(std::u32string_view s) noexcept;

		[[nodiscard]] 
		constexpr StringView(const value_type* s, size_type length) noexcept;

		[[nodiscard]] 
		constexpr StringView(const value_type* s) noexcept;

		template <class Range>
			requires(
			!std::same_as<std::remove_cvref_t<Range>, StringView>
			&& std::ranges::contiguous_range<Range>
			&& std::ranges::sized_range<Range>
			&& std::same_as<std::ranges::range_value_t<Range>, char32>
			&& !std::is_convertible_v<Range, const char32*>
			&& !requires(std::remove_cvref_t<Range>& range) { range.operator StringView; }
			&& std::ranges::borrowed_range<Range>)
		[[nodiscard]]
		explicit constexpr StringView(Range&& range);

		template <class Range>
			requires(
		!std::same_as<std::remove_cvref_t<Range>, StringView>
			&& std::ranges::contiguous_range<Range>
			&& std::ranges::sized_range<Range>
			&& std::same_as<std::ranges::range_value_t<Range>, char32>
			&& !std::is_convertible_v<Range, const char32*>
			&& !requires(std::remove_cvref_t<Range>& range) { range.operator StringView; }
			&& !std::ranges::borrowed_range<Range>)
			[[nodiscard]]
		explicit constexpr StringView(Range&& range SIV3D_LIFETIMEBOUND);

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

		/// @brief 文字列の先頭位置を指すイテレータを返します。
		/// @return 文字列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator begin() const noexcept;

		/// @brief 文字列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 文字列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator end() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	cbegin, cend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の先頭位置を指すイテレータを返します。
		/// @return 文字列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator cbegin() const noexcept;

		/// @brief 文字列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 文字列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator cend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rbegin, rend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の末尾位置を指すリバース・イテレータを返します。
		/// @return 文字列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rbegin() const noexcept;

		/// @brief 文字列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 文字列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	crbegin, crend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の末尾位置を指すリバース・イテレータを返します。
		/// @return 文字列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator crbegin() const noexcept;

		/// @brief 文字列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 文字列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator crend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の要素を返します。 | Returns the element at the specified index.
		/// @param index インデックス | Index
		/// @return 指定したインデックスの要素 | Element at the specified index
		[[nodiscard]]
		constexpr const_reference operator [](size_type index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の要素を返します。 | Returns the element at the specified index.
		/// @param index インデックス | Index
		/// @return 指定したインデックスの要素 | Element at the specified index
		/// @throw std::out_of_range index が範囲外の場合 | Throws if index is out of range
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
		/// @return 文字列のバイト数 | Number of bytes in the string
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
		//	indexInBounds
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスが配列の範囲内かを返します。
		/// @param index インデックス
		/// @return インデックスが範囲内の場合 true, それ以外の場合は false
		/// @remark `index < size()` と同じです。
		[[nodiscard]]
		constexpr bool indexInBounds(size_type index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列が空でないかを返します。 | Returns whether the string is not empty.
		/// @return 文字列が空でない場合 true, それ以外の場合は false | If the string is not empty, true, otherwise false
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept;

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

		/// @brief 文字列のハッシュ値を返します。 | Returns the hash value of the string.
		/// @return 文字列のハッシュ値 | Hash value of the string
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
		//	all
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素が条件を満たすかを返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return すべての要素が条件を満たすか、配列が空の場合 true, それ以外の場合は false
		template <class Fty>
		[[nodiscard]]
		constexpr bool all(Fty f) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	any
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす要素があるかを返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 条件を満たす要素が 1 つでもあれば true, 俺以外の場合は false
		/// @remark `.contains_if(f)` と同じです。
		template <class Fty>
		[[nodiscard]]
		constexpr bool any(Fty f) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	case_insensitive_compare
		//
		////////////////////////////////////////////////////////////////

		/// @brief 英字の大小を無視して文字列の大小を比較します。
		/// @param s 比較対象の文字列
		/// @return 比較結果。等しければ 0, 小さければ -1, 大きければ 1		
		[[nodiscard]]
		int32 case_insensitive_compare(StringView s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	case_insensitive_equals
		//
		////////////////////////////////////////////////////////////////

		/// @brief 英字の大小を無視して文字列を比較します。
		/// @param s 比較対象の文字列
		/// @return 英字の大小を無視した時に文字列が等しい場合 true, それ以外の場合は false	
		[[nodiscard]]
		bool case_insensitive_equals(StringView s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	capitalized
		//
		////////////////////////////////////////////////////////////////

		/// @brief 最初に登場する英字を大文字にした文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String capitalized() const;

		////////////////////////////////////////////////////////////////
		//
		//	choice
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の要素を 1 つランダムに返します。
		/// @return 文字列からランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type choice() const;

		/// @brief 指定した乱数エンジンを用いて、配列の要素を 1 つランダムに返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 文字列からランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type choice(Concept::UniformRandomBitGenerator auto&& rbg) const;

		////////////////////////////////////////////////////////////////
		//
		//	contains_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素があるかを返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @remark `.any(f)` と同じです。
		/// @return 条件を満たす要素が 1 つでもあれば true, 俺以外の場合は false
		template <class Fty>
		[[nodiscard]]
		constexpr bool contains_if(Fty f) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	count
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しい要素の個数を返します。
		/// @param ch 検索する値
		/// @return 指定した値と等しい要素の個数	
		[[nodiscard]]
		constexpr int64 count(value_type ch) const noexcept;

		/// @brief 指定した文字列の個数を数えます。
		/// @param s 検索する文字列
		/// @remark `"aaa"` から `"aa"` を検索する場合の結果は 2 です。
		/// @return 見つかった文字列の個数
		[[nodiscard]]
		int64 count(StringView s) const;

		////////////////////////////////////////////////////////////////
		//
		//	count_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす要素の個数を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 条件を満たす要素の個数	
		template <class Fty>
		[[nodiscard]]
		constexpr int64 count_if(Fty f) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	each
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (const auto& ch : s) f(ch);` と同じです。
		template <class Fty>
		constexpr void each(Fty f) const requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	each_index
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (size_t i = 0; auto ch : s) f(i++, ch);` と同じです。
		template <class Fty>
		constexpr void each_index(Fty f) const requires std::invocable<Fty&, size_t, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	each_sindex
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (isize i = 0; auto ch : s) f(i++, ch);` と同じです。
		template <class Fty>
		constexpr void each_sindex(Fty f) const requires std::invocable<Fty&, isize, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	expandTabs
		//
		////////////////////////////////////////////////////////////////

		/// @brief タブ文字を半角空白に置換した文字列を返します。
		/// @param tabSize タブ置換後の半角空白の数
		/// @return 新しい文字列	
		[[nodiscard]]
		String expandTabs(size_type tabSize = 4) const;

		////////////////////////////////////////////////////////////////
		//
		//	fetch
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにある要素を返します。インデックスが範囲外の場合デフォルト値を返します。
		/// @param index インデックス
		/// @param defaultValue インデックスが範囲外の場合に返すデフォルト値
		/// @return 指定したインデックスにある要素、範囲外の場合 defaultValue	
		[[nodiscard]]
		constexpr value_type fetch(size_type index, value_type defaultValue) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	filter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素だけを集めた新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 指定した条件を満たす要素を集めた新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String filter(Fty f) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	isSorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列が昇順にソートされているかを返します。
		/// @return 文字列が昇順にソートされている場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isSorted() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	layout
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字数で改行するようにした新しい文字列を返します。
		/// @param width 1 行の文字数
		/// @return 新しい文字列	
		[[nodiscard]]
		String layout(size_type width) const;

		////////////////////////////////////////////////////////////////
		//
		//	leftPadded
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の左を埋め文字で埋めた新しい文字列を返します。
		/// @param length 文字列の左を埋め文字で埋めた後の文字列の長さ
		/// @param fillChar 埋め文字
		/// @remark 元の文字列の長さが `length` より大きい場合、元の文字列のコピーが返ります。
		/// @return 新しい文字列
		[[nodiscard]]
		String leftPadded(size_type length, value_type fillChar = U' ') const;

		////////////////////////////////////////////////////////////////
		//
		//	levenshteinDistanceFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の文字列とのレーベンシュタイン距離を計算して返します。
		/// @param other 別の文字列
		/// @return 別の文字列とのレーベンシュタイン距離
		/// @remark `.levenshteinDistanceTo(other)` と同じです。
		[[nodiscard]]
		size_type levenshteinDistanceFrom(StringView other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	levenshteinDistanceTo
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の文字列とのレーベンシュタイン距離を計算して返します。
		/// @param other 別の文字列
		/// @return 別の文字列とのレーベンシュタイン距離
		/// @remark `.levenshteinDistanceFrom(other)` と同じです。
		[[nodiscard]]
		size_type levenshteinDistanceTo(StringView other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lowercase, lowercased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 英字をすべて小文字にした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String lowercased() const;

		////////////////////////////////////////////////////////////////
		//
		//	leftTrim, leftTrimmed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の先頭に付いている空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String leftTrimmed() const;

		////////////////////////////////////////////////////////////////
		//
		//	map
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の各要素に関数を適用した戻り値からなる配列を返します。
		/// @tparam Fty 各文字に適用する関数の型
		/// @param f 各文字に適用する関数
		/// @return 文字列の各要素に関数を適用した戻り値からなる配列
		template <class Fty>
		[[nodiscard]]
		constexpr auto map(Fty f) const requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	none
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす要素が存在しないかを返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 条件を満たす要素数が 0 個の場合 true, それ以外の場合は false
		template <class Fty>
		[[nodiscard]]
		constexpr bool none(Fty f) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	removed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字を文字列から削除した新しい文字列を返します。
		/// @param ch 削除する文字
		/// @return 新しい文字列
		[[nodiscard]]
		String removed(value_type ch) const;

		/// @brief 指定した文字列を削除した新しい文字列を返します。
		/// @param s 削除する文字列
		/// @return 新しい文字列
		[[nodiscard]]
		String removed(StringView s) const;

		////////////////////////////////////////////////////////////////
		//
		//	removed_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにある要素を文字列から削除した新しい文字列を返します。
		/// @param index インデックス
		/// @remark 範囲外のインデックスを指定した場合、何も削除しません。
		/// @return 新しい文字列
		[[nodiscard]]
		String removed_at(size_type index) const;

		////////////////////////////////////////////////////////////////
		//
		//	removed_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素を削除した新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String removed_if(Fty f) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	repeat
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を指定した回数繰り返した新しい文字列を返します。
		/// @param count 繰り返す回数
		/// @return 新しい文字列
		[[nodiscard]]
		String repeat(size_t count) const;

		////////////////////////////////////////////////////////////////
		//
		//	replaced
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字を置換した新しい文字列を返します。
		/// @param oldChar 置換対象の文字
		/// @param newChar 置換後の文字
		/// @return 新しい文字列
		[[nodiscard]]
		String replaced(value_type oldChar, value_type newChar) const;

		/// @brief 指定した文字列を置換した新しい文字列を返します。
		/// @param oldStr 置換対象の文字列
		/// @param newStr 置換後の文字列
		/// @return 新しい文字列
		[[nodiscard]]
		String replaced(StringView oldStr, StringView newStr) const;

		////////////////////////////////////////////////////////////////
		//
		//	replaced_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素を別の文字に置き換えた新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @param newChar 置換後の文字
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String replaced_if(Fty f, value_type newChar) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	reversed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を反転した文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String reversed() const;

		////////////////////////////////////////////////////////////////
		//
		//	reverse_each
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各要素を引数にして関数を呼び出します。順番は後ろからです。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		constexpr void reverse_each(Fty f) const requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	rightPadded
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の右を埋め文字で埋めた新しい文字列を返します。
		/// @param length 文字列の右を埋め文字で埋めた後の文字列の長さ
		/// @param fillChar 埋め文字
		/// @remark 元の文字列の長さが `length` より大きい場合、元の文字列のコピーが返ります。
		/// @return 新しい文字列
		[[nodiscard]]
		String rightPadded(size_type length, value_type fillChar = U' ') const;

		////////////////////////////////////////////////////////////////
		//
		//	rightTrimmed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の末尾に付いている空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String rightTrimmed() const;

		////////////////////////////////////////////////////////////////
		//
		//	rotated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を中心に前半と後半を入れ替えた新しい文字列を返します。
		/// @param index 入れ替える位置
		/// @remark index が文字列の長さ以上の場合、入れ替えは行われません。
		/// @return 新しい文字列
		[[nodiscard]]
		String rotated(size_type index) const;

		////////////////////////////////////////////////////////////////
		//
		//	rsorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を降順にソートした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String rsorted() const;

		////////////////////////////////////////////////////////////////
		//
		//	shuffled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素をシャッフルした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String shuffled() const;

		/// @brief 指定された乱数生成器を使って要素をシャッフルした新しい文字列を返します。
		/// @param rbg 乱数生成器
		/// @return 新しい文字列
		[[nodiscard]]
		String shuffled(Concept::UniformRandomBitGenerator auto&& rbg) const;

		////////////////////////////////////////////////////////////////
		//
		//	sorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を昇順にソートした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String sorted() const;

		////////////////////////////////////////////////////////////////
		//
		//	sorted_by
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した比較関数を用いて要素を昇順にソートした新しい文字列を返します。
		/// @tparam Fty 比較関数の型
		/// @param f 比較関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		String sorted_by(Fty f) const requires std::predicate<Fty&, const value_type&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	split
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した区切り文字で文字列を分割した結果を String の配列で返します。
		/// @param ch 区切り文字
		/// @return 分割された文字列
		[[nodiscard]]
		Array<String, std::allocator<String>> split(value_type ch) const;

		////////////////////////////////////////////////////////////////
		//
		//	splitView
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した区切り文字で文字列を分割した結果を StringView の配列で返します。
		/// @param ch 区切り文字
		/// @return 分割された文字列
		/// @remark 戻り値は元の文字列を参照します。元の文字列のライフタイムに注意してください。
		[[nodiscard]]
		Array<StringView, std::allocator<StringView>> splitView(value_type ch) const SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	split_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置で文字列を分割します。
		/// @param pos 位置
		/// @remark pos が文字列の長さ以上の場合 `{ *this, U"" }` を返します。
		/// @return 分割された文字列
		[[nodiscard]]
		std::pair<StringView, StringView> split_at(size_type pos) const;

		////////////////////////////////////////////////////////////////
		//
		//	split_lines
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を行ごとに分割します。
		/// @return 分割された文字列
		[[nodiscard]]
		Array<String, std::allocator<String>> split_lines() const;

		////////////////////////////////////////////////////////////////
		//
		//	swapcased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 英字の大文字と小文字を入れ替えた文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String swapcased() const;

		////////////////////////////////////////////////////////////////
		//
		//	take
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の先頭から指定された文字数分取り出した新しい文字列ビューを返します。
		/// @param n 取り出す文字数
		/// @return 新しい文字列
		/// @remark n が文字列の長さ以上の場合、元の文字列のコピーが返ります。
		[[nodiscard]]
		String take(size_t n) const;

		////////////////////////////////////////////////////////////////
		//
		//	take_while
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から指定された条件を満たし続ける間、要素を取り出します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		String take_while(Fty f) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	trimmed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の先頭と末尾に付いている空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String trimmed() const;

		////////////////////////////////////////////////////////////////
		//
		//	uppercased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 英字をすべて大文字にした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String uppercased() const;

		////////////////////////////////////////////////////////////////
		//
		//	stable_uniqued
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列をソートせずに、重複する文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String stable_uniqued() const;

		////////////////////////////////////////////////////////////////
		//
		//	sorted_and_uniqued
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列をソートしたあとに重複する文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String sorted_and_uniqued() const;

		////////////////////////////////////////////////////////////////
		//
		//	uniqued_consecutive
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ文字が連続する場合、その先頭以外を除去した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String uniqued_consecutive() const;

		////////////////////////////////////////////////////////////////
		//
		//	values_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの文字からなる新しい文字列を返します。
		/// @param indices インデックス
		/// @remark `StringView{ U"abcde" }.values_at({0, 3, 4}); // U"ade"`
		/// @return 新しい文字列
		[[nodiscard]]
		String values_at(std::initializer_list<size_type> indices) const;

		////////////////////////////////////////////////////////////////
		//
		//	xml_escape, xml_escaped
		//
		////////////////////////////////////////////////////////////////

		/// @brief XML エスケープした文字列を返します。
		/// @remark &quot;, \, &amp;, &gt;, &lt; のエスケープを行います
		/// @return 新しい文字列
		[[nodiscard]]
		String xml_escaped() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==, operator <=>
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(StringView lhs, StringView rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				return (lhs.m_view == rhs.m_view);
			}
			else
			{
				const size_t length = lhs.m_view.size();

				if (length != rhs.m_view.size())
				{
					return false;
				}

				return StringView::StringEquals(lhs.m_view.data(), rhs.m_view.data(), length);
			}
		}

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

		friend std::ostream& operator <<(std::ostream& output, const StringView& value);

		friend std::wostream& operator <<(std::wostream& output, const StringView& value);

		friend std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& output, const StringView& value);

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, StringView s);

		////////////////////////////////////////////////////////////////
		//
		//	StringEquals
		//
		////////////////////////////////////////////////////////////////

		static bool StringEquals(const char32* s1, const char32* s2, size_t length) noexcept;

	private:

		[[noreturn]]
		static void ThrowAtOutOfRange();

		[[noreturn]]
		static void ThrowSubstrOutOfRange();

		[[noreturn]]
		static void ThrowChoiceOutOfRange();

		[[noreturn]]
		static void ThrowValuesAtOutOfRange();

		string_view_type m_view;
	};

	inline namespace Literals
	{
		inline namespace StringViewLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_sv
			//
			////////////////////////////////////////////////////////////////

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
