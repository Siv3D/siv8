# Scene high resolution rendering

Enable high resolution rendering before drawing:

```cpp
Scene::SetHighResolutionEnabled(true);
```

With the default `ResizeMode::Virtual`, existing layout based on `Scene::Size()`,
`Scene::Center()`, and `Scene::Rect()` keeps using the window's virtual dimensions.
The engine allocates more scene pixels when the display provides them, and keeps
rendering and cursor coordinates aligned. An additional `Transformer2D` for DPI
scaling is unnecessary.

For a fixed game canvas that becomes sharper in a larger window:

```cpp
Scene::SetResizeMode(ResizeMode::Keep);
Scene::Resize(800, 600);
Scene::SetHighResolutionEnabled(true);
Window::Resize(1200, 900);
```

Continue drawing in the 800 × 600 coordinate system. `Scene::Resize()` specifies
that coordinate system; it does not select `Keep` automatically. `Actual` instead
follows the framebuffer dimensions, so enabling high resolution rendering does
not apply another DPI multiplier. Disabling the feature keeps the logical size.

The authoritative contracts, including immediate updates, clearing on size
changes, size limits, and allocation failure, are in
[Scene.hpp](../../Siv3D/include/Siv3D/Scene.hpp). Change the configuration before
issuing that frame's draws. Reapplying an unchanged size does not clear the scene.

## Coordinate and pixel sizes

| Use | API |
| --- | --- |
| Scene layout, centers, and hit testing | `Scene::Size()` and its related getters |
| Default camera drawing area | `Graphics2D::GetDrawingSize()` |
| Scene pixel dimensions and captured image dimensions | `Scene::RenderSize()` |
| Current render target's pixel dimensions | `Graphics2D::GetRenderTargetSize()` |

When migrating code that scales rendering and cursor coordinates manually,
remove both high-resolution adjustments together. Keep transforms used for world
coordinates, object placement, and camera movement.

Code that used `Scene::Size()` for texel offsets or buffer allocation should use
the appropriate pixel-size getter. Viewports and scissor rectangles remain in
render-target pixels; they do not become logical rectangles. A custom fragment
shader's raster position also remains in pixels. A custom vertex shader receives
the scene scaling through the standard engine transform; a shader that ignores
that transform must implement its own coordinate mapping.

Textures and bitmap glyphs retain their source resolution. Use suitable assets
or MSDF fonts when crisp magnification matters. High resolution rendering also
increases render-target memory and pixel work; doubling both dimensions means
four times as many pixels, with additional storage for MSAA.

## Rendering design

[SceneUtility](../../Siv3D/src/Siv3D/Scene/SceneUtility.hpp) holds the logical size
L, last usable window geometry, and configuration. For framebuffer size F, the
presentation scale is `s = min(F.x / L.x, F.y / L.y)`. The scene is centered with
its aspect ratio preserved. When enabled, render size R is the nearest integer
size to `L * max(1, s)`. Otherwise R equals L. Shrinking the window does not reduce
R below L. A minimized or zero-sized window retains the last usable geometry.

Presentation and cursor mapping depend on L and F, independently of the integer
rounding of R. Rendering uses the rounded ratios `R.x / L.x` and `R.y / L.y`.
Thus rounding does not shift letterboxing or input coordinates.

The renderer owns the resolution scale separately from user transforms:

```text
Local → Camera → QuadWarp → Scene resolution scale → Viewport projection
```

`SetLocal`, `SetCamera`, scope restoration, and `Graphics2D::Flush()` preserve
that scale. It is incorporated into the existing transform constants, without
new vertex attributes or an extra drawing pass. Recorded transforms contain
Local / Camera / QuadWarp; scene scaling is resolved when those commands execute
against the current target. Internal curve subdivision uses the raster scale
at draw recording time; public `GetRMSScaling()` retains its Local/Camera meaning.
Explicitly constructed CPU geometry retains its requested quality.

Window events and explicit resize APIs share the same size calculation. In
particular, `ResizeActual()` with `Virtual` uses the observed virtual size, not
the physical-size argument. DPI is derived from the actual framebuffer geometry
rather than multiplying a nominal OS scale again.

Backends allocate replacement buffers before committing a change. Resizing does
not force a flush or discard pending draw commands. Those commands execute against
the current scene buffer using its resolution scale. Recorded geometry, user
transforms, physical viewports/scissors, and subdivision counts retain their
original values. A change to either logical or render size clears pixels from
already executed draws, including draws submitted by an explicit
`Graphics2D::Flush()`. Resizing cannot reconstruct those pixels.

For example, drawing an 800 × 600 rectangle and then resizing the logical scene
to 1600 × 1200 leaves an 800 × 600 rectangle in the pending commands. It is drawn
into the resized scene. If it was already flushed, its pixels are lost instead.
Configure sizes before drawing to avoid mixing layout based on different sizes.
OS-driven window resizing is applied during preparation of the next frame, after
the preceding frame has been presented.

Fullscreen switches preserve the scene resize mode, high resolution setting,
and texture filter. Windows Alt+Enter uses the same transition as
`Window::SetFullscreen()`. Keep retains its logical canvas, while Virtual and
Actual follow the fullscreen window's virtual and framebuffer dimensions.
Settings changed while fullscreen remain selected after returning to windowed
mode. Applications that expect Alt+Enter to freeze the logical canvas should
select Keep explicitly.

The cursor's base transform is refreshed without consuming an input frame.
Unchanged configurations reuse the buffers and cached transform state.

## Validation

[Test_SceneHighResolution.cpp](../../Test/Test_SceneHighResolution.cpp) covers
size calculation, DPI-only changes, all resize modes, rounding, invalid sizes,
configuration round trips, immediate cursor mapping, camera defaults, physical
viewport/scissor coordinates, pending and flushed draws, and window resize APIs.
They also check repeated target replacement across a vertex batch boundary,
texture lifetime, and clipping rectangles outside a shrunken target.
Image comparisons check shapes, patterns, bitmap/MSDF text, projective transforms,
and a custom vertex shader against explicit scaling at the same resolution.
Frame metrics compare subdivision and draw counts.

[Test_WindowFullscreen.cpp](../../Test/Test_WindowFullscreen.cpp) runs on
Windows and exercises the window-message route for Alt+Enter, mixed API/shortcut
round trips, all resize modes with high resolution on/off, policy changes while
fullscreen, disabled shortcuts, and fullscreen bounds on each attached monitor.
Nonzero and negative monitor origins require the corresponding display layout.

Run the focused tests and then the full suite on the respective host:

```sh
./macOS/run-tests.sh '--test-case=SceneHighResolution*'
./macOS/run-tests.sh
```

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=SceneHighResolution*'
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=WindowFullscreen.*'
./WindowsDesktop/run-tests.ps1
```

[The interactive check](../../Test/Manual/SceneHighResolution.md) covers OS
resizing, monitor changes, fullscreen, minimization, and pointer interaction.
Numerical simulation of DPI changes does not replace these OS checks.
Pending platform verification and integration work belong in [TODO](../../TODO.md).
