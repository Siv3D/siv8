# Screenshot destination and permission checks

This program checks the user-visible default screenshot directory and native
file-access permissions. Automated destination, validation, and PNG round-trip
tests live in [Test_ScreenCapture.cpp](../Test_ScreenCapture.cpp). Follow the
[development guide](../../docs/development/README.md) to run them on each host.

## Execution

1. Copy the complete program below into a separate Siv3D sample application.
   For a macOS first-access check, use an app identity that has not previously
   received file-access permission and launch the built app from Finder. Record
   the OS version, sandbox setting, and applicable entitlements; a launch from
   an already authorized development tool does not establish first-access behavior.
2. Press N to save `siv3d-screen-capture-check.png`, then A or F12 to save an
   automatically named image. Open the displayed directory in Finder or Explorer.
3. Press P to use the explicit `output/siv3d-screen-capture-check.png` path.
   This output belongs to the application's working directory, independent of
   the displayed screenshot directory. If that directory is not writable, use
   an absolute path in an owned writable directory instead.
4. On Windows, optionally replace `networkPath` with a path on an existing,
   writable test share you control, then press U. Check that the image appears
   at that exact UNC path. Leave the string empty to skip this check.
5. Inspect the engine log if saving fails. Remove the generated images after
   inspection; these retained manual outputs are not managed by the test runner.

## Expected results

- The default directory is the OS-provided Pictures directory plus `Screenshot/`
  on both macOS and Windows. It is created when saving if necessary. Pictures
  may be redirected by the OS; do not assume a hard-coded home directory.
- N, A, and F12 produce images of this scene in that directory. N overwrites
  the same named file when pressed again. P and U use their specified paths.
- For a normal non-sandboxed macOS app and a local writable Pictures directory,
  saving a PNG is expected not to ask for Desktop, Documents, Downloads, Photos
  library, or screen-recording access. Record any actual dialog rather than
  inferring permissions from a successful run in a previously authorized app.
  App Sandbox access must be evaluated separately with its configured entitlements.
- Failure is logged for the requested destination, without saving elsewhere or
  reporting a successful save. `HasNewFrame()` indicates image capture, not file
  save success.

## Complete program

```cpp
# include <Siv3D.hpp>

void Main()
{
	Window::Resize(800, 600);
	Scene::SetBackground(ColorF{ 0.15, 0.20, 0.30 });
	const Font font{ FontMethod::MSDF, 28 };
	const FilePath networkPath = U""; // Example: U"//server/share/siv3d-screen-capture-check.png"

	while (System::Update())
	{
		ClearPrint();
		Print << U"Screenshot directory: " << ScreenCapture::GetScreenshotDirectory();
		Print << U"Working directory: " << FileSystem::CurrentDirectory();
		Print << U"N: named   A/F12: automatic   P: explicit path   U: UNC";
		Circle{ 400, 320, 100 }.draw(ColorF{ 0.2, 0.7, 0.9 });
		font(U"Screenshot destination check").drawAt(26, Vec2{ 400, 500 });

		if (KeyN.down()) { ScreenCapture::SaveCurrentFrame(U"siv3d-screen-capture-check.png"); }
		if (KeyA.down()) { ScreenCapture::SaveCurrentFrame(); }
		if (KeyP.down()) { ScreenCapture::SaveCurrentFrameTo(U"output/siv3d-screen-capture-check.png"); }
		if (KeyU.down() && networkPath) { ScreenCapture::SaveCurrentFrameTo(networkPath); }
	}
}
```
