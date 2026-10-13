# Scene configuration and render resolution

Configure scene presentation before drawing:

```cpp
Scene::Configure(SceneConfig::Fixed({ 800, 600 }, SceneResolution::Display));
```

The game uses an 800 × 600 scene coordinate system. The engine chooses a larger
render resolution when the display provides room, keeping drawing and cursor
coordinates aligned. Window size and style remain independent:

```cpp
Window::SetStyle(WindowStyle::Sizable);
Window::Resize(1200, 900);
```

## Six common uses

Pass one of these configurations to `Scene::Configure()`:

| Use | Configuration | Scene size | Render resolution |
| --- | --- | --- | --- |
| Fixed scene | `SceneConfig::Fixed({800, 600})` | 800 × 600 | 800 × 600 |
| Fixed scene at display resolution | `SceneConfig::Fixed({800, 600}, SceneResolution::Display)` | 800 × 600 | Fitted display area, at least the scene size |
| Follow the window (default) | `SceneConfig::Window()` | Window size excluding DPI scale | Scene size |
| Follow the window at display resolution | `SceneConfig::Window(SceneResolution::Display)` | Window size excluding DPI scale | Fitted display area, at least the scene size |
| Pixel art | `SceneConfig::PixelArt({200, 150})` | 200 × 150 | 200 × 150 |
| Window pixels | `SceneConfig::WindowPixels()` | Client framebuffer pixels | Scene size |

`PixelArt()` uses Nearest sampling; the other factories use Linear. Nearest does
not restrict the presentation scale to integers. `Display` refers to the area
occupied by the scene, excluding letterbox margins, and never reduces render
resolution below scene size. An 800 × 600 scene in a 1920 × 1080 framebuffer is
rendered at 1440 × 1080 with `Display`.

Without an explicit call, the engine uses `SceneConfig{}`, equal to
`SceneConfig::Window()`. Window configurations preserve their current scene size
in fullscreen. `WindowPixels()` follows the fullscreen framebuffer instead.
Fixed configurations retain their specified size in either window state.

## Changing configuration

Factories produce values without changing the engine. Edit a value and apply it
once, or read back a copy of the applied configuration:

```cpp
auto config = Scene::GetConfig();
config.resolution = SceneResolution::SceneSize;
Scene::Configure(config);
```

For a fixed configuration, edit `fixedSceneSize` to change the scene dimensions.
For an automatically sized configuration, `fixedSceneSize` is unused and remains
stored as provided. `GetConfig()` returns policy, not a snapshot of current window
or scene dimensions. Use `Scene::Size()` and `Scene::RenderResolution()` for those.

To use the full coordinate range of a fullscreen window:

```cpp
auto config = SceneConfig::Window(SceneResolution::Display);
config.fullscreen = FullscreenSceneMode::FollowWindow;
Scene::Configure(config);
```

Switching to `FollowWindow` while fullscreen immediately follows the current
window geometry. Switching to `KeepSize` preserves the scene size at that moment.
Changing only resolution or filtering under `KeepSize` retains the current size.
Selecting a fixed configuration applies its specified size immediately. Returning
to windowed mode uses the currently selected policy; no previous configuration is
restored.

These six uses are starting points, not an exhaustive list of combinations.
For example, set `scalingFilter = TextureFilter::Nearest` on a window configuration.
Applying a factory result replaces all presentation settings, so switching from
`PixelArt()` to `Fixed()` also selects Linear sampling. Background and letterbox
colors, and window operations, remain independent.

The authoritative contracts, including immediate updates, clearing on size
changes, size limits, and allocation failure, are in
[Scene.hpp](../../Siv3D/include/Siv3D/Scene.hpp) and
[SceneConfig.hpp](../../Siv3D/include/Siv3D/SceneConfig.hpp). Apply changes before
issuing that frame's draws. Unchanged configurations and filter-only changes
preserve already rendered pixels.

## Scene MSAA

Set the sample count independently of the presentation configuration:

```cpp
Scene::SetMSAASampleCount(MSAASampleCount::X2MSAA);
const auto samples = Scene::GetMSAASampleCount();
```

The choices are `NoMSAA` (one sample), 2x, and 4x; the default is 4x. The setting
belongs to the default scene render target, and survives configuration and
window-size changes. Other render targets have independent sample counts.
Unsupported GPU sample counts throw `Error` instead of selecting another count.
The full change and failure contract is in
[Scene.hpp](../../Siv3D/include/Siv3D/Scene.hpp).

Like resizing, a sample-count change preserves pending drawing commands and
clears already executed drawing. Apply it before drawing when the entire frame
should use the new count. Sample counts do not change scene coordinates, render
resolution, or captured image dimensions: `ScreenCapture` returns an image at
`Scene::RenderResolution()` for every supported MSAA setting.

