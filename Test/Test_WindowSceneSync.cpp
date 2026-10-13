//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Window/IWindow.hpp>
# if SIV3D_PLATFORM(WINDOWS)
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# elif SIV3D_PLATFORM(MACOS)
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <objc/runtime.h>
# include <atomic>
# endif

namespace s3d
{
	namespace
	{
		struct RestoreWindowAndScene
		{
			const WindowState window = Window::GetState();
			const SceneConfig config = Scene::GetConfig();
			const ColorF background = Scene::GetBackground();

			~RestoreWindowAndScene()
			{
				if (Window::GetState().fullscreen)
				{
					Window::SetFullscreen(false);
					System::Sleep(500);
				}
				Window::Restore();
				Window::SetMinimumFrameBufferSize(window.minFrameBufferSize);
				Window::SetStyle(window.style);
				Window::SetPos(window.bounds.pos);
				Window::Resize(window.virtualSize, CenterWindow::No);
				Scene::Configure(config);
				Scene::SetBackground(background);
				if (window.maximized) { Window::Maximize(); }
				if (window.minimized) { Window::Minimize(); }
			}
		};

		void WaitForMinimized(const bool minimized)
		{
			for (int32 attempt = 0; attempt < 6; ++attempt)
			{
				System::Sleep(500);
				REQUIRE(System::Update());
				if (Window::GetState().minimized == minimized) { return; }
			}
			FAIL("The window did not complete the minimize/restore transition");
		}

		void CheckPresentation(const SceneConfig& config)
		{
			const auto& window = Window::GetState();
			REQUIRE_FALSE(window.minimized);
			REQUIRE_FALSE(window.fullscreen);
			const Size scene = (config.sizeMode == SceneSizeMode::Fixed) ? config.fixedSceneSize
				: ((config.sizeMode == SceneSizeMode::Window) ? window.virtualSize : window.frameBufferSize);
			const double scale = Min(double(window.frameBufferSize.x) / scene.x, double(window.frameBufferSize.y) / scene.y);
			const Size resolution = (config.resolution == SceneResolution::SceneSize) ? scene
				: Math::Round(scene * Max(1.0, scale)).asPoint();
			const Vec2 offset = (window.frameBufferSize - scene * scale) * 0.5;
			CHECK(Scene::GetConfig() == config);
			CHECK(Scene::Size() == scene);
			CHECK(Scene::RenderResolution() == resolution);
			CHECK(Scene::ClientToScene(window.frameBufferSize * 0.5).epsilonEquals(scene * 0.5, 1e-5));
			const auto transform = Cursor::GetBaseWindowTransform();
			const auto expected = Mat3x2::Scale(scale).translated(offset);
			CHECK(transform._11 == expected._11);
			CHECK(transform._12 == expected._12);
			CHECK(transform._21 == expected._21);
			CHECK(transform._22 == expected._22);
			// Independent centering arithmetic can leave cancellation residue on the
			// fitted axis. Allow less than one millionth of a pixel in translation.
			CHECK(Vec2{ transform._31, transform._32 }.epsilonEquals(Vec2{ expected._31, expected._32 }, 1e-6));
			const auto all = Cursor::GetLocalTransform() * Cursor::GetCameraTransform() * Cursor::GetBaseWindowTransform();
			CHECK(Cursor::PosF().epsilonEquals(all.inverse().transformPoint(Cursor::PosRaw()), 1e-3));
		}
	}

