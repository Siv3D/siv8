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
# include "Array.hpp"
# include "ArrayAlgorithm.hpp"
# include "ArrayRandom.hpp"
# include "Polygon.hpp"

namespace s3d
{
	class MultiPolygon
	{
	public:

		using container_type			= Array<Polygon>;

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
		MultiPolygon() = default;

		/// @brief コピーコンストラクタ
		/// @param other コピーする配列
		[[nodiscard]]
		MultiPolygon(const MultiPolygon& other) = default;

		/// @brief ムーブコンストラクタ
		/// @param other ムーブする配列
		[[nodiscard]]
		MultiPolygon(MultiPolygon&& other) = default;

		/// @brief Array からのコピーで多角形の配列を作成します。
		/// @param other コピーする配列
		[[nodiscard]]
		MultiPolygon(const container_type& other);

		/// @brief Array からのムーブで多角形の配列を作成します。
		/// @param other ムーブする配列
		[[nodiscard]]
		MultiPolygon(container_type&& other);

		/// @brief count 個の空の Polygon からなる多角形の配列を作成します。
		/// @param count 個数
		[[nodiscard]]
		explicit MultiPolygon(size_type count);

		/// @brief イテレータが指す範囲の要素から多角形の配列を作成します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		template <std::input_iterator Iterator>
		[[nodiscard]]
		MultiPolygon(Iterator first, Iterator last);

		[[nodiscard]]
		MultiPolygon(std::initializer_list<value_type> init);

		/// @brief 範囲から多角形の配列を作成します。
		/// @tparam Range 多角形の範囲
		/// @param tag 範囲から作成することを示すタグ
		/// @param range 多角形の範囲
		template <Concept::ContainerCompatibleRange<Polygon> Range>
		[[nodiscard]]
		MultiPolygon(std::from_range_t tag, Range&& range);

		/// @brief 空の多角形の配列を作成し、`reserve()` します。
		/// @param size `reserve()` するサイズ
		[[nodiscard]]
		explicit MultiPolygon(Arg::reserve_<size_type> size);

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		MultiPolygon& operator =(const MultiPolygon& other);

		MultiPolygon& operator =(MultiPolygon&& other) noexcept;

		MultiPolygon& operator =(const container_type& other);

		MultiPolygon& operator =(container_type&& other) noexcept;

		/// @brief リストから多角形の配列を代入します。
		/// @param list 多角形のリスト
		/// @return *this
		MultiPolygon& operator =(std::initializer_list<value_type> list);

		////////////////////////////////////////////////////////////////
		//
		//	assign
		//
		////////////////////////////////////////////////////////////////

		/// @brief イテレータが指す範囲の要素から配列を作成します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return *this
		template <std::input_iterator Iterator>
		MultiPolygon& assign(Iterator first, Iterator last);

		/// @brief リストから配列を作成します。
		/// @param list リスト
		/// @return *this
		MultiPolygon& assign(std::initializer_list<value_type> list);

		////////////////////////////////////////////////////////////////
		//
		//	assign_range
		//
		////////////////////////////////////////////////////////////////

		/// @brief 範囲から配列を作成します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		/// @return *this
		template <Concept::ContainerCompatibleRange<Polygon> Range>
		MultiPolygon& assign_range(Range&& range);

		////////////////////////////////////////////////////////////////
		//
		//	get_allocator
		//
		////////////////////////////////////////////////////////////////

		/// @brief アロケータを返します。
		/// @return アロケータ
		[[nodiscard]]
		allocator_type get_allocator() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asArray
		//
		////////////////////////////////////////////////////////////////

		/// @brief Array への const 参照を返します。
		/// @return Array への const 参照
		[[nodiscard]]
		const container_type& asArray() const& noexcept;

		/// @brief Array を返します。
		/// @return Array
		[[nodiscard]]
		container_type asArray() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		const value_type& at(size_type index) const&;

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		value_type& at(size_type index)&;

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		value_type at(size_type index)&&;

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		[[nodiscard]]
		const value_type& operator [](size_type index) const& noexcept;

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		[[nodiscard]]
		value_type& operator [](size_type index) & noexcept;

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素
		[[nodiscard]]
		value_type operator [](size_type index) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の要素への参照を返します。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		reference front() & noexcept;

		/// @brief 先頭の要素への参照を返します。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		const_reference front() const& noexcept;

		/// @brief 先頭の要素を返します。
		/// @return 先頭の要素
		[[nodiscard]]
		value_type front() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の要素への参照を返します。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		reference back() & noexcept;

		/// @brief 末尾の要素への参照を返します。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		const_reference back() const& noexcept;

		/// @brief 末尾の要素を返します。
		/// @return 末尾の要素
		[[nodiscard]]
		value_type back() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	data
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の要素を指すポインタを返します。
		/// @return 先頭の要素を指すポインタ
		[[nodiscard]]
		value_type* data() noexcept;

		/// @brief 先頭の要素を指すポインタを返します。
		/// @return 先頭の要素を指すポインタ
		[[nodiscard]]
		const value_type* data() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	begin, end
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭位置を指すイテレータを返します。
		/// @return 配列の先頭位置を指すイテレータ
		[[nodiscard]]
		iterator begin() noexcept;

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		iterator end() noexcept;

