# Independent-corner rounded rectangle drawing

This program compares direct `RectF::drawRounded()` / `drawRoundedFrame()` with
per-frame `rounded(tl, tr, br, bl)` Polygon construction. The public contracts
are in [RectF.hpp](../../Siv3D/include/Siv3D/RectF.hpp); implementation and cost
notes are in the [rounded drawing guide](../../docs/geometry2d/rounded-drawing.md#independent-corner-rectangles).

## Run

1. Copy the complete program below into an application's `Main.cpp`, build, and run.
2. Drag within the slider bars to change the four radii, dimensions, inner/outer widths, and opacity.
3. Try the single large corner, alternating corners, square corners, and thick
   frame presets. Set opacity to 0.5 to inspect joins.
4. Test inner width 0, outer width 0, and inner width at or above half the short
   side. Move the window between displays with different pixel densities.

## Expected results

- Radius order is top-left, top-right, bottom-right, bottom-left. Negative radii
  become square corners. Excessive neighboring radii shrink proportionally.
- Direct and Polygon fills describe the same shape; arc tessellation can differ.
- The direct frame has no dark joins or internal seams at partial opacity.
  Its inner corners can become square; when the inner width or height vanishes,
  the entire outer shape is filled.
- The Polygon reference frame is centered and uses a generic polyline stroke.
  Compare widths with equal inner/outer values. Asymmetric widths, zero-radius
  outer corners, and heavily contracted inner corners intentionally differ.
- The direct frame's outer radius is the adjusted base radius plus the outer
  width, including corners whose base radius is zero. Contracted inner radii
  are fitted to the smaller rectangle; very thick frames need not be exact
  constant-distance offsets of the original curve.

## Program

```cpp
# include <Siv3D.hpp>

void Slider(const Font& font, const StringView label, double& value,
	const double minimum, const double maximum, const Vec2 pos,
	const double labelWidth, const double barWidth)
{
	const RectF track{ pos.x + labelWidth, pos.y + 7, barWidth, 20 };
	if (track.mouseOver() && MouseL.pressed())
	{
		const double t = Clamp((Cursor::PosF().x - track.x) / track.w, 0.0, 1.0);
		value = (minimum + (maximum - minimum) * t);
	}
	font(label).draw(pos);
	track.draw(ColorF{ 0.25 });
	const double t = ((value - minimum) / (maximum - minimum));
	RectF{ track.pos, track.w * t, track.h }.draw(ColorF{ 0.2, 0.6, 0.8 });
}

void Main()
{
	Window::Resize(1100, 850);
	Scene::SetBackground(ColorF{ 0.12 });
	const Font font{ 18 };
	std::array<double, 4> radii{ 28, 6, 28, 6 };
	double width = 240, height = 120;
	double inner = 8, outer = 8, opacity = 0.5;

	while (System::Update())
	{
		Slider(font, U"TL", radii[0], -20.0, 300.0, Vec2{ 20, 20 }, 35, 180);
		Slider(font, U"TR", radii[1], -20.0, 300.0, Vec2{ 280, 20 }, 35, 180);
		Slider(font, U"BR", radii[2], -20.0, 300.0, Vec2{ 540, 20 }, 35, 180);
		Slider(font, U"BL", radii[3], -20.0, 300.0, Vec2{ 800, 20 }, 35, 180);
		Slider(font, U"Width", width, 1.0, 300.0, Vec2{ 20, 65 }, 65, 155);
		Slider(font, U"Height", height, 1.0, 160.0, Vec2{ 280, 65 }, 65, 155);
		Slider(font, U"Inner", inner, 0.0, 100.0, Vec2{ 540, 65 }, 65, 155);
		Slider(font, U"Outer", outer, 0.0, 40.0, Vec2{ 800, 65 }, 65, 155);
		Slider(font, U"Opacity", opacity, 0.05, 1.0, Vec2{ 20, 110 }, 80, 140);

		if (SimpleGUI::Button(U"Single corner", Vec2{ 280, 110 })) radii = { 160, 0, 0, 0 };
		if (SimpleGUI::Button(U"Alternating", Vec2{ 465, 110 })) radii = { 120, 0, 120, 0 };
		if (SimpleGUI::Button(U"Square", Vec2{ 640, 110 })) radii = { 0, 0, 0, 0 };
		if (SimpleGUI::Button(U"Thick frame", Vec2{ 770, 110 }))
		{
			radii = { 120, 0, 0, 0 };
			width = 200;
			height = 120;
			inner = 58;
			outer = 5;
		}

		const ColorF ink{ 0.2, 0.75, 0.95, opacity };
		const double tl = radii[0], tr = radii[1], br = radii[2], bl = radii[3];
		const RectF a{ 110, 230, width, height }, b{ 650, 230, width, height };
		const RectF c{ 110, 560, width, height }, d{ 650, 560, width, height };
		font(U"Direct fill").draw(40, 180);
		font(U"Polygon fill").draw(580, 180);
		font(U"Direct frame: separate inner / outer widths").draw(40, 460);
		font(U"Polygon frame: centered total width").draw(580, 460);
		a.drawRounded(tl, tr, br, bl, ink);
		b.rounded(tl, tr, br, bl).draw(ink);
		c.drawRoundedFrame(tl, tr, br, bl, inner, outer, ink);
		d.rounded(tl, tr, br, bl).drawFrame(inner + outer, ink);
		font(U"Radii: {:.1f}, {:.1f}, {:.1f}, {:.1f}"_fmt(tl, tr, br, bl)).draw(40, 810);
	}
}
```

## Automated checks and CPU measurements

On macOS, from the repository root:

```sh
./macOS/run-tests.sh '--test-case=RectRoundedDrawing.*'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=RectRoundedDrawing.submission_benchmark'
```

The benchmark reports median wall time, 10th/90th percentiles, and (on macOS)
thread CPU time for direct drawing, temporary Polygon construction, and cached
Polygons. It cycles the same radius sequence, warms up buffers, and rotates
measurement order. Cached Polygons exclude construction. The fill comparison
also includes a cached Polygon built from the exact direct-fill mesh, to isolate
submission cost from tessellation density. Triangle totals and draw-call counts
confirm that geometry was submitted and show differences in batching.

Rendering is offscreen, and timings exclude frame presentation and GPU execution.
The normal Polygon conversion has a different tessellation density; its timings
are end-to-end API costs. Read-modify-write operations on GPU upload memory can
also dominate the comparison; see the [upload-memory diagnostic](../../docs/renderer2d/README.md#measuring-cpu-writes-to-upload-buffers).
The tests check mesh budgets, winding, single coverage, allocation failure,
public overloads, normalization, transforms, resolution, and partial opacity
in memory.
