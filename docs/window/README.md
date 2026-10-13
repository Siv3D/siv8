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
| `Window::SetMinimumFrameBufferSize()`, `WindowState::minFrameBufferSize` | Client backing pixels | Client pixels |
| `Scene::Size()`, `Cursor::PosF()`, shape hit testing | Scene/user coordinates | Scene/user coordinates |
| `Scene::RenderResolution()`, captured scene image dimensions | Render-target pixels | Render-target pixels |

The API contracts are in [Window.hpp](../../Siv3D/include/Siv3D/Window.hpp),
[WindowState.hpp](../../Siv3D/include/Siv3D/WindowState.hpp),
[MonitorInfo.hpp](../../Siv3D/include/Siv3D/MonitorInfo.hpp), and
[Cursor.hpp](../../Siv3D/include/Siv3D/Cursor.hpp).
See [scene configuration](../renderer2d/scene-high-resolution.md) for the
independent choice of scene size and render resolution.

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
framebuffer dimensions can change. `SceneSizeMode::Fixed` retains the scene
size, `Window` follows the client point dimensions, and `WindowPixels` follows the
framebuffer pixels. The selected render-resolution and interpolation settings
continue to apply. Monitor `fullscreenResolution` is the framebuffer size in
pixels for fullscreen at the current desktop setting. On macOS it describes
backing pixels, which may differ from the panel's physical pixel count. Use
`displayRect` for desktop placement, and the window's `frameBufferSize` for its
actual framebuffer dimensions.

`Window::SetMinimumFrameBufferSize()` specifies a minimum for manual resizing in
pixels. On macOS, the native limit is rounded up to whole points: 321 × 241 pixels
becomes 161 × 121 points at 2×, and 321 × 241 points at 1×. The stored pixel request
stays unchanged when the window moves between monitors. The point limit is
recomputed when the backing scale changes, and applied when the window becomes
resizable if it was set while using Fixed or Frameless style.

## Applying window changes to Scene

Geometry operations in the public `Window::` API apply the observed window state
to Scene before returning. For example, `SetPos()` across displays updates the
render resolution and cursor transform for the observed destination scale.
Native animations may produce more changes later; frame preparation consumes
those notifications before rendering and input updates. Minimization retains the
last usable scene geometry. See [Window.hpp](../../Siv3D/include/Siv3D/Window.hpp)
for the return-value and failure contracts.

The shared [Window facade](../../Siv3D/src/Siv3D/Window/SivWindow.cpp) owns the Scene
update at the end of each public geometry operation. Native backends publish
`WindowState` without applying Scene. Their intermediate state remains available
for restored-monitor DPI conversion and cross-DPI centering. Resize completes its
native restoration, size request, and optional centering before applying Scene;
a `false` resize result still applies the observed final window state. Shortcut
requests use the public fullscreen path.

