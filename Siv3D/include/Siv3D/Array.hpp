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
# include "String.hpp"
# include "Unicode.hpp"
# include "Format.hpp"
# include "Utility.hpp"
# include "PredefinedNamedParameter.hpp"

namespace s3d
{
	template <class Type, class Allocator>
	class Array;	

	template <class Type>
	concept HasAsArray = requires(Type t)
	{
		{ t.asArray() } -> std::convertible_to<Array<typename Type::value_type, std::allocator<typename Type::value_type>>>;
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

		/// @brief 
		/// @param alloc 
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

		/// @brief 
		/// @param other 
		/// @param alloc 
		[[nodiscard]]
		constexpr Array(const Array& other, const Allocator& alloc);

		/// @brief 
		/// @param other 
		/// @param alloc 
		[[nodiscard]]
		constexpr Array(Array&& other, const Allocator& alloc);

		/// @brief 
		/// @param other 
		/// @param alloc 
		[[nodiscard]]
		constexpr Array(const container_type& other, const Allocator& alloc);

		/// @brief 
		/// @param other 
		/// @param alloc 
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
		
		template <Concept::ContainerCompatibleRange<Type> Range>
		[[nodiscard]]
		constexpr Array(std::from_range_t, Range&& range, const Allocator& alloc = Allocator{});

	# endif
		
		/// @brief ジェネレータ関数を使って配列を作成します。
		/// @tparam Fty ジェネレータ関数の型
		/// @param size 作成する配列の要素数
		/// @param generator ジェネレータ関数
		template <class Fty> requires (std::invocable<Fty> && std::same_as<std::invoke_result_t<Fty>, Type>)
		[[nodiscard]]
		Array(size_type size, Arg::generator_<Fty> generator);

		/// @brief インデックス指定ジェネレータ関数を使って配列を作成します。
		/// @tparam Fty ジェネレータ関数の型
		/// @param size 作成する配列の要素数
		/// @param indexedGenerator インデックス指定ジェネレータ関数
		template <class Fty>
		[[nodiscard]]
		Array(size_type size, Arg::indexedGenerator_<Fty> indexedGenerator);

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

		constexpr Array& operator =(const container_type& other);

		constexpr Array& operator =(container_type&& other);

		constexpr Array& operator =(const HasAsArray auto& a);

		constexpr Array& operator =(HasAsArray auto&& a);

		constexpr Array& operator =(std::initializer_list<value_type> list);

		////////////////////////////////////////////////////////////////
		//
		//	assign
		//
		////////////////////////////////////////////////////////////////

		constexpr Array& assign(size_type count, const value_type& value);

		template <class Iterator>
		constexpr Array& assign(Iterator first, Iterator last);

		constexpr Array& assign(std::initializer_list<value_type> list);

		////////////////////////////////////////////////////////////////
		//
		//	assign_range
		//
		////////////////////////////////////////////////////////////////

		template <Concept::ContainerCompatibleRange<Type> Range>
		constexpr Array& assign_range(Range&& range);

		////////////////////////////////////////////////////////////////
		//
		//	get_allocator
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr allocator_type get_allocator() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getContainer
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr const container_type& getContainer() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator container_type
		//
		////////////////////////////////////////////////////////////////

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
		constexpr const value_type& at(size_type index) const;

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		constexpr value_type& at(size_type index);

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		[[nodiscard]]
		constexpr const value_type& operator [](size_type index) const noexcept;

		/// @brief 要素にアクセスします。
		/// @param index 要素へのインデックス
		/// @return 要素への参照
		[[nodiscard]]
		constexpr value_type& operator [](size_type index) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の要素への参照を返します。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		constexpr reference front() noexcept;

		/// @brief 先頭の要素への参照を返します。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		constexpr const_reference front() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の要素への参照を返します。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		constexpr reference back() noexcept;

		/// @brief 末尾の要素への参照を返します。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		constexpr const_reference back() const noexcept;

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
		//	begin / end
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
		//	cbegin / cend
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
		//	rbegin / rend
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
		//	crbegin / crend
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
		constexpr size_t size_bytes() const noexcept requires (Concept::TriviallyCopyable<Type>);

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

		/// @brief 要素数 n までメモリの再確保無しで格納できるよう、必要に応じてメモリを再確保します
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

		/// @brief capacity() を size() まで減らすようにします。
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

		constexpr iterator insert(const_iterator pos, const value_type& value);

		constexpr iterator insert(const_iterator pos, value_type&& value);

		constexpr iterator insert(const_iterator pos, size_type count, const value_type& value);

		template <class Iterator>
		constexpr iterator insert(const_iterator pos, Iterator first, Iterator last);

		constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> list);

