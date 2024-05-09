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
# include <Siv3D/Window.hpp>
# include <Siv3D/Stopwatch.hpp>
# include <Siv3D/WindowState.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/GLFW/GLFW.hpp>
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

	private:

		GLFWwindow* m_glfwWindow = nullptr;
		
		WindowTitle m_windowTitle;
		
		WindowState m_state;
		
		Stopwatch m_moveResizeStabilizer;
		
		Rect m_storedWindowRect = { 0, 0, 0, 0 };

		void updateState();
		
		static void OnMove(GLFWwindow* glfwWindow, int x, int y);
		
		static void OnResize(GLFWwindow* glfwWindow, int width, int height);
		
		static void OnFrameBufferSize(GLFWwindow* glfwWindow, int width, int height);
		
		static void OnScalingChange(GLFWwindow* glfwWindow, float sx, float sy);
		
		static void OnIconify(GLFWwindow* glfwWindow, int iconified);
		
		static void OnMaximize(GLFWwindow* glfwWindow, int maximized);
		
		static void OnFocus(GLFWwindow* glfwWindow, int focused);
	};
}