Native intermediate sizes do not trigger Scene resource updates. Separate public
calls each apply their own result. See [Scene resource updates](../renderer2d/scene-high-resolution.md#rendering-design)
for buffer reuse and the treatment of pending and executed drawing.

A native operation and its Scene update are not a rollback transaction. In
particular, Windows can restore a fullscreen or maximized window before rejecting
a below-minimum resize. Scene follows that restored state when the call returns
`false`. Resource-update exceptions propagate; already completed native changes
are not undone. Centering catches failure to resolve the current monitor, but
not errors from the subsequent move or Scene update.
Windows error-reporting cleanup calls native window operations directly, so
preparing an error dialog does not retry Scene resource allocation.

## Fullscreen

`Window::SetFullscreen()` covers the selected display without changing its OS
display mode. Resolution, refresh rate, and desktop scaling remain under the
user's control. The full display rectangle is used, including the area normally
occupied by the menu bar and Dock. On macOS, a fullscreen window minimizes
when it loses focus.

Calling `Window::SetFullscreen(true, monitorIndex)` while fullscreen moves the
fullscreen window to that display. The original windowed position and size remain
the restoration target, even after multiple monitor changes. Scene keeps its
configuration: `KeepSize` retains the scene size, while `FollowWindow` follows
the destination geometry. Display-resolution rendering uses the destination
framebuffer. Reapplying the current display does not move the window; an
out-of-range index falls back to that display.

Native monitor changes do not pass through windowed mode. Windows keeps the
fullscreen style and uses the existing DPI-transition scope; macOS switches the
GLFW monitor attachment. Both leave the saved windowed rectangle intact, and the
shared Window facade applies Scene from the final observed geometry.

If macOS disconnects the fullscreen monitor, GLFW detaches the window from it.
The next `System::Update()` reads the native fullscreen state along with the new
geometry. The current `SceneConfig` remains selected; window-following scene sizes
resume following the restored window, including with `KeepSize` selected.

See [Scene configurations](../renderer2d/scene-high-resolution.md#six-common-uses)
for each preset's fullscreen sizing rule. macOS's final display composition may
resample the framebuffer according to the selected desktop mode.

Calling `Window::Resize()`, `ResizeVirtual()`, or `ResizeActual()` with a changed
size exits fullscreen and resizes the restored window. Virtual-size requests use
the restored window's monitor scale. The macOS actual-size
conversion uses the restored window's monitor scale; a request in backing pixels
is rounded to a client size in points. Reapplying the current size leaves the
window fullscreen. The current `SceneConfig` remains selected throughout these operations.

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

## Windows native placement

The public outer frame uses DWM's visible frame bounds. Win32 `GetWindowRect()`
and `SetWindowPos()` include the invisible resize border. Explicit placement
converts between these origins using the current native and DWM rectangles;
it does not reconstruct the offset by adding independently rounded system metrics.
If placement changes DPI, one final move uses the destination frame offset while
keeping the client size established by the DPI notification. A style change applies
the new frame before restoring the visible origin.

Resizing with `CenterWindow::No` uses `SWP_NOMOVE`, so repeated size changes do
not feed the public position through a coordinate conversion. `WM_DPICHANGED`
already supplies a native origin, which is used directly while preserving the
existing virtual-size policy. Fullscreen continues to save and restore native
rectangles. These operations run on the HWND thread within the existing state
update boundary. Native/DWM offset queries occur only during explicit placement
or style correction; public getters and ordinary resize notifications do not add
queries or locks for this conversion.

## Windows state ownership

Windows operations that select the current display from a captured monitor list
resolve the HWND's native display-device name against that list. They do not use
an index obtained by a second enumeration to select from an earlier list. The internal
[WindowsMonitor helpers](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/MonitorInfo/WindowsMonitor.hpp)
keep the public enumeration order, including its active-child and fallback rules.
`GetCurrentMonitorIndex()` retains its zero fallback; snapshot resolution instead
reports an absent result when the current display cannot be matched.

Fullscreen entry resolves a valid requested index, or the current display for an
invalid index, before changing the native style or saved windowed rectangle.
An unresolved target leaves fullscreen state unchanged. This protects resolution
failure, without attempting to make OS topology changes atomic or roll back every
possible Win32 failure. `GetCurrentMonitor()` throws when its snapshot cannot
identify the current display. Both centering overloads return false in fullscreen;
on Windows they first use the existing `show()` synchronization boundary. Normal
cross-DPI centering retains the intermediate move and synchronized bounds read.

The HWND thread owns native geometry and window policy in
[CWindow](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Window/CWindow.hpp):
the OS state, DPI, border metrics, style, minimum size, saved windowed rectangle,
fullscreen transition, and shown flag. The engine thread owns the published
`WindowState` and the title.
The HWND and loaded User32 entry points are initialized before the startup promise
releases the engine thread. Destruction keeps the window owner alive until the
HWND thread has destroyed the window and acknowledged the existing shutdown flag.
Window messages find their owner through HWND user data from `WM_NCCREATE` through
`WM_NCDESTROY`, independently of the public snapshot and other component lifetimes.

[WindowStateSync](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Window/WindowStateSync.hpp)
copies a completed OS state through a mutex-protected slot into a fixed engine-owned
object. A retained `Window::GetState()` reference keeps pointing at that object.
Getters only read it: no lock, wait, allocation, or atomic field loads are added.
Ordinary frame preparation takes one short mutex lock and copies one `WindowState`,
before the renderer prepares the backbuffer and Scene and the cursor is updated.
The HWND thread cannot change that snapshot during those operations.

Explicit native operations run on the HWND thread via a synchronous internal
command. Completion includes publication to the slot; the engine consumes it
before returning from the API or updating Scene. This also supplies the intermediate
geometry read by cross-DPI `CenterToScreen()`. Commands edit the current OS state;
they never write an engine snapshot back to it. Fullscreen exit and maximized
restore publish their intermediate Window state for destination-DPI calculations;
Scene is applied by the shared facade after the whole operation, including a
below-minimum rejection. GPU work, Scene updates, and cursor transforms stay on
the engine thread. Alt+Enter retains its atomic request/cancellation state and
uses the public fullscreen operation.

State-notification scopes nest without locking. Only the outermost completion
copies to the shared slot, so `WM_SIZE` publishes bounds, flags and both dimensions
together, and `WM_DPICHANGED` includes its nested resize notifications.
`WM_WINDOWPOSCHANGED` also encloses the
[WM_MOVE and WM_SIZE notifications sent by DefWindowProc](https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-windowposchanged).
The modal move/size loop itself is outside these scopes, allowing each completed
notification to reach the next frame during continuous dragging. WindowProc reads
OS state for its own decisions, including the previous maximized flag.

[SendMessage can reenter while a thread waits](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendmessage).
If a command arrives inside a state scope, it is linked into a window-owned queue
using its sender-owned stack storage and executed after that scope commits.
The sender waits for completion; the HWND thread never waits for the engine.
Command completion has a separate mutex/condition variable to keep the stack
storage alive through deferred execution and carry exceptions back to the sender.
Neither synchronization mutex spans a Win32 call, callback, Scene update, or GPU
operation. These additional message and completion costs occur at explicit API
boundaries, not in getters or ordinary frame synchronization. This ownership model
does not make arbitrary concurrent public Window API calls supported.

## Validation

Use the [development guide](../development/README.md#window-ui-tests) for test
selection, transition pauses, and report checks. Fullscreen, maximize/minimize,
cross-monitor placement, and desktop pointer cases use `[.window-ui]`; the default
suite still checks calculations, native state, and local resizing.

| Tests | Responsibility |
| --- | --- |
| [WindowSceneSync](../../Test/Test_WindowSceneSync.cpp) | Immediate and next-frame Scene/cursor state, final geometry of compound operations, below-minimum rejection, buffer reuse and executed/pending drawing; Metal allocation failure and retry |
| [WindowStateSync](../../Test/Test_WindowStateSync.cpp) | Windows state generations and stable references, native notifications, reentrant commands and exceptions, API synchronization, startup/shutdown |
| [MonitorInfo](../../Test/Test_MonitorInfo.cpp) | Enumeration rules and identity against one snapshot; missing, reordered, or resized lists; failure before mutation and recovery |
| [ScreenCoordinates](../../Test/Test_ScreenCoordinates.cpp) | Public/native coordinates, initial placement, frame styles, centering, DPI transitions, and pointer conversion |
| [WindowFullscreen](../../Test/Test_WindowFullscreen.cpp), [Windows](../../Test/Test_WindowFullscreen_Windows.cpp), [macOS](../../Test/Test_WindowFullscreen_macOS.cpp) | API/shortcut routes and cancellation, policy changes, fullscreen monitor changes, original window restoration, and Resize semantics; macOS desktop-mode preservation |
| [WindowMacOS](../../Test/Test_WindowMacOS.cpp) | Pixel minimum sizes, point rounding and style changes, scale reapplication, and native fullscreen detachment |

Run Windows `ScreenCoordinates.Windows.initial_placement` alone to check the
initially hidden HWND before the first frame; see the command in the development
guide. Its execution later in the full suite checks a different starting state.

Monitor tests use attached displays. Mixed-DPI and negative-origin coverage
requires the corresponding layout. Fullscreen transfer needs two displays and
uses a three-display route when available; resize restoration selects one
mixed-DPI pair in both directions, or reports the single-monitor fallback.
macOS mode-preservation checks compare display modes and monitor origins, not
work areas, because the Dock may move.

[WindowSceneTestHelper.hpp](../../Test/WindowSceneTestHelper.hpp) captures state
without side effects. Each suite chooses its restoration order, size unit, and
guard lifetimes; sharing helpers does not make these sequences interchangeable.
`PauseForWindowTransition()` waits 500 ms without advancing a frame;
`UpdateWithPause()` performs one asserted `System::Update()` before that wait.
Keep those distinctions and conditional waits when changing restoration code.
The [fullscreen helper](../../Test/WindowFullscreenTestHelper.hpp) adds its own
guard and presentation checks.

See [Scene validation](../renderer2d/scene-high-resolution.md#validation) for
rendering tests. The [manual program](../../Test/Manual/SceneHighResolution.md)
covers continuous dragging, physical input, OS settings, and monitor unplugging.
Injected monitor lists, reported backing scales, allocation failures, and native
detachment exercise specific paths without reproducing those physical conditions.
Record verification according to the [evidence guidance](../development/README.md#validation-evidence);
remaining conditions belong in [TODO.md](../../TODO.md).
