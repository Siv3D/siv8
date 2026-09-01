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
# include <array>
# include <istream>
# include <ostream>
# include <streambuf>
# include <string>
# include <string_view>
# include <sys/types.h>
# include <Siv3D/ChildProcess.hpp>

namespace s3d
{
	class InputPipeStreamBuffer final : public std::streambuf
	{
	public:

		InputPipeStreamBuffer();

		~InputPipeStreamBuffer() override;

		void attach(int fileDescriptor);

		void close();

	protected:

		int_type underflow() override;

	private:

		static constexpr size_t BufferSize = 4096;

		std::array<char, BufferSize> m_buffer{};

		int m_fileDescriptor = -1;
	};

	class OutputPipeStreamBuffer final : public std::streambuf
	{
	public:

		OutputPipeStreamBuffer();

		~OutputPipeStreamBuffer() override;

		void attach(int fileDescriptor);

		void close();

	protected:

		int_type overflow(int_type ch) override;

		int sync() override;

	private:

		[[nodiscard]]
		bool flushBuffer();

		static constexpr size_t BufferSize = 4096;

		std::array<char, BufferSize> m_buffer{};

		int m_fileDescriptor = -1;
	};

	class ChildProcessHandle
	{
	public:

		ChildProcessHandle();

		ChildProcessHandle(const std::string& path, const Array<String>& commands, ChildProcessPipe pipe);

		~ChildProcessHandle();

		[[nodiscard]]
		bool isValid() const;

		bool isRunning();

		void wait();

		void terminate();

		Optional<int32> getExitCode();

		std::istream& istream();

		std::ostream& ostream();

	private:

		[[nodiscard]]
		bool updateStatus(bool block);

		void storeExitStatus(int status);

		InputPipeStreamBuffer m_inputBuffer;

		OutputPipeStreamBuffer m_outputBuffer;

		std::istream m_inputStream;

		std::ostream m_outputStream;

		pid_t m_processID = -1;

		bool m_valid = false;

		bool m_reaped = false;

		Optional<int32> m_exitCode;
	};
}
