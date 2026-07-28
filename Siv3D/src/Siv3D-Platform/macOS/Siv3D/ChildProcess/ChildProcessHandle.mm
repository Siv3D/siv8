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

# include "ChildProcessHandle.hpp"
# include <cerrno>
# include <csignal>
# include <fcntl.h>
# include <spawn.h>
# include <thread>
# include <unistd.h>
# include <vector>
# include <sys/stat.h>
# include <sys/wait.h>

extern char** environ;

namespace s3d
{
	namespace
	{
		void CloseFileDescriptor(int& fileDescriptor)
		{
			if (fileDescriptor < 0)
			{
				return;
			}

			while ((::close(fileDescriptor) < 0) && (errno == EINTR))
			{
			}

			fileDescriptor = -1;
		}

		[[nodiscard]]
		bool CreatePipe(int fileDescriptors[2])
		{
			if (::pipe(fileDescriptors) < 0)
			{
				return false;
			}

			for (size_t i = 0; i < 2; ++i)
			{
				int& fileDescriptor = fileDescriptors[i];

				if (fileDescriptor <= STDERR_FILENO)
				{
					const int duplicatedFileDescriptor = ::fcntl(fileDescriptor, F_DUPFD_CLOEXEC, (STDERR_FILENO + 1));

					if (duplicatedFileDescriptor < 0)
					{
						CloseFileDescriptor(fileDescriptors[0]);
						CloseFileDescriptor(fileDescriptors[1]);
						return false;
					}

					CloseFileDescriptor(fileDescriptor);
					fileDescriptor = duplicatedFileDescriptor;
				}
				else
				{
					if (::fcntl(fileDescriptor, F_SETFD, FD_CLOEXEC) < 0)
					{
						CloseFileDescriptor(fileDescriptors[0]);
						CloseFileDescriptor(fileDescriptors[1]);
						return false;
					}
				}
			}

			return true;
		}

		[[nodiscard]]
		bool IsApplicationBundle(const std::string& path)
		{
			const size_t lastCharacter = path.find_last_not_of('/');

			if (lastCharacter == std::string::npos)
			{
				return false;
			}

			constexpr std::string_view Extension = ".app";
			const size_t length = (lastCharacter + 1);

			return ((Extension.size() <= length)
				&& (path.compare((length - Extension.size()), Extension.size(), Extension) == 0));
		}

		void ReapProcess(const pid_t processID)
		{
			int status = 0;

			while ((::waitpid(processID, &status, 0) < 0) && (errno == EINTR))
			{
			}
		}
	}

	InputPipeStreamBuffer::InputPipeStreamBuffer()
	{
		setg(m_buffer.data(), m_buffer.data(), m_buffer.data());
	}

	InputPipeStreamBuffer::~InputPipeStreamBuffer()
	{
		close();
	}

	void InputPipeStreamBuffer::attach(const int fileDescriptor)
	{
		close();
		m_fileDescriptor = fileDescriptor;
		setg(m_buffer.data(), m_buffer.data(), m_buffer.data());
	}

	void InputPipeStreamBuffer::close()
	{
		CloseFileDescriptor(m_fileDescriptor);
		setg(m_buffer.data(), m_buffer.data(), m_buffer.data());
	}

	InputPipeStreamBuffer::int_type InputPipeStreamBuffer::underflow()
	{
		if (m_fileDescriptor < 0)
		{
			return traits_type::eof();
		}

		ssize_t readSize = 0;

		do
		{
			readSize = ::read(m_fileDescriptor, m_buffer.data(), m_buffer.size());
		}
		while ((readSize < 0) && (errno == EINTR));

		if (readSize <= 0)
		{
			close();
			return traits_type::eof();
		}

		setg(m_buffer.data(), m_buffer.data(), (m_buffer.data() + readSize));
		return traits_type::to_int_type(*gptr());
	}

	OutputPipeStreamBuffer::OutputPipeStreamBuffer()
	{
		setp(m_buffer.data(), (m_buffer.data() + m_buffer.size()));
	}

	OutputPipeStreamBuffer::~OutputPipeStreamBuffer()
	{
		close();
	}

