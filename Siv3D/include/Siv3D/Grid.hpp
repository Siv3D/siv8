//-----------------------------------------------
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
# include <ranges>
# include "Common.hpp"
# include "Array.hpp"
# include "PointVector.hpp"
# include "Random.hpp"

namespace s3d
{
	template <class Type, class Allocator>
	class Grid;

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

		[[nodiscard]]
		Grid() = default;

		[[nodiscard]]
		Grid(const Grid&) = default;

		[[nodiscard]]
		Grid(Grid&&) = default;

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

		[[nodiscard]]
		constexpr Grid(const std::initializer_list<std::initializer_list<value_type>>& set);

		[[nodiscard]]
		constexpr Grid(size_type w, size_type h, Arg::generator_<FunctionRef<value_type()>> generator);

		[[nodiscard]]
		constexpr Grid(size_type w, size_type h, Arg::generator_<FunctionRef<value_type(int32, int32)>> generator);

		[[nodiscard]]
		constexpr Grid(size_type w, size_type h, Arg::generator_<FunctionRef<value_type(Point)>> generator);

		[[nodiscard]]
		constexpr Grid(Size size, Arg::generator_<FunctionRef<value_type()>> generator);

		[[nodiscard]]
		constexpr Grid(Size size, Arg::generator_<FunctionRef<value_type(int32, int32)>> generator);

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

		constexpr Grid& assign(size_type w, size_type h, const value_type& value) SIV3D_LIFETIMEBOUND;

		constexpr Grid& assign(Size size, const value_type& value) SIV3D_LIFETIMEBOUND;

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

		constexpr reference at(size_type y, size_type x)& SIV3D_LIFETIMEBOUND;

		constexpr const_reference at(size_type y, size_type x) const& SIV3D_LIFETIMEBOUND;

		constexpr value_type at(size_type y, size_type x)&&;

		constexpr reference at(Point pos)& SIV3D_LIFETIMEBOUND;

		constexpr const_reference at(Point pos) const& SIV3D_LIFETIMEBOUND;

		constexpr value_type at(Point pos)&&;

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr pointer operator [](size_type y) SIV3D_LIFETIMEBOUND;

		[[nodiscard]]
		constexpr const_pointer operator [](size_type y) const SIV3D_LIFETIMEBOUND;

		[[nodiscard]]
		constexpr reference operator [](Point pos)& SIV3D_LIFETIMEBOUND;

		[[nodiscard]]
		constexpr const_reference operator [](Point pos) const& SIV3D_LIFETIMEBOUND;

		[[nodiscard]]
		constexpr value_type operator [](Point pos)&&;

	# ifdef __cpp_multidimensional_subscript

		[[nodiscard]]
		constexpr reference operator [](size_type y, size_type x)& SIV3D_LIFETIMEBOUND;

		[[nodiscard]]
		constexpr const_reference operator [](size_type y, size_type x) const& SIV3D_LIFETIMEBOUND;

		[[nodiscard]]
		constexpr value_type operator [](size_type y, size_type x)&&;

	# endif

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
		constexpr value_type front() && noexcept;

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
		constexpr value_type back() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	indexInBounds
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr bool indexInBounds(size_type y, size_type x) const noexcept;

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
		constexpr uint32 bytesPerRow() const noexcept requires (Concept::TriviallyCopyable<value_type>);

		////////////////////////////////////////////////////////////////
		//
		//	size_bytes
		//
		////////////////////////////////////////////////////////////////

