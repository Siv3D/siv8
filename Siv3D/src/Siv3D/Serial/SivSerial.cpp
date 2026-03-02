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

# include <Siv3D/Serial.hpp>
# include "SerialDetail.hpp"

namespace s3d
{
	Serial::Serial()
		: pImpl{ std::make_shared<SerialDetail>() } {}

	Serial::Serial(const StringView port, const int32 baudrate, const Config& config)
		: Serial{}
	{
		open(port, baudrate, config);
	}

	Serial::~Serial() = default;

	bool Serial::open(const StringView port, const int32 baudrate, const Config& config)
	{
		return pImpl->open(port, baudrate, config);
	}

	void Serial::close()
	{
		pImpl->close();
	}

	bool Serial::isOpen() const
	{
		return pImpl->isOpen();
	}

	Serial::operator bool() const
	{
		return isOpen();
	}

	int32 Serial::baudrate() const noexcept
	{
		return pImpl->baudrate();
	}

	bool Serial::setBaudrate(const int32 baudrate)
	{
		return pImpl->setBaudrate(baudrate);
	}

	bool Serial::setConfig(const Config& config)
	{
		return pImpl->setConfig(config);
	}

	const String& Serial::port() const noexcept
	{
		return pImpl->port();
	}

	size_t Serial::available()
	{
		return pImpl->available();
	}

	void Serial::clearInput()
	{
		pImpl->clearInput();
	}

	void Serial::clearOutput()
	{
		pImpl->clearOutput();
	}

	void Serial::clear()
	{
		pImpl->clear();
	}

	size_t Serial::read(void* dst, const size_t size)
	{
		return pImpl->read(dst, size);
	}

	Array<uint8> Serial::readBytes()
	{
		const size_t bytesAvailable = available();

		Array<uint8> result(bytesAvailable);

		if (bytesAvailable)
		{
			read(result.data(), bytesAvailable);
		}

		return result;
	}

	bool Serial::readBytes(Array<uint8>& dst)
	{
		dst.clear();

		const size_t bytesAvailable = available();

		dst.resize(bytesAvailable);

		if (bytesAvailable)
		{
			read(dst.data(), bytesAvailable);
		}

		return (0 < bytesAvailable);
	}

	bool Serial::readLine(String& line)
	{
		return pImpl->readLine(line);
	}

	size_t Serial::write(const void* src, const size_t size)
	{
		return pImpl->write(src, size);
	}

	bool Serial::writeByte(const uint8 byte)
	{
		return write(byte);
	}

	bool Serial::writeByte(const Byte byte)
	{
		return write(byte);
	}

	bool Serial::write(const StringView s)
	{
		const std::string utf8 = Unicode::ToUTF8(s);
		return (pImpl->write(utf8.data(), utf8.size()) == utf8.size());
	}

	bool Serial::writeln(const StringView s)
	{
		const std::string utf8 = (Unicode::ToUTF8(s) + '\n');
		return (pImpl->write(utf8.data(), utf8.size()) == utf8.size());
	}

	void Serial::setBreak(const bool level)
	{
		pImpl->setBreak(level);
	}

	void Serial::setRTS(const bool level)
	{
		pImpl->setRTS(level);
	}

	void Serial::setDTR(const bool level)
	{
		pImpl->setDTR(level);
	}

	bool Serial::waitForChange()
	{
		return pImpl->waitForChange();
	}

	bool Serial::getCTS() const
	{
		return pImpl->getCTS();
	}

	bool Serial::getDSR() const
	{
		return pImpl->getDSR();
	}

	bool Serial::getRI() const
	{
		return pImpl->getRI();
	}

	bool Serial::getCD() const
	{
		return pImpl->getCD();
	}
}
