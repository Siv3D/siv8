# Scene high resolution and input

This program checks scene layout and pointer input while the OS changes the
window's size or DPI. It complements the numerical and image comparisons in
[Test_SceneHighResolution.cpp](../Test_SceneHighResolution.cpp).
See the [usage and design guide](../../docs/renderer2d/scene-high-resolution.md).

## Execution

Build the complete program below in a Siv3D application on each target platform.
Do not replace a repository platform `Main.cpp` permanently with this program.
Use a resizable window and, where available, monitors with different scale factors.

1. Start in fixed 800 × 600 mode. Press **H** to toggle high resolution rendering.
   Compare the displayed logical and render sizes, circle edges, text, and hover
   feedback. Click the center circle and check the click count.
2. Resize the window continuously, including wide, tall, and smaller-than-canvas
   sizes. Move between different-DPI monitors and change the OS display scale.
   Repeat with **1** (Virtual), **2** (Actual), and **3** (Keep, 800 × 600).
3. Use **4** (virtual 800 × 600), **5** (virtual 1200 × 900), and **6** (actual
   1200 × 900). Check the size displays and input immediately after each change.
4. Use **M** to maximize, **R** to restore, **N** to minimize, and the OS to bring
   the window back. For every resize mode with high resolution both on and off,
   test **F** and, on Windows, **Alt+Enter** to enter and leave fullscreen.
   Also enter with one method and leave with the other. The selected resize mode,
   high resolution setting, and texture filter must remain unchanged. While
   fullscreen, change the mode or toggle **H**, then return to windowed mode;
   the new settings must persist. Press **T** to disable the Windows shortcut:
   **Alt+Enter** must do nothing, while **F** must still work. Re-enabling **T**
   must not execute a previously ignored shortcut.
   Repeat on monitors to the right, left, above, and below the primary monitor,
   including different DPI settings. Fullscreen must cover exactly the selected
   monitor without changing the OS display mode or rearranging other monitors,
   and returning must restore the window's position and dimensions. On macOS,
   confirm the selected Retina desktop scale is retained. While fullscreen,
   use **4/5/6** to resize: a changed size must return to a window; the scene
   policy must stay selected.
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

## Expected results

- Toggling H preserves logical size, layout, camera view, hover targets, and cursor
  position. The text and curves can become sharper; bitmap assets are not rebuilt.
- Virtual follows the observed virtual window size. Actual follows framebuffer
  pixels without another DPI multiplication. Keep preserves 800 × 600.
- Fullscreen preserves the current scene policy. Virtual/Actual follow the
  fullscreen dimensions; Keep retains its logical size. High resolution and
  filtering remain as selected. **P** switches Linear/Nearest for this check.
- With high resolution enabled, render size follows the fitted display area,
  never falling below logical size. Wide/tall windows keep centered letterboxing.
- The red border follows the scene bounds and all four yellow corner markers
  remain inside it. The cyan pointer marker aligns with hover/click feedback,
  including through camera transforms. Letterbox coordinates are not clamped.
- Minimize/restore does not create a zero-sized scene or corrupt later frames.
  Fullscreen and monitor transitions leave rendering and input aligned.
- Without camera movement, a toggle changes detail rather than object size or
  pattern spacing. Print remains readable in the logical scene coordinate system.

Record backend, OS scale, observed virtual/framebuffer/logical/render sizes, mode,
and operation when a result differs. OS transition checks require a real display;
passing the automated suite alone does not establish them.

## Complete program

```cpp
# include <Siv3D.hpp>

void Main()
{
    Window::SetStyle(WindowStyle::Sizable);
    Window::Resize(800, 600);
    Scene::SetResizeMode(ResizeMode::Keep);
    Scene::Resize(800, 600);
    Scene::SetHighResolutionEnabled(true);
    Scene::SetBackground(ColorF{ 0.12, 0.15, 0.2 });
    Scene::SetLetterbox(ColorF{ 0.3, 0.05, 0.1 });

    const Font font{ FontMethod::MSDF, 28 };
    Camera2D camera{ Scene::CenterF(), 1.0 };
    bool useCamera = false;
    int32 clicks = 0;
    Size captureSize{ 0, 0 };

    while (System::Update())
    {
        if (ScreenCapture::HasNewFrame())
        {
            captureSize = ScreenCapture::GetFrame().size();
        }
        if (KeyH.down()) { Scene::SetHighResolutionEnabled(!Scene::IsHighResolutionEnabled()); }
        if (Key1.down()) { Scene::SetResizeMode(ResizeMode::Virtual); }
        if (Key2.down()) { Scene::SetResizeMode(ResizeMode::Actual); }
        if (Key3.down())
        {
            Scene::SetResizeMode(ResizeMode::Keep);
            Scene::Resize(800, 600);
        }
        if (Key4.down()) { Window::Resize(800, 600); }
        if (Key5.down()) { Window::Resize(1200, 900); }
        if (Key6.down()) { Window::ResizeActual(1200, 900); }
        if (KeyM.down()) { Window::Maximize(); }
        if (KeyR.down()) { Window::Restore(); }
        if (KeyN.down()) { Window::Minimize(); }
        if (KeyF.down()) { Window::SetFullscreen(!Window::GetState().fullscreen); }
        if (KeyB.down() && !Window::GetState().fullscreen)
        {
            const auto monitors = System::EnumerateMonitors();
            if (!monitors.isEmpty())
            {
                Window::CenterToScreen((System::GetCurrentMonitorIndex() + 1) % monitors.size());
            }
        }
        if (KeyT.down()) { Window::SetToggleFullscreenEnabled(!Window::IsToggleFullscreenEnabled()); }
        if (KeyP.down())
        {
            Scene::SetTextureFilter((Scene::GetTextureFilter() == TextureFilter::Linear)
                ? TextureFilter::Nearest : TextureFilter::Linear);
        }
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
            font(U"Scene high resolution").draw(24, Scene::CenterF() + Vec2{ -150, -130 });
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
        Print << U"H: high resolution = " << Scene::IsHighResolutionEnabled();
        Print << U"1/2/3: mode = " << Scene::GetResizeMode();
        Print << U"T: Alt+Enter = " << Window::IsToggleFullscreenEnabled()
              << U"  P: filter = " << Scene::GetTextureFilter();
        Print << U"L: " << Scene::Size() << U"  R: " << Scene::RenderSize();
        Print << U"Window: " << Window::GetState().virtualSize
              << U"  framebuffer: " << Window::GetState().frameBufferSize;
        Print << U"B: monitor = " << System::GetCurrentMonitorIndex()
              << U"  bounds: " << Window::GetState().bounds;
        Print << U"Cursor desktop: " << Cursor::ScreenPos() << U"  client pixels: " << Cursor::PosRaw();
        Print << U"C: camera = " << useCamera << U"  clicks: " << clicks;
        Print << U"S: last capture = " << captureSize;
        Print << U"4/5/6: resize  M/R/N: window  F: fullscreen";
    }
}
```