		/// @brief 二次元配列の要素の合計サイズ（バイト）を返します。
		/// @return 二次元配列の要素の合計サイズ（バイト）
		[[nodiscard]]
		constexpr size_t size_bytes() const noexcept requires (Concept::TriviallyCopyable<value_type>);

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
		constexpr void swap(Grid& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	row
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した行の要素にアクセスするビューを返します。
		/// @param y 行のインデックス
		/// @return 指定した行の要素にアクセスするビュー
		[[nodiscard]]
		constexpr auto row(size_type y) noexcept;

		/// @brief 指定した行の要素にアクセスするビューを返します。
		/// @param y 行のインデックス
		/// @return 指定した行の要素にアクセスするビュー
		[[nodiscard]]
		constexpr auto row(size_type y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	column
		//
		////////////////////////////////////////////////////////////////

	# if defined(__cpp_lib_ranges_stride)

		/// @brief 指定した列の要素にアクセスするビューを返します。
		/// @param x 列のインデックス
		/// @return 指定した列の要素にアクセスするビュー
		[[nodiscard]]
		constexpr auto column(size_type x) noexcept;

		/// @brief 指定した列の要素にアクセスするビューを返します。
		/// @param x 列のインデックス
		/// @return 指定した列の要素にアクセスするビュー
		[[nodiscard]]
		constexpr auto column(size_type x) const noexcept;

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
		//	rotate90
		//
		////////////////////////////////////////////////////////////////

		/// @brief 90° 回転します。
		/// @remark 幅と高さが入れ替わります。
		void rotate90();

		////////////////////////////////////////////////////////////////
		//
		//	rotate180
		//
		////////////////////////////////////////////////////////////////

		/// @brief 180° 回転します。
		constexpr void rotate180() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotate270
		//
		////////////////////////////////////////////////////////////////

		/// @brief 270° 回転します。
		/// @remark 幅と高さが入れ替わります。
		void rotate270();

		////////////////////////////////////////////////////////////////
		//
		//	mirror
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左右反転します。
		constexpr void mirror() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	flip
		//
		////////////////////////////////////////////////////////////////

		/// @brief 上下反転します。
		constexpr void flip() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transpose
		//
		////////////////////////////////////////////////////////////////

		/// @brief 転置します。
		void transpose();

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

		/// @brief すべての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (auto& x : xs) f(x);` と同じです。
		template <class Fty>
		constexpr void each(Fty f) requires std::invocable<Fty&, value_type&>;

		/// @brief すべての要素を順番に引数にして関数を呼び出します。
		/// @tparam Fty 呼び出す関数の型
		/// @param f 呼び出す関数
		/// @remark `for (const auto& x : xs) f(x);` と同じです。
		template <class Fty>
		constexpr void each(Fty f) const requires std::invocable<Fty&, const value_type&>;

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
		constexpr value_type fetch(size_type y, size_type x, U&& defaultValue) const noexcept(std::is_nothrow_constructible_v<value_type, U>) requires std::constructible_from<value_type, U>;

		/// @brief 指定した位置にある要素を返します。位置が範囲外の場合デフォルト値を返します。
		/// @tparam U デフォルト値の型
		/// @param pos インデックス
		/// @param defaultValue インデックスが範囲外の場合に返すデフォルト値
		/// @return 指定したインデックスにある要素。範囲外の場合は defaultValue
		template <class U>
		[[nodiscard]]
		constexpr value_type fetch(Point pos, U&& defaultValue) const noexcept(std::is_nothrow_constructible_v<value_type, U>) requires std::constructible_from<value_type, U>;

		////////////////////////////////////////////////////////////////
		//
		//	fill
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した値をすべての要素に代入します。
		/// @param value 代入する値
		/// @return *this
		constexpr Grid& fill(const value_type& value) SIV3D_LIFETIMEBOUND;

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
		constexpr Grid& replace_if(Fty f, const value_type& newValue)& SIV3D_LIFETIMEBOUND requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid replace_if(Fty f, const value_type& newValue) && requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid replaced_if(Fty f, const value_type& newValue) const& requires std::predicate<Fty&, const value_type&>;

		/// @brief 指定した条件を満たすすべての要素を別の値に置き換えた新しい配列を返します。
		/// @tparam Fty 条件を記述した関数の型
		/// @param f 条件
		/// @param newValue 新しい値
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid replaced_if(Fty f, const value_type& newValue) && requires std::predicate<Fty&, const value_type&>;

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
		constexpr Grid& rsort()& SIV3D_LIFETIMEBOUND requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を降順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid rsort() && requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を降順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid rsorted() const& requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を降順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid rsorted() && requires Concept::LessThanComparable<value_type>;

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
		/// @param rbg 使用する乱数エンジン
		/// @return *this
		constexpr Grid& shuffle(Concept::UniformRandomBitGenerator auto&& rbg)& SIV3D_LIFETIMEBOUND;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid shuffle(Concept::UniformRandomBitGenerator auto&& rbg)&&;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid shuffled(Concept::UniformRandomBitGenerator auto&& rbg) const&;

		/// @brief 指定した乱数エンジンを用いて、配列の要素の並び順をランダムにシャッフルした新しい配列を返します。
		/// @param rbg 使用する乱数エンジン
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid shuffled(Concept::UniformRandomBitGenerator auto&& rbg)&&;

		////////////////////////////////////////////////////////////////
		//
		//	sort, sorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を昇順に並び替えます。
		/// @return *this
		constexpr Grid& sort()& SIV3D_LIFETIMEBOUND requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid sort() && requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid sorted() const& requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid sorted() && requires Concept::LessThanComparable<value_type>;

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
		constexpr Grid& sort_by(Fty f)& SIV3D_LIFETIMEBOUND requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid sort_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid sorted_by(Fty f) const& requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid sorted_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>;

		////////////////////////////////////////////////////////////////
		//
		//	stable_sort, stable_sorted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素を相対順序を保ちながら昇順に並び替えます。
		/// @return *this
		constexpr Grid& stable_sort()& SIV3D_LIFETIMEBOUND requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid stable_sort() && requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid stable_sorted() const& requires Concept::LessThanComparable<value_type>;

		/// @brief 要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @return 新しい配列
		[[nodiscard]]
		constexpr Grid stable_sorted() && requires Concept::LessThanComparable<value_type>;

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
		constexpr Grid& stable_sort_by(Fty f)& SIV3D_LIFETIMEBOUND requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid stable_sort_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid stable_sorted_by(Fty f) const& requires std::predicate<Fty&, const value_type&, const value_type&>;

		/// @brief 指定した関数を用いて要素を相対順序を保ちながら昇順に並び替えた新しい配列を返します。
		/// @tparam Fty 比較に使用する関数の型
		/// @param f 比較に使用する関数
		/// @return 新しい配列
		template <class Fty>
		[[nodiscard]]
		constexpr Grid stable_sorted_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>;

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
		//	swap_columns
		//
		////////////////////////////////////////////////////////////////

		constexpr Grid& swap_columns(size_type a, size_type b);

		////////////////////////////////////////////////////////////////
		//
		//	swap_rows
		//
		////////////////////////////////////////////////////////////////

		constexpr Grid& swap_rows(size_type a, size_type b);

		////////////////////////////////////////////////////////////////
		//
		//	values_at
		//
		////////////////////////////////////////////////////////////////
		
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
		isize parallel_count_if(Fty f) const requires std::predicate<Fty&, const value_type&>;
		
		////////////////////////////////////////////////////////////////
		//
		//	parallel_each
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素に対して関数を並列実行します。
		/// @tparam Fty 関数の型
		/// @param f 関数
		template <class Fty>
		void parallel_each(Fty f) requires std::invocable<Fty&, value_type&>;

		/// @brief すべての要素に対して関数を並列実行します。
		/// @tparam Fty 関数の型
		/// @param f 関数
		template <class Fty>
		void parallel_each(Fty f) const requires std::invocable<Fty&, const value_type&>;

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
		friend constexpr void swap(Grid& lhs, Grid& rhs) noexcept
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
