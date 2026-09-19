# Vendored GLFW

[GLFW sources](../../Siv3D/src/ThirdParty/GLFW) are based on the official
[3.5.1 release](https://github.com/glfw/glfw/releases/tag/3.5.1), under GLFW's
zlib/libpng license. Upstream released 3.5.1 with the same contents as 3.5 to
avoid an incorrectly published 3.5.0 tag; see the
[version history](https://www.glfw.org/changelog).

The upstream `src` C, Objective-C and header files and the two public headers
are stored together. Build scripts, examples and generated Wayland protocol
files are not included. The current macOS engine builds the Cocoa backend;
Windows uses Siv3D's own window/input backend and does not compile GLFW.

## Local changes to preserve

The `[Siv3D]` blocks identify the local integration changes. Compare against the
official release when updating; an ordinary merge without conflicts does not
establish behavioral compatibility.

| File | Integration behavior |
| --- | --- |
| `glfw3.h`, `cocoa_init.m` | JIS yen and underscore key tokens 200/201 map to macOS scancodes `0x5D`/`0x5E`. |
| `input.c` | `glfwGetKeysSiv3D` exposes the existing key-state buffer used by `CKeyboard`. |
| `cocoa_init.m` | Secure restorable-state support; context backend termination calls are disabled to match the Metal-only integration. |
| `cocoa_monitor.m` | Siv3D monitor rectangle, display ID, unit number, work area and current-monitor queries. |
| `cocoa_window.m` | Window positions use outer-frame backing pixels; cursor positions use client backing pixels. Move callbacks carry `(1, 0)` at move start and `(0, 0)` at completion, as consumed by `CWindow::OnMove`. |
| `cocoa_window.m` | File/text drag acceptance, drag-over state, inbound drops and outbound dragging use `GLFW_Siv3D_DragDropBridge.h` and the macOS `CDragDropBridge.mm`. |
| `cocoa_window.m` | Pointer/tablet events forward proximity, pressure and tilt to `CPentablet`. |
| `cocoa_window.m` | NSGL/EGL/OSMesa context creation is disabled; Siv3D creates `GLFW_NO_API` windows and attaches its Metal layer. |
| `internal.h` | Public-header includes use the flattened vendor directory layout. |

`GLFW_Siv3D_DragDropBridge.h` is a Siv3D-owned addition. Apart from the integration
files above, `nsgl_context.m` differs from the release only by removal of trailing
whitespace in a comment.

Keep these behaviors when replacing the upstream files. In particular, restoring
upstream position callbacks would pass coordinates where Siv3D expects movement
flags, and restoring upstream point coordinates would change Retina behavior.
The context restrictions are intentional integration changes, not full GLFW
OpenGL support.

## Keyboard integration

The engine's [CKeyboard](../../Siv3D/src/Siv3D-Platform/macOS_Linux/Siv3D/Keyboard/CKeyboard.cpp)
polls the GLFW key-state buffer once per frame. Backslash and JIS yen share the
existing Siv3D input code `0xDC`; their states are combined before updating that
input once. A compile-time check rejects duplicate input codes in the conversion
table. Aggregate modifier inputs are also updated once per frame.

On Cocoa, GLFW maps the physical F13 key to `GLFW_KEY_PRINT_SCREEN` and keypad
Clear to `GLFW_KEY_NUM_LOCK`. Siv3D exposes each through both corresponding
inputs: F13/PrintScreen and Clear/NumLock. The original GLFW mappings and existing
Siv3D input codes are retained.

Eisu and Kana inputs are excluded: adding token/scancode mappings alone did not
produce pressed states in hardware testing. Synthetic key-buffer tests cannot
establish delivery of these keys from the OS.

Name initialization skips unmapped entries instead of passing key zero to GLFW.
The custom JIS underscore token uses the scancode form of
[glfwGetKeyName](https://www.glfw.org/docs/latest/input#input_key_name);
fixed fallback names are used when a printable name is unavailable.
Outstanding input work and layout decisions are tracked in [TODO.md](../../TODO.md).

## Updating and checking

1. Compare the current vendor tree with its exact upstream release, then port
   the local changes onto the new release. Update common and platform sources
   together because they share internal structures.
2. Reconcile removed and renamed files with the platform projects. In 3.5.1,
   `cocoa_time.c/.h` became `macos_time.c/.h`, and `xkb_unicode.h` was removed.
   QuartzCore is a link dependency and is already linked by the macOS app.
3. Run `./macOS/validate-projects.sh`, the focused
   `./macOS/run-tests.sh '--test-case=GLFW.*,Keyboard.*'`, and the full
   `./macOS/run-tests.sh` outside the sandbox on macOS.
4. Use the [interactive compatibility test](../../Test/Manual/GLFWCompatibility.md)
   for hardware and desktop interaction. Automated tests cannot establish real
   JIS keyboard, mixed-DPI monitor, drag-session or tablet behavior.
5. Check LF line endings and run `git diff --check` and
   `python3 tools/check_docs.py`.

[Test_GLFW.cpp](../../Test/Test_GLFW.cpp) exercises the linked version and timer,
JIS scancode mappings, Cocoa shortcut press/release and the shared key buffer,
move notifications, monitor queries, backing-pixel window coordinates and the
Metal window configuration. It is compiled as Objective-C++ on macOS and excluded
by a platform guard on Windows. Existing Metal tests cover rendering through the
native content view. See the [development guide](README.md) for platform test
workflows.

[Test_Keyboard.cpp](../../Test/Test_Keyboard.cpp) injects cached GLFW key states
into an isolated keyboard instance, restoring the engine's state afterward. It
covers press/hold/release, overlapping backslash/yen sources,
F13/Clear aliases, input enumeration and name initialization without GLFW errors.
The tests do not generate OS input or establish physical keyboard delivery.
