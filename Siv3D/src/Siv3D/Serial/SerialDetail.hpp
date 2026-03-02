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
# include <Siv3D/Serial.hpp>
# include <ThirdParty/serial_cpp/serial.h>

namespace s3d
{
	class Serial::SerialDetail
	{
	public:

		bool open(StringView port, int32 baudrate, const Config& config);

		void close();

		bool isOpen();

		int32 baudrate() const noexcept;

		bool setBaudrate(int32 baudrate);

		bool setConfig(const Config& config);

		const String& port() const noexcept;

		size_t available();

		void clearInput();

		void clearOutput();

		void clear();

		size_t read(void* dst, size_t size);

		bool readLine(String& line);

		size_t write(const void* src, size_t size);

		void setBreak(bool level);

		void setRTS(bool level);

		void setDTR(bool level);

		bool waitForChange();

		bool getCTS();

		bool getDSR();

		bool getRI();

		bool getCD();

	private:

		static constexpr size_t MaxInternalReadBufferBytes = (64 * 1024);

		serial_cpp::Serial m_serial;

		String m_port;

		int32 m_baudrate = 0;

		Array<char> m_internalReadBuffer;

		void fillInternalReadBuffer();
	};
}
