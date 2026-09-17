# GLFW / Cocoa compatibility

This macOS test checks Siv3D's GLFW customizations using real desktop input.
Run it after changing the vendored GLFW version. Automated coverage and the
local-change inventory are described in [GLFW maintenance](../../docs/development/glfw.md).

## Setup and execution

Build a separate macOS Siv3D sample application against this checkout's engine
and use the complete code below as its `Main.cpp`. Keep the repository's
`macOS/Main.cpp` and its `--test-only` block intact. Launch the sample normally.
For full coverage, use a JIS keyboard, a tablet and two monitors with different
backing scales. Record unavailable hardware as untested.

1. Move and resize the window, minimize/restore it, and move it between monitors.
   Observe the bounds, backing size, scale and current-monitor index.
2. Press `P` to move the window by 20 backing pixels. Press `C` to move the cursor
   to the scene center. Repeat on each monitor. Press `F` to enter/leave fullscreen.
3. Press the JIS yen and underscore keys separately. Also try Ctrl+Tab,
   Ctrl+Esc and Cmd+Period. Release each combination and repeat it.
4. Drag files and selected text from other applications into the window, then
   outside it without dropping. Drop multiple files and non-ASCII text. Use `T`
   to disable/enable text acceptance and repeat. Drag the upper-right box to a
   text editor. After dropping a file into the sample, drag the lower-right box
   to a temporary Finder folder to test outbound file dragging.
5. Use the pen and eraser near the tablet, press lightly/firmly, tilt it, then
   switch back to the mouse. Close the window after the checks.

## Expected results

- Rendering continues throughout movement, resizing and fullscreen transitions.
  Bounds and cursor positioning remain consistent in backing pixels on each
  display. `C` places the crosshair at the scene center without a Retina offset.
- The two JIS indicators respond independently. Shortcut indicators turn on while
  pressed and turn off after release; no key remains stuck.
- Accepted drags show a hover position, cleared when leaving the window. Each
  drop is reported once at the expected scene position. Disabled text drops do
  not appear. Outbound text and files reach the destination and another drag
  can be started afterward. Use disposable destination files for this check.
- Pen proximity and eraser status update, pressure changes smoothly, and pen
  movement updates the cursor. A plain mouse does not generate pen pressure.
- Unsupported tablet channels may remain zero. Compare supported channels with
  the device's behavior before the GLFW update.

```cpp
# include <Siv3D.hpp>

void Main()
{
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);
	Window::SetTitle(U"GLFW compatibility");
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetResizeMode(ResizeMode::Keep);
	DragDrop::AcceptText(true);
	bool acceptText = true;
	String lastDrop = U"No drop yet";
	FilePath lastFile;
	Optional<Point> dropPosition;
	const Rect textSource{ 530, 20, 240, 60 };
	const Rect fileSource{ 530, 100, 240, 60 };
	const Font font{ 18 };

	while (System::Update())
	{
		if (KeyP.down())
		{
			Window::SetPos(Window::GetState().bounds.pos + Point{ 20, 20 });
		}
		if (KeyC.down())
		{
			Cursor::SetPos(Scene::Center());
		}
		if (KeyF.down())
		{
			Window::SetFullscreen(not Window::GetState().fullscreen);
		}
		if (KeyT.down())
		{
			acceptText = not acceptText;
			DragDrop::AcceptText(acceptText);
		}
		if (textSource.leftClicked())
		{
			DragDrop::BeginDragText(U"Siv3D drag: 日本語 🐥");
		}
		if (fileSource.leftClicked() && not lastFile.isEmpty())
		{
			DragDrop::BeginDragFile(lastFile);
		}
		for (const auto& item : DragDrop::ExtractDroppedFilePaths())
		{
			lastFile = item.path;
			lastDrop = U"File: " + item.path;
			dropPosition = item.pos;
		}
		for (const auto& item : DragDrop::ExtractDroppedTexts())
		{
			lastDrop = U"Text: " + item.text;
			dropPosition = item.pos;
		}

		const auto& window = Window::GetState();
		const auto& pen = Pentablet::GetState();
		ClearPrint();
		Print << U"P: move, C: cursor center, F: fullscreen, T: accept text";
		Print << U"Bounds: " << window.bounds;
		Print << U"Backing: " << window.frameBufferSize << U", scale: " << window.scaling;
		Print << U"Monitor: " << System::GetCurrentMonitorIndex();
		Print << U"Cursor: " << Cursor::Pos() << U", raw: " << Cursor::PosRaw();
		Print << U"JIS yen: " << KeyYen_JIS.pressed() << U", underscore: " << KeyUnderscore_JIS.pressed();
		Print << U"Tab: " << KeyTab.pressed() << U", Esc: " << KeyEscape.pressed() << U", period: " << KeyPeriod.pressed();
		Print << U"Accept text: " << acceptText;
		Print << U"Pen nearby: " << pen.inProximity << U", eraser: " << pen.isEraser;
		Print << U"Pressure: " << pen.normalPressure << U", tilt: " << pen.altitude << U", " << pen.azimuth;
		Print << lastDrop;
		if (const auto hover = DragDrop::DragOver())
		{
			Print << U"Drag hover: " << hover->cursorPos;
			Circle{ hover->cursorPos, 15 }.drawFrame(2, Palette::Orange);
		}
		if (dropPosition)
		{
			Circle{ *dropPosition, 10 }.drawFrame(2, Palette::Green);
		}
		textSource.draw(Palette::Darkblue);
		fileSource.draw(Palette::Darkgreen);
		font(U"Drag text out").draw(540, 35);
		font(U"Drag last file out").draw(540, 115);
		Circle{ Cursor::Pos(), 5 }.draw(Palette::Red);
	}
}
```
