# Scene background timing

This program makes the one-frame delay of `Scene::SetBackground()` visible.
It complements the pixel comparisons in
[Test_SceneBackground.cpp](../Test_SceneBackground.cpp).
See the [background guide](../../docs/renderer2d/scene-high-resolution.md#background-color).

## Execution

Build the complete program below in a separate Siv3D application on each target
platform. Keep the repository's platform Main.cpp files intact.

- Press **1**, **2**, or **3** to select no intermediate flush, an empty flush,
  or a flush containing the white square. **F** moves the flush before or after
  the background setting.
- **R** alternates scene sizes, **H** changes render resolution, and **M** switches
  between no MSAA and 4x MSAA. Changes take place after the background setting.
- Each frame is held for 250 ms so the background and the requested color can
  be compared. The program creates no files.

## Expected results

- The first frame has a blue background. The outlined left square always matches
  the background; the right square shows the color selected for the next frame.
- Neither moving the flush nor changing its contents changes this relationship.
  Size, resolution, and MSAA changes also keep the background at the left square's
  color for that frame.
- In mode 3 the white square remains visible across the background setting and
  later drawing. A size, resolution, or MSAA change can clear it if it was already
  flushed; this is the existing scene reconfiguration behavior.

## Complete code

```cpp
# include <Siv3D.hpp>

void Main()
{
	Scene::Configure(SceneConfig::Fixed({ 640, 480 }));
	Scene::SetBackground(Palette::Blue);
	const Array<Color> colors{ Palette::Red, Palette::Green, Palette::Blue, Palette::Yellow };
	Color current = Palette::Blue;
	size_t frame = 0;
	int32 mode = 0;
	bool flushBefore = true;
	bool large = false;
	bool display = false;
	bool msaa = true;
	while (System::Update())
	{
		if (Key1.down()) { mode = 0; }
		if (Key2.down()) { mode = 1; }
		if (Key3.down()) { mode = 2; }
		if (KeyF.down()) { flushBefore = not flushBefore; }
		const auto flush = [&]
		{
			if (mode == 2) { Rect{ 20, 20, 40, 40 }.draw(Palette::White); }
			if (mode != 0) { Graphics2D::Flush(); }
		};
		if (flushBefore) { flush(); }
		const Color next = colors[frame++ % colors.size()];
		Scene::SetBackground(next);
		if (not flushBefore) { flush(); }
		if (KeyR.down() || KeyH.down())
		{
			if (KeyR.down()) { large = not large; }
			if (KeyH.down()) { display = not display; }
			Scene::Configure(SceneConfig::Fixed(large ? Size{ 800, 600 } : Size{ 640, 480 },
				display ? SceneResolution::Display : SceneResolution::SceneSize));
		}
		if (KeyM.down())
		{
			msaa = not msaa;
			Scene::SetMSAASampleCount(msaa ? MSAASampleCount::X4MSAA : MSAASampleCount::NoMSAA);
		}
		Rect{ 100, 180, 120, 120 }.draw(current).drawFrame(2, Palette::White);
		Rect{ 340, 180, 120, 120 }.draw(next).drawFrame(2, Palette::White);
		ClearPrint();
		Print << U"1-3: flush mode " << (mode + 1) << U"  F: " << (flushBefore ? U"before" : U"after");
		Print << U"R: size  H: resolution  M: MSAA " << (msaa ? 4 : 1);
		Print << U"Left: this frame    Right: next frame";
		current = next;
		System::Sleep(250);
	}
}
```
