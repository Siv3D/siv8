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
# include <array>
# include "Common.hpp"
# include "Utility.hpp"
# include "StringView.hpp"
# include "Optional.hpp"
# include "Random.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	UUIDValue
	//
	////////////////////////////////////////////////////////////////

	/// @brief UUID
	class UUIDValue
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	Variant
		//
		////////////////////////////////////////////////////////////////

		/// @brief バリアント
		enum class Variant : uint8
		{
			NCS,
			RFC,
			Microsoft,
			Reserved,
		};

		////////////////////////////////////////////////////////////////
		//
		//	Version
		//
		////////////////////////////////////////////////////////////////

		/// @brief バージョン
		enum class Version : uint8
		{
			Nil,
			TimeBased,
			DCE,
			NameBasedMD5,
			Random,
			NameBasedSHA1,
		};

		using value_type = uint8;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		UUIDValue() = default;

		/// @brief UUID を作成します。
		/// @param uuid UUID のデータ
		[[nodiscard]]
		explicit UUIDValue(const value_type(&uuid)[16]) noexcept;

		/// @brief UUID を作成します。
		/// @param uuid UUID のデータ
		[[nodiscard]]
		explicit constexpr UUIDValue(const std::array<value_type, 16>& uuid) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isNil
		//
		////////////////////////////////////////////////////////////////

		/// @brief UUID が nil であるかを返します。
		/// @return UUID が nil である場合 true, それ以外の場合は false
		/// @remark すべてのバイトが 0 の場合、UUID は nil です。
		[[nodiscard]]
		constexpr bool isNil() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	variant
		//
		////////////////////////////////////////////////////////////////

		/// @brief バリアントを返します。
		/// @return バリアント
		[[nodiscard]]
		Variant variant() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	version
		//
		////////////////////////////////////////////////////////////////

		/// @brief バージョンを返します。
		/// @return バージョン
		[[nodiscard]]
		Version version() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	to_string
		//
		////////////////////////////////////////////////////////////////

		/// @brief UUID を文字列に変換します。
		/// @return 変換した文字列
		[[nodiscard]]
		std::string to_string() const;

		////////////////////////////////////////////////////////////////
		//
		//	str
		//
		////////////////////////////////////////////////////////////////

		/// @brief UUID を文字列に変換します。
		/// @return 変換した文字列
		[[nodiscard]]
		String str() const;

		////////////////////////////////////////////////////////////////
		//
		//	getData
		//
		////////////////////////////////////////////////////////////////

		/// @brief UUID のデータを返します。
		/// @return UUID のデータ
		[[nodiscard]]
		constexpr const std::array<value_type, 16>& getData() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の UUID と内容を交換します。
		/// @param other もう一方の UUID
		constexpr void swap(UUIDValue& other);

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		/// @brief ハッシュ値を返します。
		/// @return ハッシュ値
		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==, operator <=>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの UUID が等しいかを返します。
		/// @param lhs 一方の UUID
		/// @param rhs もう一方の UUID
		/// @return 2 つの UUID が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const UUIDValue& lhs, const UUIDValue& rhs) = default;

		/// @brief 2 つの UUID を比較します。
		/// @param lhs 一方の UUID
		/// @param rhs もう一方の UUID
		/// @return 比較結果
		[[nodiscard]]
		friend constexpr auto operator <=>(const UUIDValue& lhs, const UUIDValue& rhs) = default;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの UUID の内容を交換します。
		/// @param lhs 一方の UUID
		/// @param rhs もう一方の UUID
		friend void swap(UUIDValue& lhs, UUIDValue& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Generate
		//
		////////////////////////////////////////////////////////////////

		/// @brief ランダムな UUID を生成します。
		/// @return 生成した UUID
		[[nodiscard]]
		static UUIDValue Generate();

		////////////////////////////////////////////////////////////////
		//
		//	GenerateFromRNG
		//
		////////////////////////////////////////////////////////////////

		/// @brief ランダムな UUID を生成します。
		/// @param rng 乱数生成器
		/// @return 生成した UUID
		[[nodiscard]]
		static UUIDValue GenerateFromRNG(DefaultRNG& rng = GetDefaultRNG());

		////////////////////////////////////////////////////////////////
		//
		//	GenerateFromName
		//
		////////////////////////////////////////////////////////////////

		/// @brief 名前から UUID を生成します。
		/// @param namespaceUUIDValue 名前空間の UUID
		/// @param name 名前
		/// @return 生成した UUID
		[[nodiscard]]
		static UUIDValue GenerateFromName(const UUIDValue& namespaceUUIDValue, std::string_view name);

		/// @brief 名前から UUID を生成します。
		/// @param namespaceUUIDValue 名前空間の UUID
		/// @param name 名前
		/// @return 生成した UUID
		[[nodiscard]]
		static UUIDValue GenerateFromName(const UUIDValue& namespaceUUIDValue, StringView name);

		////////////////////////////////////////////////////////////////
		//
		//	Nil
		//
		////////////////////////////////////////////////////////////////

		/// @brief nil の UUID を返します。
		/// @return nil の UUID
		[[nodiscard]]
		static UUIDValue Nil();

		////////////////////////////////////////////////////////////////
		//
		//	NamespaceDNS
		//
		////////////////////////////////////////////////////////////////

		/// @brief DNS 名前空間の UUID を返します。
		/// @return DNS 名前空間の UUID
		[[nodiscard]]
		static UUIDValue NamespaceDNS() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	NamespaceURL
		//
		////////////////////////////////////////////////////////////////

		/// @brief URL 名前空間の UUID を返します。
		/// @return URL 名前空間の UUID
		[[nodiscard]]
		static UUIDValue NamespaceURL() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	NamespaceOID
		//
		////////////////////////////////////////////////////////////////

		/// @brief OID 名前空間の UUID を返します。
		/// @return OID 名前空間の UUID
		[[nodiscard]]
		static UUIDValue NamespaceOID() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	NamespaceX500
		//
		////////////////////////////////////////////////////////////////

		/// @brief X.500 名前空間の UUID を返します。
		/// @return X.500 名前空間の UUID
		[[nodiscard]]
		static UUIDValue NamespaceX500() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	IsValid
		//
		////////////////////////////////////////////////////////////////

		/// @brief UUID の形式が正しいかを返します。
		/// @param uuid UUID
		/// @return UUID の形式が正しい場合 true, それ以外の場合は false
		[[nodiscard]]
		static bool IsValid(std::string_view uuid);

		/// @brief UUID の形式が正しいかを返します。
		/// @param uuid UUID
		/// @return UUID の形式が正しい場合 true, それ以外の場合は false
		[[nodiscard]]
		static bool IsValid(StringView uuid);

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		/// @brief UUID をパースします。
		/// @param uuid UUID
		/// @return パースした結果。パースに失敗した場合は none 
		[[nodiscard]]
		static Optional<UUIDValue> Parse(std::string_view uuid);

		/// @brief UUID をパースします。
		/// @param uuid UUID
		/// @return パースした結果。パースに失敗した場合は none
		[[nodiscard]]
		static Optional<UUIDValue> Parse(StringView uuid);

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief UUID を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value UUID
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const UUIDValue& value)
		{
			return (output << value.str());
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief UUID を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value UUID
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const UUIDValue& value);

	private:

		std::array<value_type, 16> m_data{};
	};
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::UUIDValue>
{
	[[nodiscard]]
	size_t operator ()(const s3d::UUIDValue& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/UUIDValue.ipp"