		////////////////////////////////////////////////////////////////
		//
		//	emplace
		//
		////////////////////////////////////////////////////////////////

		template <class... Args>
		constexpr iterator emplace(const_iterator pos, Args&&... args);

		////////////////////////////////////////////////////////////////
		//
		//	append_range
		//
		////////////////////////////////////////////////////////////////

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
		//	all
		//
		////////////////////////////////////////////////////////////////

		/// @brief 全ての要素が条件を満たすかを返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件を記述した関数
		/// @return 全ての要素が条件を満たすか、配列が空の場合 true, それ以外の場合は false
		template <class Fty = decltype(Identity)>
		[[nodiscard]]
		constexpr bool all(Fty f = Identity) const requires std::predicate<Fty, value_type>;

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
		constexpr bool any(Fty f = Identity) const requires std::predicate<Fty, value_type>;

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に別の配列を追加します。
		/// @param other 追加する配列
		/// @return *this
		constexpr Array& append(const Array& other);

	//	/// @brief 配列の要素を 1 つランダムに返します。
	//	/// @return 配列からランダムに選ばれた要素への参照
	//	[[nodiscard]]
	//	value_type& choice();

	//	/// @brief 配列の要素を 1 つランダムに返します。
	//	/// @return 配列からランダムに選ばれた要素への参照
	//	[[nodiscard]]
	//	const value_type& choice() const;

	//	/// @brief 指定した乱数エンジンを用いて、配列の要素を 1 つランダムに返します。
	//	/// @tparam URBG 使用する乱数エンジンの型
	//	/// @param rbg 使用する乱数エンジン
	//	/// @return 配列からランダムに選ばれた要素への参照
	//	[[nodiscard]]
	//	value_type& choice(Concept::UniformRandomBitGenerator auto&& rbg);

	//	/// @brief 指定した乱数エンジンを用いて、配列の要素を 1 つランダムに返します。
	//	/// @tparam URBG 使用する乱数エンジンの型
	//	/// @param rbg 使用する乱数エンジン
	//	/// @return 配列からランダムに選ばれた要素への参照
	//	[[nodiscard]]
	//	const value_type& choice(Concept::UniformRandomBitGenerator auto&& rbg) const;

	//	/// @brief 配列の要素から指定した個数だけ重複なくランダムに選んで返します。
	//	/// @tparam Int 個数に使う整数型
	//	/// @param n 選択する個数
	//	/// @return ランダムに選ばれた要素の配列
	//	[[nodiscard]]
	//	Array choice(Concept::Integral auto n) const;

	//	/// @brief 指定した乱数エンジンを用いて、 配列の要素から指定した個数だけ重複なくランダムに選んで返します。
	//	/// @tparam Size_t 個数に使う整数型
	//	/// @param n 選択する個数
	//	/// @param rbg 使用する乱数エンジン
	//	/// @return ランダムに選ばれた要素の配列
	//	template <Concept::Integral Size_t, Concept::UniformRandomBitGenerator URBG>
	//	[[nodiscard]]
	//	Array choice(Size_t n, URBG&& rbg) const;

	//	/// @brief 指定した個数の要素を持つ配列のグループに分割します。最後のグループの要素数は n 個未満になることがあります。
	//	/// @param n 1 つのグループが持つ要素数
	//	/// @remark { 0, 1, 2, 3, 4, 5, 6 } を 3 個の要素を持つ配列のグループに分割すると { { 0, 1, 2 }, { 3, 4, 5 }, { 6 }} になります。
	//	/// @return 分割されたグループ
	//	[[nodiscard]]
	//	Array<Array<value_type>> chunk(size_t n) const;

	//	/// @brief 指定した値と等しい要素があるかを返します。
	//	/// @param value 検索する値
	//	/// @return 指定した値と等しい要素がある場合 true, それ以外の場合は false
	//	[[nodiscard]]
	//	bool contains(const value_type& value) const;

