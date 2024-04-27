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

# include <Siv3D/Window.hpp>
# include <Siv3D/WindowState.hpp>
# include <Siv3D/MonitorInfo.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/Window/IWindow.hpp>
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

		static void CenterToOtherMonitor(const MonitorInfo& currentMonitor, const MonitorInfo& nextMonitor)
		{
			if (currentMonitor.scaling == nextMonitor.scaling)
			{
				CenterToMonitor(nextMonitor);
			}
			else
			{
				const Rect workArea = nextMonitor.workArea;
				SIV3D_ENGINE(Window)->setPos(workArea.pos); // 先に bounds を更新してから計算する
				const Rect windowBounds = SIV3D_ENGINE(Window)->getState().bounds;
				const Point pos = (workArea.pos + (workArea.size - windowBounds.size) / 2);
				SIV3D_ENGINE(Window)->setPos(pos);
			}
		}
	}

	namespace Window
	{
		////////////////////////////////////////////////////////////////
		//
		//	SetTitle
		//
		////////////////////////////////////////////////////////////////

		void SetTitle(const String& title)
		{
			SIV3D_ENGINE(Window)->setWindowTitle(title);
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetTitle
		//
		////////////////////////////////////////////////////////////////

		const String& GetTitle() noexcept
		{
			return SIV3D_ENGINE(Window)->getWindowTitle();
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetState
		//
		////////////////////////////////////////////////////////////////

		const WindowState& GetState() noexcept
		{
			return SIV3D_ENGINE(Window)->getState();
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetStyle
		//
		////////////////////////////////////////////////////////////////

		void SetStyle(const WindowStyle style)
		{
			SIV3D_ENGINE(Window)->setStyle(style);
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetStyle
		//
		////////////////////////////////////////////////////////////////

		WindowStyle GetStyle() noexcept
		{
			return SIV3D_ENGINE(Window)->getState().style;
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetPos
		//
		////////////////////////////////////////////////////////////////

		void SetPos(const Point screenPos)
		{
			SIV3D_ENGINE(Window)->setPos(screenPos);
		}

		void SetPos(const int32 screenX, const int32 screenY)
		{
			SetPos(Point{ screenX, screenY });
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetPos
		//
		////////////////////////////////////////////////////////////////

		Point GetPos() noexcept
		{
			return SIV3D_ENGINE(Window)->getState().bounds.pos;
		}

		////////////////////////////////////////////////////////////////
		//
		//	CenterToScreen
		//
		////////////////////////////////////////////////////////////////

		bool CenterToScreen()
		{
			SIV3D_ENGINE(Window)->show();

			try
			{
				CenterToMonitor(System::GetCurrentMonitor());

				return true;
			}
			catch (const Error&)
			{
				return false;
			}
		}

		bool CenterToScreen(const size_t monitorIndex)
		{
			SIV3D_ENGINE(Window)->show();

			const Array<MonitorInfo> monitors = System::EnumerateMonitors();

			if (monitors.size() <= monitorIndex)
			{
				return false;
			}

			const size_t currentMonitorIndex = System::GetCurrentMonitorIndex();

			if (currentMonitorIndex == monitorIndex)
			{
				CenterToMonitor(monitors[monitorIndex]);
			}
			else
			{
				CenterToOtherMonitor(monitors[currentMonitorIndex], monitors[monitorIndex]);
			}

			return true;
		}
		////////////////////////////////////////////////////////////////
		//
		//	Maximize
		//
		////////////////////////////////////////////////////////////////

		void Maximize()
		{
			SIV3D_ENGINE(Window)->maximize();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Restore
		//
		////////////////////////////////////////////////////////////////

		void Restore()
		{
			SIV3D_ENGINE(Window)->restore();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Minimize
		//
		////////////////////////////////////////////////////////////////

		void Minimize()
		{
			SIV3D_ENGINE(Window)->minimize();
		}
	}
}