		/// @brief 配列の先頭位置を指すイテレータを返します。
		/// @return 配列の先頭位置を指すイテレータ
		[[nodiscard]]
		const_iterator begin() const noexcept;

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		const_iterator end() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	cbegin, cend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭位置を指すイテレータを返します。
		/// @return 配列の先頭位置を指すイテレータ
		[[nodiscard]]
		const_iterator cbegin() const noexcept;

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		const_iterator cend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rbegin, rend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾位置を指すリバース・イテレータを返します。
		/// @return 配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		reverse_iterator rbegin() noexcept;

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		reverse_iterator rend() noexcept;

		/// @brief 配列の末尾位置を指すリバース・イテレータを返します。
		/// @return 配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		const_reverse_iterator rbegin() const noexcept;

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		const_reverse_iterator rend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	crbegin, crend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾位置を指すリバース・イテレータを返します。
		/// @return 配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		const_reverse_iterator crbegin() const noexcept;

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		const_reverse_iterator crend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	empty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列が空であるかを返します。
		/// @return 配列が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool empty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列が空であるかを返します。
		/// @remark `empty()` と同じです。
		/// @return 配列が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmpty() const noexcept;

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
		bool indexInBounds(size_type index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列が要素を持っているかを返します。
		/// @return 配列が要素を持っている場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数を返します。
		/// @return 配列の要素数
		[[nodiscard]]
		size_t size() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	ssize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数を符号付き整数型で返します。 | Return the number of elements in the array as a signed integer type.
		/// @return 配列の要素数
		[[nodiscard]]
		isize ssize() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	max_size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数の最大値を返します。
		/// @return 配列の要素数の最大値
		[[nodiscard]]
		size_t max_size() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reserve
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素数 n までメモリの再確保無しで格納できるよう、必要に応じてメモリを再確保します。
		/// @param n メモリを確保する要素数
		void reserve(size_type n);

		////////////////////////////////////////////////////////////////
		//
		//	capacity
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリの再確保無しで格納できる要素数を返します。
		/// @return メモリの再確保無しで格納できる要素数
		[[nodiscard]]
		size_t capacity() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	shrink_to_fit
		//
		////////////////////////////////////////////////////////////////

		/// @brief 使用するメモリ量を現在のサイズまで切り詰めます。
		/// @remark この関数の呼び出しの後で capacity() == size() になるとは限りません。
		void shrink_to_fit();

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素を削除し、空の配列にします。
		void clear() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	release
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素をすべて消去し、メモリも解放します。
		void release();

		////////////////////////////////////////////////////////////////
		//
		//	insert
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に要素を挿入します。
		/// @param pos 挿入する位置
		/// @param value 挿入する値
		/// @return 挿入された要素を指すイテレータ
		iterator insert(const_iterator pos, const value_type& value);

		/// @brief 指定した位置に要素を挿入します。
		/// @param pos 挿入する位置
		/// @param value 挿入する値
		/// @return 挿入された要素を指すイテレータ
		iterator insert(const_iterator pos, value_type&& value);

		/// @brief 指定した位置にイテレータが指す範囲の要素を挿入します。
		/// @tparam Iterator イテレータ
		/// @param pos 挿入する位置
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return 挿入された要素の先頭を指すイテレータ
		template <std::input_iterator Iterator>
		iterator insert(const_iterator pos, Iterator first, Iterator last);

		/// @brief 指定した位置にリストの要素を挿入します。
		/// @param pos 挿入する位置
		/// @param list リスト
		/// @return 挿入された要素の先頭を指すイテレータ
		iterator insert(const_iterator pos, std::initializer_list<value_type> list);

		////////////////////////////////////////////////////////////////
		//
		//	insert_range
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に範囲の要素を挿入します。
		/// @param pos 挿入する位置
		/// @param range 挿入する要素の範囲
		/// @return 挿入された要素の先頭を指すイテレータ
		template <Concept::ContainerCompatibleRange<Polygon> Range>
		iterator insert_range(const_iterator pos, Range&& range);

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
		iterator emplace(const_iterator pos, Args&&... args);

		////////////////////////////////////////////////////////////////
		//
		//	append_range
		//
		////////////////////////////////////////////////////////////////

		/// @brief 範囲の要素を配列の末尾に追加します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		template <Concept::ContainerCompatibleRange<Polygon> Range>
		void append_range(Range&& range);

		////////////////////////////////////////////////////////////////
		//
		//	erase
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の要素を削除します。
		/// @param pos 削除する要素の位置
		/// @return 削除した要素の次の要素を指すイテレータ
		iterator erase(const_iterator pos);

		/// @brief 指定した範囲の要素を削除します。
		/// @param first 削除する範囲の開始位置
		/// @param last 削除する範囲の終端位置
		/// @return 削除された範囲の次を指すイテレータ
		iterator erase(const_iterator first, const_iterator last);

		////////////////////////////////////////////////////////////////
		//
		//	erase_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの要素を削除します。
		/// @param index 削除する要素のインデックス
		/// @return *this
		MultiPolygon& erase_at(size_type index) &;

		/// @brief 指定したインデックスの要素を削除します。
		/// @param index 削除する要素のインデックス
		/// @return 削除後の配列
		MultiPolygon erase_at(size_type index) &&;

		////////////////////////////////////////////////////////////////
		//
		//	erase_at_unstable
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの要素を削除します。削除後の順序は保証されません。
		/// @param index 削除する要素のインデックス
		/// @return *this
		MultiPolygon& erase_at_unstable(size_type index) &;

		/// @brief 指定したインデックスの要素を削除します。削除後の順序は保証されません。
		/// @param index 削除する要素のインデックス
		/// @return 削除後の MultiPolygon
		[[nodiscard]]
		MultiPolygon erase_at_unstable(size_type index) &&;

		////////////////////////////////////////////////////////////////
		//
		//	erase_all_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たすすべての要素を削除します。
		/// @tparam Fty 要素が条件を満たすかを判定する関数オブジェクトの型
		/// @param f 要素が条件を満たすかを判定する関数オブジェクト
		/// @return 削除した要素の個数
		template <class Fty>
		size_type erase_all_if(Fty f) requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	erase_all_if_unstable
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たすすべての要素を削除します。削除後の順序は保証されません。
		/// @tparam Fty 要素が条件を満たすかを判定する関数オブジェクトの型
		/// @param f 要素が条件を満たすかを判定する関数オブジェクト
		/// @return 削除した要素の個数
		template <class Fty>
		size_type erase_all_if_unstable(Fty f)
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	erase_first_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす最初の要素を削除します。
		/// @tparam Fty 要素が条件を満たすかを判定する関数オブジェクトの型
		/// @param f 要素が条件を満たすかを判定する関数オブジェクト
		/// @return 要素を削除した場合 true, それ以外の場合は false
		template <class Fty>
		bool erase_first_if(Fty f) requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	erase_first_if_unstable
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす最初の要素を削除します。削除後の順序は保証されません。
		/// @tparam Fty 要素が条件を満たすかを判定する関数オブジェクトの型
		/// @param f 要素が条件を満たすかを判定する関数オブジェクト
		/// @return 要素を削除した場合 true, それ以外の場合は false
		template <class Fty>
		bool erase_first_if_unstable(Fty f)
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	push_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に要素を追加します。
		/// @param value 追加する値
		void push_back(const value_type& value);

		/// @brief 配列の末尾に要素をムーブして追加します。
		/// @param value 追加する値
		void push_back(value_type&& value);

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
		reference emplace_back(Args&&... args);

		////////////////////////////////////////////////////////////////
		//
		//	pop_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾の要素を削除します。
		/// @remark 配列が空の時に呼んではいけません。
		void pop_back();

		////////////////////////////////////////////////////////////////
		//
		//	resize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数を変更します。
		/// @param count 新しい要素数
		void resize(size_type count);

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 他の配列と要素を入れ替えます。
		/// @param other 入れ替える配列
		void swap(MultiPolygon& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	push_front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭に要素を追加します。
		/// @param value 追加する値
		void push_front(const value_type& value);

		/// @brief 配列の先頭に要素を追加します。
		/// @param value 追加する値
		void push_front(value_type&& value);

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
		reference emplace_front(Args&&... args);

		////////////////////////////////////////////////////////////////
		//
		//	pop_front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭から要素を削除します。
		void pop_front();

		////////////////////////////////////////////////////////////////
		//
		//	pop_front_N
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭から指定した個数の要素を削除します。
		/// @param n 削除する個数
		/// @remark n が `size()` より多い場合はすべての要素を削除します。
		void pop_front_N(size_type n);

		////////////////////////////////////////////////////////////////
		//
		//	pop_back_N
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾から指定した個数の要素を削除します。
		/// @param n 削除する個数
		/// @remark n が `size()` より多い場合はすべての要素を削除します。
		void pop_back_N(size_type n);

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に要素を追加します。
		/// @param value 追加する値
		/// @return *this
		MultiPolygon& operator <<(const value_type& value);

		/// @brief 配列の末尾に要素をムーブして追加します。
		/// @param value 追加する値
		/// @return *this
		MultiPolygon& operator <<(value_type&& value);

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
		std::span<value_type> subspan(size_type pos, size_type count) noexcept;

		/// @brief 部分配列を指す span を返します。
		/// @param pos 部分配列の開始位置
		/// @param count 部分配列の要素数
		/// @return 部分配列を指す span
		[[nodiscard]]
		std::span<const value_type> subspan(size_type pos, size_type count) const noexcept;

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
		bool all(Fty f) const
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
		template <class Fty>
		[[nodiscard]]
		bool any(Fty f) const
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に別の MultiPolygon の要素を追加します。
		/// @param other 追加する MultiPolygon
		/// @return *this
		MultiPolygon& append(const MultiPolygon& other);

		/// @brief 配列の末尾に別の MultiPolygon の要素をムーブして追加します。
		/// @param other 追加する MultiPolygon
		/// @return *this
		MultiPolygon& append(MultiPolygon&& other);

		/// @brief 配列の末尾に別の配列の要素を追加します。
		/// @param other 追加する配列
		/// @return *this
		MultiPolygon& append(const container_type& other);

		/// @brief 配列の末尾に指定した範囲の要素を追加します。
		/// @tparam Iterator イテレータの型
		/// @param first 範囲の先頭を指すイテレータ
		/// @param last 範囲の終端を指すイテレータ
		/// @return *this
		template <std::input_iterator Iterator>
		MultiPolygon& append(Iterator first, Iterator last);

		/// @brief 配列の末尾にリストの要素を追加します。
		/// @param list 追加する要素のリスト
		/// @return *this
		MultiPolygon& append(std::initializer_list<value_type> list);

		/// @brief 配列の末尾に count 個の value を追加します。
		/// @param count 追加する要素数
		/// @param value 追加する値
		/// @return *this
		MultiPolygon& append(size_type count, const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	choice
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の配列から要素を 1 つランダムに返します。
		/// @return ランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type& choice();

		/// @brief 多角形の配列から要素を 1 つランダムに返します。
		/// @return ランダムに選ばれた要素への参照
		[[nodiscard]]
		const value_type& choice() const;

		/// @brief 指定した乱数エンジンを用いて、多角形の配列から要素を 1 つランダムに返します。
		/// @param urbg 使用する乱数エンジン
		/// @return ランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type& choice(Concept::UniformRandomBitGenerator auto&& urbg);

		/// @brief 指定した乱数エンジンを用いて、多角形の配列から要素を 1 つランダムに返します。
		/// @param urbg 使用する乱数エンジン
		/// @return ランダムに選ばれた要素への参照
		[[nodiscard]]
		const value_type& choice(Concept::UniformRandomBitGenerator auto&& urbg) const;

		/// @brief 多角形の配列から指定した個数だけ重複なくランダムに選んで返します。
		/// @param n 選択する個数
		/// @return ランダムに選ばれた要素の MultiPolygon
		[[nodiscard]]
		MultiPolygon choice(size_t n) const;

		/// @brief 指定した乱数エンジンを用いて、多角形の配列から指定した個数だけ重複なくランダムに選んで返します。
		/// @param n 選択する個数
		/// @param urbg 使用する乱数エンジン
		/// @return ランダムに選ばれた要素の MultiPolygon
		[[nodiscard]]
		MultiPolygon choice(size_t n, Concept::UniformRandomBitGenerator auto&& urbg) const;

		////////////////////////////////////////////////////////////////
		//
		//	chunk
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の配列を指定した個数の要素を持つグループに分割します。最後のグループの要素数は n 個未満になることがあります。
		/// @param n 1 つのグループが持つ要素数
		/// @return 分割された MultiPolygon のグループ
		[[nodiscard]]
		Array<MultiPolygon> chunk(size_type n) const;

		////////////////////////////////////////////////////////////////
		//
		//	contains_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素があるかを返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 条件を満たす要素が 1 つでもあれば true, それ以外の場合は false
		template <class Fty>
		[[nodiscard]]
		bool contains_if(Fty f) const
			requires std::predicate<Fty&, const value_type&>;

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
		isize count_if(Fty f) const
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
		void each(Fty f)
			requires std::invocable<Fty&, value_type&>;

		/// @brief すべての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		void each(Fty f) const
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
		void each_index(Fty f)
			requires std::invocable<Fty&, size_t, value_type&>;

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		void each_index(Fty f) const
			requires std::invocable<Fty&, size_t, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	each_sindex
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素とその符号付きインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		void each_sindex(Fty f)
			requires std::invocable<Fty&, isize, value_type&>;

		/// @brief すべての要素とその符号付きインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		void each_sindex(Fty f) const
			requires std::invocable<Fty&, isize, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	fetch
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの要素を返します。範囲外の場合はデフォルト値を返します。
		/// @tparam U デフォルト値の型
		/// @param index インデックス
		/// @param defaultValue 範囲外の場合に返すデフォルト値
		/// @return 指定したインデックスの要素、範囲外の場合は defaultValue
		template <class U>
		[[nodiscard]]
		value_type fetch(size_type index, U&& defaultValue) const
			noexcept(std::is_nothrow_constructible_v<value_type, U> && std::is_nothrow_copy_constructible_v<value_type>)
			requires std::constructible_from<value_type, U>;

		////////////////////////////////////////////////////////////////
		//
		//	fill
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素に同じ値を代入します。
		/// @param value 代入する値
		/// @return *this
		MultiPolygon& fill(const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	filter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素だけを集めた新しい MultiPolygon を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 条件を満たす要素を集めた新しい MultiPolygon
		template <class Fty>
		[[nodiscard]]
		MultiPolygon filter(Fty f) const
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	fold_left
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を左から順番に関数へ適用し、1 つの値にまとめます。
		/// @tparam R 初期値の型
		/// @tparam Fty 適用する関数の型
		/// @param init 初期値
		/// @param f 適用する関数
		/// @return まとめられた値
		template <class R, class Fty>
		auto fold_left(R init, Fty f) const;

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
		auto map(Fty f) const
			requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	none
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす要素が存在しないかを返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 条件を満たす要素が存在しなければ true, それ以外の場合は false
		template <class Fty>
		[[nodiscard]]
		bool none(Fty f) const
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	slice
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した範囲の要素からなる新しい MultiPolygon を返します。
		/// @param index 開始インデックス
		/// @param length 要素数
		/// @return 新しい MultiPolygon
		[[nodiscard]]
		MultiPolygon slice(size_type index, size_type length) const&;

		/// @brief 指定した範囲の要素からなる新しい MultiPolygon を返します。
		/// @param index 開始インデックス
		/// @param length 要素数
		/// @return 新しい MultiPolygon
		[[nodiscard]]
		MultiPolygon slice(size_type index, size_type length) &&;

		////////////////////////////////////////////////////////////////
		//
		//	head
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から最大 n 個の要素を含む新しい MultiPolygon を返します。
		/// @param n 取り出す最大要素数
		/// @return 新しい MultiPolygon
		[[nodiscard]]
		MultiPolygon head(size_type n) const&;

		/// @brief 先頭から最大 n 個の要素を含む新しい MultiPolygon を返します。
		/// @param n 取り出す最大要素数
		/// @return 新しい MultiPolygon
		[[nodiscard]]
		MultiPolygon head(size_type n) &&;

		////////////////////////////////////////////////////////////////
		//
		//	head_span
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から最大 n 個の要素を参照する span を返します。
		[[nodiscard]]
		std::span<value_type> head_span(size_type n) & noexcept;

		/// @brief 先頭から最大 n 個の要素を参照する span を返します。
		[[nodiscard]]
		std::span<const value_type> head_span(size_type n) const& noexcept;

		std::span<value_type> head_span(size_type n) && = delete;

		////////////////////////////////////////////////////////////////
		//
		//	head_view
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から最大 n 個の要素を参照するビューを返します。
		[[nodiscard]]
		auto head_view(size_type n) & noexcept;

		/// @brief 先頭から最大 n 個の要素を参照するビューを返します。
		[[nodiscard]]
		auto head_view(size_type n) const& noexcept;

		/// @brief 先頭から最大 n 個の要素を保持するビューを返します。
		[[nodiscard]]
		auto head_view(size_type n) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	in_groups
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の配列を指定したグループ数に分割します。
		/// @param group グループ数
		/// @remark group が要素数より大きい場合、空のグループは作られず、返されるグループ数は要素数になります。
		/// @return 分割した MultiPolygon のグループ
		[[nodiscard]]
		Array<MultiPolygon> in_groups(size_type group) const;

		////////////////////////////////////////////////////////////////
		//
		//	tail
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の最大 n 個の要素を含む新しい MultiPolygon を返します。
		/// @param n 取り出す最大要素数
		/// @return 新しい MultiPolygon
		[[nodiscard]]
		MultiPolygon tail(size_type n) const&;

		/// @brief 末尾の最大 n 個の要素を含む新しい MultiPolygon を返します。
		/// @param n 取り出す最大要素数
		/// @return 新しい MultiPolygon
		[[nodiscard]]
		MultiPolygon tail(size_type n) &&;

		////////////////////////////////////////////////////////////////
		//
		//	tail_span
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の最大 n 個の要素を参照する span を返します。
		[[nodiscard]]
		std::span<value_type> tail_span(size_type n) & noexcept;

		/// @brief 末尾の最大 n 個の要素を参照する span を返します。
		[[nodiscard]]
		std::span<const value_type> tail_span(size_type n) const& noexcept;

		std::span<value_type> tail_span(size_type n) && = delete;

		////////////////////////////////////////////////////////////////
		//
		//	tail_view
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の最大 n 個の要素を参照するビューを返します。
		[[nodiscard]]
		auto tail_view(size_type n) & noexcept;

		/// @brief 末尾の最大 n 個の要素を参照するビューを返します。
		[[nodiscard]]
		auto tail_view(size_type n) const& noexcept;

		/// @brief 末尾の最大 n 個の要素を保持するビューを返します。
		[[nodiscard]]
		auto tail_view(size_type n) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	take
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から最大 n 個の要素を含む新しい MultiPolygon を返します。
		/// @param n 取り出す最大要素数
		/// @return 新しい MultiPolygon
		[[nodiscard]]
		MultiPolygon take(size_type n) const&;

		/// @brief 先頭から最大 n 個の要素を含む新しい MultiPolygon を返します。
		/// @param n 取り出す最大要素数
		/// @return 新しい MultiPolygon
		[[nodiscard]]
		MultiPolygon take(size_type n) &&;

		////////////////////////////////////////////////////////////////
		//
		//	take_while
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から条件を満たさなくなる直前までの要素からなる新しい MultiPolygon を返します。
		template <class Fty>
		[[nodiscard]]
		MultiPolygon take_while(Fty f) const&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 先頭から条件を満たさなくなる直前までの要素からなる新しい MultiPolygon を返します。
		template <class Fty>
		[[nodiscard]]
		MultiPolygon take_while(Fty f) &&
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	values_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの要素からなる新しい MultiPolygon を返します。
		/// @param indices インデックス
		/// @return 新しい MultiPolygon
		[[nodiscard]]
		MultiPolygon values_at(std::initializer_list<size_type> indices) const;

		////////////////////////////////////////////////////////////////
		//
		//	without_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの要素を除いた新しい MultiPolygon を返します。
		[[nodiscard]]
		MultiPolygon without_at(size_type index) const&;

		/// @brief 指定したインデックスの要素を除いた新しい MultiPolygon を返します。
		[[nodiscard]]
		MultiPolygon without_at(size_type index) &&;

		////////////////////////////////////////////////////////////////
		//
		//	without_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす要素を除いた新しい MultiPolygon を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 条件を満たす要素を除いた MultiPolygon
		template <class Fty>
		[[nodiscard]]
		MultiPolygon without_if(Fty f) const&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 条件を満たす要素を除いた新しい MultiPolygon を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 条件を満たす要素を除いた MultiPolygon
		template <class Fty>
		[[nodiscard]]
		MultiPolygon without_if(Fty f) &&
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	replace_if, replaced_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たすすべての要素を別の値に置き換えます。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @param newValue 新しい値
		/// @return *this
		template <class Fty>
		MultiPolygon& replace_if(Fty f, const value_type& newValue) &
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 条件を満たすすべての要素を別の値に置き換えた MultiPolygon を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @param newValue 新しい値
		/// @return 置き換え後の MultiPolygon
		template <class Fty>
		[[nodiscard]]
		MultiPolygon replace_if(Fty f, const value_type& newValue) &&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 条件を満たすすべての要素を別の値に置き換えた MultiPolygon を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @param newValue 新しい値
		/// @return 置き換え後の MultiPolygon
		template <class Fty>
		[[nodiscard]]
		MultiPolygon replaced_if(Fty f, const value_type& newValue) const&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 条件を満たすすべての要素を別の値に置き換えた MultiPolygon を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @param newValue 新しい値
		/// @return 置き換え後の MultiPolygon
		template <class Fty>
		[[nodiscard]]
		MultiPolygon replaced_if(Fty f, const value_type& newValue) &&
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	reverse, reversed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を逆順に並び替えます。
		/// @return *this
		MultiPolygon& reverse() &;

		/// @brief 要素を逆順に並び替えた MultiPolygon を返します。
		/// @return 逆順に並び替えた MultiPolygon
		[[nodiscard]]
		MultiPolygon reverse() &&;

		/// @brief 要素を逆順に並び替えた MultiPolygon を返します。
		/// @return 逆順に並び替えた MultiPolygon
		[[nodiscard]]
		MultiPolygon reversed() const&;

		/// @brief 要素を逆順に並び替えた MultiPolygon を返します。
		/// @return 逆順に並び替えた MultiPolygon
		[[nodiscard]]
		MultiPolygon reversed() &&;

		////////////////////////////////////////////////////////////////
		//
		//	reverse_each
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素を逆順に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		void reverse_each(Fty f)
			requires std::invocable<Fty&, value_type&>;

		/// @brief すべての要素を逆順に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		template <class Fty>
		void reverse_each(Fty f) const
			requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	reverse_view
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を逆順に参照するビューを返します。
		/// @return 要素を逆順に参照するビュー
		[[nodiscard]]
		auto reverse_view() &;

		/// @brief 要素を逆順に参照するビューを返します。
		/// @return 要素を逆順に参照するビュー
		[[nodiscard]]
		auto reverse_view() const&;

		/// @brief 要素を逆順に保持するビューを返します。
		/// @return 要素を逆順に保持するビュー
		[[nodiscard]]
		auto reverse_view() &&;

		////////////////////////////////////////////////////////////////
		//
		//	shuffle, shuffled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素の並び順をランダムにシャッフルします。
		/// @return *this
		MultiPolygon& shuffle() &;

		/// @brief 要素の並び順をランダムにシャッフルした MultiPolygon を返します。
		/// @return シャッフルした MultiPolygon
		[[nodiscard]]
		MultiPolygon shuffle() &&;

		/// @brief 要素の並び順をランダムにシャッフルした MultiPolygon を返します。
		/// @return シャッフルした MultiPolygon
		[[nodiscard]]
		MultiPolygon shuffled() const&;

		/// @brief 要素の並び順をランダムにシャッフルした MultiPolygon を返します。
		/// @return シャッフルした MultiPolygon
		[[nodiscard]]
		MultiPolygon shuffled() &&;

		/// @brief 指定した乱数エンジンを用いて要素をシャッフルします。
		/// @param urbg 使用する乱数エンジン
		/// @return *this
		MultiPolygon& shuffle(Concept::UniformRandomBitGenerator auto&& urbg) &;

		/// @brief 指定した乱数エンジンを用いて要素をシャッフルした MultiPolygon を返します。
		/// @param urbg 使用する乱数エンジン
		/// @return シャッフルした MultiPolygon
		[[nodiscard]]
		MultiPolygon shuffle(Concept::UniformRandomBitGenerator auto&& urbg) &&;

		/// @brief 指定した乱数エンジンを用いて要素をシャッフルした MultiPolygon を返します。
		/// @param urbg 使用する乱数エンジン
		/// @return シャッフルした MultiPolygon
		[[nodiscard]]
		MultiPolygon shuffled(Concept::UniformRandomBitGenerator auto&& urbg) const&;

		/// @brief 指定した乱数エンジンを用いて要素をシャッフルした MultiPolygon を返します。
		/// @param urbg 使用する乱数エンジン
		/// @return シャッフルした MultiPolygon
		[[nodiscard]]
		MultiPolygon shuffled(Concept::UniformRandomBitGenerator auto&& urbg) &&;

		////////////////////////////////////////////////////////////////
		//
		//	sort_by, sorted_by
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した比較関数を用いて要素を並び替えます。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return *this
		template <class Fty>
		MultiPolygon& sort_by(Fty f) &
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した比較関数を用いて要素を並び替えた MultiPolygon を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 並び替えた MultiPolygon
		template <class Fty>
		[[nodiscard]]
		MultiPolygon sort_by(Fty f) &&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した比較関数を用いて要素を並び替えた MultiPolygon を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 並び替えた MultiPolygon
		template <class Fty>
		[[nodiscard]]
		MultiPolygon sorted_by(Fty f) const&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した比較関数を用いて要素を並び替えた MultiPolygon を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 並び替えた MultiPolygon
		template <class Fty>
		[[nodiscard]]
		MultiPolygon sorted_by(Fty f) &&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	stable_sort_by, stable_sorted_by
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した比較関数を用いて要素を相対順序を保ちながら並び替えます。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return *this
		template <class Fty>
		MultiPolygon& stable_sort_by(Fty f) &
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した比較関数を用いて要素を相対順序を保ちながら並び替えた MultiPolygon を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 安定ソートした MultiPolygon
		template <class Fty>
		[[nodiscard]]
		MultiPolygon stable_sort_by(Fty f) &&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した比較関数を用いて要素を相対順序を保ちながら並び替えた MultiPolygon を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 安定ソートした MultiPolygon
		template <class Fty>
		[[nodiscard]]
		MultiPolygon stable_sorted_by(Fty f) const&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した比較関数を用いて要素を相対順序を保ちながら並び替えた MultiPolygon を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 安定ソートした MultiPolygon
		template <class Fty>
		[[nodiscard]]
		MultiPolygon stable_sorted_by(Fty f) &&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各要素に関数を適用します。
		/// @tparam Fty 各要素に適用する関数の型
		/// @param f 各要素に適用する関数
		/// @remark Fty が戻り値を持たない場合は `.each(f)`、戻り値を持つ場合は `.map(f)` と同じです。
		/// @return 各要素に関数を適用した結果の配列。Fty が戻り値を持たない場合は void
		template <class Fty>
		auto operator >>(Fty f) const
			requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 平行移動した多角形を返します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return 平行移動した多角形
		[[nodiscard]]
		MultiPolygon movedBy(double x, double y) const&;

		/// @brief 平行移動した多角形を返します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return 平行移動した多角形
		[[nodiscard]]
		MultiPolygon movedBy(double x, double y) && noexcept;

		/// @brief 平行移動した多角形を返します。
		/// @param v 移動量
		/// @return 平行移動した多角形
		[[nodiscard]]
		MultiPolygon movedBy(Vec2 v) const&;

		/// @brief 平行移動した多角形を返します。
		/// @param v 移動量
		/// @return 平行移動した多角形
		[[nodiscard]]
		MultiPolygon movedBy(Vec2 v) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を平行移動します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return *this
		MultiPolygon& moveBy(double x, double y) noexcept;

		/// @brief 多角形を平行移動します。
		/// @param v 移動量
		/// @return *this
		MultiPolygon& moveBy(Vec2 v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffset
		//
		////////////////////////////////////////////////////////////////

		MultiPolygon withOffset(double x, double y) const&;

		MultiPolygon withOffset(double x, double y) && noexcept;

		MultiPolygon withOffset(Vec2 v) const&;

		MultiPolygon withOffset(Vec2 v) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffsetX, withOffsetY
		//
		////////////////////////////////////////////////////////////////

		MultiPolygon withOffsetX(double x) const&;

		MultiPolygon withOffsetX(double x) && noexcept;

		MultiPolygon withOffsetY(double y) const&;

		MultiPolygon withOffsetY(double y) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		MultiPolygon rotated(double angle) const&;

		[[nodiscard]]
		MultiPolygon rotated(double angle)&&;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAt
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		MultiPolygon rotatedAt(Vec2 pos, double angle) const&;

		[[nodiscard]]
		MultiPolygon rotatedAt(Vec2 pos, double angle)&&;

		////////////////////////////////////////////////////////////////
		//
		//	rotate
		//
		////////////////////////////////////////////////////////////////

		MultiPolygon& rotate(double angle);

		////////////////////////////////////////////////////////////////
		//
		//	rotateAt
		//
		////////////////////////////////////////////////////////////////

		MultiPolygon& rotateAt(Vec2 pos, double angle);

		////////////////////////////////////////////////////////////////
		//
		//	transformed
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		MultiPolygon transformed(double s, double c, const Vec2& pos) const&;

		[[nodiscard]]
		MultiPolygon transformed(double s, double c, const Vec2& pos)&&;

		////////////////////////////////////////////////////////////////
		//
		//	transform
		//
		////////////////////////////////////////////////////////////////

		MultiPolygon& transform(double s, double c, const Vec2& pos);

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromOrigin
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		MultiPolygon scaledFromOrigin(double s) const&;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		MultiPolygon scaledFromOrigin(double s)&&;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		MultiPolygon scaledFromOrigin(double sx, double sy) const&;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		MultiPolygon scaledFromOrigin(double sx, double sy)&&;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		MultiPolygon scaledFromOrigin(Vec2 s) const&;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		MultiPolygon scaledFromOrigin(Vec2 s)&&;

		////////////////////////////////////////////////////////////////
		//
		//	scaleFromOrigin
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点 (0, 0) を中心に拡大・縮小します。
		/// @param s 拡大率
		/// @return *this
		MultiPolygon& scaleFromOrigin(double s);

		/// @brief 原点 (0, 0) を中心に拡大・縮小します。
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return *this
		MultiPolygon& scaleFromOrigin(double sx, double sy);

		/// @brief 原点 (0, 0) を中心に拡大・縮小します。
		/// @param s 拡大率
		/// @return *this
		MultiPolygon& scaleFromOrigin(Vec2 s);

		////////////////////////////////////////////////////////////////
		//
		//	scaledFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		/// @param pos 拡大・縮小の中心位置
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		MultiPolygon scaledFrom(Vec2 pos, double s) const&;

		/// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		/// @param pos 拡大・縮小の中心位置
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		MultiPolygon scaledFrom(Vec2 pos, double s)&&;

		/// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		/// @param pos 拡大・縮小の中心位置
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		MultiPolygon scaledFrom(Vec2 pos, double sx, double sy) const&;

		/// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		/// @param pos 拡大・縮小の中心位置
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		MultiPolygon scaledFrom(Vec2 pos, double sx, double sy)&&;

		/// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		/// @param pos 拡大・縮小の中心位置
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		MultiPolygon scaledFrom(Vec2 pos, Vec2 s) const&;

		/// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		/// @param pos 拡大・縮小の中心位置
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		MultiPolygon scaledFrom(Vec2 pos, Vec2 s)&&;

		////////////////////////////////////////////////////////////////
		//
		//	scaleFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を中心に拡大・縮小します。
		/// @param pos 拡大・縮小の中心位置
		/// @param s 拡大率
		/// @return *this
		MultiPolygon& scaleFrom(Vec2 pos, double s);

		/// @brief 指定した位置を中心に拡大・縮小します。
		/// @param pos 拡大・縮小の中心位置
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return *this
		MultiPolygon& scaleFrom(Vec2 pos, double sx, double sy);

		/// @brief 指定した位置を中心に拡大・縮小します。
		/// @param pos 拡大・縮小の中心位置
		/// @param s 拡大率
		/// @return *this
		MultiPolygon& scaleFrom(Vec2 pos, Vec2 s);

		////////////////////////////////////////////////////////////////
		//
		//	area
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の配列面積を返します。
		/// @return 多角形の配列の面積
		[[nodiscard]]
		double area() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	perimeter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の配列の、穴を含めた輪郭の長さを返します。
		/// @return 多角形の配列の、穴を含めた輪郭の長さ
		[[nodiscard]]
		double perimeter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	centroid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の配列の重心の座標を返します。
		/// @return 多角形の配列の重心の座標、面積を持たない場合は none
		[[nodiscard]]
		Optional<Vec2> centroid() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	intersects
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の図形と交差しているかを返します。
		/// @tparam Shape2DType 別の図形の型
		/// @param other 別の図形
		/// @return 別の図形と交差している場合 true, それ以外の場合は false
		template <class Shape2DType>
		[[nodiscard]]
		constexpr bool intersects(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	overlaps
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の図形と交差する領域が面積を持つかを返します。
		/// @tparam Shape2DType 別の図形の型
		/// @param other 別の図形
		/// @return 別の図形と交差する領域が面積を持つ場合 true, それ以外の場合は false
		template <class Shape2DType>
		[[nodiscard]]
		constexpr bool overlaps(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	contains
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の図形を完全に含んでいるかを返します。
		/// @tparam Shape2DType 別の図形の型
		/// @param other 別の図形
		/// @return 別の図形を完全に含んでいる場合 true, それ以外の場合は false
		/// @remark 制約: 現在の実装では、複数の要素にまたがって完全に含まれる図形に対して false を返します。
		template <class Shape2DType>
		[[nodiscard]]
		constexpr bool contains(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	intersectsAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の図形と点で交差している場合、その座標を返します。
		/// @tparam Shape2DType 別の図形の型
		/// @param other 別の図形
		/// @return 別の図形と点で交差している場合、その座標の配列を返します。交差が存在しても、一次元以上の共有部分しかない場合は空の配列を返します。交差していない場合は none を返します。
		template <class Shape2DType>
		[[nodiscard]]
		Optional<Array<Vec2>> intersectsAt(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	paint
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての多角形を Image に描き込みます。
		/// @param dst 描き込み先の Image
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const MultiPolygon& paint(Image& dst, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief すべての多角形を移動させた位置で Image に描き込みます。
		/// @param dst 描き込み先の Image
		/// @param offset 座標のオフセット
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const MultiPolygon& paint(Image& dst, const Vec2& offset, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての多角形を描画します。
		/// @param color 色
		/// @return *this
		const MultiPolygon& draw(const ColorF& color = Palette::White) const;

		/// @brief すべての多角形を移動させた位置に描画します。
		/// @param offset 座標のオフセット
		/// @param color 色
		/// @return *this
		const MultiPolygon& draw(const Vec2& offset, const ColorF& color = Palette::White) const;

		/// @brief すべての多角形を塗りつぶしパターンで描画します。
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const MultiPolygon& draw(const PatternParameters& pattern) const;

		/// @brief すべての多角形を移動させた位置に塗りつぶしパターンで描画します。
		/// @param offset 座標のオフセット
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const MultiPolygon& draw(const Vec2& offset, const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの配列を入れ替えます。
		/// @param lhs 一方の配列
		/// @param rhs もう一方の配列
		friend void swap(MultiPolygon& lhs, MultiPolygon& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const MultiPolygon& value);

	private:

		container_type m_polygons;
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::MultiPolygon, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::MultiPolygon& value, s3d::BufferContext& ctx) const;
};

# include "detail/MultiPolygon.ipp"
