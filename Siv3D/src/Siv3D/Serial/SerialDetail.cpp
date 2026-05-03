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

# include "SerialDetail.hpp"
# include <Siv3D/Error.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	bool Serial::SerialDetail::open(const StringView port, const int32 baudrate, const Config& config)
	{
		if (isOpen())
		{
			close();
		}

		m_serial.setPort(port.toUTF8());
		m_serial.setBaudrate(static_cast<uint32>(baudrate));
		setConfig(config);

		try
		{
			m_serial.open();
		}
		catch (const serial_cpp::IOException& e)
		{
			LOG_FAIL(fmt::format("❌ Failed to open Serial `{0}`. {1}", port.toUTF8(), e.what()));
			return false;
		}

		m_port = port;
		m_baudrate = baudrate;
		return m_serial.isOpen();
	}

	void Serial::SerialDetail::close()
	{
		try
		{
			m_serial.close();
		}
		catch (const serial_cpp::IOException&)
		{

		}

		m_port.clear();
		m_baudrate = 0;
		m_internalReadBuffer.clear();
	}

	bool Serial::SerialDetail::isOpen()
	{
		if (not m_serial.isOpen())
		{
			return false;
		}

		try
		{
			m_serial.available();
			return true;
		}
		catch (const serial_cpp::IOException&)
		{
			close();
			return false;
		}
	}

	int32 Serial::SerialDetail::baudrate() const noexcept
	{
		return m_baudrate;
	}

	bool Serial::SerialDetail::setBaudrate(int32 baudrate)
	{
		if (baudrate == m_baudrate)
		{
			return true;
		}
	
		try
		{
			m_serial.setBaudrate(static_cast<uint32>(baudrate));
		}
		catch (const std::exception&)
		{
			return false;
		}

		m_baudrate = baudrate;
		return true;
	}

	bool Serial::SerialDetail::setConfig(const Config& config)
	{
		try
		{
			m_serial.setBytesize(static_cast<serial_cpp::bytesize_t>(FromEnum(config.dataBits)));
			m_serial.setParity(static_cast<serial_cpp::parity_t>(FromEnum(config.parity)));
			m_serial.setStopbits(static_cast<serial_cpp::stopbits_t>(FromEnum(config.stopBits)));
			m_serial.setFlowcontrol(static_cast<serial_cpp::flowcontrol_t>(FromEnum(config.flowControl)));
			m_serial.setTimeout(serial_cpp::Timeout{ config.timeout.readIntervalTimeoutMillisec,
				config.timeout.readTotalTimeoutMillisec,
				config.timeout.readTotalTimeoutMultiplier,
				config.timeout.writeTotalTimeoutMillisec,
				config.timeout.writeTotalTimeoutMultiplier });
		}
		catch (const std::exception&)
		{
			return false;
		}

		return true;
	}

	const String& Serial::SerialDetail::port() const noexcept
	{
		return m_port;
	}

	size_t Serial::SerialDetail::available()
	{
		if (not m_serial.isOpen())
		{
			return 0;
		}

		try
		{
			return (m_internalReadBuffer.size() + m_serial.available());
		}
		catch (const serial_cpp::IOException&)
		{
			close();
			return 0;
		}
	}

	void Serial::SerialDetail::clearInput()
	{
		if (not isOpen())
		{
			return;
		}

		m_internalReadBuffer.clear();
		m_serial.flushInput();
	}

	void Serial::SerialDetail::clearOutput()
	{
		if (not isOpen())
		{
			return;
		}

		m_serial.flushOutput();
	}

	void Serial::SerialDetail::clear()
	{
		if (not isOpen())
		{
			return;
		}

		m_internalReadBuffer.clear();
		m_serial.flush();
	}

	size_t Serial::SerialDetail::read(void* dst, const size_t size)
	{
		if (not isOpen() || (size == 0))
		{
			return 0;
		}

		size_t copied = 0;
		uint8* out = static_cast<uint8*>(dst);

		// (1) 内部バッファから消費
		if (m_internalReadBuffer)
		{
			const size_t n = Min<size_t>(size, m_internalReadBuffer.size());
			std::memcpy(out, m_internalReadBuffer.data(), n);

			// 先頭 n バイトを削除
			m_internalReadBuffer.pop_front_N(n);

			copied += n;
		}

		// (2) 足りない分をデバイスから読む
		const size_t remain = (size - copied);
		if (remain == 0)
		{
			return copied;
		}

		try
		{
			const size_t r = m_serial.read(out + copied, remain);
			copied += r;
			return copied;
		}
		catch (const serial_cpp::IOException&)
		{
			close();
			return 0;
		}
	}

	bool Serial::SerialDetail::readLine(String& line)
	{
		line.clear();

		if (not isOpen())
		{
			return false;
		}

		// その時点で available な分を全吸い上げ
		fillInternalReadBuffer();

		// '\n' を探索
		const auto itLF = std::find(m_internalReadBuffer.begin(), m_internalReadBuffer.end(), '\n');
		if (itLF == m_internalReadBuffer.end())
		{
			return false; // 行未完成。バッファ保持
		}

		// [0, lfPos) が行本体（末尾 CR は除外）
		const size_t lfPos = static_cast<size_t>(itLF - m_internalReadBuffer.begin());
		size_t endPos = lfPos;

		if ((0 < endPos) && (m_internalReadBuffer[endPos - 1] == '\r'))
		{
			--endPos;
		}

		const char* p = m_internalReadBuffer.data();
		std::string_view sv(p, endPos);

		// UTF-8 -> String
		line = Unicode::FromUTF8(sv);

		// 消費：'\n' まで（= lfPos+1）
		m_internalReadBuffer.pop_front_N(lfPos + 1);

		return true;
	}

	size_t Serial::SerialDetail::write(const void* src, const size_t size)
	{
		if (not isOpen())
		{
			return 0;
		}

		try
		{
			return m_serial.write(static_cast<const uint8*>(src), size);
		}
		catch (const serial_cpp::IOException&)
		{
			close();
			return 0;
		}
	}

	void Serial::SerialDetail::setBreak(const bool level)
	{
		if (not isOpen())
		{
			return;
		}

		m_serial.setBreak(level);
	}

	void Serial::SerialDetail::setRTS(const bool level)
	{
		if (not isOpen())
		{
			return;
		}

		m_serial.setRTS(level);
	}

	void Serial::SerialDetail::setDTR(const bool level)
	{
		if (not isOpen())
		{
			return;
		}

		m_serial.setDTR(level);
	}

	bool Serial::SerialDetail::waitForChange()
	{
		if (not isOpen())
		{
			return false;
		}

		try
		{
			return m_serial.waitForChange();
		}
		catch (const serial_cpp::SerialException&)
		{
			return false;
		}
	}

	bool Serial::SerialDetail::getCTS()
	{
		if (not isOpen())
		{
			return false;
		}

		return m_serial.getCTS();
	}

	bool Serial::SerialDetail::getDSR()
	{
		if (not isOpen())
		{
			return false;
		}

		return m_serial.getDSR();
	}

	bool Serial::SerialDetail::getRI()
	{
		if (not isOpen())
		{
			return false;
		}

		return m_serial.getRI();
	}

	bool Serial::SerialDetail::getCD()
	{
		if (not isOpen())
		{
			return false;
		}

		return m_serial.getCD();
	}

	void Serial::SerialDetail::fillInternalReadBuffer()
	{
		size_t bytesAvailable = 0;

		try
		{
			bytesAvailable = m_serial.available();
		}
		catch (const serial_cpp::IOException&)
		{
			close();
			return;
		}

		if (bytesAvailable == 0)
		{
			return;
		}

		const size_t oldSize = m_internalReadBuffer.size();

		if ((MaxInternalReadBufferBytes < oldSize) || ((MaxInternalReadBufferBytes - oldSize) < bytesAvailable))
		{
			throw Error{ "Serial internal read buffer overflow: no newline received for too long (exceeded 64KB). "
						 "Check sender protocol / line termination, or consume the stream with read()." };
		}

		m_internalReadBuffer.resize(oldSize + bytesAvailable);

		try
		{
			m_serial.read(reinterpret_cast<uint8*>(m_internalReadBuffer.data() + oldSize), bytesAvailable);
		}
		catch (const serial_cpp::IOException&)
		{
			close();
			return;
		}
	}
}