	//	/// @brief 指定した条件を満たす要素があるかを返します。
	//	/// @tparam Fty 条件を記述した関数の型
	//	/// @param f 条件を記述した関数
	//	/// @remark `.any(f)` と同じです。
	//	/// @return 条件を満たす要素が 1 つでもあれば true, それ以外の場合は false
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	[[nodiscard]]
	//	bool contains_if(Fty f) const;

	//	/// @brief 指定した値と等しい要素の個数を返します。
	//	/// @param value 検索する値
	//	/// @return 指定した値と等しい要素の個数
	//	[[nodiscard]]
	//	size_t count(const value_type& value) const;

	//	/// @brief 条件を満たす要素の個数を返します。
	//	/// @tparam Fty 条件を記述した関数の型
	//	/// @param f 条件を記述した関数
	//	/// @return 条件を満たす要素の個数
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	[[nodiscard]]
	//	size_t count_if(Fty f) const;

	//	/// @brief 全ての要素を順番に引数にして関数を呼び出します。
	//	/// @tparam Fty 呼び出す関数の型
	//	/// @param f 呼び出す関数
	//	/// @remark `for (auto& x : xs) f(x);` と同じです。
	//	/// @return *this
	//	template <class Fty, std::enable_if_t<std::is_invocable_v<Fty, Type&>>* = nullptr>
	//	Array& each(Fty f);

	//	/// @brief 全ての要素を順番に引数にして関数を呼び出します。
	//	/// @tparam Fty 呼び出す関数の型
	//	/// @param f 呼び出す関数
	//	/// @remark `for (const auto& x : xs) f(x);` と同じです。
	//	/// @return *this
	//	template <class Fty, std::enable_if_t<std::is_invocable_v<Fty, Type>>* = nullptr>
	//	const Array& each(Fty f) const;

	//	/// @brief 全ての要素とそのインデックスを順番に引数にして関数を呼び出します。
	//	/// @tparam Fty 呼び出す関数の型
	//	/// @param f 呼び出す関数
	//	/// @remark `for (size_t i = 0; auto& x : xs) f(i++, x);` と同じです。
	//	/// @return *this
	//	template <class Fty, std::enable_if_t<std::is_invocable_v<Fty, size_t, Type&>>* = nullptr>
	//	Array& each_index(Fty f);

	//	/// @brief 全ての要素とそのインデックスを順番に引数にして関数を呼び出します。
	//	/// @tparam Fty 呼び出す関数の型
	//	/// @param f 呼び出す関数
	//	/// @remark `for (size_t i = 0; const auto& x : xs) f(i++, x);` と同じです。
	//	/// @return *this
	//	template <class Fty, std::enable_if_t<std::is_invocable_v<Fty, size_t, Type>>* = nullptr>
	//	const Array& each_index(Fty f) const;

	//	/// @brief 指定したインデックスにある要素を返します。インデックスが範囲外の場合デフォルト値を返します。
	//	/// @param index インデックス
	//	/// @param defaultValue インデックスが範囲外の場合に返すデフォルト値
	//	/// @return 指定したインデックスにある要素、範囲外の場合 defaultValue
	//	template <class U>
	//	[[nodiscard]]
	//	value_type fetch(size_t index, U&& defaultValue) const;

	//	/// @brief 指定した値を全ての要素に代入します。
	//	/// @param value 代入する値
	//	/// @return *this
	//	Array& fill(const value_type& value);

	//	/// @brief 指定した条件を満たす要素だけを集めた新しい配列を返します。
	//	/// @tparam Fty 条件を記述した関数の型
	//	/// @param f 条件を記述した関数
	//	/// @remark 結果において、要素の前後関係は維持されます。
	//	/// @return 指定した条件を満たす要素を集めた新しい配列
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	[[nodiscard]]
	//	Array filter(Fty f) const;

	//	/// @brief 要素を指定したグループ数に分割します。
	//	/// @param group グループ数
	//	/// @remark { 0, 1, 2, 3, 4, 5, 6 } を 3 グループに分割すると { { 0, 1, 2 }, { 3, 4 }, { 5, 6 }} になります。
	//	/// @return 分割したグループ
	//	[[nodiscard]]
	//	Array<Array<value_type>> in_groups(size_t group) const;

	//	/// @brief 指定した値と等しい要素があるかを返します。
	//	/// @param value 検索する値
	//	/// @return 指定した値と等しい要素がある場合 true, それ以外の場合は false
	//	/// @remark `.contains(value)` と同じです。
	//	[[nodiscard]]
	//	bool includes(const value_type& value) const;

