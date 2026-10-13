# Scene configuration and input

This program checks scene layout and pointer input while the OS changes the
window's size or DPI. It complements the numerical and image comparisons in
[Test_SceneHighResolution.cpp](../Test_SceneHighResolution.cpp).
See the [usage and design guide](../../docs/renderer2d/scene-high-resolution.md).

## Execution

Build the complete program below in a Siv3D application on each target platform.
Do not replace a repository platform `Main.cpp` permanently with this program.
Use a resizable window and, where available, monitors with different scale factors.
Pass `--preset=1` through `--preset=6` to select the initial configuration (default: 2).
Pass `--hide-status` to inspect the drawing without diagnostic text, especially
for the small PixelArt scene. These options also allow repeatable startup checks
when injected key presses are shorter than the engine input sampling interval.

1. Start in fixed 800 × 600 mode at display resolution. Keys **1–6** select the
   six configurations in the usage guide, in the same order. Press **H** to switch
   between SceneSize and Display resolution.
   Compare the displayed scene size and render resolution, circle edges, text, and hover
   feedback. Click the center circle and check the click count.
2. Resize the window continuously, including wide, tall, and smaller-than-canvas
   sizes. Move between different-DPI monitors and change the OS display scale.
   Repeat with all six configurations, including PixelArt and WindowPixels.
3. Use **7** (`Resize`, virtual 800 × 600), **8** (`ResizeVirtual`, 800 × 600),
   and **9** (`ResizeActual`, 800 × 600 pixels). All use `CenterWindow::No`.
   **0** requests a larger virtual 1200 × 900 window. Check the size displays and
   input immediately after each change.
4. Use **M** to maximize, **R** to restore, **N** to minimize, and the OS to bring
   the window back. For all six configurations, and both fullscreen sizing policies selected by **U**,
   test **F** and the platform shortcut (**Alt+Enter** on Windows,
   **Control+Command+F** on macOS) to enter and leave fullscreen.
   On macOS, also enter using the Window menu's fullscreen
   command, then leave with the shortcut or menu. Check Fixed and Sizable styles;
   change the initial `Window::SetStyle()` argument and rerun for Fixed and
   Frameless. Use the shortcut/menu for Frameless. No separate fullscreen Space should appear.
   Hold the shortcut to confirm that macOS toggles only once.
   Also enter with one method and leave with the other. The selected resize mode,
   resolution policy, fullscreen sizing policy, and scaling filter must remain unchanged. While
   fullscreen, change the mode or toggle **H**, then return to windowed mode;
   the new settings must persist. Press **T** to disable the platform controls:
   the shortcut must do nothing, while **F** must still work. On macOS the
   fullscreen menu must be disabled too. Re-enabling **T** must not execute a
   previously ignored shortcut. The native green button and Window menu's Zoom
   command must maximize a Sizable window regardless of **T**. In Fixed style,
   the green button stays disabled while the enabled fullscreen shortcut/menu
   still work. Check that the title bar has no extra button covering its title.
   Repeat on monitors to the right, left, above, and below the primary monitor,
   including different DPI settings. Fullscreen must cover exactly the selected
   monitor without changing the OS display mode or rearranging other monitors,
   and returning must restore the window's position and dimensions. On macOS,
   confirm the selected Retina desktop scale is retained. While fullscreen,
   use **7/8/9** to resize: a changed size must return to a window; the scene
   policy must stay selected.
   For the mixed-DPI resize check, place the window on monitor A using **B**, then
   press **G** to enter fullscreen on the next monitor B without moving the
   window first (**Shift+G** selects the previous monitor). Press **7/8/9** to return.
   While fullscreen, **G** / **Shift+G** also move directly to another monitor.
   Move several times, then leave using **F**, the shortcut, or **7/8/9**. The
   restoration target must remain the original window on A. Check that KeepSize
   retains the scene size, FollowWindow follows the destination, and rendering
   and hover/click coordinates agree immediately after moving.
   The original window position on A must be retained. On Windows, with A at 125%,
   **7/8** must give virtual 800 × 600 and framebuffer
   1000 × 750; **9** must give framebuffer 800 × 600 and virtual 640 × 480.
   Repeat with A at 150% and B at 125%: **7/8** must give 800 × 600 / 1200 × 900,
   and **9** must give 533 × 400 / 800 × 600. Read the immediate and next-frame
   snapshots, then check rendering and click the circle. Change the scene mode,
   **H**, or **P** while fullscreen and confirm the new settings remain selected.
5. Press **C** to enable camera controls, then use the wheel and right-drag to
   zoom and move. Toggle **H** during a drag and check that the same point stays
   under the cursor. Press **C** again to return to scene coordinates.
6. Press **S** to capture a scene frame in memory. On the next frame, the capture
   dimensions should match the render size at the time of the request. Letterbox
   margins are not part of the capture. No output file is created.
