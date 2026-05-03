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

# include <Siv3D/OSCMessage.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/Blob.hpp>
# include <Siv3D/OSCMessage/OSCMessageDetail.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	OSCMessage::OSCMessage(const size_t bufferSize)
		: pImpl{ std::make_shared<OSCMessageDetail>(bufferSize) } {}

	////////////////////////////////////////////////////////////////
	//
	//	beginBundle, endBundle
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::beginBundle()
	{
		try
		{
			pImpl->getStream() << osc::BeginBundleImmediate;
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::beginBundle(), out of buffer" };
		}
	}

	OSCMessage& OSCMessage::endBundle()
	{
		try
		{
			pImpl->getStream() << osc::EndBundle;
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::endBundle(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginMessage, endMessage
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::beginMessage(const StringView addressPattern)
	{
		try
		{
			pImpl->getStream() << osc::BeginMessage(addressPattern.toUTF8().c_str());
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::beginMessage(), out of buffer" };
		}
	}

	OSCMessage& OSCMessage::endMessage()
	{
		try
		{
			pImpl->getStream() << osc::EndMessage;
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::endMessage(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addBool
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addBool(const bool value)
	{
		try
		{
			pImpl->getStream() << value;
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addBool(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addChar
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addChar(const char value)
	{
		try
		{
			pImpl->getStream() << value;
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addChar(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addInt32
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addInt32(const int32 value)
	{
		try
		{
			pImpl->getStream() << static_cast<osc::int32>(value);
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addInt32(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addInt64
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addInt64(const int64 value)
	{
		try
		{
			pImpl->getStream() << static_cast<osc::int64>(value);
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addInt64(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addFloat
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addFloat(const float value)
	{
		try
		{
			pImpl->getStream() << value;
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addFloat(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addDouble
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addDouble(const double value)
	{
		try
		{
			pImpl->getStream() << value;
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addDouble(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addMIDIMessage
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addMIDIMessage(const uint32 value)
	{
		try
		{
			pImpl->getStream() << osc::MidiMessage{ value };
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addMIDIMessage(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTimeTag
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addTimeTag(const uint64 value)
	{
		try
		{
			pImpl->getStream() << osc::TimeTag{ value };
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addTimeTag(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRGBA
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addRGBA(const Color& value)
	{
		try
		{
			pImpl->getStream() << osc::RgbaColor{ value.asUint32() };
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addRGBA(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addString
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addString(const StringView value)
	{
		try
		{
			const std::string utf8 = value.toUTF8();
			pImpl->getStream() << utf8.c_str();
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addString(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addSymbol
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addSymbol(const StringView value)
	{
		try
		{
			const std::string utf8 = value.toUTF8();
			pImpl->getStream() << osc::Symbol{ utf8.c_str() };
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addSymbol(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addBlob
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addBlob(const Blob& blob)
	{
		return addBlob(blob.data(), blob.size_bytes());
	}

	OSCMessage& OSCMessage::addBlob(const void* data, const size_t size)
	{
		try
		{
			pImpl->getStream() << osc::Blob{ data, static_cast<osc::osc_bundle_element_size_t>(size) };
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addBlob(), out of buffer" };
		}
	}

	OSCMessage& OSCMessage::addBlob(const std::span<const Byte> data)
	{
		return addBlob(data.data(), data.size_bytes());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addNil
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addNil()
	{
		try
		{
			pImpl->getStream() << osc::OscNil;
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addNil(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addInf
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::addInf()
	{
		try
		{
			pImpl->getStream() << osc::Infinitum;
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::addInf(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginArray, endArray
	//
	////////////////////////////////////////////////////////////////

	OSCMessage& OSCMessage::beginArray()
	{
		try
		{
			pImpl->getStream() << osc::BeginArray;
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::beginArray(), out of buffer" };
		}
	}

	OSCMessage& OSCMessage::endArray()
	{
		try
		{
			pImpl->getStream() << osc::EndArray;
			return *this;
		}
		catch (const osc::OutOfBufferMemoryException&)
		{
			throw Error{ "OSCMessage::endArray(), out of buffer" };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	_detail
	//
	////////////////////////////////////////////////////////////////

	const osc::OutboundPacketStream* OSCMessage::_detail() const noexcept
	{
		return pImpl->getStreamPtr();
	}
}
