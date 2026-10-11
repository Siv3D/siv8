# Window and desktop coordinates

Use `Window::CenterToScreen(index)` to center the outer window frame in a
monitor's work area. Use `Window::SetPos()` for explicit placement; coordinates
from `MonitorInfo::workArea` can be passed directly without a DPI conversion.

Desktop coordinates have their origin at the primary monitor's top-left corner,
with X increasing rightward and Y downward. Monitors to the left or above it can
have negative coordinates. On macOS, this is one continuous coordinate space in
OS points, including when monitors have different backing scales. On Windows,
the existing desktop pixel coordinates are retained.

| Values | macOS units | Windows units |
| --- | --- | --- |
| `Window::SetPos()` / `GetPos()`, `WindowState::bounds` | Desktop points, including the outer-frame size | Desktop pixels |
| `MonitorInfo::displayRect` / `workArea` | Desktop points | Desktop pixels |
| `WindowState::frameThickness` / `titleBarHeight` | Points | Pixels |
| `Cursor::ScreenPos()` / `ScreenPreviousPos()` / `ScreenDelta()` | Desktop points | Desktop pixels |
| `WindowState::virtualSize` | Client points | Client size before desktop scaling |
| `WindowState::frameBufferSize`, `Cursor::PosRaw()` | Client backing pixels | Client pixels |
| `Scene::Size()`, `Cursor::PosF()`, shape hit testing | Scene/user coordinates | Scene/user coordinates |
| `Scene::RenderSize()`, captured scene image dimensions | Render-target pixels | Render-target pixels |

The API contracts are in [Window.hpp](../../Siv3D/include/Siv3D/Window.hpp),
[WindowState.hpp](../../Siv3D/include/Siv3D/WindowState.hpp),
[MonitorInfo.hpp](../../Siv3D/include/Siv3D/MonitorInfo.hpp), and
[Cursor.hpp](../../Siv3D/include/Siv3D/Cursor.hpp).
See [scene high resolution](../renderer2d/scene-high-resolution.md) for the
independent choice of logical size and rendering resolution.

## Using desktop and client coordinates

For example, place the outer frame 40 desktop units from the work area's top-left:

```cpp
const auto monitor = System::GetCurrentMonitor();
Window::SetPos(monitor.workArea.pos + Point{ 40, 40 });
```

Do not calculate client pixels by subtracting the window position from
`Cursor::ScreenPos()`. Use `Cursor::PosRaw()` for framebuffer pixels, or
`Cursor::PosF()` for drawing and hit testing with the current cursor transforms.
This also handles a cursor outside the window on a monitor with a different DPI.

Moving a window between macOS monitors keeps its point dimensions while its
framebuffer dimensions can change. `ResizeMode::Keep` retains the logical scene
size, `Virtual` follows the client point dimensions, and `Actual` follows the
framebuffer pixels. The selected high-resolution and interpolation settings
continue to apply. Monitor `fullscreenResolution` is the framebuffer size in
pixels for fullscreen at the current desktop setting. On macOS it describes
backing pixels, which may differ from the panel's physical pixel count. Use
`displayRect` for desktop placement, and the window's `frameBufferSize` for its
actual framebuffer dimensions.

## Fullscreen

`Window::SetFullscreen()` covers the selected display without changing its OS
display mode. Resolution, refresh rate, and desktop scaling remain under the
user's control. The full display rectangle is used, including the area normally
occupied by the menu bar and Dock. On macOS, a fullscreen window minimizes
when it loses focus.

A Retina fullscreen window keeps its point dimensions separate from its backing
pixels. `Virtual` follows the point dimensions; high resolution rendering uses
the available framebuffer pixels. `Actual` follows those pixels directly.
Applications with a fixed game canvas should select `Keep`. macOS's final display
scaling can resample the framebuffer, so nearest scene sampling alone does not
guarantee integer scaling on the physical panel.

Calling `Window::Resize()`, `ResizeVirtual()`, or `ResizeActual()` with a changed
size exits fullscreen and resizes the restored window. The macOS actual-size
conversion uses the restored window's monitor scale; a request in backing pixels
is rounded to a client size in points. Reapplying the current size leaves the
window fullscreen. Scene resize mode, high resolution, and filtering remain
selected throughout these operations.

Windows Alt+Enter and macOS Control+Command+F request the same transition as
`Window::SetFullscreen()`, processed by the next `System::Update()`. On macOS,
the fullscreen menu uses this path too. The shortcut and menu work for fixed-size
and frameless windows as well. They use the engine's desktop-covering fullscreen,
without moving into a native fullscreen Space. The title bar's green button,
the Window menu's Zoom command, and `Window::Maximize()` remain maximization
operations. The green button follows the native window style and is disabled
for fixed-size windows; fullscreen enablement does not control it.

`Window::SetToggleFullscreenEnabled(false)` disables these controls and cancels
any pending request. Explicit `Window::SetFullscreen()` calls still work.
Re-enabling does not replay an old request; reapplying `true` preserves a pending
one. macOS ignores shortcut key repeats and combines multiple requests before a
frame into one toggle. The menu title follows the current window/fullscreen state.

Previous macOS fullscreen behavior selected a large enumerated display mode.
Fullscreen point dimensions, backing dimensions, and `fullscreenResolution` can
therefore change when adopting desktop-mode preservation. The scene size rules
remain the same; the desktop setting now supplies their input dimensions.

## Migrating stored macOS positions

Older macOS desktop coordinates multiplied global positions by the containing
monitor's backing scale. Different scales could produce overlapping monitor
rectangles and ambiguous window positions. The point-based contract changes
the numeric values of the screen APIs and window frame dimensions listed above.
Windows coordinates and ordinary scene drawing/input do not change.

Version or reset persisted macOS window positions when adopting this contract.
There is no universal division by two: converting a legacy position requires
knowing which monitor and scale produced it. A saved monitor-relative offset
also needs conversion before adding it to the monitor's point-based origin.

## Validation

[Test_ScreenCoordinates.cpp](../../Test/Test_ScreenCoordinates.cpp) compares the
public geometry against native Cocoa/Core Graphics geometry and checks invalid
monitor indices in the default suite. Its placement and cursor tests are hidden
with `[.screen-coordinates]`: they move the real window and pointer on attached
monitors, pausing for 500 ms after transitions. Opt in explicitly:

```sh
./macOS/run-tests.sh '--test-case=ScreenCoordinates.*'
```

The opt-in cases cover decorated/resizable/frameless placement, both centering
overloads, position round trips, all six scene policies, cursor warping through
local and camera transforms, hit testing, and raw cursor pixels when the pointer
is on another monitor. Actual mixed-DPI and negative-origin coverage depends on
the connected display layout.
[Fullscreen tests](../../Test/Test_WindowFullscreen.cpp) separately check every
source/destination monitor pair using public placement APIs. Native display
mode IDs, point/pixel dimensions, refresh rates, and all monitor origins must
match the snapshot taken before entering fullscreen, including after
minimization/restoration and fullscreen resize calls. Work areas are not required
to remain identical because the Dock may move. Their explicit run
command is in the [scene guide](../renderer2d/scene-high-resolution.md#validation).
Use the [interactive program](../../Test/Manual/SceneHighResolution.md) to check
continuous dragging, drawing, and physical pointer interaction. Automated native
coordinate checks do not establish those visual results.
