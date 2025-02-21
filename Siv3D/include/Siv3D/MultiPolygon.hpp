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
# include "Array.hpp"
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
		template <class Iterator>
		[[nodiscard]]
		MultiPolygon(Iterator first, Iterator last);

		[[nodiscard]]
		MultiPolygon(std::initializer_list<value_type> init);

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
		template <class Iterator>
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

		/// @brief 指定した位置に count 個の value を挿入します。
		/// @param pos 挿入する位置
		/// @param count 挿入する個数
		/// @param value 挿入する値
		/// @return 挿入された要素の先頭を指すイテレータ
		iterator insert(const_iterator pos, size_type count, const value_type& value);

		/// @brief 指定した位置にイテレータが指す範囲の要素を挿入します。
		/// @tparam Iterator イテレータ
		/// @param pos 挿入する位置
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return 挿入された要素の先頭を指すイテレータ
		template <class Iterator>
		iterator insert(const_iterator pos, Iterator first, Iterator last);

		/// @brief 指定した位置にリストの要素を挿入します。
		/// @param pos 挿入する位置
		/// @param list リスト
		/// @return 挿入された要素の先頭を指すイテレータ
		iterator insert(const_iterator pos, std::initializer_list<value_type> list);

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
		template <Concept::ContainerCompatibleRange<Vec2> Range>
		void append_range(Range&& range);

		/// @brief 範囲の要素を配列の末尾に追加します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		template <Concept::ContainerCompatibleRange<Point> Range>
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
		//	push_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に要素を追加します。
		/// @param value 追加する値
		void push_back(const value_type& value);

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
