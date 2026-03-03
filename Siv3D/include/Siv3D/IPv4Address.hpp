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
# include <array>
# include "Common.hpp"
# include "String.hpp"
# include "Optional.hpp"
# include "FormatLiteral.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	IPv4Address
	//
	////////////////////////////////////////////////////////////////

	/// @brief IPv4 アドレス（A.B.C.D）を表します。
	/// @remark 文字列形式は "192.168.0.1" のようなドット区切りの 10 進表記です。
	/// @remark この型はアドレスの表現・比較・分類（ループバック/プライベート等）を提供します。
	/// @remark 到達性判定、ポート番号、DNS 解決、ソケット等の機能は扱いません。
	class IPv4Address
	{
	public:

		/// @brief オクテット（0～255）を表す型です。
		using value_type = uint8;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 0.0.0.0 を作成します。
		[[nodiscard]]
		IPv4Address() = default;

		/// @brief 4 つのオクテットから IPv4 アドレスを作成します。
		/// @param a 先頭オクテット
		/// @param b 2 番目のオクテット
		/// @param c 3 番目のオクテット
		/// @param d 末尾オクテット
		/// @remark それぞれ 0～255 で表現されます。
		[[nodiscard]]
		constexpr IPv4Address(value_type a, value_type b, value_type c, value_type d) noexcept;

		/// @brief 4 要素の span から IPv4 アドレスを作成します。
		/// @param ipv4 オクテット列
		[[nodiscard]]
		explicit constexpr IPv4Address(std::span<const value_type, 4> ipv4);

		////////////////////////////////////////////////////////////////
		//
		//	isUnspecified, isAny
		//
		////////////////////////////////////////////////////////////////

		/// @brief 0.0.0.0（unspecified address）であるかを返します。
		/// @remark "any address"（isAny()）と同じです。
		/// @return 0.0.0.0 の場合 true, それ以外の場合 false
		[[nodiscard]]
		constexpr bool isUnspecified() const noexcept;

		/// @brief 0.0.0.0（any address）であるかを返します。
		/// @remark "unspecified address"（isUnspecified()）と同じです。
		/// @return 0.0.0.0 の場合 true, それ以外の場合 false
		[[nodiscard]]
		constexpr bool isAny() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isLoopback
		//
		////////////////////////////////////////////////////////////////

		/// @brief ループバックアドレス（127.0.0.0/8）であるかを返します。
		/// @return ループバックアドレスの場合 true, それ以外の場合 false
		[[nodiscard]]
		constexpr bool isLoopback() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isPrivate
		//
		////////////////////////////////////////////////////////////////

		/// @brief RFC1918 のプライベートアドレスであるかを返します。
		/// @remark 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16 のいずれかに該当する場合 true を返します。
		[[nodiscard]]
		constexpr bool isPrivate() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isCarrierGradeNAT
		//
		////////////////////////////////////////////////////////////////

		/// @brief キャリアグレード NAT（CGNAT）アドレス（100.64.0.0/10）であるかを返します。
		/// @remark ISP 側 NAT で利用されることがあり、RFC1918 のプライベートアドレスではありません。
		/// @remark インターネット越しに到達可能なグローバル IPv4 と誤認しないために使用できます。
		/// @return 100.64.0.0/10 に該当する場合 true, それ以外の場合 false
		[[nodiscard]]
		constexpr bool isCarrierGradeNAT() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//  isLinkLocal
		//
		////////////////////////////////////////////////////////////////

		/// @brief リンクローカルアドレス（169.254.0.0/16）であるかを返します。
		/// @remark DHCP に失敗した場合などに自動設定されることがあります。
		/// @return リンクローカルアドレスの場合 true, それ以外の場合 false
		[[nodiscard]]
		constexpr bool isLinkLocal() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//  isMulticast
		//
		////////////////////////////////////////////////////////////////

		/// @brief マルチキャストアドレス（224.0.0.0/4）であるかを返します。
		/// @return マルチキャストアドレスの場合 true, それ以外の場合 false
		[[nodiscard]]
		constexpr bool isMulticast() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isBroadcast
		//
		////////////////////////////////////////////////////////////////

		/// @brief ブロードキャストアドレス（255.255.255.255）であるかを返します。
		/// @remark サブネットの directed broadcast（例: 192.168.0.255）ではなく、
		/// @remark limited broadcast（255.255.255.255）の判定です。
		/// @return 255.255.255.255 の場合 true, それ以外の場合 false
		[[nodiscard]]
		constexpr bool isBroadcast() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	to_string, str
		//
		////////////////////////////////////////////////////////////////

		/// @brief IPv4 アドレスを ASCII 文字列に変換します。
		/// @return "A.B.C.D" 形式の文字列
		[[nodiscard]]
		std::string to_string() const;

		/// @brief IPv4 アドレスを String に変換します。
		/// @return U"A.B.C.D" 形式の文字列
		[[nodiscard]]
		String str() const;

		////////////////////////////////////////////////////////////////
		//
		//  toNetworkUint32
		//
		////////////////////////////////////////////////////////////////

		/// @brief IPv4 アドレスを 32-bit 整数に変換します。
		/// @return ネットワークバイトオーダー（ビッグエンディアン）表現の 32-bit 整数
		/// @remark 例: 192.168.0.1 -> 0xC0A80001
		/// @remark ホストのエンディアンに依存しません。
		[[nodiscard]]
		constexpr uint32 toNetworkUint32() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	octets
		//
		////////////////////////////////////////////////////////////////

		/// @brief 4 つのオクテットを返します。
		/// @return IPv4 アドレスのオクテット列
		[[nodiscard]]
		constexpr const std::array<value_type, 4>& octets() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の IPv4 アドレスと入れ替えます。
		/// @param other 別の IPv4 アドレス
		constexpr void swap(IPv4Address& other);

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

		/// @brief 2 つの IPv4 アドレスが等しいかを返します。
		/// @param lhs 一方の IPv4 アドレス
		/// @param rhs もう一方の IPv4 アドレス
		/// @return 2 つの IPv4 アドレスが等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const IPv4Address& lhs, const IPv4Address& rhs) = default;

		/// @brief 2 つの IPv4 アドレスを比較します。
		/// @param lhs 一方の IPv4 アドレス
		/// @param rhs もう一方の IPv4 アドレス
		/// @return 比較結果
		[[nodiscard]]
		friend constexpr auto operator <=>(const IPv4Address& lhs, const IPv4Address& rhs) = default;

		////////////////////////////////////////////////////////////////
		//
		//	Unspecified, Any
		//
		////////////////////////////////////////////////////////////////

		/// @brief 0.0.0.0 を返します。
		[[nodiscard]]
		static constexpr IPv4Address Unspecified() noexcept;

		/// @brief 0.0.0.0 を返します。
		[[nodiscard]]
		static constexpr IPv4Address Any() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Localhost
		//
		////////////////////////////////////////////////////////////////

		/// @brief 127.0.0.1 を返します。
		[[nodiscard]]
		static constexpr IPv4Address Localhost() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief IPv4 アドレスを出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value IPv4 アドレス
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const IPv4Address& value)
		{
			return (output << value.str());
		}

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの IPv4 アドレスの内容を交換します。
		/// @param lhs 一方の IPv4 アドレス
		/// @param rhs もう一方の IPv4 アドレス
		friend void swap(IPv4Address& lhs, IPv4Address& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		/// @brief "A.B.C.D" 形式の IPv4 アドレスをパースします。
		/// @param ip パースする文字列
		/// @return パースに成功した場合 IPv4Address、失敗した場合 none
		/// @remark 各オクテットは 0～255 の 10 進数です。
		/// @remark ゼロ埋めや空白は許容されません。
		[[nodiscard]]
		static Optional<IPv4Address> Parse(std::string_view ip);

		/// @brief U"A.B.C.D" 形式の IPv4 アドレスをパースします。
		/// @param ip パースする文字列
		/// @return パースに成功した場合 IPv4Address、失敗した場合 none
		/// @remark 各オクテットは 0～255 の 10 進数です。
		/// @remark ゼロ埋めや空白は許容されません。
		[[nodiscard]]
		static Optional<IPv4Address> Parse(StringView ip);

		////////////////////////////////////////////////////////////////
		//
		//  FromNetworkUint32
		//
		////////////////////////////////////////////////////////////////

		/// @brief 32-bit 整数から IPv4 アドレスを作成します。
		/// @param value ネットワークバイトオーダー（ビッグエンディアン）表現の 32-bit 整数
		/// @return IPv4 アドレス
		/// @remark 例: 0xC0A80001 -> 192.168.0.1
		/// @remark ホストのエンディアンに依存しません。
		[[nodiscard]]
		static constexpr IPv4Address FromNetworkUint32(uint32 value) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief IPv4Address を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value IPv4Address
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const IPv4Address& value);

	private:

		std::array<value_type, 4> m_octets{};
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::IPv4Address>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::IPv4Address& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::IPv4Address, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::IPv4Address& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::IPv4Address>
{
	[[nodiscard]]
	size_t operator ()(const s3d::IPv4Address& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/IPv4Address.ipp"
