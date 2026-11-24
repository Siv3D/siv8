# Screenshot destination and permission checks

This program checks the user-visible default screenshot directory and native
file-access permissions. Automated destination, validation, and PNG round-trip
tests live in [Test_ScreenCapture.cpp](../Test_ScreenCapture.cpp). Follow the
[development guide](../../docs/development/README.md) to run them on each host.
Those automated tests save under `Test/output/`; they do not establish first-access
permission behavior in the user's Pictures directory.

## Execution

1. Copy the complete program below into a separate Siv3D sample application.
   For a macOS first-access check, use an app identity that has not previously
   received file-access permission and launch the built app from Finder. Record
   the OS version, sandbox setting, and applicable entitlements; a launch from
   an already authorized development tool does not establish first-access behavior.
2. The application automatically saves a timestamp-named image and a
   `siv3d-screen-capture-check-<run ID>.png` image in the default directory, plus
   a third image under `output/` in its working directory. That working directory
   must be writable. Wait for SUCCESS or FAIL without pressing any keys. If a
   permission dialog appears, record its text before responding.
3. Inspect `screen-capture-check-<run ID>.json` in the working directory. It records
   the output paths and whether all three saved images match the captured frame.
   SUCCESS also requires this report to be written. The report cannot establish
   whether a permission dialog appeared; record that observation separately.
   After this initial check, N repeats the named save, A or F12 saves another
   automatically named image, and P repeats the explicit `output/` save.
4. On Windows, optionally replace `networkPath` with a path on an existing,
   writable test share you control, then press U. Check that the image appears
   at that exact UNC path. Leave the string empty to skip this check.
5. Inspect the engine log if saving fails. On macOS the program redirects logging
   from `Main()` onward to `screen-capture-check-<run ID>.log` in the working
   directory. Remove the generated images and reports after inspection; these
   retained manual outputs are not managed by the test runner.

## Expected results

- The default directory is the OS-provided Pictures directory plus `Screenshot/`
  on both macOS and Windows. It is created when saving if necessary. Pictures
  may be redirected by the OS; do not assume a hard-coded home directory.
- The initial three saved images match the captured frame and the application
  displays SUCCESS. N, A, and F12 produce images of this scene in the default
  directory. N overwrites the same named file when pressed again. P and U use
  their specified paths.
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
# include <cstdio>

void Main()
{
	Window::Resize(800, 600);
	Scene::SetBackground(ColorF{ 0.15, 0.20, 0.30 });
	const Font font{ FontMethod::MSDF, 28 };
	const FilePath networkPath = U""; // Example: U"//server/share/siv3d-screen-capture-check.png"
	const String runID = DateTime::Now().format(U"yyyyMMdd-HHmmss-SSS");
	const String stem = (U"siv3d-screen-capture-check-" + runID);
	const FilePath directory = ScreenCapture::GetScreenshotDirectory();
	const FilePath namedPath = (directory + stem + U".png");
	const FilePath explicitPath = (U"output/" + stem + U".png");
	const FilePath reportPath = (U"screen-capture-check-" + runID + U".json");

# if SIV3D_PLATFORM(MACOS)
	const std::string logPath = Unicode::ToUTF8(U"screen-capture-check-" + runID + U".log");
	std::freopen(logPath.c_str(), "w", stderr);
# endif

	String automaticNameBegin, automaticNameEnd;
	String status = U"Waiting for automatic save...";
	int32 frame = 0;
	bool success = false;

	while (System::Update())
	{
		++frame;
		if (frame == 4)
		{
			const Image& captured = ScreenCapture::GetFrame();
			const bool capturedOK = (ScreenCapture::HasNewFrame() && not captured.isEmpty());
			const bool namedOK = (capturedOK && (Image{ namedPath } == captured));
			const bool explicitOK = (capturedOK && (Image{ explicitPath } == captured));
			Array<FilePath> automaticPaths;
			if (not directory.isEmpty())
			{
				// Inspect Pictures only after the save attempt, so the first operation is a write.
				for (const auto& path : FileSystem::DirectoryContents(directory, Recursive::No))
				{
					const String name = FileSystem::FileName(path);
					if ((name.size() == 23) && name.ends_with(U".png")
						&& (automaticNameBegin <= name) && (name <= automaticNameEnd))
					{
						automaticPaths << path;
					}
				}
			}
			const bool automaticOK = (capturedOK && (automaticPaths.size() == 1)
				&& (Image{ automaticPaths.front() } == captured));
			JSON report;
			report["run_id"] = runID;
			report["screenshot_directory"] = directory;
			report["working_directory"] = FileSystem::CurrentDirectory();
			report["named_path"] = namedPath;
			report["explicit_path"] = FileSystem::FullPath(explicitPath);
			report["automatic_paths"] = automaticPaths;
			report["capture_ok"] = capturedOK;
			report["named_matches_capture"] = namedOK;
			report["explicit_matches_capture"] = explicitOK;
			report["automatic_matches_capture"] = automaticOK;
			report["width"] = captured.width();
			report["height"] = captured.height();
			report["all_images_match"] = (namedOK && explicitOK && automaticOK);
			const bool reportSaved = report.save(reportPath);
			success = (namedOK && explicitOK && automaticOK && reportSaved);
			status = success ? U"SUCCESS" : U"FAIL - inspect report and log";
		}

		ClearPrint();
		Print << U"Screenshot directory: " << directory;
		Print << U"Working directory: " << FileSystem::CurrentDirectory();
		Print << U"N: named   A/F12: automatic   P: explicit path   U: UNC";
		Circle{ 400, 320, 100 }.draw(ColorF{ 0.2, 0.7, 0.9 });
		font(status).drawAt(26, Vec2{ 400, 480 }, success ? Palette::Lightgreen : Palette::White);
		font(U"Report any permission dialog, then close.").drawAt(20, Vec2{ 400, 540 });

		if (frame == 3)
		{
			automaticNameBegin = (DateTime::Now().format(U"yyyyMMdd-HHmmss-SSS") + U".png");
			ScreenCapture::SaveCurrentFrame();
			automaticNameEnd = (DateTime::Now().format(U"yyyyMMdd-HHmmss-SSS") + U".png");
			ScreenCapture::SaveCurrentFrame(stem + U".png");
			ScreenCapture::SaveCurrentFrameTo(explicitPath);
		}

		if (KeyN.down()) { ScreenCapture::SaveCurrentFrame(stem + U".png"); }
		if (KeyA.down()) { ScreenCapture::SaveCurrentFrame(); }
		if (KeyP.down()) { ScreenCapture::SaveCurrentFrameTo(explicitPath); }
		if (KeyU.down() && networkPath) { ScreenCapture::SaveCurrentFrameTo(networkPath); }
	}
}
```