	//	/// @brief 指定した条件を満たす要素があるかを返します。
	//	/// @tparam Fty 条件を記述した関数の型
	//	/// @param f 条件を記述した関数
	//	/// @remark `.any(f)` と同じです。
	//	/// @return 条件を満たす要素が 1 つでもあれば true, それ以外の場合は false
	//	/// @remark `.contains_if(f)` および `.any(f)` と同じです。
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	[[nodiscard]]
	//	bool includes_if(Fty f) const;

	//	/// @brief 配列の要素が昇順にソートされているかを返します。
	//	/// @return 配列の要素が昇順にソートされている場合 true, それ以外の場合は false
	//	[[nodiscard]]
	//	bool isSorted() const requires Concept::LessThanComparable<Type>;

	//	/// @brief 
	//	/// @param sep 
	//	/// @param begin 
	//	/// @param end 
	//	/// @return 
	//	[[nodiscard]]
	//	String join(StringView sep = U", ", StringView begin = U"{", StringView end = U"}") const;

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	/// @return 
	//	template <class Fty, std::enable_if_t<std::is_invocable_v<Fty, Type>>* = nullptr>
	//	auto map(Fty f) const;

	//	/// @brief 条件を満たす要素が存在しないかを返します。
	//	/// @tparam Fty 条件を記述した関数の型
	//	/// @param f 条件を記述した関数
	//	/// @return 条件を満たす要素数が 0 個の場合 true, それ以外の場合は false
	//	template <class Fty = decltype(Identity), std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	[[nodiscard]]
	//	bool none(Fty f = Identity) const;

	//	/// @brief 条件を満たすすべての要素を、条件を満たさないすべての要素より前に移動させます。
	//	/// @tparam Fty 条件を記述した関数の型
	//	/// @param f 条件を記述した関数
	//	/// @return 区分化された境界を指すイテレータ
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	auto partition(Fty f);

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @tparam R 
	//	/// @param f 
	//	/// @param init 
	//	/// @return 
	//	template <class Fty, class R = std::decay_t<std::invoke_result_t<Fty, Type, Type>>>
	//	auto reduce(Fty f, R init) const;

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	/// @return 
	//	template <class Fty, std::enable_if_t<std::is_invocable_v<Fty, Type, Type>>* = nullptr>
	//	auto reduce1(Fty f) const;

	//	/// @brief 
	//	/// @param value 
	//	/// @return 
	//	Array& remove(const value_type& value);

	//	/// @brief 
	//	/// @param value 
	//	/// @return 
	//	[[nodiscard]]
	//	Array removed(const value_type& value) const&;

	//	/// @brief 
	//	/// @param value 
	//	/// @return 
	//	[[nodiscard]]
	//	Array removed(const value_type& value)&&;

	//	/// @brief 指定したインデックスにある要素を削除します。
	//	/// @param index インデックス
	//	/// @return *this
	//	Array& remove_at(size_t index);

	//	/// @brief 指定したインデックスにある要素を削除した新しい配列を返します。
	//	/// @param index インデックス
	//	/// @return 新しい配列
	//	[[nodiscard]]
	//	Array removed_at(size_t index) const&;

	//	/// @brief 指定したインデックスにある要素を削除した新しい配列を返します。
	//	/// @param index インデックス
	//	/// @return 新しい配列
	//	[[nodiscard]]
	//	Array removed_at(size_t index) &&;

	//	/// @brief 条件を満たす要素を配列から削除します。
	//	/// @tparam Fty 条件を記述した関数の型
	//	/// @param f 条件
	//	/// @return *this
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	Array& remove_if(Fty f);

	//	/// @brief 条件を満たす要素を配列から削除した新しい配列を返します。
	//	/// @tparam Fty 条件を記述した関数の型
	//	/// @param f 条件
	//	/// @return 新しい配列
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	[[nodiscard]]
	//	Array removed_if(Fty f) const&;

	//	/// @brief 条件を満たす要素を配列から削除した新しい配列を返します。
	//	/// @tparam Fty 条件を記述した関数の型
	//	/// @param f 条件
	//	/// @return 新しい配列
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	[[nodiscard]]
	//	Array removed_if(Fty f)&&;

