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
# include "Byte.hpp"
# include "IReader.hpp"

namespace s3d
{
	class MD5Value;
	class Base64Value;

	////////////////////////////////////////////////////////////////
	//
	//	Blob
	//
	////////////////////////////////////////////////////////////////

	/// @brief メモリ上のバイナリデータ
	class Blob
	{
	public:

		using base_type					= Array<Byte>;
		using iterator					= base_type::iterator;
		using const_iterator			= base_type::const_iterator;
		using reverse_iterator			= base_type::reverse_iterator;
		using const_reverse_iterator	= base_type::const_reverse_iterator;
		using size_type					= base_type::size_type;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空のバイナリデータを作成します。
		[[nodiscard]]
		Blob() = default;

		/// @brief コピーコンストラクタ
		[[nodiscard]]
		Blob(const Blob&) = default;

		/// @brief ムーブコンストラクタ
		[[nodiscard]]
		Blob(Blob&&) noexcept = default;

		/// @brief 指定したサイズのバイナリデータを作成します。
		/// @param sizeBytes バイナリデータのサイズ（バイト）
		[[nodiscard]]
		explicit constexpr Blob(size_type sizeBytes);

		/// @brief 空のバイナリデータを作成します。
		/// @param reserveSizeBytes reserve するサイズ（バイト）
		[[nodiscard]]
		explicit constexpr Blob(Arg::reserve_<size_type> reserveSizeBytes);

		/// @brief ファイルの中身をコピーしたバイナリデータを作成します。
		/// @param path ファイルパス
		[[nodiscard]]
		explicit Blob(FilePathView path);

		/// @brief IReader からデータを読み込んでバイナリデータを作成します。
		/// @param reader IReader
		[[nodiscard]]
		explicit Blob(IReader& reader);

		/// @brief メモリ上のデータをコピーして作成します。
		/// @param src コピーするデータの先頭ポインタ
		/// @param sizeBytes コピーするデータのサイズ（バイト）
		[[nodiscard]]
		Blob(const void* src, size_type sizeBytes);

		/// @brief バイナリデータをコピーして作成します。
		/// @param data コピーするデータ
		[[nodiscard]]
		explicit Blob(std::span<const Byte> data);

		/// @brief バイナリデータをコピーして作成します。
		/// @param data コピーするデータ
		[[nodiscard]]
		explicit constexpr Blob(const Array<Byte>& data);

		/// @brief バイナリデータをムーブして作成します。
		/// @param data ムーブするデータ
		[[nodiscard]]
		explicit constexpr Blob(Array<Byte>&& data);

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		/// @brief コピー代入演算子
		/// @param other コピーするデータ
		/// @return *this
		constexpr Blob& operator =(const Array<Byte>& other);

		/// @brief ムーブ代入演算子
		/// @param other ムーブするデータ
		/// @return *this
		constexpr Blob& operator =(Array<Byte>&& other) noexcept;

		/// @brief コピー代入演算子
		/// @param other コピーするデータ
		/// @return *this
		constexpr Blob& operator =(const Blob& other);

