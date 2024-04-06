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
# include <vector>
# include <numeric>
# include <future>
# if SIV3D_PLATFORM(WINDOWS)
#	include <execution>
# endif
# include "String.hpp"
# include "Unicode.hpp"
# include "Format.hpp"
# include "Utility.hpp"
# include "KahanSummation.hpp"
# include "HashSet.hpp"
# include "Threading.hpp"
# include "FunctionRef.hpp"
# include "PredefinedNamedParameter.hpp"

namespace s3d
{
	template <class Type, class Allocator>
	class Array;

	template <class Type>
	concept ArrayLike = requires(Type&& t)
	{
		Array<typename std::remove_cvref_t<Type>::value_type, typename std::remove_cvref_t<Type>::allocator_type>{ std::forward<Type>(t) };
	};

	template <class Type>
	concept HasAsArray = requires(Type&& t)
	{
		{ std::forward<Type>(t).asArray() } -> ArrayLike;
	};

	////////////////////////////////////////////////////////////////
	//
	//	Array
	//
	////////////////////////////////////////////////////////////////

	/// @brief 動的配列
	/// @tparam Type 要素の型
	/// @tparam Allocator アロケータ
	template <class Type, class Allocator = std::allocator<Type>>
	class Array
	{
	public:

		using container_type			= std::vector<Type, Allocator>;

		/// @brief 配列の要素の型
		using value_type				= typename container_type::value_type;

		/// @brief 要素のポインタ型
		using pointer					= typename container_type::pointer;
		
		/// @brief 要素の const ポインタ型
		using const_pointer				= typename container_type::const_pointer;
		
		/// @brief 要素の参照型
		using reference					= typename container_type::reference;
		
		/// @brief 要素の const 参照型
		using const_reference			= typename container_type::const_reference;
		
		/// @brief イテレータ型
		using iterator					= typename container_type::iterator;
		
		/// @brief const イテレータ型
		using const_iterator			= typename container_type::const_iterator;
		
		/// @brief リバース・イテレータ型
		using reverse_iterator			= typename container_type::reverse_iterator;
		
		/// @brief const リバース・イテレータ型
		using const_reverse_iterator	= typename container_type::const_reverse_iterator;
		
		/// @brief 要素の個数を表現する型
		using size_type					= typename container_type::size_type;
		
		/// @brief 2 つの要素間の距離を表現する型
		using difference_type			= typename container_type::difference_type;
		
		/// @brief アロケータの型
		using allocator_type			= typename container_type::allocator_type;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Array() = default;

		/// @brief コピーコンストラクタ
		/// @param other コピーする配列
		[[nodiscard]]
		Array(const Array&) = default;

		/// @brief ムーブコンストラクタ
		/// @param other ムーブする配列
		[[nodiscard]]
		Array(Array&&) = default;

		/// @brief std::vector からのコピーで配列を作成します。
		/// @param other コピーする配列
		[[nodiscard]]
		constexpr Array(const container_type& other);

		/// @brief std::vector からのムーブで配列を作成します。
		/// @param other ムーブする配列
		[[nodiscard]]
		constexpr Array(container_type&& other);

		/// @brief アロケータを指定して空の配列を作成します。
		/// @param alloc アロケータ
		[[nodiscard]]
		explicit constexpr Array(const Allocator& alloc) noexcept;

		/// @brief count 個の value で配列を作成します。
		/// @param count 個数
		/// @param value 要素
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr Array(size_type count, const value_type& value, const Allocator& alloc = Allocator{});

		/// @brief count 個の Type() で配列を作成します。
		/// @param count 個数
		/// @param alloc アロケータ
		[[nodiscard]]
		explicit constexpr Array(size_type count, const Allocator& alloc = Allocator{});

		/// @brief イテレータが指す範囲の要素から配列を作成します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @param alloc アロケータ
		template <class Iterator>
		[[nodiscard]]
		constexpr Array(Iterator first, Iterator last, const Allocator& alloc = Allocator{});

		/// @brief アロケータを指定して、別の配列から配列を作成します。
		/// @param other コピーする配列
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr Array(const Array& other, const Allocator& alloc);

		/// @brief アロケータを指定して、別の配列から配列を作成します。
		/// @param other ムーブする配列
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr Array(Array&& other, const Allocator& alloc);

		/// @brief アロケータを指定して、std::vector から配列を作成します。
		/// @param other コピーする配列
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr Array(const container_type& other, const Allocator& alloc);

		/// @brief アロケータを指定して、std::vector から配列を作成します。
		/// @param other ムーブする配列
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr Array(container_type&& other, const Allocator& alloc);

		/// @brief メンバ関数 `.asArray()` を持つ型から配列を作成します。
		/// @tparam ArrayIsh メンバ関数 `.asArray()` を持つ型
		/// @param a `.asArray()` を持つ型のオブジェクト
		[[nodiscard]]
		explicit constexpr Array(const HasAsArray auto& a);

		/// @brief メンバ関数 `.asArray()` を持つ型から配列を作成します。
		/// @tparam ArrayIsh メンバ関数 `.asArray()` を持つ型
		/// @param a `.asArray()` を持つ型のオブジェクト
		[[nodiscard]]
		explicit constexpr Array(HasAsArray auto&& a);

		/// @brief 初期化リストから配列を作成します。
		/// @param list 初期化リスト
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr Array(std::initializer_list<value_type> list, const Allocator& alloc = Allocator{});

