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
# include <memory>
# include "Common.hpp"

class UdpTransmitSocket;

namespace s3d
{
	class IPv4Endpoint;
	class OSCMessage;

	////////////////////////////////////////////////////////////////
	//
	//  OSCSender
	//
	////////////////////////////////////////////////////////////////

	/// @brief OSC 送信クラス
	class OSCSender
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		OSCSender() = default;

		/// @brief OSC の送信先を設定します。
		/// @param endpoint 送信先の IPv4 エンドポイント
		[[nodiscard]]
		OSCSender(const IPv4Endpoint& endpoint);

		////////////////////////////////////////////////////////////////
		//
		//	send
		//
		////////////////////////////////////////////////////////////////

		/// @brief OSC メッセージを送信します。
		/// @param message メッセージ
		void send(const OSCMessage& message);

	public:

		std::shared_ptr<UdpTransmitSocket> m_transmitSocket;
	};
}
