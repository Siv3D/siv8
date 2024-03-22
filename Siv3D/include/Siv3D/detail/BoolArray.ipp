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
	template <class Allocator>
	class Array<bool, Allocator>
	{
	public:

		using container_type			= std::basic_string<bool, std::char_traits<bool>, Allocator>;

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
		constexpr Array(const container_type& other)
			: m_container(other) {}

		/// @brief std::vector からのムーブで配列を作成します。
		/// @param other ムーブする配列
		[[nodiscard]]
		constexpr Array(container_type&& other)
			: m_container(std::move(other)) {}

		/// @brief アロケータを指定して空の配列を作成します。
		/// @param alloc アロケータ
		[[nodiscard]]
		explicit constexpr Array(const Allocator& alloc) noexcept
			: m_container(alloc) {}

		/// @brief count 個の value で配列を作成します。
		/// @param count 個数
		/// @param value 要素
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr Array(size_type count, value_type value, const Allocator& alloc = Allocator{})
			: m_container(count, value, alloc) {}

		/// @brief count 個の bool() で配列を作成します。
		/// @param count 個数
		/// @param alloc アロケータ
		[[nodiscard]]
		explicit constexpr Array(size_type count, const Allocator& alloc = Allocator{})
			: m_container(count, alloc) {}

		/// @brief イテレータが指す範囲の要素から配列を作成します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @param alloc アロケータ
		template <class Iterator>
		[[nodiscard]]
		constexpr Array(Iterator first, Iterator last, const Allocator& alloc = Allocator{})
			: m_container(first, last, alloc) {}

		/// @brief アロケータを指定して、別の配列から配列を作成します。
		/// @param other コピーする配列
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr Array(const Array& other, const Allocator& alloc)
			: m_container(other.m_container, alloc) {}

		/// @brief アロケータを指定して、別の配列から配列を作成します。
		/// @param other ムーブする配列
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr Array(Array&& other, const Allocator& alloc)
			: m_container(std::move(other.m_container), alloc) {}

		/// @brief アロケータを指定して、std::vector から配列を作成します。
		/// @param other コピーする配列
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr Array(const container_type& other, const Allocator& alloc)
			: m_container(other, alloc) {}

		/// @brief アロケータを指定して、std::vector から配列を作成します。
		/// @param other ムーブする配列
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr Array(container_type&& other, const Allocator& alloc)
			: m_container(std::move(other), alloc) {}

		/// @brief メンバ関数 `.asArray()` を持つ型から配列を作成します。
		/// @tparam ArrayIsh メンバ関数 `.asArray()` を持つ型
		/// @param a `.asArray()` を持つ型のオブジェクト
		[[nodiscard]]
		explicit constexpr Array(const HasAsArray auto& a)
			: Array{ a.asArray() } {}

		/// @brief メンバ関数 `.asArray()` を持つ型から配列を作成します。
		/// @tparam ArrayIsh メンバ関数 `.asArray()` を持つ型
		/// @param a `.asArray()` を持つ型のオブジェクト
		[[nodiscard]]
		explicit constexpr Array(HasAsArray auto&& a)
			: Array{ std::forward<decltype(a)>(a).asArray() } {}

		/// @brief 初期化リストから配列を作成します。
		/// @param list 初期化リスト
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr Array(std::initializer_list<value_type> list, const Allocator& alloc = Allocator{})
			: m_container(list, alloc) {}

	# ifdef __cpp_lib_containers_ranges
		
		/// @brief 範囲から配列を作成します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		/// @param alloc アロケータ
		template <Concept::ContainerCompatibleRange<bool> Range>
		[[nodiscard]]
		constexpr Array(std::from_range_t, Range&& range, const Allocator& alloc = Allocator{})
			: m_container(std::from_range, std::forward<Range>(range), alloc) {}

	# endif
		
		/// @brief ジェネレータ関数を使って配列を作成します。
		/// @tparam Fty ジェネレータ関数の型
		/// @param size 作成する配列の要素数
		/// @param generator ジェネレータ関数
		template <class Fty> requires (std::invocable<Fty> && std::convertible_to<std::invoke_result_t<Fty>, value_type>)
		[[nodiscard]]
		constexpr Array(size_type size, Arg::generator_<Fty> generator)
			: Array(Generate<Fty>(size, *generator)) {}

		/// @brief インデックス指定ジェネレータ関数を使って配列を作成します。
		/// @tparam Fty ジェネレータ関数の型
		/// @param size 作成する配列の要素数
		/// @param indexedGenerator インデックス指定ジェネレータ関数
		template <class Fty>
		[[nodiscard]]
		constexpr Array(size_type size, Arg::indexedGenerator_<Fty> indexedGenerator)
			: Array(IndexedGenerate<Fty>(size, *indexedGenerator)) {}

		/// @brief 配列を作成し、`reserve()` します。
		/// @param size `reserve()` するサイズ
		[[nodiscard]]
		explicit constexpr Array(Arg::reserve_<size_type> size)
		{
			m_container.reserve(*size);
		}

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
		constexpr Array& operator =(const container_type& other)
		{
			m_container = other;
			return *this;
		}

		/// @brief ムーブ代入演算子
		/// @param other ムーブする配列
		/// @return *this
		constexpr Array& operator =(container_type&& other)
		{
			m_container = std::move(other);
			return *this;
		}

		/// @brief コピー代入演算子
		/// @param a メンバ関数 `.asArray()` を持つ型
		/// @return *this
		constexpr Array& operator =(const HasAsArray auto& a)
		{
			m_container = a.asArray();
			return *this;
		}

		/// @brief ムーブ代入演算子
		/// @param a メンバ関数 `.asArray()` を持つ型
		/// @return *this
		constexpr Array& operator =(HasAsArray auto&& a)
		{
			m_container = std::forward<decltype(a)>(a).asArray();
			return *this;
		}

		/// @brief コピー代入演算子
		/// @param list リスト
		/// @return *this
		constexpr Array& operator =(std::initializer_list<value_type> list)
		{
			m_container = list;
			return *this;
		}

		////////////////////////////////////////////////////////////////
		//
		//	assign
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した個数の要素からなる配列を作成します。
		/// @param count 要素数
		/// @param value 要素の値
		/// @return *this
		constexpr Array& assign(size_type count, value_type value)
		{
			m_container.assign(count, value);
			return *this;
		}

		/// @brief イテレータが指す範囲の要素から配列を作成します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return *this
		template <class Iterator>
		constexpr Array& assign(Iterator first, Iterator last)
		{
			m_container.assign(first, last);
			return *this;
		}

		/// @brief リストから配列を作成します。
		/// @param list リスト
		/// @return *this
		constexpr Array& assign(std::initializer_list<value_type> list)
		{
			m_container.assign(list);
			return *this;
		}

		////////////////////////////////////////////////////////////////
		//
		//	assign_range
		//
		////////////////////////////////////////////////////////////////

		/// @brief 範囲から配列を作成します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		/// @return *this
		template <Concept::ContainerCompatibleRange<bool> Range>
		constexpr Array& assign_range(Range&& range)
		{
			m_container.assign_range(std::forward<Range>(range));
			return *this;
		}

		////////////////////////////////////////////////////////////////
		//
		//	get_allocator
		//
		////////////////////////////////////////////////////////////////

		/// @brief アロケータを返します。
		/// @return アロケータ
		[[nodiscard]]
		constexpr allocator_type get_allocator() const noexcept
		{
			return m_container.get_allocator();
		}

		////////////////////////////////////////////////////////////////
		//
		//	getContainer
		//
		////////////////////////////////////////////////////////////////

		/// @brief std::basic_string&gt;bool&lt; への const 参照を返します。
		/// @return std::basic_string&gt;bool&lt; への const 参照
		[[nodiscard]]
		constexpr const container_type& getContainer() const noexcept
		{
			return m_container;
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator container_type
		//
		////////////////////////////////////////////////////////////////

		/// @brief std::basic_string&gt;bool&lt; への暗黙の変換を行います。
		[[nodiscard]]
		constexpr operator container_type() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		/// @return 要素への参照
		constexpr const value_type& at(size_type index) const
		{
			return m_container.at(index);
		}

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		constexpr value_type& at(size_type index)
		{
			return m_container.at(index);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		[[nodiscard]]
		constexpr const value_type& operator [](size_type index) const noexcept
		{
			return m_container[index];
		}

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		[[nodiscard]]
		constexpr value_type& operator [](size_type index) noexcept
		{
			return m_container[index];
		}

		////////////////////////////////////////////////////////////////
		//
		//	front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の要素への参照を返します。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		constexpr reference front() noexcept
		{
			return m_container.front();
		}

		/// @brief 先頭の要素への参照を返します。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		constexpr const_reference front() const noexcept
		{
			return m_container.front();
		}

		////////////////////////////////////////////////////////////////
		//
		//	back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の要素への参照を返します。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		constexpr reference back() noexcept
		{
			return m_container.back();
		}

		/// @brief 末尾の要素への参照を返します。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		constexpr const_reference back() const noexcept
		{
			return m_container.back();
		}

		////////////////////////////////////////////////////////////////
		//
		//	data
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の要素を指すポインタを返します。
		/// @return 先頭の要素を指すポインタ
		[[nodiscard]]
		constexpr value_type* data() noexcept
		{
			return m_container.data();
		}

		/// @brief 先頭の要素を指すポインタを返します。
		/// @return 先頭の要素を指すポインタ
		[[nodiscard]]
		constexpr const value_type* data() const noexcept
		{
			return m_container.data();
		}

		////////////////////////////////////////////////////////////////
		//
		//	begin, end
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭位置を指すイテレータを返します。
		/// @return 配列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr iterator begin() noexcept
		{
			return m_container.begin();
		}

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr iterator end() noexcept
		{
			return m_container.end();
		}

		/// @brief 配列の先頭位置を指すイテレータを返します。
		/// @return 配列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator begin() const noexcept
		{
			return m_container.begin();
		}

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator end() const noexcept
		{
			return m_container.end();
		}

		////////////////////////////////////////////////////////////////
		//
		//	cbegin, cend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭位置を指すイテレータを返します。
		/// @return 配列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator cbegin() const noexcept
		{
			return m_container.cbegin();
		}

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator cend() const noexcept
		{
			return m_container.cend();
		}

		////////////////////////////////////////////////////////////////
		//
		//	rbegin, rend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾位置を指すリバース・イテレータを返します。
		/// @return 配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr reverse_iterator rbegin() noexcept
		{
			return m_container.rbegin();
		}

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr reverse_iterator rend() noexcept
		{
			return m_container.rend();
		}

		/// @brief 配列の末尾位置を指すリバース・イテレータを返します。
		/// @return 配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rbegin() const noexcept
		{
			return m_container.rbegin();
		}

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rend() const noexcept
		{
			return m_container.rend();
		}

		////////////////////////////////////////////////////////////////
		//
		//	crbegin, crend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾位置を指すリバース・イテレータを返します。
		/// @return 配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator crbegin() const noexcept
		{
			return m_container.crbegin();
		}

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator crend() const noexcept
		{
			return m_container.crend();
		}

		////////////////////////////////////////////////////////////////
		//
		//	empty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列が空であるかを返します。
		/// @return 配列が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool empty() const noexcept
		{
			return m_container.empty();
		}

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列が空であるかを返します。
		/// @remark `empty()` と同じです。
		/// @return 配列が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isEmpty() const noexcept
		{
			return m_container.empty();
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列が要素を持っているかを返します。
		/// @return 配列が要素を持っている場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept
		{
			return (not m_container.empty());
		}

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数を返します。
		/// @return 配列の要素数
		[[nodiscard]]
		constexpr size_t size() const noexcept
		{
			return m_container.size();
		}

		////////////////////////////////////////////////////////////////
		//
		//	ssize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数を符号付き整数型で返します。 | Return the number of elements in the array as a signed integer type.
		/// @return 配列の要素数
		[[nodiscard]]
		constexpr isize ssize() const noexcept
		{
			return static_cast<isize>(m_container.size());
		}

		////////////////////////////////////////////////////////////////
		//
		//	size_bytes
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素の合計サイズ（バイト）を返します。
		/// @return 配列の要素の合計サイズ（バイト）
		[[nodiscard]]
		constexpr size_t size_bytes() const noexcept
		{
			return (m_container.size() * sizeof(value_type));
		}

		////////////////////////////////////////////////////////////////
		//
		//	max_size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数の最大値を返します。
		/// @return 配列の要素数の最大値
		[[nodiscard]]
		constexpr size_t max_size() const noexcept
		{
			return m_container.max_size();
		}

		////////////////////////////////////////////////////////////////
		//
		//	reserve
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素数 n までメモリの再確保無しで格納できるよう、必要に応じてメモリを再確保します
		/// @param n メモリを確保する要素数
		constexpr void reserve(size_type n)
		{
			m_container.reserve(n);
		}

		////////////////////////////////////////////////////////////////
		//
		//	capacity
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリの再確保無しで格納できる要素数を返します。
		/// @return メモリの再確保無しで格納できる要素数
		[[nodiscard]]
		constexpr size_t capacity() const noexcept
		{
			return m_container.capacity();
		}

		////////////////////////////////////////////////////////////////
		//
		//	shrink_to_fit
		//
		////////////////////////////////////////////////////////////////

		/// @brief capacity() を size() まで減らすようにします。
		/// @remark この関数の呼び出しの後で capacity() == size() になるとは限りません。
		constexpr void shrink_to_fit()
		{
			m_container.shrink_to_fit();
		}

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素を削除し、空の配列にします。
		constexpr void clear() noexcept
		{
			m_container.clear();
		}

		////////////////////////////////////////////////////////////////
		//
		//	release
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素を全て消去し、メモリも解放します。
		constexpr void release()
		{
			m_container.clear();
			m_container.shrink_to_fit();
		}

		////////////////////////////////////////////////////////////////
		//
		//	insert
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に要素を挿入します。
		/// @param pos 挿入する位置
		/// @param value 挿入する値
		/// @return 挿入された要素を指すイテレータ
		constexpr iterator insert(const_iterator pos, value_type value)
		{
			return m_container.insert(pos, value);
		}

		/// @brief 指定した位置に count 個の value を挿入します。
		/// @param pos 挿入する位置
		/// @param count 挿入する個数
		/// @param value 挿入する値
		/// @return 挿入された要素の先頭を指すイテレータ
		constexpr iterator insert(const_iterator pos, size_type count, value_type value)
		{
			return m_container.insert(pos, count, value);
		}

		/// @brief 指定した位置にイテレータが指す範囲の要素を挿入します。
		/// @tparam Iterator イテレータ
		/// @param pos 挿入する位置
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return 挿入された要素の先頭を指すイテレータ
		template <class Iterator>
		constexpr iterator insert(const_iterator pos, Iterator first, Iterator last)
		{
			return m_container.insert(pos, first, last);
		}

		/// @brief 指定した位置にリストの要素を挿入します。
		/// @param pos 挿入する位置
		/// @param list リスト
		/// @return 挿入された要素の先頭を指すイテレータ
		constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> list)
		{
			return m_container.insert(pos, list);
		}

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
		constexpr iterator emplace(const_iterator pos, Args&&... args)
		{
			const value_type value(std::forward<Args>(args)...);
			return m_container.insert(pos, value);
		}

		////////////////////////////////////////////////////////////////
		//
		//	append_range
		//
		////////////////////////////////////////////////////////////////

		/// @brief 範囲の要素を配列の末尾に追加します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		template <Concept::ContainerCompatibleRange<Type> Range>
		constexpr void append_range(Range&& range)
		{
			m_container.append_range(std::forward<Range>(range));
		}

		////////////////////////////////////////////////////////////////
		//
		//	erase
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の要素を削除します。
		/// @param pos 削除する要素の位置
		/// @return 削除した要素の次の要素を指すイテレータ
		constexpr iterator erase(const_iterator pos)
		{
			return m_container.erase(pos);
		}

		/// @brief 指定した範囲の要素を削除します。
		/// @param first 削除する範囲の開始位置
		/// @param last 削除する範囲の終端位置
		/// @return 削除された範囲の次を指すイテレータ
		constexpr iterator erase(const_iterator first, const_iterator last)
		{
			return m_container.erase(first, last);
		}

		////////////////////////////////////////////////////////////////
		//
		//	push_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に要素を追加します。
		/// @param value 追加する値
		constexpr void push_back(value_type value)
		{
			m_container.push_back(value);
		}

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
		constexpr reference emplace_back(Args&&... args)
		{
			const value_type value(std::forward<Args>(args)...);
			push_back(value);
		}

		////////////////////////////////////////////////////////////////
		//
		//	pop_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾の要素を削除します。
		/// @remark 配列が空の時に呼んではいけません。
		constexpr void pop_back()
		{
			m_container.pop_back();
		}

		////////////////////////////////////////////////////////////////
		//
		//	resize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数を変更します。
		/// @param count 新しい要素数
		constexpr void resize(size_type count)
		{
			m_container.resize(count);
		}

		/// @brief 配列の要素数を変更します。
		/// @param count 新しい要素数
		/// @param value 新しく増える要素の初期値
		constexpr void resize(size_type count, value_type value)
		{
			m_container.resize(count, value);
		}

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 他の配列と要素を入れ替えます。
		/// @param other 入れ替える配列
		constexpr void swap(Array& other) noexcept
		{
			m_container.swap(other.m_container);
		}

		////////////////////////////////////////////////////////////////
		//
		//	push_front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭に要素を追加します。
		/// @param value 追加する値
		constexpr void push_front(value_type value)
		{
			m_container.insert(m_container.begin(), value);
		}

		////////////////////////////////////////////////////////////////
		//
		//	pop_front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭から要素を削除します。
		constexpr void pop_front()
		{
			m_container.erase(m_container.begin());
		}

		////////////////////////////////////////////////////////////////
		//
		//	pop_front_N
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭から指定した個数の要素を削除します。
		/// @param n 削除する個数
		/// @remark n が `size()` より多い場合は全ての要素を削除します。
		constexpr void pop_front_N(size_type n)
		{
			m_container.erase(m_container.begin(), (m_container.begin() + Min(n, m_container.size())));
		}

		////////////////////////////////////////////////////////////////
		//
		//	pop_back_N
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾から指定した個数の要素を削除します。
		/// @param n 削除する個数
		/// @remark n が `size()` より多い場合は全ての要素を削除します。
		constexpr void pop_back_N(size_type n)
		{
			m_container.erase((m_container.end() - Min(n, m_container.size())), m_container.end());
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に要素を追加します。
		/// @param value 追加する値
		/// @return *this
		constexpr Array& operator <<(value_type value)
		{
			m_container.push_back(value);
			return *this;
		}















		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		friend std::ostream& operator <<(std::ostream& output, const Array& value)
		{
			return (output << Unicode::ToUTF8(Format(value)));
		}

		friend std::wostream& operator <<(std::wostream& output, const Array& value)
		{
			return (output << Unicode::ToWstring(Format(value)));
		}

		friend std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& output, const Array& value)
		{
			return output << Unicode::ToUTF32(Format(value));
		}

		////////////////////////////////////////////////////////////////
		//
		//	Generate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 関数を用いて配列を生成します。
		/// @tparam Fty 生成に使用する関数の型
		/// @param size 生成する配列の要素数
		/// @param generator 生成に使用する関数
		/// @return 生成した配列
		template <class Fty>
		[[nodiscard]]
		static constexpr Array Generate(size_type size, Fty generator) requires (std::invocable<Fty> && std::convertible_to<std::invoke_result_t<Fty>, value_type>)
		{
			Array result(Arg::reserve = size);

			for (size_type i = 0; i < size; ++i)
			{
				result.m_container.push_back(generator());
			}

			return result;
		}

		////////////////////////////////////////////////////////////////
		//
		//	IndexedGenerate
		//
		////////////////////////////////////////////////////////////////

		/// @brief インデックスと関数を用いて配列を生成します。
		/// @tparam Fty 生成に使用する関数の型
		/// @param size 生成する配列の要素数
		/// @param indexedGenerator 生成に使用する関数
		/// @return 生成した配列
		template <class Fty>
		[[nodiscard]]
		static constexpr Array IndexedGenerate(size_type size, Fty indexedGenerator)
		{
			Array result(Arg::reserve = size);

			for (size_type i = 0; i < size; ++i)
			{
				result.m_container.push_back(indexedGenerator(i));
			}

			return result;
		}

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
}
