//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/FileSystem.hpp>
# include "ChildProcessDetail.hpp"
# include <boost/process/io.hpp>
# include <boost/process/start_dir.hpp>
# include <boost/process/search_path.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static std::wstring GetPath(const FilePathView path)
		{
			if (FileSystem::Exists(path))
			{
				return path.toWstr();
			}
			else
			{
				return boost::process::search_path(path.toWstr()).wstring();
			}
		}
	}

	ChildProcess::ChildProcessDetail::ChildProcessDetail() {}

	ChildProcess::ChildProcessDetail::ChildProcessDetail(const FilePathView path, const Array<String>& commands, const Pipe pipe)
	{
		const std::wstring pathW = GetPath(path);
		
		std::vector<std::wstring> args = { pathW };

		for (const auto& command : commands)
		{
			args.push_back(command.toWstr());
		}

		switch (pipe)
		{
		case Pipe::None:
			m_child = boost::process::child(args);
			break;
		case Pipe::StdIn:
			m_child = boost::process::child(args, boost::process::std_out > m_iPipeStream);
			break;
		case Pipe::StdOut:
			m_child = boost::process::child(args, boost::process::std_in < m_oPipeStream);
			break;
		case Pipe::StdInOut:
			m_child = boost::process::child(args, boost::process::std_out > m_iPipeStream, boost::process::std_in < m_oPipeStream);
			break;
		}

		m_launched = m_child.valid();
	}

	ChildProcess::ChildProcessDetail::~ChildProcessDetail()
	{
		if (m_child.joinable())
		{
			m_child.detach();
		}
	}

	bool ChildProcess::ChildProcessDetail::isValid() const
	{
		return m_child.valid();
	}

	bool ChildProcess::ChildProcessDetail::isRunning()
	{
		return m_child.running();
	}

	void ChildProcess::ChildProcessDetail::wait()
	{
		m_child.wait();
	}

	void ChildProcess::ChildProcessDetail::terminate()
	{
		m_child.terminate();
	}

	Optional<int32> ChildProcess::ChildProcessDetail::getExitCode()
	{
		if ((not m_launched) ||
			m_child.running())
		{
			return none;
		}
		else
		{
			return m_child.exit_code();
		}
	}

	std::istream& ChildProcess::ChildProcessDetail::istream()
	{
		return m_iPipeStream;
	}

	std::ostream& ChildProcess::ChildProcessDetail::ostream()
	{
		return m_oPipeStream;
	}
}
