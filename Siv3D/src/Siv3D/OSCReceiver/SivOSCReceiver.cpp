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

# include <Siv3D/OSCReceiver.hpp>
# include <Siv3D/IPv4Endpoint.hpp>
# include <Siv3D/OSCReceiver/OSCReceiverDetail.hpp>

namespace s3d
{
	OSCReceiver::OSCReceiver()
		: pImpl{ std::make_shared<OSCReceiverDetail>() } {}

	OSCReceiver::OSCReceiver(const IPv4Endpoint& endpoint)
		: pImpl{ std::make_shared<OSCReceiverDetail>(endpoint) } {}

	OSCReceiver::~OSCReceiver() {}

	bool OSCReceiver::hasMessages()
	{
		return pImpl->hasMessages();
	}

	size_t OSCReceiver::num_messages()
	{
		return pImpl->num_messages();
	}

	OSCReceivedMessage OSCReceiver::pop()
	{
		return pImpl->pop();
	}

	void OSCReceiver::clear()
	{
		pImpl->clear();
	}
}
