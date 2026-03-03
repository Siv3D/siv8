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

# include <Siv3D/OSCSender.hpp>
# include <Siv3D/OSCMessage.hpp>
# include <Siv3D/IPv4Endpoint.hpp>
# include <ThirdParty/oscpack/ip/IpEndpointName.h>
# include <ThirdParty/oscpack/ip/UdpSocket.h>
# include <ThirdParty/oscpack/osc/OscOutboundPacketStream.h>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	OSCSender::OSCSender(const IPv4Endpoint& endpoint)
		: m_transmitSocket{ std::make_shared<UdpTransmitSocket>(IpEndpointName{ endpoint.address().toNetworkUint32(),endpoint.port() }) } {}

	////////////////////////////////////////////////////////////////
	//
	//	send
	//
	////////////////////////////////////////////////////////////////

	void OSCSender::send(const OSCMessage& message)
	{
		if (not m_transmitSocket)
		{
			return;
		}

		const osc::OutboundPacketStream* pStream = message._detail();
		m_transmitSocket->Send(pStream->Data(), pStream->Size());
	}
}