7. In windowed mode, press **B** to center the window on the next monitor. On
   macOS, check that the outer frame is centered in the work area even when the
   monitor scales differ, and that the virtual client size stays the same.
   Repeat with displays left/right/above/below the primary display. Move the
   pointer across monitor boundaries, including outside the application window:
   desktop coordinates should stay continuous in points; raw client coordinates
   should remain pixels at the window's backing scale. See the
   [coordinate guide](../../docs/window/README.md) for units.
8. On macOS, add `Window::SetMinimumFrameBufferSize({ 321, 241 });` after the
   initial `Window::SetStyle()` call. Shrink the window with its border on each
   attached monitor. At 2×, the native minimum is 161 × 121 points (322 × 242
   backing pixels); at 1× it is 321 × 241 points/pixels. A native title-bar minimum
   can further restrict width. Move between scales without setting the minimum
   again, then repeat after a fullscreen round trip. The pixel request must
   remain 321 × 241 while the point limit adapts.
9. On macOS, enter fullscreen on an external monitor, then physically disconnect
   it. Rendering and input must continue in a window on a remaining display.
   With preset **3** or **4**, the scene size must resume following the window;
   the selected configuration and interpolation must remain unchanged. Check
   **B** placement, border resizing, **7** resizing, and hover/click alignment.
   Reconnect the monitor, then use the shortcut and **F** for another fullscreen
   round trip. Record the display layout and scale before and after disconnection.

## Expected results

- Toggling H preserves scene size, layout, camera view, hover targets, and cursor
  position. The text and curves can become sharper; bitmap assets are not rebuilt.
- Window follows the observed window size excluding DPI scale. WindowPixels
  follows framebuffer pixels. Fixed retains the specified scene size.
- Fullscreen preserves the configuration. Window presets retain their current
  scene size; WindowPixels follows fullscreen pixels. **U** changes the fullscreen
  sizing policy. Changes made in fullscreen must remain selected after exiting.
- Display resolution follows the fitted display area, never falling below scene
  size. Wide/tall windows keep centered letterboxing. PixelArt uses Nearest but
  does not guarantee integer scaling. **P** switches Linear/Nearest.
- The red border follows the scene bounds and all four yellow corner markers
  remain inside it. The cyan pointer marker aligns with hover/click feedback,
  including through camera transforms. Letterbox coordinates are not clamped.
- Minimize/restore does not create a zero-sized scene or corrupt later frames.
  Fullscreen and monitor transitions leave rendering and input aligned.
- Without camera movement, a toggle changes detail rather than object size or
  pattern spacing. Print remains readable in the scene coordinate system.

Record backend, OS scale, observed window/framebuffer/scene/render sizes, mode,
and operation when a result differs. OS transition checks require a real display;
passing the automated suite alone does not establish them.

## Complete program