	# ifdef __cpp_lib_containers_ranges
		
		/// @brief 範囲から配列を作成します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		/// @param alloc アロケータ
		template <Concept::ContainerCompatibleRange<Type> Range>
		[[nodiscard]]
		constexpr Array(std::from_range_t, Range&& range, const Allocator& alloc = Allocator{});

	# endif
		
		/// @brief ジェネレータ関数を使って配列を作成します。
		/// @param size 作成する配列の要素数
		/// @param generator ジェネレータ関数
		[[nodiscard]]
		constexpr Array(size_type size, Arg::generator_<FunctionRef<value_type()>> generator);

		/// @brief インデックス指定ジェネレータ関数を使って配列を作成します。
		/// @param size 作成する配列の要素数
		/// @param generator インデックス指定ジェネレータ関数
		[[nodiscard]]
		constexpr Array(size_type size, Arg::generator_<FunctionRef<value_type(size_t)>> generator);

		/// @brief 配列を作成し、`reserve()` します。
		/// @param size `reserve()` するサイズ
		[[nodiscard]]
		explicit constexpr Array(Arg::reserve_<size_type> size);

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		/// @brief コピー代入演算子
		/// @param other コピーする配列
		/// @return *this
		Array& operator =(const Array&) = default;

		/// @brief ムーブ代入演算子
		/// @param other ムーブする配列
		/// @return *this
		Array& operator =(Array&&) = default;

		/// @brief コピー代入演算子
		/// @param other コピーする配列
		/// @return *this
		constexpr Array& operator =(const container_type& other);

		/// @brief ムーブ代入演算子
		/// @param other ムーブする配列
		/// @return *this
		constexpr Array& operator =(container_type&& other);

		/// @brief コピー代入演算子
		/// @param a メンバ関数 `.asArray()` を持つ型
		/// @return *this
		constexpr Array& operator =(const HasAsArray auto& a);

		/// @brief ムーブ代入演算子
		/// @param a メンバ関数 `.asArray()` を持つ型
		/// @return *this
		constexpr Array& operator =(HasAsArray auto&& a);

		/// @brief コピー代入演算子
		/// @param list リスト
		/// @return *this
		constexpr Array& operator =(std::initializer_list<value_type> list);

		////////////////////////////////////////////////////////////////
		//
		//	assign
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した個数の要素からなる配列を作成します。
		/// @param count 要素数
		/// @param value 要素の値
		/// @return *this
		constexpr Array& assign(size_type count, const value_type& value);

		/// @brief イテレータが指す範囲の要素から配列を作成します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return *this
		template <class Iterator>
		constexpr Array& assign(Iterator first, Iterator last);

		/// @brief リストから配列を作成します。
		/// @param list リスト
		/// @return *this
		constexpr Array& assign(std::initializer_list<value_type> list);

		////////////////////////////////////////////////////////////////
		//
		//	assign_range
		//
		////////////////////////////////////////////////////////////////

		/// @brief 範囲から配列を作成します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		/// @return *this
		template <Concept::ContainerCompatibleRange<Type> Range>
		constexpr Array& assign_range(Range&& range);

		////////////////////////////////////////////////////////////////
		//
		//	get_allocator
		//
		////////////////////////////////////////////////////////////////

		/// @brief アロケータを返します。
		/// @return アロケータ
		[[nodiscard]]
		constexpr allocator_type get_allocator() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getContainer
		//
		////////////////////////////////////////////////////////////////

		/// @brief std::vector への const 参照を返します。
		/// @return std::vector への const 参照
		[[nodiscard]]
		constexpr const container_type& getContainer() const& noexcept;

		/// @brief std::vector を返します。
		/// @return std::vector
		[[nodiscard]]
		constexpr container_type getContainer() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator container_type
		//
		////////////////////////////////////////////////////////////////

		/// @brief std::vector への暗黙の変換を行います。
		[[nodiscard]]
		constexpr operator container_type() const& noexcept;

		/// @brief std::vector への暗黙の変換を行います。
		[[nodiscard]]
		constexpr operator container_type() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		constexpr const value_type& at(size_type index) const&;

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		constexpr value_type& at(size_type index) &;

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		constexpr value_type at(size_type index) &&;

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		[[nodiscard]]
		constexpr const value_type& operator [](size_type index) const& noexcept;

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		[[nodiscard]]
		constexpr value_type& operator [](size_type index)& noexcept;

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素
		[[nodiscard]]
		constexpr value_type operator [](size_type index) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の要素への参照を返します。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		constexpr reference front()& noexcept;

		/// @brief 先頭の要素への参照を返します。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		constexpr const_reference front() const& noexcept;

		/// @brief 先頭の要素を返します。
		/// @return 先頭の要素
		[[nodiscard]]
		constexpr value_type front() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の要素への参照を返します。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		constexpr reference back()& noexcept;

		/// @brief 末尾の要素への参照を返します。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		constexpr const_reference back() const& noexcept;

		/// @brief 末尾の要素を返します。
		/// @return 末尾の要素
		[[nodiscard]]
		constexpr value_type back() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	data
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の要素を指すポインタを返します。
		/// @return 先頭の要素を指すポインタ
		[[nodiscard]]
		constexpr value_type* data() noexcept;

