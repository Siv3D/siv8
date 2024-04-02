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
# include <string>
# include <algorithm>
# include <functional>
# include "Concepts.hpp"
# include "StringView.hpp"
# include "Random.hpp"
# include "Shuffle.hpp"

namespace s3d
{
	template <class Type, class Allocator>
	class Array;

	template <class Type>
	concept StringViewLike = (std::is_convertible_v<const Type&, std::u32string_view> && (not std::is_convertible_v<const Type&, const char32*>));

	////////////////////////////////////////////////////////////////
	//
	//	String
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列
	class String
	{
	public:
		
		using string_type				= std::u32string;
		using traits_type				= typename string_type::traits_type;
		using allocator_type			= typename string_type::allocator_type;
		using value_type				= typename string_type::value_type;
		using size_type					= typename string_type::size_type;
		using difference_type			= typename string_type::difference_type;
		using pointer					= typename string_type::pointer;
		using const_pointer				= typename string_type::const_pointer;
		using reference					= typename string_type::reference;
		using const_reference			= typename string_type::const_reference;
		using iterator					= typename string_type::iterator;
		using const_iterator			= typename string_type::const_iterator;
		using reverse_iterator			= typename string_type::reverse_iterator;
		using const_reverse_iterator	= typename string_type::const_reverse_iterator;

		static constexpr size_type npos{ static_cast<size_type>(-1) };

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ | default constructor
		[[nodiscard]]
		constexpr String() = default;

		/// @brief 文字列をコピーして新しい文字列を作成します。
		/// @param other コピーする文字列
		[[nodiscard]]
		constexpr String(const String& other) = default;

		[[nodiscard]]
		constexpr String(String&& other) noexcept = default;

		/// @brief 文字列をコピーして新しい文字列を作成します。
		/// @param other コピーする文字列
		[[nodiscard]]
		constexpr String(const string_type& other);

		[[nodiscard]]
		constexpr String(string_type&& other) noexcept;

		[[nodiscard]]
		constexpr String(const String& other, size_type pos);

		[[nodiscard]]
		constexpr String(const String& other, size_type pos, size_type count);

		[[nodiscard]]
		constexpr String(const value_type* s);

		[[nodiscard]]
		constexpr String(const value_type* s, size_type count);

		[[nodiscard]]
		constexpr String(size_type count, value_type ch);

		[[nodiscard]]
		constexpr String(std::initializer_list<value_type> list);

		template <class Iterator>
		[[nodiscard]]
		constexpr String(Iterator first, Iterator last);

		[[nodiscard]]
		explicit constexpr String(const StringViewLike auto& s);

		[[nodiscard]]
		constexpr String(const StringViewLike auto& s, size_type pos, size_type count);

	# ifdef __cpp_lib_containers_ranges
		
		template <class Range> requires Concept::ContainerCompatibleRange<String::value_type, Range>
		[[nodiscard]]
		constexpr String(std::from_range_t, Range&& range);
	
	# endif
		
		constexpr String(std::nullptr_t) = delete;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		constexpr String& operator =(const String& other);

		constexpr String& operator =(const string_type& other);

		constexpr String& operator =(String&& other) noexcept;

		constexpr String& operator =(string_type&& other) noexcept;

		constexpr String& operator =(const value_type* s);

		constexpr String& operator =(std::initializer_list<value_type> list);

		constexpr String& operator =(const StringViewLike auto& s);

		constexpr String& operator =(std::nullptr_t) = delete;

		////////////////////////////////////////////////////////////////
		//
		//	assign
		//
		////////////////////////////////////////////////////////////////

		constexpr String& assign(size_type count, value_type ch);

		constexpr String& assign(const String& s);

		constexpr String& assign(const string_type& s);

		constexpr String& assign(const String& s, size_type pos, size_type count = npos);

		constexpr String& assign(const string_type& s, size_type pos, size_type count = npos);

		constexpr String& assign(String&& s) noexcept;

		constexpr String& assign(string_type&& s) noexcept;

		constexpr String& assign(const value_type* s, size_type count);

		constexpr String& assign(const value_type* s);

		template <class Iterator>
		constexpr String& assign(Iterator first, Iterator last);

		constexpr String& assign(std::initializer_list<value_type> list);

		constexpr String& assign(const StringViewLike auto& s);

		constexpr String& assign(const StringViewLike auto& s, size_type pos, size_type count = npos);

		////////////////////////////////////////////////////////////////
		//
		//	assign_range
		//
		////////////////////////////////////////////////////////////////

		template <class Range> requires Concept::ContainerCompatibleRange<String::value_type, Range>
		constexpr String& assign_range(Range&& range);

		////////////////////////////////////////////////////////////////
		//
		//	get_allocator
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr allocator_type get_allocator() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	at
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr value_type& at(size_type index)&;

		[[nodiscard]]
		constexpr const value_type& at(size_type index) const&;

		[[nodiscard]]
		constexpr value_type at(size_type index)&&;

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr value_type& operator [](size_type index) & noexcept;

		[[nodiscard]]
		constexpr const value_type& operator [](size_type index) const& noexcept;

		[[nodiscard]]
		constexpr value_type operator [](size_type index) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の要素への参照を返します。
		/// @remark 空の文字列に対しては使えません。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		constexpr value_type& front() noexcept;

		/// @brief 先頭の要素への参照を返します。
		/// @remark 空の文字列に対しては使えません。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		constexpr const value_type& front() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の要素への参照を返します。
		/// @remark 空の文字列に対しては使えません。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		constexpr value_type& back() noexcept;

		/// @brief 末尾の要素への参照を返します。
		/// @remark 空の文字列に対しては使えません。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		constexpr const value_type& back() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	data
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr const value_type* data() const noexcept;

		[[nodiscard]]
		constexpr value_type* data() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	c_str
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr const value_type* c_str() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	str
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr string_type& str() noexcept;

		[[nodiscard]]
		constexpr const string_type& str() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator StringView
		//
		////////////////////////////////////////////////////////////////

		constexpr operator StringView() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	begin, end
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr iterator begin() noexcept;

		[[nodiscard]]
		constexpr iterator end() noexcept;

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
		constexpr reverse_iterator rbegin() noexcept;

