//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/ChildProcess.hpp>
# include <boost/process/child.hpp>

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4244)
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4297)
# include <boost/process/pipe.hpp>
SIV3D_DISABLE_MSVC_WARNINGS_POP()
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	class ChildProcess::ChildProcessDetail
	{
	public:

		ChildProcessDetail();

		ChildProcessDetail(const FilePathView path, const Array<String>& commands, Pipe pipe);

		~ChildProcessDetail();

		bool isValid() const;

		bool isRunning();

		void wait();

		void terminate();

		Optional<int32> getExitCode();

		std::istream& istream();

		std::ostream& ostream();

	private:

		boost::process::opstream m_oPipeStream;

		boost::process::ipstream m_iPipeStream;

		boost::process::child m_child;

		bool m_launched = false;
	};
}
