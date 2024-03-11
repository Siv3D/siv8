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
# include <Siv3D/WindowState.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Windows/ComPtr.hpp>
# include <Siv3D/Windows/ComPtr.hpp>
# include <ShObjIdl_core.h> // ITaskbarList3
# include "User32Lib.hpp"
# include "WindowClass.hpp"
# include "WindowTitle.hpp"

namespace s3d
{
	class CWindow final : public ISiv3DWindow
	{
	public:

		CWindow() = default;

		~CWindow() override;

		void init() override;

		void update() override;

		void setWindowTitle(const String& title) override;

		const String& getWindowTitle() const noexcept override;

		void* getHandle() const noexcept override;

		void show();

		void destroy();

	private:

		HWND m_hWnd = nullptr;

		HINSTANCE m_hInstance = nullptr;

		User32Lib m_user32;

		WindowClass m_windowClass;

		WindowTitle m_windowTitle;

		WindowState m_state;

		int32 m_dpi = USER_DEFAULT_SCREEN_DPI;

		HDEVNOTIFY m_deviceNotificationHandle = nullptr;

		ComPtr<ITaskbarList3> m_taskbarList;

		bool m_windowShown = false;
	};
}
