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
# include "Common.hpp"
# include "PointVector.hpp"
# include "ColorHSV.hpp"
# include "Array.hpp"
# include "ArrayAlgorithm.hpp"
# include "ArrayRandom.hpp"
# include "2DShapes.hpp"
# include "PredefinedYesNo.hpp"
# include "RangeFormatter.hpp"

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
		using value_type				= container_type::value_type;

		/// @brief 要素のポインタ型
		using pointer					= container_type::pointer;
		
		/// @brief 要素の const ポインタ型
		using const_pointer				= container_type::const_pointer;
		
		/// @brief 要素の参照型
		using reference					= container_type::reference;
		
		/// @brief 要素の const 参照型
		using const_reference			= container_type::const_reference;
		
		/// @brief イテレータ型
		using iterator					= container_type::iterator;
		
		/// @brief const イテレータ型
		using const_iterator			= container_type::const_iterator;
		
		/// @brief リバース・イテレータ型
		using reverse_iterator			= container_type::reverse_iterator;
		
		/// @brief const リバース・イテレータ型
		using const_reverse_iterator	= container_type::const_reverse_iterator;
		
		/// @brief 要素の個数を表現する型
		using size_type					= container_type::size_type;
		
		/// @brief 2 つの要素間の距離を表現する型
		using difference_type			= container_type::difference_type;
		
		/// @brief アロケータの型
		using allocator_type			= container_type::allocator_type;

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

		/// @brief Array からのコピーで頂点の配列を作成します。
		/// @param other コピーする配列
		[[nodiscard]]
		constexpr LineString(const container_type& other);

		/// @brief Array からのムーブで頂点の配列を作成します。
		/// @param other ムーブする配列
		[[nodiscard]]
		constexpr LineString(container_type&& other);

		/// @brief count 個の value からなる頂点の配列を作成します。
		/// @param count 個数
		/// @param value 要素
		[[nodiscard]]
		constexpr LineString(size_type count, const value_type& value);

		/// @brief count 個の Vec2{ 0, 0 } からなる頂点の配列を作成します。
		/// @param count 個数
		[[nodiscard]]
		explicit constexpr LineString(size_type count);

		/// @brief イテレータが指す範囲の要素から頂点の配列を作成します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		template <std::input_iterator Iterator>
		[[nodiscard]]
		constexpr LineString(Iterator first, Iterator last);

		/// @brief 頂点の配列を作成します。
		/// @param vertices 頂点の配列
		[[nodiscard]]
		explicit constexpr LineString(const Array<Point>& vertices);

		/// @brief メンバ関数 `.asArray()` を持つ型から頂点の配列を作成します。
		/// @tparam ArrayIsh メンバ関数 `.asArray()` を持つ型
		/// @param a `.asArray()` を持つ型のオブジェクト
		[[nodiscard]]
		explicit constexpr LineString(const HasAsArray auto& a);

		/// @brief メンバ関数 `.asArray()` を持つ型から頂点の配列を作成します。
		/// @tparam ArrayIsh メンバ関数 `.asArray()` を持つ型
		/// @param a `.asArray()` を持つ型のオブジェクト
		[[nodiscard]]
		explicit constexpr LineString(HasAsArray auto&& a);

		/// @brief 初期化リストから頂点の配列を作成します。
		/// @param list 初期化リスト
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr LineString(std::initializer_list<value_type> list);

		/// @brief 範囲から頂点の配列を作成します。
		/// @tparam Range 範囲の型
		/// @param tag 範囲から構築することを示すタグ
		/// @param range 範囲
		template <Concept::ContainerCompatibleRange<Vec2> Range>
		[[nodiscard]]
		constexpr LineString(std::from_range_t tag, Range&& range);

		/// @brief 範囲から頂点の配列を作成します。
		/// @tparam Range 範囲の型
		/// @param tag 範囲から構築することを示すタグ
		/// @param range 範囲
		template <Concept::ContainerCompatibleRange<Point> Range>
		[[nodiscard]]
		constexpr LineString(std::from_range_t tag, Range&& range);

		/// @brief 空の頂点の配列を作成し、`reserve()` します。
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
		template <std::input_iterator Iterator>
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
		/// @return Array
		[[nodiscard]]
		constexpr operator container_type() const& noexcept;

		/// @brief Array への暗黙の変換を行います。
		/// @return Array
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

		/// @brief 配列の合計サイズ（バイト）を返します。
		/// @return 配列の合計サイズ（バイト）
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
		template <std::input_iterator Iterator>
		constexpr iterator insert(const_iterator pos, Iterator first, Iterator last);

		/// @brief 指定した位置にリストの要素を挿入します。
		/// @param pos 挿入する位置
		/// @param list リスト
		/// @return 挿入された要素の先頭を指すイテレータ
		constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> list);

		////////////////////////////////////////////////////////////////
		//
		//	insert_range
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に範囲の要素を挿入します。
		/// @tparam Range 範囲の型
		/// @param pos 挿入する位置
		/// @param range 範囲
		/// @return 挿入された要素の先頭を指すイテレータ
		template <Concept::ContainerCompatibleRange<Vec2> Range>
		constexpr iterator insert_range(const_iterator pos, Range&& range);

		////////////////////////////////////////////////////////////////
		//
		//	emplace
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に要素を構築して挿入します。
		/// @tparam Args 構築する要素の引数の型
		/// @param pos 挿入する位置
		/// @param args 構築する要素の引数
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
		//	erase_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにある要素を削除します。
		/// @param index インデックス
		/// @return *this
		constexpr LineString& erase_at(size_type index)&;

		/// @brief 指定したインデックスにある要素を削除した新しい LineString を返します。
		/// @param index インデックス
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString erase_at(size_type index)&&;

		////////////////////////////////////////////////////////////////
		//
		//	erase_all
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しい要素をすべて削除します。
		/// @param value 値
		/// @return 削除した要素の個数
		constexpr size_type erase_all(const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	erase_first
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から見て、最初に指定した値と等しい要素を削除します。
		/// @param value 値
		/// @return 削除された要素があった場合 true, それ以外の場合は false
		constexpr bool erase_first(const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	erase_all_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素をすべて削除します。
		/// @tparam Fty 条件を表す述語の型
		/// @param f 条件を表す述語
		/// @return 削除した要素の個数
		template <class Fty>
		constexpr size_type erase_all_if(Fty f)
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	erase_first_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から見て、最初に指定した条件を満たす要素を削除します。
		/// @tparam Fty 条件を表す述語の型
		/// @param f 条件を表す述語
		/// @return 削除された要素があった場合 true, それ以外の場合は false
		template <class Fty>
		constexpr bool erase_first_if(Fty f)
			requires std::predicate<Fty&, const value_type&>;

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
		/// @tparam Args 構築する要素の引数の型
		/// @param args 構築する要素の引数
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
		/// @tparam Args 構築する要素の引数の型
		/// @param args 構築する要素の引数
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
		//	all
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素が条件を満たすかを返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return すべての要素が条件を満たすか、配列が空の場合 true, それ以外の場合は false
		template <class Fty = decltype(Identity)>
		[[nodiscard]]
		constexpr bool all(Fty f = Identity) const
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
		template <class Fty = decltype(Identity)>
		[[nodiscard]]
		constexpr bool any(Fty f = Identity) const
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に別の LineString を追加します。
		/// @param other 追加する LineString
		/// @return *this
		constexpr LineString& append(const LineString& other);

		/// @brief 配列の末尾に別の配列を追加します。
		/// @param other 追加する配列
		/// @return *this
		constexpr LineString& append(const container_type& other);

		/// @brief 配列の末尾に別の範囲の要素を追加します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return *this
		template <std::input_iterator Iterator>
		constexpr LineString& append(Iterator first, Iterator last);

		/// @brief 配列の末尾にリストの要素を追加します。
		/// @param list リスト
		/// @return *this
		constexpr LineString& append(std::initializer_list<value_type> list);

		/// @brief 配列の末尾に要素を追加します。
		/// @param count 追加する個数
		/// @param value 追加する値
		/// @return *this
		constexpr LineString& append(size_type count, const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	choice
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点列の要素を 1 つランダムに返します。
		/// @return 頂点列からランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type& choice();

		/// @brief 頂点列の要素を 1 つランダムに返します。
		/// @return 頂点列からランダムに選ばれた要素への参照
		[[nodiscard]]
		const value_type& choice() const;

		/// @brief 指定した乱数エンジンを用いて、頂点列の要素を 1 つランダムに返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 頂点列からランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type& choice(Concept::UniformRandomBitGenerator auto&& rbg);

		/// @brief 指定した乱数エンジンを用いて、頂点列の要素を 1 つランダムに返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 頂点列からランダムに選ばれた要素への参照
		[[nodiscard]]
		const value_type& choice(Concept::UniformRandomBitGenerator auto&& rbg) const;

		/// @brief 頂点列の要素から指定した個数だけ重複なくランダムに選んで返します。
		/// @param n 選択する個数
		/// @return ランダムに選ばれた要素の LineString
		[[nodiscard]]
		LineString choice(size_t n) const;

		/// @brief 指定した乱数エンジンを用いて、頂点列の要素から指定した個数だけ重複なくランダムに選んで返します。
		/// @param n 選択する個数
		/// @param rbg 使用する乱数エンジン
		/// @return ランダムに選ばれた要素の LineString
		[[nodiscard]]
		LineString choice(size_t n, Concept::UniformRandomBitGenerator auto&& rbg) const;

		////////////////////////////////////////////////////////////////
		//
		//	chunk
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点列を指定した個数の要素を持つグループに分割します。最後のグループの要素数は n 個未満になることがあります。
		/// @param n 1 つのグループが持つ要素数
		/// @return 分割された頂点列のグループ
		[[nodiscard]]
		constexpr Array<LineString> chunk(size_type n) const;

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
		constexpr bool contains_if(Fty f) const
			requires std::predicate<Fty&, const value_type&>;

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
		constexpr isize count_if(Fty f) const
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	each
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		constexpr void each(Fty f)
			requires std::invocable<Fty&, value_type&>;

		/// @brief すべての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
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
		template <class Fty>
		constexpr void each_index(Fty f)
			requires std::invocable<Fty&, size_t, value_type&>;

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
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
		template <class Fty>
		constexpr void each_sindex(Fty f)
			requires std::invocable<Fty&, isize, value_type&>;

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		constexpr void each_sindex(Fty f) const
			requires std::invocable<Fty&, isize, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	fetch
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにある要素を返します。インデックスが範囲外の場合デフォルト値を返します。
		/// @tparam U デフォルト値の型
		/// @param index インデックス
		/// @param defaultValue インデックスが範囲外の場合に返すデフォルト値
		/// @return 指定したインデックスにある要素。範囲外の場合は defaultValue
		template <class U>
		[[nodiscard]]
		constexpr value_type fetch(size_type index, U&& defaultValue) const
			noexcept(std::is_nothrow_constructible_v<value_type, U> && std::is_nothrow_copy_constructible_v<value_type>)
			requires std::constructible_from<value_type, U>;

		////////////////////////////////////////////////////////////////
		//
		//	fill
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値をすべての要素に代入します。
		/// @param value 代入する値
		/// @return *this
		constexpr LineString& fill(const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	filter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素だけを集めた新しい LineString を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 指定した条件を満たす要素を集めた新しい LineString
		template <class Fty>
		[[nodiscard]]
		constexpr LineString filter(Fty f) const
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	in_groups
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点列を指定したグループ数に分割します。
		/// @param group グループ数
		/// @remark group が要素数より大きい場合、空のグループは作られず、返されるグループ数は要素数になります。
		/// @return 分割した頂点列のグループ
		[[nodiscard]]
		constexpr Array<LineString> in_groups(size_type group) const;

		////////////////////////////////////////////////////////////////
		//
		//	indexOf
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しい最初の要素のインデックスを返します。
		/// @param value 検索する値
		/// @return 指定した値と等しい最初の要素のインデックス。見つからなかった場合は none
		[[nodiscard]]
		constexpr Optional<size_t> indexOf(const value_type& value) const;

		////////////////////////////////////////////////////////////////
		//
		//	join
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素から文字列を生成します。
		/// @param sep 要素の間に挿入する文字列
		/// @return 生成された文字列
		[[nodiscard]]
		String join(StringView sep = U", ") const;

		/// @brief 配列の要素から文字列を生成します。
		/// @param sep 要素の間に挿入する文字列
		/// @param begin 先頭に挿入する文字列
		/// @param end 末尾に挿入する文字列
		/// @return 生成された文字列
		[[nodiscard]]
		String join(StringView sep, StringView begin, StringView end) const;

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
		template <class Fty = decltype(Identity)>
		[[nodiscard]]
		constexpr bool none(Fty f = Identity) const
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	slice
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した範囲の要素からなる新しい LineString を返します。
		/// @param index インデックス
		/// @param length 長さ
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString slice(size_type index, size_type length) const&;

		/// @brief 指定した範囲の要素からなる新しい LineString を返します。
		/// @param index インデックス
		/// @param length 長さ
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString slice(size_type index, size_type length) &&;

		////////////////////////////////////////////////////////////////
		//
		//	head
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から最大 n 個の要素を取り出した新しい LineString を返します。
		/// @param n 取り出す最大要素数
		/// @return 先頭から最大 n 個の要素を含む新しい LineString
		[[nodiscard]]
		constexpr LineString head(size_type n) const&;

		/// @brief 先頭から最大 n 個の要素を取り出した新しい LineString を返します。
		/// @param n 取り出す最大要素数
		/// @return 先頭から最大 n 個の要素を含む新しい LineString
		[[nodiscard]]
		constexpr LineString head(size_type n) &&;

		////////////////////////////////////////////////////////////////
		//
		//	head_span
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から最大 n 個の要素を参照する span を返します。
		/// @param n 参照する最大要素数
		/// @return 先頭から最大 n 個の要素を参照する `std::span`
		[[nodiscard]]
		constexpr std::span<value_type> head_span(size_type n) & noexcept;

		/// @brief 先頭から最大 n 個の要素を参照する span を返します。
		/// @param n 参照する最大要素数
		/// @return 先頭から最大 n 個の要素を参照する `std::span`
		[[nodiscard]]
		constexpr std::span<const value_type> head_span(size_type n) const& noexcept;

		/// @brief 先頭から最大 n 個の要素を参照する span を返します。
		/// @param n 参照する最大要素数
		/// @return 先頭から最大 n 個の要素を参照する `std::span`
		constexpr std::span<value_type> head_span(size_type n) && = delete;

		////////////////////////////////////////////////////////////////
		//
		//	head_view
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から最大 n 個の要素を取り出す Ranges ビューを返します。
		/// @param n 取り出す最大要素数
		/// @return `std::views::take` による遅延評価ビュー
		[[nodiscard]]
		constexpr auto head_view(size_type n) & noexcept;

		/// @brief 先頭から最大 n 個の要素を取り出す Ranges ビューを返します。
		/// @param n 取り出す最大要素数
		/// @return `std::views::take` による遅延評価ビュー
		[[nodiscard]]
		constexpr auto head_view(size_type n) const& noexcept;

		/// @brief 先頭から最大 n 個の要素を取り出す Ranges ビューを返します。
		/// @param n 取り出す最大要素数
		/// @return `std::views::take` による遅延評価ビュー
		[[nodiscard]]
		constexpr auto head_view(size_type n) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	tail
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の最大 n 個の要素を取り出した新しい LineString を返します。
		/// @param n 取り出す最大要素数
		/// @return 末尾の最大 n 個の要素を含む新しい LineString
		[[nodiscard]]
		constexpr LineString tail(size_type n) const&;

		/// @brief 末尾の最大 n 個の要素を取り出した新しい LineString を返します。
		/// @param n 取り出す最大要素数
		/// @return 末尾の最大 n 個の要素を含む新しい LineString
		[[nodiscard]]
		constexpr LineString tail(size_type n) &&;

		////////////////////////////////////////////////////////////////
		//
		//	tail_span
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の最大 n 個の要素を参照する span を返します。
		/// @param n 参照する最大要素数
		/// @return 末尾の最大 n 個の要素を参照する `std::span`
		[[nodiscard]]
		constexpr std::span<value_type> tail_span(size_type n) & noexcept;

		/// @brief 末尾の最大 n 個の要素を参照する span を返します。
		/// @param n 参照する最大要素数
		/// @return 末尾の最大 n 個の要素を参照する `std::span`
		[[nodiscard]]
		constexpr std::span<const value_type> tail_span(size_type n) const& noexcept;

		/// @brief 末尾の最大 n 個の要素を参照する span を返します。
		/// @param n 参照する最大要素数
		/// @return 末尾の最大 n 個の要素を参照する `std::span`
		constexpr std::span<value_type> tail_span(size_type n) && = delete;

		////////////////////////////////////////////////////////////////
		//
		//	tail_view
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の最大 n 個の要素を取り出す Ranges ビューを返します。
		/// @param n 取り出す最大要素数
		/// @return `std::views::take` による遅延評価ビュー
		[[nodiscard]]
		constexpr auto tail_view(size_type n) & noexcept;

		/// @brief 末尾の最大 n 個の要素を取り出す Ranges ビューを返します。
		/// @param n 取り出す最大要素数
		/// @return `std::views::take` による遅延評価ビュー
		[[nodiscard]]
		constexpr auto tail_view(size_type n) const& noexcept;

		/// @brief 末尾の最大 n 個の要素を取り出す Ranges ビューを返します。
		/// @param n 取り出す最大要素数
		/// @return `std::views::take` による遅延評価ビュー
		[[nodiscard]]
		constexpr auto tail_view(size_type n) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	take
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から最大 n 個の要素を取り出した新しい LineString を返します。
		/// @param n 取り出す最大要素数
		/// @return 先頭から最大 n 個の要素を含む新しい LineString
		[[nodiscard]]
		constexpr LineString take(size_type n) const&;

		/// @brief 先頭から最大 n 個の要素を取り出した新しい LineString を返します。
		/// @param n 取り出す最大要素数
		/// @return 先頭から最大 n 個の要素を含む新しい LineString
		[[nodiscard]]
		constexpr LineString take(size_type n) &&;

		////////////////////////////////////////////////////////////////
		//
		//	take_while
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から、条件を満たさなくなる直前までの要素からなる新しい LineString を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい LineString
		template <class Fty>
		[[nodiscard]]
		constexpr LineString take_while(Fty f) const&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 先頭から、条件を満たさなくなる直前までの要素からなる新しい LineString を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい LineString
		template <class Fty>
		[[nodiscard]]
		constexpr LineString take_while(Fty f) &&
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	values_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの要素からなる新しい LineString を返します。
		/// @param indices インデックス
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString values_at(std::initializer_list<size_type> indices) const;

		////////////////////////////////////////////////////////////////
		//
		//	without
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しいすべての要素を削除した新しい LineString を返します。
		/// @param value 削除する値
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString without(const value_type& value) const&;

		/// @brief 指定した値と等しいすべての要素を削除した新しい LineString を返します。
		/// @param value 削除する値
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString without(const value_type& value) &&;

		////////////////////////////////////////////////////////////////
		//
		//	without_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにある要素を削除した新しい LineString を返します。
		/// @param index インデックス
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString without_at(size_type index) const&;

		/// @brief 指定したインデックスにある要素を削除した新しい LineString を返します。
		/// @param index インデックス
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString without_at(size_type index) &&;

		////////////////////////////////////////////////////////////////
		//
		//	without_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす要素を配列から削除した新しい LineString を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい LineString
		template <class Fty>
		[[nodiscard]]
		constexpr LineString without_if(Fty f) const&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 条件を満たす要素を配列から削除した新しい LineString を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい LineString
		template <class Fty>
		[[nodiscard]]
		constexpr LineString without_if(Fty f) &&
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	replace, replaced
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しいすべての要素を別の値に置き換えます。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return *this
		constexpr LineString& replace(const value_type& oldValue, const value_type& newValue)&;

		/// @brief 指定した値と等しいすべての要素を別の値に置き換えた新しい LineString を返します。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString replace(const value_type& oldValue, const value_type& newValue) &&;

		/// @brief 指定した値と等しいすべての要素を別の値に置き換えた新しい LineString を返します。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString replaced(const value_type& oldValue, const value_type& newValue) const&;

		/// @brief 指定した値と等しいすべての要素を別の値に置き換えた新しい LineString を返します。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString replaced(const value_type& oldValue, const value_type& newValue) &&;

		////////////////////////////////////////////////////////////////
		//
		//	replace_if, replaced_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えます。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return *this
		template <class Fty>
		constexpr LineString& replace_if(Fty f, const value_type& newValue)&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えた新しい LineString を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい LineString
		template <class Fty>
		[[nodiscard]]
		constexpr LineString replace_if(Fty f, const value_type& newValue) &&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えた新しい LineString を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい LineString
		template <class Fty>
		[[nodiscard]]
		constexpr LineString replaced_if(Fty f, const value_type& newValue) const&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えた新しい LineString を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい LineString
		template <class Fty>
		[[nodiscard]]
		constexpr LineString replaced_if(Fty f, const value_type& newValue) &&
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	rotate, rotated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を境に頂点列の前半と後半を入れ替えます。
		/// @param middle 境の位置
		/// @return *this
		constexpr LineString& rotate(size_type middle)&;

		/// @brief 指定した位置を境に頂点列の前半と後半を入れ替えた新しい LineString を返します。
		/// @param middle 境の位置
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString rotate(size_type middle) &&;

		/// @brief 指定した位置を境に頂点列の前半と後半を入れ替えた新しい LineString を返します。
		/// @param middle 境の位置
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString rotated(size_type middle) const&;

		/// @brief 指定した位置を境に頂点列の前半と後半を入れ替えた新しい LineString を返します。
		/// @param middle 境の位置
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString rotated(size_type middle) &&;

		////////////////////////////////////////////////////////////////
		//
		//	reverse_each
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾から順番に、すべての要素に対して関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		constexpr void reverse_each(Fty f)
			requires std::invocable<Fty&, value_type&>;

		/// @brief 末尾から順番に、すべての要素に対して関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		constexpr void reverse_each(Fty f) const
			requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	reverse_view
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点列の逆順ビューを返します。
		/// @return 頂点列の逆順ビュー
		[[nodiscard]]
		constexpr auto reverse_view() &;

		/// @brief 頂点列の逆順ビューを返します。
		/// @return 頂点列の逆順ビュー
		[[nodiscard]]
		constexpr auto reverse_view() const&;

		/// @brief 頂点列の逆順ビューを返します。
		/// @return 頂点列の逆順ビュー
		[[nodiscard]]
		constexpr auto reverse_view() &&;

		////////////////////////////////////////////////////////////////
		//
		//	shuffle, shuffled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点列の並び順をランダムにシャッフルします。
		/// @return *this
		constexpr LineString& shuffle()&;

		/// @brief 頂点列の並び順をランダムにシャッフルした新しい LineString を返します。
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString shuffle() &&;

		/// @brief 頂点列の並び順をランダムにシャッフルした新しい LineString を返します。
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString shuffled() const&;

		/// @brief 頂点列の並び順をランダムにシャッフルした新しい LineString を返します。
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString shuffled() &&;

		/// @brief 指定した乱数エンジンを用いて、頂点列の並び順をランダムにシャッフルします。
		/// @param rbg 使用する乱数エンジン
		/// @return *this
		constexpr LineString& shuffle(Concept::UniformRandomBitGenerator auto&& rbg)&;

		/// @brief 指定した乱数エンジンを用いて、頂点列の並び順をランダムにシャッフルした新しい LineString を返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString shuffle(Concept::UniformRandomBitGenerator auto&& rbg) &&;

		/// @brief 指定した乱数エンジンを用いて、頂点列の並び順をランダムにシャッフルした新しい LineString を返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString shuffled(Concept::UniformRandomBitGenerator auto&& rbg) const&;

		/// @brief 指定した乱数エンジンを用いて、頂点列の並び順をランダムにシャッフルした新しい LineString を返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString shuffled(Concept::UniformRandomBitGenerator auto&& rbg) &&;

		////////////////////////////////////////////////////////////////
		//
		//	sort_by, sorted_by
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した関数を用いて頂点列を昇順に並び替えます。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return *this
		template <class Fty>
		constexpr LineString& sort_by(Fty f)&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて頂点列を昇順に並び替えた新しい LineString を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい LineString
		template <class Fty>
		[[nodiscard]]
		constexpr LineString sort_by(Fty f) &&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて頂点列を昇順に並び替えた新しい LineString を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい LineString
		template <class Fty>
		[[nodiscard]]
		constexpr LineString sorted_by(Fty f) const&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて頂点列を昇順に並び替えた新しい LineString を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい LineString
		template <class Fty>
		[[nodiscard]]
		constexpr LineString sorted_by(Fty f) &&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	sum
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点列の要素を `+` 演算子を用いて合計します。
		/// @return 合計値
		[[nodiscard]]
		constexpr auto sum() const;

		////////////////////////////////////////////////////////////////
		//
		//	reverse
		//
		////////////////////////////////////////////////////////////////

		/// @brief 連続する線分の向きを反転します。
		/// @return *this
		constexpr LineString& reverse() & noexcept;

		/// @brief 連続する線分の向きを反転した新しい LineString を返します。
		/// @return 連続する線分の向きを反転した新しい LineString
		[[nodiscard]]
		constexpr LineString reverse() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reversed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 連続する線分の向きを反転した新しい LineString を返します。
		/// @return 連続する線分の向きを反転した新しい LineString
		[[nodiscard]]
		constexpr LineString reversed() const&;

		/// @brief 連続する線分の向きを反転した新しい LineString を返します。
		/// @return 連続する線分の向きを反転した新しい LineString
		[[nodiscard]]
		constexpr LineString reversed() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	unique_consecutive, uniqued_consecutive
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ要素が連続する場合、その先頭以外を除去します。
		/// @return *this
		constexpr LineString& unique_consecutive() &;

		/// @brief 同じ要素が連続する場合、その先頭以外を除去した新しい LineString を返します。
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString unique_consecutive() &&;

		/// @brief 同じ要素が連続する場合、その先頭以外を除去した新しい LineString を返します。
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString uniqued_consecutive() const&;

		/// @brief 同じ要素が連続する場合、その先頭以外を除去した新しい LineString を返します。
		/// @return 新しい LineString
		[[nodiscard]]
		constexpr LineString uniqued_consecutive() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	num_vertices
		//
		////////////////////////////////////////////////////////////////

		/// @brief LineString を構成する頂点の数を返します。
		/// @remark `size()` と同じです。
		/// @return LineString を構成する頂点の数
		[[nodiscard]]
		constexpr size_t num_vertices() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	num_segments
		//
		////////////////////////////////////////////////////////////////

		/// @brief LineString を構成する線分の数を返します。
		/// @param closeRing 終点と始点を結ぶか
		/// @return LineString を構成する線分の数
		[[nodiscard]]
		constexpr size_t num_segments(CloseRing closeRing = CloseRing::No) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	segment
		//
		////////////////////////////////////////////////////////////////

		/// @brief LineString を構成する線分を返します。
		/// @param index 線分のインデックス
		/// @param closeRing 終点と始点を結ぶか
		/// @return LineString を構成する線分
		[[nodiscard]]
		Line segment(size_t index, CloseRing closeRing = CloseRing::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	tangentAtVertex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した頂点における進行方向の単位ベクトルを返します。
		/// @param index 頂点のインデックス
		/// @param closeRing 終点と始点を結ぶか
		/// @return 指定した頂点における進行方向の単位ベクトル
		[[nodiscard]]
		Vec2 tangentAtVertex(size_t index, CloseRing closeRing = CloseRing::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	tangentAtSegment
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した線分における進行方向の単位ベクトルを返します。
		/// @param index 線分のインデックス
		/// @param closeRing 終点と始点を結ぶか
		/// @return 指定した線分における進行方向の単位ベクトル
		[[nodiscard]]
		Vec2 tangentAtSegment(size_t index, CloseRing closeRing = CloseRing::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	normalAtVertex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した頂点における進行方向左手の単位ベクトルを返します。
		/// @param index 頂点のインデックス
		/// @param closeRing 終点と始点を結ぶか
		/// @return 指定した頂点における進行方向左手の単位ベクトル
		[[nodiscard]]
		Vec2 normalAtVertex(size_t index, CloseRing closeRing = CloseRing::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	normalAtSegment
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した線分における進行方向左手の単位ベクトルを返します。
		/// @param index 線分のインデックス
		/// @param closeRing 終点と始点を結ぶか
		/// @return  指定した線分における進行方向左手の単位ベクトル
		[[nodiscard]]
		Vec2 normalAtSegment(size_t index, CloseRing closeRing = CloseRing::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 平行移動した LineString を返します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return 平行移動した LineString
		[[nodiscard]]
		constexpr LineString movedBy(double x, double y) const&;

		/// @brief 平行移動した LineString を返します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return 平行移動した LineString
		[[nodiscard]]
		constexpr LineString movedBy(double x, double y) && noexcept;

		/// @brief 平行移動した LineString を返します。
		/// @param v 移動量
		/// @return 平行移動した LineString
		[[nodiscard]]
		constexpr LineString movedBy(Vec2 v) const&;

		/// @brief 平行移動した LineString を返します。
		/// @param v 移動量
		/// @return 平行移動した LineString
		[[nodiscard]]
		constexpr LineString movedBy(Vec2 v) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief LineString を平行移動します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return *this
		constexpr LineString& moveBy(double x, double y) noexcept;

		/// @brief LineString を平行移動します。
		/// @param v 移動量
		/// @return *this
		constexpr LineString& moveBy(Vec2 v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffset
		//
		////////////////////////////////////////////////////////////////

		/// @brief 平行移動した LineString を返します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return 平行移動した LineString
		[[nodiscard]]
		constexpr LineString withOffset(double x, double y) const&;
		
		/// @brief 平行移動した LineString を返します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return 平行移動した LineString
		[[nodiscard]]
		constexpr LineString withOffset(double x, double y) && noexcept;
		
		/// @brief 平行移動した LineString を返します。
		/// @param v 移動量
		/// @return 平行移動した LineString
		[[nodiscard]]
		constexpr LineString withOffset(Vec2 v) const&;
		
		/// @brief 平行移動した LineString を返します。
		/// @param v 移動量
		/// @return 平行移動した LineString
		[[nodiscard]]
		constexpr LineString withOffset(Vec2 v) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffsetX, withOffsetY
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 方向に平行移動した LineString を返します。
		/// @param x X 方向の移動量
		/// @return X 方向に平行移動した LineString
		[[nodiscard]]
		constexpr LineString withOffsetX(double x) const&;
		
		/// @brief X 方向に平行移動した LineString を返します。
		/// @param x X 方向の移動量
		/// @return X 方向に平行移動した LineString
		[[nodiscard]]
		constexpr LineString withOffsetX(double x) && noexcept;
		
		/// @brief Y 方向に平行移動した LineString を返します。
		/// @param y Y 方向の移動量
		/// @return Y 方向に平行移動した LineString
		[[nodiscard]]
		constexpr LineString withOffsetY(double y) const&;
		
		/// @brief Y 方向に平行移動した LineString を返します。
		/// @param y Y 方向の移動量
		/// @return Y 方向に平行移動した LineString
		[[nodiscard]]
		constexpr LineString withOffsetY(double y) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromOrigin
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点 (0, 0) を中心に拡大・縮小した LineString を返します。
		/// @param s 拡大率
		/// @return 拡大・縮小した LineString
		[[nodiscard]]
		constexpr LineString scaledFromOrigin(double s) const&;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した LineString を返します。
		/// @param s 拡大率
		/// @return 拡大・縮小した LineString
		[[nodiscard]]
		constexpr LineString scaledFromOrigin(double s) && noexcept;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した LineString を返します。
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return 拡大・縮小した LineString
		[[nodiscard]]
		constexpr LineString scaledFromOrigin(double sx, double sy) const&;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した LineString を返します。
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return 拡大・縮小した LineString
		[[nodiscard]]
		constexpr LineString scaledFromOrigin(double sx, double sy) && noexcept;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した LineString を返します。
		/// @param s 拡大率
		/// @return 拡大・縮小した LineString
		[[nodiscard]]
		constexpr LineString scaledFromOrigin(Vec2 s) const&;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した LineString を返します。
		/// @param s 拡大率
		/// @return 拡大・縮小した LineString
		[[nodiscard]]
		constexpr LineString scaledFromOrigin(Vec2 s) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaleFromOrigin
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点 (0, 0) を中心に LineString を拡大・縮小します。
		/// @param s 拡大率
		/// @return *this
		constexpr LineString& scaleFromOrigin(double s);

		/// @brief 原点 (0, 0) を中心に LineString を拡大・縮小します。
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return *this
		constexpr LineString& scaleFromOrigin(double sx, double sy);

		/// @brief 原点 (0, 0) を中心に LineString を拡大・縮小します。
		/// @param s 拡大率
		/// @return *this
		constexpr LineString& scaleFromOrigin(Vec2 s);

		////////////////////////////////////////////////////////////////
		//
		//	scaledFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標を中心に拡大・縮小した LineString を返します。
		/// @param pos 拡大・縮小の中心座標
		/// @param s 拡大率
		/// @return 拡大・縮小した LineString
		[[nodiscard]]
		constexpr LineString scaledFrom(Vec2 pos, double s) const&;

		/// @brief 指定した座標を中心に拡大・縮小した LineString を返します。
		/// @param pos 拡大・縮小の中心座標
		/// @param s 拡大率
		/// @return 拡大・縮小した LineString
		[[nodiscard]]
		constexpr LineString scaledFrom(Vec2 pos, double s) && noexcept;

		/// @brief 指定した座標を中心に拡大・縮小した LineString を返します。
		/// @param pos 拡大・縮小の中心座標
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return 拡大・縮小した LineString
		[[nodiscard]]
		constexpr LineString scaledFrom(Vec2 pos, double sx, double sy) const&;

		/// @brief 指定した座標を中心に拡大・縮小した LineString を返します。
		/// @param pos 拡大・縮小の中心座標
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return 拡大・縮小した LineString
		[[nodiscard]]
		constexpr LineString scaledFrom(Vec2 pos, double sx, double sy) && noexcept;

		/// @brief 指定した座標を中心に拡大・縮小した LineString を返します。
		/// @param pos 拡大・縮小の中心座標
		/// @param s 拡大率
		/// @return 拡大・縮小した LineString
		[[nodiscard]]
		constexpr LineString scaledFrom(Vec2 pos, Vec2 s) const&;

		/// @brief 指定した座標を中心に拡大・縮小した LineString を返します。
		/// @param pos 拡大・縮小の中心座標
		/// @param s 拡大率
		/// @return 拡大・縮小した LineString
		[[nodiscard]]
		constexpr LineString scaledFrom(Vec2 pos, Vec2 s) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaleFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標を中心に LineString を拡大・縮小します。
		/// @param pos 拡大・縮小の中心座標
		/// @param s 拡大率
		/// @return *this
		constexpr LineString& scaleFrom(Vec2 pos, double s);

		/// @brief 指定した座標を中心に LineString を拡大・縮小します。
		/// @param pos 拡大・縮小の中心座標
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return *this
		constexpr LineString& scaleFrom(Vec2 pos, double sx, double sy);

		/// @brief 指定した座標を中心に LineString を拡大・縮小します。
		/// @param pos 拡大・縮小の中心座標
		/// @param s 拡大率
		/// @return *this
		constexpr LineString& scaleFrom(Vec2 pos, Vec2 s);

		////////////////////////////////////////////////////////////////
		//
		//	computeBoundingRect
		//
		////////////////////////////////////////////////////////////////

		/// @brief 連続する線分を含む最小の長方形を返します。
		/// @return 連続する線分を含む最小の長方形
		[[nodiscard]]
		RectF computeBoundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	simplified
		//
		////////////////////////////////////////////////////////////////

		/// @brief 連続する線分を単純化した LineString を返します。
		/// @param maxDistance 単純化の大きさ
		/// @param closeRing 終点と始点を結ぶか
		/// @return 単純化した LineString
		[[nodiscard]]
		LineString simplified(double maxDistance = 2.0, CloseRing closeRing = CloseRing::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	densified
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点間の距離が `maxDistance` より大きくならないよう、区間ごとに最小回数で均等に分割した結果を返します。
		/// @param maxSegmentLength 分割後の各線分の最大長
		/// @param closeRing 終点と始点を結ぶか
		/// @return 分割した結果
		[[nodiscard]]
		LineString densified(double maxSegmentLength, CloseRing closeRing = CloseRing::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	catmullRom
		//
		////////////////////////////////////////////////////////////////

		/// @brief Catmull-Rom スプライン曲線を返します。
		/// @param subdivisionsPerSegment 線分ごとの分割数
		/// @return Catmull-Rom スプライン曲線
		[[nodiscard]]
		LineString catmullRom(int32 subdivisionsPerSegment = 24) const;

		/// @brief Catmull-Rom スプライン曲線を返します。
		/// @param closeRing 終点と始点を結ぶか
		/// @param subdivisionsPerSegment 線分ごとの分割数
		/// @return Catmull-Rom スプライン曲線
		[[nodiscard]]
		LineString catmullRom(CloseRing closeRing, int32 subdivisionsPerSegment = 24) const;

		////////////////////////////////////////////////////////////////
		//
		//	computeLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 連続する線分全体の長さを返します。
		/// @param closeRing 終点と始点を結ぶか
		/// @return 連続する線分全体の長さ
		[[nodiscard]]
		double computeLength(CloseRing closeRing = CloseRing::No) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	computePointAtDistance
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点から指定した距離にある、線分上の点を返します
		/// @param distanceFromStart 始点からの距離
		/// @param closeRing 終点と始点を結ぶか
		/// @return 始点から指定した距離にある線分上の点
		[[nodiscard]]
		Vec2 computePointAtDistance(double distanceFromStart, CloseRing closeRing = CloseRing::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	computeVertexNormals
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各頂点における進行方向左手の単位ベクトルを返します。
		/// @param closeRing 終点と始点を結ぶか
		/// @return 各頂点における進行方向左手の単位ベクトル
		[[nodiscard]]
		Array<Vec2> computeVertexNormals(CloseRing closeRing = CloseRing::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	sliceByDistance
		//
		////////////////////////////////////////////////////////////////

		/// @brief 部分 LineString を返します。
		/// @param distanceFromStart 始点からの距離
		/// @param closeRing 終点と始点を結ぶか
		/// @return 部分 LineString
		[[nodiscard]]
		LineString sliceByDistance(double distanceFromStart, CloseRing closeRing = CloseRing::No) const;

		/// @brief 部分 LineString を返します。
		/// @param distanceFromStart 始点からの距離
		/// @param length 長さ
		/// @param closeRing 終点と始点を結ぶか
		/// @return 部分 LineString
		[[nodiscard]]
		LineString sliceByDistance(double distanceFromStart, double length, CloseRing closeRing = CloseRing::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	computeMiterBufferPolygon
		//
		////////////////////////////////////////////////////////////////

		/// @brief LineString を指定した距離だけ太らせた Polygon を返します。
		/// @param distance 太らせる距離（ピクセル）
		/// @return LineString を指定した距離だけ太らせた Polygon
		[[nodiscard]]
		Polygon computeMiterBufferPolygon(double distance) const;

		/// @brief LineString を指定した距離だけ太らせた Polygon を返します。
		/// @param distance 太らせる距離（ピクセル）
		/// @param closeRing 終点と始点を結ぶか
		/// @return LineString を指定した距離だけ太らせた Polygon
		[[nodiscard]]
		Polygon computeMiterBufferPolygon(double distance, CloseRing closeRing) const;

		////////////////////////////////////////////////////////////////
		//
		//	computeRoundBufferPolygon
		//
		////////////////////////////////////////////////////////////////

		/// @brief LineString を指定した距離だけ丸く太らせた Polygon を返します。
		/// @param distance 太らせる距離（ピクセル）
		/// @param qualityFactor 品質係数。大きいほど分割数が増えます。
		/// @return LineString を指定した距離だけ丸く太らせた Polygon
		[[nodiscard]]
		Polygon computeRoundBufferPolygon(double distance, const QualityFactor& qualityFactor = QualityFactor{ 1.0 }) const;
		
		/// @brief LineString を指定した距離だけ丸く太らせた Polygon を返します。
		/// @param distance 太らせる距離（ピクセル）
		/// @param closeRing 終点と始点を結ぶか
		/// @param qualityFactor 品質係数。大きいほど分割数が増えます。
		/// @return LineString を指定した距離だけ丸く太らせた Polygon
		[[nodiscard]]
		Polygon computeRoundBufferPolygon(double distance, CloseRing closeRing, const QualityFactor& qualityFactor = QualityFactor{ 1.0 }) const;

		////////////////////////////////////////////////////////////////
		//
		//	toSpline
		//
		////////////////////////////////////////////////////////////////

//		[[nodiscard]]
//		Spline2D toSpline(CloseRing closeRing = CloseRing::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	intersects
		//
		////////////////////////////////////////////////////////////////

//		template <class Shape2DType>
//		[[nodiscard]]
//		constexpr bool intersects(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	intersectsAt
		//
		////////////////////////////////////////////////////////////////

//		template <class Shape2DType>
//		[[nodiscard]]
//		Optional<Array<Vec2>> intersectsAt(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各要素に関数を適用します。
		/// @tparam Fty 適用する関数の型
		/// @param f 適用する関数
		/// @remark Fty が戻り値を持たない場合 `.each(f)`, 戻り値を持つ場合は `.map(f)` と同じです。
		/// @return 各要素に関数を適用した結果の配列。Fty が戻り値を持たない場合 void
		template <class Fty>
		constexpr auto operator >>(Fty f) const
			requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const LineString& value)
		{
			return output << Format(value.m_vertices);
		}

		////////////////////////////////////////////////////////////////
		//
		//	paint
		//
		////////////////////////////////////////////////////////////////

		/// @brief 連続する線分を Image に描き込みます。
		/// @param dst 描き込み先の Image
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const LineString& paint(Image& dst, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 連続する線分を Image に描き込みます。
		/// @param dst 描き込み先の Image
		/// @param thickness 線の太さ
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const LineString& paint(Image& dst, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 連続する線分を Image に描き込みます。
		/// @param dst 描き込み先の Image
		/// @param lineCap 線端の形状
		/// @param thickness 線の太さ
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const LineString& paint(Image& dst, LineCap lineCap, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwrite
		//
		////////////////////////////////////////////////////////////////

		/// @brief 連続する線分を Image に上書きします。
		/// @param dst 上書き先の Image
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const LineString& overwrite(Image& dst, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 連続する線分を Image に上書きします。
		/// @param dst 上書き先の Image
		/// @param thickness 線の太さ
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const LineString& overwrite(Image& dst, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 連続する線分を Image に上書きします。
		/// @param dst 上書き先の Image
		/// @param lineCap 線端の形状
		/// @param thickness 線の太さ
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const LineString& overwrite(Image& dst, LineCap lineCap, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	paintClosed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 終点と始点を結んだ連続する線分を Image に描き込みます。
		/// @param dst 描き込み先の Image
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const LineString& paintClosed(Image& dst, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 終点と始点を結んだ連続する線分を Image に描き込みます。
		/// @param dst 描き込み先の Image
		/// @param thickness 線の太さ
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const LineString& paintClosed(Image& dst, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwriteClosed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 終点と始点を結んだ連続する線分を Image に上書きします。
		/// @param dst 上書き先の Image
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const LineString& overwriteClosed(Image& dst, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 終点と始点を結んだ連続する線分を Image に上書きします。
		/// @param dst 上書き先の Image
		/// @param thickness 線の太さ
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const LineString& overwriteClosed(Image& dst, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 連続する線分を描画します。
		/// @param color 色
		/// @return *this
		const LineString& draw(const ColorF& color = Palette::White) const;

		/// @brief 連続する線分を描画します。
		/// @param colorStart 始点側の色
		/// @param colorEnd 終点側の色
		/// @return *this
		const LineString& draw(const ColorF& colorStart, const ColorF& colorEnd) const;

		/// @brief 連続する線分を描画します。
		/// @param pattern パターン
		/// @return *this
		const LineString& draw(const PatternParameters& pattern) const;

		/// @brief 連続する線分を描画します。
		/// @param thickness 線の太さ
		/// @param color 色
		/// @return *this
		const LineString& draw(double thickness, const ColorF& color = Palette::White) const;

		/// @brief 連続する線分を描画します。
		/// @param thickness 線の太さ
		/// @param colorStart 始点側の色
		/// @param colorEnd 終点側の色
		/// @return *this
		const LineString& draw(double thickness, const ColorF& colorStart, const ColorF& colorEnd) const;

		/// @brief 連続する線分を描画します。
		/// @param thickness 線の太さ
		/// @param pattern パターン
		/// @return *this
		const LineString& draw(double thickness, const PatternParameters& pattern) const;

		/// @brief 連続する線分を描画します。
		/// @param lineCap 線端の形状
		/// @param thickness 線の太さ
		/// @param color 色
		/// @return *this
		const LineString& draw(LineCap lineCap, double thickness, const ColorF& color = Palette::White) const;

		/// @brief 連続する線分を描画します。
		/// @param lineCap 線端の形状
		/// @param thickness 線の太さ
		/// @param colorStart 始点側の色
		/// @param colorEnd 終点側の色
		/// @return *this
		const LineString& draw(LineCap lineCap, double thickness, const ColorF& colorStart, const ColorF& colorEnd) const;

		/// @brief 連続する線分を描画します。
		/// @param lineCap 線端の形状
		/// @param thickness 線の太さ
		/// @param pattern パターン
		/// @return *this
		const LineString& draw(LineCap lineCap, double thickness, const PatternParameters& pattern) const;

		/// @brief 連続する線分を描画します。
		/// @param startCap 始点側の線端の形状
		/// @param endCap 終点側の線端の形状
		/// @param thickness 線の太さ
		/// @param color 色
		/// @return *this
		const LineString& draw(LineCap startCap, LineCap endCap, double thickness, const ColorF& color = Palette::White) const;

		/// @brief 連続する線分を描画します。
		/// @param startCap 始点側の線端の形状
		/// @param endCap 終点側の線端の形状
		/// @param thickness 線の太さ
		/// @param colorStart 始点側の色
		/// @param colorEnd 終点側の色
		/// @return *this
		const LineString& draw(LineCap startCap, LineCap endCap, double thickness, const ColorF& colorStart, const ColorF& colorEnd) const;

		/// @brief 連続する線分を描画します。
		/// @param startCap 始点側の線端の形状
		/// @param endCap 終点側の線端の形状
		/// @param thickness 線の太さ
		/// @param pattern パターン
		/// @return *this
		const LineString& draw(LineCap startCap, LineCap endCap, double thickness, const PatternParameters& pattern) const;

		/// @brief 頂点ごとの色で連続する線分を描画します。
		/// @param colors 頂点ごとの色
		/// @return *this
		const LineString& draw(std::span<const ColorF> colors) const;

		/// @brief 頂点ごとの色で連続する線分を描画します。
		/// @param thickness 線の太さ
		/// @param colors 頂点ごとの色
		/// @return *this
		const LineString& draw(double thickness, std::span<const ColorF> colors) const;

		/// @brief 頂点ごとの色で連続する線分を描画します。
		/// @param lineCap 線端の形状
		/// @param thickness 線の太さ
		/// @param colors 頂点ごとの色
		/// @return *this
		const LineString& draw(LineCap lineCap, double thickness, std::span<const ColorF> colors) const;

		/// @brief 頂点ごとの色で連続する線分を描画します。
		/// @param startCap 始点側の線端の形状
		/// @param endCap 終点側の線端の形状
		/// @param thickness 線の太さ
		/// @param colors 頂点ごとの色
		/// @return *this
		const LineString& draw(LineCap startCap, LineCap endCap, double thickness, std::span<const ColorF> colors) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawClosed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 終点と始点を結んだ連続する線分を描画します。
		/// @param color 色
		/// @return *this
		const LineString& drawClosed(const ColorF& color = Palette::White) const;

		/// @brief 終点と始点を結んだ連続する線分を描画します。
		/// @param pattern パターン
		/// @return *this
		const LineString& drawClosed(const PatternParameters& pattern) const;

		/// @brief 終点と始点を結んだ連続する線分を描画します。
		/// @param thickness 線の太さ
		/// @param color 色
		/// @return *this
		const LineString& drawClosed(double thickness, const ColorF& color = Palette::White) const;

		/// @brief 終点と始点を結んだ連続する線分を描画します。
		/// @param thickness 線の太さ
		/// @param pattern パターン
		/// @return *this
		const LineString& drawClosed(double thickness, const PatternParameters& pattern) const;

		/// @brief 頂点ごとの色で、終点と始点を結んだ連続する線分を描画します。
		/// @param colors 頂点ごとの色
		/// @return *this
		const LineString& drawClosed(std::span<const ColorF> colors) const;

		/// @brief 頂点ごとの色で、終点と始点を結んだ連続する線分を描画します。
		/// @param thickness 線の太さ
		/// @param colors 頂点ごとの色
		/// @return *this
		const LineString& drawClosed(double thickness, std::span<const ColorF> colors) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawVertices
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各頂点を円で描画します。
		/// @param r 円の半径
		/// @param color 色
		/// @return *this
		const LineString& drawVertices(double r, const ColorF& color = Palette::White) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawVerticesFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各頂点を円の枠で描画します。
		/// @param r 円の半径
		/// @param thickness 枠の太さ
		/// @param color 色
		/// @return *this
		const LineString& drawVerticesFrame(double r, double thickness = 1.0, const ColorF& color = Palette::White) const;

		/// @brief 各頂点を円の枠で描画します。
		/// @param r 円の半径
		/// @param innerThickness 内側方向の枠の太さ
		/// @param outerThickness 外側方向の枠の太さ
		/// @param color 色
		/// @return *this
		const LineString& drawVerticesFrame(double r, double innerThickness, double outerThickness, const ColorF& color = Palette::White) const;

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
			return (lhs.m_vertices == rhs.m_vertices);
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

		container_type m_vertices;
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

	s3d::BufferContext::iterator format(const s3d::LineString& value, s3d::BufferContext& ctx) const;
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
