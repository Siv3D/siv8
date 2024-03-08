﻿//-----------------------------------------------
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
# include <array>
# include "Common.hpp"
# include "String.hpp"
# include "Utility.hpp"
# include "Optional.hpp"
# include "Error.hpp"
# include "Char.hpp"
# include "FormatData.hpp"

namespace s3d
{
	class Blob;

	class MD5Value
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空のデータに対する MD5 ハッシュ値 (d41d8cd98f00b204e9800998ecf8427e) で初期化
		[[nodiscard]]
		constexpr MD5Value() noexcept;

		[[nodiscard]]
		explicit constexpr MD5Value(const std::array<uint8, 16>& md5) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmptyHash
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空のデータに対する MD5 ハッシュ値 (d41d8cd98f00b204e9800998ecf8427e) であるかを返します。
		/// @return 空のデータに対する MD5 ハッシュ値である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isEmptyHash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getData
		//
		////////////////////////////////////////////////////////////////

		/// @brief MD5 ハッシュ値への参照を返します。
		/// @return MD5 ハッシュ値への参照
		[[nodiscard]]
		constexpr const std::array<uint8, 16>& getData() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		constexpr void swap(MD5Value& other);

		////////////////////////////////////////////////////////////////
		//
		//	to_string
		//
		////////////////////////////////////////////////////////////////

		/// @brief MD5 ハッシュ値を文字列で返します。
		/// @return MD5 ハッシュ値の文字列
		[[nodiscard]]
		std::string to_string() const;

		////////////////////////////////////////////////////////////////
		//
		//	str
		//
		////////////////////////////////////////////////////////////////

		/// @brief MD5 ハッシュ値を文字列で返します。
		/// @return MD5 ハッシュ値の文字列
		[[nodiscard]]
		String str() const;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==, operator <=>
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(const MD5Value&, const MD5Value&) = default;

		[[nodiscard]]
		friend constexpr auto operator <=>(const MD5Value& lhs, const MD5Value& rhs) = default;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		friend void swap(MD5Value& lhs, MD5Value& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	EmptyHash
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空のデータに対する MD5 ハッシュ値 (d41d8cd98f00b204e9800998ecf8427e) を返します。
		/// @return 空のデータに対する MD5 ハッシュ値
		[[nodiscard]]
		static constexpr MD5Value EmptyHash() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromBinary
		//
		////////////////////////////////////////////////////////////////

		/// @brief バイナリデータから MD5 ハッシュを計算します。
		/// @param data データの先頭ポインタ
		/// @param size データのサイズ（バイト）
		/// @return 計算された MD5 ハッシュ
		[[nodiscard]]
		static MD5Value FromBinary(const void* const data, size_t size) noexcept;

		/// @brief バイナリデータから MD5 ハッシュを計算します。
		/// @param blob バイナリデータ
		/// @return 計算された MD5 ハッシュ
		[[nodiscard]]
		static MD5Value FromBinary(const Blob& blob) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列 (UTF-8) から MD5 ハッシュを計算します。
		/// @param view 文字列 (UTF-8)
		/// @return 計算された MD5 ハッシュ
		[[nodiscard]]
		static MD5Value FromUTF8(std::string_view view) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromUTF32
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列 (UTF-32) から MD5 ハッシュを計算します。
		/// @param view 文字列 (UTF-32)
		/// @return 計算された MD5 ハッシュ
		[[nodiscard]]
		static MD5Value FromUTF32(StringView view) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルから MD5 ハッシュを計算します。
		/// @param path ファイルのパス
		/// @return 計算された MD5 ハッシュ
		[[nodiscard]]
		static MD5Value FromFile(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr Optional<MD5Value> Parse(std::string_view md5);

		[[nodiscard]]
		static constexpr Optional<MD5Value> Parse(StringView md5);

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const MD5Value& value)
		{
			return (output << value.str());
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const MD5Value& value);

	private:

		std::array<uint8, 16> m_values{};
	};
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::MD5Value>
{
	[[nodiscard]]
	size_t operator ()(const s3d::MD5Value& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/MD5Value.ipp"
