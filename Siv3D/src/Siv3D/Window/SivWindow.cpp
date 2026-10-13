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

# include <Siv3D/Window.hpp>
# include <Siv3D/WindowState.hpp>
# include <Siv3D/MonitorInfo.hpp>
# if SIV3D_PLATFORM(WINDOWS)
# include <Siv3D/Window/WindowMisc.hpp>
# endif
# include <Siv3D/Error.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer/IRenderer.hpp>

namespace s3d
{
	namespace
	{
# if !SIV3D_PLATFORM(WINDOWS)
		static void CenterToMonitor(const MonitorInfo& monitor)
		{
			const Rect workArea = monitor.workArea;
			const Rect windowBounds = SIV3D_ENGINE(Window)->getState().bounds;
			const Point pos = (workArea.pos + (workArea.size - windowBounds.size) / 2);
			SIV3D_ENGINE(Window)->setPos(pos);
		}

# if !SIV3D_PLATFORM(MACOS)
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
# endif
# endif
		bool CenterWindowOnCurrentScreen()
		{
# if SIV3D_PLATFORM(WINDOWS)
			return WindowMisc::CenterToScreen(*SIV3D_ENGINE(Window), none);
# else
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
# endif
		}

		bool CenterWindowOnScreen(const size_t monitorIndex)
		{
# if SIV3D_PLATFORM(WINDOWS)
			return WindowMisc::CenterToScreen(*SIV3D_ENGINE(Window), monitorIndex);
# else
			SIV3D_ENGINE(Window)->show();
			if (SIV3D_ENGINE(Window)->getState().fullscreen) { return false; }

			const Array<MonitorInfo> monitors = System::EnumerateMonitors();

			if (monitors.size() <= monitorIndex)
			{
				return false;
			}

# if SIV3D_PLATFORM(MACOS)
			// Outer-frame dimensions stay in points across backing-scale changes.
			CenterToMonitor(monitors[monitorIndex]);
# else
			const size_t currentMonitorIndex = System::GetCurrentMonitorIndex();

			if (currentMonitorIndex == monitorIndex)
			{
				CenterToMonitor(monitors[monitorIndex]);
			}
			else
			{
				CenterToOtherMonitor(monitors[currentMonitorIndex], monitors[monitorIndex]);
			}
# endif

			return true;
# endif
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
			SIV3D_ENGINE(Renderer)->updateSceneSize();
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
			SIV3D_ENGINE(Renderer)->updateSceneSize();
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
			const bool centered = CenterWindowOnCurrentScreen();
			SIV3D_ENGINE(Renderer)->updateSceneSize();
			return centered;
		}

		bool CenterToScreen(const size_t monitorIndex)
		{
			const bool centered = CenterWindowOnScreen(monitorIndex);
			SIV3D_ENGINE(Renderer)->updateSceneSize();
			return centered;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Maximize
		//
		////////////////////////////////////////////////////////////////

		void Maximize()
		{
			SIV3D_ENGINE(Window)->maximize();
			SIV3D_ENGINE(Renderer)->updateSceneSize();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Restore
		//
		////////////////////////////////////////////////////////////////

		void Restore()
		{
			SIV3D_ENGINE(Window)->restore();
			SIV3D_ENGINE(Renderer)->updateSceneSize();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Minimize
		//
		////////////////////////////////////////////////////////////////

		void Minimize()
		{
			SIV3D_ENGINE(Window)->minimize();
			SIV3D_ENGINE(Renderer)->updateSceneSize();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Resize
		//
		////////////////////////////////////////////////////////////////

		bool Resize(const Size size, const CenterWindow centerWindow)
		{
			return ResizeVirtual(size, centerWindow);
		}

		bool Resize(const int32 width, const int32 height, const CenterWindow centerWindow)
		{
			return ResizeVirtual(Size{ width, height }, centerWindow);
		}

		////////////////////////////////////////////////////////////////
		//
		//	ResizeVirtual
		//
		////////////////////////////////////////////////////////////////

		bool ResizeVirtual(const Size size, const CenterWindow centerWindow)
		{
			if ((not InRange(size.x, 1, 8192))
				|| (not InRange(size.y, 1, 8192)))
			{
				throw Error{ "Window::ResizeVirtual(): width and height must be in the range [1, 8192]" };
			}

			const bool resized = SIV3D_ENGINE(Window)->resizeByVirtualSize(size);
			if (resized && centerWindow)
			{
				CenterWindowOnCurrentScreen();
			}
			SIV3D_ENGINE(Renderer)->updateSceneSize();
			return resized;
		}

		bool ResizeVirtual(const int32 width, const int32 height, const CenterWindow centerWindow)
		{
			return ResizeVirtual(Size{ width, height }, centerWindow);
		}

		////////////////////////////////////////////////////////////////
		//
		//	ResizeActual
		//
		////////////////////////////////////////////////////////////////

		bool ResizeActual(const Size size, const CenterWindow centerWindow)
		{
			if ((not InRange(size.x, 1, 8192))
				|| (not InRange(size.y, 1, 8192)))
			{
				throw Error{ "Window::ResizeActual(): width and height must be in the range [1, 8192]" };
			}

			const bool resized = SIV3D_ENGINE(Window)->resizeByFrameBufferSize(size);
			if (resized && centerWindow)
			{
				CenterWindowOnCurrentScreen();
			}
			SIV3D_ENGINE(Renderer)->updateSceneSize();
			return resized;
		}

		bool ResizeActual(const int32 width, const int32 height, const CenterWindow centerWindow)
		{
			return ResizeActual(Size{ width, height }, centerWindow);
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetMinimumFrameBufferSize
		//
		////////////////////////////////////////////////////////////////

		void SetMinimumFrameBufferSize(const Size size)
		{
			SIV3D_ENGINE(Window)->setMinimumFrameBufferSize(size);
			SIV3D_ENGINE(Renderer)->updateSceneSize();
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetFullscreen
		//
		////////////////////////////////////////////////////////////////

		void SetFullscreen(const bool fullscreen, const size_t monitorIndex)
		{
			SIV3D_ENGINE(Window)->setFullscreen(fullscreen, monitorIndex);
			SIV3D_ENGINE(Renderer)->updateSceneSize();
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetToggleFullscreenEnabled
		//
		////////////////////////////////////////////////////////////////

		void SetToggleFullscreenEnabled(const bool enabled)
		{
			SIV3D_ENGINE(Window)->setToggleFullscreenEnabled(enabled);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IsToggleFullscreenEnabled
		//
		////////////////////////////////////////////////////////////////

		bool IsToggleFullscreenEnabled()
		{
			return SIV3D_ENGINE(Window)->isToggleFullscreenEnabled();
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetTaskbarProgressBar
		//
		////////////////////////////////////////////////////////////////

		void SetTaskbarProgressBar(const double progress0_1)
		{
			SIV3D_ENGINE(Window)->setTaskbarProgressBar(progress0_1);
		}
	}

# if SIV3D_PLATFORM(WINDOWS)

	namespace Platform::Windows::Window
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetHWND
		//
		////////////////////////////////////////////////////////////////

		void* GetHWND()
		{
			return SIV3D_ENGINE(Window)->getHandle();
		}
	}

# endif
}