	TEST_CASE("WindowSceneSync.explicit_geometry_immediate_and_next_frame")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		const Transformer2D local{ Mat3x2::Translate(20, 10), TransformCursor::Yes, Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Scale(1.5), TransformCursor::Yes, Transformer2D::Target::SetCamera };
		int32 step = 0;
		for (const auto& config : { SceneConfig::Fixed({ 320, 240 }), SceneConfig::Fixed({ 320, 240 }, SceneResolution::Display),
			SceneConfig::PixelArt({ 200, 150 }), SceneConfig::Window(), SceneConfig::Window(SceneResolution::Display), SceneConfig::WindowPixels() })
		{
			CAPTURE(config.sizeMode, config.resolution, config.scalingFilter);
			Scene::Configure(config);
			const Size requested{ 640 + step * 2, 480 + step * 2 };
			if ((step % 3) == 0) { REQUIRE(Window::Resize(requested)); }
			else if ((step % 3) == 1) { REQUIRE(Window::ResizeVirtual(requested, CenterWindow::No)); }
			else { REQUIRE(Window::ResizeActual(requested, CenterWindow::No)); }
			CheckPresentation(config);
			Window::SetPos(Window::GetPos() + Point{ 13, 7 });
			CheckPresentation(config);
			REQUIRE(((step % 2) ? Window::CenterToScreen() : Window::CenterToScreen(System::GetCurrentMonitorIndex())));
			CheckPresentation(config);
			const WindowStyle style = (step % 3 == 0) ? WindowStyle::Fixed : ((step % 3 == 1) ? WindowStyle::Sizable : WindowStyle::Frameless);
			Window::SetStyle(style);
			CheckPresentation(config);
			REQUIRE(System::Update());
			CheckPresentation(config);
			++step;
		}
	}

	TEST_CASE("WindowSceneSync.maximize_restore_and_minimize")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		Window::SetStyle(WindowStyle::Sizable);
		REQUIRE(Window::Resize(600, 400, CenterWindow::No));
		for (const auto& config : { SceneConfig::Window(SceneResolution::Display), SceneConfig::Fixed({ 320, 240 }, SceneResolution::Display) })
		{
			Scene::Configure(config);
			Window::Maximize();
			CheckPresentation(config);
			System::Sleep(500);
			REQUIRE(System::Update());
			CheckPresentation(config);
			Window::Restore();
			CheckPresentation(config);
			System::Sleep(500);
			REQUIRE(System::Update());
			CheckPresentation(config);
		}
		const Size scene = Scene::Size(), resolution = Scene::RenderResolution();
		const auto config = Scene::GetConfig();
		Window::Minimize();
		CHECK(Scene::Size() == scene);
		CHECK(Scene::RenderResolution() == resolution);
		WaitForMinimized(true);
		CHECK(Window::GetState().minimized);
		CHECK(Scene::GetConfig() == config);
		CHECK(Scene::Size() == scene);
		CHECK(Scene::RenderResolution() == resolution);
		Window::Restore();
		if (Window::GetState().minimized)
		{
			CHECK(Scene::Size() == scene);
			CHECK(Scene::RenderResolution() == resolution);
		}
		else { CheckPresentation(config); }
		WaitForMinimized(false);
		CheckPresentation(config);
	}

	TEST_CASE("WindowSceneSync.rejected_center_applies_observed_geometry")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		REQUIRE(Window::Resize(600, 400, CenterWindow::No));
		const auto config = SceneConfig::Window(SceneResolution::Display);
		Scene::Configure(config);
		// Native work publishes geometry without applying Scene. A rejected public
		// operation must still synchronize that observed state before returning.
		REQUIRE(SIV3D_ENGINE(Window)->resizeByVirtualSize({ 620, 420 }));
		REQUIRE(Scene::Size() == Size{ 600, 400 });
		const Rect bounds = Window::GetState().bounds;
		CHECK_FALSE(Window::CenterToScreen(std::numeric_limits<size_t>::max()));
		CHECK(Window::GetState().bounds == bounds);
		CheckPresentation(config);
		REQUIRE(System::Update());
		CheckPresentation(config);
	}

	TEST_CASE("WindowSceneSync.invalid_and_same_size_requests")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		REQUIRE(Window::Resize(600, 400, CenterWindow::No));
		Scene::Configure(SceneConfig::Window(SceneResolution::Display));
		const auto window = Window::GetState();
		const auto config = Scene::GetConfig();
		const Size scene = Scene::Size(), resolution = Scene::RenderResolution();
		const auto cursor = Cursor::GetBaseWindowTransform();
		using ResizeFunction = bool(*)(Size, CenterWindow);
		const ResizeFunction functions[] = { Window::Resize, Window::ResizeVirtual, Window::ResizeActual };
		for (int32 api = 0; api < 3; ++api)
		{
			for (const Size invalid : { Size{ 0, 400 }, Size{ 600, -1 }, Size{ 8193, 400 }, Size{ 600, 8193 } })
			{
				CAPTURE(api, invalid);
				CHECK_THROWS_AS(functions[api](invalid, CenterWindow::Yes), Error);
				CHECK(Window::GetState().bounds == window.bounds);
				CHECK(Window::GetState().frameBufferSize == window.frameBufferSize);
				CHECK(Window::GetState().fullscreen == window.fullscreen);
				CHECK(Scene::GetConfig() == config);
				CHECK(Scene::Size() == scene);
				CHECK(Scene::RenderResolution() == resolution);
				CHECK(Cursor::GetBaseWindowTransform() == cursor);
			}
			REQUIRE(functions[api]((api == 2) ? window.frameBufferSize : window.virtualSize, CenterWindow::No));
			CHECK(Window::GetState().bounds == window.bounds);
		}
		Window::SetPos(window.bounds.pos + Point{ 17, 11 });
		REQUIRE(Window::Resize(window.virtualSize));
		const Rect centered = Window::GetState().bounds;
		REQUIRE(Window::CenterToScreen());
		CHECK(Window::GetState().bounds == centered);
		CHECK_FALSE(Window::CenterToScreen(std::numeric_limits<size_t>::max()));
		CHECK(Window::GetState().bounds == centered);
		CheckPresentation(config);
	}