```cpp
# include <Siv3D.hpp>

void Main()
{
    Window::SetStyle(WindowStyle::Sizable);
    Window::Resize(800, 600);
    const std::array configurations{
        SceneConfig::Fixed({ 800, 600 }),
        SceneConfig::Fixed({ 800, 600 }, SceneResolution::Display),
        SceneConfig::Window(),
        SceneConfig::Window(SceneResolution::Display),
        SceneConfig::PixelArt({ 200, 150 }),
        SceneConfig::WindowPixels(),
    };
    const std::array presetKeys{ Key1, Key2, Key3, Key4, Key5, Key6 };
    const auto& arguments = System::GetCommandLineArgs();
    size_t initialPreset = 1;
    for (size_t i = 0; i < configurations.size(); ++i)
    {
        if (arguments.contains(U"--preset={}"_fmt(i + 1))) { initialPreset = i; }
    }
    const bool showStatus = !arguments.contains(U"--hide-status");
    Scene::Configure(configurations[initialPreset]);
    Scene::SetBackground(ColorF{ 0.12, 0.15, 0.2 });
    Scene::SetLetterbox(ColorF{ 0.3, 0.05, 0.1 });

    const Font font{ FontMethod::MSDF, 28 };
    Camera2D camera{ Scene::CenterF(), 1.0 };
    bool useCamera = false;
    int32 clicks = 0;
    Size captureSize{ 0, 0 };
    String resizeImmediate, resizeNextFrame;
    bool recordNextFrame = false;
    const auto geometry = []
    {
        const auto& window = Window::GetState();
        return U"monitor={} scale={} full={} V={} F={} L={} R={}"_fmt(
            System::GetCurrentMonitorIndex(), window.scaling, window.fullscreen,
            window.virtualSize, window.frameBufferSize, Scene::Size(), Scene::RenderResolution());
    };

    while (System::Update())
    {
        if (recordNextFrame)
        {
            resizeNextFrame = geometry();
            recordNextFrame = false;
        }
        if (ScreenCapture::HasNewFrame())
        {
            captureSize = ScreenCapture::GetFrame().size();
        }
        auto config = Scene::GetConfig();
        for (size_t i = 0; i < configurations.size(); ++i)
        {
            if (presetKeys[i].down()) { config = configurations[i]; }
        }
        if (KeyH.down())
        {
            config.resolution = (config.resolution == SceneResolution::Display)
                ? SceneResolution::SceneSize : SceneResolution::Display;
        }
        if (KeyU.down())
        {
            config.fullscreen = (config.fullscreen == FullscreenSceneMode::KeepSize)
                ? FullscreenSceneMode::FollowWindow : FullscreenSceneMode::KeepSize;
        }
        if (KeyP.down())
        {
            config.scalingFilter = (config.scalingFilter == TextureFilter::Linear)
                ? TextureFilter::Nearest : TextureFilter::Linear;
        }
        if (config != Scene::GetConfig()) { Scene::Configure(config); }
        if (Key7.down()) { Window::Resize(800, 600, CenterWindow::No); }
        if (Key8.down()) { Window::ResizeVirtual(800, 600, CenterWindow::No); }
        if (Key9.down()) { Window::ResizeActual(800, 600, CenterWindow::No); }
        if (Key0.down()) { Window::Resize(1200, 900, CenterWindow::No); }
        if (Key7.down() || Key8.down() || Key9.down() || Key0.down())
        {
            resizeImmediate = geometry();
            resizeNextFrame.clear();
            recordNextFrame = true;
        }
        if (KeyM.down()) { Window::Maximize(); }
        if (KeyR.down()) { Window::Restore(); }
        if (KeyN.down()) { Window::Minimize(); }
        if (KeyF.down()) { Window::SetFullscreen(!Window::GetState().fullscreen); }
        if (KeyG.down())
        {
            const auto monitors = System::EnumerateMonitors();
            if (!monitors.isEmpty())
            {
                const size_t step = KeyShift.pressed() ? (monitors.size() - 1) : 1;
                Window::SetFullscreen(true, (System::GetCurrentMonitorIndex() + step) % monitors.size());
            }
        }
        if (KeyB.down() && !Window::GetState().fullscreen)
        {
            const auto monitors = System::EnumerateMonitors();
            if (!monitors.isEmpty())
            {
                Window::CenterToScreen((System::GetCurrentMonitorIndex() + 1) % monitors.size());
            }
        }
        if (KeyT.down()) { Window::SetToggleFullscreenEnabled(!Window::IsToggleFullscreenEnabled()); }
        if (KeyC.down())
        {
            useCamera = !useCamera;
            camera.jumpTo(Scene::CenterF(), 1.0);
        }
        if (KeyS.down()) { ScreenCapture::RequestCurrentFrame(); }

        Scene::Rect().drawFrame(4, 0, Palette::Red);
        for (const Vec2 corner : { Vec2{ 12, 12 }, Vec2{ Scene::Width() - 12.0, 12 },
            Vec2{ 12, Scene::Height() - 12.0 }, Vec2{ Scene::Width() - 12.0, Scene::Height() - 12.0 } })
        {
            Circle{ corner, 6 }.draw(Palette::Yellow);
        }

        const auto drawWorld = [&]
        {
            const Circle target{ Scene::CenterF(), 65 };
            target.draw(target.mouseOver() ? Palette::Orange : Palette::White);
            if (target.leftClicked()) { ++clicks; }
            RoundRect{ Scene::CenterF() + Vec2{ -150, 100 }, 300, 70, 16 }.draw(Pattern::Checker{});
            font(U"Scene configuration").draw(24, Scene::CenterF() + Vec2{ -150, -130 });
            Circle{ Cursor::PosF(), 5 }.drawFrame(2, Palette::Cyan);
        };
        if (useCamera)
        {
            camera.update();
            const auto transform = camera.createTransformer();
            drawWorld();
        }
        else
        {
            drawWorld();
        }

        ClearPrint();
        if (showStatus)
        {
            Print << U"H: resolution = " << Scene::GetConfig().resolution;
            Print << U"1..6: preset, size mode = " << Scene::GetConfig().sizeMode;
            Print << U"U: fullscreen sizing = " << Scene::GetConfig().fullscreen;
            Print << U"T: fullscreen controls = " << Window::IsToggleFullscreenEnabled()
                  << U"  P: filter = " << Scene::GetConfig().scalingFilter;
            Print << U"Scene: " << Scene::Size() << U"  render: " << Scene::RenderResolution();
            Print << U"Window: " << Window::GetState().virtualSize
                  << U"  framebuffer: " << Window::GetState().frameBufferSize;
            Print << U"B: monitor = " << System::GetCurrentMonitorIndex()
                  << U"  scale: " << Window::GetState().scaling
                  << U"  full: " << Window::GetState().fullscreen
                  << U"  bounds: " << Window::GetState().bounds;
            Print << U"Cursor desktop: " << Cursor::ScreenPos() << U"  client pixels: " << Cursor::PosRaw();
            Print << U"C: camera = " << useCamera << U"  clicks: " << clicks;
            Print << U"S: last capture = " << captureSize;
            Print << U"7/8/9/0: resize  M/R/N: window  F: fullscreen  (Shift+)G: next/previous fullscreen";
            Print << U"Resize immediate: " << resizeImmediate;
            Print << U"Resize next frame: " << resizeNextFrame;
        }
    }
}
```
