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
# include <Siv3D/Error.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include "WindowUtility.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer/IRenderer.hpp>

namespace s3d
{
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
			const bool centered = WindowMisc::CenterWindowOnCurrentScreen();
			SIV3D_ENGINE(Renderer)->updateSceneSize();
			return centered;
		}

		bool CenterToScreen(const size_t monitorIndex)
		{
			const bool centered = WindowMisc::CenterWindowOnScreen(monitorIndex);
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
				WindowMisc::CenterWindowOnCurrentScreen();
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
				WindowMisc::CenterWindowOnCurrentScreen();
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
}