		/// @brief 先頭の要素を指すポインタを返します。
		/// @return 先頭の要素を指すポインタ
		[[nodiscard]]
		constexpr const value_type* data() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	begin, end
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭位置を指すイテレータを返します。
		/// @return 配列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr iterator begin() noexcept;

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr iterator end() noexcept;

		/// @brief 配列の先頭位置を指すイテレータを返します。
		/// @return 配列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator begin() const noexcept;

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator end() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	cbegin, cend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭位置を指すイテレータを返します。
		/// @return 配列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator cbegin() const noexcept;

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator cend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rbegin, rend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾位置を指すリバース・イテレータを返します。
		/// @return 配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr reverse_iterator rbegin() noexcept;

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr reverse_iterator rend() noexcept;

		/// @brief 配列の末尾位置を指すリバース・イテレータを返します。
		/// @return 配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rbegin() const noexcept;

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	crbegin, crend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾位置を指すリバース・イテレータを返します。
		/// @return 配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator crbegin() const noexcept;

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator crend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	empty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列が空であるかを返します。
		/// @return 配列が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool empty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列が空であるかを返します。
		/// @remark `empty()` と同じです。
		/// @return 配列が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列が要素を持っているかを返します。
		/// @return 配列が要素を持っている場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数を返します。
		/// @return 配列の要素数
		[[nodiscard]]
		constexpr size_t size() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	ssize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数を符号付き整数型で返します。 | Return the number of elements in the array as a signed integer type.
		/// @return 配列の要素数
		[[nodiscard]]
		constexpr isize ssize() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	size_bytes
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素の合計サイズ（バイト）を返します。
		/// @return 配列の要素の合計サイズ（バイト）
		[[nodiscard]]
		constexpr size_t size_bytes() const noexcept requires (Concept::TriviallyCopyable<value_type>);

		////////////////////////////////////////////////////////////////
		//
		//	max_size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数の最大値を返します。
		/// @return 配列の要素数の最大値
		[[nodiscard]]
		constexpr size_t max_size() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reserve
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素数 n までメモリの再確保無しで格納できるよう、必要に応じてメモリを再確保します。
		/// @param n メモリを確保する要素数
		constexpr void reserve(size_type n);

		////////////////////////////////////////////////////////////////
		//
		//	capacity
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリの再確保無しで格納できる要素数を返します。
		/// @return メモリの再確保無しで格納できる要素数
		[[nodiscard]]
		constexpr size_t capacity() const noexcept;

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

		/// @brief 配列の要素を削除し、空の配列にします。
		constexpr void clear() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	release
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素を全て消去し、メモリも解放します。
		constexpr void release();

		////////////////////////////////////////////////////////////////
		//
		//	insert
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に要素を挿入します。
		/// @param pos 挿入する位置
		/// @param value 挿入する値
		/// @return 挿入された要素を指すイテレータ
		constexpr iterator insert(const_iterator pos, const value_type& value);

		/// @brief 指定した位置に要素を挿入します。
		/// @param pos 挿入する位置
		/// @param value 挿入する値
		/// @return 挿入された要素を指すイテレータ
		constexpr iterator insert(const_iterator pos, value_type&& value);

		/// @brief 指定した位置に count 個の value を挿入します。
		/// @param pos 挿入する位置
		/// @param count 挿入する個数
		/// @param value 挿入する値
		/// @return 挿入された要素の先頭を指すイテレータ
		constexpr iterator insert(const_iterator pos, size_type count, const value_type& value);

		/// @brief 指定した位置にイテレータが指す範囲の要素を挿入します。
		/// @tparam Iterator イテレータ
		/// @param pos 挿入する位置
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return 挿入された要素の先頭を指すイテレータ
		template <class Iterator>
		constexpr iterator insert(const_iterator pos, Iterator first, Iterator last);

		/// @brief 指定した位置にリストの要素を挿入します。
		/// @param pos 挿入する位置
		/// @param list リスト
		/// @return 挿入された要素の先頭を指すイテレータ
		constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> list);

		////////////////////////////////////////////////////////////////
		//
		//	emplace
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に要素を構築して挿入します。
		/// @tparam ...Args 構築する要素の引数の型
		/// @param pos 挿入する位置
		/// @param ...args 構築する要素の引数
		/// @return 挿入された要素を指すイテレータ
		template <class... Args>
		constexpr iterator emplace(const_iterator pos, Args&&... args);

		////////////////////////////////////////////////////////////////
		//
		//	append_range
		//
		////////////////////////////////////////////////////////////////

		/// @brief 範囲の要素を配列の末尾に追加します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		template <Concept::ContainerCompatibleRange<Type> Range>
		constexpr void append_range(Range&& range);

		////////////////////////////////////////////////////////////////
		//
		//	erase
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の要素を削除します。
		/// @param pos 削除する要素の位置
		/// @return 削除した要素の次の要素を指すイテレータ
		constexpr iterator erase(const_iterator pos);

		/// @brief 指定した範囲の要素を削除します。
		/// @param first 削除する範囲の開始位置
		/// @param last 削除する範囲の終端位置
		/// @return 削除された範囲の次を指すイテレータ
		constexpr iterator erase(const_iterator first, const_iterator last);

		////////////////////////////////////////////////////////////////
		//
		//	push_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に要素を追加します。
		/// @param value 追加する値
		constexpr void push_back(const value_type& value);

