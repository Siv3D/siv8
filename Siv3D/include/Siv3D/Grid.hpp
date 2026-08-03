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
# include <ranges>
# include "Common.hpp"
# include "Array.hpp"
# include "PointVector.hpp"
# include "Shuffle.hpp"
# include "GridConnectivity.hpp"

namespace s3d
{
	struct Rect;

	namespace detail
	{
		template <class Container, class Fty>
		concept GridHasParallelCountIf = requires(Container& container, Fty&& function)
		{
			container.parallel_count_if(std::forward<Fty>(function));
		};

		template <class Container, class Fty>
		concept GridHasParallelEach = requires(Container& container, Fty&& function)
		{
			container.parallel_each(std::forward<Fty>(function));
		};

		template <class Container, class Fty>
		concept GridHasParallelMap = requires(Container& container, Fty&& function)
		{
			container.parallel_map(std::forward<Fty>(function));
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Grid
	//
	////////////////////////////////////////////////////////////////

	/// @brief 二次元配列クラス
	/// @tparam Type 要素の型
	/// @tparam Allocator アロケータの型
	template <class Type, class Allocator = std::allocator<Type>>
	class Grid
	{
	public:

		using container_type			= Array<Type, Allocator>;
		using value_type				= typename container_type::value_type;
		using pointer					= typename container_type::pointer;
		using const_pointer				= typename container_type::const_pointer;
		using reference					= typename container_type::reference;
		using const_reference			= typename container_type::const_reference;
		using iterator					= typename container_type::iterator;
		using const_iterator			= typename container_type::const_iterator;
		using reverse_iterator			= typename container_type::reverse_iterator;
		using const_reverse_iterator	= typename container_type::const_reverse_iterator;
		using size_type					= typename container_type::size_type;
		using difference_type			= typename container_type::difference_type;
		using allocator_type			= typename container_type::allocator_type;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ。空の二次元配列を作成します。
		[[nodiscard]]
		Grid() = default;

		/// @brief コピーコンストラクタ
		/// @param other コピー元の Grid
		[[nodiscard]]
		Grid(const Grid& other) = default;

		/// @brief ムーブコンストラクタ
		/// @param other ムーブ元の Grid
		[[nodiscard]]
		Grid(Grid&& other) = default;

		/// @brief 二次元配列を作成します。
		/// @param w 幅
		/// @param h 高さ
		[[nodiscard]]
		constexpr Grid(size_type w, size_type h);

		/// @brief 二次元配列を作成します。
		/// @param w 幅
		/// @param h 高さ
		/// @param value 要素の初期値
		[[nodiscard]]
		constexpr Grid(size_type w, size_type h, const value_type& value);

		/// @brief 二次元配列を作成します。
		/// @param size 幅と高さ
		[[nodiscard]]
		constexpr explicit Grid(Size size);

		/// @brief 二次元配列を作成します。
		/// @param size 幅と高さ
		/// @param value 要素の初期値
		[[nodiscard]]
		constexpr Grid(Size size, const value_type& value);

		/// @brief 配列から二次元配列を作成します。
		/// @param w 幅
		/// @param h 高さ
		/// @param data 幅 × 高さの要素数を持つ配列
		[[nodiscard]]
		constexpr Grid(size_type w, size_type h, const Array<value_type>& data);

		/// @brief 配列から二次元配列を作成します。
		/// @param w 幅
		/// @param h 高さ
		/// @param data 幅 × 高さの要素数を持つ配列
		[[nodiscard]]
		constexpr Grid(size_type w, size_type h, Array<value_type>&& data);

		/// @brief 配列から二次元配列を作成します。
		/// @param size 幅と高さ
		/// @param data 幅 × 高さの要素数を持つ配列
		[[nodiscard]]
		constexpr Grid(Size size, const Array<value_type>& data);

		/// @brief 配列から二次元配列を作成します。
		/// @param size 幅と高さ
		/// @param data 幅 × 高さの要素数を持つ配列
		[[nodiscard]]
		constexpr Grid(Size size, Array<value_type>&& data);

		/// @brief 初期化リストから二次元配列を作成します。
		/// @param set 初期化リスト
		[[nodiscard]]
		constexpr Grid(const std::initializer_list<std::initializer_list<value_type>>& set);

		/// @brief ジェネレータ関数を使って二次元配列を作成します。
		/// @param w 幅
		/// @param h 高さ
		/// @param generator ジェネレータ関数
		[[nodiscard]]
		constexpr Grid(size_type w, size_type h, Arg::generator_<FunctionRef<value_type()>> generator);

		/// @brief インデックス指定ジェネレータ関数を使って二次元配列を作成します。
		/// @param w 幅
		/// @param h 高さ
		/// @param generator インデックス指定ジェネレータ関数 (x, y) を受け取る
		[[nodiscard]]
		constexpr Grid(size_type w, size_type h, Arg::generator_<FunctionRef<value_type(int32, int32)>> generator);

		/// @brief Point 指定ジェネレータ関数を使って二次元配列を作成します。
		/// @param w 幅
		/// @param h 高さ
		/// @param generator Point 指定ジェネレータ関数
		[[nodiscard]]
		constexpr Grid(size_type w, size_type h, Arg::generator_<FunctionRef<value_type(Point)>> generator);

		/// @brief ジェネレータ関数を使って二次元配列を作成します。
		/// @param size 幅と高さ
		/// @param generator ジェネレータ関数
		[[nodiscard]]
		constexpr Grid(Size size, Arg::generator_<FunctionRef<value_type()>> generator);

		/// @brief インデックス指定ジェネレータ関数を使って二次元配列を作成します。
		/// @param size 幅と高さ
		/// @param generator インデックス指定ジェネレータ関数 (x, y) を受け取る
		[[nodiscard]]
		constexpr Grid(Size size, Arg::generator_<FunctionRef<value_type(int32, int32)>> generator);

		/// @brief Point 指定ジェネレータ関数を使って二次元配列を作成します。
		/// @param size 幅と高さ
		/// @param generator Point 指定ジェネレータ関数
		[[nodiscard]]
		constexpr Grid(Size size, Arg::generator_<FunctionRef<value_type(Point)>> generator);

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		/// @brief コピー代入演算子
		/// @param other コピーする配列
		/// @return *this
		Grid& operator =(const Grid&) = default;

		/// @brief ムーブ代入演算子
		/// @param other ムーブする配列
		/// @return *this
		Grid& operator =(Grid&&) = default;

		////////////////////////////////////////////////////////////////
		//
		//	assign
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したサイズと値で二次元配列を再構築します。
		/// @param w 幅
		/// @param h 高さ
		/// @param value 要素の値
		/// @return *this
		constexpr Grid& assign(size_type w, size_type h, const value_type& value) SIV3D_LIFETIMEBOUND;

		/// @brief 指定したサイズと値で二次元配列を再構築します。
		/// @param size 幅と高さ
		/// @param value 要素の値
		/// @return *this
		constexpr Grid& assign(Size size, const value_type& value) SIV3D_LIFETIMEBOUND;

		/// @brief 初期化リストで二次元配列を再構築します。
		/// @param set 初期化リスト
		/// @return *this
		constexpr Grid& assign(const std::initializer_list<std::initializer_list<value_type>>& set) SIV3D_LIFETIMEBOUND;

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

		/// @brief Array への const 参照を返します。
		/// @return Array への const 参照
		[[nodiscard]]
		constexpr const container_type& getContainer() const& noexcept SIV3D_LIFETIMEBOUND;

		/// @brief Array を返します。
		/// @return Array
		[[nodiscard]]
		constexpr container_type getContainer() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	at
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の要素にアクセスします。
		/// @param y 行番号
		/// @param x 列番号
		/// @return 要素への参照
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		constexpr reference at(size_type y, size_type x)& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の要素にアクセスします。
		/// @param y 行番号
		/// @param x 列番号
		/// @return 要素への参照
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		constexpr const_reference at(size_type y, size_type x) const& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の要素にアクセスします。
		/// @param y 行番号
		/// @param x 列番号
		/// @return 要素
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		constexpr value_type at(size_type y, size_type x)&&;

		/// @brief 指定した位置の要素にアクセスします。
		/// @param pos 位置
		/// @return 要素への参照
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		constexpr reference at(Point pos)& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の要素にアクセスします。
		/// @param pos 位置
		/// @return 要素への参照
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		constexpr const_reference at(Point pos) const& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の要素にアクセスします。
		/// @param pos 位置
		/// @return 要素
		/// @throw std::out_of_range 範囲外アクセスの場合 throw
		constexpr value_type at(Point pos)&&;

		////////////////////////////////////////////////////////////////
		//
		//	wrappedAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を二次元配列の範囲内に循環させて、要素にアクセスします。
		/// @param pos 位置
		/// @return 要素への参照
		/// @throw std::out_of_range 二次元配列の幅または高さが 0 の場合
		[[nodiscard]]
		constexpr reference wrappedAt(Point pos)& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置を二次元配列の範囲内に循環させて、要素にアクセスします。
		/// @param pos 位置
		/// @return 要素への参照
		/// @throw std::out_of_range 二次元配列の幅または高さが 0 の場合
		[[nodiscard]]
		constexpr const_reference wrappedAt(Point pos) const& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置を二次元配列の範囲内に循環させて、要素にアクセスします。
		/// @param pos 位置
		/// @return 要素
		/// @throw std::out_of_range 二次元配列の幅または高さが 0 の場合
		[[nodiscard]]
		constexpr value_type wrappedAt(Point pos)&&;

		////////////////////////////////////////////////////////////////
		//
		//	clampedAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を二次元配列の範囲内に制限して、要素にアクセスします。
		/// @param pos 位置
		/// @return 要素への参照
		/// @throw std::out_of_range 二次元配列の幅または高さが 0 の場合
		[[nodiscard]]
		constexpr reference clampedAt(Point pos)& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置を二次元配列の範囲内に制限して、要素にアクセスします。
		/// @param pos 位置
		/// @return 要素への参照
		/// @throw std::out_of_range 二次元配列の幅または高さが 0 の場合
		[[nodiscard]]
		constexpr const_reference clampedAt(Point pos) const& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置を二次元配列の範囲内に制限して、要素にアクセスします。
		/// @param pos 位置
		/// @return 要素
		/// @throw std::out_of_range 二次元配列の幅または高さが 0 の場合
		[[nodiscard]]
		constexpr value_type clampedAt(Point pos)&&;

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した行のポインタを返します。
		/// @param y 行番号
		/// @return 行の先頭ポインタ
		[[nodiscard]]
		constexpr pointer operator [](size_type y) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した行のポインタを返します。
		/// @param y 行番号
		/// @return 行の先頭ポインタ
		[[nodiscard]]
		constexpr const_pointer operator [](size_type y) const SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の要素にアクセスします。
		/// @param pos 位置
		/// @return 要素への参照
		[[nodiscard]]
		constexpr reference operator [](Point pos)& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の要素にアクセスします。
		/// @param pos 位置
		/// @return 要素への参照
		[[nodiscard]]
		constexpr const_reference operator [](Point pos) const& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の要素にアクセスします。
		/// @param pos 位置
		/// @return 要素
		[[nodiscard]]
		constexpr value_type operator [](Point pos)&&;

		/// @brief 指定した位置の要素にアクセスします。
		/// @param y 行番号
		/// @param x 列番号
		/// @return 要素への参照
		[[nodiscard]]
		constexpr reference operator [](size_type y, size_type x)& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の要素にアクセスします。
		/// @param y 行番号
		/// @param x 列番号
		/// @return 要素への参照
		[[nodiscard]]
		constexpr const_reference operator [](size_type y, size_type x) const& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置の要素にアクセスします。
		/// @param y 行番号
		/// @param x 列番号
		/// @return 要素
		[[nodiscard]]
		constexpr value_type operator [](size_type y, size_type x)&&;

		////////////////////////////////////////////////////////////////
		//
		//	front
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の要素への参照を返します。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		constexpr reference front() & noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 先頭の要素への参照を返します。
		/// @return 先頭の要素への参照
		[[nodiscard]]
		constexpr const_reference front() const& noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 先頭の要素を返します。
		/// @return 先頭の要素
		[[nodiscard]]
		constexpr value_type front() &&
			noexcept(std::is_nothrow_move_constructible_v<value_type>);

		////////////////////////////////////////////////////////////////
		//
		//	back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の要素への参照を返します。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		constexpr reference back() & noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 末尾の要素への参照を返します。
		/// @return 末尾の要素への参照
		[[nodiscard]]
		constexpr const_reference back() const& noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 末尾の要素を返します。
		/// @return 末尾の要素
		[[nodiscard]]
		constexpr value_type back() &&
			noexcept(std::is_nothrow_move_constructible_v<value_type>);

		////////////////////////////////////////////////////////////////
		//
		//	indexInBounds
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスが範囲内であるかを返します。
		/// @param y 行番号
		/// @param x 列番号
		/// @return 範囲内の場合 true、そうでない場合 false
		[[nodiscard]]
		constexpr bool indexInBounds(size_type y, size_type x) const noexcept;

		/// @brief 指定したインデックスが範囲内であるかを返します。
		/// @param pos 位置
		/// @return 範囲内の場合 true、そうでない場合 false
		[[nodiscard]]
		constexpr bool indexInBounds(Point pos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	data
		//
		////////////////////////////////////////////////////////////////

		/// @brief 先頭の要素を指すポインタを返します。
		/// @return 先頭の要素を指すポインタ
		[[nodiscard]]
		constexpr pointer data() noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 先頭の要素を指すポインタを返します。
		/// @return 先頭の要素を指すポインタ
		[[nodiscard]]
		constexpr const_pointer data() const noexcept SIV3D_LIFETIMEBOUND;

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
		//	width, height
		//
		////////////////////////////////////////////////////////////////

		/// @brief 二次元配列の幅（列数）を返します。
		/// @return 二次元配列の幅（列数）
		[[nodiscard]]
		constexpr int32 width() const noexcept;

		/// @brief 二次元配列の高さ（行数）を返します。
		/// @return 二次元配列の高さ（行数）
		[[nodiscard]]
		constexpr int32 height() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////
	
		/// @brief 二次元配列の幅と高さ（列数と行数）を返します。
		/// @return 二次元配列の幅と高さ（列数と行数）
		[[nodiscard]]
		constexpr Size size() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	bounds
		//
		////////////////////////////////////////////////////////////////

		/// @brief この二次元配列の範囲を表す長方形を返します。
		/// @return `(0, 0)` を左上とし、配列の幅と高さを持つ長方形
		/// @remark この関数を使用するには `<Siv3D/GridRect.hpp>` をインクルードしてください。
		[[nodiscard]]
		constexpr Rect bounds() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	num_elements
		//
		////////////////////////////////////////////////////////////////

		/// @brief 二次元配列の要素数を返します。
		/// @return 二次元配列の要素数
		[[nodiscard]]
		constexpr size_t num_elements() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	bytesPerRow
		//
		////////////////////////////////////////////////////////////////

		/// @brief 1 行あたりの合計サイズ（バイト）を返します。
		/// @return 1 行あたりの合計サイズ（バイト）
		[[nodiscard]]
		constexpr uint32 bytesPerRow() const noexcept
			requires (Concept::TriviallyCopyable<value_type>);

		////////////////////////////////////////////////////////////////
		//
		//	size_bytes
		//
		////////////////////////////////////////////////////////////////

		/// @brief 二次元配列の合計サイズ（バイト）を返します。
		/// @return 二次元配列の合計サイズ（バイト）
		[[nodiscard]]
		constexpr size_t size_bytes() const noexcept
			requires (Concept::TriviallyCopyable<value_type>);

		////////////////////////////////////////////////////////////////
		//
		//	reserve
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素数 w * h までメモリの再確保無しで格納できるよう、必要に応じてメモリを再確保します。
		/// @param w 幅
		/// @param h 高さ
		constexpr void reserve(size_type w, size_type h);

		/// @brief 要素数 size までメモリの再確保無しで格納できるよう、必要に応じてメモリを再確保します。
		/// @param size 幅と高さ
		constexpr void reserve(Size size);

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

		/// @brief 配列を空にします。
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
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 他の配列と要素を入れ替えます。
		/// @param other 入れ替える配列
		constexpr void swap(Grid& other)
			noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value || std::allocator_traits<Allocator>::is_always_equal::value);

		////////////////////////////////////////////////////////////////
		//
		//	row
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した行の要素にアクセスするビューを返します。
		/// @param y 行のインデックス
		/// @return 指定した行の要素にアクセスするビュー
		/// @remark ダングリング参照を防ぐため、右辺値オブジェクトからの呼び出しはコンパイルエラーになります。
		[[nodiscard]]
		constexpr std::span<value_type> row(size_type y) & noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 指定した行の要素にアクセスするビューを返します。
		/// @param y 行のインデックス
		/// @return 指定した行の要素にアクセスするビュー
		[[nodiscard]]
		constexpr std::span<const value_type> row(size_type y) const& noexcept SIV3D_LIFETIMEBOUND;

		void row(size_type) && = delete;

		void row(size_type) const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	rows
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各行の要素にアクセスするビューを行優先で列挙する range を返します。
		/// @return 各行の要素にアクセスする `std::span<value_type>` の range
		/// @remark 返された range はこの Grid を参照します。Grid の破棄またはサイズ変更後は使用できません。
		/// @remark ダングリング参照を防ぐため、右辺値オブジェクトからの呼び出しはコンパイルエラーになります。
		[[nodiscard]]
		constexpr auto rows() & noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 各行の要素にアクセスするビューを行優先で列挙する range を返します。
		/// @return 各行の要素にアクセスする `std::span<const value_type>` の range
		/// @remark 返された range はこの Grid を参照します。Grid の破棄またはサイズ変更後は使用できません。
		[[nodiscard]]
		constexpr auto rows() const& noexcept SIV3D_LIFETIMEBOUND;

		void rows() && = delete;

		void rows() const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	column
		//
		////////////////////////////////////////////////////////////////

	# if defined(__cpp_lib_ranges_stride)

		/// @brief 指定した列の要素にアクセスするビューを返します。
		/// @param x 列のインデックス
		/// @return 指定した列の要素にアクセスするビュー
		/// @remark ダングリング参照を防ぐため、右辺値オブジェクトからの呼び出しはコンパイルエラーになります。
		[[nodiscard]]
		constexpr auto column(size_type x) & noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 指定した列の要素にアクセスするビューを返します。
		/// @param x 列のインデックス
		/// @return 指定した列の要素にアクセスするビュー
		[[nodiscard]]
		constexpr auto column(size_type x) const& noexcept SIV3D_LIFETIMEBOUND;

		void column(size_type) && = delete;

		void column(size_type) const&& = delete;

		////////////////////////////////////////////////////////////////
		//
		//	columns
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各列の要素にアクセスするビューを列優先で列挙する range を返します。
		/// @return 各列の要素にアクセスする range の range
		/// @remark 返された range はこの Grid を参照します。Grid の破棄またはサイズ変更後は使用できません。
		/// @remark ダングリング参照を防ぐため、右辺値オブジェクトからの呼び出しはコンパイルエラーになります。
		[[nodiscard]]
		constexpr auto columns() & noexcept SIV3D_LIFETIMEBOUND;

		/// @brief 各列の要素にアクセスするビューを列優先で列挙する range を返します。
		/// @return 各列の要素にアクセスする range の range
		/// @remark 返された range はこの Grid を参照します。Grid の破棄またはサイズ変更後は使用できません。
		[[nodiscard]]
		constexpr auto columns() const& noexcept SIV3D_LIFETIMEBOUND;

		void columns() && = delete;

		void columns() const&& = delete;

	# endif

		////////////////////////////////////////////////////////////////
		//
		//	push_back_row
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾に行を追加します。
		/// @param value 追加した行の要素の初期値
		constexpr void push_back_row(const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	pop_back_row
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の行を削除します。
		/// @throw std::out_of_range 配列の高さ（行数）が 0 の場合
		constexpr void pop_back_row();

		////////////////////////////////////////////////////////////////
		//
		//	push_back_column
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾に列を追加します。
		/// @param value 追加した列の要素の初期値
		constexpr void push_back_column(const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	pop_back_column
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の列を削除します。
		/// @throw std::out_of_range 配列の幅（列数）が 0 の場合
		constexpr void pop_back_column();

		////////////////////////////////////////////////////////////////
		//
		//	insert_row
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した行に行を挿入します。
		/// @param y 挿入する行のインデックス
		/// @param value 挿入する行の要素の初期値
		/// @throw std::out_of_range 挿入する行のインデックスが範囲外の場合
		constexpr void insert_row(size_type y, const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	insert_rows
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した行に行を挿入します。
		/// @param y 挿入する行のインデックス
		/// @param n 挿入する行の数
		/// @param value 挿入する行の要素の初期値
		/// @throw std::out_of_range 挿入する行のインデックスが範囲外の場合
		constexpr void insert_rows(size_type y, size_type n, const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	insert_column
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した列に列を挿入します。
		/// @param x 挿入する列のインデックス
		/// @param value 挿入する列の要素の初期値
		/// @throw std::out_of_range 挿入する列のインデックスが範囲外の場合
		constexpr void insert_column(size_type x, const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	insert_columns
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した列に列を挿入します。
		/// @param x 挿入する列のインデックス
		/// @param n 挿入する列の数
		/// @param value 挿入する列の要素の初期値
		/// @throw std::out_of_range 挿入する列のインデックスが範囲外の場合
		constexpr void insert_columns(size_type x, size_type n, const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	remove_row
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した行を削除します。
		/// @param y 削除する行のインデックス
		/// @throw std::out_of_range 削除する行のインデックスが範囲外の場合
		constexpr void remove_row(size_type y);

		////////////////////////////////////////////////////////////////
		//
		//	remove_rows
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した行を削除します。
		/// @param y 削除を開始する行のインデックス
		/// @param count 削除する行の数
		/// @throw std::out_of_range 削除する対象が範囲外の場合
		constexpr void remove_rows(size_type y, size_type n);

		////////////////////////////////////////////////////////////////
		//
		//	remove_column
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した列を削除します。
		/// @param x 削除する列のインデックス
		/// @throw std::out_of_range 削除する列のインデックスが範囲外の場合
		constexpr void remove_column(size_type x);

		////////////////////////////////////////////////////////////////
		//
		//	remove_columns
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した列を削除します。
		/// @param x 削除を開始する列のインデックス
		/// @param count 削除する列の数
		/// @throw std::out_of_range 削除する対象が範囲外の場合
		constexpr void remove_columns(size_type x, size_type count);

		////////////////////////////////////////////////////////////////
		//
		//	resizeWidth
		//
		////////////////////////////////////////////////////////////////

		/// @brief 幅を変更します。
		/// @param w 新しい幅
		/// @remark 新しい幅が現在の幅より小さい場合、右端の列が削除されます。
		/// @remark 新しい幅が現在の幅より大きい場合、右端に追加される列の要素は `value_type{}` で初期化されます。
		constexpr void resizeWidth(size_type w);

		/// @brief 幅を変更します。
		/// @param w 新しい幅
		/// @param value 新しい列の要素の初期値
		/// @remark 新しい幅が現在の幅より小さい場合、右端の列が削除されます。
		/// @remark 新しい幅が現在の幅より大きい場合、右端に追加される列の要素は `value` で初期化されます。
		constexpr void resizeWidth(size_type w, const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	resizeHeight
		//
		////////////////////////////////////////////////////////////////

		/// @brief 高さを変更します。
		/// @param h 新しい高さ
		/// @remark 新しい高さが現在の高さより小さい場合、下端の行が削除されます。
		/// @remark 新しい高さが現在の高さより大きい場合、下端に追加される行の要素は `value_type{}` で初期化されます。
		constexpr void resizeHeight(size_type h);

		/// @brief 高さを変更します。
		/// @param h 新しい高さ
		/// @param value 新しい行の要素の初期値
		/// @remark 新しい高さが現在の高さより小さい場合、下端の行が削除されます。
		/// @remark 新しい高さが現在の高さより大きい場合、下端に追加される行の要素は `value` で初期化されます。
		constexpr void resizeHeight(size_type h, const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	resize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 幅と高さを変更します。
		/// @param w 新しい幅
		/// @param h 新しい高さ
		/// @remark 新しい幅や高さが現在のよりも小さい場合、右端の列、下端の行が削除されます。
		/// @remark 新しい幅や高さが現在のよりも大きい場合、右端に追加される列、下端に追加される行の要素は `value_type{}` で初期化されます。
		constexpr void resize(size_type w, size_type h);

		/// @brief 幅と高さを変更します。
		/// @param size 新しい幅と高さ
		/// @remark 新しい幅や高さが現在のよりも小さい場合、右端の列、下端の行が削除されます。
		/// @remark 新しい幅や高さが現在のよりも大きい場合、右端に追加される列、下端に追加される行の要素は `value_type{}` で初期化されます。
		constexpr void resize(Size size);

		/// @brief 幅と高さを変更します。
		/// @param w 新しい幅
		/// @param h 新しい高さ
		/// @param value 新しい列、行の要素の初期値
		/// @remark 新しい幅や高さが現在のよりも小さい場合、右端の列、下端の行が削除されます。
		/// @remark 新しい幅や高さが現在のよりも大きい場合、右端に追加される列、下端に追加される行の要素は `value` で初期化されます。
		constexpr void resize(size_type w, size_type h, const value_type& value);

		/// @brief 幅と高さを変更します。
		/// @param size 新しい幅と高さ
		/// @param value 新しい列、行の要素の初期値
		/// @remark 新しい幅や高さが現在のよりも小さい場合、右端の列、下端の行が削除されます。
		/// @remark 新しい幅や高さが現在のよりも大きい場合、右端に追加される列、下端に追加される行の要素は `value` で初期化されます。
		constexpr void resize(Size size, const value_type& value);

		////////////////////////////////////////////////////////////////
		//
		//	rotate90, rotated90
		//
		////////////////////////////////////////////////////////////////

		/// @brief 時計回りに 90° 回転します。
		/// @remark 幅と高さが入れ替わります。
		/// @remark `rotate_rows()` / `rotate_columns()` は行・列の巡回シフトであり、この幾何回転とは異なります。
		void rotate90();

		/// @brief 時計回りに 90° 回転した新しい二次元配列を返します。
		/// @return 新しい二次元配列
		/// @remark 幅と高さが入れ替わります。
		[[nodiscard]]
		Grid rotated90() const&;

		/// @brief 時計回りに 90° 回転した新しい二次元配列を返します。
		/// @return 新しい二次元配列
		/// @remark 幅と高さが入れ替わります。
		[[nodiscard]]
		Grid rotated90() &&;

		////////////////////////////////////////////////////////////////
		//
		//	rotate180, rotated180
		//
		////////////////////////////////////////////////////////////////

		/// @brief 180° 回転します。
		constexpr void rotate180() noexcept(std::is_nothrow_swappable_v<value_type>);

		/// @brief 180° 回転した新しい二次元配列を返します。
		/// @return 新しい二次元配列
		[[nodiscard]]
		constexpr Grid rotated180() const&;

		/// @brief 180° 回転した新しい二次元配列を返します。
		/// @return 新しい二次元配列
		[[nodiscard]]
		constexpr Grid rotated180() &&;

		////////////////////////////////////////////////////////////////
		//
		//	rotate270, rotated270
		//
		////////////////////////////////////////////////////////////////

		/// @brief 時計回りに 270° 回転します。
		/// @remark 幅と高さが入れ替わります。
		/// @remark 反時計回りの 90° 回転と同じです。
		/// @remark `rotate_rows()` / `rotate_columns()` は行・列の巡回シフトであり、この幾何回転とは異なります。
		void rotate270();

		/// @brief 時計回りに 270° 回転した新しい二次元配列を返します。
		/// @return 新しい二次元配列
		/// @remark 幅と高さが入れ替わります。
		[[nodiscard]]
		Grid rotated270() const&;

		/// @brief 時計回りに 270° 回転した新しい二次元配列を返します。
		/// @return 新しい二次元配列
		/// @remark 幅と高さが入れ替わります。
		[[nodiscard]]
		Grid rotated270() &&;

		////////////////////////////////////////////////////////////////
		//
		//	mirror, mirrored
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左右反転します。
		constexpr void mirror() noexcept(std::is_nothrow_swappable_v<value_type>);

		/// @brief 左右反転した新しい二次元配列を返します。
		/// @return 新しい二次元配列
		[[nodiscard]]
		constexpr Grid mirrored() const&;

		/// @brief 左右反転した新しい二次元配列を返します。
		/// @return 新しい二次元配列
		[[nodiscard]]
		constexpr Grid mirrored() &&;

		////////////////////////////////////////////////////////////////
		//
		//	flip, flipped
		//
		////////////////////////////////////////////////////////////////

		/// @brief 上下反転します。
		constexpr void flip() noexcept(std::is_nothrow_swappable_v<value_type>);

		/// @brief 上下反転した新しい二次元配列を返します。
		/// @return 新しい二次元配列
		[[nodiscard]]
		constexpr Grid flipped() const&;

		/// @brief 上下反転した新しい二次元配列を返します。
		/// @return 新しい二次元配列
		[[nodiscard]]
		constexpr Grid flipped() &&;

		////////////////////////////////////////////////////////////////
		//
		//	transpose, transposed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 転置します。
		/// @remark 幅と高さが入れ替わります。
		void transpose();

		/// @brief 転置した新しい二次元配列を返します。
		/// @return 新しい二次元配列
		/// @remark 幅と高さが入れ替わります。
		[[nodiscard]]
		Grid transposed() const&;

		/// @brief 転置した新しい二次元配列を返します。
		/// @return 新しい二次元配列
		/// @remark 幅と高さが入れ替わります。
		[[nodiscard]]
		Grid transposed() &&;

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
		/// @remark `for (auto& x : xs) f(x);` と同じです。
		template <class Fty>
		constexpr void each(Fty f)
			requires std::invocable<Fty&, value_type&>;

		/// @brief すべての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (const auto& x : xs) f(x);` と同じです。
		template <class Fty>
		constexpr void each(Fty f) const
			requires std::invocable<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	each_index
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素とその位置を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark 関数には `Point{ x, y }` と要素への参照を渡します。
		/// @remark `y = 0..height-1` の各行について `x = 0..width-1` の順（行優先）に呼び出します。
		template <class Fty>
		constexpr void each_index(Fty f)
			requires std::invocable<Fty&, Point, value_type&>;

		/// @brief すべての要素とその位置を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark 関数には `Point{ x, y }` と要素への const 参照を渡します。
		/// @remark `y = 0..height-1` の各行について `x = 0..width-1` の順（行優先）に呼び出します。
		template <class Fty>
		constexpr void each_index(Fty f) const
			requires std::invocable<Fty&, Point, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	each_neighbor4
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に隣接する 4 近傍の要素に関数を適用します。
		/// @tparam Fty 呼び出す関数の型
		/// @param pos 中心の位置
		/// @param f 呼び出す関数
		/// @pre `pos` が二次元配列の範囲内であること
		/// @remark 関数には近傍の `Point{ x, y }` と要素への参照を渡します。
		/// @remark 範囲外の近傍を除き、上、左、右、下の順に呼び出します。
		template <class Fty>
		constexpr void each_neighbor4(Point pos, Fty f)
			requires std::invocable<Fty&, Point, value_type&>;

		/// @brief 指定した位置に隣接する 4 近傍の要素に関数を適用します。
		/// @tparam Fty 呼び出す関数の型
		/// @param pos 中心の位置
		/// @param f 呼び出す関数
		/// @pre `pos` が二次元配列の範囲内であること
		/// @remark 関数には近傍の `Point{ x, y }` と要素への const 参照を渡します。
		/// @remark 範囲外の近傍を除き、上、左、右、下の順に呼び出します。
		template <class Fty>
		constexpr void each_neighbor4(Point pos, Fty f) const
			requires std::invocable<Fty&, Point, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	each_neighbor8
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に隣接する 8 近傍の要素に関数を適用します。
		/// @tparam Fty 呼び出す関数の型
		/// @param pos 中心の位置
		/// @param f 呼び出す関数
		/// @pre `pos` が二次元配列の範囲内であること
		/// @remark 関数には近傍の `Point{ x, y }` と要素への参照を渡します。
		/// @remark 範囲外の近傍を除き、左上、上、右上、左、右、左下、下、右下の順に呼び出します。
		template <class Fty>
		constexpr void each_neighbor8(Point pos, Fty f)
			requires std::invocable<Fty&, Point, value_type&>;

		/// @brief 指定した位置に隣接する 8 近傍の要素に関数を適用します。
		/// @tparam Fty 呼び出す関数の型
		/// @param pos 中心の位置
		/// @param f 呼び出す関数
		/// @pre `pos` が二次元配列の範囲内であること
		/// @remark 関数には近傍の `Point{ x, y }` と要素への const 参照を渡します。
		/// @remark 範囲外の近傍を除き、左上、上、右上、左、右、左下、下、右下の順に呼び出します。
		template <class Fty>
		constexpr void each_neighbor8(Point pos, Fty f) const
			requires std::invocable<Fty&, Point, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	count_neighbors4, count_neighbors8
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の 4 近傍にある、指定した値と等しい要素の個数を返します。
		/// @param pos 中心の位置
		/// @param value 検索する値
		/// @return 指定した値と等しい近傍要素の個数
		/// @pre `pos` が二次元配列の範囲内であること
		[[nodiscard]]
		constexpr isize count_neighbors4(Point pos, const value_type& value) const;

		/// @brief 指定した位置の 8 近傍にある、指定した値と等しい要素の個数を返します。
		/// @param pos 中心の位置
		/// @param value 検索する値
		/// @return 指定した値と等しい近傍要素の個数
		/// @pre `pos` が二次元配列の範囲内であること
		[[nodiscard]]
		constexpr isize count_neighbors8(Point pos, const value_type& value) const;

		////////////////////////////////////////////////////////////////
		//
		//	count_neighbors4_if, count_neighbors8_if
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の 4 近傍にある、条件を満たす要素の個数を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param pos 中心の位置
		/// @param f 条件を記述した関数
		/// @return 条件を満たす近傍要素の個数
		/// @pre `pos` が二次元配列の範囲内であること
		template <class Fty>
		[[nodiscard]]
		constexpr isize count_neighbors4_if(Point pos, Fty f) const
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した位置の 8 近傍にある、条件を満たす要素の個数を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param pos 中心の位置
		/// @param f 条件を記述した関数
		/// @return 条件を満たす近傍要素の個数
		/// @pre `pos` が二次元配列の範囲内であること
		template <class Fty>
		[[nodiscard]]
		constexpr isize count_neighbors8_if(Point pos, Fty f) const
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	fetch
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにある要素を返します。インデックスが範囲外の場合デフォルト値を返します。
		/// @tparam U デフォルト値の型
		/// @param y Y インデックス
		/// @param x X インデックス
		/// @param defaultValue インデックスが範囲外の場合に返すデフォルト値
		/// @return 指定したインデックスにある要素。範囲外の場合は defaultValue
		template <class U>
		[[nodiscard]]
		constexpr value_type fetch(size_type y, size_type x, U&& defaultValue) const
			noexcept(std::is_nothrow_constructible_v<value_type, U> && std::is_nothrow_copy_constructible_v<value_type>)
			requires std::constructible_from<value_type, U>;

		/// @brief 指定した位置にある要素を返します。位置が範囲外の場合デフォルト値を返します。
		/// @tparam U デフォルト値の型
		/// @param pos インデックス
		/// @param defaultValue インデックスが範囲外の場合に返すデフォルト値
		/// @return 指定したインデックスにある要素。範囲外の場合は defaultValue
		template <class U>
		[[nodiscard]]
		constexpr value_type fetch(Point pos, U&& defaultValue) const
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
		constexpr Grid& fill(const value_type& value) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した領域と重なる要素に値を代入します。
		/// @param pos 領域の左上の位置
		/// @param size 領域の幅と高さ
		/// @param value 代入する値
		/// @return *this
		/// @throw std::invalid_argument 領域の幅または高さが負の場合
		/// @remark 領域はこの二次元配列の範囲にクリップされます。重なる領域が無い場合は何もしません。
		constexpr Grid& fill(Point pos, Size size, const value_type& value) SIV3D_LIFETIMEBOUND;

		/// @brief 指定した長方形と重なる要素に値を代入します。
		/// @param rect 領域を表す長方形
		/// @param value 代入する値
		/// @return *this
		/// @throw std::invalid_argument 長方形の幅または高さが負の場合
		/// @remark 長方形はこの二次元配列の範囲にクリップされます。重なる領域が無い場合は何もしません。
		/// @remark この関数を使用するには `<Siv3D/GridRect.hpp>` をインクルードしてください。
		constexpr Grid& fill(Rect rect, const value_type& value) SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	floodFill
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置につながる同じ値の要素を塗りつぶします。
		/// @param pos 塗りつぶしを開始する位置
		/// @param newValue 塗りつぶし後の値
		/// @param connectivity セルの連結方法
		/// @return 値を変更した要素の個数。`pos` が範囲外、または開始位置の値が `newValue` と等しい場合は 0
		constexpr isize floodFill(Point pos, const value_type& newValue,
			GridConnectivity connectivity = GridConnectivity::Four);

		////////////////////////////////////////////////////////////////
		//
		//	isSorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素が昇順にソートされているかを返します。
		/// @return 配列の要素が昇順にソートされている場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isSorted() const
			requires Concept::LessThanComparable<value_type>;

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
		//	map_indexed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各要素とその位置に関数を適用した戻り値からなる新しい二次元配列を返します。
		/// @tparam Fty 各要素に適用する関数の型
		/// @param f 各要素に適用する関数
		/// @return 各要素とその位置に関数を適用した戻り値からなる新しい二次元配列
		/// @remark 関数には `Point{ x, y }` と要素への const 参照を渡します。
		/// @remark `y = 0..height-1` の各行について `x = 0..width-1` の順（行優先）に適用します。
		template <class Fty>
		[[nodiscard]]
		constexpr auto map_indexed(Fty f) const
			requires std::invocable<Fty&, Point, const value_type&>;

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
		//	paste
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に別の二次元配列を貼り付けます。
		/// @param pos 貼り付け先の左上の位置
		/// @param source 貼り付ける二次元配列
		/// @return *this
		/// @remark 貼り付ける領域はこの二次元配列の範囲にクリップされます。重なる領域が無い場合は何もしません。
		/// @remark `source` がこの二次元配列自身である場合は何もしません。
		constexpr Grid& paste(Point pos, const Grid& source) SIV3D_LIFETIMEBOUND;

		////////////////////////////////////////////////////////////////
		//
		//	replace, replaced
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値と等しいすべての要素を別の値に置き換えます。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return *this
		constexpr Grid& replace(const value_type& oldValue, const value_type& newValue)& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した値と等しいすべての要素を別の値に置き換えた新しい配列を返します。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid replace(const value_type& oldValue, const value_type& newValue)&&;

		/// @brief 指定した値と等しいすべての要素を別の値に置き換えた新しい配列を返します。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid replaced(const value_type& oldValue, const value_type& newValue) const&;

		/// @brief 指定した値と等しいすべての要素を別の値に置き換えた新しい配列を返します。
		/// @param oldValue 置き換えられる値
		/// @param newValue 新しい値
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid replaced(const value_type& oldValue, const value_type& newValue)&&;

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
		constexpr Grid& replace_if(Fty f, const value_type& newValue)& SIV3D_LIFETIMEBOUND
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid replace_if(Fty f, const value_type& newValue) &&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid replaced_if(Fty f, const value_type& newValue) const&
			requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid replaced_if(Fty f, const value_type& newValue) &&
			requires std::predicate<Fty&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	reverse, reversed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素を逆順に並び替えます。
		/// @return *this
		constexpr Grid& reverse()& SIV3D_LIFETIMEBOUND;

		/// @brief 配列の要素を逆順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid reverse()&&;

		/// @brief 配列の要素を逆順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid reversed() const&;

		/// @brief 配列の要素を逆順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid reversed()&&;

		////////////////////////////////////////////////////////////////
		//
		//	rotate_columns, rotated_columns
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を境に前半の列と後半の列を入れ替えます。
		/// @param middle 境の位置
		/// @return *this
		constexpr Grid& rotate_columns(size_type middle)& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した位置を境に前半の列と後半の列を入れ替えた新しい配列を返します。
		/// @param middle 境の位置
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid rotate_columns(size_type middle)&&;

		/// @brief 指定した位置を境に前半の列と後半の列を入れ替えた新しい配列を返します。
		/// @param middle 境の位置
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid rotated_columns(size_type middle) const&;

		/// @brief 指定した位置を境に前半の列と後半の列を入れ替えた新しい配列を返します。
		/// @param middle 境の位置
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid rotated_columns(size_type middle)&&;

		////////////////////////////////////////////////////////////////
		//
		//	rotate_rows, rotated_rows
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を境に前半の行と後半の行を入れ替えます。
		/// @param middle 境の位置
		/// @return *this
		constexpr Grid& rotate_rows(size_type middle)& SIV3D_LIFETIMEBOUND;
		
		/// @brief 指定した位置を境に前半の行と後半の行を入れ替えた新しい配列を返します。
		/// @param middle 境の位置
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid rotate_rows(size_type middle)&&;

		/// @brief 指定した位置を境に前半の行と後半の行を入れ替えた新しい配列を返します。
		/// @param middle 境の位置
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid rotated_rows(size_type middle) const&;

		/// @brief 指定した位置を境に前半の行と後半の行を入れ替えた新しい配列を返します。
		/// @param middle 境の位置
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid rotated_rows(size_type middle)&&;

		////////////////////////////////////////////////////////////////
		//
		//	rsort, rsorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を降順に並び替えます。
		/// @return *this
		constexpr Grid& rsort()& SIV3D_LIFETIMEBOUND
			requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を降順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid rsort() &&
			requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を降順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid rsorted() const&
			requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を降順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid rsorted() &&
			requires Concept::LessThanComparable<value_type>;

		////////////////////////////////////////////////////////////////
		//
		//	scaled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各要素を縦横に指定した倍率で拡大した新しい二次元配列を返します。
		/// @param n 拡大倍率
		/// @return 拡大された新しい二次元配列
		/// @throw std::invalid_argument `n <= 0` の場合
		/// @throw std::length_error 拡大後の幅、高さ、または要素数を表現できない場合
		[[nodiscard]]
		constexpr Grid scaled(int32 n) const&;

		/// @brief 各要素を縦横に指定した倍率で拡大した新しい二次元配列を返します。
		/// @param n 拡大倍率
		/// @return 拡大された新しい二次元配列
		/// @throw std::invalid_argument `n <= 0` の場合
		/// @throw std::length_error 拡大後の幅、高さ、または要素数を表現できない場合
		[[nodiscard]]
		constexpr Grid scaled(int32 n)&&;

		////////////////////////////////////////////////////////////////
		//
		//	shift, shifted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を指定した量だけ移動し、空いた領域を指定した値で埋めます。
		/// @param dx X 方向の移動量。正の場合は右、負の場合は左に移動します。
		/// @param dy Y 方向の移動量。正の場合は下、負の場合は上に移動します。
		/// @param fillValue 空いた領域を埋める値
		/// @return *this
		/// @remark 範囲外に移動した要素は破棄されます。
		constexpr Grid& shift(int32 dx, int32 dy, const value_type& fillValue)& SIV3D_LIFETIMEBOUND;

		/// @brief 要素を指定した量だけ移動し、空いた領域を指定した値で埋めます。
		/// @param dx X 方向の移動量。正の場合は右、負の場合は左に移動します。
		/// @param dy Y 方向の移動量。正の場合は下、負の場合は上に移動します。
		/// @param fillValue 空いた領域を埋める値
		/// @return 移動後の二次元配列
		/// @remark 範囲外に移動した要素は破棄されます。
		[[nodiscard]]
		constexpr Grid shift(int32 dx, int32 dy, const value_type& fillValue)&&;

		/// @brief 要素を指定した量だけ移動した新しい二次元配列を返します。
		/// @param dx X 方向の移動量。正の場合は右、負の場合は左に移動します。
		/// @param dy Y 方向の移動量。正の場合は下、負の場合は上に移動します。
		/// @param fillValue 空いた領域を埋める値
		/// @return 移動後の新しい二次元配列
		/// @remark 範囲外に移動した要素は破棄されます。
		[[nodiscard]]
		constexpr Grid shifted(int32 dx, int32 dy, const value_type& fillValue) const&;

		/// @brief 要素を指定した量だけ移動した新しい二次元配列を返します。
		/// @param dx X 方向の移動量。正の場合は右、負の場合は左に移動します。
		/// @param dy Y 方向の移動量。正の場合は下、負の場合は上に移動します。
		/// @param fillValue 空いた領域を埋める値
		/// @return 移動後の新しい二次元配列
		/// @remark 範囲外に移動した要素は破棄されます。
		[[nodiscard]]
		constexpr Grid shifted(int32 dx, int32 dy, const value_type& fillValue)&&;

		////////////////////////////////////////////////////////////////
		//
		//	shuffle, shuffled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素の並び順をランダムにシャッフルします。
		/// @return *this
		constexpr Grid& shuffle()& SIV3D_LIFETIMEBOUND;

		/// @brief 配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid shuffle()&&;

		/// @brief 配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid shuffled() const&;

		/// @brief 配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid shuffled()&&;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルします。
		/// @param urbg 使用する乱数エンジン
		/// @return *this
		constexpr Grid& shuffle(Concept::UniformRandomBitGenerator auto&& urbg)& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @param urbg 使用する乱数エンジン
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid shuffle(Concept::UniformRandomBitGenerator auto&& urbg)&&;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @param urbg 使用する乱数エンジン
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid shuffled(Concept::UniformRandomBitGenerator auto&& urbg) const&;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @param urbg 使用する乱数エンジン
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid shuffled(Concept::UniformRandomBitGenerator auto&& urbg)&&;

		////////////////////////////////////////////////////////////////
		//
		//	sort, sorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を昇順に並び替えます。
		/// @return *this
		constexpr Grid& sort()& SIV3D_LIFETIMEBOUND
			requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid sort() &&
			requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid sorted() const&
			requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid sorted() &&
			requires Concept::LessThanComparable<value_type>;

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
		constexpr Grid& sort_by(Fty f)& SIV3D_LIFETIMEBOUND
			requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid sort_by(Fty f) &&
			requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid sorted_by(Fty f) const&
			requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid sorted_by(Fty f) &&
			requires std::predicate<Fty&, const value_type&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	stable_sort, stable_sorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を相対順序を保ちながら昇順に並び替えます。
		/// @return *this
		constexpr Grid& stable_sort()& SIV3D_LIFETIMEBOUND
			requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid stable_sort() &&
			requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid stable_sorted() const&
			requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid stable_sorted() &&
			requires Concept::LessThanComparable<value_type>;

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
		constexpr Grid& stable_sort_by(Fty f)& SIV3D_LIFETIMEBOUND
			requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid stable_sort_by(Fty f) &&
			requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid stable_sorted_by(Fty f) const&
			requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid stable_sorted_by(Fty f) &&
			requires std::predicate<Fty&, const value_type&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	subgrid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した領域の要素からなる新しい二次元配列を返します。
		/// @param pos 領域の左上の位置
		/// @param size 領域の幅と高さ
		/// @return 指定した領域の要素からなる新しい二次元配列
		/// @throw std::invalid_argument 領域の幅または高さが負の場合
		/// @throw std::out_of_range 指定した領域全体がこの二次元配列に収まらない場合
		[[nodiscard]]
		constexpr Grid subgrid(Point pos, Size size) const;

		/// @brief 指定した領域の要素からなる新しい二次元配列を返します。
		/// @param x 領域の左上の X 座標
		/// @param y 領域の左上の Y 座標
		/// @param w 領域の幅
		/// @param h 領域の高さ
		/// @return 指定した領域の要素からなる新しい二次元配列
		/// @throw std::invalid_argument 領域の幅または高さが負の場合
		/// @throw std::out_of_range 指定した領域全体がこの二次元配列に収まらない場合
		[[nodiscard]]
		constexpr Grid subgrid(int32 x, int32 y, int32 w, int32 h) const;

		/// @brief 指定した長方形の要素からなる新しい二次元配列を返します。
		/// @param rect 領域を表す長方形
		/// @return 指定した長方形の要素からなる新しい二次元配列
		/// @throw std::invalid_argument 長方形の幅または高さが負の場合
		/// @throw std::out_of_range 指定した長方形全体がこの二次元配列に収まらない場合
		/// @remark この関数を使用するには `<Siv3D/GridRect.hpp>` をインクルードしてください。
		[[nodiscard]]
		constexpr Grid subgrid(Rect rect) const;

		////////////////////////////////////////////////////////////////
		//
		//	sum
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を `+` 演算子を用いて合計します。
		/// @return 合計値
		[[nodiscard]]
		constexpr auto sum() const
			requires (Concept::Addable<value_type> || Concept::AddAssignable<value_type>);

		////////////////////////////////////////////////////////////////
		//
		//	sumF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 浮動小数点数型の要素を、誤差が小さくなるように合計します。
		/// @remark `sum()` よりも浮動小数点数誤差が小さくなります。
		/// @return 合計値
		[[nodiscard]]
		constexpr auto sumF() const
			requires Concept::FloatingPoint<value_type>;

		////////////////////////////////////////////////////////////////
		//
		//	swap_columns
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した 2 つの列を交換します。
		/// @param a 交換する列のインデックス
		/// @param b 交換する列のインデックス
		/// @return *this
		constexpr Grid& swap_columns(size_type a, size_type b);

		////////////////////////////////////////////////////////////////
		//
		//	swap_rows
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した 2 つの行を交換します。
		/// @param a 交換する行のインデックス
		/// @param b 交換する行のインデックス
		/// @return *this
		constexpr Grid& swap_rows(size_type a, size_type b);

		////////////////////////////////////////////////////////////////
		//
		//	values_at
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief 指定した複数の位置にある要素を配列で取得します。
		/// @param indices 取得する位置のリスト
		/// @return 指定した位置にある要素の配列
		/// @throw std::out_of_range 範囲外の位置が含まれている場合
		[[nodiscard]]
		constexpr Array<Type> values_at(std::initializer_list<Point> indices) const;
		
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
		isize parallel_count_if(Fty f) const
			requires std::predicate<Fty&, const value_type&>
				&& detail::GridHasParallelCountIf<const container_type, Fty>;
		
		////////////////////////////////////////////////////////////////
		//
		//	parallel_each
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素に対して関数を並列実行します。
		/// @tparam Fty 関数の型
		/// @param f 関数
		template <class Fty>
		void parallel_each(Fty f)
			requires std::invocable<Fty&, value_type&>
				&& detail::GridHasParallelEach<container_type, Fty>;

		/// @brief すべての要素に対して関数を並列実行します。
		/// @tparam Fty 関数の型
		/// @param f 関数
		template <class Fty>
		void parallel_each(Fty f) const
			requires std::invocable<Fty&, const value_type&>
				&& detail::GridHasParallelEach<const container_type, Fty>;

		////////////////////////////////////////////////////////////////
		//
		//	parallel_map
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素に対して関数を適用した結果からなる新しい配列を返します（並列実行）。
		/// @tparam Fty 関数の型
		/// @param f 関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		auto parallel_map(Fty f) const
			requires std::invocable<Fty&, const value_type&>
				&& detail::GridHasParallelMap<const container_type, Fty>;

		////////////////////////////////////////////////////////////////
		//
		//	Generate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 関数を用いて二次元配列を生成します。
		/// @param size 生成する二次元配列の幅と高さ
		/// @param generator 生成に使用する関数
		/// @return 生成した二次元配列
		[[nodiscard]]
		static constexpr Grid Generate(Size size, FunctionRef<value_type()> generator);

		////////////////////////////////////////////////////////////////
		//
		//	IndexedGenerate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 座標と関数を用いて二次元配列を生成します。
		/// @param size 生成する二次元配列の幅と高さ
		/// @param generator 生成に使用する関数。列、行の順で座標 `(x, y)` を受け取ります。
		/// @return 生成した二次元配列
		[[nodiscard]]
		static constexpr Grid IndexedGenerate(Size size, FunctionRef<value_type(int32, int32)> generator);

		/// @brief 座標と関数を用いて二次元配列を生成します。
		/// @param size 生成する二次元配列の幅と高さ
		/// @param generator 生成に使用する関数。座標 `Point{ x, y }` を受け取ります。
		/// @return 生成した二次元配列
		[[nodiscard]]
		static constexpr Grid IndexedGenerate(Size size, FunctionRef<value_type(Point)> generator);

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

		/// @brief 二次元配列を出力ストリームに出力します。
		/// @tparam CharType 文字型
		/// @param output 出力ストリーム
		/// @param value 出力する二次元配列
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Grid& value)
		{
			return output << Format(value);
		}

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
		friend constexpr bool operator ==(const Grid& lhs, const Grid& rhs)
		{
			return ((lhs.m_size == rhs.m_size)
				 && (lhs.m_container == rhs.m_container));
		}

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの配列を入れ替えます。
		/// @param lhs 一方の配列
		/// @param rhs もう一方の配列
		friend constexpr void swap(Grid& lhs, Grid& rhs) noexcept(noexcept(lhs.swap(rhs)))
		{
			lhs.swap(rhs);
		}

	private:

		Size m_size{ 0, 0 };

		container_type m_container;
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator = std::allocator<Type>>
	void Formatter(FormatData& formatData, const Grid<Type, Allocator>& v);

	////////////////////////////////////////////////////////////////
	//
	//	(deduction guide)
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Grid(size_t, size_t, Type) -> Grid<Type>;

	template <class Type>
	Grid(Size, Type) -> Grid<Type>;

	template <class Type>
	Grid(std::initializer_list<std::initializer_list<Type>>) -> Grid<Type>;
	
	template <class Type>
	Grid(size_t, size_t, Array<Type>) -> Grid<Type>;

	template <class Type>
	Grid(Size, Array<Type>) -> Grid<Type>;
}

# include "detail/Grid.ipp"
