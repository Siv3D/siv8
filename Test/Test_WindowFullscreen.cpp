//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(WINDOWS) || SIV3D_PLATFORM(MACOS)

# include "../Siv3D/src/Siv3D/Engine/Siv3DEngine.hpp"
# include "../Siv3D/src/Siv3D/Window/IWindow.hpp"
# include "../Siv3D/src/Siv3D/Scene/SceneUtility.hpp"

# if SIV3D_PLATFORM(WINDOWS)
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Window/CWindow.hpp>
# else
// Xcode compiles this file as Objective-C++ with NO_S3D_USING.
# include <Siv3D/GLFW/GLFW.hpp>
# endif

namespace s3d
{
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
			const SceneConfig config = Scene::GetConfig();
			const bool toggleEnabled = Window::IsToggleFullscreenEnabled();

			~RestoreWindowAndScene()
			{
				SetFullscreenWithPause(false);
				Window::SetPos(window.bounds.pos);
				Window::Resize(window.virtualSize, CenterWindow::No);
				if (window.maximized) { Window::Maximize(); }
				Scene::Configure(config);
				Window::SetToggleFullscreenEnabled(toggleEnabled);
			}
		};

# if SIV3D_PLATFORM(WINDOWS)
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

		void ReportWindow(const char* phase)
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
		constexpr ResizeFunction ResizeFunctions[] = { Window::Resize, Window::ResizeVirtual, Window::ResizeActual };
		constexpr const char* ResizeNames[] = { "Resize", "ResizeVirtual", "ResizeActual" };

		Array<std::pair<size_t, size_t>> MixedDPIRoutes(const Array<MonitorInfo>& monitors)
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

# endif

		void CheckSelectedConfig(const SceneConfig& config, const Size expected)
		{
			CHECK(Scene::GetConfig() == config);
			CHECK(Scene::Size() == expected);
			SceneMisc::RenderState reference;
			reference.config = config;
			reference.size = expected;
			reference.frameBufferSize = Window::GetState().frameBufferSize;
			CHECK(Scene::RenderResolution() == reference.renderSize());
			const auto [scale, area] = reference.composition();
			CHECK(Cursor::GetBaseWindowTransform() == Mat3x2::Scale(scale).translated(area.pos));
			const Mat3x2 all = (Cursor::GetLocalTransform() * Cursor::GetCameraTransform() * Cursor::GetBaseWindowTransform());
			CHECK(Cursor::PosF().epsilonEquals(all.inverse().transformPoint(Cursor::PosRaw()), 1e-3));
			CHECK(Scene::ClientToScene(reference.frameBufferSize * 0.5).epsilonEquals(expected * 0.5, 1e-6));
		}

		void CheckPresentation(const SceneSizeMode mode, const bool highResolution, const TextureFilter filter,
			const Size fixedSize = Size{ 400, 300 })
		{
			CHECK(Scene::GetConfig().sizeMode == mode);
			CHECK((Scene::GetConfig().resolution == SceneResolution::Display) == highResolution);
			CHECK(Scene::GetConfig().scalingFilter == filter);
			const auto& window = Window::GetState();
			const Size expected = (mode == SceneSizeMode::Fixed) ? fixedSize
				: ((mode == SceneSizeMode::Window) ? window.virtualSize : window.frameBufferSize);
			CHECK(Scene::Size() == expected);
			SceneMisc::RenderState reference;
			reference.size = expected;
			reference.frameBufferSize = window.frameBufferSize;
			reference.config.resolution = (highResolution ? SceneResolution::Display : SceneResolution::SceneSize);
			CHECK(Scene::RenderResolution() == reference.renderSize());
			const auto [scale, area] = reference.composition();
			CHECK(Cursor::GetBaseWindowTransform() == Mat3x2::Scale(scale).translated(area.pos));
		}
	}