Each backend prepares replacement scene resources before committing the change.
The single-sample presentation/capture texture is reused when its dimensions
match. Metal restarts the scene pass and selects a pipeline cached by sample
count; D3D11 clears the replacement drawing target and uses its existing resolve
paths. Switching does not force a flush or wait for GPU completion.

## Coordinate and pixel sizes

| Use | API |
| --- | --- |
| Scene layout, centers, and hit testing | `Scene::Size()` and its related getters |
| Default camera drawing area | `Graphics2D::GetDrawingSize()` |
| Scene pixel dimensions and captured image dimensions | `Scene::RenderResolution()` |
| Current render target's pixel dimensions | `Graphics2D::GetRenderTargetSize()` |

When migrating code that scales rendering and cursor coordinates manually,
remove both high-resolution adjustments together. Keep transforms used for world
coordinates, object placement, and camera movement.

Code that used `Scene::Size()` for texel offsets or buffer allocation should use
the appropriate pixel-size getter. Viewports and scissor rectangles remain in
render-target pixels rather than scene coordinates. A custom fragment
shader's raster position also remains in pixels. A custom vertex shader receives
the scene scaling through the standard engine transform; a shader that ignores
that transform must implement its own coordinate mapping.

Textures and bitmap glyphs retain their source resolution. Use suitable assets
or MSDF fonts when crisp magnification matters. High resolution rendering also
increases render-target memory and pixel work; doubling both dimensions means
four times as many pixels, with additional storage for MSAA.

## Rendering design

[SceneUtility](../../Siv3D/src/Siv3D/Scene/SceneUtility.hpp) holds the scene size
L, last usable window geometry, and configuration. For framebuffer size F, the
presentation scale is `s = min(F.x / L.x, F.y / L.y)`. The scene is centered with
its aspect ratio preserved. With `SceneResolution::Display`, render resolution R is the nearest integer
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

