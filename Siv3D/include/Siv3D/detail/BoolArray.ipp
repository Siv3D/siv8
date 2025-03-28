﻿//-----------------------------------------------
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

namespace s3d
{
	namespace detail
	{
		struct BoolCharTraits
		{
			using char_type	= bool;
			using int_type	= signed char;
			using off_type	= size_t;
			using pos_type	= size_t;

			static constexpr void assign(char_type& c1, const char_type& c2) noexcept
			{
				c1 = c2;
			}

			static constexpr char_type* assign(char_type* s, size_t n, char_type a) noexcept
			{
				for (size_t i = 0; i < n; ++i)
				{
					s[i] = a;
				}

				return s;
			}

			static constexpr bool eq(const char_type& c1, const char_type& c2) noexcept
			{
				return (c1 == c2);
			}

			static constexpr bool lt(const char_type& c1, const char_type& c2) noexcept
			{
				return (c1 < c2);
			}

			static constexpr char_type* move(char_type* dst, const char_type* src, size_t n) noexcept
			{
				if (dst < src)
				{
					for (size_t i = 0; i < n; ++i)
					{
						dst[i] = src[i];
					}
				}
				else
				{
					for (size_t i = n; i > 0; --i)
					{
						dst[i - 1] = src[i - 1];
					}
				}

				return dst;
			}

			static constexpr char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept
			{
				for (size_t i = 0; i < n; ++i)
				{
					dst[i] = src[i];
				}

				return dst;
			}

			static constexpr int compare(const char_type* s1, const char_type* s2, size_t n) noexcept
			{
				for (size_t i = 0; i < n; ++i)
				{
					if (s1[i] < s2[i])
					{
						return -1;
					}
					else if (s1[i] > s2[i])
					{
						return 1;
					}
				}

				return 0;
			}

			static constexpr size_t length(const char_type* s) noexcept
			{
				size_t len = 0;

				while (s[len])
				{
					++len;
				}

				return len;
			}

			static constexpr const char_type* find(const char_type* s, size_t n, const char_type& a) noexcept
			{
				for (size_t i = 0; i < n; ++i)
				{
					if (s[i] == a)
					{
						return (s + i);
					}
				}

				return nullptr;
			}

			static constexpr char_type to_char_type(const int_type& c) noexcept
			{
				return static_cast<char_type>(c);
			}

			static constexpr int_type to_int_type(const char_type& c) noexcept
			{
				return static_cast<int_type>(c);
			}

			static constexpr bool eq_int_type(const int_type& c1, const int_type& c2) noexcept
			{
				return (c1 == c2);
			}

			static constexpr int_type eof() noexcept
			{
				return static_cast<int_type>(-1);
			}

			static constexpr int_type not_eof(const int_type& c) noexcept
			{
				return ((c == eof()) ? 0 : c);
			}
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Array<bool>
	//
	////////////////////////////////////////////////////////////////

	template <class Allocator>
	class Array<bool, Allocator>
	{
	public:

		using container_type			= std::basic_string<bool, detail::BoolCharTraits, Allocator>;

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
		Array(const Array& other) = default;

		/// @brief ムーブコンストラクタ
		/// @param other ムーブする配列
		[[nodiscard]]
		Array(Array&& other) = default;

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
			: m_container(count, false, alloc) {}

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

	# if __cpp_lib_containers_ranges >= 202202L

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
		/// @param size 作成する配列の要素数
		/// @param generator ジェネレータ関数
		[[nodiscard]]
		constexpr Array(size_type size, Arg::generator_<FunctionRef<value_type()>> generator)
			: Array(Generate(size, *generator)) {}

		/// @brief インデックス指定ジェネレータ関数を使って配列を作成します。
		/// @param size 作成する配列の要素数
		/// @param generator インデックス指定ジェネレータ関数
		[[nodiscard]]
		constexpr Array(size_type size, Arg::generator_<FunctionRef<value_type(size_t)>> generator)
			: Array(IndexedGenerate(size, *generator)) {}

		/// @brief 空の配列を作成し、`reserve()` します。
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
		Array& operator =(const Array& other) SIV3D_LIFETIMEBOUND = default;

		/// @brief ムーブ代入演算子
		/// @param other ムーブする配列
		/// @return *this
		Array& operator =(Array&& other) SIV3D_LIFETIMEBOUND = default;

		/// @brief コピー代入演算子
		/// @param other コピーする配列
		/// @return *this
		constexpr Array& operator =(const container_type& other) SIV3D_LIFETIMEBOUND
		{
			m_container = other;
			return *this;
		}

		/// @brief ムーブ代入演算子
		/// @param other ムーブする配列
		/// @return *this
		constexpr Array& operator =(container_type&& other) SIV3D_LIFETIMEBOUND
		{
			m_container = std::move(other);
			return *this;
		}

