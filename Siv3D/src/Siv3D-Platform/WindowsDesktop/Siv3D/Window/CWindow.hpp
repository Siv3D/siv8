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

# pragma once
# include <atomic>
# include <Siv3D/WindowState.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Windows/ComPtr.hpp>
# include <ShObjIdl_core.h> // ITaskbarList3
# include "User32Lib.hpp"
# include "WindowClass.hpp"
# include "WindowTitle.hpp"
# include "WindowStateSync.hpp"
# include "WindowProc.hpp"

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

		void setTaskbarProgressBar(double progress0_1) override;



		void destroy();

		void attach(HWND hWnd) noexcept;

		void beginStateUpdate() noexcept;

		void endStateUpdate();

		void dispatchCommand(WindowThreadCommand& command);

		const WindowState& getOSState() const noexcept;

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

		// All native geometry and policy below belong to the window thread.
		WindowState m_osState;

		WindowStateSync m_stateSync;

		WindowThreadCommand* m_deferredCommands = nullptr;

		int32 m_dpi = USER_DEFAULT_SCREEN_DPI;

		Size m_border{ 0, 0 };

		/// @brief フルスクリーンからウィンドウモードに復帰するときのウィンドウサイズ
		RECT m_storedWindowRect{ 0, 0, 0, 0 };

		bool m_fullscreenTransition = false;

		enum class ToggleFullscreenState : uint8
		{
			Disabled,
			Enabled,
			Requested,
		};

		std::atomic<ToggleFullscreenState> m_toggleFullscreenState = ToggleFullscreenState::Enabled;

		HDEVNOTIFY m_deviceNotificationHandle = nullptr;

		ComPtr<ITaskbarList3> m_taskbarList;

		bool m_windowShown = false;

		int32 getSystemMetrics(int32 index) const;

		void setWindowPos(const Rect& rect, uint32 flags);

		void showOnWindowThread();

		void setStyleOnWindowThread(WindowStyle style);

		void setPosOnWindowThread(Point pos);

		bool resizeOnWindowThread(Size size, bool virtualSize);

		bool setFullscreenOnWindowThread(bool fullscreen, size_t monitorIndex);

		bool resize(Size size, bool virtualSize);

		// No shared mutex is held while Win32 synchronously dispatches/reenters.
		template <class F>
		void invokeOnWindowThread(F function)
		{
			WindowThreadCommand command{ [](void* context) { (*static_cast<F*>(context))(); }, &function, {} };
			::SendMessageW(m_hWnd, WindowThreadMessage, 0, reinterpret_cast<LPARAM>(&command));
			command.wait();
			m_stateSync.synchronize();
			if (command.exception) { std::rethrow_exception(command.exception); }
		}
	};
}