	//	/// @brief 指定した値と等しい全ての要素を別の値に置き換えます。
	//	/// @param oldValue 置き換えられる値
	//	/// @param newValue 新しい値
	//	/// @return *this
	//	Array& replace(const value_type& oldValue, const value_type& newValue);

	//	/// @brief 指定した値と等しい全ての要素を別の値に置き換えた新しい配列を返します。
	//	/// @param oldValue 置き換えられる値
	//	/// @param newValue 新しい値
	//	/// @return 新しい配列
	//	[[nodiscard]]
	//	Array replaced(const value_type& oldValue, const value_type& newValue) const&;

	//	/// @brief 指定した値と等しい全ての要素を別の値に置き換えた新しい配列を返します。
	//	/// @param oldValue 置き換えられる値
	//	/// @param newValue 新しい値
	//	/// @return 新しい配列
	//	[[nodiscard]]
	//	Array replaced(const value_type& oldValue, const value_type& newValue)&&;

	//	/// @brief 指定した条件を満たす全ての要素を別の値に置き換えます。
	//	/// @tparam Fty 条件を記述した関数の型
	//	/// @param f 条件
	//	/// @param newValue 新しい値
	//	/// @return *this
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	Array& replace_if(Fty f, const value_type& newValue);

	//	/// @brief 指定した条件を満たす全ての要素を別の値に置き換えた新しい配列を返します。
	//	/// @tparam Fty 条件を記述した関数の型
	//	/// @param f 条件
	//	/// @param newValue 新しい値
	//	/// @return 新しい配列
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	[[nodiscard]]
	//	Array replaced_if(Fty f, const value_type& newValue) const&;

	//	/// @brief 指定した条件を満たす全ての要素を別の値に置き換えた新しい配列を返します。
	//	/// @tparam Fty 条件を記述した関数の型
	//	/// @param f 条件
	//	/// @param newValue 新しい値
	//	/// @return 新しい配列
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	[[nodiscard]]
	//	Array replaced_if(Fty f, const value_type& newValue)&&;

	//	/// @brief 配列の要素を逆順に並び替えます。
	//	/// @return *this
	//	Array& reverse();

	//	/// @brief 配列の要素を逆順に並び替えた新しい配列を返します。
	//	/// @return 新しい配列
	//	[[nodiscard]]
	//	Array reversed() const&;

	//	/// @brief 配列の要素を逆順に並び替えた新しい配列を返します。
	//	/// @return 新しい配列
	//	[[nodiscard]]
	//	Array reversed()&&;

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	/// @return 
	//	template <class Fty, std::enable_if_t<std::is_invocable_v<Fty, Type&>>* = nullptr>
	//	Array& reverse_each(Fty f);

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	/// @return 
	//	template <class Fty, std::enable_if_t<std::is_invocable_v<Fty, Type>>* = nullptr>
	//	const Array& reverse_each(Fty f) const;

	//	/// @brief 
	//	/// @param count 
	//	/// @return 
	//	Array& rotate(std::ptrdiff_t count = 1);

	//	/// @brief 
	//	/// @param count 
	//	/// @return 
	//	[[nodiscard]]
	//	Array rotated(std::ptrdiff_t count = 1) const&;

	//	/// @brief 
	//	/// @param count 
	//	/// @return 
	//	[[nodiscard]]
	//	Array rotated(std::ptrdiff_t count = 1)&&;

	//	/// @brief 要素を降順に並び替えます。
	//	/// @return *this
	//	Array& rsort() requires Concept::GreaterThanComparable<Type>;

	//	/// @brief 要素を降順に並び替えた新しい配列を返します。
	//	/// @return 新しい配列
	//	[[nodiscard]]
	//	Array rsorted() const& requires Concept::GreaterThanComparable<Type>;

	//	/// @brief 要素を降順に並び替えた新しい配列を返します。
	//	/// @return 新しい配列
	//	[[nodiscard]]
	//	Array rsorted()&& requires Concept::GreaterThanComparable<Type>;

	//	/// @brief 配列の要素の並び順をランダムにシャッフルします。
	//	/// @return *this
	//	Array& shuffle();

	//	/// @brief 
	//	/// @param rbg 
	//	/// @return 
	//	Array& shuffle(Concept::UniformRandomBitGenerator auto&& rbg);

	//	/// @brief 
	//	/// @return 
	//	[[nodiscard]]
	//	Array shuffled() const&;

	//	/// @brief 
	//	/// @return 
	//	[[nodiscard]]
	//	Array shuffled()&&;

