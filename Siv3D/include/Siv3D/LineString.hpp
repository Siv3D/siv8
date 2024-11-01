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
# include "Common.hpp"
# include "PointVector.hpp"
# include "ColorHSV.hpp"
# include "Array.hpp"
# include "2DShapes.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	LineString
	//
	////////////////////////////////////////////////////////////////

	class LineString
	{
	public:

		using container_type			= Array<Vec2>;

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
		LineString() = default;

		/// @brief コピーコンストラクタ
		/// @param other コピーする配列
		[[nodiscard]]
		LineString(const LineString& other) = default;

		/// @brief ムーブコンストラクタ
		/// @param other ムーブする配列
		[[nodiscard]]
		LineString(LineString&& other) = default;

		/// @brief std::vector からのコピーで点の配列を作成します。
		/// @param other コピーする配列
		[[nodiscard]]
		constexpr LineString(const container_type& other);

		/// @brief std::vector からのムーブで点の配列を作成します。
		/// @param other ムーブする配列
		[[nodiscard]]
		constexpr LineString(container_type&& other);

		/// @brief count 個の value からなる点の配列を作成します。
		/// @param count 個数
		/// @param value 要素
		[[nodiscard]]
		constexpr LineString(size_type count, const value_type& value);

		/// @brief count 個の Vec2{ 0, 0 } からなる点の配列を作成します。
		/// @param count 個数
		[[nodiscard]]
		explicit constexpr LineString(size_type count);

		/// @brief イテレータが指す範囲の要素から点の配列を作成します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		template <class Iterator>
		[[nodiscard]]
		constexpr LineString(Iterator first, Iterator last);

		/// @brief 点の配列を作成します。
		/// @param points 
		[[nodiscard]]
		explicit constexpr LineString(const Array<Point>& points);

		/// @brief メンバ関数 `.asArray()` を持つ型から点の配列を作成します。
		/// @tparam ArrayIsh メンバ関数 `.asArray()` を持つ型
		/// @param a `.asArray()` を持つ型のオブジェクト
		[[nodiscard]]
		explicit constexpr LineString(const HasAsArray auto& a);

		/// @brief メンバ関数 `.asArray()` を持つ型から点の配列を作成します。
		/// @tparam ArrayIsh メンバ関数 `.asArray()` を持つ型
		/// @param a `.asArray()` を持つ型のオブジェクト
		[[nodiscard]]
		explicit constexpr LineString(HasAsArray auto&& a);

		/// @brief 初期化リストから点の配列を作成します。
		/// @param list 初期化リスト
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr LineString(std::initializer_list<value_type> list);

	# ifdef __cpp_lib_containers_ranges
		
		/// @brief 範囲から点の配列を作成します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		/// @param alloc アロケータ
		template <Concept::ContainerCompatibleRange<Vec2> Range>
		[[nodiscard]]
		constexpr LineString(std::from_range_t, Range&& range);

		/// @brief 範囲から点の配列を作成します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		/// @param alloc アロケータ
		template <Concept::ContainerCompatibleRange<Point> Range>
		[[nodiscard]]
		constexpr LineString(std::from_range_t, Range&& range);

	# endif

		/// @brief 空の点の配列を作成し、`reserve()` します。
		/// @param size `reserve()` するサイズ
		[[nodiscard]]
		explicit constexpr LineString(Arg::reserve_<size_type> size);

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		/// @brief コピー代入演算子
		/// @param other コピーする配列
		/// @return *this
		LineString& operator =(const LineString& other) = default;

		/// @brief ムーブ代入演算子
		/// @param other ムーブする配列
		/// @return *this
		LineString& operator =(LineString&& other) = default;

		/// @brief コピー代入演算子
		/// @param other コピーする配列
		/// @return *this
		constexpr LineString& operator =(const container_type& other);

		/// @brief ムーブ代入演算子
		/// @param other ムーブする配列
		/// @return *this
		constexpr LineString& operator =(container_type&& other);

		/// @brief コピー代入演算子
		/// @param other コピーする配列
		/// @return *this
		constexpr LineString& operator =(const Array<Point>& other);

		/// @brief コピー代入演算子
		/// @param a メンバ関数 `.asArray()` を持つ型
		/// @return *this
		constexpr LineString& operator =(const HasAsArray auto& a);

		/// @brief ムーブ代入演算子
		/// @param a メンバ関数 `.asArray()` を持つ型
		/// @return *this
		constexpr LineString& operator =(HasAsArray auto&& a);

		/// @brief コピー代入演算子
		/// @param list リスト
		/// @return *this
		constexpr LineString& operator =(std::initializer_list<value_type> list);

		////////////////////////////////////////////////////////////////
		//
		//	assign
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した個数の要素からなる配列を作成します。
		/// @param count 要素数
		/// @param value 要素の値
		/// @return *this
		constexpr LineString& assign(size_type count, const value_type& value);

		/// @brief イテレータが指す範囲の要素から配列を作成します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return *this
		template <class Iterator>
		constexpr LineString& assign(Iterator first, Iterator last);

		/// @brief リストから配列を作成します。
		/// @param list リスト
		/// @return *this
		constexpr LineString& assign(std::initializer_list<value_type> list);

		////////////////////////////////////////////////////////////////
		//
		//	assign_range
		//
		////////////////////////////////////////////////////////////////

		/// @brief 範囲から配列を作成します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		/// @return *this
		template <Concept::ContainerCompatibleRange<Vec2> Range>
		constexpr LineString& assign_range(Range&& range);

		/// @brief 範囲から配列を作成します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		/// @return *this
		template <Concept::ContainerCompatibleRange<Point> Range>
		constexpr LineString& assign_range(Range&& range);

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
		//	asArray
		//
		////////////////////////////////////////////////////////////////

		/// @brief Array への const 参照を返します。
		/// @return Array への const 参照
		[[nodiscard]]
		constexpr const container_type& asArray() const& noexcept;

		/// @brief Array を返します。
		/// @return Array
		[[nodiscard]]
		constexpr container_type asArray() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator container_type
		//
		////////////////////////////////////////////////////////////////

		/// @brief Array への暗黙の変換を行います。
		[[nodiscard]]
		constexpr operator container_type() const& noexcept;

		/// @brief Array への暗黙の変換を行います。
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
		constexpr value_type& at(size_type index)&;

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		constexpr value_type at(size_type index)&&;

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
		constexpr value_type& operator [](size_type index) & noexcept;

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
		constexpr reference front() & noexcept;

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
		constexpr reference back() & noexcept;

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
		constexpr size_t size_bytes() const noexcept;

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

		/// @brief 配列の要素をすべて消去し、メモリも解放します。
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
		template <Concept::ContainerCompatibleRange<Vec2> Range>
		constexpr void append_range(Range&& range);

		/// @brief 範囲の要素を配列の末尾に追加します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		template <Concept::ContainerCompatibleRange<Point> Range>
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
		constexpr void swap(LineString& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	push_front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭に要素を追加します。
		/// @param value 追加する値
		constexpr void push_front(const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	emplace_front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭に要素を構築して追加します。
		/// @tparam ...Args 構築する要素の引数の型
		/// @param ...args 構築する要素の引数
		/// @return 追加された要素への参照
		template <class... Args>
		constexpr reference emplace_front(Args&&... args);

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
		/// @remark n が `size()` より多い場合はすべての要素を削除します。
		constexpr void pop_front_N(size_type n);

		////////////////////////////////////////////////////////////////
		//
		//	pop_back_N
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾から指定した個数の要素を削除します。
		/// @param n 削除する個数
		/// @remark n が `size()` より多い場合はすべての要素を削除します。
		constexpr void pop_back_N(size_type n);

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に要素を追加します。
		/// @param value 追加する値
		/// @return *this
		constexpr LineString& operator <<(const value_type& value);

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
		//	hash
		//
		////////////////////////////////////////////////////////////////
	
		/// @brief 配列のハッシュ値を返します。
		/// @return 配列のハッシュ値
		[[nodiscard]]
		uint64 hash() const noexcept;




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
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const LineString& value)
		{
			return output << Format(value.m_points);
		}

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		const LineString& draw(const ColorF& color = Palette::White) const;

		const LineString& draw(const ColorF& colorStart, const ColorF& colorEnd) const;

		const LineString& draw(const PatternParameters& pattern) const;

		const LineString& draw(double thickness, const ColorF& color = Palette::White) const;

		const LineString& draw(double thickness, const ColorF& colorStart, const ColorF& colorEnd) const;

		const LineString& draw(double thickness, const PatternParameters& pattern) const;

		const LineString& draw(LineCap lineCap, double thickness, const ColorF& color = Palette::White) const;

		const LineString& draw(LineCap lineCap, double thickness, const ColorF& colorStart, const ColorF& colorEnd) const;

		const LineString& draw(LineCap lineCap, double thickness, const PatternParameters& pattern) const;

		const LineString& draw(LineCap startCap, LineCap endCap, double thickness, const ColorF& color = Palette::White) const;

		const LineString& draw(LineCap startCap, LineCap endCap, double thickness, const ColorF& colorStart, const ColorF& colorEnd) const;

		const LineString& draw(LineCap startCap, LineCap endCap, double thickness, const PatternParameters& pattern) const;

		const LineString& draw(std::span<const ColorF> colors) const;

		const LineString& draw(double thickness, std::span<const ColorF> colors) const;

		const LineString& draw(LineCap lineCap, double thickness, std::span<const ColorF> colors) const;

		const LineString& draw(LineCap startCap, LineCap endCap, double thickness, std::span<const ColorF> colors) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawClosed
		//
		////////////////////////////////////////////////////////////////

		const LineString& drawClosed(const ColorF& color = Palette::White) const;

		const LineString& drawClosed(const ColorF& colorStart, const ColorF& colorEnd) const;

		const LineString& drawClosed(const PatternParameters& pattern) const;

		const LineString& drawClosed(double thickness, const ColorF& color = Palette::White) const;

		const LineString& drawClosed(double thickness, const ColorF& colorStart, const ColorF& colorEnd) const;

		const LineString& drawClosed(double thickness, const PatternParameters& pattern) const;

		const LineString& drawClosed(std::span<const ColorF> colors) const;

		const LineString& drawClosed(double thickness, std::span<const ColorF> colors) const;


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
		static LineString Generate(size_type size, FunctionRef<value_type()> generator);

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
		static LineString IndexedGenerate(size_type size, FunctionRef<value_type(size_t)> generator);

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの配列が等しいかを返します。
		/// @param lhs 一方の配列
		/// @param rhs もう一方の配列
		/// @return 2 つの配列が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const LineString& lhs, const LineString& rhs)
		{
			return (lhs.m_points == rhs.m_points);
		}

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの配列を入れ替えます。
		/// @param lhs 一方の配列
		/// @param rhs もう一方の配列
		friend constexpr void swap(LineString& lhs, LineString& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const LineString& value);

	private:

		container_type m_points;
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::LineString, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::LineString& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::LineString>
{
	[[nodiscard]]
	size_t operator ()(const s3d::LineString& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/LineString.ipp"