# if SIV3D_PLATFORM(WINDOWS)

	TEST_CASE("WindowSceneSync.below_minimum_resize_keeps_restoration")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		Window::SetStyle(WindowStyle::Sizable);
		Window::SetMinimumFrameBufferSize({ 200, 200 });
		REQUIRE(Window::Resize(600, 400, CenterWindow::No));
		const auto config = SceneConfig::Window(SceneResolution::Display);
		Scene::Configure(config);
		const auto windowed = Window::GetState();
		Window::Maximize();
		System::Sleep(500);
		REQUIRE(Window::GetState().maximized);
		CHECK_FALSE(Window::Resize(1, 1));
		CHECK_FALSE(Window::GetState().maximized);
		CHECK(Window::GetState().bounds == windowed.bounds);
		CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
		CheckPresentation(config);
		REQUIRE(System::Update());
		CheckPresentation(config);
	}

# elif SIV3D_PLATFORM(MACOS)

	namespace
	{
		// Confine the failure to the real renderer's device and this lexical scope.
		// This exercises allocation-error propagation without exhausting GPU memory
		// or adding a production failure flag.
		class TextureAllocationFailure
		{
		public:

			std::atomic<int32> attempts = 0;

			explicit TextureAllocationFailure(MTL::Device* device)
			{
				id target = (__bridge id)device;
				m_method = class_getInstanceMethod(object_getClass(target), @selector(newTextureWithDescriptor:));
				REQUIRE(m_method);
				m_original = method_getImplementation(m_method);
				typedef id<MTLTexture> (*NewTexture)(id, SEL, MTLTextureDescriptor*) NS_RETURNS_RETAINED;
				const auto original = reinterpret_cast<NewTexture>(m_original);
				auto* count = &attempts;
				m_replacement = imp_implementationWithBlock(^id<MTLTexture>(id receiver, MTLTextureDescriptor* descriptor)
				{
					if (receiver == target)
					{
						++(*count);
						return nil;
					}
					return original(receiver, @selector(newTextureWithDescriptor:), descriptor);
				});
				REQUIRE(m_replacement);
				method_setImplementation(m_method, m_replacement);
			}

			~TextureAllocationFailure()
			{
				method_setImplementation(m_method, m_original);
				imp_removeBlock(m_replacement);
			}

		private:

			Method m_method = nullptr;
			IMP m_original = nullptr;
			IMP m_replacement = nullptr;
		};
	}

	TEST_CASE("WindowSceneSync.macOS.center_propagates_scene_allocation_error")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const auto config = SceneConfig::Window();
		for (const bool indexed : { false, true })
		{
			CAPTURE(indexed);
			REQUIRE(Window::Resize(600, 400, CenterWindow::No));
			Scene::Configure(config);
			const auto scene = renderer->getSceneTextureMSAA();
			const auto resolved = renderer->getSceneTextureNonMSAA();
			REQUIRE(SIV3D_ENGINE(Window)->resizeByVirtualSize({ 620, 420 }));
			const auto nativeSize = Window::GetState().virtualSize;
			{
				TextureAllocationFailure failure{ renderer->getDevice() };
				if (indexed) { CHECK_THROWS_AS(Window::CenterToScreen(System::GetCurrentMonitorIndex()), Error); }
				else { CHECK_THROWS_AS(Window::CenterToScreen(), Error); }
				CHECK(failure.attempts.load() == 1);
			}
			CHECK(Window::GetState().virtualSize == nativeSize);
			CHECK(Scene::GetConfig() == config);
			CHECK(Scene::Size() == Size{ 600, 400 });
			CHECK(renderer->getSceneTextureMSAA().getTexture() == scene.getTexture());
			CHECK(renderer->getSceneTextureNonMSAA().getTexture() == resolved.getTexture());
			// A subsequent operation can apply the new geometry after allocation recovers.
			REQUIRE(Window::CenterToScreen());
			CheckPresentation(config);
			REQUIRE(System::Update());
			CheckPresentation(config);
		}
	}

