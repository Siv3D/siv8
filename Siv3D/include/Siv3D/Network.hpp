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
# include "IPv4Address.hpp"

namespace s3d
{
	namespace Network
	{
		////////////////////////////////////////////////////////////////
		//
		//	HasInternetConnectivity
		//
		////////////////////////////////////////////////////////////////

		/// @brief OS がインターネット到達性ありと判断しているかを返します。
		/// @remark 実際の通信成否（特定サーバへの接続可否）を保証するものではありません。
		/// @remark VPN, キャプティブポータル、企業ネットワークの制限等により、true でも通信が失敗する場合があります。
		/// @return インターネット到達性ありと判断される場合 true, それ以外の場合 false
		[[nodiscard]]
		bool HasInternetConnectivity();

		////////////////////////////////////////////////////////////////
		//
		//	EnumerateLocalIPv4Addresses
		//
		////////////////////////////////////////////////////////////////

		/// @brief ローカルネットワークで利用可能な IPv4 アドレスを列挙します。
		/// @remark ネットワークインターフェース（有線 / 無線 / VPN / 仮想 NIC など）に設定された IPv4 を返します。
		/// @remark ループバック (127.0.0.0/8) とリンクローカル (169.254.0.0/16) は含まれません。
		/// @remark 有効なネットワークインターフェース（OperStatus == Up）のみを対象とします。
		/// @remark 返却順序は OS に依存し、固定ではありません。
		/// @return IPv4 アドレスの配列。取得できなかった場合は空の配列
		[[nodiscard]]
		Array<IPv4Address> EnumerateLocalIPv4Addresses();

		////////////////////////////////////////////////////////////////
		//
		//	FetchPublicIPv4Address
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//Optional<IPv4Address> FetchPublicIPv4Address();
	}
}
