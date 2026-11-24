//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(WINDOWS)
# include "WindowFullscreenTestHelper.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Window/CWindow.hpp>

namespace s3d::Test::Fullscreen
{
	inline HWND MainWindow()
	{
		return static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());
	}

	inline void AltEnter()
	{
		// Exercise the real window message route without synthesizing global keyboard input.
		::SendMessageW(MainWindow(), WM_SYSKEYDOWN, VK_RETURN, (LPARAM{ 1 } << 29));
		Test::UpdateWithPause();
	}

	inline void ReportWindow(const char* phase)
	{
		const auto& window = Window::GetState();
		Catch::cout() << phase << ": monitor=" << System::GetCurrentMonitorIndex()
			<< " dpi=" << ::GetDpiForWindow(MainWindow()) << " scaling=" << window.scaling
			<< " fullscreen=" << window.fullscreen << " bounds=" << window.bounds
			<< " virtual=" << window.virtualSize << " framebuffer=" << window.frameBufferSize
			<< " scene=" << Scene::Size() << " render=" << Scene::RenderResolution()
			<< " mode=" << Format(Scene::GetConfig().sizeMode).toUTF8()
			<< " highResolution=" << (Scene::GetConfig().resolution == SceneResolution::Display)
			<< " fullscreenSizing=" << Format(Scene::GetConfig().fullscreen).toUTF8()
			<< " filter=" << Format(Scene::GetConfig().scalingFilter).toUTF8() << '\n';
	}

	using ResizeFunction = bool (*)(Size, CenterWindow);
	inline constexpr ResizeFunction ResizeFunctions[] = { Window::Resize, Window::ResizeVirtual, Window::ResizeActual };
	inline constexpr const char* ResizeNames[] = { "Resize", "ResizeVirtual", "ResizeActual" };

	inline Array<std::pair<size_t, size_t>> MixedDPIRoutes(const Array<MonitorInfo>& monitors)
	{
		REQUIRE_FALSE(monitors.isEmpty());
		Array<std::pair<size_t, size_t>> routes;
		for (size_t i = 0; i < monitors.size(); ++i)
		{
			Catch::cout() << "monitor[" << i << "] " << monitors[i].format().toUTF8() << '\n';
			for (size_t j = i + 1; j < monitors.size(); ++j)
			{
				if (routes.isEmpty() && monitors[i].scaling && monitors[j].scaling
					&& (monitors[i].scaling != monitors[j].scaling))
				{
					// Select one pair in both directions, without a display Cartesian product.
					routes.emplace_back(i, j);
					routes.emplace_back(j, i);
				}
			}
		}
		if (routes.isEmpty())
		{
			WARN("No mixed-DPI monitor pair: only same-monitor resize coverage is available.");
			const size_t monitor = System::GetCurrentMonitorIndex();
			routes.emplace_back(monitor, monitor);
		}
		return routes;
	}
}

# endif