# endif

	TEST_CASE("WindowSceneSync.placement_across_monitors", "[.screen-coordinates]")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		REQUIRE(Window::Resize(600, 400, CenterWindow::No));
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		for (const auto& config : { SceneConfig::Fixed({ 320, 240 }, SceneResolution::Display),
			SceneConfig::Window(SceneResolution::Display), SceneConfig::WindowPixels() })
		{
			Scene::Configure(config);
			for (const bool center : { false, true })
			{
				// Close each route so both APIs cross DPI boundaries in both directions,
				// including when only two monitors are attached.
				const size_t moves = (monitors.size() + (monitors.size() > 1));
				for (size_t step = 0; step < moves; ++step)
				{
					const size_t i = (step % monitors.size());
					const double sourceScaling = Window::GetState().scaling;
					CAPTURE(center, i, monitors[i].name, monitors[i].displayRect, config.sizeMode);
					if (center) { REQUIRE(Window::CenterToScreen(i)); }
					else { Window::SetPos(monitors[i].workArea.pos + Point{ 40, 40 }); }
					CHECK(System::GetCurrentMonitorIndex() == i);
					Catch::cout() << (center ? "CenterToScreen" : "SetPos") << " monitor[" << i
						<< "] scaling " << sourceScaling << " -> " << Window::GetState().scaling
						<< " mode=" << static_cast<int32>(config.sizeMode) << '\n';
					CheckPresentation(config);
					System::Sleep(500);
					REQUIRE(System::Update());
					CheckPresentation(config);
				}
			}
		}
	}

	TEST_CASE("WindowSceneSync.fullscreen_resize_uses_final_geometry", "[.fullscreen]")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		REQUIRE(Window::Resize(600, 400, CenterWindow::No));
		Window::SetFullscreen(true);
		System::Sleep(500);
		REQUIRE(System::Update());
		REQUIRE(Window::GetState().fullscreen);
		const auto fullscreen = Window::GetState();
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		Window::SetFullscreen(true, (System::GetCurrentMonitorIndex() + 1) % monitors.size());
		CHECK(Window::GetState().bounds == fullscreen.bounds);
		CHECK(Window::GetState().frameBufferSize == fullscreen.frameBufferSize);
		CHECK(Window::GetState().fullscreen);
		Scene::SetBackground(Palette::Black);
		Scene::Configure(SceneConfig::Fixed({ 640, 480 }));
		const auto config = SceneConfig::Window();
		Scene::Configure(config); // KeepSize retains 640 x 480 in fullscreen.
		REQUIRE(Scene::Size() == Size{ 640, 480 });
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		const ScopedViewport2D viewport{ none };
		const ScopedScissorRect2D scissor{ none };
# if SIV3D_PLATFORM(WINDOWS)
		auto& buffer = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer))->getBackBuffer();
		const ComPtr<ID3D11Texture2D> texture{ buffer.getSceneTexture() };
# elif SIV3D_PLATFORM(MACOS)
		auto* renderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		const auto texture = renderer->getSceneTextureMSAA();
		const auto resolved = renderer->getSceneTextureNonMSAA();
# endif
		Scene::Rect().draw(Palette::Red);
		Graphics2D::Flush();
		Rect{ 0, 0, 32, 32 }.draw(Palette::Blue);
		// Restoring 600 x 400 internally must not replace/clear the 640 x 480 scene.
		REQUIRE(Window::Resize(640, 480));
		System::Sleep(500);
		REQUIRE_FALSE(Window::GetState().fullscreen);
		CheckPresentation(config);
# if SIV3D_PLATFORM(WINDOWS)
		CHECK(buffer.getSceneTexture() == texture.Get());
# elif SIV3D_PLATFORM(MACOS)
		CHECK(renderer->getSceneTextureMSAA().getTexture() == texture.getTexture());
		CHECK(renderer->getSceneTextureNonMSAA().getTexture() == resolved.getTexture());
# endif
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		const auto& image = ScreenCapture::GetFrame();
		REQUIRE(image.size() == Size{ 640, 480 });
		CHECK(image[240][320] == Palette::Red);
		CHECK(image[16][16] == Palette::Blue);
		CheckPresentation(config);
	}
}