		/// @brief 配列の末尾に要素を追加します。
		/// @param value 追加する値
		constexpr void push_back(value_type&& value);

		////////////////////////////////////////////////////////////////
		//
		//	emplace_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に要素を構築して追加します。
		/// @tparam ...Args 構築する要素の引数の型
		/// @param ...args 構築する要素の引数
		/// @return 追加された要素への参照
		template <class... Args>
		constexpr reference emplace_back(Args&&... args);

		////////////////////////////////////////////////////////////////
		//
		//	pop_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾の要素を削除します。
		/// @remark 配列が空の時に呼んではいけません。
		constexpr void pop_back();

		////////////////////////////////////////////////////////////////
		//
		//	resize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数を変更します。
		/// @param count 新しい要素数
		constexpr void resize(size_type count);

		/// @brief 配列の要素数を変更します。
		/// @param count 新しい要素数
		/// @param value 新しく増える要素の初期値
		constexpr void resize(size_type count, const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 他の配列と要素を入れ替えます。
		/// @param other 入れ替える配列
		constexpr void swap(Array& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	push_front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭に要素を追加します。
		/// @param value 追加する値
		constexpr void push_front(const value_type& value);

		/// @brief 配列の先頭に要素を追加します。
		/// @param value 追加する値
		constexpr void push_front(value_type&& value);

		////////////////////////////////////////////////////////////////
		//
		//	pop_front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭から要素を削除します。
		constexpr void pop_front();

		////////////////////////////////////////////////////////////////
		//
		//	pop_front_N
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭から指定した個数の要素を削除します。
		/// @param n 削除する個数
		/// @remark n が `size()` より多い場合は全ての要素を削除します。
		constexpr void pop_front_N(size_type n);

		////////////////////////////////////////////////////////////////
		//
		//	pop_back_N
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾から指定した個数の要素を削除します。
		/// @param n 削除する個数
		/// @remark n が `size()` より多い場合は全ての要素を削除します。
		constexpr void pop_back_N(size_type n);

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に要素を追加します。
		/// @param value 追加する値
		/// @return *this
		constexpr Array& operator <<(const value_type& value);

		/// @brief 配列の末尾に要素を追加します。
		/// @param value 追加する値
		/// @return *this
		constexpr Array& operator <<(value_type&& value);

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
		constexpr std::span<value_type> subspan(size_type pos, size_type count) noexcept;

		/// @brief 部分配列を指す span を返します。
		/// @param pos 部分配列の開始位置
		/// @param count 部分配列の要素数
		/// @return 部分配列を指す span
		[[nodiscard]]
		constexpr std::span<const value_type> subspan(size_type pos, size_type count) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	all
		//
		////////////////////////////////////////////////////////////////

		/// @brief 全ての要素が条件を満たすかを返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 全ての要素が条件を満たすか、配列が空の場合 true, それ以外の場合は false
		template <class Fty = decltype(Identity)>
		[[nodiscard]]
		constexpr bool all(Fty f = Identity) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	any
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす要素があるかを返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 条件を満たす要素が 1 つでもあれば true, それ以外の場合は false
		template <class Fty = decltype(Identity)>
		[[nodiscard]]
		constexpr bool any(Fty f = Identity) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に別の配列を追加します。
		/// @param other 追加する配列
		/// @return *this
		constexpr Array& append(const Array& other);

		/// @brief 配列の末尾に別の範囲の要素を追加します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return *this
		template <class Iterator>
		constexpr Array& append(Iterator first, Iterator last);

		/// @brief 配列の末尾にリストの要素を追加します。
		/// @param list リスト
		/// @return *this
		constexpr Array& append(std::initializer_list<value_type> list);

		/// @brief 配列の末尾に要素を追加します。
		/// @param count 追加する個数
		/// @param value 追加する値
		/// @return *this
		constexpr Array& append(size_type count, const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	choice
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素を 1 つランダムに返します。
		/// @return 配列からランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type& choice();

		/// @brief 配列の要素を 1 つランダムに返します。
		/// @return 配列からランダムに選ばれた要素への参照
		[[nodiscard]]
		const value_type& choice() const;

		/// @brief 指定した乱数エンジンを用いて、配列の要素を 1 つランダムに返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 配列からランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type& choice(Concept::UniformRandomBitGenerator auto&& rbg);

		/// @brief 指定した乱数エンジンを用いて、配列の要素を 1 つランダムに返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 配列からランダムに選ばれた要素への参照
		[[nodiscard]]
		const value_type& choice(Concept::UniformRandomBitGenerator auto&& rbg) const;

		/// @brief 配列の要素から指定した個数だけ重複なくランダムに選んで返します。
		/// @param n 選択する個数
		/// @return ランダムに選ばれた要素の配列
		[[nodiscard]]
		Array choice(Concept::Integral auto n) const;

		/// @brief 指定した乱数エンジンを用いて、配列の要素から指定した個数だけ重複なくランダムに選んで返します。
		/// @param n 選択する個数
		/// @param rbg 使用する乱数エンジン
		/// @return ランダムに選ばれた要素の配列
		[[nodiscard]]
		Array choice(Concept::Integral auto n, Concept::UniformRandomBitGenerator auto&& rbg) const;

		////////////////////////////////////////////////////////////////
		//
		//	chunk
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した個数の要素を持つ配列のグループに分割します。最後のグループの要素数は n 個未満になることがあります。
		/// @param n 1 つのグループが持つ要素数
		/// @remark { 0, 1, 2, 3, 4, 5, 6 } を 3 個の要素を持つ配列のグループに分割すると { { 0, 1, 2 }, { 3, 4, 5 }, { 6 }} になります。
		/// @return 分割されたグループ
		[[nodiscard]]
		constexpr Array<Array<value_type>> chunk(size_type n) const;

		////////////////////////////////////////////////////////////////
		//
		//	contains
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しい要素があるかを返します。
		/// @param value 検索する値
		/// @return 指定した値と等しい要素がある場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool contains(const value_type& value) const;

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
		constexpr bool contains_if(Fty f) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	count
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しい要素の個数を返します。
		/// @param value 検索する値
		/// @return 指定した値と等しい要素の個数
		[[nodiscard]]
		constexpr isize count(const value_type& value) const;

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
		constexpr isize count_if(Fty f) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	each
		//
		////////////////////////////////////////////////////////////////

		/// @brief 全ての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (auto& x : xs) f(x);` と同じです。
		template <class Fty>
		constexpr void each(Fty f) requires std::invocable<Fty&, value_type&>;

		/// @brief 全ての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (const auto& x : xs) f(x);` と同じです。
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
		/// @remark `for (size_t i = 0; auto& x : xs) f(i++, x);` と同じです。
		template <class Fty>
		constexpr void each_index(Fty f) requires std::invocable<Fty&, size_t, value_type&>;

		/// @brief 全ての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (size_t i = 0; const auto& x : xs) f(i++, x);` と同じです。
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
		/// @remark `for (isize i = 0; auto& x : xs) f(i++, x);` と同じです。
		template <class Fty>
		constexpr void each_sindex(Fty f) requires std::invocable<Fty&, isize, value_type&>;

		/// @brief 全ての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (isize i = 0; auto x : xs) f(i++, x);` と同じです。
		template <class Fty>
		constexpr void each_sindex(Fty f) const requires std::invocable<Fty&, isize, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	fetch
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにある要素を返します。インデックスが範囲外の場合デフォルト値を返します。
		/// @param index インデックス
		/// @param defaultValue インデックスが範囲外の場合に返すデフォルト値
		/// @return 指定したインデックスにある要素、範囲外の場合 defaultValue
		template <class U>
		[[nodiscard]]
		constexpr value_type fetch(size_type index, U&& defaultValue) const noexcept(std::is_nothrow_constructible_v<value_type, U>) requires std::constructible_from<value_type, U>;

		////////////////////////////////////////////////////////////////
		//
		//	fill
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値を全ての要素に代入します。
		/// @param value 代入する値
		/// @return *this
		constexpr Array& fill(const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	filter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素だけを集めた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @remark 結果において、要素の前後関係は維持されます。
		/// @return 指定した条件を満たす要素を集めた新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array filter(Fty f) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	in_groups
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を指定したグループ数に分割します。
		/// @param group グループ数
		/// @remark { 0, 1, 2, 3, 4, 5, 6 } を 3 グループに分割すると { { 0, 1, 2 }, { 3, 4 }, { 5, 6 }} になります。
		/// @return 分割したグループ
		[[nodiscard]]
		constexpr Array<Array<value_type>> in_groups(size_type group) const;

		////////////////////////////////////////////////////////////////
		//
		//	isSorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素が昇順にソートされているかを返します。
		/// @return 配列の要素が昇順にソートされている場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isSorted() const requires Concept::LessThanComparable<value_type>;

		////////////////////////////////////////////////////////////////
		//
		//	join
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素から文字列を生成します。
		/// @param sep 要素の間に挿入する文字列
		/// @param begin 先頭に挿入する文字列
		/// @param end 末尾に挿入する文字列
		/// @return 生成された文字列
		[[nodiscard]]
		constexpr String join(StringView sep = U", ", StringView begin = U"{", StringView end = U"}") const;

		////////////////////////////////////////////////////////////////
		//
		//	map
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各要素に関数を適用した戻り値からなる新しい配列を返します。
		/// @tparam Fty 各要素に適用する関数の型
		/// @param f 各要素に適用する関数
		/// @return 各要素に関数を適用した戻り値からなる新しい配列
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
		template <class Fty = decltype(Identity)>
		[[nodiscard]]
		constexpr bool none(Fty f = Identity) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	partition
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たすすべての要素を、条件を満たさないすべての要素より前に移動させます。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 区分化された境界を指すイテレータ
		template <class Fty>
		constexpr auto partition(Fty f) requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	reduce
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素を左から順に関数に適用していき、1 つの値にまとめます。
		/// @tparam Fty 関数の型
		/// @tparam R 結果の型
		/// @param f 関数
		/// @param init 初期値
		/// @return まとめられた値
		template <class Fty, class R>
		constexpr auto reduce(Fty f, R init) const;

		////////////////////////////////////////////////////////////////
		//
		//	remove, removed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しい全ての要素を削除します。
		/// @param value 削除する値
		/// @return *this
		constexpr Array& remove(const value_type& value)&;

		/// @brief 指定した値と等しい全ての要素を削除した新しい配列を返します。
		/// @param value 削除する値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array remove(const value_type& value) &&;

		/// @brief 指定した値と等しい全ての要素を削除した新しい配列を返します。
		/// @param value 削除する値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array removed(const value_type& value) const&;

		/// @brief 指定した値と等しい全ての要素を削除した新しい配列を返します。
		/// @param value 削除する値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array removed(const value_type& value)&&;

		////////////////////////////////////////////////////////////////
		//
		//	remove_at, removed_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにある要素を削除します。
		/// @param index インデックス
		/// @return *this
		constexpr Array& remove_at(size_type index)&;

		/// @brief 指定したインデックスにある要素を削除した新しい配列を返します。
		/// @param index インデックス
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array remove_at(size_type index)&&;

		/// @brief 指定したインデックスにある要素を削除した新しい配列を返します。
		/// @param index インデックス
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array removed_at(size_type index) const&;

		/// @brief 指定したインデックスにある要素を削除した新しい配列を返します。
		/// @param index インデックス
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array removed_at(size_type index)&&;

		////////////////////////////////////////////////////////////////
		//
		//	remove_if, removed_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす要素を配列から削除します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return *this
		template <class Fty>
		constexpr Array& remove_if(Fty f)& requires std::predicate<Fty&, const value_type&>;

		/// @brief 条件を満たす要素を配列から削除した新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array remove_if(Fty f)&& requires std::predicate<Fty&, const value_type&>;

		/// @brief 条件を満たす要素を配列から削除した新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array removed_if(Fty f) const& requires std::predicate<Fty&, const value_type&>;

		/// @brief 条件を満たす要素を配列から削除した新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array removed_if(Fty f)&& requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	replace, replaced
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しい全ての要素を別の値に置き換えます。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return *this
		constexpr Array& replace(const value_type& oldValue, const value_type& newValue)&;

		/// @brief 指定した値と等しい全ての要素を別の値に置き換えた新しい配列を返します。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array replace(const value_type& oldValue, const value_type& newValue)&&;

		/// @brief 指定した値と等しい全ての要素を別の値に置き換えた新しい配列を返します。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array replaced(const value_type& oldValue, const value_type& newValue) const&;

		/// @brief 指定した値と等しい全ての要素を別の値に置き換えた新しい配列を返します。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array replaced(const value_type& oldValue, const value_type& newValue)&&;

		////////////////////////////////////////////////////////////////
		//
		//	replace_if, replaced_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす全ての要素を別の値に置き換えます。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return *this
		template <class Fty>
		constexpr Array& replace_if(Fty f, const value_type& newValue)& requires std::predicate<Fty&, const value_type&>;
		
		/// @brief 指定した条件を満たす全ての要素を別の値に置き換えた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array replace_if(Fty f, const value_type& newValue)&& requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす全ての要素を別の値に置き換えた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array replaced_if(Fty f, const value_type& newValue) const& requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たす全ての要素を別の値に置き換えた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array replaced_if(Fty f, const value_type& newValue)&& requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	reverse, reversed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素を逆順に並び替えます。
		/// @return *this
		constexpr Array& reverse()&;

		/// @brief 配列の要素を逆順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array reverse()&&;

		/// @brief 配列の要素を逆順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array reversed() const&;

		/// @brief 配列の要素を逆順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array reversed()&&;

		////////////////////////////////////////////////////////////////
		//
		//	reverse_each
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾から順番に、全ての要素に対して関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (auto& x : xs) f(x);` と同じです。
		template <class Fty>
		constexpr void reverse_each(Fty f) requires std::invocable<Fty&, value_type&>;

		/// @brief 末尾から順番に、全ての要素に対して関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (const auto& x : xs) f(x);` と同じです。
		template <class Fty>
		constexpr void reverse_each(Fty f) const requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	rotate, rotated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を境に配列の前半と後半を入れ替えます。
		/// @param middle 境の位置
		/// @return *this
		constexpr Array& rotate(size_type middle)&;

		/// @brief 指定した位置を境に配列の前半と後半を入れ替えた新しい配列を返します。
		/// @param middle 境の位置
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array rotate(size_type middle)&&;

		/// @brief 指定した位置を境に配列の前半と後半を入れ替えた新しい配列を返します。
		/// @param middle 境の位置
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array rotated(size_type middle) const&;

		/// @brief 指定した位置を境に配列の前半と後半を入れ替えた新しい配列を返します。
		/// @param middle 境の位置
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array rotated(size_type middle)&&;

		////////////////////////////////////////////////////////////////
		//
		//	rsort, rsorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を降順に並び替えます。
		/// @return *this
		constexpr Array& rsort()& requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を降順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array rsort() && requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を降順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array rsorted() const& requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を降順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array rsorted() && requires Concept::LessThanComparable<value_type>;

		////////////////////////////////////////////////////////////////
		//
		//	shuffle, shuffled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素の並び順をランダムにシャッフルします。
		/// @return *this
		constexpr Array& shuffle()&;

		/// @brief 配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array shuffle()&&;

		/// @brief 配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array shuffled() const&;

		/// @brief 配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array shuffled()&&;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルします。
		/// @param rbg 使用する乱数エンジン
		/// @return *this
		constexpr Array& shuffle(Concept::UniformRandomBitGenerator auto&& rbg)&;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array shuffle(Concept::UniformRandomBitGenerator auto&& rbg)&&;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array shuffled(Concept::UniformRandomBitGenerator auto&& rbg) const&;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array shuffled(Concept::UniformRandomBitGenerator auto&& rbg)&&;

		////////////////////////////////////////////////////////////////
		//
		//	slice
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した範囲の要素からなる新しい配列を返します。
		/// @param index インデックス
		/// @param length 長さ
		/// @return 新しい配列
		/// @remark `Array((v.begin() + index), (v.begin() + index + length))` と同じです。
		[[nodiscard]]
		constexpr Array slice(size_type index, size_type length) const&;

		/// @brief 指定した範囲の要素からなる新しい配列を返します。
		/// @param index インデックス
		/// @param length 長さ
		/// @return 新しい配列
		/// @remark `Array((v.begin() + index), (v.begin() + index + length))` と同じです。
		[[nodiscard]]
		constexpr Array slice(size_type index, size_type length) &&;

		////////////////////////////////////////////////////////////////
		//
		//	sort, sorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を昇順に並び替えます。
		/// @return *this
		constexpr Array& sort()& requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sort()&& requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sorted() const& requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sorted()&& requires Concept::LessThanComparable<value_type>;

		////////////////////////////////////////////////////////////////
		//
		//	sort_and_unique, sorted_and_uniqued
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列をソートしたあとに重複する要素を削除します。
		/// @return *this
		constexpr Array& sort_and_unique() & noexcept;

		/// @brief 配列をソートしたあとに重複する要素を削除した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sort_and_unique() && noexcept;

		/// @brief 配列をソートしたあとに重複する要素を削除した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sorted_and_uniqued() const&;

		/// @brief 配列をソートしたあとに重複する要素を削除した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sorted_and_uniqued() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	sort_by, sorted_by
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した関数を用いて要素を昇順に並び替えます。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return *this
		template <class Fty>
		constexpr Array& sort_by(Fty f)& requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array sort_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array sorted_by(Fty f) const& requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array sorted_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	stable_sort, stable_sorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を相対順序を保ちながら昇順に並び替えます。
		/// @return *this
		constexpr Array& stable_sort()& requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array stable_sort() && requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array stable_sorted() const& requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array stable_sorted() && requires Concept::LessThanComparable<value_type>;

		////////////////////////////////////////////////////////////////
		//
		//	stable_sort_by, stable_sorted_by
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えます。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return *this
		template <class Fty>
		constexpr Array& stable_sort_by(Fty f)& requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array stable_sort_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array stable_sorted_by(Fty f) const& requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array stable_sorted_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	stable_partition
		//
		////////////////////////////////////////////////////////////////

		/// @brief 相対順序を保ちながら、条件を満たすすべての要素を、条件を満たさないすべての要素より前に移動させます。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 区分化された境界を指すイテレータ
		template <class Fty>
		constexpr auto stable_partition(Fty f) requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	stable_unique, stable_uniqued
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素をソートせずに、重複する要素を削除します。
		/// @return *this
		constexpr Array& stable_unique() & noexcept;

		/// @brief 要素をソートせずに、重複する要素を削除した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array stable_unique() && noexcept;

		/// @brief 要素をソートせずに、重複する要素を削除した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array stable_uniqued() const;

		////////////////////////////////////////////////////////////////
		//
		//	sum
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を `+` 演算子を用いて合計します。
		/// @return 合計値
		[[nodiscard]]
		constexpr auto sum() const requires (Concept::Addable<value_type> || Concept::AddAssignable<value_type>);

		////////////////////////////////////////////////////////////////
		//
		//	sumF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 浮動小数点数型の要素を、誤差が小さくなるように合計します。
		/// @remark `sum()` よりも浮動小数点数誤差が小さくなります。
		/// @return 合計値
		[[nodiscard]]
		constexpr auto sumF() const requires Concept::FloatingPoint<value_type>;

		////////////////////////////////////////////////////////////////
		//
		//	take
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から指定した個数の要素からなる新しい配列を返します。
		/// @param n 取り出す要素数
		/// @return 新しい配列
		/// @remark n が配列の要素数以上の場合、配列全体を返します。
		/// @remark `Array(begin(), (begin() + Min(n, size())))` と同じです。
		[[nodiscard]]
		constexpr Array take(size_type n) const&;

		/// @brief 先頭から指定した個数の要素からなる新しい配列を返します。
		/// @param n 取り出す要素数
		/// @return 新しい配列
		/// @remark n が配列の要素数以上の場合、配列全体を返します。
		/// @remark `Array(begin(), (begin() + Min(n, size())))` と同じです。
		[[nodiscard]]
		constexpr Array take(size_type n)&&;

		////////////////////////////////////////////////////////////////
		//
		//	take_while
		//
		////////////////////////////////////////////////////////////////
	
		/// @brief 先頭から、条件を満たさなくなる直前までの要素からなる新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array take_while(Fty f) const& requires std::predicate<Fty&, const value_type&>;

		/// @brief 先頭から、条件を満たさなくなる直前までの要素からなる新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array take_while(Fty f) && requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	unique_consecutive, uniqued_consecutive
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ要素が連続する場合、その先頭以外を除去します。
		/// @return *this
		constexpr Array& unique_consecutive() & noexcept;

		/// @brief 同じ要素が連続する場合、その先頭以外を除去した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array unique_consecutive() && noexcept;

		/// @brief 同じ要素が連続する場合、その先頭以外を除去した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array uniqued_consecutive() const&;

		/// @brief 同じ要素が連続する場合、その先頭以外を除去した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array uniqued_consecutive() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	values_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの要素からなる新しい配列を返します。
		/// @param indices インデックス
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array values_at(std::initializer_list<size_type> indices) const;

		////////////////////////////////////////////////////////////////
		//
		//	parallel_count_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす要素の個数を返します（並列実行）。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 条件を満たす要素の個数
		template <class Fty>
		[[nodiscard]]
		isize parallel_count_if(Fty f) const requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	parallel_each
		//
		////////////////////////////////////////////////////////////////

		/// @brief 全ての要素に対して関数を並列実行します。
		/// @tparam Fty 関数の型
		/// @param f 関数
		template <class Fty>
		void parallel_each(Fty f) requires std::invocable<Fty&, value_type&>;

		/// @brief 全ての要素に対して関数を並列実行します。
		/// @tparam Fty 関数の型
		/// @param f 関数
		template <class Fty>
		void parallel_each(Fty f) const requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	parallel_map
		//
		////////////////////////////////////////////////////////////////

		/// @brief 全ての要素に対して関数を適用した結果からなる新しい配列を返します（並列実行）。
		/// @tparam Fty 関数の型
		/// @param f 関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		auto parallel_map(Fty f) const requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各要素に関数を適用します。
		/// @tparam Fty 適用する関数の型
		/// @param f 適用する関数
		/// @remark Fty が戻り値を持たない場合 `.each(f), 戻り値を持つ場合は `.map(f)` と同じです。
		/// @return 各要素に関数を適用した結果の配列。Fty が戻り値を持たない場合 void
		template <class Fty>
		constexpr auto operator >>(Fty f) const requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Array& value)
		{
			return output << Format(value);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Generate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 関数を用いて配列を生成します。
		/// @param size 生成する配列の要素数
		/// @param generator 生成に使用する関数
		/// @return 生成した配列
		[[nodiscard]]
		static constexpr Array Generate(size_type size, FunctionRef<value_type()> generator);

		////////////////////////////////////////////////////////////////
		//
		//	IndexedGenerate
		//
		////////////////////////////////////////////////////////////////

		/// @brief インデックスと関数を用いて配列を生成します。
		/// @param size 生成する配列の要素数
		/// @param generator 生成に使用する関数
		/// @return 生成した配列
		[[nodiscard]]
		static constexpr Array IndexedGenerate(size_type size, FunctionRef<value_type(size_t)> generator);

		////////////////////////////////////////////////////////////////
		//
		//	operator ==, operator <=>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの配列が等しいかを返します。
		/// @param lhs 一方の配列
		/// @param rhs もう一方の配列
		/// @return 2 つの配列が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const Array& lhs, const Array& rhs)
		{
			return (lhs.m_container == rhs.m_container);
		}

		/// @brief 2 つの配列の三方比較を行います。
		/// @param lhs 一方の配列
		/// @param rhs もう一方の配列
		/// @return 三方比較の結果
		[[nodiscard]]
		friend constexpr auto operator <=>(const Array& lhs, const Array& rhs)
		{
			return (lhs.m_container <=> rhs.m_container);
		}

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの配列を入れ替えます。
		/// @param lhs 一方の配列
		/// @param rhs もう一方の配列
		friend constexpr void swap(Array& lhs, Array& rhs) noexcept
		{
			lhs.swap(rhs);
		}

	private:

		container_type m_container;

		[[noreturn]]
		static void ThrowValuesAtOutOfRange();
	};

	////////////////////////////////////////////////////////////////
	//
	//	ToArray
	//
	////////////////////////////////////////////////////////////////

	/// @brief range から Array を作成します。
	/// @tparam Range range の型
	/// @tparam Elem range の要素の型
	/// @param range range
	/// @return 作成した Array
	template <class Range, class Elem = std::ranges::range_value_t<Range>>
	constexpr Array<Elem> ToArray(Range&& range) requires Concept::ContainerCompatibleRange<Range, Elem>;

	////////////////////////////////////////////////////////////////
	//
	//	(deduction guide)
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator = std::allocator<Type>>
	Array(size_t count, Type value, Allocator alloc = Allocator{})
		-> Array<Type, Allocator>;

	template <class Type, class Allocator = std::allocator<Type>>
	Array(std::initializer_list<Type>, Allocator = Allocator{})
		-> Array<Type, Allocator>;

	template <HasAsArray ArrayIsh>
	Array(ArrayIsh&&)
		-> Array<typename std::remove_cvref_t<decltype(std::declval<ArrayIsh>().asArray())>::value_type, typename std::remove_cvref_t<decltype(std::declval<ArrayIsh>().asArray())>::allocator_type>;

	template <class Iterator, class Allocator = std::allocator<typename std::iterator_traits<Iterator>::value_type>>
	Array(Iterator, Iterator, Allocator = Allocator{})
		-> Array<typename std::iterator_traits<Iterator>::value_type, Allocator>;

# ifdef __cpp_lib_containers_ranges
	   
	template<std::ranges::input_range Range, class Allocator = std::allocator<std::ranges::range_value_t<Range>>>
	Array(std::from_range_t, Range&&, Allocator = Allocator{})
		-> Array<std::ranges::range_value_t<Range>, Allocator>;

# endif
}

# include "detail/Array.ipp"
# include "detail/BoolArray.ipp"