# if SIV3D_PLATFORM(WINDOWS)

	TEST_CASE("WindowFullscreen.api_and_shortcut_preserve_presentation", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::SetToggleFullscreenEnabled(true);
		UpdateWithPause();
		for (const SceneSizeMode mode : { SceneSizeMode::Fixed, SceneSizeMode::Window, SceneSizeMode::WindowPixels })
		{
			for (const bool enabled : { false, true })
			{
				INFO(static_cast<int32>(mode));
				INFO(enabled);
				{
					auto config = Scene::GetConfig();
					config.sizeMode = mode;
					config.fullscreen = FullscreenSceneMode::FollowWindow;
					config.fixedSceneSize = Size{ 400, 300 };
					config.resolution = (enabled ? SceneResolution::Display : SceneResolution::SceneSize);
					config.scalingFilter = TextureFilter::Nearest;
					Scene::Configure(config);
				}

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
			{
				auto config = Scene::GetConfig();
				config.sizeMode = SceneSizeMode::Window;
				config.fullscreen = FullscreenSceneMode::FollowWindow;
				config.resolution = SceneResolution::SceneSize;
				config.scalingFilter = TextureFilter::Linear;
				Scene::Configure(config);
			}
			AltEnter();
			REQUIRE(Window::GetState().fullscreen);

			{
				auto config = Scene::GetConfig();
				config.sizeMode = SceneSizeMode::Fixed;
				config.fullscreen = FullscreenSceneMode::FollowWindow;
				config.fixedSceneSize = Size{ 320, 240 };
				config.resolution = SceneResolution::Display;
				config.scalingFilter = TextureFilter::Nearest;
				Scene::Configure(config);
			}
			if (exitByShortcut) { AltEnter(); }
			else { SetFullscreenWithPause(false); }
			REQUIRE_FALSE(Window::GetState().fullscreen);
			CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });
			UpdateWithPause();
			CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });
		}

		// A disabled shortcut does not change the window, or leave a deferred toggle behind.
		Window::SetToggleFullscreenEnabled(false);
		AltEnter();
		CHECK_FALSE(Window::GetState().fullscreen);
		SetFullscreenWithPause(true);
		REQUIRE(Window::GetState().fullscreen);
		CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });
		SetFullscreenWithPause(false);
		REQUIRE_FALSE(Window::GetState().fullscreen);
		Window::SetToggleFullscreenEnabled(true);
		UpdateWithPause();
		CHECK_FALSE(Window::GetState().fullscreen);
		CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });

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

	TEST_CASE("WindowFullscreen.resize_restores_monitor", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		UpdateWithPause();
		const auto monitors = System::EnumerateMonitors();
		const auto routes = MixedDPIRoutes(monitors);
		const Transformer2D local{ Mat3x2::Translate(20, 10), TransformCursor::Yes, Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Scale(1.5), TransformCursor::Yes, Transformer2D::Target::SetCamera };
		for (const auto& [source, target] : routes)
		{
			REQUIRE(Window::Resize(600, 400, CenterWindow::No));
			REQUIRE(Window::CenterToScreen(source));
			UpdateWithPause();
			REQUIRE(System::GetCurrentMonitorIndex() == source);
			for (const SceneSizeMode mode : { SceneSizeMode::Fixed, SceneSizeMode::Window, SceneSizeMode::WindowPixels })
			{
				for (const bool enabled : { false, true })
				{
					// Resize delegates to ResizeVirtual: alternate those aliases instead of
					// repeating their entire matrix. Exercise actual sizes for every policy.
					for (const int32 api : { (enabled ? 1 : 0), 2 })
					{
						CAPTURE(source, target, mode, enabled, ResizeNames[api]);
						Catch::cout() << "route=" << source << "->" << target << " api=" << ResizeNames[api] << '\n';
						REQUIRE(Window::Resize(600, 400, CenterWindow::No));
						const auto windowed = Window::GetState();
						RECT nativeWindowed{};
						REQUIRE(::GetWindowRect(MainWindow(), &nativeWindowed));
						{
							auto config = Scene::GetConfig();
							config.sizeMode = (mode == SceneSizeMode::Window) ? SceneSizeMode::WindowPixels : SceneSizeMode::Window;
							config.fullscreen = FullscreenSceneMode::FollowWindow;
							config.resolution = (not enabled ? SceneResolution::Display : SceneResolution::SceneSize);
							config.scalingFilter = TextureFilter::Linear;
							Scene::Configure(config);
						}
						ReportWindow("windowed");
						SetFullscreenWithPause(true, target);
						REQUIRE(Window::GetState().fullscreen);
						// All three settings changed in fullscreen must survive the resize exit.
						{
							auto config = Scene::GetConfig();
							config.sizeMode = mode;
							config.fullscreen = FullscreenSceneMode::FollowWindow;
							config.fixedSceneSize = Size{ 400, 300 };
							config.resolution = (enabled ? SceneResolution::Display : SceneResolution::SceneSize);
							config.scalingFilter = TextureFilter::Nearest;
							Scene::Configure(config);
						}
						ReportWindow("fullscreen");
						const auto full = Window::GetState();
						CHECK(System::GetCurrentMonitorIndex() == target);
						CHECK(full.frameBufferSize == monitors[target].displayRect.size);
						CheckPresentation(mode, enabled, TextureFilter::Nearest);
						for (int32 sameAPI = 0; sameAPI < 3; ++sameAPI)
						{
							CAPTURE(ResizeNames[sameAPI]);
							CHECK(ResizeFunctions[sameAPI]((sameAPI == 2) ? full.frameBufferSize : full.virtualSize, CenterWindow::No));
							CHECK(Window::GetState().fullscreen);
							CHECK(Window::GetState().bounds == full.bounds);
							CHECK(Window::GetState().frameBufferSize == full.frameBufferSize);
							CheckPresentation(mode, enabled, TextureFilter::Nearest);
						}
						for (const Size invalid : { Size{ 0, 600 }, Size{ 800, -1 }, Size{ 8193, 600 }, Size{ 800, 8193 } })
						{
							CHECK_THROWS_AS(ResizeFunctions[api](invalid, CenterWindow::No), Error);
						}
						UpdateWithPause();
						CHECK(Window::GetState().fullscreen);
						CHECK(Window::GetState().virtualSize == full.virtualSize);
						CHECK(Window::GetState().frameBufferSize == full.frameBufferSize);
						CHECK(Window::GetState().bounds == full.bounds);
						CheckPresentation(mode, enabled, TextureFilter::Nearest);

						const Size requested{ 800, 600 };
						const bool resized = ResizeFunctions[api](requested, CenterWindow::No);
						const auto check = [&](const char* phase)
						{
							INFO(phase);
							ReportWindow(phase);
							const auto& window = Window::GetState();
							CHECK_FALSE(window.fullscreen);
							CHECK(System::GetCurrentMonitorIndex() == source);
							CHECK(window.scaling == windowed.scaling);
							CHECK(::GetDpiForWindow(MainWindow()) == Math::Round(windowed.scaling * 96));
							CHECK(window.bounds.pos == windowed.bounds.pos);
							RECT bounds{}, client{};
							REQUIRE(::GetWindowRect(MainWindow(), &bounds));
							REQUIRE(::GetClientRect(MainWindow(), &client));
							CHECK(bounds.left == nativeWindowed.left);
							CHECK(bounds.top == nativeWindowed.top);
							const Size pixels = (api == 2) ? requested : Math::Round(requested * windowed.scaling).asPoint();
							const Size points = (api == 2) ? Math::Round(requested / windowed.scaling).asPoint() : requested;
							CHECK(window.virtualSize == points);
							CHECK(window.frameBufferSize == pixels);
							CHECK(Size{ client.right, client.bottom } == pixels);
							CheckPresentation(mode, enabled, TextureFilter::Nearest);
							CHECK(Scene::ClientToScene(pixels * 0.5).epsilonEquals(Scene::Size() * 0.5, 1e-6));
							const auto all = (Cursor::GetLocalTransform() * Cursor::GetCameraTransform() * Cursor::GetBaseWindowTransform());
							CHECK(Cursor::PosF().epsilonEquals(all.inverse().transformPoint(Cursor::PosRaw()), 1e-3));
						};
						check("resize immediate");
						System::Sleep(FullscreenPauseMillis);
						REQUIRE(resized);
						UpdateWithPause();
						check("resize next frame");
					}
				}
			}
		}
	}

	TEST_CASE("WindowFullscreen.keep_size_resize_restores_monitor", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		UpdateWithPause();
		const auto routes = MixedDPIRoutes(System::EnumerateMonitors());
		const Transformer2D local{ Mat3x2::Translate(20, 10), TransformCursor::Yes, Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Scale(1.5), TransformCursor::Yes, Transformer2D::Target::SetCamera };
		for (const auto& [source, target] : routes)
		{
			REQUIRE(Window::Resize(600, 400, CenterWindow::No));
			REQUIRE(Window::CenterToScreen(source));
			UpdateWithPause();
			REQUIRE(System::GetCurrentMonitorIndex() == source);
			for (int32 api = 0; api < 3; ++api)
			{
				CAPTURE(source, target, ResizeNames[api]);
				Catch::cout() << "KeepSize route=" << source << "->" << target << " api=" << ResizeNames[api] << '\n';
				REQUIRE(Window::Resize(600, 400, CenterWindow::No));
				Scene::Configure(SceneConfig{});
				const auto windowed = Window::GetState();
				RECT nativeWindowed{};
				REQUIRE(::GetWindowRect(MainWindow(), &nativeWindowed));
				SetFullscreenWithPause(true, target);
				REQUIRE(Window::GetState().fullscreen);
				CHECK(System::GetCurrentMonitorIndex() == target);
				CheckSelectedConfig(SceneConfig{}, windowed.virtualSize);
				UpdateWithPause();
				CheckSelectedConfig(SceneConfig{}, windowed.virtualSize);
				auto config = Scene::GetConfig();
				Size heldSize = Scene::Size();
				if (api != 0)
				{
					config.resolution = SceneResolution::Display;
					config.scalingFilter = TextureFilter::Nearest;
					Scene::Configure(config);
					CheckSelectedConfig(config, heldSize);
				}
				if (api == 2)
				{
					config.fullscreen = FullscreenSceneMode::FollowWindow;
					Scene::Configure(config);
					heldSize = Window::GetState().virtualSize;
					CheckSelectedConfig(config, heldSize);
					config.fullscreen = FullscreenSceneMode::KeepSize;
					Scene::Configure(config);
					CheckSelectedConfig(config, heldSize);
				}
				Scene::Configure(Scene::GetConfig());
				CheckSelectedConfig(config, heldSize);
				const auto full = Window::GetState();
				CHECK(ResizeFunctions[api]((api == 2) ? full.frameBufferSize : full.virtualSize, CenterWindow::No));
				CHECK_THROWS_AS(ResizeFunctions[api]({ 0, 600 }, CenterWindow::No), Error);
				CHECK(Window::GetState().fullscreen);
				CHECK(Window::GetState().bounds == full.bounds);
				CheckSelectedConfig(config, heldSize);
				UpdateWithPause();
				CHECK(Window::GetState().fullscreen);
				CheckSelectedConfig(config, heldSize);
				ReportWindow("KeepSize fullscreen");

				const Size requested{ 800, 600 };
				const bool resized = ResizeFunctions[api](requested, CenterWindow::No);
				const auto check = [&](const char* phase)
				{
					INFO(phase);
					ReportWindow(phase);
					const auto& window = Window::GetState();
					CHECK_FALSE(window.fullscreen);
					CHECK(System::GetCurrentMonitorIndex() == source);
					CHECK(window.scaling == windowed.scaling);
					CHECK(::GetDpiForWindow(MainWindow()) == Math::Round(windowed.scaling * 96));
					CHECK(window.bounds.pos == windowed.bounds.pos);
					RECT bounds{}, client{};
					REQUIRE(::GetWindowRect(MainWindow(), &bounds));
					REQUIRE(::GetClientRect(MainWindow(), &client));
					CHECK(bounds.left == nativeWindowed.left);
					CHECK(bounds.top == nativeWindowed.top);
					const Size pixels = (api == 2) ? requested : Math::Round(requested * windowed.scaling).asPoint();
					const Size points = (api == 2) ? Math::Round(requested / windowed.scaling).asPoint() : requested;
					CHECK(window.virtualSize == points);
					CHECK(window.frameBufferSize == pixels);
					CHECK(Size{ client.right, client.bottom } == pixels);
					CheckSelectedConfig(config, points);
				};
				check("KeepSize resize immediate");
				System::Sleep(FullscreenPauseMillis);
				REQUIRE(resized);
				UpdateWithPause();
				check("KeepSize resize next frame");
			}
		}
	}

	TEST_CASE("WindowFullscreen.below_minimum_resize_restores_window", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const ScopeExit restoreMinimum{ [&] { Window::SetMinimumFrameBufferSize(restore.window.minFrameBufferSize); } };
		UpdateWithPause();
		REQUIRE(Window::Resize(600, 400, CenterWindow::No));
		Window::SetMinimumFrameBufferSize({ 200, 200 });
		{
			auto config = Scene::GetConfig();
			config.sizeMode = SceneSizeMode::Window;
			config.fullscreen = FullscreenSceneMode::FollowWindow;
			config.resolution = SceneResolution::Display;
			config.scalingFilter = TextureFilter::Linear;
			Scene::Configure(config);
		}
		const auto windowed = Window::GetState();
		for (int32 api = 0; api < 3; ++api)
		{
			CAPTURE(ResizeNames[api]);
			auto config = Scene::GetConfig();
			config.fullscreen = (api == 0) ? FullscreenSceneMode::FollowWindow : FullscreenSceneMode::KeepSize;
			Scene::Configure(config);
			SetFullscreenWithPause(true);
			REQUIRE(Window::GetState().fullscreen);
			// In-range requests below the minimum return false after restoring the window.
			CHECK_FALSE(ResizeFunctions[api]({ 1, 1 }, CenterWindow::No));
			const auto check = [&]
			{
				CHECK_FALSE(Window::GetState().fullscreen);
				CHECK(Window::GetState().bounds == windowed.bounds);
				CHECK(Window::GetState().virtualSize == windowed.virtualSize);
				CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
				CHECK(Scene::GetConfig() == config);
				CheckPresentation(SceneSizeMode::Window, true, TextureFilter::Linear);
			};
			check();
			System::Sleep(FullscreenPauseMillis);
			UpdateWithPause();
			check();
		}
	}

	TEST_CASE("WindowFullscreen.bounds_match_selected_monitor", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		UpdateWithPause();
		{
			auto config = Scene::GetConfig();
			config.sizeMode = SceneSizeMode::WindowPixels;
			config.fullscreen = FullscreenSceneMode::FollowWindow;
			config.resolution = SceneResolution::SceneSize;
			config.scalingFilter = TextureFilter::Linear;
			Scene::Configure(config);
		}
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		auto& window = *static_cast<CWindow*>(SIV3D_ENGINE(Window));
		WindowsMonitor::Functions functions;
		functions.enumerateMonitors = [] { return Array<MonitorInfo>{}; };
		const size_t invalid = std::numeric_limits<size_t>::max();
		window.setFullscreen(true, invalid, functions);
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const size_t firstMonitor = System::GetCurrentMonitorIndex();
		REQUIRE(firstMonitor < monitors.size());
		for (size_t step = 0; step < monitors.size(); ++step)
		{
			const size_t i = ((firstMonitor + step) % monitors.size());
			RECT windowedBounds{};
			REQUIRE(::GetWindowRect(MainWindow(), &windowedBounds));
			const auto windowed = Window::GetState();
			const Rect expected = monitors[i].displayRect;
			INFO(i);
			INFO(expected);
			if (step == 0)
			{
				// Recover from failure, falling back by identity in a differently ordered list.
				functions.enumerateMonitors = []
				{
					auto snapshot = System::EnumerateMonitors();
					if (not snapshot.isEmpty()) { std::rotate(snapshot.begin(), snapshot.begin() + 1, snapshot.end()); }
					return snapshot;
				};
				window.setFullscreen(true, invalid, functions);
				System::Sleep(FullscreenPauseMillis);
			}
			else { SetFullscreenWithPause(true, i); }
			UpdateWithPause();
			REQUIRE(Window::GetState().fullscreen);
			RECT bounds{};
			REQUIRE(::GetWindowRect(MainWindow(), &bounds));
			CHECK(Rect{ bounds.left, bounds.top, (bounds.right - bounds.left), (bounds.bottom - bounds.top) } == expected);
			CHECK(Window::GetState().frameBufferSize == expected.size);
			CheckPresentation(SceneSizeMode::WindowPixels, false, TextureFilter::Linear);
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

# else

	namespace
	{
		GLFWwindow* MainWindow()
		{
			return static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
		}

		NSMenuItem* FullscreenMenuItem(NSMenu* menu)
		{
			for (NSMenuItem* item in menu.itemArray)
			{
				if (item.action == NSSelectorFromString(@"toggleFullscreen_Siv3D:")) { return item; }
				if (NSMenuItem* found = FullscreenMenuItem(item.submenu)) { return found; }
			}
			return nil;
		}

		NSEvent* FullscreenKeyEvent(const bool repeat)
		{
			NSWindow* native = glfwGetCocoaWindow(MainWindow());
			return [NSEvent keyEventWithType:NSEventTypeKeyDown location:NSZeroPoint
				modifierFlags:(NSEventModifierFlagControl | NSEventModifierFlagCommand) timestamp:0
				windowNumber:native.windowNumber context:nil characters:@"f"
				charactersIgnoringModifiers:@"f" isARepeat:repeat keyCode:0x03];
		}

		void FullscreenShortcut(const bool repeat = false)
		{
			NSWindow* native = glfwGetCocoaWindow(MainWindow());
			NSEvent* event = FullscreenKeyEvent(repeat);
			REQUIRE([native performKeyEquivalent:event]);
		}

		enum class FullscreenRoute { Shortcut, Menu, API };

		void RequestFullscreen(const FullscreenRoute route)
		{
			NSWindow* native = glfwGetCocoaWindow(MainWindow());
			if (route == FullscreenRoute::API)
			{
				SetFullscreenWithPause(not Window::GetState().fullscreen);
				return;
			}
			const bool previous = Window::GetState().fullscreen;
			if (route == FullscreenRoute::Shortcut) { FullscreenShortcut(); }
			else if (route == FullscreenRoute::Menu)
			{
				NSMenuItem* item = FullscreenMenuItem(NSApp.mainMenu);
				REQUIRE(item != nil);
				[item.menu update];
				REQUIRE(item.enabled);
				REQUIRE([NSApp targetForAction:item.action to:item.target from:item] == native);
				[item.menu performActionForItemAtIndex:[item.menu indexOfItem:item]];
			}
			CHECK(Window::GetState().fullscreen == previous);
			UpdateWithPause();
		}

		struct NativeDisplay
		{
			CGDirectDisplayID id;
			CGRect bounds;
			int32 modeID;
			Size modeSize;
			Size pixels;
			double refreshRate;
			uint32 flags;
		};

		Array<NativeDisplay> ReadNativeDisplays()
		{
			uint32 count = 0;
			REQUIRE(::CGGetActiveDisplayList(0, nullptr, &count) == kCGErrorSuccess);
			Array<CGDirectDisplayID> ids(count);
			REQUIRE(::CGGetActiveDisplayList(count, ids.data(), &count) == kCGErrorSuccess);
			ids.resize(count);
			Array<NativeDisplay> result;
			for (const auto id : ids)
			{
				CGDisplayModeRef mode = ::CGDisplayCopyDisplayMode(id);
				REQUIRE(mode != nullptr);
				const ScopeExit release{ [=] { ::CGDisplayModeRelease(mode); } };
				result.push_back({ id, ::CGDisplayBounds(id), ::CGDisplayModeGetIODisplayModeID(mode),
					Size{ static_cast<int32>(::CGDisplayModeGetWidth(mode)), static_cast<int32>(::CGDisplayModeGetHeight(mode)) },
					Size{ static_cast<int32>(::CGDisplayModeGetPixelWidth(mode)), static_cast<int32>(::CGDisplayModeGetPixelHeight(mode)) },
					::CGDisplayModeGetRefreshRate(mode), ::CGDisplayModeGetIOFlags(mode) });
			}
			return result;
		}

		struct DesktopSnapshot
		{
			const Array<NativeDisplay> native = ReadNativeDisplays();
			const Array<MonitorInfo> monitors = System::EnumerateMonitors();

			void check() const
			{
				const auto currentNative = ReadNativeDisplays();
				REQUIRE(currentNative.size() == native.size());
				for (const auto& expected : native)
				{
					const auto actual = currentNative.find_if([&](const auto& display) { return (display.id == expected.id); });
					REQUIRE(actual != nullptr);
					CAPTURE(expected.id, actual->modeID, expected.modeID, actual->modeSize, actual->pixels);
					CHECK(::CGRectEqualToRect(actual->bounds, expected.bounds));
					CHECK(actual->modeID == expected.modeID);
					CHECK(actual->modeSize == expected.modeSize);
					CHECK(actual->pixels == expected.pixels);
					CHECK(actual->refreshRate == expected.refreshRate);
					CHECK(actual->flags == expected.flags);
				}
				const auto currentMonitors = System::EnumerateMonitors();
				REQUIRE(currentMonitors.size() == monitors.size());
				for (size_t i = 0; i < monitors.size(); ++i)
				{
					CAPTURE(i, monitors[i].displayRect, currentMonitors[i].displayRect);
					CHECK(currentMonitors[i].id == monitors[i].id);
					CHECK(currentMonitors[i].displayRect == monitors[i].displayRect);
					CHECK(currentMonitors[i].scaling == monitors[i].scaling);
					CHECK(currentMonitors[i].fullscreenResolution == monitors[i].fullscreenResolution);
				}
			}
		};
	}

	TEST_CASE("WindowFullscreen.macOS.ui_and_api_preserve_presentation", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const ScopeExit restoreStyle{ [&] { SetFullscreenWithPause(false); Window::SetStyle(restore.window.style); } };
		Window::SetToggleFullscreenEnabled(true);
		UpdateWithPause();
		const DesktopSnapshot desktop;
		NSWindow* native = glfwGetCocoaWindow(MainWindow());
		NSMenuItem* menu = FullscreenMenuItem(NSApp.mainMenu);
		REQUIRE(menu != nil);
		CHECK(std::string{ menu.keyEquivalent.UTF8String } == "f");
		CHECK(menu.keyEquivalentModifierMask == (NSEventModifierFlagControl | NSEventModifierFlagCommand));
		for (size_t monitor = 0; monitor < desktop.monitors.size(); ++monitor)
		{
			REQUIRE(Window::CenterToScreen(monitor));
			UpdateWithPause();
			for (const SceneSizeMode mode : { SceneSizeMode::Fixed, SceneSizeMode::Window, SceneSizeMode::WindowPixels })
			{
				for (const bool highResolution : { false, true })
				{
					CAPTURE(monitor, mode, highResolution);
					Window::SetStyle(highResolution ? WindowStyle::Sizable : WindowStyle::Fixed);
					{
						auto config = Scene::GetConfig();
						config.sizeMode = mode;
						config.fullscreen = FullscreenSceneMode::FollowWindow;
						config.fixedSceneSize = Size{ 400, 300 };
						config.resolution = (highResolution ? SceneResolution::Display : SceneResolution::SceneSize);
						config.scalingFilter = TextureFilter::Nearest;
						Scene::Configure(config);
					}
					const auto windowed = Window::GetState();
					for (const auto route : { FullscreenRoute::Shortcut, FullscreenRoute::Menu,
						FullscreenRoute::API })
					{
						CAPTURE(route);
						RequestFullscreen(route);
						REQUIRE(Window::GetState().fullscreen);
						CHECK(Window::GetState().bounds == desktop.monitors[monitor].displayRect);
						CHECK((native.styleMask & NSWindowStyleMaskFullScreen) == 0);
						CHECK([native validateMenuItem:menu]);
						CHECK([menu.title isEqualToString:@"Exit Full Screen"]);
						CheckPresentation(mode, highResolution, TextureFilter::Nearest);
						desktop.check();
						const auto exit = (route == FullscreenRoute::Shortcut) ? FullscreenRoute::API
							: ((route == FullscreenRoute::Menu) ? FullscreenRoute::Menu : FullscreenRoute::Shortcut);
						RequestFullscreen(exit);
						REQUIRE_FALSE(Window::GetState().fullscreen);
						CHECK(Window::GetState().bounds == windowed.bounds);
						CHECK(Window::GetState().virtualSize == windowed.virtualSize);
						CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
						CHECK([native validateMenuItem:menu]);
						CHECK([menu.title isEqualToString:@"Enter Full Screen"]);
						CheckPresentation(mode, highResolution, TextureFilter::Nearest);
						desktop.check();
					}
				}
			}
		}
	}

	TEST_CASE("WindowFullscreen.macOS.disabled_pending_repeat_and_styles", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const ScopeExit restoreStyle{ [&] { SetFullscreenWithPause(false); Window::SetStyle(restore.window.style); } };
		NSWindow* native = glfwGetCocoaWindow(MainWindow());
		NSMenuItem* menu = FullscreenMenuItem(NSApp.mainMenu);
		REQUIRE(menu != nil);
		const DesktopSnapshot desktop;
		for (const WindowStyle style : { WindowStyle::Fixed, WindowStyle::Sizable, WindowStyle::Frameless })
		{
			CAPTURE(style);
			Window::SetStyle(style);
			Window::SetToggleFullscreenEnabled(false);
			CHECK_FALSE(Window::IsToggleFullscreenEnabled());
			CHECK_FALSE([native validateMenuItem:menu]);
			CHECK((native.collectionBehavior & NSWindowCollectionBehaviorFullScreenPrimary) == 0);
			NSButton* green = [native standardWindowButton:NSWindowZoomButton];
			if (style != WindowStyle::Frameless)
			{
				REQUIRE(green != nil);
				CHECK(green.enabled == (style == WindowStyle::Sizable));
				CHECK_FALSE(green.hidden);
				CHECK(green.action != NSSelectorFromString(@"toggleFullscreen_Siv3D:"));
			}
			[native toggleFullScreen:menu];
			FullscreenShortcut();
			Window::SetToggleFullscreenEnabled(true);
			UpdateWithPause();
			CHECK_FALSE(Window::GetState().fullscreen);
			FullscreenShortcut();
			Window::SetToggleFullscreenEnabled(false);
			Window::SetToggleFullscreenEnabled(true);
			UpdateWithPause();
			CHECK_FALSE(Window::GetState().fullscreen);

			FullscreenShortcut();
			FullscreenShortcut(true);
			[native toggleFullScreen:menu]; // Two requests before a frame coalesce.
			Window::SetToggleFullscreenEnabled(true);
			UpdateWithPause();
			REQUIRE(Window::GetState().fullscreen);
			FullscreenShortcut(true);
			UpdateWithPause();
			CHECK(Window::GetState().fullscreen);
			// Settings changed while fullscreen survive an exit through the shortcut.
			{
				auto config = Scene::GetConfig();
				config.sizeMode = SceneSizeMode::Fixed;
				config.fullscreen = FullscreenSceneMode::FollowWindow;
				config.fixedSceneSize = Size{ 320, 240 };
				config.resolution = SceneResolution::Display;
				config.scalingFilter = TextureFilter::Nearest;
				Scene::Configure(config);
			}
			RequestFullscreen(FullscreenRoute::Shortcut);
			REQUIRE_FALSE(Window::GetState().fullscreen);
			CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });
			Window::SetToggleFullscreenEnabled(false);
			SetFullscreenWithPause(true);
			REQUIRE(Window::GetState().fullscreen);
			FullscreenShortcut();
			UpdateWithPause();
			CHECK(Window::GetState().fullscreen);
			SetFullscreenWithPause(false);
			REQUIRE_FALSE(Window::GetState().fullscreen);
			CHECK(Window::GetStyle() == style);
			desktop.check();
		}

		// Exercise NSApplication's event queue and main-menu key-equivalent path,
		// including currentEvent-based repeat suppression in toggleFullScreen:.
		Window::SetStyle(WindowStyle::Sizable);
		Window::SetToggleFullscreenEnabled(true);
		[NSApp postEvent:FullscreenKeyEvent(false) atStart:YES];
		UpdateWithPause();
		REQUIRE(Window::GetState().fullscreen);
		[NSApp postEvent:FullscreenKeyEvent(true) atStart:YES];
		UpdateWithPause();
		CHECK(Window::GetState().fullscreen);
		[NSApp postEvent:FullscreenKeyEvent(false) atStart:YES];
		UpdateWithPause();
		REQUIRE_FALSE(Window::GetState().fullscreen);

		// Fullscreen enablement must not affect native maximization controls.
		for (const bool enabled : { false, true })
		{
			Window::SetToggleFullscreenEnabled(enabled);
			NSButton* green = [native standardWindowButton:NSWindowZoomButton];
			REQUIRE(green != nil);
			REQUIRE(green.enabled);
			CHECK_FALSE(green.hidden);
			[green performClick:nil];
			UpdateWithPause();
			CHECK_FALSE(Window::GetState().fullscreen);
			CHECK(Window::GetState().maximized);
			Window::Restore();
			UpdateWithPause();
			CHECK_FALSE(Window::GetState().maximized);
		}
		NSMenuItem* zoom = [[NSMenuItem alloc] initWithTitle:@"Zoom" action:@selector(performZoom:) keyEquivalent:@""];
		CHECK([native validateMenuItem:zoom]);
		[native performZoom:zoom];
		UpdateWithPause();
		CHECK_FALSE(Window::GetState().fullscreen);
		CHECK(Window::GetState().maximized);
		Window::Restore();
		UpdateWithPause();
		CHECK_FALSE(Window::GetState().maximized);
		desktop.check();
	}

	TEST_CASE("WindowFullscreen.macOS.api_preserves_presentation", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		UpdateWithPause();
		const DesktopSnapshot desktop;
		const Point previousCursor = Cursor::ScreenPos();
		const ScopeExit restoreCursor{ [=] { ::CGWarpMouseCursorPosition(CGPointMake(previousCursor.x, previousCursor.y)); } };
		for (size_t monitor = 0; monitor < desktop.monitors.size(); ++monitor)
		{
			CAPTURE(monitor);
			REQUIRE(Window::CenterToScreen(monitor));
			UpdateWithPause();
			for (const SceneSizeMode mode : { SceneSizeMode::Fixed, SceneSizeMode::Window, SceneSizeMode::WindowPixels })
			{
				for (const bool enabled : { false, true })
				{
					CAPTURE(mode, enabled);
					{
						auto config = Scene::GetConfig();
						config.sizeMode = mode;
						config.fullscreen = FullscreenSceneMode::FollowWindow;
						config.fixedSceneSize = Size{ 400, 300 };
						config.resolution = (enabled ? SceneResolution::Display : SceneResolution::SceneSize);
						config.scalingFilter = TextureFilter::Nearest;
						Scene::Configure(config);
					}
					SetFullscreenWithPause(true);
					REQUIRE(Window::GetState().fullscreen);
					desktop.check();
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					UpdateWithPause();
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					{
						const Transformer2D local{ Mat3x2::Translate(20, 10), TransformCursor::Yes, Transformer2D::Target::SetLocal };
						const Transformer2D camera{ Mat3x2::Scale(1.5), TransformCursor::Yes, Transformer2D::Target::SetCamera };
						const Point logical{ 100, 80 };
						const Mat3x2 all = (Cursor::GetLocalTransform() * Cursor::GetCameraTransform() * Cursor::GetBaseWindowTransform());
						Cursor::SetPos(logical);
						Circle{ logical, 12 }.draw(Palette::Red);
						ScreenCapture::RequestCurrentFrame();
						UpdateWithPause();
						CHECK(Cursor::PosF().epsilonEquals(logical, 1.0));
						CHECK(Vec2{ Cursor::PosRaw() }.epsilonEquals(all.transformPoint(logical), 1.0));
						CHECK(Circle{ logical, 3 }.mouseOver());
						CHECK_FALSE(Circle{ logical + Point{ 30, 0 }, 3 }.mouseOver());
						REQUIRE(ScreenCapture::HasNewFrame());
						CHECK(ScreenCapture::GetFrame().size() == Scene::RenderResolution());
					}
					// Reapplying fullscreen must not change the selected policy.
					SetFullscreenWithPause(true);
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					SetFullscreenWithPause(false);
					REQUIRE_FALSE(Window::GetState().fullscreen);
					desktop.check();
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					UpdateWithPause();
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
				}
			}
		}
	}

	TEST_CASE("WindowFullscreen.macOS.monitor_round_trip_and_changed_policy", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		UpdateWithPause();
		const DesktopSnapshot desktop;
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		for (size_t source = 0; source < monitors.size(); ++source)
		{
			CAPTURE(source);
			REQUIRE(Window::CenterToScreen(source));
			UpdateWithPause();
			REQUIRE(System::GetCurrentMonitorIndex() == source);
			for (size_t i = 0; i < monitors.size(); ++i)
			{
				CAPTURE(i, monitors[i].name, monitors[i].displayRect, monitors[i].scaling.value_or(1.0));
				const auto windowed = Window::GetState();
				CAPTURE(windowed.bounds.pos, windowed.virtualSize, windowed.frameBufferSize, windowed.scaling);
				{
					auto config = Scene::GetConfig();
					config.sizeMode = SceneSizeMode::Window;
					config.fullscreen = FullscreenSceneMode::FollowWindow;
					config.resolution = SceneResolution::SceneSize;
					config.scalingFilter = TextureFilter::Linear;
					Scene::Configure(config);
				}
				SetFullscreenWithPause(true, i);
				REQUIRE(Window::GetState().fullscreen);
				CheckPresentation(SceneSizeMode::Window, false, TextureFilter::Linear);
				UpdateWithPause();
				desktop.check();
				// Compare to the desktop before entering fullscreen, not the new mode.
				const auto& fullscreenMonitor = monitors[i];
				CAPTURE(fullscreenMonitor.displayRect, fullscreenMonitor.scaling.value_or(1.0));
				CHECK(System::GetCurrentMonitorIndex() == i);
				CHECK(Window::GetState().bounds.pos == fullscreenMonitor.displayRect.pos);
				CHECK(Window::GetState().virtualSize == fullscreenMonitor.displayRect.size);
				CHECK(Window::GetState().frameBufferSize == fullscreenMonitor.fullscreenResolution);
				CHECK(Window::GetState().scaling == fullscreenMonitor.scaling.value_or(1.0));
				CheckPresentation(SceneSizeMode::Window, false, TextureFilter::Linear);
				{
					auto config = Scene::GetConfig();
					config.sizeMode = SceneSizeMode::Fixed;
					config.fullscreen = FullscreenSceneMode::FollowWindow;
					config.fixedSceneSize = Size{ 320, 240 };
					config.resolution = SceneResolution::Display;
					config.scalingFilter = TextureFilter::Nearest;
					Scene::Configure(config);
				}
				SetFullscreenWithPause(false);
				REQUIRE_FALSE(Window::GetState().fullscreen);
				CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });
				UpdateWithPause();
				desktop.check();
				CHECK(Window::GetState().bounds == windowed.bounds);
				CHECK(Window::GetState().virtualSize == windowed.virtualSize);
				CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
				CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });
			}
		}
	}

	TEST_CASE("WindowFullscreen.macOS.resize_exits_fullscreen", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const ScopeExit restoreStyle{ [&] { SetFullscreenWithPause(false); Window::SetStyle(restore.window.style); } };
		UpdateWithPause();
		const DesktopSnapshot desktop;
		REQUIRE_FALSE(desktop.monitors.isEmpty());
		{
			auto config = Scene::GetConfig();
			config.sizeMode = SceneSizeMode::Fixed;
			config.fullscreen = FullscreenSceneMode::FollowWindow;
			config.fixedSceneSize = Size{ 400, 300 };
			config.resolution = SceneResolution::Display;
			config.scalingFilter = TextureFilter::Nearest;
			Scene::Configure(config);
		}
		for (size_t source = 0; source < desktop.monitors.size(); ++source)
		{
			for (size_t target = 0; target < desktop.monitors.size(); ++target)
			{
				for (int32 api = 0; api < 3; ++api)
				{
					CAPTURE(source, target, api);
					const WindowStyle style = (api == 0) ? WindowStyle::Fixed
						: ((api == 1) ? WindowStyle::Sizable : WindowStyle::Frameless);
					Window::SetStyle(style);
					REQUIRE(Window::Resize(600, 400, CenterWindow::No));
					REQUIRE(Window::CenterToScreen(source));
					UpdateWithPause();
					const auto windowed = Window::GetState();
					SetFullscreenWithPause(true, target);
					REQUIRE(Window::GetState().fullscreen);
					const auto full = Window::GetState();
					CHECK(Window::Resize(full.virtualSize, CenterWindow::No));
					CHECK(Window::ResizeVirtual(full.virtualSize, CenterWindow::No));
					CHECK(Window::ResizeActual(full.frameBufferSize, CenterWindow::No));
					CHECK(Window::GetState().fullscreen);
					CHECK_THROWS_AS(Window::Resize(0, 400, CenterWindow::No), Error);
					CHECK_THROWS_AS(Window::ResizeActual(8193, 400, CenterWindow::No), Error);
					CHECK(Window::GetState().bounds == full.bounds);
					desktop.check();

					const Size requested{ 640, 420 };
					if (api == 0) { REQUIRE(Window::Resize(requested, CenterWindow::No)); }
					else if (api == 1) { REQUIRE(Window::ResizeVirtual(requested, CenterWindow::No)); }
					else { REQUIRE(Window::ResizeActual(requested, CenterWindow::No)); }
					System::Sleep(FullscreenPauseMillis);
					REQUIRE_FALSE(Window::GetState().fullscreen);
					CHECK(Window::GetStyle() == style);
					CHECK(System::GetCurrentMonitorIndex() == source);
					CHECK(Window::GetState().scaling == windowed.scaling);
					CHECK(Window::GetState().bounds.pos == windowed.bounds.pos);
					if (api == 2) { CHECK(Window::GetState().frameBufferSize == requested); }
					else { CHECK(Window::GetState().virtualSize == requested); }
					CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest);
					UpdateWithPause();
					desktop.check();
					CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest);
				}
			}
		}
	}

	TEST_CASE("WindowFullscreen.macOS.minimized_state_matches_native_window", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const ScopeExit restoreVisibility{ [] { Window::Restore(); System::Sleep(FullscreenPauseMillis); } };
		auto* window = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
		const auto waitForIconified = [window](const bool iconified)
		{
			// Cocoa may finish its animation over several event-loop iterations.
			for (int32 attempt = 0; attempt < 6; ++attempt)
			{
				UpdateWithPause();
				if ((::glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_TRUE) == iconified) { return; }
			}
			FAIL("The native window did not complete the minimize/restore transition");
		};
		const DesktopSnapshot desktop;
		for (size_t mode = 0; mode <= desktop.monitors.size(); ++mode)
		{
			const bool fullscreen = (mode != 0);
			CAPTURE(mode, fullscreen);
			if (fullscreen) { SetFullscreenWithPause(true, (mode - 1)); }
			Window::Minimize();
			System::Sleep(FullscreenPauseMillis);
			waitForIconified(true);
			REQUIRE(::glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_TRUE);
			CHECK(Window::GetState().minimized);
			desktop.check();
			Window::Restore();
			System::Sleep(FullscreenPauseMillis);
			waitForIconified(false);
			REQUIRE(::glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_FALSE);
			CHECK_FALSE(Window::GetState().minimized);
			desktop.check();
			CHECK(Window::GetState().fullscreen == fullscreen);
			CHECK(Scene::Size().x > 0);
			CHECK(Scene::Size().y > 0);
			if (fullscreen) { SetFullscreenWithPause(false); }
			desktop.check();
		}
	}