		/// @brief コピー代入演算子
		/// @param a メンバ関数 `.asArray()` を持つ型
		/// @return *this
		constexpr Array& operator =(const HasAsArray auto& a) SIV3D_LIFETIMEBOUND
		{
			m_container = a.asArray();
			return *this;
		}

		/// @brief ムーブ代入演算子
		/// @param a メンバ関数 `.asArray()` を持つ型
		/// @return *this
		constexpr Array& operator =(HasAsArray auto&& a) SIV3D_LIFETIMEBOUND
		{
			m_container = std::forward<decltype(a)>(a).asArray();
			return *this;
		}

		/// @brief コピー代入演算子
		/// @param list リスト
		/// @return *this
		constexpr Array& operator =(std::initializer_list<value_type> list) SIV3D_LIFETIMEBOUND
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
		constexpr Array& assign(size_type count, value_type value) SIV3D_LIFETIMEBOUND
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
		constexpr Array& assign(Iterator first, Iterator last) SIV3D_LIFETIMEBOUND
		{
			m_container.assign(first, last);
			return *this;
		}

		/// @brief リストから配列を作成します。
		/// @param list リスト
		/// @return *this
		constexpr Array& assign(std::initializer_list<value_type> list) SIV3D_LIFETIMEBOUND
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
		constexpr Array& assign_range(Range&& range) SIV3D_LIFETIMEBOUND
		{
		# if __cpp_lib_containers_ranges >= 202202L
			m_container.assign_range(std::forward<Range>(range));
		# else
			auto common_range = std::views::common(std::forward<Range>(range));
			m_container.assign(std::ranges::begin(common_range), std::ranges::end(common_range));
		# endif
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
		constexpr const container_type& getContainer() const noexcept SIV3D_LIFETIMEBOUND
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
		/// @return 要素への参照
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		constexpr const value_type& at(size_type index) const& SIV3D_LIFETIMEBOUND
		{
			return m_container.at(index);
		}

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		constexpr value_type& at(size_type index)& SIV3D_LIFETIMEBOUND
		{
			return m_container.at(index);
		}

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		constexpr value_type at(size_type index) &&
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
		constexpr const value_type& operator [](size_type index) const& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container[index];
		}

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		[[nodiscard]]
		constexpr value_type& operator [](size_type index) & noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container[index];
		}

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素
		[[nodiscard]]
		constexpr value_type operator [](size_type index) && noexcept
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
		constexpr reference front()& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.front();
		}

		/// @brief 先頭の要素への参照を返します。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		constexpr const_reference front() const& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.front();
		}

