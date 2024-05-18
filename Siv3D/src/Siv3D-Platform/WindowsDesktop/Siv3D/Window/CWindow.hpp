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
# include <atomic>
# include <Siv3D/WindowState.hpp>
# include <Siv3D/ResizeMode.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Windows/Windows.hpp>
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

		const WindowState& getState() const noexcept override;

		void setStyle(WindowStyle style) override;

		void setPos(Point pos) override;

		void show() override;

		void maximize() override;

		void restore() override;

		void minimize() override;

		bool resizeByVirtualSize(Size virtualSize) override;

		bool resizeByFrameBufferSize(Size frameBufferSize) override;

		void setMinimumFrameBufferSize(Size size) override;

		void setFullscreen(bool fullscreen, size_t monitorIndex) override;

		void setToggleFullscreenEnabled(bool enabled) override;

		bool isToggleFullscreenEnabled() const override;



		void destroy();


		void onResize(bool minimized, bool maximized);

		void onFocus(bool focused);

		void onFrameBufferResize(Size size);

		void onBoundsUpdate();

		void onDPIChange(uint32 dpi, Point suggestedPos);

		void onEnterSizeMove();

		void onExitSizeMove();

		Size getMinTrackSize() noexcept;

		void requestToggleFullscreen();

	private:

		HWND m_hWnd = nullptr;

		HMODULE m_moduleHandle = nullptr;

		User32Lib m_user32;

		WindowClass m_windowClass;

		WindowTitle m_windowTitle;

		WindowState m_state;

		int32 m_dpi = USER_DEFAULT_SCREEN_DPI;

		Size m_border{ 0, 0 };

		/// @brief フルスクリーンからウィンドウモードに復帰するときのウィンドウサイズ
		RECT m_storedWindowRect{ 0, 0, 0, 0 };

		Optional<ResizeMode> m_oldResizeMode;

		bool m_toggleFullscreenEnabled = true;

		std::atomic<bool> m_toggleFullscreenRequested = false;

		std::atomic<bool> m_moving = false;

		std::mutex m_minimumFrameBufferSizeMutex;

		HDEVNOTIFY m_deviceNotificationHandle = nullptr;

		ComPtr<ITaskbarList3> m_taskbarList;

		bool m_windowShown = false;

		int32 getSystemMetrics(int32 index) const;

		void setWindowPos(const Rect& rect, uint32 flags);

		void setFullscreen(bool fullscreen, size_t monitorIndex, bool skipSceneResize);
	};
}