	void OutputPipeStreamBuffer::attach(const int fileDescriptor)
	{
		close();
		m_fileDescriptor = fileDescriptor;
		setp(m_buffer.data(), (m_buffer.data() + m_buffer.size()));
	}

	void OutputPipeStreamBuffer::close()
	{
		if (m_fileDescriptor < 0)
		{
			return;
		}

		(void)flushBuffer();
		CloseFileDescriptor(m_fileDescriptor);
	}

	OutputPipeStreamBuffer::int_type OutputPipeStreamBuffer::overflow(const int_type ch)
	{
		if (not flushBuffer())
		{
			return traits_type::eof();
		}

		if (not traits_type::eq_int_type(ch, traits_type::eof()))
		{
			*pptr() = traits_type::to_char_type(ch);
			pbump(1);
		}

		return traits_type::not_eof(ch);
	}

	int OutputPipeStreamBuffer::sync()
	{
		return (flushBuffer() ? 0 : -1);
	}

	bool OutputPipeStreamBuffer::flushBuffer()
	{
		const auto dataSize = static_cast<size_t>(pptr() - pbase());

		if (dataSize == 0)
		{
			return true;
		}

		if (m_fileDescriptor < 0)
		{
			setp(m_buffer.data(), (m_buffer.data() + m_buffer.size()));
			return false;
		}

		size_t writtenSize = 0;

		while (writtenSize < dataSize)
		{
			const ssize_t result = ::write(m_fileDescriptor, (m_buffer.data() + writtenSize), (dataSize - writtenSize));

			if (0 < result)
			{
				writtenSize += static_cast<size_t>(result);
			}
			else if ((result < 0) && (errno == EINTR))
			{
				continue;
			}
			else
			{
				setp(m_buffer.data(), (m_buffer.data() + m_buffer.size()));
				return false;
			}
		}

		setp(m_buffer.data(), (m_buffer.data() + m_buffer.size()));
		return true;
	}

	ChildProcessHandle::ChildProcessHandle()
		: m_inputStream{ &m_inputBuffer }
		, m_outputStream{ &m_outputBuffer } {}