		/// @brief 先頭の要素を返します。
		/// @return 先頭の要素
		[[nodiscard]]
		constexpr value_type front() && noexcept
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
		constexpr reference back()& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.back();
		}

		/// @brief 末尾の要素への参照を返します。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		constexpr const_reference back() const& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.back();
		}

		/// @brief 末尾の要素を返します。
		/// @return 末尾の要素
		[[nodiscard]]
		constexpr value_type back() && noexcept
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
		constexpr value_type* data() noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.data();
		}

		/// @brief 先頭の要素を指すポインタを返します。
		/// @return 先頭の要素を指すポインタ
		[[nodiscard]]
		constexpr const value_type* data() const noexcept SIV3D_LIFETIMEBOUND
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
		constexpr iterator begin() noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.begin();
		}

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr iterator end() noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.end();
		}

		/// @brief 配列の先頭位置を指すイテレータを返します。
		/// @return 配列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator begin() const noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.begin();
		}

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator end() const noexcept SIV3D_LIFETIMEBOUND
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
		constexpr const_iterator cbegin() const noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.cbegin();
		}

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator cend() const noexcept SIV3D_LIFETIMEBOUND
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
		constexpr reverse_iterator rbegin() noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.rbegin();
		}

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr reverse_iterator rend() noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.rend();
		}

		/// @brief 配列の末尾位置を指すリバース・イテレータを返します。
		/// @return 配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rbegin() const noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.rbegin();
		}

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rend() const noexcept SIV3D_LIFETIMEBOUND
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
		constexpr const_reverse_iterator crbegin() const noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.crbegin();
		}

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator crend() const noexcept SIV3D_LIFETIMEBOUND
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
		//	indexInBounds
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスが配列の範囲内かを返します。
		/// @param index インデックス
		/// @return インデックスが範囲内の場合 true, それ以外の場合は false
		/// @remark `index < size()` と同じです。
		[[nodiscard]]
		constexpr bool indexInBounds(size_type index) const noexcept
		{
			return (index < m_container.size());
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

		/// @brief 配列の要素をすべて消去し、メモリも解放します。
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
		constexpr iterator insert(const_iterator pos, value_type value) SIV3D_LIFETIMEBOUND
		{
			return m_container.insert(pos, value);
		}

		/// @brief 指定した位置に count 個の value を挿入します。
		/// @param pos 挿入する位置
		/// @param count 挿入する個数
		/// @param value 挿入する値
		/// @return 挿入された要素の先頭を指すイテレータ
		constexpr iterator insert(const_iterator pos, size_type count, value_type value) SIV3D_LIFETIMEBOUND
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
		constexpr iterator insert(const_iterator pos, Iterator first, Iterator last) SIV3D_LIFETIMEBOUND
		{
			return m_container.insert(pos, first, last);
		}

		/// @brief 指定した位置にリストの要素を挿入します。
		/// @param pos 挿入する位置
		/// @param list リスト
		/// @return 挿入された要素の先頭を指すイテレータ
		constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> list) SIV3D_LIFETIMEBOUND
		{
			return m_container.insert(pos, list);
		}

		////////////////////////////////////////////////////////////////
		//
		//	insert_range
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に範囲の要素を挿入します。
		/// @param pos 挿入する位置
		/// @tparam Range 範囲の型
		/// @param range 範囲
		template <Concept::ContainerCompatibleRange<bool> Range>
		constexpr iterator insert_range(const_iterator pos, Range&& range) {
		# if __cpp_lib_containers_ranges >= 202202L
			return m_container.insert_range(pos, std::forward<Range>(range));
		# else
			auto common_range = std::views::common(std::forward<Range>(range));
			return m_container.insert(pos, std::ranges::begin(common_range), std::ranges::end(common_range));
		# endif
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
		constexpr iterator emplace(const_iterator pos, Args&&... args) SIV3D_LIFETIMEBOUND
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
		template <Concept::ContainerCompatibleRange<bool> Range>
		constexpr void append_range(Range&& range)
		{
		# if __cpp_lib_containers_ranges >= 202202L
			m_container.append_range(std::forward<Range>(range));
		# else
			auto common_range = std::views::common(std::forward<Range>(range));
			m_container.insert(m_container.end(), std::ranges::begin(common_range), std::ranges::end(common_range));
		# endif
		}

		////////////////////////////////////////////////////////////////
		//
		//	erase
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の要素を削除します。
		/// @param pos 削除する要素の位置
		/// @return 削除した要素の次の要素を指すイテレータ
		constexpr iterator erase(const_iterator pos) SIV3D_LIFETIMEBOUND
		{
			return m_container.erase(pos);
		}

		/// @brief 指定した範囲の要素を削除します。
		/// @param first 削除する範囲の開始位置
		/// @param last 削除する範囲の終端位置
		/// @return 削除された範囲の次を指すイテレータ
		constexpr iterator erase(const_iterator first, const_iterator last) SIV3D_LIFETIMEBOUND
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
		constexpr reference emplace_back(Args&&... args) SIV3D_LIFETIMEBOUND
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
		//	emplace_front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭に要素を構築して追加します。
		/// @tparam ...Args 構築する要素の引数の型
		/// @param ...args 構築する要素の引数
		/// @return 追加された要素への参照
		template <class... Args>
		constexpr reference emplace_front(Args&&... args)
		{
			const value_type value(std::forward<Args>(args)...);
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
		/// @remark n が `size()` より多い場合はすべての要素を削除します。
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
		/// @remark n が `size()` より多い場合はすべての要素を削除します。
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
		constexpr Array& operator <<(value_type value) SIV3D_LIFETIMEBOUND
		{
			m_container.push_back(value);
			return *this;
		}

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
		constexpr std::span<value_type> subspan(size_type pos, size_type count) noexcept SIV3D_LIFETIMEBOUND
		{
			return std::span{ m_container }.subspan(pos, count);
		}

		/// @brief 部分配列を指す span を返します。
		/// @param pos 部分配列の開始位置
		/// @param count 部分配列の要素数
		/// @return 部分配列を指す span
		[[nodiscard]]
		constexpr std::span<const value_type> subspan(size_type pos, size_type count) const noexcept SIV3D_LIFETIMEBOUND
		{
			return std::span{ m_container }.subspan(pos, count);
		}

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
		constexpr bool all(Fty f = Identity) const requires std::predicate<Fty&, const value_type&>
		{
			return std::all_of(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
		}

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
		constexpr bool any(Fty f = Identity) const requires std::predicate<Fty&, const value_type&>
		{
			return std::any_of(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に別の配列を追加します。
		/// @param other 追加する配列
		/// @return *this
		constexpr Array& append(const Array& other) SIV3D_LIFETIMEBOUND
		{
			m_container.insert(m_container.end(), other.m_container.begin(), other.m_container.end());
			return *this;
		}

		/// @brief 配列の末尾に別の範囲の要素を追加します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return *this
		template <class Iterator>
		constexpr Array& append(Iterator first, Iterator last) SIV3D_LIFETIMEBOUND
		{
			m_container.insert(m_container.end(), first, last);
			return *this;
		}

		/// @brief 配列の末尾にリストの要素を追加します。
		/// @param list リスト
		/// @return *this
		constexpr Array& append(std::initializer_list<value_type> list) SIV3D_LIFETIMEBOUND
		{
			m_container.insert(m_container.end(), list);
			return *this;
		}

		////////////////////////////////////////////////////////////////
		//
		//	choice
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素を 1 つランダムに返します。
		/// @return 配列からランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type& choice() SIV3D_LIFETIMEBOUND;

		/// @brief 配列の要素を 1 つランダムに返します。
		/// @return 配列からランダムに選ばれた要素への参照
		[[nodiscard]]
		const value_type& choice() const SIV3D_LIFETIMEBOUND;

		/// @brief 指定した乱数エンジンを用いて、配列の要素を 1 つランダムに返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 配列からランダムに選ばれた要素への参照
		[[nodiscard]]
		value_type& choice(Concept::UniformRandomBitGenerator auto&& rbg) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した乱数エンジンを用いて、配列の要素を 1 つランダムに返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 配列からランダムに選ばれた要素への参照
		[[nodiscard]]
		const value_type& choice(Concept::UniformRandomBitGenerator auto&& rbg) const SIV3D_LIFETIMEBOUND;

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
		/// @return 分割されたグループ
		[[nodiscard]]
		constexpr Array<Array<value_type>> chunk(size_type n) const
		{
			Array<Array<value_type>> result;

			if (n == 0)
			{
				return result;
			}

			for (size_t i = 0; i < (size() + n - 1) / n; ++i)
			{
				result.push_back(slice((i * n), n));
			}

			return result;
		}

		////////////////////////////////////////////////////////////////
		//
		//	contains
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しい要素があるかを返します。
		/// @param value 検索する値
		/// @return 指定した値と等しい要素がある場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool contains(const value_type& value) const
		{
			return (std::find(m_container.begin(), m_container.end(), value) != m_container.end());
		}

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
		constexpr bool contains_if(Fty f) const requires std::predicate<Fty&, const value_type&>
		{
			return std::any_of(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	count
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しい要素の個数を返します。
		/// @param value 検索する値
		/// @return 指定した値と等しい要素の個数
		[[nodiscard]]
		constexpr isize count(const value_type& value) const
		{
			return std::count(m_container.begin(), m_container.end(), value);
		}

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
		constexpr isize count_if(Fty f) const requires std::predicate<Fty&, const value_type&>
		{
			return std::count_if(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	each
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (auto& x : xs) f(x);` と同じです。
		template <class Fty>
		constexpr void each(Fty f) requires std::invocable<Fty&, value_type&>
		{
			std::for_each(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
		}

		/// @brief すべての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (const auto& x : xs) f(x);` と同じです。
		template <class Fty>
		constexpr void each(Fty f) const requires std::invocable<Fty&, const value_type&>
		{
			std::for_each(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	each_index
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (size_t i = 0; auto& x : xs) f(i++, x);` と同じです。
		template <class Fty>
		constexpr void each_index(Fty f) requires std::invocable<Fty&, size_t, value_type&>
		{
			for (size_t i = 0; auto & elem : m_container)
			{
				f(i++, elem);
			}
		}

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (size_t i = 0; const auto& x : xs) f(i++, x);` と同じです。
		template <class Fty>
		constexpr void each_index(Fty f) const requires std::invocable<Fty&, size_t, const value_type&>
		{
			for (size_t i = 0; const auto & elem : m_container)
			{
				f(i++, elem);
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	each_sindex
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (isize i = 0; auto& x : xs) f(i++, x);` と同じです。
		template <class Fty>
		constexpr void each_sindex(Fty f) requires std::invocable<Fty&, isize, value_type&>
		{
			for (isize i = 0; auto & elem : m_container)
			{
				f(i++, elem);
			}
		}

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (isize i = 0; auto x : xs) f(i++, x);` と同じです。
		template <class Fty>
		constexpr void each_sindex(Fty f) const requires std::invocable<Fty&, isize, const value_type&>
		{
			for (isize i = 0; const auto & elem : m_container)
			{
				f(i++, elem);
			}
		}

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
		constexpr value_type fetch(size_type index, U&& defaultValue) const noexcept(std::is_nothrow_constructible_v<value_type, U>) requires std::constructible_from<value_type, U>
		{
			if (m_container.size() <= index)
			{
				return value_type(std::forward<U>(defaultValue));
			}

			return m_container[index];
		}

		////////////////////////////////////////////////////////////////
		//
		//	fill
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値をすべての要素に代入します。
		/// @param value 代入する値
		/// @return *this
		constexpr Array& fill(const value_type& value) SIV3D_LIFETIMEBOUND
		{
			std::fill(m_container.begin(), m_container.end(), value);
			return *this;
		}

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
		constexpr Array filter(Fty f) const requires std::predicate<Fty&, const value_type&>
		{
			Array result;

			for (const auto& value : m_container)
			{
				if (f(value))
				{
					result.push_back(value);
				}
			}

			return result;
		}

		////////////////////////////////////////////////////////////////
		//
		//	in_groups
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を指定したグループ数に分割します。
		/// @param group グループ数
		/// @return 分割したグループ
		[[nodiscard]]
		constexpr Array<Array<value_type>> in_groups(size_type group) const
		{
			Array<Array<value_type>> result;

			if (group == 0)
			{
				return result;
			}

			const size_t div = (m_container.size() / group);
			const size_t mod = (m_container.size() % group);
			size_t index = 0;

			for (size_t i = 0; i < group; ++i)
			{
				const size_t length = (div + ((0 < mod) && (i < mod)));

				result.push_back(slice(index, length));

				index += length;
			}

			return result;
		}

		////////////////////////////////////////////////////////////////
		//
		//	isSorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素が昇順にソートされているかを返します。
		/// @return 配列の要素が昇順にソートされている場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isSorted() const
		{
			return std::is_sorted(m_container.begin(), m_container.end());
		}

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
		constexpr String join(StringView sep = U", ", StringView begin = U"{", StringView end = U"}") const
		{
			String result;

			result.append(begin);

			bool isFirst = true;

			for (const auto& value : m_container)
			{
				if (isFirst)
				{
					isFirst = false;
				}
				else
				{
					result.append(sep);
				}

				result.append(Format(value));
			}

			result.append(end);

			return result;
		}

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
		constexpr auto map(Fty f) const requires std::invocable<Fty&, const value_type&>
		{
			using result_value_type = std::decay_t<std::invoke_result_t<Fty&, const value_type&>>;

			Array<result_value_type> result;

			result.reserve(m_container.size());

			for (const auto& value : m_container)
			{
				result.push_back(f(value));
			}

			return result;
		}

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
		constexpr bool none(Fty f = Identity) const requires std::predicate<Fty&, const value_type&>
		{
			return std::none_of(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
		}

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
		constexpr auto partition(Fty f) SIV3D_LIFETIMEBOUND requires std::predicate<Fty&, const value_type&>
		{
			return std::partition(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
		}

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

		/// @brief 指定した値と等しいすべての要素を削除します。
		/// @param value 削除する値
		/// @return *this
		constexpr Array& remove(const value_type& value)& SIV3D_LIFETIMEBOUND
		{
			m_container.erase(std::remove(m_container.begin(), m_container.end(), value), m_container.end());
			return *this;
		}

		/// @brief 指定した値と等しいすべての要素を削除した新しい配列を返します。
		/// @param value 削除する値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array remove(const value_type& value)&&
		{
			return std::move(remove(value));
		}

		/// @brief 指定した値と等しいすべての要素を削除した新しい配列を返します。
		/// @param value 削除する値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array removed(const value_type& value) const&
		{
			const isize newSize = std::count(m_container.begin(), m_container.end(), (not value));

			return Array(newSize, (not value));
		}

		/// @brief 指定した値と等しいすべての要素を削除した新しい配列を返します。
		/// @param value 削除する値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array removed(const value_type& value)&&
		{
			return std::move(remove(value));
		}

		////////////////////////////////////////////////////////////////
		//
		//	remove_at, removed_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにある要素を削除します。
		/// @param index インデックス
		/// @return *this
		constexpr Array& remove_at(size_type index)& SIV3D_LIFETIMEBOUND
		{
			if (m_container.size() <= index)
			{
				detail::ThrowArrayRemoveAtIndexOutOfRange();
			}

			erase(m_container.begin() + index);

			return *this;
		}

		/// @brief 指定したインデックスにある要素を削除した新しい配列を返します。
		/// @param index インデックス
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array remove_at(size_type index)&&
		{
			return std::move(remove_at(index));
		}

		/// @brief 指定したインデックスにある要素を削除した新しい配列を返します。
		/// @param index インデックス
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array removed_at(size_type index) const&
		{
			if (m_container.size() <= index)
			{
				detail::ThrowArrayRemovedAtIndexOutOfRange();
			}

			Array result(Arg::reserve = m_container.size() - 1);
			result.insert(result.end(), m_container.begin(), (m_container.begin() + index));
			result.insert(result.end(), (m_container.begin() + index + 1), m_container.end());

			return result;
		}

		/// @brief 指定したインデックスにある要素を削除した新しい配列を返します。
		/// @param index インデックス
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array removed_at(size_type index)&&
		{
			return std::move(remove_at(index));
		}

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
		constexpr Array& remove_if(Fty f)& SIV3D_LIFETIMEBOUND requires std::predicate<Fty&, const value_type&>
		{
			erase(std::remove_if(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f))), m_container.end());
			return *this;
		}

		/// @brief 条件を満たす要素を配列から削除した新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array remove_if(Fty f) && requires std::predicate<Fty&, const value_type&>
		{
			return std::move(remove_if(std::forward<Fty>(f)));
		}

		/// @brief 条件を満たす要素を配列から削除した新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array removed_if(Fty f) const& requires std::predicate<Fty&, const value_type&>
		{
			Array result;

			for (const auto& v : m_container)
			{
				if (not f(v))
				{
					result.push_back(v);
				}
			}

			return result;
		}

		/// @brief 条件を満たす要素を配列から削除した新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array removed_if(Fty f) && requires std::predicate<Fty&, const value_type&>
		{
			return std::move(remove_if(std::forward<Fty>(f)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	replace, replaced
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しいすべての要素を別の値に置き換えます。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return *this
		constexpr Array& replace(const value_type& oldValue, const value_type& newValue)& SIV3D_LIFETIMEBOUND
		{
			if (oldValue == newValue)
			{
				return *this;
			}
			else
			{
				std::fill(m_container.begin(), m_container.end(), newValue);
				return *this;
			}
		}

		/// @brief 指定した値と等しいすべての要素を別の値に置き換えた新しい配列を返します。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array replace(const value_type& oldValue, const value_type& newValue)&&
		{
			return std::move(replace(oldValue, newValue));
		}

		/// @brief 指定した値と等しいすべての要素を別の値に置き換えた新しい配列を返します。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array replaced(const value_type& oldValue, const value_type& newValue) const&
		{
			if (oldValue == newValue)
			{
				return *this;
			}
			else
			{
				return Array(m_container.size(), newValue);
			}
		}

		/// @brief 指定した値と等しいすべての要素を別の値に置き換えた新しい配列を返します。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array replaced(const value_type& oldValue, const value_type& newValue)&&
		{
			return std::move(replace(oldValue, newValue));
		}

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
		constexpr Array& replace_if(Fty f, const value_type& newValue)& SIV3D_LIFETIMEBOUND requires std::predicate<Fty&, const value_type&>
		{
			std::replace_if(m_container.begin(), m_container.end(), f, newValue);
			return *this;
		}

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array replace_if(Fty f, const value_type& newValue) && requires std::predicate<Fty&, const value_type&>
		{
			return std::move(replace_if(f, newValue));
		}

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array replaced_if(Fty f, const value_type& newValue) const& requires std::predicate<Fty&, const value_type&>
		{
			Array result(Arg::reserve = m_container.size());

			for (const auto& v : m_container)
			{
				result.push_back(f(v) ? newValue : v);
			}

			return result;
		}

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array replaced_if(Fty f, const value_type& newValue) && requires std::predicate<Fty&, const value_type&>
		{
			return std::move(replace_if(f, newValue));
		}

		////////////////////////////////////////////////////////////////
		//
		//	reverse, reversed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素を逆順に並び替えます。
		/// @return *this
		constexpr Array& reverse()& SIV3D_LIFETIMEBOUND
		{
			std::reverse(m_container.begin(), m_container.end());
			return *this;
		}

		/// @brief 配列の要素を逆順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array reverse()&&
		{
			return std::move(reverse());
		}

		/// @brief 配列の要素を逆順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array reversed() const&
		{
			return Array(m_container.rbegin(), m_container.rend());
		}

		/// @brief 配列の要素を逆順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array reversed()&&
		{
			return std::move(reverse());
		}

		////////////////////////////////////////////////////////////////
		//
		//	reverse_each
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾から順番に、すべての要素に対して関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (auto& x : xs) f(x);` と同じです。
		template <class Fty>
		constexpr void reverse_each(Fty f) requires std::invocable<Fty&, value_type&>
		{
			std::for_each(m_container.rbegin(), m_container.rend(), detail::PassFunction(std::forward<Fty>(f)));
		}

		/// @brief 末尾から順番に、すべての要素に対して関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (const auto& x : xs) f(x);` と同じです。
		template <class Fty>
		constexpr void reverse_each(Fty f) const requires std::invocable<Fty&, const value_type&>
		{
			std::for_each(m_container.rbegin(), m_container.rend(), detail::PassFunction(std::forward<Fty>(f)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	rotate, rotated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を境に配列の前半と後半を入れ替えます。
		/// @param middle 境の位置
		/// @return *this
		constexpr Array& rotate(size_type middle)& SIV3D_LIFETIMEBOUND
		{
			std::rotate(m_container.begin(), (m_container.begin() + middle), m_container.end());
			return *this;
		}

		/// @brief 指定した位置を境に配列の前半と後半を入れ替えた新しい配列を返します。
		/// @param middle 境の位置
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array rotate(size_type middle)&&
		{
			return std::move(rotate(middle));
		}

		/// @brief 指定した位置を境に配列の前半と後半を入れ替えた新しい配列を返します。
		/// @param middle 境の位置
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array rotated(size_type middle) const&
		{
			Array result(Arg::reserve = m_container.size());

			result.insert(result.end(), (m_container.begin() + middle), m_container.end());

			result.insert(result.end(), m_container.begin(), (m_container.begin() + middle));

			return result;
		}

		/// @brief 指定した位置を境に配列の前半と後半を入れ替えた新しい配列を返します。
		/// @param middle 境の位置
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array rotated(size_type middle)&&
		{
			return std::move(rotate(middle));
		}

		////////////////////////////////////////////////////////////////
		//
		//	rsort, rsorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を降順に並び替えます。
		/// @return *this
		constexpr Array& rsort()& SIV3D_LIFETIMEBOUND
		{
			const isize trueCount = std::count(m_container.begin(), m_container.end(), true);

			std::fill(m_container.begin(), (m_container.begin() + trueCount), true);
			std::fill((m_container.begin() + trueCount), m_container.end(), false);

			return *this;
		}

		/// @brief 要素を降順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array rsort() &&
		{
			return std::move(rsort());
		}

		/// @brief 要素を降順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array rsorted() const&
		{
			const isize trueCount = std::count(m_container.begin(), m_container.end(), true);

			Array result(m_container.size(), false);
			std::fill(result.begin(), (result.begin() + trueCount), true);

			return result;
		}

		/// @brief 要素を降順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array rsorted() &&
		{
			return std::move(rsort());
		}

		////////////////////////////////////////////////////////////////
		//
		//	shuffle, shuffled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素の並び順をランダムにシャッフルします。
		/// @return *this
		constexpr Array& shuffle()& SIV3D_LIFETIMEBOUND;

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
		constexpr Array& shuffle(Concept::UniformRandomBitGenerator auto&& rbg)& SIV3D_LIFETIMEBOUND;

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
		constexpr Array slice(size_type index, size_type length) const
		{
			return Array((m_container.begin() + index), (m_container.begin() + index + length));
		}

		////////////////////////////////////////////////////////////////
		//
		//	sort, sorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を昇順に並び替えます。
		/// @return *this
		constexpr Array& sort()& SIV3D_LIFETIMEBOUND
		{
			const isize falseCount = std::count(m_container.begin(), m_container.end(), false);

			std::fill(m_container.begin(), (m_container.begin() + falseCount), false);
			std::fill((m_container.begin() + falseCount), m_container.end(), true);

			return *this;
		}

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sort() &&
		{
			return std::move(sort());
		}

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sorted() const&
		{
			const isize trueCount = std::count(m_container.begin(), m_container.end(), true);

			Array result(m_container.size(), true);
			std::fill(result.begin(), (result.begin() + trueCount), false);

			return result;
		}

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sorted() &&
		{
			return std::move(sort());
		}

		////////////////////////////////////////////////////////////////
		//
		//	sort_and_unique, sorted_and_uniqued
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列をソートしたあとに重複する要素を削除します。
		/// @return *this
		constexpr Array& sort_and_unique() & noexcept SIV3D_LIFETIMEBOUND
		{
			const bool hasTrue = (std::find(m_container.begin(), m_container.end(), true) != m_container.end());
			const bool hasFalse = (std::find(m_container.begin(), m_container.end(), false) != m_container.end());

			if (hasTrue && hasFalse)
			{
				m_container = { false, true };
			}
			else if (hasTrue)
			{
				m_container = { true };
			}
			else if (hasFalse)
			{
				m_container = { false };
			}

			return *this;
		}

		/// @brief 配列をソートしたあとに重複する要素を削除した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sort_and_unique() && noexcept
		{
			return std::move(sort_and_unique());
		}

		/// @brief 配列をソートしたあとに重複する要素を削除した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sorted_and_uniqued() const&
		{
			const bool hasTrue = (std::find(m_container.begin(), m_container.end(), true) != m_container.end());
			const bool hasFalse = (std::find(m_container.begin(), m_container.end(), false) != m_container.end());

			if (hasTrue && hasFalse)
			{
				return{ false, true };
			}
			else if (hasTrue)
			{
				return{ true };
			}
			else if (hasFalse)
			{
				return{ false };
			}

			return{};
		}

		/// @brief 配列をソートしたあとに重複する要素を削除した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sorted_and_uniqued() && noexcept
		{
			return std::move(sort_and_unique());
		}

		////////////////////////////////////////////////////////////////
		//
		//	stable_sort, stable_sorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を相対順序を保ちながら昇順に並び替えます。
		/// @return *this
		constexpr Array& stable_sort()& SIV3D_LIFETIMEBOUND
		{
			return sort();
		}

		/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array stable_sort()&&
		{
			return std::move(sort());
		}

		/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array stable_sorted() const&
		{
			return sorted();
		}

		/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array stable_sorted() &&
		{
			return std::move(sort());
		}

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
		constexpr auto stable_partition(Fty f) SIV3D_LIFETIMEBOUND requires std::predicate<Fty&, const value_type&>
		{
			return partition(std::forward<Fty>(f));
		}

		////////////////////////////////////////////////////////////////
		//
		//	stable_unique, stable_uniqued
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素をソートせずに、重複する要素を削除します。
		/// @return *this
		constexpr Array& stable_unique() & noexcept SIV3D_LIFETIMEBOUND
		{
			const auto itTrue = std::find(m_container.begin(), m_container.end(), true);
			const auto itFalse = std::find(m_container.begin(), m_container.end(), false);

			const bool hasTrue = (itTrue != m_container.end());
			const bool hasFalse = (itFalse != m_container.end());

			if (hasTrue && hasFalse)
			{
				if (itFalse < itTrue)
				{
					m_container = { false, true };
				}
				else
				{
					m_container = { true, false };
				}
			}
			else if (hasTrue)
			{
				m_container = { true };
			}
			else if (hasFalse)
			{
				m_container = { false };
			}

			return *this;
		}

		/// @brief 要素をソートせずに、重複する要素を削除した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array stable_unique() && noexcept
		{
			return std::move(stable_unique());
		}

		/// @brief 要素をソートせずに、重複する要素を削除した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array stable_uniqued() const
		{
			const auto itTrue = std::find(m_container.begin(), m_container.end(), true);
			const auto itFalse = std::find(m_container.begin(), m_container.end(), false);

			const bool hasTrue = (itTrue != m_container.end());
			const bool hasFalse = (itFalse != m_container.end());

			if (hasTrue && hasFalse)
			{
				if (itFalse < itTrue)
				{
					return{ false, true };
				}
				else
				{
					return{ true, false };
				}
			}
			else if (hasTrue)
			{
				return{ true };
			}
			else if (hasFalse)
			{
				return{ false };
			}

			return{};
		}

		////////////////////////////////////////////////////////////////
		//
		//	sum
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を `+` 演算子を用いて合計します。
		/// @return 合計値
		[[nodiscard]]
		constexpr isize sum() const
		{
			return std::count(m_container.begin(), m_container.end(), true);
		}

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
		constexpr Array take(size_type n) const&
		{
			return Array(m_container.begin(), (m_container.begin() + Min(n, m_container.size())));
		}

		/// @brief 先頭から指定した個数の要素からなる新しい配列を返します。
		/// @param n 取り出す要素数
		/// @return 新しい配列
		/// @remark n が配列の要素数以上の場合、配列全体を返します。
		/// @remark `Array(begin(), (begin() + Min(n, size())))` と同じです。
		[[nodiscard]]
		constexpr Array take(size_type n)&&
		{
			return Array(std::make_move_iterator(m_container.begin()), std::make_move_iterator(m_container.begin() + Min(n, m_container.size())));
		}

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
		constexpr Array take_while(Fty f) const& requires std::predicate<Fty&, const value_type&>
		{
			return Array(m_container.begin(), std::find_if_not(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f))));
		}

		/// @brief 先頭から、条件を満たさなくなる直前までの要素からなる新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array take_while(Fty f) && requires std::predicate<Fty&, const value_type&>
		{
			return Array(std::make_move_iterator(m_container.begin()), std::make_move_iterator(std::find_if_not(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)))));
		}

		////////////////////////////////////////////////////////////////
		//
		//	unique_consecutive, uniqued_consecutive
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ要素が連続する場合、その先頭以外を除去します。
		/// @return *this
		constexpr Array& unique_consecutive() & noexcept SIV3D_LIFETIMEBOUND;

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
		constexpr Array values_at(std::initializer_list<size_type> indices) const
		{
			Array result;
			result.reserve(indices.size());

			for (auto index : indices)
			{
				if (index < m_container.size())
				{
					result.push_back(m_container[index]);
				}
				else
				{
					detail::ThrowArrayValuesAtIndexOutOfRange();
				}
			}

			return result;
		}

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
		constexpr auto operator >>(Fty f) const requires std::invocable<Fty&, const value_type&>
		{
			using result_value_type = std::decay_t<std::invoke_result_t<Fty&, const value_type&>>;

			if constexpr (std::is_same_v<result_value_type, void>)
			{
				each(std::forward<Fty>(f));
			}
			else
			{
				return map(std::forward<Fty>(f));
			}
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
		/// @param size 生成する配列の要素数
		/// @param generator 生成に使用する関数
		/// @return 生成した配列
		[[nodiscard]]
		static constexpr Array Generate(size_type size, FunctionRef<value_type()> generator)
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
		/// @param size 生成する配列の要素数
		/// @param generator 生成に使用する関数
		/// @return 生成した配列
		[[nodiscard]]
		static constexpr Array IndexedGenerate(size_type size, FunctionRef<value_type(size_t)> indexedGenerator)
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
	};
}