		[[nodiscard]]
		constexpr reverse_iterator rend() noexcept;

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
		//	empty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列が空であるかを返します。
		/// @return 文字列が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool empty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列が空であるかを返します。
		/// @remark `empty()` と同じです。
		/// @return 文字列が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列が空でないかを返します。
		/// @return 文字列が空でない場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の長さ（要素数）を返します。
		/// @return 文字列の長さ（要素数）
		[[nodiscard]]
		constexpr size_type size() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	ssize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の長さ（要素数）を符号付き整数型で返します。 | Returns the length of the string as a signed integer type.
		/// @return 文字列の長さ（要素数）
		[[nodiscard]]
		constexpr isize ssize() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	size_bytes
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列のデータサイズ（バイト）を返します。
		/// @remark `sizeof(value_type) * length()` です。
		/// @return 文字列のデータサイズ（バイト）
		[[nodiscard]]
		constexpr size_type size_bytes() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	length
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の長さ（要素数）を返します。
		/// @return 文字列の長さ（要素数）
		/// @remark `.size()` と同じです。
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
		//	reserve
		//
		////////////////////////////////////////////////////////////////

		constexpr void reserve(size_type newCapacity);

		////////////////////////////////////////////////////////////////
		//
		//	capacity
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr size_type capacity() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	shrink_to_fit
		//
		////////////////////////////////////////////////////////////////

		constexpr void shrink_to_fit();

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 格納している文字列を消去し、空の文字列にします。
		constexpr void clear() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	release
		//
		////////////////////////////////////////////////////////////////

		constexpr void release();

		////////////////////////////////////////////////////////////////
		//
		//	insert
		//
		////////////////////////////////////////////////////////////////

		constexpr String& insert(size_type offset, size_type count, value_type ch);

		constexpr String& insert(size_type offset, const value_type* s);

		constexpr String& insert(size_type offset, const value_type* s, size_type count);

		constexpr String& insert(size_type offset, const String& s);

		constexpr String& insert(size_type offset, const String& s, size_type offset2, size_type count = npos);

		constexpr iterator insert(const_iterator pos, value_type ch);

		constexpr iterator insert(const_iterator pos, size_type count, value_type ch);

		template <class Iterator>
		constexpr iterator insert(const_iterator pos, Iterator first, Iterator last);

		constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> list);

		constexpr String& insert(size_type offset, const StringViewLike auto& s);

		constexpr String& insert(size_type offset, const StringViewLike auto& s, size_type offset2, size_type count = npos);

		////////////////////////////////////////////////////////////////
		//
		//	insert_range
		//
		////////////////////////////////////////////////////////////////

		template <class Range> requires Concept::ContainerCompatibleRange<String::value_type, Range>
		constexpr iterator insert_range(const_iterator pos, Range&& range);

		////////////////////////////////////////////////////////////////
		//
		//	erase
		//
		////////////////////////////////////////////////////////////////

		constexpr String& erase(size_type offset = 0, size_type count = npos);

		constexpr iterator erase(const_iterator pos) noexcept;

		constexpr iterator erase(const_iterator first, const_iterator last) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	push_front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭に文字を追加します。
		/// @param ch 追加する文字
		constexpr void push_front(value_type ch);

		////////////////////////////////////////////////////////////////
		//
		//	push_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾に文字を追加します。
		/// @param ch 追加する文字
		constexpr void push_back(value_type ch);

		////////////////////////////////////////////////////////////////
		//
		//	pop_front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の文字を削除します。
		constexpr void pop_front();

		////////////////////////////////////////////////////////////////
		//
		//	pop_front_N
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の n 文字を削除します。
		/// @remark n が現在の文字数より大きい場合は空の文字列にします。
		/// @param n 削除する文字数
		constexpr void pop_front_N(size_type n);

		////////////////////////////////////////////////////////////////
		//
		//	pop_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の文字を削除します。
		constexpr void pop_back() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pop_back_N
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の n 文字を削除します。
		/// @remark n が現在の文字数より大きい場合は空の文字列にします。
		/// @param n 削除する文字数
		constexpr void pop_back_N(size_type n) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字を末尾に追加します。
		/// @param ch 追加する文字
		/// @return *this
		constexpr String& operator <<(value_type ch);

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		constexpr String& append(value_type ch);

		constexpr String& append(size_type count, value_type ch);

		constexpr String& append(const String& s);

		constexpr String& append(const string_type& s);

		constexpr String& append(const String& s, size_type offset, size_type count = npos);

		constexpr String& append(const value_type* s, size_type count);

		constexpr String& append(const value_type* s);

		template <class Iterator>
		constexpr String& append(Iterator first, Iterator last);

		constexpr String& append(std::initializer_list<value_type> list);

		constexpr String& append(const StringViewLike auto& s);

		constexpr String& append(const StringViewLike auto& s, size_type pos, size_type count = npos);

		////////////////////////////////////////////////////////////////
		//
		//	append_range
		//
		////////////////////////////////////////////////////////////////

		template <class Range> requires Concept::ContainerCompatibleRange<String::value_type, Range>
		constexpr String& append_range(Range&& range);

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		constexpr String& operator +=(const String& s);

		constexpr String& operator +=(const string_type& s);

		constexpr String& operator +=(value_type ch);

		constexpr String& operator +=(const value_type* s);

		constexpr String& operator +=(std::initializer_list<value_type> list);

		constexpr String& operator +=(const StringViewLike auto& s);

		////////////////////////////////////////////////////////////////
		//
		//	replace
		//
		////////////////////////////////////////////////////////////////

		constexpr String& replace(size_type pos, size_type count, const String& s);

		constexpr String& replate(const_iterator first, const_iterator last, const String& s);
		
		constexpr String& replace(const_iterator first, const_iterator last, const value_type* s);

		constexpr String& replate(const_iterator first, const_iterator last, const StringViewLike auto& s);

		constexpr String& replace(size_type pos, size_type count, const value_type* s);

		template <class Iterator>
		constexpr String& replace(const_iterator first, const_iterator last, Iterator newFirst, Iterator newLast);

		constexpr String& replace(size_type pos, size_type count, const StringViewLike auto& s);

		////////////////////////////////////////////////////////////////
		//
		//	resize
		//
		////////////////////////////////////////////////////////////////

		constexpr void resize(size_type newSize);

		constexpr void resize(size_type newSize, value_type ch);

		////////////////////////////////////////////////////////////////
		//
		//	resize_and_overwrite
		//
		////////////////////////////////////////////////////////////////

