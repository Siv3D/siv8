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

# include "ExitCode.hpp"

namespace s3d
{
	int32 g_exitCode = 0;
	
	void SetExitCode(const int32 exitCode) noexcept
	{
		g_exitCode = exitCode;
	}

	int32 GetExitCode() noexcept
	{
		return g_exitCode;
	}
}
