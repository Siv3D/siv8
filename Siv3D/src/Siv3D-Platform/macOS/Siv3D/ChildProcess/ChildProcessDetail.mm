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

# include "ChildProcessDetail.hpp"
# include "ChildProcessHandle.hpp"

namespace s3d
{
	ChildProcess::ChildProcessDetail::ChildProcessDetail()
		: m_handle{ std::make_unique<ChildProcessHandle>() } {}

	ChildProcess::ChildProcessDetail::ChildProcessDetail(const FilePathView path, const Array<String>& commands, const ChildProcessPipe pipe)
		: m_handle{ std::make_unique<ChildProcessHandle>(path.toUTF8(), commands, pipe) } {}

	ChildProcess::ChildProcessDetail::~ChildProcessDetail() = default;

	bool ChildProcess::ChildProcessDetail::isValid() const
	{
		return m_handle->isValid();
	}

	bool ChildProcess::ChildProcessDetail::isRunning()
	{
		return m_handle->isRunning();
	}

	void ChildProcess::ChildProcessDetail::wait()
	{
		m_handle->wait();
	}

	void ChildProcess::ChildProcessDetail::terminate()
	{
		m_handle->terminate();
	}

	Optional<int32> ChildProcess::ChildProcessDetail::getExitCode()
	{
		return m_handle->getExitCode();
	}

	std::istream& ChildProcess::ChildProcessDetail::istream()
	{
		return m_handle->istream();
	}

	std::ostream& ChildProcess::ChildProcessDetail::ostream()
	{
		return m_handle->ostream();
	}
}