	ChildProcessHandle::ChildProcessHandle(const std::string& path, const Array<String>& commands, const Pipe pipe)
		: ChildProcessHandle{}
	{
		if (path.empty())
		{
			return;
		}

		const bool isApplicationBundle = IsApplicationBundle(path);

		if (isApplicationBundle && (pipe != Pipe::None))
		{
			return;
		}

		if (isApplicationBundle)
		{
			struct stat status{};

			if ((::stat(path.c_str(), &status) < 0) || (not S_ISDIR(status.st_mode)))
			{
				return;
			}
		}

		std::string executablePath = path;
		std::vector<std::string> arguments;

		if (isApplicationBundle)
		{
			executablePath = "/usr/bin/open";
			arguments.emplace_back("open");
			arguments.push_back(path);

			if (not commands.empty())
			{
				arguments.emplace_back("--args");
			}
		}
		else
		{
			arguments.push_back(path);
		}

		for (const auto& command : commands)
		{
			arguments.push_back(command.toUTF8());
		}

		std::vector<char*> argumentPointers;
		argumentPointers.reserve(arguments.size() + 1);

		for (auto& argument : arguments)
		{
			argumentPointers.push_back(argument.data());
		}

		argumentPointers.push_back(nullptr);

		const bool captureStandardOutput = ((pipe == Pipe::StdIn) || (pipe == Pipe::StdInOut));
		const bool provideStandardInput = ((pipe == Pipe::StdOut) || (pipe == Pipe::StdInOut));
		int standardOutputPipe[2] = { -1, -1 };
		int standardInputPipe[2] = { -1, -1 };

		if (captureStandardOutput && (not CreatePipe(standardOutputPipe)))
		{
			return;
		}

		if (provideStandardInput && (not CreatePipe(standardInputPipe)))
		{
			CloseFileDescriptor(standardOutputPipe[0]);
			CloseFileDescriptor(standardOutputPipe[1]);
			return;
		}

		posix_spawn_file_actions_t fileActions;
		int error = ::posix_spawn_file_actions_init(&fileActions);
		const bool fileActionsInitialized = (error == 0);

		if ((error == 0) && captureStandardOutput)
		{
			error = ::posix_spawn_file_actions_adddup2(&fileActions, standardOutputPipe[1], STDOUT_FILENO);
		}

		if ((error == 0) && captureStandardOutput)
		{
			error = ::posix_spawn_file_actions_addclose(&fileActions, standardOutputPipe[0]);
		}

		if ((error == 0) && captureStandardOutput)
		{
			error = ::posix_spawn_file_actions_addclose(&fileActions, standardOutputPipe[1]);
		}

		if ((error == 0) && provideStandardInput)
		{
			error = ::posix_spawn_file_actions_adddup2(&fileActions, standardInputPipe[0], STDIN_FILENO);
		}

		if ((error == 0) && provideStandardInput)
		{
			error = ::posix_spawn_file_actions_addclose(&fileActions, standardInputPipe[1]);
		}

		if ((error == 0) && provideStandardInput)
		{
			error = ::posix_spawn_file_actions_addclose(&fileActions, standardInputPipe[0]);
		}

		pid_t processID = -1;

		if (error == 0)
		{
			error = ::posix_spawnp(&processID, executablePath.c_str(), &fileActions, nullptr,
				argumentPointers.data(), environ);
		}

		if (fileActionsInitialized)
		{
			::posix_spawn_file_actions_destroy(&fileActions);
		}

		CloseFileDescriptor(standardOutputPipe[1]);
		CloseFileDescriptor(standardInputPipe[0]);

		if (error != 0)
		{
			CloseFileDescriptor(standardOutputPipe[0]);
			CloseFileDescriptor(standardInputPipe[1]);
			return;
		}

		m_processID = processID;
		m_valid = true;

		if (captureStandardOutput)
		{
			m_inputBuffer.attach(standardOutputPipe[0]);
			standardOutputPipe[0] = -1;
		}

		if (provideStandardInput)
		{
			::fcntl(standardInputPipe[1], F_SETNOSIGPIPE, 1);
			m_outputBuffer.attach(standardInputPipe[1]);
			standardInputPipe[1] = -1;
		}
	}

	ChildProcessHandle::~ChildProcessHandle()
	{
		m_outputBuffer.close();
		m_inputBuffer.close();

		if (m_valid && updateStatus(false))
		{
			const pid_t processID = m_processID;

			try
			{
				std::thread{ ReapProcess, processID }.detach();
			}
			catch (...)
			{
			}
		}
	}

	bool ChildProcessHandle::isValid() const
	{
		return m_valid;
	}

	bool ChildProcessHandle::isRunning()
	{
		return (m_valid && updateStatus(false));
	}

	void ChildProcessHandle::wait()
	{
		if (m_valid)
		{
			(void)updateStatus(true);
		}
	}

	void ChildProcessHandle::terminate()
	{
		if (m_valid && updateStatus(false))
		{
			::kill(m_processID, SIGTERM);
		}
	}

	Optional<int32> ChildProcessHandle::getExitCode()
	{
		if (m_valid)
		{
			(void)updateStatus(false);
		}

		return m_exitCode;
	}

	std::istream& ChildProcessHandle::istream()
	{
		return m_inputStream;
	}

	std::ostream& ChildProcessHandle::ostream()
	{
		return m_outputStream;
	}

	bool ChildProcessHandle::updateStatus(const bool block)
	{
		if (m_reaped)
		{
			return false;
		}

		int status = 0;
		pid_t result = -1;

		do
		{
			result = ::waitpid(m_processID, &status, (block ? 0 : WNOHANG));
		}
		while ((result < 0) && (errno == EINTR));

		if (result == 0)
		{
			return true;
		}

		m_reaped = true;

		if (result == m_processID)
		{
			storeExitStatus(status);
		}

		return false;
	}

	void ChildProcessHandle::storeExitStatus(const int status)
	{
		if (WIFEXITED(status))
		{
			m_exitCode = WEXITSTATUS(status);
		}
		else if (WIFSIGNALED(status))
		{
			m_exitCode = WTERMSIG(status);
		}
	}
}
