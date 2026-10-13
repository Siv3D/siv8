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

Platform-specific centering is declared in
[WindowUtility.hpp](../../Siv3D/src/Siv3D/Window/WindowUtility.hpp) and implemented in
[SivWindow_Windows.cpp](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Window/SivWindow_Windows.cpp)
and [SivWindow_macOS.cpp](../../Siv3D/src/Siv3D-Platform/macOS/Siv3D/Window/SivWindow_macOS.cpp).
These helpers perform native placement only, so Resize can reuse them before its
single Scene update. The Windows file also defines `Platform::Windows::Window::GetHWND()`.

Monitor enumeration and current-monitor lookup are implemented together in
[SivMonitorInfo_Windows.cpp](../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/MonitorInfo/SivMonitorInfo_Windows.cpp)
and [SivMonitorInfo_macOS.cpp](../../Siv3D/src/Siv3D-Platform/macOS/Siv3D/MonitorInfo/SivMonitorInfo_macOS.cpp).
The shared `SivMonitorInfo.cpp` contains monitor information formatting.

Only the final scene size and render resolution determine whether to clear or
replace the scene buffers. Native intermediate sizes do not discard executed
pixels or allocate temporary scene buffers. Pending drawing is retained and no
flush is forced. Separate public calls each apply their own result.

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

A Retina fullscreen window keeps its point dimensions separate from its backing
pixels. `SceneConfig::Window()` preserves the pre-fullscreen scene size and
fits that scene into the fullscreen area. `SceneResolution::Display` increases
render resolution to use the fitted framebuffer pixels. To expand scene
coordinates in fullscreen, select `FullscreenSceneMode::FollowWindow`.
`SceneConfig::WindowPixels()` follows framebuffer pixels in both window states.
Fixed configurations retain their specified scene size. macOS's final display
composition may resample the framebuffer according to the selected desktop mode.

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
the OS state, DPI, border metrics, saved windowed rectangle, fullscreen transition,
and shown flag. The engine thread owns the published `WindowState` and the title.
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

[Test_WindowSceneSync.cpp](../../Test/Test_WindowSceneSync.cpp) checks public
operation boundaries: immediate and next-frame presentation for the six common
scene configurations, maximization/restoration, asynchronous minimization, and
Scene synchronization after a rejected centering request. Invalid resize sizes
leave state unchanged, while same-size requests retain the centering option.
The macOS allocation-error case temporarily rejects texture creation on the
renderer device, checks both centering overloads, and verifies retained resources
and recovery. This is injected allocation failure, not physical GPU exhaustion.
The Windows case checks below-minimum rejection after maximized restoration.
Its opt-in `placement_across_monitors` case checks explicit placement and centering against
attached displays. The `fullscreen_resize_uses_final_geometry` case checks
native scene-buffer identity and executed/pending pixels across a compound
fullscreen-exit resize; it uses `[.fullscreen]` and 500 ms transition pauses.

