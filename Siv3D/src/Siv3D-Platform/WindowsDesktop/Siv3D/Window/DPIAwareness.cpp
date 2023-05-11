//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "DPIAwareness.hpp"
# include <ShellScalingApi.h> // SetProcessDpiAwareness()
# include <Siv3D/DLL.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d::detail
{
	void SetDPIAwareness(HMODULE user32)
	{
		LOG_SCOPED_TRACE("SetDPIAwareness()");

		// Windows 10 1703-
		if (decltype(SetProcessDpiAwarenessContext)* p_SetProcessDpiAwarenessContext = DLL::GetFunctionNoThrow(user32, "SetProcessDpiAwarenessContext"))
		{
			LOG_TRACE("SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)");
			p_SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
			return;
		}

		// Windows 10 1607-
		if (decltype(SetThreadDpiAwarenessContext)* p_SetThreadDpiAwarenessContext = DLL::GetFunctionNoThrow(user32, "SetThreadDpiAwarenessContext"))
		{
			LOG_TRACE("SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)");
			p_SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

			LOG_TRACE("SetProcessDPIAware()");
			::SetProcessDPIAware();
			return;
		}

		// Windows 8.1-
		if (LibraryHandle shcore = DLL::LoadSystemLibraryNoThrow(L"shcore.dll"))
		{
			if (decltype(SetProcessDpiAwareness)* p_SetProcessDpiAwareness = DLL::GetFunctionNoThrow(shcore, "SetProcessDpiAwareness"))
			{
				LOG_TRACE("SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE)");
				p_SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
			}

			DLL::Unload(shcore);
		}
	}
}