		/// @brief ムーブ代入演算子
		/// @param other ムーブするデータ
		/// @return *this
		constexpr Blob& operator =(Blob&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	assign
		//
		////////////////////////////////////////////////////////////////

		/// @brief 他の Blob からデータをコピーします。
		/// @param other コピー元の Blob
		constexpr void assign(const Blob& other);

		/// @brief 他の Blob からデータをムーブします。
		/// @param other ムーブ元の Blob
		constexpr void assign(Blob&& other);

		/// @brief メモリ上のデータをコピーします。
		/// @param src コピーするデータの先頭ポインタ
		/// @param sizeBytes コピーするデータのサイズ（バイト）
		void assign(const void* src, size_type sizeBytes);

		/// @brief バイナリデータをコピーします。
		/// @param data コピーするデータ
		constexpr void assign(const Array<Byte>& data);

		/// @brief バイナリデータをムーブしてバイナリデータを作成します。
		/// @param data ムーブするデータ
		constexpr void assign(Array<Byte>&& data);

		////////////////////////////////////////////////////////////////
		//
		//	assignRange
		//
		////////////////////////////////////////////////////////////////

		template <Concept::ContainerCompatibleRange<Byte> Range>
		constexpr void assign_range(Range&& range);

		////////////////////////////////////////////////////////////////
		//
		//	createFromFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルの中身をコピーします。
		/// @param path ファイルパス
		/// @return ファイルの読み込みに成功した場合 true, それ以外の場合は false
		bool createFromFile(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスにあるバイナリ値の参照を返します。
		/// @param index インデックス
		/// @return 指定したインデックスにあるバイナリ値の参照
		[[nodiscard]]
		constexpr const Byte& operator [](size_type index) const;

		/// @brief 指定したインデックスにあるバイナリ値の参照を返します。
		/// @param index インデックス
		/// @return 指定したインデックスにあるバイナリ値の参照
		[[nodiscard]]
		constexpr Byte& operator [](size_type index);

		////////////////////////////////////////////////////////////////
		//
		//	data
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータを格納する配列の先頭ポインタを返します。
		/// @return バイナリデータを格納する配列の先頭ポインタ
		[[nodiscard]]
		constexpr const Byte* data() const noexcept;

		/// @brief バイナリデータを格納する配列の先頭ポインタを返します。
		/// @return バイナリデータを格納する配列の先頭ポインタ
		[[nodiscard]]
		constexpr Byte* data() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asArray
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータを格納する配列にアクセスします。
		/// @return バイナリデータを格納する配列
		[[nodiscard]]
		constexpr const Array<Byte>& asArray() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	empty
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータが空であるかを返します。
		/// @return バイナリデータが空である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool empty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータが空であるかを返します。
		/// @return バイナリデータが空である場合 true, それ以外の場合は false
		/// @remark `empty()` と同じです。
		[[nodiscard]]
		constexpr bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータが空でないかを返します。
		/// @return バイナリデータが空でない場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータのサイズ（バイト）を返します。
		/// @remark `size_bytes()` と同じです。
		/// @return バイナリデータのサイズ（バイト）
		[[nodiscard]]
		constexpr size_t size() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	ssize
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータのサイズ（バイト）を返します。
		/// @return バイナリデータのサイズ（バイト）
		[[nodiscard]]
		constexpr isize ssize() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	size_bytes
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータのサイズ（バイト）を返します。
		/// @remark `size` と同じです。
		/// @return バイナリデータのサイズ（バイト）
		[[nodiscard]]
		constexpr size_t size_bytes() const noexcept;

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
		//	reserve
		//
		////////////////////////////////////////////////////////////////

		/// @brief sizeBytes バイトまでの要素をメモリの再確保無しで格納できるよう、必要に応じてメモリを再確保します。
		/// @param sizeBytes メモリを確保するバイト数
		constexpr void reserve(size_type sizeBytes);

		////////////////////////////////////////////////////////////////
		//
		//	resize
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータのサイズを sizeBytes バイトに変更します。
		/// @param sizeBytes 新しいバイナリデータのサイズ（バイト）
		constexpr void resize(size_type sizeBytes);

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 他の Blob と要素を入れ替えます。
		/// @param other 入れ替える配列
		constexpr void swap(Blob& other) noexcept;

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

		/// @brief バイナリデータを空にします。
		constexpr void clear();

		////////////////////////////////////////////////////////////////
		//
		//	release
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータを空にし、メモリを解放します。
		/// @remark `clear()` + `shrink_to_fit()` と同じです。
		constexpr void release();

		////////////////////////////////////////////////////////////////
		//
		//	begin / end
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータの先頭位置を指すイテレータを返します。
		/// @return バイナリデータの先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr iterator begin() noexcept;

		/// @brief バイナリデータの終端位置を指すイテレータを返します。
		/// @return バイナリデータの終端位置を指すイテレータ
		[[nodiscard]]
		constexpr iterator end() noexcept;

		/// @brief バイナリデータの先頭位置を指すイテレータを返します。
		/// @return バイナリデータの先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator begin() const noexcept;

		/// @brief バイナリデータの終端位置を指すイテレータを返します。
		/// @return バイナリデータの終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator end() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	cbegin / cend
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータの先頭位置を指すイテレータを返します。
		/// @return バイナリデータの先頭位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator cbegin() const noexcept;

		/// @brief バイナリデータの終端位置を指すイテレータを返します。
		/// @return バイナリデータの終端位置を指すイテレータ
		[[nodiscard]]
		constexpr const_iterator cend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rbegin / rend
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータの末尾位置を指すリバース・イテレータを返します。
		/// @return バイナリデータの末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr reverse_iterator rbegin() noexcept;

		/// @brief バイナリデータの先端位置を指すリバース・イテレータを返します。
		/// @return バイナリデータの先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr reverse_iterator rend() noexcept;

		/// @brief バイナリデータの末尾位置を指すリバース・イテレータを返します。
		/// @return バイナリデータの末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rbegin() const noexcept;

		/// @brief バイナリデータの先端位置を指すリバース・イテレータを返します。
		/// @return バイナリデータの先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator rend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	crbegin / crend
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータの末尾位置を指すリバース・イテレータを返します。
		/// @return バイナリデータの末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator crbegin() const noexcept;

		/// @brief バイナリデータの先端位置を指すリバース・イテレータを返します。
		/// @return バイナリデータの先端位置を指すリバース・イテレータ
		[[nodiscard]]
		constexpr const_reverse_iterator crend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	push_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾に 1 バイトを追加します。
		/// @param value 追加するバイト値
		constexpr void push_back(Byte value);

		////////////////////////////////////////////////////////////////
		//
		//	pop_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾の 1 バイトを削除します。
		constexpr void pop_back();

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		/// @brief 末尾にバイナリデータを追加します。
		/// @param other 追加するデータ
		constexpr void append(const Blob& other);

		/// @brief 末尾にバイナリデータを追加します。
		/// @param src 追加するデータの先頭ポインタ
		/// @param sizeBytes 追加するデータのサイズ
		void append(const void* src, size_type sizeBytes);

		////////////////////////////////////////////////////////////////
		//
		//	insert
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置に 1 バイトを挿入します。
		/// @param pos 挿入位置を指すイテレータ
		/// @param value 挿入するバイト値
		/// @return 挿入されたバイトを指すイテレータ
		constexpr iterator insert(const_iterator pos, Byte value);

		/// @brief 指定した位置にバイナリデータを挿入します。
		/// @tparam Iterator 挿入するデータのイテレータの型
		/// @param pos 挿入位置を指すイテレータ
		/// @param first 挿入するデータの先頭を指すイテレータ
		/// @param last 挿入するデータの終端を指すイテレータ
		/// @return 挿入された最初のバイトを指すイテレータ
		template <std::input_iterator Iterator>
		constexpr iterator insert(const_iterator pos, Iterator first, Iterator last);

		////////////////////////////////////////////////////////////////
		//
		//	subspan
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータの部分配列を指す span を返します。
		/// @param pos 部分配列の開始位置
		/// @param count 部分配列の要素数
		/// @return 部分配列を指す span
		[[nodiscard]]
		constexpr std::span<Byte> subspan(size_type pos, size_type count) noexcept;

		/// @brief バイナリデータの部分配列を指す span を返します。
		/// @param pos 部分配列の開始位置
		/// @param count 部分配列の要素数
		/// @return 部分配列を指す span
		[[nodiscard]]
		constexpr std::span<const Byte> subspan(size_type pos, size_type count) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	save
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータをファイルに保存します。
		/// @param path ファイルパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool save(FilePathView path) const;

		////////////////////////////////////////////////////////////////
		//
		//	md5
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータから MD5 ハッシュ値を計算します。
		/// @return バイナリデータの MD5 ハッシュ値
		[[nodiscard]]
		MD5Value md5() const;

		////////////////////////////////////////////////////////////////
		//
		//	base64
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータを Base64 エンコードします。
		/// @return エンコードされたデータ
		[[nodiscard]]
		Base64Value base64() const;

		/// @brief バイナリデータを Base64 エンコードし、dst に格納します。
		/// @param dst エンコードされたデータの格納先
		void base64(Base64Value& dst) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つのバイナリデータが等しいかを返します。
		/// @param lhs 一方のバイナリデータ
		/// @param rhs もう一方のバイナリデータ
		/// @return 2 つのバイナリデータが等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const Blob& lhs, const Blob& rhs) = default;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの Blob を入れ替えます。
		/// @param lhs 一方の Blob
		/// @param rhs もう一方の Blob
		friend constexpr void swap(Blob& lhs, Blob& rhs) noexcept
		{
			lhs.swap(rhs);
		}

	private:

		base_type m_data;
	};
}

# include "detail/Blob.ipp"