# endif

	TEST_CASE("WindowFullscreen.change_monitor_preserves_window_and_config", "[.fullscreen]")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		if (monitors.size() < 2)
		{
			SKIP("Fullscreen monitor changes require two attached displays.");
		}
		size_t first = 0, second = 1;
		// Prefer a mixed-DPI pair; cover both directions without a monitor Cartesian product.
		for (size_t i = 0; i < monitors.size(); ++i)
		{
			Catch::cout() << "transfer monitor[" << i << "] " << monitors[i].format().toUTF8() << '\n';
			for (size_t j = i + 1; j < monitors.size(); ++j)
			{
				if (monitors[i].scaling && monitors[j].scaling && (monitors[i].scaling != monitors[j].scaling))
				{
					first = i;
					second = j;
					break;
				}
			}
		}
		Catch::cout() << "transfer pair=" << first << " <-> " << second << '\n';
# if SIV3D_PLATFORM(MACOS)
		const DesktopSnapshot desktop;
# endif
		Window::SetToggleFullscreenEnabled(true);
		Window::Restore();
		const auto checkTarget = [&](const size_t target)
		{
			const auto& state = Window::GetState();
			REQUIRE(state.fullscreen);
			CHECK_FALSE(state.minimized);
			CHECK(System::GetCurrentMonitorIndex() == target);
			CHECK(state.bounds == monitors[target].displayRect);
			CHECK(state.scaling == monitors[target].scaling.value_or(1.0));
# if SIV3D_PLATFORM(WINDOWS)
			RECT native{};
			REQUIRE(::GetWindowRect(MainWindow(), &native));
			CHECK(Rect{ native.left, native.top, native.right - native.left, native.bottom - native.top } == monitors[target].displayRect);
			CHECK(state.frameBufferSize == monitors[target].displayRect.size);
# else
			CHECK(state.virtualSize == monitors[target].displayRect.size);
			CHECK(state.frameBufferSize == monitors[target].fullscreenResolution);
			desktop.check();
# endif
		};
		auto following = SceneConfig::Window(SceneResolution::Display);
		following.fullscreen = FullscreenSceneMode::FollowWindow;
		size_t step = 0;
		for (auto config : { SceneConfig::Fixed({ 320, 240 }), SceneConfig::Fixed({ 320, 240 }, SceneResolution::Display),
			SceneConfig::PixelArt({ 200, 150 }), SceneConfig::Window(), SceneConfig::Window(SceneResolution::Display),
			SceneConfig::WindowPixels(), following })
		{
			const size_t source = ((step % 2) ? second : first), target = ((step % 2) ? first : second);
			CAPTURE(step, source, target, config.sizeMode, config.resolution, config.fullscreen);
			REQUIRE(Window::CenterToScreen(source));
			REQUIRE(Window::Resize(600, 400, CenterWindow::No));
			const auto windowed = Window::GetState();
			Scene::Configure(config);
			Size expected = Scene::Size();
			const auto check = [&]
			{
				if ((config.sizeMode != SceneSizeMode::Fixed) &&
					(not Window::GetState().fullscreen || config.fullscreen == FullscreenSceneMode::FollowWindow))
				{
					expected = ((config.sizeMode == SceneSizeMode::Window) ? Window::GetState().virtualSize : Window::GetState().frameBufferSize);
				}
				CheckSelectedConfig(config, expected);
			};
			SetFullscreenWithPause(true, source);
			checkTarget(source);
			check();
			REQUIRE(System::Update());
			check();
			SetFullscreenWithPause(true, target);
			checkTarget(target);
			check();
			REQUIRE(System::Update());
			checkTarget(target);
			check();
			const auto full = Window::GetState();
			const auto checkUnmoved = [&]
			{
				CHECK(Window::GetState().bounds == full.bounds);
				CHECK(Window::GetState().frameBufferSize == full.frameBufferSize);
				CHECK(Window::GetState().fullscreen);
				check();
			};
			Window::SetFullscreen(true, target);
			checkUnmoved();
			Window::SetFullscreen(true);
			checkUnmoved();
			Window::SetFullscreen(true, std::numeric_limits<size_t>::max());
			checkUnmoved();
# if SIV3D_PLATFORM(WINDOWS)
			if (step == 0)
			{
				auto& window = *static_cast<CWindow*>(SIV3D_ENGINE(Window));
				WindowsMonitor::Functions functions;
				functions.enumerateMonitors = [] { return Array<MonitorInfo>{}; };
				window.setFullscreen(true, source, functions);
				checkUnmoved();
				functions.enumerateMonitors = []
				{
					auto result = System::EnumerateMonitors();
					for (auto& monitor : result) { monitor.displayDeviceName = U"not-an-attached-display"; }
					return result;
				};
				window.setFullscreen(true, std::numeric_limits<size_t>::max(), functions);
				checkUnmoved();
			}
# endif
			if (step == 6)
			{
				// KeepSize selected in fullscreen preserves the current scene through another move.
				config.fullscreen = FullscreenSceneMode::KeepSize;
				config.scalingFilter = TextureFilter::Nearest;
				Scene::Configure(config);
				SetFullscreenWithPause(true, source);
				checkTarget(source);
				check();
				REQUIRE(System::Update());
				check();
			}
			if (step == 1)
			{
# if SIV3D_PLATFORM(WINDOWS)
				AltEnter();
# else
				RequestFullscreen(FullscreenRoute::Shortcut);
# endif
			}
			else if ((2 <= step) && (step <= 4))
			{
				const Size requested{ 620, 420 };
				if (step == 2) { REQUIRE(Window::Resize(requested, CenterWindow::No)); }
				else if (step == 3) { REQUIRE(Window::ResizeVirtual(requested, CenterWindow::No)); }
				else { REQUIRE(Window::ResizeActual(requested, CenterWindow::No)); }
				System::Sleep(FullscreenPauseMillis);
				if (step != 4) { CHECK(Window::GetState().virtualSize == requested); }
# if SIV3D_PLATFORM(WINDOWS)
				else { CHECK(Window::GetState().frameBufferSize == requested); }
# else
				else { CHECK(Window::GetState().virtualSize == Math::Round(requested / windowed.scaling).asPoint()); }
# endif
			}
			else { SetFullscreenWithPause(false, target); }
			const auto checkRestored = [&]
			{
				REQUIRE_FALSE(Window::GetState().fullscreen);
				CHECK(Window::GetState().bounds.pos == windowed.bounds.pos);
				CHECK(System::GetCurrentMonitorIndex() == source);
				CHECK(Window::GetState().scaling == windowed.scaling);
				if ((step < 2) || (4 < step))
				{
					CHECK(Window::GetState().bounds == windowed.bounds);
					CHECK(Window::GetState().virtualSize == windowed.virtualSize);
					CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
				}
				else
				{
					const Size requested{ 620, 420 };
					const Size points = ((step == 4) ? Math::Round(requested / windowed.scaling).asPoint() : requested);
					CHECK(Window::GetState().virtualSize == points);
# if SIV3D_PLATFORM(WINDOWS)
					const Size pixels = ((step == 4) ? requested : Math::Round(points * windowed.scaling).asPoint());
# else
					const Size pixels = Math::Round(points * windowed.scaling).asPoint();
# endif
					CHECK(Window::GetState().frameBufferSize == pixels);
				}
				check();
			};
			checkRestored();
			REQUIRE(System::Update());
			checkRestored();
			++step;
		}
	}

	TEST_CASE("WindowFullscreen.scene_config_default_and_reconfigure", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::SetToggleFullscreenEnabled(true);
		UpdateWithPause();
		const auto monitors = System::EnumerateMonitors();
		REQUIRE(not monitors.isEmpty());
		for (const auto& monitor : monitors)
		{
			Catch::cout() << "SceneConfig monitor: " << monitor.name.toUTF8()
				<< " desktop=" << monitor.displayRect << " framebuffer=" << monitor.fullscreenResolution
				<< " scale=" << monitor.scaling.value_or(1.0) << '\n';
		}
		const auto shortcut = []
		{
# if SIV3D_PLATFORM(WINDOWS)
			AltEnter();
# else
			RequestFullscreen(FullscreenRoute::Shortcut);
# endif
		};
		const auto check = [](const SceneConfig& config, const Size expected)
		{
			CHECK(Scene::GetConfig() == config);
			CHECK(Scene::Size() == expected);
			const auto& window = Window::GetState();
			SceneMisc::RenderState reference;
			reference.config = config;
			reference.size = expected;
			reference.frameBufferSize = window.frameBufferSize;
			CHECK(Scene::RenderResolution() == reference.renderSize());
			const auto [scale, area] = reference.composition();
			CHECK(Cursor::GetBaseWindowTransform() == Mat3x2::Scale(scale).translated(area.pos));
			CHECK(Scene::ClientToScene(window.frameBufferSize * 0.5).epsilonEquals(expected * 0.5, 1e-6));
		};
		// Two mixed-route round trips cover both entry paths without multiplying all presets.
		for (const bool enterByShortcut : { false, true })
		{
			Scene::Configure(SceneConfig{});
			const Size windowScene = Scene::Size();
			if (enterByShortcut) { shortcut(); }
			else { SetFullscreenWithPause(true, (System::GetCurrentMonitorIndex() + 1) % monitors.size()); }
			REQUIRE(Window::GetState().fullscreen);
			check(SceneConfig{}, windowScene);
			UpdateWithPause();
			check(SceneConfig{}, windowScene);
			auto config = SceneConfig::Window(SceneResolution::Display);
			config.scalingFilter = TextureFilter::Nearest;
			Scene::Configure(config);
			check(config, windowScene);
			Scene::Configure(Scene::GetConfig());
			check(config, windowScene);
			const auto full = Window::GetState();
			const auto checkUnmoved = [&]
			{
				CHECK(Window::GetState().fullscreen);
				CHECK(Window::GetState().bounds == full.bounds);
				CHECK(Window::GetState().virtualSize == full.virtualSize);
				CHECK(Window::GetState().frameBufferSize == full.frameBufferSize);
				CHECK(Scene::GetConfig() == config);
			};
			CHECK_FALSE(Window::CenterToScreen());
			checkUnmoved();
			CHECK_FALSE(Window::CenterToScreen((System::GetCurrentMonitorIndex() + 1) % monitors.size()));
			checkUnmoved();
			CHECK_FALSE(Window::CenterToScreen(std::numeric_limits<size_t>::max()));
			checkUnmoved();
			// Default CenterWindow::Yes must not turn a same-size resize into an exit.
			CHECK(Window::Resize(full.virtualSize));
			CHECK(Window::ResizeVirtual(full.virtualSize));
			CHECK(Window::ResizeActual(full.frameBufferSize));
			checkUnmoved();
			check(config, windowScene);
			if (enterByShortcut) { SetFullscreenWithPause(false); }
			else { shortcut(); }
			REQUIRE_FALSE(Window::GetState().fullscreen);
			check(config, Window::GetState().virtualSize);
			UpdateWithPause();
			check(config, Window::GetState().virtualSize);
			REQUIRE(Window::CenterToScreen());
			REQUIRE(Window::CenterToScreen(System::GetCurrentMonitorIndex()));
			CHECK_FALSE(Window::CenterToScreen(std::numeric_limits<size_t>::max()));
			CHECK_FALSE(Window::GetState().fullscreen);
			CHECK(Scene::GetConfig() == config);
		}
	}
}

# endif
