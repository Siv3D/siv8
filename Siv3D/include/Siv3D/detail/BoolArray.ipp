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

	/// @brief アドレスを取得可能な bool 要素の動的配列。
	/// @remark 同じ要素型の派生配列は元のアロケータを引き継ぎます。通常のコピー構築・代入は allocator_traits の規約に従います。
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
		template <std::input_iterator Iterator>
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

		/// @brief 同じ Array 型または container_type を返す `.asArray()` から配列を作成します。
		/// @tparam ArrayIsh メンバ関数 `.asArray()` を持つ型
		/// @param a `.asArray()` を持つ型のオブジェクト
		[[nodiscard]]
		explicit constexpr Array(const HasAsArray auto& a)
			requires detail::AsArrayOf<decltype(a), Array>
			: Array{ a.asArray() } {}

		/// @brief 同じ Array 型または container_type を返す `.asArray()` から配列を作成します。
		/// @tparam ArrayIsh メンバ関数 `.asArray()` を持つ型
		/// @param a `.asArray()` を持つ型のオブジェクト
		[[nodiscard]]
		explicit constexpr Array(HasAsArray auto&& a)
			requires detail::AsArrayOf<decltype(a), Array>
			: Array{ std::forward<decltype(a)>(a).asArray() } {}

		/// @brief 初期化リストから配列を作成します。
		/// @param list 初期化リスト
		/// @param alloc アロケータ
		[[nodiscard]]
		constexpr Array(std::initializer_list<value_type> list, const Allocator& alloc = Allocator{})
			: m_container(list, alloc) {}

		/// @brief 範囲から配列を作成します。
		/// @tparam Range 範囲の型
		/// @param range 範囲
		/// @param alloc アロケータ
		template <Concept::ContainerCompatibleRange<bool> Range>
		[[nodiscard]]
		constexpr Array(std::from_range_t, Range&& range, const Allocator& alloc = Allocator{})
			: m_container(std::from_range, std::forward<Range>(range), alloc) {}

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
		/// @param a 同じ Array 型または container_type を返す `.asArray()` を持つ型
		/// @return *this
		constexpr Array& operator =(const HasAsArray auto& a) SIV3D_LIFETIMEBOUND
			requires detail::AsArrayOf<decltype(a), Array>
		{
			*this = a.asArray();
			return *this;
		}

		/// @brief ムーブ代入演算子
		/// @param a 同じ Array 型または container_type を返す `.asArray()` を持つ型
		/// @return *this
		constexpr Array& operator =(HasAsArray auto&& a) SIV3D_LIFETIMEBOUND
			requires detail::AsArrayOf<decltype(a), Array>
		{
			*this = std::forward<decltype(a)>(a).asArray();
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
		constexpr void assign(size_type count, value_type value)
		{
			m_container.assign(count, value);
		}

		/// @brief イテレータが指す範囲の要素から配列を作成します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return *this
		template <std::input_iterator Iterator>
		constexpr void assign(Iterator first, Iterator last)
		{
			m_container.assign(first, last);
		}

		/// @brief リストから配列を作成します。
		/// @param list リスト
		/// @return *this
		constexpr void assign(std::initializer_list<value_type> list)
		{
			m_container.assign(list);
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
		constexpr void assign_range(Range&& range)
		{
			m_container.assign_range(std::forward<Range>(range));
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
		constexpr const container_type& getContainer() const& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container;
		}

		/// @brief const な一時配列からの呼び出しを禁止します。
		constexpr container_type getContainer() const&& = delete;

		/// @brief 一時配列の内部コンテナをムーブして返します。
		[[nodiscard]]
		constexpr container_type getContainer() && noexcept
		{
			return std::move(m_container);
		}

		////////////////////////////////////////////////////////////////
		//
		//	asArray
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素の型を変換した新しい配列を返します。
		/// @tparam ElementType 変換後の要素の型
		/// @return 要素の型を変換した新しい配列
		template <class ElementType>
		[[nodiscard]]
		constexpr Array<ElementType> asArray() const
			requires requires(const value_type& value) { static_cast<ElementType>(value); }
		{
			Array<ElementType> result(Arg::reserve = m_container.size());

			for (const auto& value : m_container)
			{
				result.push_back(static_cast<ElementType>(value));
			}

			return result;
		}

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

		/// @brief const な一時配列からの呼び出しを禁止します。
		constexpr value_type at(size_type index) const&& = delete;

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

		/// @brief const な一時配列からの呼び出しを禁止します。
		constexpr value_type operator [](size_type index) const&& = delete;

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

		/// @brief const な一時配列からの呼び出しを禁止します。
		constexpr value_type front() const&& = delete;

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

		/// @brief const な一時配列からの呼び出しを禁止します。
		constexpr value_type back() const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	data
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の要素を指すポインタを返します。
		/// @return 先頭の要素を指すポインタ
		[[nodiscard]]
		constexpr value_type* data() & noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.data();
		}

		/// @brief 先頭の要素を指すポインタを返します。
		/// @return 先頭の要素を指すポインタ
		[[nodiscard]]
		constexpr const value_type* data() const& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.data();
		}

		/// @brief 一時配列からの借用を禁止します。
		constexpr const value_type* data() const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	begin, end
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭位置を指すイテレータを返します。
		/// @return 配列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr iterator begin() & noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.begin();
		}

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr iterator end() & noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.end();
		}

		/// @brief 配列の先頭位置を指すイテレータを返します。
		/// @return 配列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator begin() const& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.begin();
		}

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator end() const& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.end();
		}

		/// @brief 一時配列からの借用を禁止します。
		constexpr const_iterator begin() const&& = delete;

		/// @brief 一時配列からの借用を禁止します。
		constexpr const_iterator end() const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	cbegin, cend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の先頭位置を指すイテレータを返します。
		/// @return 配列の先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator cbegin() const& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.cbegin();
		}

		/// @brief 配列の終端位置を指すイテレータを返します。
		/// @remark 有効な範囲は [begin, end) であるため、この位置に要素は存在しません
		/// @return 配列の終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator cend() const& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.cend();
		}

		/// @brief 一時配列からの借用を禁止します。
		constexpr const_iterator cbegin() const&& = delete;

		/// @brief 一時配列からの借用を禁止します。
		constexpr const_iterator cend() const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	rbegin, rend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾位置を指すリバース・イテレータを返します。
		/// @return 配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr reverse_iterator rbegin() & noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.rbegin();
		}

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr reverse_iterator rend() & noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.rend();
		}

		/// @brief 配列の末尾位置を指すリバース・イテレータを返します。
		/// @return 配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rbegin() const& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.rbegin();
		}

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rend() const& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.rend();
		}

		/// @brief 一時配列からの借用を禁止します。
		constexpr const_reverse_iterator rbegin() const&& = delete;

		/// @brief 一時配列からの借用を禁止します。
		constexpr const_reverse_iterator rend() const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	crbegin, crend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾位置を指すリバース・イテレータを返します。
		/// @return 配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator crbegin() const& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.crbegin();
		}

		/// @brief 配列の先端位置を指すリバース・イテレータを返します。
		/// @remark 有効な範囲は [rbegin, rend) であるため、この位置に要素は存在しません
		/// @return 配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator crend() const& noexcept SIV3D_LIFETIMEBOUND
		{
			return m_container.crend();
		}

		/// @brief 一時配列からの借用を禁止します。
		constexpr const_reverse_iterator crbegin() const&& = delete;

		/// @brief 一時配列からの借用を禁止します。
		constexpr const_reverse_iterator crend() const&& = delete;

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

		/// @brief 配列の合計サイズ（バイト）を返します。
		/// @return 配列の合計サイズ（バイト）
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
			container_type empty(m_container.get_allocator());
			m_container.swap(empty);
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
		constexpr iterator insert(const_iterator pos, value_type value) & SIV3D_LIFETIMEBOUND
		{
			return m_container.insert(pos, value);
		}

		/// @brief 指定した位置に count 個の value を挿入します。
		/// @param pos 挿入する位置
		/// @param count 挿入する個数
		/// @param value 挿入する値
		/// @return 挿入された要素の先頭を指すイテレータ
		constexpr iterator insert(const_iterator pos, size_type count, value_type value) & SIV3D_LIFETIMEBOUND
		{
			return m_container.insert(pos, count, value);
		}

		/// @brief 指定した位置にイテレータが指す範囲の要素を挿入します。
		/// @tparam Iterator イテレータ
		/// @param pos 挿入する位置
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return 挿入された要素の先頭を指すイテレータ
		template <std::input_iterator Iterator>
		constexpr iterator insert(const_iterator pos, Iterator first, Iterator last) & SIV3D_LIFETIMEBOUND
		{
			return m_container.insert(pos, first, last);
		}

		/// @brief 指定した位置にリストの要素を挿入します。
		/// @param pos 挿入する位置
		/// @param list リスト
		/// @return 挿入された要素の先頭を指すイテレータ
		constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> list) & SIV3D_LIFETIMEBOUND
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
		constexpr iterator insert_range(const_iterator pos, Range&& range) &
		{
			return m_container.insert_range(pos, std::forward<Range>(range));
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
		constexpr iterator emplace(const_iterator pos, Args&&... args) & SIV3D_LIFETIMEBOUND
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
		constexpr iterator erase(const_iterator pos) & SIV3D_LIFETIMEBOUND
		{
			return m_container.erase(pos);
		}

		/// @brief 指定した範囲の要素を削除します。
		/// @param first 削除する範囲の開始位置
		/// @param last 削除する範囲の終端位置
		/// @return 削除された範囲の次を指すイテレータ
		constexpr iterator erase(const_iterator first, const_iterator last) & SIV3D_LIFETIMEBOUND
		{
			return m_container.erase(first, last);
		}

		////////////////////////////////////////////////////////////////
		//
		//	erase_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにある要素を削除します。
		/// @param index インデックス
		/// @return *this
		constexpr Array& erase_at(size_type index)& SIV3D_LIFETIMEBOUND
		{
			if (m_container.size() <= index)
			{
				detail::ThrowArrayEraseAtIndexOutOfRange();
			}

			erase(m_container.begin() + index);

			return *this;
		}

		/// @brief 指定したインデックスにある要素を削除した新しい配列を返します。
		/// @param index インデックス
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array erase_at(size_type index)&&
		{
			return std::move(erase_at(index));
		}

		////////////////////////////////////////////////////////////////
		//
		//	erase_at_unstable
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにある要素を削除します。削除後の要素の順序は保証されません。
		/// @param index インデックス
		/// @return *this
		/// @remark 削除対象を末尾の要素と入れ替えてから末尾を削除します。
		constexpr Array& erase_at_unstable(size_type index)& SIV3D_LIFETIMEBOUND
		{
			if (m_container.size() <= index)
			{
				detail::ThrowArrayEraseAtUnstableIndexOutOfRange();
			}

			if (const size_type lastIndex = (m_container.size() - 1);
				index != lastIndex)
			{
				std::ranges::swap(m_container[index], m_container[lastIndex]);
			}

			m_container.pop_back();

			return *this;
		}

		/// @brief 指定したインデックスにある要素を削除した新しい配列を返します。削除後の要素の順序は保証されません。
		/// @param index インデックス
		/// @return 新しい配列
		/// @remark 削除対象を末尾の要素と入れ替えてから末尾を削除します。
		constexpr Array erase_at_unstable(size_type index)&&
		{
			return std::move(erase_at_unstable(index));
		}

		////////////////////////////////////////////////////////////////
		//
		//	erase_all
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しい要素をすべて削除します。
		/// @param value 値
		/// @return 削除した要素の個数
		constexpr size_type erase_all(const value_type value)
		{
			const size_type erasedCount = static_cast<size_type>(std::ranges::count(m_container, value));
			m_container.assign((m_container.size() - erasedCount), (not value));
			return erasedCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	erase_all_unstable
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しい要素をすべて削除します。削除後の要素の順序は保証されません。
		/// @param value 値
		/// @return 削除した要素の個数
		/// @remark 削除対象を末尾の要素と入れ替えてから末尾を削除します。
		constexpr size_type erase_all_unstable(const value_type value)
		{
			size_type count = 0;

			for (size_type i = 0; i < m_container.size();)
			{
				if (m_container[i] == value)
				{
					if (const size_type lastIndex = (m_container.size() - 1);
						i != lastIndex)
					{
						std::ranges::iter_swap((m_container.begin() + i), (m_container.begin() + lastIndex));
					}

					m_container.pop_back();
					++count;
				}
				else
				{
					++i;
				}
			}

			return count;
		}

		////////////////////////////////////////////////////////////////
		//
		//	erase_first
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から見て、最初に指定した値と等しい要素を削除します。
		/// @param value 値
		/// @return 削除された要素があった場合 true, それ以外の場合は false
		constexpr bool erase_first(const value_type value)
		{
			if (const auto it = std::ranges::find(m_container, value);
				it != m_container.end())
			{
				m_container.erase(it);
				return true;
			}
			else
			{
				return false;
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	erase_first_unstable
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から見て、最初に指定した値と等しい要素を削除します。削除後の要素の順序は保証されません。
		/// @param value 値
		/// @return 削除された要素があった場合 true, それ以外の場合は false
		/// @remark 削除対象を末尾の要素と入れ替えてから末尾を削除します。
		constexpr bool erase_first_unstable(const value_type value)
		{
			if (const auto it = std::ranges::find(m_container, value);
				it != m_container.end())
			{
				const auto last = (m_container.end() - 1);

				if (it != last)
				{
					std::ranges::iter_swap(it, last);
				}

				m_container.pop_back();

				return true;
			}
			else
			{
				return false;
			}
		}

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
		{
			return std::erase_if(m_container, detail::PassFunction(std::forward<Fty>(f)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	erase_all_if_unstable
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した条件を満たす要素をすべて削除します。削除後の要素の順序は保証されません。
		/// @tparam Fty 条件を表す述語の型
		/// @param f 条件を表す述語
		/// @return 削除した要素の個数
		/// @remark 削除対象を末尾の要素と入れ替えてから末尾を削除します。
		template <class Fty>
		constexpr size_type erase_all_if_unstable(Fty f)
		{
			size_type count = 0;

			auto pred = detail::PassFunction(std::forward<Fty>(f));

			for (size_type i = 0; i < m_container.size();)
			{
				if (pred(m_container[i]))
				{
					if (const size_type lastIndex = (m_container.size() - 1);
						i != lastIndex)
					{
						std::ranges::iter_swap(m_container.begin() + i, m_container.begin() + lastIndex);
					}

					m_container.pop_back();
					++count;
				}
				else
				{
					++i;
				}
			}

			return count;
		}

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
		{
			if (const auto it = std::ranges::find_if(m_container, detail::PassFunction(std::forward<Fty>(f)));
				it != m_container.end())
			{
				m_container.erase(it);
				return true;
			}
			else
			{
				return false;
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	erase_first_if_unstable
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から見て、最初に指定した条件を満たす要素を削除します。削除後の要素の順序は保証されません。
		/// @tparam Fty 条件を表す述語の型
		/// @param f 条件を表す述語
		/// @return 削除された要素があった場合 true, それ以外の場合は false
		/// @remark 削除対象を末尾の要素と入れ替えてから末尾を削除します。
		template <class Fty>
		constexpr bool erase_first_if_unstable(Fty f)
		{
			if (const auto it = std::ranges::find_if(m_container, detail::PassFunction(std::forward<Fty>(f)));
				it != m_container.end())
			{
				const auto last = (m_container.end() - 1);

				if (it != last)
				{
					std::ranges::iter_swap(it, last);
				}

				m_container.pop_back();

				return true;
			}
			else
			{
				return false;
			}
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
		constexpr reference emplace_back(Args&&... args) & SIV3D_LIFETIMEBOUND
		{
			const value_type value(std::forward<Args>(args)...);
			push_back(value);
			return m_container.back();
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
		constexpr reference emplace_front(Args&&... args) &
		{
			const value_type value(std::forward<Args>(args)...);
			m_container.insert(m_container.begin(), value);
			return m_container.front();
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
		constexpr Array& operator <<(const value_type& value) & SIV3D_LIFETIMEBOUND
		{
			m_container.push_back(value);
			return *this;
		}

		/// @brief 一時配列を変更し、所有権を持つ配列を返します。
		/// @return 記憶域とアロケータを引き継いだ配列
		[[nodiscard]]
		constexpr Array operator <<(const value_type& value) &&
		{
			(*this << value);
			return std::move(*this);
		}

		/// @brief 一時配列へ要素を追加し、所有権を持つ配列を返します。
		[[nodiscard]]
		constexpr Array operator <<(value_type&& value) &&
		{
			(*this << std::move(value));
			return std::move(*this);
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
		constexpr std::span<value_type> subspan(size_type pos, size_type count) & noexcept SIV3D_LIFETIMEBOUND
		{
			return std::span{ m_container }.subspan(pos, count);
		}

		/// @brief 部分配列を指す span を返します。
		/// @param pos 部分配列の開始位置
		/// @param count 部分配列の要素数
		/// @return 部分配列を指す span
		[[nodiscard]]
		constexpr std::span<const value_type> subspan(size_type pos, size_type count) const& noexcept SIV3D_LIFETIMEBOUND
		{
			return std::span{ m_container }.subspan(pos, count);
		}

		/// @brief 一時配列からの借用を禁止します。
		constexpr std::span<value_type> subspan(size_type pos, size_type count) && = delete;

		/// @brief const な一時配列からの借用を禁止します。
		constexpr std::span<value_type> subspan(size_type pos, size_type count) const&& = delete;

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
			requires std::predicate<Fty&, const value_type&>
		{
			return std::ranges::all_of(m_container, detail::PassFunction(std::forward<Fty>(f)));
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
		constexpr bool any(Fty f = Identity) const
			requires std::predicate<Fty&, const value_type&>
		{
			return std::ranges::any_of(m_container, detail::PassFunction(std::forward<Fty>(f)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に別の配列を追加します。
		/// @param other 追加する配列
		/// @remark 自己コピー追記は要素を複製します。自己 move 追記は何もしません。
		/// @return *this
		constexpr Array& append(const Array& other) & SIV3D_LIFETIMEBOUND
		{
			if (std::addressof(other) == this)
			{
				Array copy(other, m_container.get_allocator());
				m_container.insert(m_container.end(), copy.m_container.begin(), copy.m_container.end());
				return *this;
			}
			else
			{
				m_container.insert(m_container.end(), other.m_container.begin(), other.m_container.end());
				return *this;
			}
		}

		/// @brief 配列の末尾に別の配列を追加します。
		/// @param other 追加する配列
		/// @return *this
		/// @remark 自己 move 追記は何もしません。
		constexpr Array& append(Array&& other) & SIV3D_LIFETIMEBOUND
		{
			if (std::addressof(other) != this)
			{
				m_container.append(other.m_container);
			}
			return *this;
		}

		/// @brief 配列の末尾に同じ値を指定した個数追加します。
		/// @param count 追加する個数
		/// @param value 追加する値
		/// @return *this
		constexpr Array& append(size_type count, const value_type& value) & SIV3D_LIFETIMEBOUND
		{
			m_container.append(count, value);
			return *this;
		}

		/// @brief 配列の末尾に別の範囲の要素を追加します。
		/// @tparam Iterator イテレータ
		/// @param first 範囲の開始位置を指すイテレータ
		/// @param last 範囲の終端位置を指すイテレータ
		/// @return *this
		template <std::input_iterator Iterator>
		constexpr Array& append(Iterator first, Iterator last) & SIV3D_LIFETIMEBOUND
		{
			m_container.insert(m_container.end(), first, last);
			return *this;
		}

		/// @brief 配列の末尾にリストの要素を追加します。
		/// @param list リスト
		/// @return *this
		constexpr Array& append(std::initializer_list<value_type> list) & SIV3D_LIFETIMEBOUND
		{
			m_container.insert(m_container.end(), list);
			return *this;
		}

		/// @brief 一時配列を変更し、所有権を持つ配列を返します。
		/// @return 記憶域とアロケータを引き継いだ配列
		[[nodiscard]]
		constexpr Array append(const Array& other) &&
		{
			append(other);
			return std::move(*this);
		}

		/// @brief 一時配列を変更し、所有権を持つ配列を返します。
		/// @return 記憶域とアロケータを引き継いだ配列
		[[nodiscard]]
		constexpr Array append(Array&& other) &&
		{
			append(std::move(other));
			return std::move(*this);
		}

		/// @brief 一時配列を変更し、所有権を持つ配列を返します。
		/// @return 記憶域とアロケータを引き継いだ配列
		template <std::input_iterator Iterator>
		[[nodiscard]]
		constexpr Array append(Iterator first, Iterator last) &&
		{
			append(first, last);
			return std::move(*this);
		}

		/// @brief 一時配列を変更し、所有権を持つ配列を返します。
		/// @return 記憶域とアロケータを引き継いだ配列
		[[nodiscard]]
		constexpr Array append(std::initializer_list<value_type> list) &&
		{
			append(list);
			return std::move(*this);
		}

		/// @brief 一時配列を変更し、所有権を持つ配列を返します。
		/// @return 記憶域とアロケータを引き継いだ配列
		[[nodiscard]]
		constexpr Array append(size_type count, const value_type& value) &&
		{
			append(count, value);
			return std::move(*this);
		}

		////////////////////////////////////////////////////////////////
		//
		//	choice
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素を 1 つランダムに返します。
		/// @return 配列からランダムに選ばれた要素への参照
		/// @throw std::out_of_range 配列が空の場合
		[[nodiscard]]
		value_type& choice() & SIV3D_LIFETIMEBOUND;

		/// @brief 配列の要素を 1 つランダムに返します。
		/// @return 配列からランダムに選ばれた要素への参照
		/// @throw std::out_of_range 配列が空の場合
		[[nodiscard]]
		const value_type& choice() const& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した乱数エンジンを用いて、配列の要素を 1 つランダムに返します。
		/// @param urbg 使用する乱数エンジン
		/// @return 配列からランダムに選ばれた要素への参照
		/// @throw std::out_of_range 配列が空の場合
		[[nodiscard]]
		value_type& choice(Concept::UniformRandomBitGenerator auto&& urbg) & SIV3D_LIFETIMEBOUND;

		/// @brief 指定した乱数エンジンを用いて、配列の要素を 1 つランダムに返します。
		/// @param urbg 使用する乱数エンジン
		/// @return 配列からランダムに選ばれた要素への参照
		/// @throw std::out_of_range 配列が空の場合
		[[nodiscard]]
		const value_type& choice(Concept::UniformRandomBitGenerator auto&& urbg) const& SIV3D_LIFETIMEBOUND;

		/// @brief 配列の要素から指定した個数だけ重複なくランダムに選んで返します。
		/// @param n 選択する個数
		/// @return 元の順序とアロケータを維持した、最大 min(n, size()) 個の要素の配列
		[[nodiscard]]
		Array choice(size_t n) const;

		/// @brief 指定した乱数エンジンを用いて、配列の要素から指定した個数だけ重複なくランダムに選んで返します。
		/// @param n 選択する個数
		/// @param urbg 使用する乱数エンジン
		/// @return 元の順序とアロケータを維持した、最大 min(n, size()) 個の要素の配列
		[[nodiscard]]
		Array choice(size_t n, Concept::UniformRandomBitGenerator auto&& urbg) const;

		/// @brief 一時配列からランダムに選んだ要素をムーブして返します。空の場合は std::out_of_range。
		[[nodiscard]]
		value_type choice() &&;

		/// @brief const な一時配列からの呼び出しを禁止します。
		value_type choice() const&& = delete;

		/// @brief 一時配列からランダムに選んだ要素をムーブして返します。空の場合は std::out_of_range。
		[[nodiscard]]
		value_type choice(Concept::UniformRandomBitGenerator auto&& urbg) &&;

		/// @brief const な一時配列からの呼び出しを禁止します。
		value_type choice(Concept::UniformRandomBitGenerator auto&& urbg) const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	chunk
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した個数の要素を持つ配列のグループに分割します。最後のグループの要素数は n 個未満になることがあります。
		/// @param n 1 つのグループが持つ要素数
		/// @return 元のアロケータの型と状態を各内側配列に引き継いだグループ
		[[nodiscard]]
		constexpr Array<Array<value_type, Allocator>> chunk(const size_type n) const
		{
			Array<Array<value_type, Allocator>> result;

			if (n == 0)
			{
				return result;
			}

			const size_type s = size();
			const size_type chunkCount = (s / n) + static_cast<size_type>((s % n) != 0);

			result.reserve(chunkCount);

			for (size_type i = 0; i < chunkCount; ++i)
			{
				const size_type index = (i * n);
				const size_type length = (((s - index) < n) ? (s - index) : n);

				result.push_back(slice(index, length));
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
			return (std::ranges::find(m_container, value) != m_container.end());
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
		constexpr bool contains_if(Fty f) const
			requires std::predicate<Fty&, const value_type&>
		{
			return std::ranges::any_of(m_container, detail::PassFunction(std::forward<Fty>(f)));
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
			return std::ranges::count(m_container, value);
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
		constexpr isize count_if(Fty f) const
			requires std::predicate<Fty&, const value_type&>
		{
			return std::ranges::count_if(m_container, detail::PassFunction(std::forward<Fty>(f)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	drop
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から最大 n 個を除いた配列を返します。
		/// @param n 取り除く最大要素数
		/// @return 残りの要素の配列
		/// @remark n は size() にクランプされ、アロケータを引き継ぎます。
		[[nodiscard]]
		constexpr Array drop(size_type n) const&
		{
			return Array((m_container.begin() + Min(n, size())), m_container.end(), m_container.get_allocator());
		}

		/// @brief 先頭から最大 n 個を除いた配列を返します。
		/// @param n 取り除く最大要素数
		/// @return 残りの要素の配列
		/// @remark n は size() にクランプされ、アロケータを引き継ぎます。記憶域を再利用します。
		[[nodiscard]]
		constexpr Array drop(size_type n) &&
		{
			m_container.erase(m_container.begin(), (m_container.begin() + Min(n, size())));
			return std::move(*this);
		}

		////////////////////////////////////////////////////////////////
		//
		//	drop_while
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から条件を満たす間の要素を除いた配列を返します。
		/// @param f 要素を判定する述語
		/// @return 最初に条件を満たさなくなった要素から末尾までの配列
		/// @remark 順序とアロケータを維持します。
		template <class Fty>
		[[nodiscard]]
		constexpr Array drop_while(Fty f) const&
			requires std::predicate<Fty&, const value_type&>
		{
			const auto first = std::find_if_not(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
			return Array(first, m_container.end(), m_container.get_allocator());
		}

		/// @brief 先頭から条件を満たす間の要素を除いた配列を返します。
		/// @param f 要素を判定する述語
		/// @return 最初に条件を満たさなくなった要素から末尾までの配列
		/// @remark 順序とアロケータを維持します。記憶域を再利用します。
		template <class Fty>
		[[nodiscard]]
		constexpr Array drop_while(Fty f) &&
			requires std::predicate<Fty&, const value_type&>
		{
			const auto first = std::find_if_not(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
			m_container.erase(m_container.begin(), first);
			return std::move(*this);
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
		constexpr void each(Fty f)
			requires std::invocable<Fty&, value_type&>
		{
			std::ranges::for_each(m_container, detail::PassFunction(std::forward<Fty>(f)));
		}

		/// @brief すべての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (const auto& x : xs) f(x);` と同じです。
		template <class Fty>
		constexpr void each(Fty f) const
			requires std::invocable<Fty&, const value_type&>
		{
			std::ranges::for_each(m_container, detail::PassFunction(std::forward<Fty>(f)));
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
		constexpr void each_index(Fty f)
			requires std::invocable<Fty&, size_t, value_type&>
		{
			for (size_t i = 0; auto & elem : m_container)
			{
				std::invoke(f, i++, elem);
			}
		}

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (size_t i = 0; const auto& x : xs) f(i++, x);` と同じです。
		template <class Fty>
		constexpr void each_index(Fty f) const
			requires std::invocable<Fty&, size_t, const value_type&>
		{
			for (size_t i = 0; const auto & elem : m_container)
			{
				std::invoke(f, i++, elem);
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
		constexpr void each_sindex(Fty f)
			requires std::invocable<Fty&, isize, value_type&>
		{
			for (isize i = 0; auto & elem : m_container)
			{
				std::invoke(f, i++, elem);
			}
		}

		/// @brief すべての要素とそのインデックスを順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (isize i = 0; auto x : xs) f(i++, x);` と同じです。
		template <class Fty>
		constexpr void each_sindex(Fty f) const
			requires std::invocable<Fty&, isize, const value_type&>
		{
			for (isize i = 0; const auto & elem : m_container)
			{
				std::invoke(f, i++, elem);
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
		constexpr value_type fetch(size_type index, U&& defaultValue) const
			noexcept(std::is_nothrow_constructible_v<value_type, U>)
			requires std::constructible_from<value_type, U>
		{
			if (m_container.size() <= index)
			{
				return value_type(std::forward<U>(defaultValue));
			}

			return m_container[index];
		}

		////////////////////////////////////////////////////////////////
		//
		//	find_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす最初の要素へのポインタを返します。
		/// @param f 要素を判定する述語
		/// @return 見つかった要素へのポインタ。見つからなければ nullptr
		/// @remark 配列の破棄・再確保・要素の移動で無効になります。
		template <class Fty>
		[[nodiscard]]
		constexpr value_type* find_if(Fty f) & SIV3D_LIFETIMEBOUND
			requires std::predicate<Fty&, const value_type&>
		{
			const auto it = std::ranges::find_if(m_container, detail::PassFunction(std::forward<Fty>(f)));
			return (it == m_container.end()) ? nullptr : std::addressof(*it);
		}

		/// @brief 条件を満たす最初の要素へのポインタを返します。
		/// @param f 要素を判定する述語
		/// @return 見つかった要素へのポインタ。見つからなければ nullptr
		/// @remark 配列の破棄・再確保・要素の移動で無効になります。
		template <class Fty>
		[[nodiscard]]
		constexpr const value_type* find_if(Fty f) const& SIV3D_LIFETIMEBOUND
			requires std::predicate<Fty&, const value_type&>
		{
			const auto it = std::ranges::find_if(m_container, detail::PassFunction(std::forward<Fty>(f)));
			return (it == m_container.end()) ? nullptr : std::addressof(*it);
		}

		/// @brief 一時配列からの要素ポインタの取得を禁止します。
		template <class Fty>
		constexpr const value_type* find_if(Fty f) const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	fill
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値をすべての要素に代入します。
		/// @param value 代入する値
		/// @return *this
		constexpr Array& fill(const value_type& value) & SIV3D_LIFETIMEBOUND
		{
			std::ranges::fill(m_container, value);
			return *this;
		}

		/// @brief 一時配列を変更し、所有権を持つ配列を返します。
		/// @return 記憶域とアロケータを引き継いだ配列
		[[nodiscard]]
		constexpr Array fill(const value_type& value) &&
		{
			fill(value);
			return std::move(*this);
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
		constexpr Array filter(Fty f) const&
			requires std::predicate<Fty&, const value_type&>
		{
			Array result(m_container.get_allocator());

			for (const auto& value : m_container)
			{
				if (std::invoke(f, value))
				{
					result.push_back(value);
				}
			}

			return result;
		}

		/// @brief 条件を満たす要素を元の順序で残します。
		/// @param f 要素を判定する述語
		/// @return 記憶域とアロケータを再利用した配列
		/// @remark 例外時は要素の一部が移動済みの場合があります。
		template <class Fty>
		[[nodiscard]]
		constexpr Array filter(Fty f) &&
			requires std::predicate<Fty&, const value_type&>
		{
			std::erase_if(m_container, [&f](const value_type& value) { return (not std::invoke(f, value)); });
			return std::move(*this);
		}

		////////////////////////////////////////////////////////////////
		//
		//	fold_left
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素を左から順に関数に適用していき、1 つの値にまとめます。
		/// @tparam R 結果の型
		/// @tparam Fty 関数の型
		/// @param init 初期値
		/// @param f 関数
		/// @return まとめられた値
		template <class R, class Fty>
		constexpr auto fold_left(R init, Fty f) const;

		////////////////////////////////////////////////////////////////
		//
		//	get_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した要素へのポインタを返します。
		/// @param index 要素のインデックス
		/// @return 範囲内なら要素へのポインタ、範囲外なら nullptr
		/// @remark 配列の破棄・再確保・要素の移動で無効になります。
		[[nodiscard]]
		constexpr value_type* get_if(size_type index) & noexcept SIV3D_LIFETIMEBOUND
		{
			return (index < size()) ? std::addressof(m_container[index]) : nullptr;
		}

		/// @brief 指定した要素へのポインタを返します。
		/// @param index 要素のインデックス
		/// @return 範囲内なら要素へのポインタ、範囲外なら nullptr
		/// @remark 配列の破棄・再確保・要素の移動で無効になります。
		[[nodiscard]]
		constexpr const value_type* get_if(size_type index) const& noexcept SIV3D_LIFETIMEBOUND
		{
			return (index < size()) ? std::addressof(m_container[index]) : nullptr;
		}

		/// @brief 一時配列からの要素ポインタの取得を禁止します。
		constexpr const value_type* get_if(size_type index) const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	head_span
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から最大 n 個の要素を参照する span を返します。
		/// @param n 参照する最大要素数
		/// @return 先頭から最大 n 個の要素を参照する `std::span`
		/// @remark メモリのコピーは発生しません。元の配列が破棄・再確保されると参照先が無効になるため注意してください。
		/// @remark `n` が現在の要素数を超える場合は現在の要素数にクランプされます。
		/// @remark ダングリング参照を防ぐため、右辺値オブジェクトからの呼び出しはコンパイルエラーになります。
		[[nodiscard]]
		constexpr std::span<value_type> head_span(size_type n) & noexcept
		{
			const auto k = Min(n, m_container.size());
			return std::span<value_type>{ m_container }.first(k);
		}

		/// @brief 先頭から最大 n 個の要素を参照する span を返します。
		/// @param n 参照する最大要素数
		/// @return 先頭から最大 n 個の要素を参照する `std::span`
		/// @remark メモリのコピーは発生しません。元の配列が破棄・再確保されると参照先が無効になるため注意してください。
		/// @remark `n` が現在の要素数を超える場合は現在の要素数にクランプされます。
		/// @remark ダングリング参照を防ぐため、右辺値オブジェクトからの呼び出しはコンパイルエラーになります。	
		[[nodiscard]]
		constexpr std::span<const value_type> head_span(size_type n) const& noexcept
		{
			const auto k = Min(n, m_container.size());
			return std::span<const value_type>{ m_container }.first(k);
		}

		/// @brief 先頭から最大 n 個の要素を参照する span を返します。
		/// @param n 参照する最大要素数
		/// @return 先頭から最大 n 個の要素を参照する `std::span`
		/// @remark メモリのコピーは発生しません。元の配列が破棄・再確保されると参照先が無効になるため注意してください。
		/// @remark `n` が現在の要素数を超える場合は現在の要素数にクランプされます。
		/// @remark ダングリング参照を防ぐため、右辺値オブジェクトからの呼び出しはコンパイルエラーになります。	
		constexpr std::span<value_type> head_span(size_type n) && = delete;

		/// @brief const な一時配列からの借用を禁止します。
		constexpr std::span<value_type> head_span(size_type n) const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	head_view
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から最大 n 個の要素を取り出す Ranges ビューを返します。
		/// @param n 取り出す最大要素数
		/// @return `std::views::take` による遅延評価ビュー
		[[nodiscard]]
		constexpr auto head_view(size_type n) & noexcept
		{
			return std::views::take(m_container, n);
		}

		/// @brief 先頭から最大 n 個の要素を取り出す Ranges ビューを返します。
		/// @param n 取り出す最大要素数
		/// @return `std::views::take` による遅延評価ビュー
		[[nodiscard]]
		constexpr auto head_view(size_type n) const& noexcept
		{
			return std::views::take(m_container, n);
		}

		/// @brief 先頭から最大 n 個の要素を取り出す Ranges ビューを返します。
		/// @param n 取り出す最大要素数
		/// @return `std::views::take` による遅延評価ビュー
		[[nodiscard]]
		constexpr auto head_view(size_type n) && noexcept
		{
			return std::views::take(std::move(*this), n);
		}

		/// @brief const な一時配列からの呼び出しを禁止します。
		constexpr auto head_view(size_type n) const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	in_groups
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を指定したグループ数に分割します。
		/// @param group グループ数
		/// @return 元のアロケータの型と状態を各内側配列に引き継いだグループ
		[[nodiscard]]
		constexpr Array<Array<value_type, Allocator>> in_groups(size_type group) const
		{
			Array<Array<value_type, Allocator>> result;

			if (group == 0)
			{
				return result;
			}

			const size_type s = size();
			if (s == 0)
			{
				return result;
			}

			const size_type g = ((group < s) ? group : s); // 空グループを作らない
			result.reserve(g);

			const size_type div = s / g;
			const size_type mod = s % g;

			size_type index = 0;
			for (size_type i = 0; i < g; ++i)
			{
				const size_type length = div + (i < mod ? 1 : 0);
				result.emplace_back(slice(index, length));
				index += length;
			}

			return result;
		}

		////////////////////////////////////////////////////////////////
		//
		//	indexOf_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす最初の要素のインデックスを返します。
		/// @param f 要素を判定する述語
		/// @return 見つかった要素のインデックス。見つからなければ none
		template <class Fty>
		[[nodiscard]]
		constexpr Optional<size_t> indexOf_if(Fty f) const
			requires std::predicate<Fty&, const value_type&>
		{
			const auto it = std::ranges::find_if(m_container, detail::PassFunction(std::forward<Fty>(f)));
			if (it == m_container.end())
			{
				return s3d::none;
			}
			return static_cast<size_t>(it - m_container.begin());
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
			return std::ranges::is_sorted(m_container);
		}

		////////////////////////////////////////////////////////////////
		//
		//	indexOf
		//
		////////////////////////////////////////////////////////////////

		constexpr Optional<size_t> indexOf(const value_type& value) const noexcept
		{
			if (const auto it = std::ranges::find(m_container, value); 
				it != m_container.end())
			{
				return std::ranges::distance(m_container.begin(), it);
			}
			else
			{
				return s3d::none;
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	join
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素から文字列を生成します。
		/// @param sep 要素の間に挿入する文字列
		/// @return 生成された文字列
		[[nodiscard]]
		constexpr String join(const StringView sep = U", ") const
		{
			return join(sep, U"", U"");
		}

		/// @brief 配列の要素から文字列を生成します。
		/// @param sep 要素の間に挿入する文字列
		/// @param begin 先頭に挿入する文字列
		/// @param end 末尾に挿入する文字列
		/// @return 生成された文字列
		[[nodiscard]]
		constexpr String join(const StringView sep, const StringView begin, const StringView end) const
		{
			const size_t n = m_container.size();
			const size_t sepCount = (n > 1 ? (n - 1) : 0);

			FormatData formatData;
			formatData.string.reserve(begin.size() + end.size() + (sep.size() * sepCount) + (n * 5));
			formatData.string.append(begin);

			auto it = m_container.begin();
			const auto itEnd = m_container.end();

			if (it != itEnd)
			{
				Formatter(formatData, *it);
				++it;

				for (; it != itEnd; ++it)
				{
					formatData.string.append(sep);
					Formatter(formatData, *it);
				}
			}

			formatData.string.append(end);
			return formatData.string;
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
		constexpr auto map(Fty f) const
			requires detail::ArrayMapFunction<Fty, value_type>
		{
			using result_value_type = std::decay_t<std::invoke_result_t<Fty&, const value_type&>>;

			Array<result_value_type> result;

			result.reserve(m_container.size());

			for (const auto& value : m_container)
			{
				result.push_back(std::invoke(f, value));
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
		constexpr bool none(Fty f = Identity) const
			requires std::predicate<Fty&, const value_type&>
		{
			return std::ranges::none_of(m_container, detail::PassFunction(std::forward<Fty>(f)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	partition
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たすすべての要素を、条件を満たさないすべての要素より前に移動させます。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 条件を満たさない最初の要素のイテレータ。すべて満たす場合は end()
		/// @remark 左辺値の配列でのみ呼び出せます。
		template <class Fty>
		constexpr iterator partition(Fty f) & SIV3D_LIFETIMEBOUND
			requires std::predicate<Fty&, const value_type&>
		{
			return std::ranges::partition(m_container, detail::PassFunction(std::forward<Fty>(f))).begin();
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
				std::ranges::fill(m_container, newValue);
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
				return Array(*this, m_container.get_allocator());
			}
			else
			{
				return Array(m_container.size(), newValue, m_container.get_allocator());
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
		constexpr Array& replace_if(Fty f, const value_type& newValue)& SIV3D_LIFETIMEBOUND
			requires std::predicate<Fty&, const value_type&>
		{
			std::ranges::replace_if(m_container, f, newValue);
			return *this;
		}

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array replace_if(Fty f, const value_type& newValue) &&
			requires std::predicate<Fty&, const value_type&>
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
		constexpr Array replaced_if(Fty f, const value_type& newValue) const&
			requires std::predicate<Fty&, const value_type&>
		{
			Array result(m_container.get_allocator());
			result.reserve(m_container.size());

			for (const auto& v : m_container)
			{
				result.push_back(std::invoke(f, v) ? newValue : v);
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
		constexpr Array replaced_if(Fty f, const value_type& newValue) &&
			requires std::predicate<Fty&, const value_type&>
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
			std::ranges::reverse(m_container);
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
			return Array(m_container.rbegin(), m_container.rend(), m_container.get_allocator());
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
		constexpr void reverse_each(Fty f)
			requires std::invocable<Fty&, value_type&>
		{
			std::for_each(m_container.rbegin(), m_container.rend(), detail::PassFunction(std::forward<Fty>(f)));
		}

		/// @brief 末尾から順番に、すべての要素に対して関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (const auto& x : xs) f(x);` と同じです。
		template <class Fty>
		constexpr void reverse_each(Fty f) const
			requires std::invocable<Fty&, const value_type&>
		{
			std::for_each(m_container.rbegin(), m_container.rend(), detail::PassFunction(std::forward<Fty>(f)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	reverse_view
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の逆順ビューを返します。
		/// @return 配列の逆順ビュー
		[[nodiscard]]
		constexpr auto reverse_view()&
		{
			return std::views::reverse(m_container);
		}

		/// @brief 配列の逆順ビューを返します。
		/// @return 配列の逆順ビュー
		[[nodiscard]]
		constexpr auto reverse_view() const&
		{
			return std::views::reverse(m_container);
		}

		/// @brief 配列の逆順ビューを返します。
		/// @return 配列の逆順ビュー
		[[nodiscard]]
		constexpr auto reverse_view()&&
		{
			return std::views::reverse(std::move(*this));
		}

		/// @brief const な一時配列からの呼び出しを禁止します。
		constexpr auto reverse_view() const&& = delete;

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
			if (m_container.size() < middle)
			{
				detail::ThrowArrayRotateMiddleOutOfRange();
			}

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
			if (m_container.size() < middle)
			{
				detail::ThrowArrayRotateMiddleOutOfRange();
			}

			Array result(m_container.get_allocator());
			result.reserve(m_container.size());

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
			const isize trueCount = std::ranges::count(m_container, true);

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
			const isize trueCount = std::ranges::count(m_container, true);

			Array result(m_container.size(), false, m_container.get_allocator());
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
		/// @param urbg 使用する乱数エンジン
		/// @return *this
		constexpr Array& shuffle(Concept::UniformRandomBitGenerator auto&& urbg)& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @param urbg 使用する乱数エンジン
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array shuffle(Concept::UniformRandomBitGenerator auto&& urbg)&&;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @param urbg 使用する乱数エンジン
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array shuffled(Concept::UniformRandomBitGenerator auto&& urbg) const&;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @param urbg 使用する乱数エンジン
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array shuffled(Concept::UniformRandomBitGenerator auto&& urbg)&&;

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
			if (m_container.size() < index)
			{
				detail::ThrowArraySliceIndexOutOfRange();
			}

			const size_type maxLength = (m_container.size() - index);

			if (maxLength < length)
			{
				detail::ThrowArraySliceIndexOutOfRange();
			}

			return Array((m_container.begin() + index), (m_container.begin() + index + length), m_container.get_allocator());
		}

		/// @brief index から末尾までの配列を返します。
		/// @param index 開始位置
		/// @return index から末尾までの配列。index == size() は空の配列
		/// @throw std::out_of_range index > size() の場合
		/// @remark アロケータを引き継ぎます。
		[[nodiscard]]
		constexpr Array slice(size_type index) const&
		{
			if (size() < index)
			{
				detail::ThrowArraySliceIndexOutOfRange();
			}
			return slice(index, (size() - index));
		}

		/// @brief index から末尾までの配列を返します。
		/// @param index 開始位置
		/// @return index から末尾までの配列。index == size() は空の配列
		/// @throw std::out_of_range index > size() の場合
		/// @remark アロケータを引き継ぎます。
		[[nodiscard]]
		constexpr Array slice(size_type index) &&
		{
			if (size() < index)
			{
				detail::ThrowArraySliceIndexOutOfRange();
			}
			return std::move(*this).slice(index, (size() - index));
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
			const isize falseCount = std::ranges::count(m_container, false);

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
			const size_type falseCount = static_cast<size_type>(std::ranges::count(m_container, false));

			Array result(size(), true, get_allocator());
			std::fill_n(result.begin(), falseCount, false);
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
		constexpr Array& sort_and_unique() & SIV3D_LIFETIMEBOUND
		{
			const bool hasTrue = (std::ranges::find(m_container, true) != m_container.end());
			const bool hasFalse = (std::ranges::find(m_container, false) != m_container.end());

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
		constexpr Array sort_and_unique() &&
		{
			return std::move(sort_and_unique());
		}

		/// @brief 配列をソートしたあとに重複する要素を削除した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sorted_and_uniqued() const&
		{
			const bool hasTrue = (std::ranges::find(m_container, true) != m_container.end());
			const bool hasFalse = (std::ranges::find(m_container, false) != m_container.end());

			if (hasTrue && hasFalse)
			{
				return Array({ false, true }, m_container.get_allocator());
			}
			else if (hasTrue)
			{
				return Array({ true }, m_container.get_allocator());
			}
			else if (hasFalse)
			{
				return Array({ false }, m_container.get_allocator());
			}

			return Array(m_container.get_allocator());
		}

		/// @brief 配列をソートしたあとに重複する要素を削除した新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sorted_and_uniqued() &&
		{
			return std::move(sort_and_unique());
		}

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
		constexpr Array& sort_by(Fty f)& SIV3D_LIFETIMEBOUND
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
		{
			const value_type falseValue = false;
			const value_type trueValue = true;

			if (std::invoke(f, trueValue, falseValue))
			{
				return rsort();
			}
			else if (std::invoke(f, falseValue, trueValue))
			{
				return sort();
			}

			return *this;
		}

		/// @brief 指定した関数を用いて要素を昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array sort_by(Fty f) &&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
		{
			return std::move(sort_by(std::forward<Fty>(f)));
		}

		/// @brief 指定した関数を用いて要素を昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array sorted_by(Fty f) const&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
		{
			Array result(*this, m_container.get_allocator());
			result.sort_by(std::forward<Fty>(f));
			return result;
		}

		/// @brief 指定した関数を用いて要素を昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array sorted_by(Fty f) &&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
		{
			return std::move(sort_by(std::forward<Fty>(f)));
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
		//	stable_sort_by, stable_sorted_by
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えます。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return *this
		template <class Fty>
		constexpr Array& stable_sort_by(Fty f)& SIV3D_LIFETIMEBOUND
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
		{
			return sort_by(std::forward<Fty>(f));
		}

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array stable_sort_by(Fty f) &&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
		{
			return std::move(stable_sort_by(std::forward<Fty>(f)));
		}

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array stable_sorted_by(Fty f) const&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
		{
			Array result(*this, m_container.get_allocator());
			result.stable_sort_by(std::forward<Fty>(f));
			return result;
		}

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array stable_sorted_by(Fty f) &&
			requires std::strict_weak_order<Fty&, const value_type&, const value_type&>
		{
			return std::move(stable_sort_by(std::forward<Fty>(f)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	stable_partition
		//
		////////////////////////////////////////////////////////////////

		/// @brief 相対順序を保ちながら、条件を満たすすべての要素を、条件を満たさないすべての要素より前に移動させます。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 条件を満たさない最初の要素のイテレータ。すべて満たす場合は end()
		/// @remark 左辺値の配列でのみ呼び出せます。
		template <class Fty>
		constexpr iterator stable_partition(Fty f) & SIV3D_LIFETIMEBOUND
			requires std::predicate<Fty&, const value_type&>
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
			const auto itTrue = std::ranges::find(m_container, true);
			const auto itFalse = std::ranges::find(m_container, false);

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
			const auto itTrue = std::ranges::find(m_container, true);
			const auto itFalse = std::ranges::find(m_container, false);

			const bool hasTrue = (itTrue != m_container.end());
			const bool hasFalse = (itFalse != m_container.end());

			if (hasTrue && hasFalse)
			{
				if (itFalse < itTrue)
				{
					return Array({ false, true }, m_container.get_allocator());
				}
				else
				{
					return Array({ true, false }, m_container.get_allocator());
				}
			}
			else if (hasTrue)
			{
				return Array({ true }, m_container.get_allocator());
			}
			else if (hasFalse)
			{
				return Array({ false }, m_container.get_allocator());
			}

			return Array(m_container.get_allocator());
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
			return std::ranges::count(m_container, true);
		}

		////////////////////////////////////////////////////////////////
		//
		//	tail
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾から最大 n 個の要素を取り出した新しい配列を返します。
		/// @param n 取り出す要素数
		/// @return 末尾から最大 n 個を含む新しい配列
		/// @remark n が size() を超える場合は size() にクランプされます。
		[[nodiscard]]
		constexpr Array tail(size_type n) const&
		{
			const auto k = Min(n, m_container.size());
			return Array(m_container.end() - k, m_container.end(), m_container.get_allocator());
		}

		/// @brief 末尾から最大 n 個の要素を取り出した新しい配列を返します。
		/// @param n 取り出す要素数
		/// @return 末尾から最大 n 個を含む新しい配列
		/// @remark n が size() を超える場合は size() にクランプされます。
		[[nodiscard]]
		constexpr Array tail(size_type n)&&
		{
			const auto k = Min(n, m_container.size());
			return Array(
				std::make_move_iterator(m_container.end() - k),
				std::make_move_iterator(m_container.end()),
				m_container.get_allocator());
		}

		////////////////////////////////////////////////////////////////
		//
		//	tail_span
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の最大 n 個の要素を参照する span を返します。
		/// @param n 参照する最大要素数
		/// @return 末尾の最大 n 個の要素を参照する `std::span`
		/// @remark メモリのコピーは発生しません。元の配列が破棄・再確保されると参照先が無効になるため注意してください。
		/// @remark `n` が現在の要素数を超える場合は現在の要素数にクランプされます。
		/// @remark ダングリング参照を防ぐため、右辺値オブジェクトからの呼び出しはコンパイルエラーになります。
		[[nodiscard]]
		constexpr std::span<value_type> tail_span(size_type n) & noexcept
		{
			const auto k = Min(n, m_container.size());
			return std::span<value_type>{ m_container }.last(k);
		}

		/// @brief 末尾の最大 n 個の要素を参照する span を返します。
		/// @param n 参照する最大要素数
		/// @return 末尾の最大 n 個の要素を参照する `std::span`
		/// @remark メモリのコピーは発生しません。元の配列が破棄・再確保されると参照先が無効になるため注意してください。
		/// @remark `n` が現在の要素数を超える場合は現在の要素数にクランプされます。
		/// @remark ダングリング参照を防ぐため、右辺値オブジェクトからの呼び出しはコンパイルエラーになります。	
		[[nodiscard]]
		constexpr std::span<const value_type> tail_span(size_type n) const& noexcept
		{
			const auto k = Min(n, m_container.size());
			return std::span<const value_type>{ m_container }.last(k);
		}

		/// @brief 末尾の最大 n 個の要素を参照する span を返します。
		/// @param n 参照する最大要素数
		/// @return 末尾の最大 n 個の要素を参照する `std::span`
		/// @remark メモリのコピーは発生しません。元の配列が破棄・再確保されると参照先が無効になるため注意してください。
		/// @remark `n` が現在の要素数を超える場合は現在の要素数にクランプされます。
		/// @remark ダングリング参照を防ぐため、右辺値オブジェクトからの呼び出しはコンパイルエラーになります。	
		constexpr std::span<value_type> tail_span(size_type n) && = delete;

		/// @brief const な一時配列からの借用を禁止します。
		constexpr std::span<value_type> tail_span(size_type n) const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	tail_view
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の最大 n 個の要素を取り出す Ranges ビューを返します。
		/// @param n 取り出す最大要素数
		/// @return `std::views::take` による遅延評価ビュー
		[[nodiscard]]
		constexpr auto tail_view(size_type n) & noexcept
		{
			const auto dropCount = (n < size()) ? (size() - n) : 0;
			return std::views::drop(m_container, dropCount);
		}

		/// @brief 末尾の最大 n 個の要素を取り出す Ranges ビューを返します。
		/// @param n 取り出す最大要素数
		/// @return `std::views::take` による遅延評価ビュー
		[[nodiscard]]
		constexpr auto tail_view(size_type n) const& noexcept
		{
			const auto dropCount = (n < size()) ? (size() - n) : 0;
			return std::views::drop(m_container, dropCount);
		}

		/// @brief 末尾の最大 n 個の要素を取り出す Ranges ビューを返します。
		/// @param n 取り出す最大要素数
		/// @return `std::views::take` による遅延評価ビュー
		[[nodiscard]]
		constexpr auto tail_view(size_type n) && noexcept
		{
			const auto dropCount = (n < size()) ? (size() - n) : 0;
			return std::views::drop(std::move(*this), dropCount);
		}

		/// @brief const な一時配列からの呼び出しを禁止します。
		constexpr auto tail_view(size_type n) const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	take
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭から指定した個数の要素からなる新しい配列を返します。
		/// @param n 取り出す要素数
		/// @return 新しい配列
		/// @remark n が配列の要素数以上の場合、配列全体を返します。
		/// @remark アロケータを引き継ぎます。右辺値では記憶域を再利用します。
		[[nodiscard]]
		constexpr Array take(size_type n) const&
		{
			return Array(m_container.begin(), (m_container.begin() + Min(n, m_container.size())), m_container.get_allocator());
		}

		/// @brief 先頭から指定した個数の要素からなる新しい配列を返します。
		/// @param n 取り出す要素数
		/// @return 新しい配列
		/// @remark n が配列の要素数以上の場合、配列全体を返します。
		/// @remark アロケータを引き継ぎます。右辺値では記憶域を再利用します。
		[[nodiscard]]
		constexpr Array take(size_type n)&&
		{
			m_container.erase(Min(n, m_container.size()));
			return std::move(*this);
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
		constexpr Array take_while(Fty f) const&
			requires std::predicate<Fty&, const value_type&>
		{
			return Array(m_container.begin(), std::find_if_not(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f))), m_container.get_allocator());
		}

		/// @brief 先頭から、条件を満たさなくなる直前までの要素からなる新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array take_while(Fty f) &&
			requires std::predicate<Fty&, const value_type&>
		{
			const auto first = std::find_if_not(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
			m_container.erase(first, m_container.end());
			return std::move(*this);
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
			Array result(m_container.get_allocator());
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
		//	without
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しいすべての要素を削除した新しい配列を返します。
		/// @param value 削除する値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array without(const value_type& value) const&
		{
			const isize newSize = std::ranges::count(m_container, (not value));

			return Array(newSize, (not value), m_container.get_allocator());
		}

		/// @brief 指定した値と等しいすべての要素を削除した新しい配列を返します。
		/// @param value 削除する値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array without(const value_type& value)&&
		{
			return std::move(without(value));
		}

		////////////////////////////////////////////////////////////////
		//
		//	without_at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにある要素を削除した新しい配列を返します。
		/// @param index インデックス
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array without_at(size_type index) const&
		{
			if (m_container.size() <= index)
			{
				detail::ThrowArrayWithoutAtIndexOutOfRange();
			}

			Array result(m_container.get_allocator());
			result.reserve(m_container.size() - 1);
			result.insert(result.end(), m_container.begin(), (m_container.begin() + index));
			result.insert(result.end(), (m_container.begin() + index + 1), m_container.end());

			return result;
		}

		/// @brief 指定したインデックスにある要素を削除した新しい配列を返します。
		/// @param index インデックス
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array without_at(size_type index)&&
		{
			return std::move(without_at(index));
		}

		////////////////////////////////////////////////////////////////
		//
		//	without_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 条件を満たす要素を配列から削除した新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Array without_if(Fty f) const&
			requires std::predicate<Fty&, const value_type&>
		{
			Array result(m_container.get_allocator());

			for (const auto& v : m_container)
			{
				if (not std::invoke(f, v))
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
		constexpr Array without_if(Fty f) &&
			requires std::predicate<Fty&, const value_type&>
		{
			return std::move(without_if(detail::PassFunction(std::forward<Fty>(f))));
		}

		////////////////////////////////////////////////////////////////

		/// @brief 各要素の変換結果を元の順序で返します（並列実行）。
		/// @param f 全 worker が共有する関数。関数自身と参照する状態への同時アクセスの同期は呼び出し側の責任です。
		/// @return std::allocator を使う変換結果の配列。結果型は構築・ムーブ構築可能である必要があります。デフォルト構築・代入は不要です。
		/// @remark 呼び出し順序は未規定です。例外は開始済み worker の終了を待ってから伝播します。入力を変更してはいけません。
		template <class Fty>
		[[nodiscard]]
		auto parallel_map(Fty f) const
			requires detail::ArrayMapFunction<Fty, value_type>;

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
		constexpr auto operator >>(Fty f) const
			requires std::invocable<Fty&, const value_type&>
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
