//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(WINDOWS)

# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include "../Siv3D/src/Siv3D/Scene/SceneUtility.hpp"

namespace
{
	// Keep real-window transitions visible instead of rapidly flashing the desktop.
	constexpr int32 FullscreenPauseMillis = 500;

	void SetFullscreenWithPause(const bool fullscreen, const size_t monitorIndex = System::GetCurrentMonitorIndex())
	{
		Window::SetFullscreen(fullscreen, monitorIndex);
		System::Sleep(FullscreenPauseMillis);
	}

	void UpdateWithPause()
	{
		REQUIRE(System::Update());
		// Sleep does not consume another frame, preserving deferred-request tests.
		System::Sleep(FullscreenPauseMillis);
	}

	struct RestoreWindowAndScene
	{
		const WindowState window = Window::GetState();
		const Size sceneSize = Scene::Size();
		const ResizeMode mode = Scene::GetResizeMode();
		const bool highResolution = Scene::IsHighResolutionEnabled();
		const TextureFilter filter = Scene::GetTextureFilter();
		const bool toggleEnabled = Window::IsToggleFullscreenEnabled();

		~RestoreWindowAndScene()
		{
			SetFullscreenWithPause(false);
			Window::SetPos(window.bounds.pos);
			Window::Resize(window.virtualSize, CenterWindow::No);
			if (window.maximized) { Window::Maximize(); }
			Scene::SetHighResolutionEnabled(false);
			Scene::SetResizeMode(mode);
			Scene::Resize(sceneSize);
			Scene::SetHighResolutionEnabled(highResolution);
			Scene::SetTextureFilter(filter);
			Window::SetToggleFullscreenEnabled(toggleEnabled);
		}
	};

	HWND MainWindow()
	{
		return static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());
	}

	void AltEnter()
	{
		// Exercise the real window message route without synthesizing global keyboard input.
		::SendMessageW(MainWindow(), WM_SYSKEYDOWN, VK_RETURN, (LPARAM{ 1 } << 29));
		UpdateWithPause();
	}

	void CheckPresentation(const ResizeMode mode, const bool highResolution, const TextureFilter filter,
		const Size fixedSize = Size{ 400, 300 })
	{
		CHECK(Scene::GetResizeMode() == mode);
		CHECK(Scene::IsHighResolutionEnabled() == highResolution);
		CHECK(Scene::GetTextureFilter() == filter);
		const auto& window = Window::GetState();
		const Size expected = (mode == ResizeMode::Keep) ? fixedSize
			: ((mode == ResizeMode::Virtual) ? window.virtualSize : window.frameBufferSize);
		CHECK(Scene::Size() == expected);
		SceneMisc::RenderState reference;
		reference.size = expected;
		reference.frameBufferSize = window.frameBufferSize;
		reference.highResolutionEnabled = highResolution;
		CHECK(Scene::RenderSize() == reference.renderSize());
		const auto [scale, area] = reference.composition();
		CHECK(Cursor::GetBaseWindowTransform() == Mat3x2::Scale(scale).translated(area.pos));
	}
}

TEST_CASE("WindowFullscreen.api_and_shortcut_preserve_presentation", "[.fullscreen]")
{
	REQUIRE_FALSE(Window::GetState().fullscreen);
	const RestoreWindowAndScene restore;
	Window::SetToggleFullscreenEnabled(true);
	UpdateWithPause();
	for (const ResizeMode mode : { ResizeMode::Keep, ResizeMode::Virtual, ResizeMode::Actual })
	{
		for (const bool enabled : { false, true })
		{
			INFO(static_cast<int32>(mode));
			INFO(enabled);
			Scene::SetResizeMode(mode);
			if (mode == ResizeMode::Keep) { Scene::Resize(400, 300); }
			Scene::SetHighResolutionEnabled(enabled);
			Scene::SetTextureFilter(TextureFilter::Nearest);

			AltEnter();
			REQUIRE(Window::GetState().fullscreen);
			CheckPresentation(mode, enabled, TextureFilter::Nearest);
			// Reapplying the current window mode must leave the scene settings intact.
			SetFullscreenWithPause(true);
			CheckPresentation(mode, enabled, TextureFilter::Nearest);
			SetFullscreenWithPause(false);
			REQUIRE_FALSE(Window::GetState().fullscreen);
			CheckPresentation(mode, enabled, TextureFilter::Nearest);
			UpdateWithPause();
			CheckPresentation(mode, enabled, TextureFilter::Nearest);

			// Also enter via the API and leave via the shortcut.
			SetFullscreenWithPause(true);
			CheckPresentation(mode, enabled, TextureFilter::Nearest);
			AltEnter();
			REQUIRE_FALSE(Window::GetState().fullscreen);
			CheckPresentation(mode, enabled, TextureFilter::Nearest);

			// Cover both same-method round trips for every presentation policy, too.
			for (const bool shortcut : { false, true })
			{
				INFO(shortcut);
				if (shortcut) { AltEnter(); }
				else { SetFullscreenWithPause(true); }
				REQUIRE(Window::GetState().fullscreen);
				CheckPresentation(mode, enabled, TextureFilter::Nearest);
				if (shortcut) { AltEnter(); }
				else { SetFullscreenWithPause(false); }
				REQUIRE_FALSE(Window::GetState().fullscreen);
				CheckPresentation(mode, enabled, TextureFilter::Nearest);
			}
		}
	}
}

