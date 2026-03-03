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
# include "IPv4Address.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	IPv4Endpoint
	//
	////////////////////////////////////////////////////////////////

	/// @brief IPv4 エンドポイント（IPv4Address + ポート番号）を表します。
	/// @remark 文字列形式は "192.168.0.1:80" のような "A.B.C.D:port" 形式です。
	/// @remark この型は表現・比較・変換・パースを提供します。
	/// @remark DNS 解決やソケット接続、到達性判定等の機能は扱いません。
	class IPv4Endpoint
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	port_type
		//
		////////////////////////////////////////////////////////////////

		/// @brief ポート番号（0～65535）を表す型です。
		using port_type = uint16;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 0.0.0.0:0 を作成します。
		[[nodiscard]]
		IPv4Endpoint() = default;

		/// @brief IPv4 アドレスとポート番号から IPv4 エンドポイントを作成します。
		/// @param address IPv4 アドレス
		/// @param port ポート番号（0～65535）
		[[nodiscard]]
		constexpr IPv4Endpoint(const IPv4Address& address, port_type port) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	address, port
		//
		////////////////////////////////////////////////////////////////

		/// @brief IPv4 アドレスを返します。
		/// @return IPv4 アドレス
		[[nodiscard]]
		constexpr const IPv4Address& address() const noexcept;

		/// @brief ポート番号を返します。
		/// @return ポート番号（0～65535）
		[[nodiscard]]
		constexpr port_type port() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isAny
		//
		////////////////////////////////////////////////////////////////

		/// @brief 0.0.0.0:0 であるかを返します。
		/// @return 0.0.0.0:0 の場合 true, それ以外の場合 false
		[[nodiscard]]
		constexpr bool isAny() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	to_string, str
		//
		////////////////////////////////////////////////////////////////

		/// @brief "A.B.C.D:port" 形式の ASCII 文字列に変換します。
		/// @return "A.B.C.D:port" 形式の ASCII 文字列
		[[nodiscard]]
		std::string to_string() const;

		/// @brief U"A.B.C.D:port" 形式の文字列に変換します。
		/// @return U"A.B.C.D:port" 形式の文字列
		[[nodiscard]]
		String str() const;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の IPv4Endpoint と入れ替えます。
		/// @param other 別の IPv4Endpoint
		constexpr void swap(IPv4Endpoint& other) noexcept;

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

		/// @brief 2 つの IPv4Endpoint が等しいかを返します。
		/// @param lhs 一方の IPv4Endpoint
		/// @param rhs もう一方の IPv4Endpoint
		/// @return 両方のアドレスとポートが等しい場合 true, それ以外の場合 false
		[[nodiscard]]
		friend constexpr bool operator ==(const IPv4Endpoint& lhs, const IPv4Endpoint& rhs) = default;

		/// @brief 2 つの IPv4Endpoint を辞書式順序で比較します。
		/// @param lhs 一方の IPv4Endpoint
		/// @param rhs もう一方の IPv4Endpoint
		/// @return 比較結果
		[[nodiscard]]
		friend constexpr auto operator <=>(const IPv4Endpoint& lhs, const IPv4Endpoint& rhs) = default;

		////////////////////////////////////////////////////////////////
		//
		//	Any
		//
		////////////////////////////////////////////////////////////////

		/// @brief 0.0.0.0:0 を返します。
		/// @return IPv4Endpoint{ IPv4Address::Any(), 0 } 
		[[nodiscard]]
		static constexpr IPv4Endpoint Any() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Localhost
		//
		////////////////////////////////////////////////////////////////

		/// @brief 127.0.0.1:port を返します。
		/// @param port ポート番号（0～65535）
		/// @return IPv4Endpoint{ IPv4Address::Localhost(), port }
		[[nodiscard]]
		static constexpr IPv4Endpoint Localhost(port_type port) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief IPv4Endpoint を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value IPv4Endpoint
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const IPv4Endpoint& value)
		{
			return (output << value.str());
		}

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの IPv4Endpoint の内容を交換します。
		/// @param lhs 一方の IPv4Endpoint
		/// @param rhs もう一方の IPv4Endpoint
		friend void swap(IPv4Endpoint& lhs, IPv4Endpoint& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		/// @brief "A.B.C.D:port" 形式の IPv4Endpoint をパースします。
		/// @param s パースする文字列
		/// @return パースに成功した場合 IPv4Endpoint、失敗した場合 none
		/// @remark A.B.C.D は IPv4Address::Parse() と同じルールです。
		/// @remark port は 0～65535 の 10 進数です。
		/// @remark 前後空白、ゼロ埋め、符号、+, 空文字は許容されません。
		/// @remark ":" は 1 つだけ必要です。
		[[nodiscard]]
		static Optional<IPv4Endpoint> Parse(std::string_view s);

		/// @brief U"A.B.C.D:port" 形式の IPv4Endpoint をパースします。
		/// @param s パースする文字列
		/// @return パースに成功した場合 IPv4Endpoint、失敗した場合 none
		/// @remark A.B.C.D は IPv4Address::Parse() と同じルールです。
		/// @remark port は 0～65535 の 10 進数です。
		/// @remark 前後空白、ゼロ埋め、符号、+, 空文字は許容されません。
		/// @remark ":" は 1 つだけ必要です。
		[[nodiscard]]
		static Optional<IPv4Endpoint> Parse(StringView s);

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief IPv4Endpoint を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value IPv4Endpoint
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const IPv4Endpoint& value);

	private:

		IPv4Address m_address{};
		
		port_type m_port = 0;
	};
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::IPv4Endpoint>
{
	[[nodiscard]]
	size_t operator ()(const s3d::IPv4Endpoint& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/IPv4Endpoint.ipp"