	//	/// @brief 
	//	/// @tparam URBG 
	//	/// @param rbg 
	//	/// @return 
	//	[[nodiscard]]
	//	Array shuffled(Concept::UniformRandomBitGenerator auto&& rbg) const&;

	//	/// @brief 
	//	/// @tparam URBG 
	//	/// @param rbg 
	//	/// @return 
	//	[[nodiscard]]
	//	Array shuffled(Concept::UniformRandomBitGenerator auto&& rbg)&&;

	//	/// @brief 
	//	/// @param index 
	//	/// @return 
	//	[[nodiscard]]
	//	Array slice(size_t index) const;

	//	/// @brief 
	//	/// @param index 
	//	/// @param length 
	//	/// @return 
	//	[[nodiscard]]
	//	Array slice(size_t index, size_t length) const;

		////////////////////////////////////////////////////////////////
		//
		//	sort / sorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を昇順に並び替えます。
		/// @return *this
		constexpr Array& sort()& requires Concept::LessThanComparable<Type>;

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sort()&& requires Concept::LessThanComparable<Type>;

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sorted() const& requires Concept::LessThanComparable<Type>;

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Array sorted()&& requires Concept::LessThanComparable<Type>;

	//	/// @brief 要素を相対順序を保ちながら昇順に並び替えます。
	//	/// @return *this
	//	Array& stable_sort() requires Concept::LessThanComparable<Type>;

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	/// @return 
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type, Type>>* = nullptr>
	//	Array& sort_by(Fty f);

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	/// @return 
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type, Type>>* = nullptr>
	//	Array& stable_sort_by(Fty f);

	//	/// @brief 要素を昇順に並び替えた新しい配列を返します。
	//	/// @return 新しい配列
	//	[[nodiscard]]
	//	Array sorted() const& requires Concept::LessThanComparable<Type>;

	//	/// @brief 相対順序を保ちながら、条件を満たすすべての要素を、条件を満たさないすべての要素より前に移動させます。
	//	/// @tparam Fty 条件を記述した関数の型
	//	/// @param f 条件を記述した関数
	//	/// @return 区分化された境界を指すイテレータ
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	auto stable_partition(Fty f);

	//	/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
	//	/// @return 新しい配列
	//	[[nodiscard]]
	//	Array stable_sorted() const& requires Concept::LessThanComparable<Type>;

	//	/// @brief 要素を昇順に並び替えた新しい配列を返します。
	//	/// @return 新しい配列
	//	[[nodiscard]]
	//	Array sorted()&& requires Concept::LessThanComparable<Type>;

	//	/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
	//	/// @return 新しい配列
	//	[[nodiscard]]
	//	Array stable_sorted()&& requires Concept::LessThanComparable<Type>;

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	/// @return 
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type, Type>>* = nullptr>
	//	[[nodiscard]]
	//	Array sorted_by(Fty f) const&;

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	/// @return 
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type, Type>>* = nullptr>
	//	[[nodiscard]]
	//	Array stable_sorted_by(Fty f) const&;

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	/// @return 
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type, Type>>* = nullptr>
	//	[[nodiscard]]
	//	Array sorted_by(Fty f)&&;

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	/// @return 
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type, Type>>* = nullptr>
	//	[[nodiscard]]
	//	Array stable_sorted_by(Fty f)&&;

	//	/// @brief 要素を `+` 演算子を用いて合計します。
	//	/// @return 合計値
	//	[[nodiscard]]
	//	auto sum() const requires Concept::Addable<Type>;

	//	/// @brief 浮動小数点数型の要素を、誤差が小さくなるように合計します。
	//	/// @remark `sum()` よりも浮動小数点数誤差が小さくなります。
	//	/// @return 合計値
	//	[[nodiscard]]
	//	auto sumF() const requires Concept::FloatingPoint<Type>;

	//	/// @brief 
	//	/// @param n 
	//	/// @return 
	//	[[nodiscard]]
	//	Array take(size_t n) const;

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	/// @return 
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	[[nodiscard]]
	//	Array take_while(Fty f) const;

	//	/// @brief 
	//	/// @return 
	//	Array& stable_unique();

	//	/// @brief 
	//	/// @return 
	//	[[nodiscard]]
	//	Array stable_uniqued() const&;

	//	/// @brief 
	//	/// @return 
	//	[[nodiscard]]
	//	Array stable_uniqued() &&;

