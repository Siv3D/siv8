# Dashed curve and path drawing

This is the canonical manual program for `draw(LineStyle, ...)` and
`LineString::drawClosed(LineStyle, ...)`. It exercises LineString, Bezier2, Bezier3, and
Spline2D using the public APIs. See the
[drawing guide](../../docs/geometry2d/dashed-drawing.md) for distance units,
closed seams, sampling and rendering limits.

## Run

Use the complete program below as the entry point in a separate Siv3D sample
project. Build and run using the normal workflow for the current host. Do not
replace the repository's platform Main.cpp with this manual test.

| Key | Action |
|---|---|
| Space | Pause or resume phase animation |
| C | Cycle Dotted / Dashed / LongDash / DashDot / RoundDot |
| F | Toggle Flat / Square path endpoints |
| A | Toggle alpha 0.5 / 1 |
| Left / Right | Move phase by 0.01; pause first for precise inspection |

## Expected results

Gentle curves should have regular spacing along their centerlines, with no
pattern restart at tessellation vertices. The closed polyline retains its pitch
rather than adjusting it to the perimeter, so the run spanning the seam can have
a different length. Square extends the open path by half the width at each end;
Flat clips at the endpoints. Individual dash cuts remain flat. RoundDot uses
round points; its open Square variant fits their pitch as Line does. Closed
RoundDot keeps whole dots whose centers lie within one cycle, so the last-to-first
spacing can differ from the regular pitch.

At the angular path's corner, a dash ending just beyond the vertex can have a
distorted outline and darker overlap. Changing the path cap does not change the miter join.
Opaque color hides extra blending but leaves the outline distortion. These are
accepted rendering limits, not guarantees of artifact-free corners. Wider lines,
tighter curves can also make separate dashes or dots overlap.

The [automated tests and benchmark](../Test_DashedPath.cpp) check interval
placement, boundaries, caps, transforms, curve APIs and submission costs. Run
ordinary checks separately from the opt-in benchmark:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=~[benchmark]DashedPath.*'
```

For Windows and the isolated Release benchmark, use the
[drawing guide's instructions](../../docs/geometry2d/dashed-drawing.md#implementation-and-measurement).

## Complete program

```cpp
# include <Siv3D.hpp>

void Main()
{
	Window::Resize(960, 640);
	Scene::SetBackground(ColorF{ 0.95 });
	const Font font{ 18 };
	const LineString corner{ { 40, 100 }, { 260, 100 }, { 260, 40 } };
	const LineString ring{ { 540, 60 }, { 820, 60 }, { 820, 180 }, { 540, 180 } };
	const Bezier2 quadratic{ { 40, 330 }, { 230, 140 }, { 440, 330 } };
	const Bezier3 cubic{ { 510, 330 }, { 590, 180 }, { 780, 460 }, { 900, 260 } };
	const LineString knots{ { 60, 530 }, { 260, 440 }, { 500, 560 }, { 850, 470 } };
	const Spline2D spline = Spline2D::FromCatmullRom(knots);
	double phase = 0;
	bool playing = true, translucent = true;
	bool flat = false;
	size_t pattern = 1;
	const Array<LineStyle::Parameters> presets{ LineStyle::Dotted, LineStyle::Dashed,
		LineStyle::LongDash, LineStyle::DashDot, LineStyle::RoundDot };
	const Array<String> names{ U"Dotted", U"Dashed", U"LongDash", U"DashDot", U"RoundDot" };
	while (System::Update())
	{
		if (KeySpace.down()) playing = !playing;
		if (KeyA.down()) translucent = !translucent;
		if (KeyC.down()) pattern = (pattern + 1) % presets.size();
		if (KeyF.down()) flat = !flat;
		if (playing) phase += Scene::DeltaTime();
		if (KeyLeft.down()) phase -= 0.01;
		if (KeyRight.down()) phase += 0.01;
		LineStyle style{ presets[pattern](phase) };
		style.cap = (flat ? LineCap::Flat : LineCap::Square);
		const ColorF ink{ 0.1, 0.4, 0.8, (translucent ? 0.5 : 1.0) };
		corner.draw(style, 20, ink);
		ring.drawClosed(style, 20, ink);
		quadratic.draw(style, 12, ink);
		cubic.draw(style, 12, ink);
		spline.draw(style, 12, ink);
		font(names[pattern], flat ? U" / Flat" : U" / Square").draw(20, 560, Palette::Black);
		font(U"Space: pause  C: pattern  F: cap  A: alpha  Left/Right: phase").draw(20, 600, Palette::Black);
	}
}
```