Window events and explicit geometry APIs share the same size calculation.
Public Window operations apply Scene from their final observed geometry; see
[window operation boundaries](../window/README.md#applying-window-changes-to-scene). In
particular, `ResizeActual()` with `SceneSizeMode::Window` uses the observed virtual size, not
the physical-size argument. DPI is derived from the actual framebuffer geometry
rather than multiplying a nominal OS scale again.

The shared state calculates the final scene size and render resolution from the
complete configuration. Each backend allocates replacement buffers before
committing the configuration, including its scaling filter. There is no sequence
of intermediate setter states or intermediate buffer allocations. Resizing does
not force a flush or discard pending draw commands. Those commands execute against
the current scene buffer using its resolution scale. Recorded geometry, user
transforms, physical viewports/scissors, and subdivision counts retain their
original values. A change to either scene size or render resolution clears pixels from
already executed draws, including draws submitted by an explicit
`Graphics2D::Flush()`. Resizing cannot reconstruct those pixels.

For example, drawing an 800 × 600 rectangle and then resizing the scene
to 1600 × 1200 leaves an 800 × 600 rectangle in the pending commands. It is drawn
into the resized scene. If it was already flushed, its pixels are lost instead.
Configure sizes before drawing to avoid mixing layout based on different sizes.
OS-driven window resizing is applied during preparation of the next frame, after
the preceding frame has been presented.

Fullscreen covers the selected monitor without changing its OS display mode.
Windows Alt+Enter and macOS Control+Command+F and the fullscreen menu use the
same transition as `Window::SetFullscreen()`. They preserve the configuration
and apply its fullscreen sizing rule. The default is a presentation of the
existing scene, fitted with letterboxing; it does not expand game coordinates.

The cursor's base transform is refreshed without consuming an input frame.
Unchanged configurations reuse the buffers and cached transform state.

## Validation

[Test_SceneMSAA.cpp](../../Test/Test_SceneMSAA.cpp) checks supported sample-count
transitions, native texture sample counts, resource reuse, pending and executed
drawing, empty frames, repeated changes, rejected values, and preservation across
scene configuration changes. Native and scaled presentation are checked at each
supported sample count. ScreenCapture checks compare dimensions and pixels
at one-pixel, odd, and display-resolution sizes for each supported sample count,
and verify partial edge coverage. A backend test rejects a native unsupported
count separately from public enum validation; unsupported valid enum values are
also checked when the device exposes one.

[Test_SceneAllocationFailure.cpp](../../Test/Test_SceneAllocationFailure.cpp)
checks Metal allocation failures during configuration and MSAA changes. It
verifies retained settings, resources, cursor mapping, and executed/pending
pixels, followed by a successful retry. Disabling MSAA reuses the single-sample
target without allocating a texture. The test-only
[allocation helper](../../Test/MetalTextureAllocationFailure.hpp) rejects a
selected texture allocation on the renderer device; this does not simulate
physical GPU memory exhaustion.

[Test_SceneConfig.cpp](../../Test/Test_SceneConfig.cpp) covers the four factories,
six common uses, fullscreen keep/follow transitions, same-dimension fullscreen
exits, startup in fullscreen, minimized reconfiguration, configuration copies,
invalid values, size boundaries, and preservation of executed pixels for
unchanged and filter-only configurations. Windows cases also check immediate and
next-frame scene dimensions and cursor transforms after maximizing and restoring
Fixed + Display, both Window resolution policies, and WindowPixels, including
restoration from minimization.
The native maximize/minimize case uses `[.window-ui]`; calculations with synthetic
fullscreen and minimized window states remain in the default suite.
[Test_ScenePresentation.cpp](../../Test/Test_ScenePresentation.cpp) exercises
the actual final composition paths: Metal into an offscreen target and D3D11
into its swap-chain buffer. It compares Nearest → Linear → Nearest pixels and
checks scene-buffer reuse across filter changes and combined scene-size/resolution
changes with unchanged final buffer dimensions. D3D11 checks MSAA resolve followed
by scaled composition, as well as direct resolve into the swap-chain buffer;
the latter must match the resolved scene pixels, including partial edge coverage.

[Test_SceneHighResolution.cpp](../../Test/Test_SceneHighResolution.cpp) covers
size calculation, DPI-only changes, all resize modes, rounding, invalid sizes,
configuration round trips, immediate cursor mapping, camera defaults, physical
viewport/scissor coordinates, pending and flushed draws, and window resize APIs.
They also check repeated target replacement across a vertex batch boundary,
texture lifetime, and clipping rectangles outside a shrunken target.
Image comparisons check shapes, patterns, bitmap/MSDF text, projective transforms,
and a custom vertex shader against explicit scaling at the same resolution.
Frame metrics compare subdivision and draw counts.

[Test_WindowFullscreen.cpp](../../Test/Test_WindowFullscreen.cpp) checks default
fullscreen size preservation and reconfiguration through mixed API/shortcut
round trips, including another-DPI display when available. Platform-specific
cases are separate from these shared scenarios.
[Test_WindowFullscreen_Windows.cpp](../../Test/Test_WindowFullscreen_Windows.cpp)
exercises the window-message route for Alt+Enter, same-method and mixed API/shortcut
round trips, all size modes with both resolution policies and explicit fullscreen following, policy changes while
fullscreen, disabled shortcuts (including cancellation before the next frame),
and fullscreen bounds and restored window dimensions on each attached monitor.
Fullscreen resize requests additionally check one mixed-DPI monitor pair in both
directions, restored positions, the six size/resolution combinations with explicit fullscreen following, same-size and invalid requests,
and immediate/next-frame dimensions and cursor mapping. A separate Windows case
covers all three Resize APIs with the default KeepSize policy, settings changed
in fullscreen, and switching between KeepSize and FollowWindow without adding
another full combination matrix. See the
[window validation guide](../window/README.md#validation) for pair selection and
the fallback on systems without mixed-DPI monitors.
[Test_WindowFullscreen_macOS.cpp](../../Test/Test_WindowFullscreen_macOS.cpp)
checks API and UI round trips for the six size/resolution combinations with explicit fullscreen following, immediate scene/cursor
updates, policy changes while fullscreen, monitor bounds, restored window
position and dimensions for each source/destination monitor pair, native
minimized state, unchanged native desktop modes and monitor positions, and
fullscreen resize requests. Local Cocoa events and menu dispatch exercise the
fullscreen UI routes without injecting global input. The native green button
and Zoom menu remain maximization operations. Disabled controls,
pending-request cancellation, shortcut repeats, and fixed/sizable/frameless
styles are covered. Capture dimensions and transformed cursor hit tests
are also checked for the six size/resolution combinations with explicit fullscreen following on each monitor. The macOS monitor test
places the window with
`Window::CenterToScreen(index)` before exercising the public fullscreen API.
See [desktop coordinates](../window/README.md) for point-based macOS placement
and client-pixel input. Mixed-DPI, nonzero and
negative monitor origins require the corresponding display layout.
These tests use `[.fullscreen][.window-ui]` and are hidden from the default suite
because they visibly change the real window. Each transition is followed by a
500 ms pause.

Run the focused tests and then the full suite on the respective host:

```sh
./macOS/run-tests.sh '--test-case=~[window-ui]Scene*'
./macOS/run-tests.sh
```

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=~[window-ui]Scene*'
./WindowsDesktop/run-tests.ps1
```

Opt in to the visible window transitions separately on the respective host:

```sh
./macOS/run-tests.sh '--test-case=[window-ui]'
```

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=[window-ui]'
```

This group also includes maximize/minimize and cross-monitor placement checks.
Use `[fullscreen]` to select only fullscreen cases. See the
[selection guide](../development/README.md#window-ui-tests) for focused filters.

[The interactive check](../../Test/Manual/SceneHighResolution.md) covers OS
resizing, monitor changes, fullscreen, minimization, and pointer interaction.
Numerical simulation of DPI changes does not replace these OS checks.
Pending platform verification and integration work belong in [TODO](../../TODO.md).
