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
# include <Siv3D/OSCMessage.hpp>
# include <ThirdParty/oscpack/osc/OscOutboundPacketStream.h>

namespace s3d
{
	class OSCMessage::OSCMessageDetail
	{
	public:

		[[nodiscard]]
		OSCMessageDetail(size_t bufferSize);

		[[nodiscard]]
		osc::OutboundPacketStream& getStream();

		[[nodiscard]]
		const osc::OutboundPacketStream* getStreamPtr() const;

	private:

		std::string m_buffer;

		osc::OutboundPacketStream m_stream;
	};
}
