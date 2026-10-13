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

# include <Siv3D/WindowState.hpp>
# include <Siv3D/MonitorInfo.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Window/WindowUtility.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		static void CenterToMonitor(const MonitorInfo& monitor)
		{
			const Rect workArea = monitor.workArea;
			const Rect windowBounds = SIV3D_ENGINE(Window)->getState().bounds;
			const Point pos = (workArea.pos + (workArea.size - windowBounds.size) / 2);
			SIV3D_ENGINE(Window)->setPos(pos);
		}
	}

	namespace WindowMisc
	{
		bool CenterWindowOnCurrentScreen()
		{
			SIV3D_ENGINE(Window)->show();
			if (SIV3D_ENGINE(Window)->getState().fullscreen) { return false; }

			MonitorInfo monitor;
			try
			{
				monitor = System::GetCurrentMonitor();
			}
			catch (const Error&)
			{
				return false;
			}
			CenterToMonitor(monitor);
			return true;
		}

		bool CenterWindowOnScreen(const size_t monitorIndex)
		{
			SIV3D_ENGINE(Window)->show();
			if (SIV3D_ENGINE(Window)->getState().fullscreen) { return false; }

			const Array<MonitorInfo> monitors = System::EnumerateMonitors();

			if (monitors.size() <= monitorIndex)
			{
				return false;
			}

			// Outer-frame dimensions stay in points across backing-scale changes.
			CenterToMonitor(monitors[monitorIndex]);
			return true;
		}
	}
}