TEST_CASE("WindowFullscreen.changed_policy_is_not_restored_on_exit", "[.fullscreen]")
{
	REQUIRE_FALSE(Window::GetState().fullscreen);
	const RestoreWindowAndScene restore;
	Window::SetToggleFullscreenEnabled(true);
	UpdateWithPause();
	for (const bool exitByShortcut : { false, true })
	{
		INFO(exitByShortcut);
		Scene::SetResizeMode(ResizeMode::Virtual);
		Scene::SetHighResolutionEnabled(false);
		Scene::SetTextureFilter(TextureFilter::Linear);
		AltEnter();
		REQUIRE(Window::GetState().fullscreen);

		Scene::SetResizeMode(ResizeMode::Keep);
		Scene::Resize(320, 240);
		Scene::SetHighResolutionEnabled(true);
		Scene::SetTextureFilter(TextureFilter::Nearest);
		if (exitByShortcut) { AltEnter(); }
		else { SetFullscreenWithPause(false); }
		REQUIRE_FALSE(Window::GetState().fullscreen);
		CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest, Size{ 320, 240 });
		UpdateWithPause();
		CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest, Size{ 320, 240 });
	}

	// A disabled shortcut does not change the window, or leave a deferred toggle behind.
	Window::SetToggleFullscreenEnabled(false);
	AltEnter();
	CHECK_FALSE(Window::GetState().fullscreen);
	SetFullscreenWithPause(true);
	REQUIRE(Window::GetState().fullscreen);
	CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest, Size{ 320, 240 });
	SetFullscreenWithPause(false);
	REQUIRE_FALSE(Window::GetState().fullscreen);
	Window::SetToggleFullscreenEnabled(true);
	UpdateWithPause();
	CHECK_FALSE(Window::GetState().fullscreen);
	CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest, Size{ 320, 240 });

	// Re-enabling before the next frame must not revive an ignored message.
	Window::SetToggleFullscreenEnabled(false);
	::SendMessageW(MainWindow(), WM_SYSKEYDOWN, VK_RETURN, (LPARAM{ 1 } << 29));
	Window::SetToggleFullscreenEnabled(true);
	UpdateWithPause();
	CHECK_FALSE(Window::GetState().fullscreen);
	SetFullscreenWithPause(false);

	// Disabling also cancels an accepted request that has not executed yet.
	::SendMessageW(MainWindow(), WM_SYSKEYDOWN, VK_RETURN, (LPARAM{ 1 } << 29));
	Window::SetToggleFullscreenEnabled(false);
	Window::SetToggleFullscreenEnabled(true);
	UpdateWithPause();
	CHECK_FALSE(Window::GetState().fullscreen);
	SetFullscreenWithPause(false);

	// Reapplying enabled without disabling must preserve a valid request.
	::SendMessageW(MainWindow(), WM_SYSKEYDOWN, VK_RETURN, (LPARAM{ 1 } << 29));
	Window::SetToggleFullscreenEnabled(true);
	UpdateWithPause();
	CHECK(Window::GetState().fullscreen);
	SetFullscreenWithPause(false);
}

TEST_CASE("WindowFullscreen.bounds_match_selected_monitor", "[.fullscreen]")
{
	REQUIRE_FALSE(Window::GetState().fullscreen);
	const RestoreWindowAndScene restore;
	UpdateWithPause();
	Scene::SetResizeMode(ResizeMode::Actual);
	Scene::SetHighResolutionEnabled(false);
	Scene::SetTextureFilter(TextureFilter::Linear);
	const auto monitors = System::EnumerateMonitors();
	REQUIRE_FALSE(monitors.isEmpty());
	for (size_t i = 0; i < monitors.size(); ++i)
	{
		RECT windowedBounds{};
		REQUIRE(::GetWindowRect(MainWindow(), &windowedBounds));
		const auto windowed = Window::GetState();
		const Rect expected = monitors[i].displayRect;
		INFO(i);
		INFO(expected);
		SetFullscreenWithPause(true, i);
		UpdateWithPause();
		REQUIRE(Window::GetState().fullscreen);
		RECT bounds{};
		REQUIRE(::GetWindowRect(MainWindow(), &bounds));
		CHECK(Rect{ bounds.left, bounds.top, (bounds.right - bounds.left), (bounds.bottom - bounds.top) } == expected);
		CHECK(Window::GetState().frameBufferSize == expected.size);
		CheckPresentation(ResizeMode::Actual, false, TextureFilter::Linear);
		SetFullscreenWithPause(false);
		UpdateWithPause();
		REQUIRE(::GetWindowRect(MainWindow(), &bounds));
		CHECK(bounds.left == windowedBounds.left);
		CHECK(bounds.top == windowedBounds.top);
		CHECK(bounds.right == windowedBounds.right);
		CHECK(bounds.bottom == windowedBounds.bottom);
		CHECK(Window::GetState().virtualSize == windowed.virtualSize);
		CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
	}
}

# endif