	//	/// @brief 
	//	/// @return 
	//	Array& sort_and_unique();

	//	/// @brief 
	//	/// @return 
	//	[[nodiscard]]
	//	Array sorted_and_uniqued() const&;

	//	/// @brief 
	//	/// @return 
	//	[[nodiscard]]
	//	Array sorted_and_uniqued()&&;

	//	/// @brief 
	//	/// @return 
	//	Array& unique_consecutive();

	//	/// @brief 
	//	/// @return 
	//	[[nodiscard]]
	//	Array uniqued_consecutive() const&;

	//	/// @brief 
	//	/// @return 
	//	[[nodiscard]]
	//	Array uniqued_consecutive()&&;

	//	/// @brief 
	//	/// @param indices 
	//	/// @return 
	//	[[nodiscard]]
	//	Array values_at(std::initializer_list<size_t> indices) const;

	//	/// @brief 各要素に関数を適用します。
	//	/// @tparam Fty 適用する関数の型
	//	/// @param f 適用する関数
	//	/// @remark Fty が戻り値を持たない場合 `.each(f), 戻り値を持つ場合は `.map(f)` と同じです。
	//	/// @return 各要素に関数を適用した結果の配列。Fty が戻り値を持たない場合 void
	//	template <class Fty, std::enable_if_t<std::is_invocable_v<Fty, Type>>* = nullptr>
	//	auto operator >>(Fty f) const;

	//# ifndef SIV3D_NO_CONCURRENT_API

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	/// @return 
	//	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, Type>>* = nullptr>
	//	[[nodiscard]]
	//	size_t parallel_count_if(Fty f) const;

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	template <class Fty, std::enable_if_t<std::is_invocable_v<Fty, Type&>>* = nullptr>
	//	void parallel_each(Fty f);

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	template <class Fty, std::enable_if_t<std::is_invocable_v<Fty, Type>>* = nullptr>
	//	void parallel_each(Fty f) const;

	//	/// @brief 
	//	/// @tparam Fty 
	//	/// @param f 
	//	/// @return 
	//	template <class Fty, std::enable_if_t<std::is_invocable_v<Fty, Type>>* = nullptr>
	//	auto parallel_map(Fty f) const;

	//# endif

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

		/// @brief 
		/// @tparam Fty 
		/// @param size 
		/// @param generator 
		/// @return 
		template <class Fty> requires (std::invocable<Fty>&& std::same_as<std::invoke_result_t<Fty>, Type>)
		[[nodiscard]]
		static Array Generate(size_type size, Fty generator);

		/// @brief 
		/// @tparam Fty 
		/// @param size 
		/// @param indexedGenerator 
		/// @return 
		template <class Fty>
		[[nodiscard]]
		static Array IndexedGenerate(size_type size, Fty indexedGenerator);

		////////////////////////////////////////////////////////////////
		//
		//	operator ==, operator <=>
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(const Array& lhs, const Array& rhs)
		{
			return (lhs.m_container == rhs.m_container);
		}

	# if __cpp_lib_three_way_comparison

		[[nodiscard]]
		friend constexpr auto operator <=>(const Array& lhs, const Array& rhs)
		{
			return (lhs.m_container <=> rhs.m_container);
		}

	# else

		[[nodiscard]]
		friend constexpr auto operator <=>(const Array& lhs, const Array& rhs)
		{
			return LexicographicalCompareThreeWay(lhs.m_container.begin(), lhs.m_container.end(), rhs.m_container.begin(), rhs.m_container.end(), std::compare_three_way{});
		}

	# endif

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		friend constexpr void swap(Array& lhs, Array& rhs) noexcept
		{
			lhs.swap(rhs);
		}

	private:

		container_type m_container;
	};

	/// @brief range から Array を作成します。
	/// @tparam Range range の型
	/// @tparam Elem range の要素の型
	/// @param range range
	/// @return 作成した Array
	template <class Range, class Elem = std::ranges::range_value_t<Range>>
	constexpr Array<Elem> ToArray(Range&& range) requires Concept::ContainerCompatibleRange<Range, Elem>;

	////////////////////////////////////////////////////////////////
	//
	//	Deduction guides
	//

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

	//
	////////////////////////////////////////////////////////////////
}

# include "detail/Array.ipp"
# include "detail/BoolArray.ipp"
