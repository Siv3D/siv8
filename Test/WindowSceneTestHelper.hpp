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

namespace s3d::Test
{
	// Capture values only; each test owns its restoration sequence.
	struct WindowSceneSnapshot
	{
		const WindowState window = Window::GetState();
		const SceneConfig config = Scene::GetConfig();
	};

	inline void PauseForWindowTransition()
	{
		// Avoid rapid desktop flashing without advancing another frame.
		System::Sleep(500);
	}

	inline void SetFullscreenWithPause(const bool fullscreen, const size_t monitorIndex = System::GetCurrentMonitorIndex())
	{
		Window::SetFullscreen(fullscreen, monitorIndex);
		PauseForWindowTransition();
	}

	inline void UpdateWithPause()
	{
		REQUIRE(System::Update());
		PauseForWindowTransition();
	}

	enum class WindowSizeUnit
	{
		Virtual,
		FrameBuffer,
	};

	enum class PlacementOrder
	{
		PositionThenSize,
		SizeThenPosition,
	};

	inline void RestorePlacement(const WindowState& window, const WindowSizeUnit unit, const PlacementOrder order)
	{
		const auto resize = [&]
		{
			if (unit == WindowSizeUnit::Virtual)
			{
				Window::Resize(window.virtualSize, CenterWindow::No);
			}
			else
			{
				Window::ResizeActual(window.frameBufferSize, CenterWindow::No);
			}
		};

		if (order == PlacementOrder::PositionThenSize)
		{
			Window::SetPos(window.bounds.pos);
			resize();
		}
		else
		{
			resize();
			Window::SetPos(window.bounds.pos);
		}
	}
}
