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
			Window::SetFullscreen(false);
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
		REQUIRE(System::Update());
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

TEST_CASE("WindowFullscreen.api_and_shortcut_preserve_presentation")
{
	REQUIRE_FALSE(Window::GetState().fullscreen);
	const RestoreWindowAndScene restore;
	Window::SetToggleFullscreenEnabled(true);
	REQUIRE(System::Update());
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
			Window::SetFullscreen(true);
			CheckPresentation(mode, enabled, TextureFilter::Nearest);
			Window::SetFullscreen(false);
			REQUIRE_FALSE(Window::GetState().fullscreen);
			CheckPresentation(mode, enabled, TextureFilter::Nearest);
			REQUIRE(System::Update());
			CheckPresentation(mode, enabled, TextureFilter::Nearest);

			// Also enter via the API and leave via the shortcut.
			Window::SetFullscreen(true);
			CheckPresentation(mode, enabled, TextureFilter::Nearest);
			AltEnter();
			REQUIRE_FALSE(Window::GetState().fullscreen);
			CheckPresentation(mode, enabled, TextureFilter::Nearest);
		}
	}
}

TEST_CASE("WindowFullscreen.changed_policy_is_not_restored_on_exit")
{
	REQUIRE_FALSE(Window::GetState().fullscreen);
	const RestoreWindowAndScene restore;
	Window::SetToggleFullscreenEnabled(true);
	REQUIRE(System::Update());
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
		else { Window::SetFullscreen(false); }
		REQUIRE_FALSE(Window::GetState().fullscreen);
		CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest, Size{ 320, 240 });
		REQUIRE(System::Update());
		CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest, Size{ 320, 240 });
	}

	// A disabled shortcut does not change the window, or leave a deferred toggle behind.
	Window::SetToggleFullscreenEnabled(false);
	AltEnter();
	CHECK_FALSE(Window::GetState().fullscreen);
	Window::SetToggleFullscreenEnabled(true);
	REQUIRE(System::Update());
	CHECK_FALSE(Window::GetState().fullscreen);
	CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest, Size{ 320, 240 });
}

TEST_CASE("WindowFullscreen.bounds_match_selected_monitor")
{
	REQUIRE_FALSE(Window::GetState().fullscreen);
	const RestoreWindowAndScene restore;
	REQUIRE(System::Update());
	Scene::SetResizeMode(ResizeMode::Actual);
	Scene::SetHighResolutionEnabled(false);
	Scene::SetTextureFilter(TextureFilter::Linear);
	const auto monitors = System::EnumerateMonitors();
	REQUIRE_FALSE(monitors.isEmpty());
	for (size_t i = 0; i < monitors.size(); ++i)
	{
		const Rect expected = monitors[i].displayRect;
		INFO(i);
		INFO(expected);
		Window::SetFullscreen(true, i);
		REQUIRE(System::Update());
		REQUIRE(Window::GetState().fullscreen);
		RECT bounds{};
		REQUIRE(::GetWindowRect(MainWindow(), &bounds));
		CHECK(Rect{ bounds.left, bounds.top, (bounds.right - bounds.left), (bounds.bottom - bounds.top) } == expected);
		CHECK(Window::GetState().frameBufferSize == expected.size);
		CheckPresentation(ResizeMode::Actual, false, TextureFilter::Linear);
		Window::SetFullscreen(false);
		REQUIRE(System::Update());
	}
}

# endif