```sh
./macOS/run-tests.sh '--test-case=WindowSceneSync.*'
```

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=WindowSceneSync.*,WindowStateSync.*,MonitorInfo.*,ScreenCoordinates.*,SceneConfig.*,SceneHighResolution.*,ScenePresentation.*,SceneMSAA.*'
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=WindowFullscreen.*'
./WindowsDesktop/run-tests.ps1
```

The explicit `WindowSceneSync.*` filter includes its opt-in cases. The normal
suite excludes both fullscreen and cross-monitor placement cases. Windows
regression runs should include a mixed-DPI pair in both directions: immediate
Scene/cursor updates after placement, compound resize buffer reuse, below-minimum
rejection after fullscreen/maximized restoration, and API/Alt+Enter transitions.
These are native runtime checks; macOS compilation does not validate them.

`WindowFullscreen.change_monitor_preserves_window_and_config` selects two
attached displays, preferring different DPI scales. It checks both directions
across the six configurations and explicit fullscreen following, repeated/current
and invalid monitor requests, changes to policy while fullscreen, and exits through
the API, shortcut, and three Resize functions. MSAA is retained, including after
changing it while fullscreen. Restoration uses the original window's monitor,
position, and scale. With three displays, one existing round
trip enters fullscreen on B, moves to C, and restores the original window on A.
Immediate and next-frame presentation are checked; macOS also checks that desktop
modes remain unchanged. Windows injects
missing-monitor snapshots through the native command path before checking recovery,
including native style retention. Native client dimensions and DPI are compared
with the published fullscreen state.
The case is hidden and skips when fewer than two displays are attached.
The WindowSceneSync buffer-preservation case also changes fullscreen monitors
when available before resizing, retaining its executed/pending pixel checks.

[Test_MonitorInfo.cpp](../../Test/Test_MonitorInfo.cpp) controls native identity and
enumeration inputs to check empty/missing displays and later enumeration growth,
shrinkage, or reordering. It also supplies snapshots to the production centering
and fullscreen call paths, checking failure before native style, saved rectangle,
or fullscreen-state changes, followed by successful placement. Function overrides
are passed per operation; production has no mutable global override. These tests
do not unplug a physical monitor. The existing hidden fullscreen round trips add
recovery, invalid-index fallback with a reordered snapshot, rejected centering,
and same-size resize checks without adding fullscreen transitions.

[Test_WindowMacOS.cpp](../../Test/Test_WindowMacOS.cpp) checks native minimum-size
limits, point rounding, and style changes in the default suite. A scoped override
of the window's reported backing scale checks 1× → 2× → 1× reapplication without
requiring multiple displays; this is not a physical DPI transition test.

Its opt-in monitor case checks minimum-size limits immediately after placement
and on the next frame on attached displays. Its fullscreen case invokes the
native detachment path used by GLFW on monitor disconnection, then checks window
state, Scene configuration, sizing, cursor conversion, placement, resizing, and a
subsequent shortcut/API fullscreen round trip. This does not unplug a monitor or
exercise OS display re-enumeration. These cases retain `[.screen-coordinates]` and
`[.fullscreen]` respectively, with 500 ms pauses after visible transitions:

```sh
./macOS/run-tests.sh '--test-case=WindowMacOS.*'
```

Use the [interactive program](../../Test/Manual/SceneHighResolution.md) for
physical monitor disconnection/reconnection and manual minimum-size checks.

[Test_WindowStateSync.cpp](../../Test/Test_WindowStateSync.cpp) separates controlled
two-thread handoff checks from real HWND message tests on Windows. The handoff
test pauses at partial and nested updates, checks complete generations, and retains
a public reference across publication. Native tests check deferred OS updates,
nearby explicit policy changes without geometry loss, reentrant command completion,
same-DPI notification reentry, immediate resizing/placement, and next-frame
backbuffer/Scene/cursor consistency. Real mixed-DPI placement is checked only when
the attached monitors provide different scale factors. Every native suite also
starts and shuts down the actual two-thread engine; the runner bounds execution
and requires complete XML reports. Passing these tests supplements the ownership
and lock-scope review; it does not constitute a race-detector result.

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=WindowStateSync.*,SceneConfig.*,SceneHighResolution.*,ScenePresentation.*,SceneMSAA.*'
./WindowsDesktop/run-tests.ps1 -Configuration Debug -TestArguments '--test-case=WindowStateSync.*'
```

[Test_ScreenCoordinates.cpp](../../Test/Test_ScreenCoordinates.cpp) checks Windows
visible-frame placement, repeated position and resize round trips, both centering
overloads, and style changes against native rectangles. These cases run in the
normal suite on attached displays, including mixed DPI and negative origins when
available. A same-DPI message dispatched through the real HWND thread separately
checks the native origin contract and reentry; it does not simulate changing the
OS display scale.

```powershell
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=ScreenCoordinates.Windows.*,WindowStateSync.*'
./WindowsDesktop/run-tests.ps1 -TestArguments '--test-case=ScreenCoordinates.Windows.initial_placement'
```

Run the initial-placement case alone to check `SetPos()` on the initially hidden
HWND before the first `System::Update()`. The report records whether the window
was already visible; the same case also runs after earlier cases in the full suite.

On macOS the same file compares public geometry against native Cocoa/Core Graphics
geometry and checks invalid monitor indices in the default suite. Its placement
and cursor tests are hidden with `[.screen-coordinates]`: they move the real window
and pointer on attached monitors, pausing for 500 ms after transitions. Opt in explicitly:

```sh
./macOS/run-tests.sh '--test-case=ScreenCoordinates.*'
```

The opt-in cases cover decorated/resizable/frameless placement, both centering
overloads, position round trips, all three scene-size modes with both resolution
policies, cursor warping through
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

The Windows fullscreen resize case selects one attached mixed-DPI pair and checks
both directions with `CenterWindow::No`. It records monitor/DPI, window and scene
dimensions, and presentation settings immediately after resizing and after the
next frame. All three scene-size modes with both resolution policies and explicit
fullscreen following use actual-size requests and alternating
`Resize()` / `ResizeVirtual()` requests, avoiding duplicate alias combinations.
It also checks restored positions, native client pixels, same-size fullscreen
requests (including rounded virtual sizes), invalid sizes, settings changed in
fullscreen, and cursor transforms. If no mixed-DPI pair exists, it reports that
limitation and checks only the current monitor; this is not mixed-DPI validation.
The KeepSize case uses the same pair selection and all three Resize APIs, checking
the default configuration and settings changed in fullscreen, including transitions
between KeepSize and FollowWindow. Below-minimum requests retain coverage of both
fullscreen sizing policies without extra fullscreen round trips.
