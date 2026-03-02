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
# include <functional>
# include <ranges>
# include <span>
# include <string>
# include <version>
# include "Concepts.hpp"
# include "RangeConcepts.hpp"
# include "StringView.hpp"

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

		/// @brief 文字列をムーブして新しい文字列を作成します。
		/// @param other ムーブする文字列
		[[nodiscard]]
		constexpr String(String&& other) noexcept = default;

		/// @brief 文字列をコピーして新しい文字列を作成します。
		/// @param other コピーする文字列
		[[nodiscard]]
		constexpr String(const string_type& other);

		/// @brief 文字列をムーブして新しい文字列を作成します。
		/// @param other ムーブする文字列
		[[nodiscard]]
		constexpr String(string_type&& other) noexcept;

		/// @brief 文字列の指定位置以降をコピーして新しい文字列を作成します。
		/// @param other 対象の文字列
		/// @param pos コピーを開始する位置
		[[nodiscard]]
		constexpr String(const String& other, size_type pos);

		/// @brief 文字列の指定位置から指定文字数をコピーして新しい文字列を作成します。
		/// @param other 対象の文字列
		/// @param pos コピーを開始する位置
		/// @param count コピーする文字数
		[[nodiscard]]
		constexpr String(const String& other, size_type pos, size_type count);

		/// @brief 文字列をコピーして新しい文字列を作成します。
		/// @param s コピーする文字列
		[[nodiscard]]
		constexpr String(const value_type* s);

		/// @brief 文字列の指定位置以降をコピーして新しい文字列を作成します。
		/// @param s 対象の文字列
		/// @param count コピーする文字数
		[[nodiscard]]
		constexpr String(const value_type* s, size_type count);

		/// @brief 指定した個数の同じ文字が連続した文字列を作成します。
		/// @param count 繰り返す回数
		/// @param ch 繰り返す文字
		[[nodiscard]]
		constexpr String(size_type count, value_type ch);

		/// @brief 文字のリストから新しい文字列を作成します。
		/// @param list 文字のリスト
		[[nodiscard]]
		constexpr String(std::initializer_list<value_type> list);

		/// @brief 指定した範囲の文字列をコピーして新しい文字列を作成します。
		/// @tparam Iterator イテレータの型
		/// @param first イテレータの開始位置
		/// @param last イテレータの終端位置
		template <std::input_iterator Iterator>
		[[nodiscard]]
		constexpr String(Iterator first, Iterator last);

		/// @brief 文字列をコピーして新しい文字列を作成します。
		/// @param s コピーする文字列
		[[nodiscard]]
		explicit constexpr String(const StringViewLike auto& s);

		/// @brief 文字列の指定位置以降をコピーして新しい文字列を作成します。
		/// @param s 対象の文字列
		/// @param pos コピーを開始する位置
		/// @param count コピーする文字数
		[[nodiscard]]
		constexpr String(const StringViewLike auto& s, size_type pos, size_type count);

	# ifdef __cpp_lib_containers_ranges
		
		/// @brief 範囲から新しい文字列を作成します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		template <class Range>
			requires Concept::ContainerCompatibleRange<String::value_type, Range>
		[[nodiscard]]
		constexpr String(std::from_range_t, Range&& range);
	
	# endif
		
		/// @brief nullptr からの構築は禁止されています。
		constexpr String(std::nullptr_t) = delete;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列をコピー代入します。
		/// @param other コピーする文字列
		/// @return *this
		constexpr String& operator =(const String& other);

		/// @brief 文字列をコピー代入します。
		/// @param other コピーする文字列
		/// @return *this
		constexpr String& operator =(const string_type& other);

		/// @brief 文字列をムーブ代入します。
		/// @param other ムーブする文字列
		/// @return *this
		constexpr String& operator =(String&& other) noexcept;

		/// @brief 文字列をムーブ代入します。
		/// @param other ムーブする文字列
		/// @return *this
		constexpr String& operator =(string_type&& other) noexcept;

		/// @brief 文字列をコピー代入します。
		/// @param s コピーする文字列
		/// @return *this
		constexpr String& operator =(const value_type* s);

		/// @brief 文字列を代入します。
		/// @param list 文字列を表現した文字のリスト
		/// @return *this
		constexpr String& operator =(std::initializer_list<value_type> list);

		/// @brief 文字列をコピー代入します。
		/// @param s コピーする文字列
		/// @return *this
		constexpr String& operator =(const StringViewLike auto& s);

		/// @brief nullptr の代入は禁止されています。
		constexpr String& operator =(std::nullptr_t) = delete;

		////////////////////////////////////////////////////////////////
		//
		//	assign
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した個数の同じ文字を文字列に代入します。
		/// @param count 繰り返す回数
		/// @param ch 繰り返す文字
		/// @return *this
		constexpr String& assign(size_type count, value_type ch) SIV3D_LIFETIMEBOUND;

		/// @brief 文字列をコピー代入します。
		/// @param s コピーする文字列
		/// @return *this
		constexpr String& assign(const String& s) SIV3D_LIFETIMEBOUND;

		/// @brief 文字列をコピー代入します。
		/// @param s コピーする文字列
		/// @return *this
		constexpr String& assign(const string_type& s) SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の指定位置から指定文字数をコピー代入します。
		/// @param s 対象の文字列
		/// @param pos コピーを開始する位置
		/// @param count コピーする文字数 (npos の場合は末尾まで)
		/// @return *this
		constexpr String& assign(const String& s, size_type pos, size_type count = npos) SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の指定位置から指定文字数をコピー代入します。
		/// @param s 対象の文字列
		/// @param pos コピーを開始する位置
		/// @param count コピーする文字数 (npos の場合は末尾まで)
		/// @return *this
		constexpr String& assign(const string_type& s, size_type pos, size_type count = npos) SIV3D_LIFETIMEBOUND;

		/// @brief 文字列をムーブ代入します。
		/// @param s ムーブする文字列
		/// @return *this
		constexpr String& assign(String&& s) noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列をムーブ代入します。
		/// @param s ムーブする文字列
		/// @return *this
		constexpr String& assign(string_type&& s) noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の先頭から指定文字数をコピー代入します。
		/// @param s 対象の文字列
		/// @param count コピーする文字数
		/// @return *this
		constexpr String& assign(const value_type* s, size_type count) SIV3D_LIFETIMEBOUND;

		/// @brief 文字列をコピー代入します。
		/// @param s コピーする文字列
		/// @return *this
		constexpr String& assign(const value_type* s) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した範囲の文字列をコピー代入します。
		/// @tparam Iterator イテレータの型
		/// @param first イテレータの開始位置
		/// @param last イテレータの終端位置
		/// @return *this
		template <std::input_iterator Iterator>
		constexpr String& assign(Iterator first, Iterator last) SIV3D_LIFETIMEBOUND;

		/// @brief 文字のリストを文字列に代入します。
		/// @param list 文字のリスト
		/// @return *this
		constexpr String& assign(std::initializer_list<value_type> list) SIV3D_LIFETIMEBOUND;

		/// @brief 文字列をコピー代入します。
		/// @param s コピーする文字列
		/// @return *this
		constexpr String& assign(const StringViewLike auto& s);

		/// @brief 文字列の指定位置から指定文字数をコピー代入します。
		/// @param s 対象の文字列
		/// @param pos コピーを開始する位置
		/// @param count コピーする文字数 (npos の場合は末尾まで)
		/// @return *this
		constexpr String& assign(const StringViewLike auto& s, size_type pos, size_type count = npos);

		////////////////////////////////////////////////////////////////
		//
		//	assign_range
		//
		////////////////////////////////////////////////////////////////

		/// @brief 範囲から文字列を代入します。
		/// @tparam Range 範囲の型
		/// @param range 代入する範囲
		/// @return *this
		template <class Range>
			requires Concept::ContainerCompatibleRange<String::value_type, Range>
		constexpr String& assign_range(Range&& range) SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	get_allocator
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列のアロケータを返します。
		/// @return 文字列のアロケータ
		[[nodiscard]]
		constexpr allocator_type get_allocator() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の文字への参照を返します。
		/// @param index 文字の位置
		/// @return 指定した位置の文字への参照
		/// @throw std::out_of_range 範囲外のインデックスを指定した場合
		[[nodiscard]]
		constexpr value_type& at(size_type index)& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の文字への参照を返します。
		/// @param index 文字の位置
		/// @return 指定した位置の文字への参照
		/// @throw std::out_of_range 範囲外のインデックスを指定した場合
		[[nodiscard]]
		constexpr const value_type& at(size_type index) const& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の文字を返します。
		/// @param index 文字の位置
		/// @return 指定した位置の文字
		/// @throw std::out_of_range 範囲外のインデックスを指定した場合
		[[nodiscard]]
		constexpr value_type at(size_type index)&&;

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の要素への参照を返します。
		/// @param index 要素の位置
		/// @return 指定した位置の要素への参照
		[[nodiscard]]
		constexpr value_type& operator [](size_type index) & noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の要素への参照を返します。
		/// @param index 要素の位置
		/// @return 指定した位置の要素への参照
		[[nodiscard]]
		constexpr const value_type& operator [](size_type index) const& noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の要素を返します。
		/// @param index 要素の位置
		/// @return 指定した位置の要素
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
		constexpr value_type& front()& noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 先頭の要素への参照を返します。
		/// @remark 空の文字列に対しては使えません。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		constexpr const value_type& front() const& noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 先頭の要素を返します。
		/// @remark 空の文字列に対しては使えません。
		/// @return 先頭の要素
		[[nodiscard]]
		constexpr value_type front()&& noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の要素への参照を返します。
		/// @remark 空の文字列に対しては使えません。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		constexpr value_type& back()& noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 末尾の要素への参照を返します。
		/// @remark 空の文字列に対しては使えません。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		constexpr const value_type& back() const& noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 末尾の要素を返します。
		/// @remark 空の文字列に対しては使えません。
		/// @return 末尾の要素
		[[nodiscard]]
		constexpr value_type back()&& noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	data
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列データへのポインタを返します。
		/// @return 文字列データへのポインタ
		/// @remark 返されるポインタは文字列が変更されると無効になります。
		[[nodiscard]]
		constexpr const value_type* data() const noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列データへのポインタを返します。
		/// @return 文字列データへのポインタ
		/// @remark 返されるポインタは文字列が変更されると無効になります。
		[[nodiscard]]
		constexpr value_type* data() noexcept SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	c_str
		//
		////////////////////////////////////////////////////////////////

		/// @brief ヌル終端された文字列データへのポインタを返します。
		/// @return ヌル終端された文字列データへのポインタ
		/// @remark 返されるポインタは文字列が変更されると無効になります。
		[[nodiscard]]
		constexpr const value_type* c_str() const noexcept SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	str
		//
		////////////////////////////////////////////////////////////////

		/// @brief 内部の std::u32string への参照を返します。
		/// @return 内部の std::u32string への参照
		[[nodiscard]]
		constexpr string_type& str() noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 内部の std::u32string への参照を返します。
		/// @return 内部の std::u32string への参照
		[[nodiscard]]
		constexpr const string_type& str() const noexcept SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	operator StringView
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を StringView に変換します。
		/// @return 文字列の StringView
		constexpr operator StringView() const noexcept SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	begin, end
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の先頭位置を指すイテレータを返します。
		/// @return 文字列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr iterator begin() noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 文字列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr iterator end() noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の先頭位置を指すイテレータを返します。
		/// @return 文字列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator begin() const noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 文字列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator end() const noexcept SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	cbegin, cend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の先頭位置を指すイテレータを返します。
		/// @return 文字列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator cbegin() const noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 文字列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator cend() const noexcept SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	rbegin, rend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の末尾位置を指すリバース・イテレータを返します。
		/// @return 文字列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr reverse_iterator rbegin() noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 文字列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr reverse_iterator rend() noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の末尾位置を指すリバース・イテレータを返します。
		/// @return 文字列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rbegin() const noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 文字列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rend() const noexcept SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	crbegin, crend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の末尾位置を指すリバース・イテレータを返します。
		/// @return 文字列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator crbegin() const noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 文字列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator crend() const noexcept SIV3D_LIFETIMEBOUND;

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

		/// @brief 文字列が保持できる最大の文字数を返します。
		/// @return 文字列が保持できる最大の文字数
		[[nodiscard]]
		constexpr size_type max_size() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reserve
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字数を格納できるメモリ領域を確保します。
		/// @param newCapacity 確保する文字数
		/// @remark newCapacity が現在の capacity() より小さい場合は何もしません。
		constexpr void reserve(size_type newCapacity);

		////////////////////////////////////////////////////////////////
		//
		//	capacity
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリの再確保なしに格納できる文字数を返します。
		/// @return メモリの再確保なしに格納できる文字数
		[[nodiscard]]
		constexpr size_type capacity() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	shrink_to_fit
		//
		////////////////////////////////////////////////////////////////

		/// @brief 使用するメモリ量を現在のサイズまで切り詰めます。
		/// @remark この関数の呼び出しの後で capacity() == size() になるとは限りません。
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

		/// @brief 要素をすべて消去し、メモリも解放します。
		constexpr void release();

		////////////////////////////////////////////////////////////////
		//
		//	insert
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に同じ文字を複数挿入します。
		/// @param offset 挿入位置
		/// @param count 挿入する文字数
		/// @param ch 挿入する文字
		/// @return *this
		constexpr String& insert(size_type offset, size_type count, value_type ch) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置に文字列を挿入します。
		/// @param offset 挿入位置
		/// @param s 挿入する文字列
		/// @return *this
		constexpr String& insert(size_type offset, const value_type* s) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置に文字列の先頭から指定文字数を挿入します。
		/// @param offset 挿入位置
		/// @param s 挿入する文字列
		/// @param count 挿入する文字数
		/// @return *this
		constexpr String& insert(size_type offset, const value_type* s, size_type count) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置に文字列を挿入します。
		/// @param offset 挿入位置
		/// @param s 挿入する文字列
		/// @return *this
		constexpr String& insert(size_type offset, const String& s) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置に文字列の一部を挿入します。
		/// @param offset 挿入位置
		/// @param s 挿入する文字列
		/// @param offset2 挿入する文字列の開始位置
		/// @param count 挿入する文字数 (npos の場合は末尾まで)
		/// @return *this
		constexpr String& insert(size_type offset, const String& s, size_type offset2, size_type count = npos) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置に文字を挿入します。
		/// @param pos 挿入位置のイテレータ
		/// @param ch 挿入する文字
		/// @return 挿入された文字を指すイテレータ
		constexpr iterator insert(const_iterator pos, value_type ch) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置に同じ文字を複数挿入します。
		/// @param pos 挿入位置のイテレータ
		/// @param count 挿入する文字数
		/// @param ch 挿入する文字
		/// @return 最初に挿入された文字を指すイテレータ
		constexpr iterator insert(const_iterator pos, size_type count, value_type ch) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置に範囲の要素を挿入します。
		/// @tparam Iterator イテレータの型
		/// @param pos 挿入位置のイテレータ
		/// @param first 範囲の開始イテレータ
		/// @param last 範囲の終端イテレータ
		/// @return 最初に挿入された要素を指すイテレータ
		template <std::input_iterator Iterator>
		constexpr iterator insert(const_iterator pos, Iterator first, Iterator last) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置に文字のリストを挿入します。
		/// @param pos 挿入位置のイテレータ
		/// @param list 挿入する文字のリスト
		/// @return 最初に挿入された要素を指すイテレータ
		constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> list) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置に文字列を挿入します。
		/// @param offset 挿入位置
		/// @param s 挿入する文字列
		/// @return *this
		constexpr String& insert(size_type offset, const StringViewLike auto& s);

		/// @brief 指定した位置に文字列の一部を挿入します。
		/// @param offset 挿入位置
		/// @param s 挿入する文字列
		/// @param offset2 挿入する文字列の開始位置
		/// @param count 挿入する文字数 (npos の場合は末尾まで)
		/// @return *this
		constexpr String& insert(size_type offset, const StringViewLike auto& s, size_type offset2, size_type count = npos);

		////////////////////////////////////////////////////////////////
		//
		//	insert_range
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に範囲の要素を挿入します。
		/// @tparam Range 範囲の型
		/// @param pos 挿入位置のイテレータ
		/// @param range 挿入する範囲
		/// @return 最初に挿入された要素を指すイテレータ
		template <class Range>
			requires Concept::ContainerCompatibleRange<String::value_type, Range>
		constexpr iterator insert_range(const_iterator pos, Range&& range) SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	erase
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置から指定した文字数を削除します。
		/// @param offset 削除を開始する位置 (デフォルトは 0)
		/// @param count 削除する文字数 (npos の場合は末尾まで)
		/// @return *this
		constexpr String& erase(size_type offset = 0, size_type count = npos) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の文字を削除します。
		/// @param pos 削除する文字の位置を指すイテレータ
		/// @return 削除された文字の次の位置を指すイテレータ
		constexpr iterator erase(const_iterator pos) noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 指定した範囲の文字を削除します。
		/// @param first 削除する範囲の開始位置を指すイテレータ
		/// @param last 削除する範囲の終端位置を指すイテレータ
		/// @return 削除された範囲の次の位置を指すイテレータ
		constexpr iterator erase(const_iterator first, const_iterator last) noexcept SIV3D_LIFETIMEBOUND;

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
		constexpr String& operator <<(value_type ch) SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字を末尾に追加します。
		/// @param ch 追加する文字
		/// @return *this
		constexpr String& append(value_type ch) SIV3D_LIFETIMEBOUND;

		/// @brief 同じ文字を複数個末尾に追加します。
		/// @param count 追加する文字数
		/// @param ch 追加する文字
		/// @return *this
		constexpr String& append(size_type count, value_type ch) SIV3D_LIFETIMEBOUND;

		/// @brief 文字列を末尾に追加します。
		/// @param s 追加する文字列
		/// @return *this
		constexpr String& append(const String& s) SIV3D_LIFETIMEBOUND;

		/// @brief 文字列を末尾に追加します。
		/// @param s 追加する文字列
		/// @return *this
		constexpr String& append(const string_type& s) SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の一部を末尾に追加します。
		/// @param s 追加する文字列
		/// @param offset 追加する文字列の開始位置
		/// @param count 追加する文字数 (npos の場合は末尾まで)
		/// @return *this
		constexpr String& append(const String& s, size_type offset, size_type count = npos) SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の先頭から指定文字数を末尾に追加します。
		/// @param s 追加する文字列
		/// @param count 追加する文字数
		/// @return *this
		constexpr String& append(const value_type* s, size_type count) SIV3D_LIFETIMEBOUND;

		/// @brief 文字列を末尾に追加します。
		/// @param s 追加する文字列
		/// @return *this
		constexpr String& append(const value_type* s) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した範囲の文字を末尾に追加します。
		/// @tparam Iterator イテレータの型
		/// @param first 範囲の開始イテレータ
		/// @param last 範囲の終端イテレータ
		/// @return *this
		template <std::input_iterator Iterator>
		constexpr String& append(Iterator first, Iterator last) SIV3D_LIFETIMEBOUND;

		/// @brief 文字のリストを末尾に追加します。
		/// @param list 追加する文字のリスト
		/// @return *this
		constexpr String& append(std::initializer_list<value_type> list) SIV3D_LIFETIMEBOUND;

		/// @brief 文字列を末尾に追加します。
		/// @param s 追加する文字列
		/// @return *this
		constexpr String& append(const StringViewLike auto& s);

		/// @brief 文字列の一部を末尾に追加します。
		/// @param s 追加する文字列
		/// @param pos 追加する文字列の開始位置
		/// @param count 追加する文字数 (npos の場合は末尾まで)
		/// @return *this
		constexpr String& append(const StringViewLike auto& s, size_type pos, size_type count = npos);

		////////////////////////////////////////////////////////////////
		//
		//	append_range
		//
		////////////////////////////////////////////////////////////////

		/// @brief 範囲の文字を末尾に追加します。
		/// @tparam Range 範囲の型
		/// @param range 追加する範囲
		/// @return *this
		template <class Range>
			requires Concept::ContainerCompatibleRange<String::value_type, Range>
		constexpr String& append_range(Range&& range) SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を末尾に追加します。
		/// @param s 追加する文字列
		/// @return *this
		constexpr String& operator +=(const String& s);

		/// @brief 文字列を末尾に追加します。
		/// @param s 追加する文字列
		/// @return *this
		constexpr String& operator +=(const string_type& s);

		/// @brief 文字を末尾に追加します。
		/// @param ch 追加する文字
		/// @return *this
		constexpr String& operator +=(value_type ch);

		/// @brief 文字列を末尾に追加します。
		/// @param s 追加する文字列
		/// @return *this
		constexpr String& operator +=(const value_type* s);

		/// @brief 文字のリストを末尾に追加します。
		/// @param list 追加する文字のリスト
		/// @return *this
		constexpr String& operator +=(std::initializer_list<value_type> list);

		/// @brief 文字列を末尾に追加します。
		/// @param s 追加する文字列
		/// @return *this
		constexpr String& operator +=(const StringViewLike auto& s);

		////////////////////////////////////////////////////////////////
		//
		//	replace
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置から指定文字数を文字列で置き換えます。
		/// @param pos 置き換える位置
		/// @param count 置き換える文字数
		/// @param s 置き換える文字列
		/// @return *this
		constexpr String& replace(size_type pos, size_type count, const String& s) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した範囲を文字列で置き換えます。
		/// @param first 置き換える範囲の開始イテレータ
		/// @param last 置き換える範囲の終端イテレータ
		/// @param s 置き換える文字列
		/// @return *this
		constexpr String& replace(const_iterator first, const_iterator last, const String& s) SIV3D_LIFETIMEBOUND;
		
		/// @brief 指定した範囲を文字列で置き換えます。
		/// @param first 置き換える範囲の開始イテレータ
		/// @param last 置き換える範囲の終端イテレータ
		/// @param s 置き換える文字列
		/// @return *this
		constexpr String& replace(const_iterator first, const_iterator last, const value_type* s) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した範囲を文字列で置き換えます。
		/// @param first 置き換える範囲の開始イテレータ
		/// @param last 置き換える範囲の終端イテレータ
		/// @param s 置き換える文字列
		/// @return *this
		constexpr String& replace(const_iterator first, const_iterator last, const StringViewLike auto& s);

		/// @brief 指定した位置から指定文字数を文字列で置き換えます。
		/// @param pos 置き換える位置
		/// @param count 置き換える文字数
		/// @param s 置き換える文字列
		/// @return *this
		constexpr String& replace(size_type pos, size_type count, const value_type* s) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した範囲を新しい範囲で置き換えます。
		/// @tparam Iterator イテレータの型
		/// @param first 置き換える範囲の開始イテレータ
		/// @param last 置き換える範囲の終端イテレータ
		/// @param newFirst 新しい範囲の開始イテレータ
		/// @param newLast 新しい範囲の終端イテレータ
		/// @return *this
		template <class Iterator>
		constexpr String& replace(const_iterator first, const_iterator last, Iterator newFirst, Iterator newLast) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置から指定文字数を文字列で置き換えます。
		/// @param pos 置き換える位置
		/// @param count 置き換える文字数
		/// @param s 置き換える文字列
		/// @return *this
		constexpr String& replace(size_type pos, size_type count, const StringViewLike auto& s);

		////////////////////////////////////////////////////////////////
		//
		//	resize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列のサイズを変更します。
		/// @param newSize 新しいサイズ
		/// @remark サイズが増加する場合、追加された部分はヌル文字で埋められます。
		constexpr void resize(size_type newSize);

		/// @brief 文字列のサイズを変更します。
		/// @param newSize 新しいサイズ
		/// @param ch サイズが増加した場合に追加される文字
		constexpr void resize(size_type newSize, value_type ch);

		////////////////////////////////////////////////////////////////
		//
		//	resize_and_overwrite
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列のサイズを変更し、指定した操作で新しい要素を書き込みます。
		/// @tparam Operation 操作の型
		/// @param count 新しいサイズ
		/// @param op サイズ変更後に呼び出される操作
		template <class Operation>
		constexpr void resize_and_overwrite(size_type count, Operation op);

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 他の文字列と内容を交換します。
		/// @param other 交換する文字列
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
		/// @remark s は ヌル終端されている必要があります。
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
		/// @remark s は ヌル終端されている必要があります。
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
		/// @remark s は ヌル終端されている必要があります。
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
		/// @remark s は ヌル終端されている必要があります。
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
		//	subview
		//
		////////////////////////////////////////////////////////////////

		/// @brief 部分文字列へのビューを取得します。
		/// @param offset 開始インデックス
		/// @param count 取得する文字数。末尾までの場合 npos
		/// @return 部分文字列へのビュー
		[[nodiscard]]
		constexpr StringView subview(size_type offset = 0, size_type count = npos) const& SIV3D_LIFETIMEBOUND;

		[[nodiscard]]
		constexpr StringView subview(size_type offset = 0, size_type count = npos)&& SIV3D_LIFETIMEBOUND = delete;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列のハッシュ値を返します。
		/// @return 文字列のハッシュ値
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
		constexpr const std::u32string& toUTF32() const noexcept SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	subspan
		//
		////////////////////////////////////////////////////////////////

		/// @brief 部分配列を指す span を返します。
		/// @param pos 部分配列の開始位置
		/// @param count 部分配列の要素数
		/// @return 部分配列を指す span
		[[nodiscard]]
		constexpr std::span<value_type> subspan(size_type pos, size_type count) noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 部分配列を指す span を返します。
		/// @param pos 部分配列の開始位置
		/// @param count 部分配列の要素数
		/// @return 部分配列を指す span
		[[nodiscard]]
		constexpr std::span<const value_type> subspan(size_type pos, size_type count) const noexcept SIV3D_LIFETIMEBOUND;

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
		constexpr bool all(Fty f) const
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	any
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす要素があるかを返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 条件を満たす要素が 1 つでもあれば true, それ以外の場合は false
		/// @remark `.contains_if(f)` と同じです。
		template <class Fty>
		[[nodiscard]]
		constexpr bool any(Fty f) const
			requires std::predicate<Fty&, const value_type&>;

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
		String& capitalize()& noexcept SIV3D_LIFETIMEBOUND;

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
		//	choice
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の要素を 1 つランダムに返します。
		/// @return 文字列からランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type& choice()& SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の要素を 1 つランダムに返します。
		/// @return 文字列からランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type choice() const&;

		/// @brief 文字列の要素を 1 つランダムに返します。
		/// @return 文字列からランダムに選ばれた要素
		[[nodiscard]]
		value_type choice()&&;

		/// @brief 指定した乱数エンジンを用いて、文字列の要素を 1 つランダムに返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 文字列からランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type& choice(Concept::UniformRandomBitGenerator auto&& rbg)& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した乱数エンジンを用いて、配列の要素を 1 つランダムに返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 文字列からランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type choice(Concept::UniformRandomBitGenerator auto&& rbg) const&;

		/// @brief 指定した乱数エンジンを用いて、文字列の要素を 1 つランダムに返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 文字列からランダムに選ばれた要素
		[[nodiscard]]
		value_type choice(Concept::UniformRandomBitGenerator auto&& rbg)&&;

		////////////////////////////////////////////////////////////////
		//
		//	contains_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素があるかを返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @remark `.any(f)` と同じです。
		/// @return 条件を満たす要素が 1 つでもあれば true, それ以外の場合は false
		template <class Fty>
		[[nodiscard]]
		constexpr bool contains_if(Fty f) const
			requires std::predicate<Fty&, const value_type&>;

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
		constexpr int64 count_if(Fty f) const
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	each
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (auto& ch : s) f(ch);` と同じです。
		template <class Fty>
		constexpr void each(Fty f)
			requires std::invocable<Fty&, value_type&>;

		/// @brief すべての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (const auto& ch : s) f(ch);` と同じです。
		template <class Fty>
		constexpr void each(Fty f) const
			requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	each_index
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (size_t i = 0; auto& ch : s) f(i++, ch);` と同じです。
		template <class Fty>
		constexpr void each_index(Fty f)
			requires std::invocable<Fty&, size_t, value_type&>;

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (size_t i = 0; auto ch : s) f(i++, ch);` と同じです。
		template <class Fty>
		constexpr void each_index(Fty f) const
			requires std::invocable<Fty&, size_t, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	each_sindex
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (isize i = 0; auto& ch : s) f(i++, ch);` と同じです。
		template <class Fty>
		constexpr void each_sindex(Fty f)
			requires std::invocable<Fty&, isize, value_type&>;

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (isize i = 0; auto ch : s) f(i++, ch);` と同じです。
		template <class Fty>
		constexpr void each_sindex(Fty f) const
			requires std::invocable<Fty&, isize, const value_type&>;

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
		//	fill
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値をすべての要素に代入します。
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
		constexpr String filter(Fty f) const
			requires std::predicate<Fty&, const value_type&>;

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
		constexpr String& keep_if(Fty f) SIV3D_LIFETIMEBOUND
			requires std::predicate<Fty&, const value_type&>;

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
		String& leftPad(size_type length, value_type fillChar = U' ') & noexcept SIV3D_LIFETIMEBOUND;

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

		/// @brief 英字をすべて小文字にします。
		/// @return *this
		String& lowercase()& noexcept SIV3D_LIFETIMEBOUND;

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

		/// @brief 文字列の先頭から空白文字を削除します。
		/// @return *this
		String& leftTrim() & noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の先頭から空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String leftTrim() && noexcept;

		/// @brief 文字列の先頭から指定した文字を削除します。
		/// @param chars 削除する文字の集合
		/// @return *this
		String& leftTrim(StringView chars) & noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の先頭から指定した文字を削除した新しい文字列を返します。
		/// @param chars 削除する文字の集合
		/// @return 新しい文字列
		[[nodiscard]]
		String leftTrim(StringView chars) && noexcept;

		/// @brief 文字列の先頭から空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String leftTrimmed() const&;

		/// @brief 文字列の先頭から空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String leftTrimmed() && noexcept;

		/// @brief 文字列の先頭から指定した文字を削除した新しい文字列を返します。
		/// @param chars 削除する文字の集合
		/// @return 新しい文字列
		[[nodiscard]]
		String leftTrimmed(StringView chars) const&;

		/// @brief 文字列の先頭から指定した文字を削除した新しい文字列を返します。
		/// @param chars 削除する文字の集合
		/// @return 新しい文字列
		[[nodiscard]]
		String leftTrimmed(StringView chars) && noexcept;

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
		constexpr auto map(Fty f) const
			requires std::invocable<Fty&, const value_type&>;

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
		constexpr bool none(Fty f) const
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	remove, removed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字を文字列から削除します。
		/// @param ch 削除する文字
		/// @return *this
		String& remove(value_type ch) & noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 指定した文字列をもとの文字列から削除します。
		/// @param s 削除する文字列
		/// @return *this
		String& remove(StringView s) & noexcept SIV3D_LIFETIMEBOUND;

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
		String& remove_at(size_type index) & noexcept SIV3D_LIFETIMEBOUND;

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
		constexpr String& remove_if(Fty f) & noexcept SIV3D_LIFETIMEBOUND
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす要素を削除した新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String remove_if(Fty f) && noexcept
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす要素を削除した新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String removed_if(Fty f) const&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす要素を削除した新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String removed_if(Fty f) && noexcept
			requires std::predicate<Fty&, const value_type&>;

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
		//	replace, replaced
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した文字を置換します。
		/// @param oldChar 置換対象の文字
		/// @param newChar 置換後の文字
		/// @return *this
		String& replace(value_type oldChar, value_type newChar)& noexcept SIV3D_LIFETIMEBOUND;

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
		String& replace(StringView oldStr, StringView newStr)& SIV3D_LIFETIMEBOUND;

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
		constexpr String& replace_if(Fty f, value_type newChar) & noexcept SIV3D_LIFETIMEBOUND
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす要素を別の文字に置き換えた新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @param newChar 置換後の文字
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String replace_if(Fty f, value_type newChar) && noexcept
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす要素を別の文字に置き換えた新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @param newChar 置換後の文字
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String replaced_if(Fty f, value_type newChar) const&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす要素を別の文字に置き換えた新しい文字列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @param newChar 置換後の文字
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		constexpr String replaced_if(Fty f, value_type newChar) && noexcept
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	replaceFirst, replacedFirst
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した正規表現にマッチする最初の部分文字列を置換します。
		/// @param regexp 置換対象の正規表現
		/// @param replacement マッチした部分文字列を置換する文字列
		/// @return *this
		String& replaceFirst(const RegExp& regexp, StringView replacement) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した正規表現にマッチする最初の部分文字列を置換した新しい文字列を返します。
		/// @param regexp 置換対象の正規表現
		/// @param replacement マッチした部分文字列を置換する文字列
		/// @return 新しい文字列
		[[nodiscard]]
		String replacedFirst(const RegExp& regexp, StringView replacement) const;

		////////////////////////////////////////////////////////////////
		//
		//	replaceAll, replacedAll
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した正規表現にマッチする部分文字列を置換します。
		/// @param regexp 置換対象の正規表現
		/// @param replacement マッチした部分文字列を置換する文字列
		/// @return *this
		String& replaceAll(const RegExp& regexp, StringView replacement) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した正規表現にマッチする部分文字列を置換します。
		/// @param regexp 置換対象の正規表現
		/// @param replacement マッチした部分文字列を置換する文字列
		/// @return *this
		String& replaceAll(const RegExp& regexp, FunctionRef<String(const MatchResults&)> replacementFunc) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した正規表現にマッチする部分文字列を置換した新しい文字列を返します。
		/// @param regexp 置換対象の正規表現
		/// @param replacement マッチした部分文字列を置換する文字列
		/// @return 新しい文字列
		[[nodiscard]]
		String replacedAll(const RegExp& regexp, StringView replacement) const;

		/// @brief 指定した正規表現にマッチする部分文字列を置換した新しい文字列を返します。
		/// @param regexp 置換対象の正規表現
		/// @param replacementFunc マッチした部分文字列を置換する関数
		/// @return 新しい文字列
		[[nodiscard]]
		String replacedAll(const RegExp& regexp, FunctionRef<String(const MatchResults&)> replacementFunc) const;

		////////////////////////////////////////////////////////////////
		//
		//	reverse, reversed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を反転します。
		/// @return *this
		String& reverse() & noexcept SIV3D_LIFETIMEBOUND;

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
		constexpr void reverse_each(Fty f)
			requires std::invocable<Fty&, value_type&>;

		/// @brief 各要素を引数にして関数を呼び出します。順番は後ろからです。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		constexpr void reverse_each(Fty f) const
			requires std::invocable<Fty&, const value_type&>;

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
		String& rightPad(size_type length, value_type fillChar = U' ') & noexcept SIV3D_LIFETIMEBOUND;

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

		/// @brief 文字列の末尾から空白文字を削除します。
		/// @return *this
		String& rightTrim() & noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の末尾から空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String rightTrim() && noexcept;

		/// @brief 文字列の末尾から指定した文字を削除します。
		/// @param chars 削除する文字の集合
		/// @return *this
		String& rightTrim(StringView chars) & noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の末尾から指定した文字を削除した新しい文字列を返します。
		/// @param chars 削除する文字の集合
		/// @return 新しい文字列
		[[nodiscard]]
		String rightTrim(StringView chars) && noexcept;

		/// @brief 文字列の末尾から空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String rightTrimmed() const&;

		/// @brief 文字列の末尾から空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String rightTrimmed() && noexcept;

		/// @brief 文字列の末尾から指定した文字を削除した新しい文字列を返します。
		/// @param chars 削除する文字の集合
		/// @return 新しい文字列
		[[nodiscard]]
		String rightTrimmed(StringView chars) const&;

		/// @brief 文字列の末尾から指定した文字を削除した新しい文字列を返します。
		/// @param chars 削除する文字の集合
		/// @return 新しい文字列
		[[nodiscard]]
		String rightTrimmed(StringView chars) && noexcept;

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
		String& rotate(size_type index) & noexcept SIV3D_LIFETIMEBOUND;

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
		String& rsort() & noexcept SIV3D_LIFETIMEBOUND;

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
		String& shuffle() & noexcept SIV3D_LIFETIMEBOUND;

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
		String& shuffle(Concept::UniformRandomBitGenerator auto&& rbg) & noexcept SIV3D_LIFETIMEBOUND;

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
		String& sort() & noexcept SIV3D_LIFETIMEBOUND;

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
		String& sort_by(Fty f) & noexcept SIV3D_LIFETIMEBOUND
			requires std::predicate<Fty&, const value_type&, const value_type&>;
		
		/// @brief 指定した比較関数を用いて要素を昇順にソートした新しい文字列を返します。
		/// @tparam Fty 比較関数の型
		/// @param f 比較関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		String sort_by(Fty f) && noexcept
			requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した比較関数を用いて要素を昇順にソートした新しい文字列を返します。
		/// @tparam Fty 比較関数の型
		/// @param f 比較関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		String sorted_by(Fty f) const&
			requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した比較関数を用いて要素を昇順にソートした新しい文字列を返します。
		/// @tparam Fty 比較関数の型
		/// @param f 比較関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		String sorted_by(Fty f) && noexcept
			requires std::predicate<Fty&, const value_type&, const value_type&>;

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
		//	splitAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置で文字列を分割します。
		/// @param pos 位置
		/// @remark pos が文字列の長さ以上の場合 `{ *this, U"" }` を返します。
		/// @return 分割された文字列
		[[nodiscard]]
		std::pair<String, String> splitAt(size_type pos) const;

		////////////////////////////////////////////////////////////////
		//
		//	splitLines
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列を行ごとに分割します。
		/// @return 分割された文字列
		[[nodiscard]]
		Array<StringView, std::allocator<StringView>> splitLines() const SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	swapcase, swapcased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 英字の大文字と小文字を入れ替えます。
		/// @return *this
		String& swapcase() & noexcept SIV3D_LIFETIMEBOUND;

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
		String take_while(Fty f) const&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 先頭から指定された条件を満たし続ける間、要素を取り出します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい文字列
		template <class Fty>
		[[nodiscard]]
		String take_while(Fty f) && noexcept
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	trim, trimmed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の先頭と末尾から空白文字を削除します。
		/// @return *this
		String& trim()& noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の先頭と末尾から空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String trim()&& noexcept;

		/// @brief 文字列の先頭と末尾から指定した文字を削除します。
		/// @param chars 削除する文字の集合
		/// @return *this
		String& trim(StringView chars) & noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 文字列の先頭と末尾から指定した文字を削除した新しい文字列を返します。
		/// @param chars 削除する文字の集合
		/// @return 新しい文字列
		[[nodiscard]]
		String trim(StringView chars) && noexcept;

		/// @brief 文字列の先頭と末尾から空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String trimmed() const&;

		/// @brief 文字列の先頭と末尾から空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String trimmed()&& noexcept;

		/// @brief 文字列の先頭と末尾から指定した文字を削除した新しい文字列を返します。
		/// @param chars 削除する文字の集合
		/// @return 新しい文字列
		[[nodiscard]]
		String trimmed(StringView chars) const&;

		/// @brief 文字列の先頭と末尾から指定した文字を削除した新しい文字列を返します。
		/// @param chars 削除する文字の集合
		/// @return 新しい文字列
		[[nodiscard]]
		String trimmed(StringView chars) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	uppercase, uppercased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 英字をすべて大文字にします。
		/// @return *this
		String& uppercase() & noexcept SIV3D_LIFETIMEBOUND;

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
		String& stable_unique() & noexcept SIV3D_LIFETIMEBOUND;

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
		String& sort_and_unique() & noexcept SIV3D_LIFETIMEBOUND;

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
		String& unique_consecutive() & noexcept SIV3D_LIFETIMEBOUND;

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
		/// @remark `String{ U"abcde" }.values_at({0, 3, 4}); // U"ade"`
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
		String& xml_escape() SIV3D_LIFETIMEBOUND;

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

		/// @brief 二つの文字列が等しいかを判定します。
		/// @param lhs 左側の文字列
		/// @param rhs 右側の文字列
		/// @return 二つの文字列が等しい場合 true, それ以外の場合は false
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

		/// @brief 二つの文字列を三方比較します。
		/// @param lhs 左側の文字列
		/// @param rhs 右側の文字列
		/// @return 比較結果
		[[nodiscard]]
		friend constexpr auto operator <=>(const String&, const String&) noexcept = default;

		/// @brief 文字列と C スタイル文字列が等しいかを判定します。
		/// @param lhs 左側の文字列
		/// @param rhs 右側の C スタイル文字列
		/// @return 二つの文字列が等しい場合 true, それ以外の場合は false
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

		/// @brief 文字列と C スタイル文字列を三方比較します。
		/// @param lhs 左側の文字列
		/// @param rhs 右側の C スタイル文字列
		/// @return 比較結果
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

		/// @brief 二つの文字列の内容を交換します。
		/// @param lhs 左側の文字列
		/// @param rhs 右側の文字列
		friend constexpr void swap(String& lhs, String& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		/// @brief 二つの文字列を連結した新しい文字列を返します。
		/// @param lhs 左側の文字列
		/// @param rhs 右側の文字列
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(const String& lhs, const String& rhs)
		{
			return String(lhs.m_string + rhs.m_string);
		}

		/// @brief 文字列と C スタイル文字列を連結した新しい文字列を返します。
		/// @param lhs 左側の文字列
		/// @param rhs 右側の C スタイル文字列
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(const String& lhs, const value_type* rhs)
		{
			return String(lhs.m_string + rhs);
		}

		/// @brief 文字列と文字を連結した新しい文字列を返します。
		/// @param lhs 左側の文字列
		/// @param rhs 右側の文字
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(const String& lhs, const value_type rhs)
		{
			return String(lhs.m_string + rhs);
		}

		/// @brief C スタイル文字列と文字列を連結した新しい文字列を返します。
		/// @param lhs 左側の C スタイル文字列
		/// @param rhs 右側の文字列
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(const value_type* lhs, const String& rhs)
		{
			return String(lhs + rhs.m_string);
		}

		/// @brief 文字と文字列を連結した新しい文字列を返します。
		/// @param lhs 左側の文字
		/// @param rhs 右側の文字列
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(const value_type lhs, const String& rhs)
		{
			return String(lhs + rhs.m_string);
		}

		/// @brief 二つの文字列を連結した新しい文字列を返します（ムーブ版）。
		/// @param lhs 左側の文字列（ムーブ）
		/// @param rhs 右側の文字列（ムーブ）
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(String&& lhs, String&& rhs)
		{
			return String(std::move(lhs.m_string) + std::move(rhs.m_string));
		}

		/// @brief 文字列と文字列を連結した新しい文字列を返します（左辺ムーブ版）。
		/// @param lhs 左側の文字列（ムーブ）
		/// @param rhs 右側の文字列
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(String&& lhs, const String& rhs)
		{
			return String(std::move(lhs.m_string) + rhs.m_string);
		}

		/// @brief 文字列と C スタイル文字列を連結した新しい文字列を返します（左辺ムーブ版）。
		/// @param lhs 左側の文字列（ムーブ）
		/// @param rhs 右側の C スタイル文字列
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(String&& lhs, const value_type* rhs)
		{
			return String(std::move(lhs.m_string) + rhs);
		}

		/// @brief 文字列と文字を連結した新しい文字列を返します（左辺ムーブ版）。
		/// @param lhs 左側の文字列（ムーブ）
		/// @param rhs 右側の文字
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(String&& lhs, const value_type rhs)
		{
			return String(std::move(lhs.m_string) + rhs);
		}

		/// @brief 文字列と文字列を連結した新しい文字列を返します（右辺ムーブ版）。
		/// @param lhs 左側の文字列
		/// @param rhs 右側の文字列（ムーブ）
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(const String& lhs, String&& rhs)
		{
			return String(lhs.m_string + std::move(rhs.m_string));
		}

		/// @brief C スタイル文字列と文字列を連結した新しい文字列を返します（右辺ムーブ版）。
		/// @param lhs 左側の C スタイル文字列
		/// @param rhs 右側の文字列（ムーブ）
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(const value_type* lhs, String&& rhs)
		{
			return String(lhs + std::move(rhs.m_string));
		}

		/// @brief 文字と文字列を連結した新しい文字列を返します（右辺ムーブ版）。
		/// @param lhs 左側の文字
		/// @param rhs 右側の文字列（ムーブ）
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(const value_type lhs, String&& rhs)
		{
			return String(lhs + std::move(rhs.m_string));
		}

		/// @brief 文字列と StringView を連結した新しい文字列を返します。
		/// @param lhs 左側の文字列
		/// @param rhs 右側の StringView
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(const String& lhs, const StringView rhs)
		{
			String s;
			s.reserve(lhs.size() + rhs.size());
			s.append(lhs).append(rhs);
			return s;
		}

		/// @brief StringView と文字列を連結した新しい文字列を返します。
		/// @param lhs 左側の StringView
		/// @param rhs 右側の文字列
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(const StringView lhs, const String& rhs)
		{
			String s;
			s.reserve(lhs.size() + rhs.size());
			s.append(lhs).append(rhs);
			return s;
		}

		/// @brief 文字列と StringView を連結した新しい文字列を返します（左辺ムーブ版）。
		/// @param lhs 左側の文字列（ムーブ）
		/// @param rhs 右側の StringView
		/// @return 連結された新しい文字列
		[[nodiscard]]
		friend constexpr String operator +(String&& lhs, const StringView rhs)
		{
			lhs.append(rhs);
			return std::move(lhs);
		}

		/// @brief StringView と文字列を連結した新しい文字列を返します（右辺ムーブ版）。
		/// @param lhs 左側の StringView
		/// @param rhs 右側の文字列（ムーブ）
		/// @return 連結された新しい文字列
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

		/// @brief 文字列を出力ストリームに出力します。
		/// @param output 出力ストリーム
		/// @param value 出力する文字列
		/// @return 出力ストリーム
		friend std::ostream& operator <<(std::ostream& output, const String& value);

		/// @brief 文字列を出力ストリームに出力します。
		/// @param output ワイド文字出力ストリーム
		/// @param value 出力する文字列
		/// @return 出力ストリーム
		friend std::wostream& operator <<(std::wostream& output, const String& value);

		/// @brief 文字列を出力ストリームに出力します。
		/// @param output char32 出力ストリーム
		/// @param value 出力する文字列
		/// @return 出力ストリーム
		friend std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& output, const String& value);

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力ストリームから文字列を読み込みます。
		/// @param input 入力ストリーム
		/// @param value 読み込み先の文字列
		/// @return 入力ストリーム
		friend std::istream& operator >>(std::istream& input, String& value);

		/// @brief 入力ストリームから文字列を読み込みます。
		/// @param input ワイド文字入力ストリーム
		/// @param value 読み込み先の文字列
		/// @return 入力ストリーム
		friend std::wistream& operator >>(std::wistream& input, String& value);

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列をフォーマットします。
		/// @param formatData フォーマットデータ
		/// @param s フォーマットする文字列
		friend void Formatter(FormatData& formatData, const String& s);

	private:

		string_type m_string;

		[[noreturn]]
		static void ThrowAtOutOfRange();

		[[noreturn]]
		static void ThrowChoiceOutOfRange();

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

			/// @brief 文字列リテラルから String オブジェクトを作成します。
			/// @param s 文字列リテラル
			/// @param length 文字列の長さ
			/// @return String オブジェクト
			/// @remark 使用例: auto str = U"Hello"_s;
			[[nodiscard]]
			constexpr String operator ""_s(const char32_t* s, size_t length);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	FilePath
	//
	////////////////////////////////////////////////////////////////

	/// @brief ファイルパスを表す文字列型
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