		template <class Operation>
		constexpr void resize_and_overwrite(size_type count, Operation op);

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		constexpr void swap(String& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	find
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を指定した位置から検索し、最初に現れた位置を返します。
		/// @param s 検索する文字列
		/// @param offset 検索を開始する位置
		/// @return 検索した文字列が最初に現れた位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type find(const String& s, size_type offset = 0) const noexcept;

		/// @brief 文字列を指定した位置から検索し、最初に現れた位置を返します。
		/// @param s 検索する文字列
		/// @param offset 検索を開始する位置
		/// @return 検索した文字列が最初に現れた位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type find(const value_type* s, size_type offset = 0) const noexcept;

		/// @brief 文字列を指定した位置から検索し、最初に現れた位置を返します。
		/// @param s 検索する文字列
		/// @param offset 検索を開始する位置
		/// @return 検索した文字列が最初に現れた位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type find(const StringViewLike auto& s, size_type offset = 0) const noexcept;

		/// @brief 文字を指定した位置から検索し、最初に現れた位置を返します。
		/// @param ch 検索する文字
		/// @param offset 検索を開始する位置
		/// @return 検索した文字が最初に現れた位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type find(value_type ch, size_type offset = 0) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rfind
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を後方から逆順に検索し、最初に現れた位置を返します。
		/// @param s 検索する文字列
		/// @param offset 検索を開始する位置。npos の場合は終端から
		/// @return 検索した文字列が最初に現れた位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type rfind(const String& s, size_type offset = npos) const noexcept;

		/// @brief 文字列を後方から逆順に検索し、最初に現れた位置を返します。
		/// @param s 検索する文字列
		/// @param offset 検索を開始する位置。npos の場合は終端から
		/// @return 検索した文字列が最初に現れた位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type rfind(const value_type* s, size_type offset = npos) const noexcept;

		/// @brief 文字列を後方から逆順に検索し、最初に現れた位置を返します。
		/// @param s 検索する文字列
		/// @param offset 検索を開始する位置。npos の場合は終端から
		/// @return 検索した文字列が最初に現れた位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type rfind(const StringViewLike auto& s, size_type offset = npos) const noexcept;

		/// @brief 文字を後方から逆順に検索し、最初に現れた位置を返します。
		/// @param ch 検索する文字
		/// @param offset 検索を開始する位置。npos の場合は終端から
		/// @return 検索した文字が最初に現れた位置。見つからなかった場合は npos	
		[[nodiscard]]
		constexpr size_type rfind(value_type ch, size_type offset = npos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	find_first_of
		//
		////////////////////////////////////////////////////////////////

		/// @brief 検索する文字のいずれかが最初に現れた位置を返します。
		/// @param s 検索する文字の集合
		/// @param offset 検索を開始する位置
		/// @return 検索した文字が最初に現れた位置。見つからなかった場合は npos	
		[[nodiscard]]
		constexpr size_type find_first_of(const String& s, size_type offset = 0) const noexcept;

		/// @brief 検索する文字のいずれかが最初に現れた位置を返します。
		/// @param s 検索する文字の集合
		/// @param offset 検索を開始する位置
		/// @return 検索した文字が最初に現れた位置。見つからなかった場合は npos	
		[[nodiscard]]
		constexpr size_type find_first_of(const value_type* s, size_type offset = 0) const noexcept;

		/// @brief 検索する文字のいずれかが最初に現れた位置を返します。
		/// @param s 検索する文字の集合
		/// @param offset 検索を開始する位置
		/// @return 検索した文字が最初に現れた位置。見つからなかった場合は npos	
		[[nodiscard]]
		constexpr size_type find_first_of(const StringViewLike auto& s, size_type offset = 0) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	find_first_not_of
		//
		////////////////////////////////////////////////////////////////

		/// @brief 検索する文字に含まれない文字が最初に現れた位置を返します。
		/// @param s 検索する文字の集合
		/// @param offset 検索を開始する位置
		/// @return 検索した文字とは異なる文字が最初に現れた位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type find_first_not_of(const String& s, size_type offset = 0) const noexcept;

		/// @brief 検索する文字に含まれない文字が最初に現れた位置を返します。
		/// @param s 検索する文字の集合
		/// @param offset 検索を開始する位置
		/// @return 検索した文字とは異なる文字が最初に現れた位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type find_first_not_of(const value_type* s, size_type offset = 0) const noexcept;

		/// @brief 検索する文字に含まれない文字が最初に現れた位置を返します。
		/// @param s 検索する文字の集合
		/// @param offset 検索を開始する位置
		/// @return 検索した文字とは異なる文字が最初に現れた位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type find_first_not_of(const StringViewLike auto& s, size_type offset = 0) const noexcept;

		/// @brief 検索する文字と異なる文字が最初に現れた位置を返します。
		/// @param ch 検索する文字
		/// @param offset 検索を開始する位置
		/// @return 検索した文字とは異なる文字が最初に現れた位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type find_first_not_of(const value_type ch, size_type offset = 0) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	find_last_of
		//
		////////////////////////////////////////////////////////////////

		/// @brief 検索する文字のいずれかが最後に現れる位置を返します。
		/// @param s 検索する文字の集合
		/// @param offset 検索を終える位置
		/// @return 検索した文字が最後に現れる位置。見つからなかった場合は npos	
		[[nodiscard]]
		constexpr size_type find_last_of(const String& s, size_type offset = npos) const noexcept;

		/// @brief 検索する文字のいずれかが最後に現れる位置を返します。
		/// @param s 検索する文字の集合
		/// @param offset 検索を終える位置
		/// @return 検索した文字が最後に現れる位置。見つからなかった場合は npos	
		[[nodiscard]]
		constexpr size_type find_last_of(const value_type* s, size_type offset = npos) const noexcept;

		/// @brief 検索する文字のいずれかが最後に現れる位置を返します。
		/// @param s 検索する文字の集合
		/// @param offset 検索を終える位置
		/// @return 検索した文字が最後に現れる位置。見つからなかった場合は npos	
		[[nodiscard]]
		constexpr size_type find_last_of(const StringViewLike auto& s, size_type offset = npos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	find_last_not_of
		//
		////////////////////////////////////////////////////////////////

		/// @brief 検索する文字に含まれない文字が最後に現れる位置を返します。
		/// @param s 検索する文字の集合
		/// @param offset 検索を終える位置。npos の場合は終端から
		/// @return 検索する文字に含まれない文字が最後に現れる位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type find_last_not_of(const String& s, size_type offset = npos) const noexcept;

		/// @brief 検索する文字に含まれない文字が最後に現れる位置を返します。
		/// @param s 検索する文字の集合
		/// @param offset 検索を終える位置。npos の場合は終端から
		/// @return 検索する文字に含まれない文字が最後に現れる位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type find_last_not_of(const value_type* s, size_type offset = npos) const noexcept;

		/// @brief 検索する文字に含まれない文字が最後に現れる位置を返します。
		/// @param s 検索する文字の集合
		/// @param offset 検索を終える位置。npos の場合は終端から
		/// @return 検索する文字に含まれない文字が最後に現れる位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type find_last_not_of(const StringViewLike auto& s, size_type offset = npos) const noexcept;

		/// @brief 検索する文字ではない文字が最後に現れる位置を返します。
		/// @param s 検索する文字
		/// @param offset 検索を終える位置。npos の場合は終端から
		/// @return 検索する文字でない文字が最後に現れる位置。見つからなかった場合は npos
		[[nodiscard]]
		constexpr size_type find_last_not_of(const value_type ch, size_type offset = npos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	compare
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の大小を比較します。
		/// @param s 比較対象の文字列
		/// @return 比較結果。等しければ 0, 小さければ &lt;0, 大きければ &gt;0
		[[nodiscard]]
		constexpr int32 compare(const String& s) const noexcept;

		/// @brief 文字列の大小を比較します。
		/// @param s 比較対象の文字列
		/// @remark s は NULL 終端されている必要があります。
		/// @return 比較結果。等しければ 0, 小さければ &lt;0, 大きければ &gt;0
		[[nodiscard]]
		constexpr int32 compare(const value_type* s) const noexcept;

		/// @brief 文字列の大小を比較します。
		/// @param s 比較対象の文字列
		/// @return 比較結果。等しければ 0, 小さければ &lt;0, 大きければ &gt;0
		[[nodiscard]]
		constexpr int32 compare(const StringViewLike auto& s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	starts_with
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列が指定した文字から始まるかを返します。
		/// @param ch 検索する文字
		/// @return 指定した文字から始まる場合 true, それ以外の場合は false	
		[[nodiscard]]
		constexpr bool starts_with(value_type ch) const noexcept;

		/// @brief 文字列が指定した文字列から始まるかを返します。
		/// @param s 検索する文字列
		/// @return 指定した文字列から始まる場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool starts_with(StringView s) const noexcept;

		/// @brief 文字列が指定した文字列から始まるかを返します。
		/// @param s 検索する文字列
		/// @remark s は NULL 終端されている必要があります。
		/// @return 指定した文字列から始まる場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool starts_with(const value_type* s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	ends_with
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列が指定した文字で終わるかを返します。
		/// @param ch 検索する文字
		/// @return 指定した文字で終わる場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool ends_with(value_type ch) const noexcept;

		/// @brief 文字列が指定した文字列で終わるかを返します。
		/// @param s 検索する文字列
		/// @return 指定した文字列で終わる場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool ends_with(StringView s) const noexcept;

		/// @brief 文字列が指定した文字列で終わるかを返します。
		/// @param s 検索する文字列
		/// @remark s は NULL 終端されている必要があります。
		/// @return 指定した文字列で終わる場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool ends_with(const value_type* s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	contains
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しい要素があるかを返します。
		/// @param ch 検索する値
		/// @return 指定した値と等しい要素がある場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool contains(value_type ch) const noexcept;

		/// @brief 指定した文字列を含むかを返します。
		/// @param s 検索する文字列
		/// @return 指定した文字列を含む場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool contains(StringView s) const noexcept;

		/// @brief 指定した文字列を含むかを返します。
		/// @param s 検索する文字列
		/// @remark s は NULL 終端されている必要があります。
		/// @return 指定した文字列を含む場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool contains(const value_type* s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	substr
		//
		////////////////////////////////////////////////////////////////

		/// @brief 部分文字列を作成します。
		/// @param offset 開始インデックス
		/// @param count 部分文字列の長さ。末尾までの場合 npos
		/// @return 部分文字列
		[[nodiscard]]
		constexpr String substr(size_type offset = 0, size_type count = npos) const&;

		/// @brief 部分文字列を作成します。
		/// @param offset 開始インデックス
		/// @param count 部分文字列の長さ。末尾までの場合 npos
		/// @return 部分文字列
		[[nodiscard]]
		constexpr String substr(size_type offset = 0, size_type count = npos) &&;

		////////////////////////////////////////////////////////////////
		//
		//	substrView
		//
		////////////////////////////////////////////////////////////////

		/// @brief 部分文字列へのビューを取得します。
		/// @param offset 開始インデックス
		/// @param count 取得する文字数。末尾までの場合 npos
		/// @return 部分文字列へのビュー
		[[nodiscard]]
		constexpr StringView substrView(size_type offset = 0, size_type count = npos) const&;

		[[nodiscard]]
		constexpr StringView substrView(size_type offset = 0, size_type count = npos)&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列のハッシュを返します。
		/// @return 文字列のハッシュ
		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を UTF-8 文字列に変換します。
		/// @return 変換された文字列
		[[nodiscard]]
		std::string toUTF8() const;

		////////////////////////////////////////////////////////////////
		//
		//	toUTF16
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を UTF-16 文字列に変換します。
		/// @return 変換された文字列
		[[nodiscard]]
		std::u16string toUTF16() const;

		////////////////////////////////////////////////////////////////
		//
		//	toWstr
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を std::wstring に変換します。
		/// @return 変換された文字列
		[[nodiscard]]
		std::wstring toWstr() const;

		////////////////////////////////////////////////////////////////
		//
		//	toUTF32
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を UTF-32 文字列に変換します。
		/// @return 変換された文字列
		[[nodiscard]]
		constexpr const std::u32string& toUTF32() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	all
		//
		////////////////////////////////////////////////////////////////

		/// @brief 全ての要素が条件を満たすかを返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 全ての要素が条件を満たすか、配列が空の場合 true, それ以外の場合は false
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
		//	capitalize, capitalized
		//
		////////////////////////////////////////////////////////////////

		/// @brief 最初に登場する英字を大文字にします。
		/// @return *this
		String& capitalize()& noexcept;

		/// @brief 最初に登場する英字を大文字にした文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String capitalize()&& noexcept;

		/// @brief 最初に登場する英字を大文字にした文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String capitalized() const&;

		/// @brief 最初に登場する英字を大文字にした文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String capitalized()&& noexcept;

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

		/// @brief 全ての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (auto& ch : s) f(ch);` と同じです。
		template <class Fty>
		constexpr void each(Fty f) requires std::invocable<Fty&, value_type&>;

		/// @brief 全ての要素を順番に引数にして関数を呼び出します。
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

		/// @brief 全ての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (size_t i = 0; auto& ch : s) f(i++, ch);` と同じです。
		template <class Fty>
		constexpr void each_index(Fty f) requires std::invocable<Fty&, size_t, value_type&>;

		/// @brief 全ての要素とそのインデックスを順番に引数にして関数を呼び出します。
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

		/// @brief 全ての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (isize i = 0; auto& ch : s) f(i++, ch);` と同じです。
		template <class Fty>
		constexpr void each_sindex(Fty f) requires std::invocable<Fty&, isize, value_type&>;

		/// @brief 全ての要素とそのインデックスを順番に引数にして関数を呼び出します。
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
		/// @return  指定したインデックスにある要素、範囲外の場合 defaultValue	
		[[nodiscard]]
		constexpr value_type fetch(size_type index, value_type defaultValue) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	fill
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値を全ての要素に代入します。
		/// @param value 代入する値
		/// @return *this
		constexpr void fill(value_type value);

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
		//	keep_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素のみを残して、それ以外を文字列から削除します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return *this
		template <class Fty>
		constexpr String& keep_if(Fty f) requires std::predicate<Fty&, const value_type&>;

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
		//	leftPad, leftPadded
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の左を埋め文字で埋めます。
		/// @param length 文字列の左を埋め文字で埋めた後の文字列の長さ
		/// @param fillChar 埋め文字
		/// @remark 元の文字列の長さが `length` より大きい場合、変更は行われません。 
		/// @return *this
		String& leftPad(size_type length, value_type fillChar = U' ') & noexcept;

		/// @brief 文字列の左を埋め文字で埋めた新しい文字列を返します。
		/// @param length 文字列の左を埋め文字で埋めた後の文字列の長さ
		/// @param fillChar 埋め文字
		/// @remark 元の文字列の長さが `length` より大きい場合、元の文字列のコピーが返ります。
		/// @return 新しい文字列
		[[nodiscard]]
		String leftPad(size_type length, value_type fillChar = U' ') && noexcept;

		/// @brief 文字列の左を埋め文字で埋めた新しい文字列を返します。
		/// @param length 文字列の左を埋め文字で埋めた後の文字列の長さ
		/// @param fillChar 埋め文字
		/// @remark 元の文字列の長さが `length` より大きい場合、元の文字列のコピーが返ります。
		/// @return 新しい文字列
		[[nodiscard]]
		String leftPadded(size_type length, value_type fillChar = U' ') const&;

		/// @brief 文字列の左を埋め文字で埋めた新しい文字列を返します。
		/// @param length 文字列の左を埋め文字で埋めた後の文字列の長さ
		/// @param fillChar 埋め文字
		/// @remark 元の文字列の長さが `length` より大きい場合、元の文字列のコピーが返ります。
		/// @return 新しい文字列
		[[nodiscard]]
		String leftPadded(size_type length, value_type fillChar = U' ') && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	levenshteinDistanceFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の文字列とのレーベンシュタイン距離を計算して返します。
		/// @param other 別の文字列
		/// @return 別の文字列とのレーベンシュタイン距離
		[[nodiscard]]
		size_type levenshteinDistanceFrom(StringView other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lowercase, lowercased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 英字をすべて小文字にします。
		/// @return *this
		String& lowercase()& noexcept;

		/// @brief 英字をすべて小文字にした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String lowercase()&& noexcept;

		/// @brief 英字をすべて小文字にした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String lowercased() const&;

		/// @brief 英字をすべて小文字にした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String lowercased()&& noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	leftTrim, leftTrimmed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の先頭に付いている空白文字を削除します。
		/// @return *this
		String& leftTrim() & noexcept;

		/// @brief 文字列の先頭に付いている空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String leftTrim() && noexcept;

		/// @brief 文字列の先頭に付いている空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String leftTrimmed() const&;

		/// @brief 文字列の先頭に付いている空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String leftTrimmed() && noexcept;

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
		//	remove, removed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字を文字列から削除します。
		/// @param ch 削除する文字
		/// @return *this
		String& remove(value_type ch) & noexcept;

		/// @brief 指定した文字列をもとの文字列から削除します。
		/// @param s 削除する文字列
		/// @return *this
		String& remove(StringView s) & noexcept;

		/// @brief 指定した文字を文字列から削除した新しい文字列を返します。
		/// @param ch 削除する文字
		/// @return 新しい文字列
		[[nodiscard]]
		String remove(value_type ch) && noexcept;

		/// @brief 指定した文字列を削除した新しい文字列を返します。
		/// @param s 削除する文字列
		/// @return 新しい文字列
		[[nodiscard]]
		String remove(StringView s) && noexcept;

		/// @brief 指定した文字を文字列から削除した新しい文字列を返します。
		/// @param ch 削除する文字
		/// @return 新しい文字列
		[[nodiscard]]
		String removed(value_type ch) const&;

		/// @brief 指定した文字列を削除した新しい文字列を返します。
		/// @param s 削除する文字列
		/// @return 新しい文字列
		[[nodiscard]]
		String removed(StringView s) const&;

		/// @brief 指定した文字を文字列から削除した新しい文字列を返します。
		/// @param ch 削除する文字
		/// @return 新しい文字列
		[[nodiscard]]
		String removed(value_type ch) && noexcept;

		/// @brief 指定した文字列を削除した新しい文字列を返します。
		/// @param s 削除する文字列
		/// @return 新しい文字列
		[[nodiscard]]
		String removed(StringView s) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	remove_at, removed_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにある要素を文字列から削除します。
		/// @param index インデックス
		/// @remark 範囲外のインデックスを指定した場合、何もしません。
		/// @return *this
		String& remove_at(size_type index) & noexcept;

		/// @brief 指定したインデックスにある要素を文字列から削除した新しい文字列を返します。
		/// @param index インデックス
		/// @remark 範囲外のインデックスを指定した場合、何も削除しません。
		/// @return 新しい文字列
		[[nodiscard]]
		String remove_at(size_type index) && noexcept;

		/// @brief 指定したインデックスにある要素を文字列から削除した新しい文字列を返します。
		/// @param index インデックス
		/// @remark 範囲外のインデックスを指定した場合、何も削除しません。
		/// @return 新しい文字列
		[[nodiscard]]
		String removed_at(size_type index) const&;

		/// @brief 指定したインデックスにある要素を文字列から削除した新しい文字列を返します。
		/// @param index インデックス
		/// @remark 範囲外のインデックスを指定した場合、何も削除しません。
		/// @return 新しい文字列
		[[nodiscard]]
		String removed_at(size_type index) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	remove_if, removed_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素を削除します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return *this
		template <class Fty>
		constexpr String& remove_if(Fty f) & noexcept requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす要素を削除した新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return　新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String remove_if(Fty f) && noexcept requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす要素を削除した新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return　新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String removed_if(Fty f) const& requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす要素を削除した新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return　新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String removed_if(Fty f) && noexcept requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	replace, replaced
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字を置換します。
		/// @param oldChar 置換対象の文字
		/// @param newChar 置換後の文字
		/// @return *this
		String& replace(value_type oldChar, value_type newChar)& noexcept;

		/// @brief 指定した文字を置換した新しい文字列を返します。
		/// @param oldChar 置換対象の文字
		/// @param newChar 置換後の文字
		/// @return *this
		String replace(value_type oldChar, value_type newChar)&& noexcept;

		/// @brief 指定した文字を置換した新しい文字列を返します。
		/// @param oldChar 置換対象の文字
		/// @param newChar 置換後の文字
		/// @return 新しい文字列
		[[nodiscard]]
		String replaced(value_type oldChar, value_type newChar) const&;

		/// @brief 指定した文字を置換した新しい文字列を返します。
		/// @param oldChar 置換対象の文字
		/// @param newChar 置換後の文字
		/// @return 新しい文字列
		[[nodiscard]]
		String replaced(value_type oldChar, value_type newChar)&& noexcept;

		/// @brief 指定した文字列を置換します。
		/// @param oldStr 置換対象の文字列
		/// @param newStr 置換後の文字列
		/// @return *this
		String& replace(StringView oldStr, StringView newStr)&;

		/// @brief 指定した文字列を置換した新しい文字列を返します。
		/// @param oldStr 置換対象の文字列
		/// @param newStr 置換後の文字列
		/// @return 新しい文字列
		[[nodiscard]]
		String replace(StringView oldStr, StringView newStr) &&;

		/// @brief 指定した文字列を置換した新しい文字列を返します。
		/// @param oldStr 置換対象の文字列
		/// @param newStr 置換後の文字列
		/// @return 新しい文字列
		[[nodiscard]]
		String replaced(StringView oldStr, StringView newStr) const&;

		////////////////////////////////////////////////////////////////
		//
		//	replace_if, replaced_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素を別の文字に置き換えます。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @param newChar 置換後の文字
		/// @return *this
		template <class Fty>
		constexpr String& replace_if(Fty f, value_type newChar) & noexcept requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす要素を別の文字に置き換えた新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @param newChar 置換後の文字
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String replace_if(Fty f, value_type newChar) && noexcept requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす要素を別の文字に置き換えた新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @param newChar 置換後の文字
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String replaced_if(Fty f, value_type newChar) const& requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす要素を別の文字に置き換えた新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @param newChar 置換後の文字
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String replaced_if(Fty f, value_type newChar) && noexcept requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	reverse, reversed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を反転します。
		/// @return *this
		String& reverse() & noexcept;

		/// @brief 文字列を反転した文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String reverse() && noexcept;

		/// @brief 文字列を反転した文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String reversed() const&;

		/// @brief 文字列を反転した文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String reversed() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reverse_each
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各要素を引数にして関数を呼び出します。順番は後ろからです。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		constexpr void reverse_each(Fty f) requires std::invocable<Fty&, value_type&>;

		/// @brief 各要素を引数にして関数を呼び出します。順番は後ろからです。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		constexpr void reverse_each(Fty f) const requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	rightPad, rightPadded
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の右を埋め文字で埋めます。
		/// @param length 文字列の右を埋め文字で埋めた後の文字列の長さ
		/// @param fillChar 埋め文字
		/// @remark 元の文字列の長さが `length` より大きい場合、変更は行われません。 
		/// @return *this
		String& rightPad(size_type length, value_type fillChar = U' ') & noexcept;

		/// @brief 文字列の右を埋め文字で埋めた新しい文字列を返します。
		/// @param length 文字列の右を埋め文字で埋めた後の文字列の長さ
		/// @param fillChar 埋め文字
		/// @remark 元の文字列の長さが `length` より大きい場合、元の文字列のコピーが返ります。
		/// @return 新しい文字列
		[[nodiscard]]
		String rightPad(size_type length, value_type fillChar = U' ') && noexcept;

		/// @brief 文字列の右を埋め文字で埋めた新しい文字列を返します。
		/// @param length 文字列の右を埋め文字で埋めた後の文字列の長さ
		/// @param fillChar 埋め文字
		/// @remark 元の文字列の長さが `length` より大きい場合、元の文字列のコピーが返ります。
		/// @return 新しい文字列
		[[nodiscard]]
		String rightPadded(size_type length, value_type fillChar = U' ') const&;

		/// @brief 文字列の右を埋め文字で埋めた新しい文字列を返します。
		/// @param length 文字列の右を埋め文字で埋めた後の文字列の長さ
		/// @param fillChar 埋め文字
		/// @remark 元の文字列の長さが `length` より大きい場合、元の文字列のコピーが返ります。
		/// @return 新しい文字列
		[[nodiscard]]
		String rightPadded(size_type length, value_type fillChar = U' ') && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rightTrim, rightTrimmed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の末尾に付いている空白文字を削除します。
		/// @return *this
		String& rightTrim() & noexcept;

		/// @brief 文字列の末尾に付いている空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String rightTrim() && noexcept;

		/// @brief 文字列の末尾に付いている空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String rightTrimmed() const&;

		/// @brief 文字列の末尾に付いている空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String rightTrimmed() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotate, rotated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を中心に前半と後半を入れ替えます。
		/// @param index 入れ替える位置
		/// @remark index が文字列の長さ以上の場合、入れ替えは行われません。
		/// @remark .rotate(2) の場合、"abcd" は "cdab" になります。
		/// @return *this
		String& rotate(size_type index) & noexcept;

		/// @brief 指定した位置を中心に前半と後半を入れ替えた新しい文字列を返します。
		/// @param index 入れ替える位置
		/// @remark index が文字列の長さ以上の場合、入れ替えは行われません。
		/// @return 新しい文字列
		[[nodiscard]]
		String rotate(size_type index) && noexcept;

		/// @brief 指定した位置を中心に前半と後半を入れ替えた新しい文字列を返します。
		/// @param index 入れ替える位置
		/// @remark index が文字列の長さ以上の場合、入れ替えは行われません。
		/// @return 新しい文字列
		[[nodiscard]]
		String rotated(size_type index) const&;

		/// @brief 指定した位置を中心に前半と後半を入れ替えた新しい文字列を返します。
		/// @param index 入れ替える位置
		/// @remark index が文字列の長さ以上の場合、入れ替えは行われません。
		/// @return 新しい文字列
		[[nodiscard]]
		String rotated(size_type index) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rsort, rsorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を降順にソートします。
		/// @return *this
		String& rsort() & noexcept;

		/// @brief 要素を降順にソートした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String rsort() && noexcept;

		/// @brief 要素を降順にソートした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String rsorted() const&;

		/// @brief 要素を降順にソートした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String rsorted() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	shuffle, shuffled
		//
		////////////////////////////////////////////////////////////////
	
		/// @brief 要素をシャッフルします。
		/// @return *this
		String& shuffle() & noexcept;

		/// @brief 要素をシャッフルした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String shuffle() && noexcept;

		/// @brief 要素をシャッフルした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String shuffled() const&;

		/// @brief 要素をシャッフルした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String shuffled() && noexcept;

		/// @brief 指定された乱数生成器を使って要素をシャッフルします。
		/// @param rbg 乱数生成器
		/// @return *this
		String& shuffle(Concept::UniformRandomBitGenerator auto&& rbg) & noexcept;

		/// @brief 指定された乱数生成器を使って要素をシャッフルした新しい文字列を返します。
		/// @param rbg 乱数生成器
		/// @return 新しい文字列
		[[nodiscard]]
		String shuffle(Concept::UniformRandomBitGenerator auto&& rbg) && noexcept;

		/// @brief 指定された乱数生成器を使って要素をシャッフルした新しい文字列を返します。
		/// @param rbg 乱数生成器
		/// @return 新しい文字列
		[[nodiscard]]
		String shuffled(Concept::UniformRandomBitGenerator auto&& rbg) const&;

		/// @brief 指定された乱数生成器を使って要素をシャッフルした新しい文字列を返します。
		/// @param rbg 乱数生成器
		/// @return 新しい文字列
		[[nodiscard]]
		String shuffled(Concept::UniformRandomBitGenerator auto&& rbg) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	sort, sorted
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief 要素を昇順にソートします。
		/// @return *this
		String& sort() & noexcept;

		/// @brief 要素を昇順にソートした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String sort() && noexcept;

		/// @brief 要素を昇順にソートした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String sorted() const&;

		/// @brief 要素を昇順にソートした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String sorted() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	sort_by, sorted_by
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief 指定した比較関数を用いて要素を昇順にソートします。
		/// @tparam Fty 比較関数の型
		/// @param f 比較関数
		/// @return *this
		template <class Fty>
		String& sort_by(Fty f) & noexcept requires std::predicate<Fty&, const value_type&, const value_type&>;
		
		/// @brief 指定した比較関数を用いて要素を昇順にソートした新しい文字列を返します。
		/// @tparam Fty 比較関数の型
		/// @param f 比較関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		String sort_by(Fty f) && noexcept requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した比較関数を用いて要素を昇順にソートした新しい文字列を返します。
		/// @tparam Fty 比較関数の型
		/// @param f 比較関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		String sorted_by(Fty f) const& requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した比較関数を用いて要素を昇順にソートした新しい文字列を返します。
		/// @tparam Fty 比較関数の型
		/// @param f 比較関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		String sorted_by(Fty f) && noexcept requires std::predicate<Fty&, const value_type&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	split
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した区切り文字で文字列を分割します。
		/// @param ch 区切り文字
		/// @return 分割された文字列
		[[nodiscard]]
		Array<String, std::allocator<String>> split(value_type ch) const;

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
		std::pair<String, String> split_at(size_type pos) const;

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
		//	swapcase, swapcased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 英字の大文字と小文字を入れ替えます。
		/// @return *this
		String& swapcase() & noexcept;

		/// @brief 英字の大文字と小文字を入れ替えた文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String swapcase() && noexcept;

		/// @brief 英字の大文字と小文字を入れ替えた文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String swapcased() const&;

		/// @brief 英字の大文字と小文字を入れ替えた文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String swapcased() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	take
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief 文字列の先頭から指定された文字数分取り出した新しい文字列を返します。
		/// @param n 取り出す文字数
		/// @return 新しい文字列
		/// @remark n が文字列の長さ以上の場合、元の文字列のコピーが返ります。
		/// @remark この関数は `.substr(0, n)` と同じです。
		[[nodiscard]]
		String take(size_t n) const&;

		/// @brief 文字列の先頭から指定された文字数分取り出した新しい文字列を返します。
		/// @param n 取り出す文字数
		/// @return 新しい文字列
		/// @remark n が文字列の長さ以上の場合、元の文字列のコピーが返ります。
		/// @remark この関数は `.substr(0, n)` と同じです。
		[[nodiscard]]
		String take(size_t n) && noexcept;
		
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
		String take_while(Fty f) const& requires std::predicate<Fty&, const value_type&>;

		/// @brief 先頭から指定された条件を満たし続ける間、要素を取り出します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		String take_while(Fty f) && noexcept requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	trim, trimmed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の先頭と末尾に付いている空白文字を削除します。
		/// @return *this
		String& trim()& noexcept;

		/// @brief 文字列の先頭と末尾に付いている空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String trim()&& noexcept;

		/// @brief 文字列の先頭と末尾に付いている空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String trimmed() const&;

		/// @brief 文字列の先頭と末尾に付いている空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String trimmed()&& noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	uppercase, uppercased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 英字をすべて大文字にします。
		/// @return *this
		String& uppercase() & noexcept;

		/// @brief 英字をすべて大文字にした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String uppercase() && noexcept;

		/// @brief 英字をすべて大文字にした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String uppercased() const&;

		/// @brief 英字をすべて大文字にした新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String uppercased() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	stable_unique, stable_uniqued
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief 文字列をソートせずに、重複する文字を削除します。
		/// @return *this
		String& stable_unique() & noexcept;

		/// @brief 文字列をソートせずに、重複する文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String stable_unique() &&;

		/// @brief 文字列をソートせずに、重複する文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String stable_uniqued() const;

		////////////////////////////////////////////////////////////////
		//
		//	sort_and_unique, sorted_and_uniqued
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列をソートしたあとに重複する文字を削除します。
		/// @return *this
		String& sort_and_unique() & noexcept;

		/// @brief 文字列をソートしたあとに重複する文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String sort_and_unique() && noexcept;

		/// @brief 文字列をソートしたあとに重複する文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String sorted_and_uniqued() const&;
		
		/// @brief 文字列をソートしたあとに重複する文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String sorted_and_uniqued() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	unique_consecutive, uniqued_consecutive
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ文字が連続する場合、その先頭以外を除去します。
		/// @return *this
		String& unique_consecutive() & noexcept;

		/// @brief 同じ文字が連続する場合、その先頭以外を除去した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String unique_consecutive() && noexcept;

		/// @brief 同じ文字が連続する場合、その先頭以外を除去した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String uniqued_consecutive() const&;

		/// @brief 同じ文字が連続する場合、その先頭以外を除去した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String uniqued_consecutive() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	values_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの文字からなる新しい文字列を返します。
		/// @param indices インデックス
		/// @remark `String(U"abcde").values_at({0, 3, 4}); // U"ade"`
		/// @return 新しい文字列
		[[nodiscard]]
		String values_at(std::initializer_list<size_type> indices) const;

		////////////////////////////////////////////////////////////////
		//
		//	xml_escape, xml_escaped
		//
		////////////////////////////////////////////////////////////////

		/// @brief XML エスケープを行います。
		/// @remark &quot;, \, &amp;, &gt;, &lt; のエスケープを行います
		/// @return *this
		String& xml_escape();

		/// @brief  XML エスケープした文字列を返します。
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
		friend constexpr bool operator ==(const String& lhs, const String& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				return (lhs.m_string == rhs.m_string);
			}
			else
			{
				const size_t length = lhs.m_string.size();

				if (length != rhs.m_string.size())
				{
					return false;
				}

				return StringView::StringEquals(lhs.m_string.data(), rhs.m_string.data(), length);
			}
		}

		[[nodiscard]]
		friend constexpr auto operator <=>(const String&, const String&) noexcept = default;

		[[nodiscard]]
		friend constexpr bool operator ==(const String& lhs, const value_type* rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				return (lhs.m_string == rhs);
			}
			else
			{
				const size_t length = lhs.m_string.size();

				if (length != std::char_traits<value_type>::length(rhs))
				{
					return false;
				}

				return StringView::StringEquals(lhs.m_string.data(), rhs, length);
			}
		}

		[[nodiscard]]
		friend constexpr auto operator <=>(const String& lhs, const value_type* rhs) noexcept
		{
			return (lhs.m_string <=> rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		friend constexpr void swap(String& lhs, String& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr String operator +(const String& lhs, const String& rhs)
		{
			return String(lhs.m_string + rhs.m_string);
		}

		[[nodiscard]]
		friend constexpr String operator +(const String& lhs, const value_type* rhs)
		{
			return String(lhs.m_string + rhs);
		}

		[[nodiscard]]
		friend constexpr String operator +(const String& lhs, const value_type rhs)
		{
			return String(lhs.m_string + rhs);
		}

		[[nodiscard]]
		friend constexpr String operator +(const value_type* lhs, const String& rhs)
		{
			return String(lhs + rhs.m_string);
		}

		[[nodiscard]]
		friend constexpr String operator +(const value_type lhs, const String& rhs)
		{
			return String(lhs + rhs.m_string);
		}

		[[nodiscard]]
		friend constexpr String operator +(String&& lhs, String&& rhs)
		{
			return String(std::move(lhs.m_string) + std::move(rhs.m_string));
		}

		[[nodiscard]]
		friend constexpr String operator +(String&& lhs, const String& rhs)
		{
			return String(std::move(lhs.m_string) + rhs.m_string);
		}

		[[nodiscard]]
		friend constexpr String operator +(String&& lhs, const value_type* rhs)
		{
			return String(std::move(lhs.m_string) + rhs);
		}

		[[nodiscard]]
		friend constexpr String operator +(String&& lhs, const value_type rhs)
		{
			return String(std::move(lhs.m_string) + rhs);
		}

		[[nodiscard]]
		friend constexpr String operator +(const String& lhs, String&& rhs)
		{
			return String(lhs.m_string + std::move(rhs.m_string));
		}

		[[nodiscard]]
		friend constexpr String operator +(const value_type* lhs, String&& rhs)
		{
			return String(lhs + std::move(rhs.m_string));
		}

		[[nodiscard]]
		friend constexpr String operator +(const value_type lhs, String&& rhs)
		{
			return String(lhs + std::move(rhs.m_string));
		}

		[[nodiscard]]
		friend constexpr String operator +(const String& lhs, const StringView rhs)
		{
			String s;
			s.reserve(lhs.size() + rhs.size());
			s.append(lhs).append(rhs);
			return s;
		}

		[[nodiscard]]
		friend constexpr String operator +(const StringView lhs, const String& rhs)
		{
			String s;
			s.reserve(lhs.size() + rhs.size());
			s.append(lhs).append(rhs);
			return s;
		}

		[[nodiscard]]
		friend constexpr String operator +(String&& lhs, const StringView rhs)
		{
			lhs.append(rhs);
			return std::move(lhs);
		}

		[[nodiscard]]
		friend constexpr String operator +(const StringView lhs, String&& rhs)
		{
			rhs.insert(0, lhs);
			return std::move(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		friend std::ostream& operator <<(std::ostream& os, const String& value);

		friend std::wostream& operator <<(std::wostream& os, const String& value);

		friend std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const String& value);

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		friend std::istream& operator >>(std::istream& input, String& value);

		friend std::wistream& operator >>(std::wistream& input, String& value);

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const String& s);

	private:

		string_type m_string;

		[[noreturn]]
		static void ThrowAtOutOfRange();

		[[noreturn]]
		static void ThrowValuesAtOutOfRange();
	};

	inline namespace Literals
	{
		inline namespace StringLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_s
			//
			////////////////////////////////////////////////////////////////

			[[nodiscard]]
			inline String operator ""_s(const char32_t* s, size_t length);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	FilePath
	//
	////////////////////////////////////////////////////////////////

	using FilePath = String;
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::String>
{
	[[nodiscard]]
	size_t operator ()(const s3d::String& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/String.ipp"
