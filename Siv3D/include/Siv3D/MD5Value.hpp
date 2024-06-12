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
# include <array>
# include "Common.hpp"
# include "Utility.hpp"
# include "Optional.hpp"
# include "Error.hpp"
# include "Char.hpp"

namespace s3d
{
	class Blob;
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	MD5Value
	//
	////////////////////////////////////////////////////////////////

	class MD5Value
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空のデータに対する MD5 ハッシュ値 (d41d8cd98f00b204e9800998ecf8427e) を作成します。
		[[nodiscard]]
		constexpr MD5Value() noexcept;

		/// @brief MD5 ハッシュ値を作成します。
		/// @param md5 MD5 ハッシュ値
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

		/// @brief 別の MD5 ハッシュ値と入れ替えます。
		/// @param other 別の MD5 ハッシュ値
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

		/// @brief 2 つの MD5 ハッシュ値が等しいかを返します。
		/// @param lhs 一方の MD5 ハッシュ値
		/// @param rhs もう一方の MD5 ハッシュ値
		/// @return 2 つの MD5 ハッシュ値が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const MD5Value& lhs, const MD5Value& rhs) = default;

		[[nodiscard]]
		friend constexpr auto operator <=>(const MD5Value& lhs, const MD5Value& rhs) = default;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの MD5 ハッシュ値を入れ替えます。
		/// @param lhs 一方の MD5 ハッシュ値
		/// @param rhs もう一方の MD5 ハッシュ値
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
		//	FromMemory
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリ上のデータから MD5 ハッシュを計算します。
		/// @param data データの先頭ポインタ
		/// @param size データのサイズ（バイト）
		/// @return 計算された MD5 ハッシュ
		[[nodiscard]]
		static MD5Value FromMemory(const void* const data, size_t size) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromBlob
		//
		////////////////////////////////////////////////////////////////

		/// @brief Blob から MD5 ハッシュを計算します。
		/// @param blob Blob
		/// @return 計算された MD5 ハッシュ
		[[nodiscard]]
		static MD5Value FromBlob(const Blob& blob) noexcept;

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
