# Rounded speech balloons

This gallery compares `RoundRect::drawBalloon()` and `drawBalloonFrame()` using
one body and tail placement. See the [public contract](../../Siv3D/include/Siv3D/RoundRect.hpp)
and [implementation guide](../../docs/geometry2d/rounded-drawing.md#rounded-speech-balloons).

## Run

1. Copy the complete program below into an application's `Main.cpp`, build, and run.
2. Move the pointer through the left panel below the controls. Both panels use
   the same relative tip. Try all four sides, the diagonal transitions, the body
   interior, its boundary, and its rounded-corner cutouts.
3. Drag the sliders. Try radius 0, negative radius, radius larger than half the
   short side, tail width 0 and larger than a side, and a very thick frame.
4. Use the presets to hold a short, sharp, diagonal, or omitted tail. Inspect at
   opacity 0.5. Move the window between displays with different pixel densities.
5. The lower examples apply rotation and nonuniform reflection to both APIs.

## Expected results

- The tail uses a straight side selected by the center-to-tip ray. Its base stays
  outside the corner arcs and shrinks to the available straight length.
- The fill has no doubled alpha or gap at the tail base. The frame follows the
  outer contour without drawing a line across that base.
- A target inside or on the bounding rectangle, nonpositive tail width, or a
  selected side without a straight section produces an ordinary RoundRect.
- The automatic side changes at a diagonal; ties select left/right. It is not
  a fixed-side attachment for an animation that must retain one side.
- The frame is centered on the reference contour. Its visible tip can extend
  beyond the target. Very thick frames and sharp turns have the same practical
  overlap limits as a closed LineString; translucent overlaps can be visible.
- The reference marker and bounding rectangle are guides, not part of the balloon.

## Program

```cpp
# include <Siv3D.hpp>

void Slider(const Font& font, const StringView label, double& value,
	const double minimum, const double maximum, const Vec2 pos)
{
	const RectF track{ pos.x + 100, pos.y + 7, 300, 20 };
	if (track.mouseOver() && MouseL.pressed())
	{
		value = (minimum + (maximum - minimum)
			* Clamp((Cursor::PosF().x - track.x) / track.w, 0.0, 1.0));
	}
	font(label).draw(pos);
	track.draw(ColorF{ 0.25 });
	RectF{ track.pos, track.w * (value - minimum) / (maximum - minimum), track.h }
		.draw(ColorF{ 0.2, 0.6, 0.8 });
}

void Main()
{
	Window::Resize(1040, 860);
	Scene::SetBackground(ColorF{ 0.12 });
	const Font font{ 18 };
	double radius = 24, tailWidth = 48, thickness = 8, opacity = 0.5;
	Vec2 target{ 400, 460 };
	bool follow = true;

	while (System::Update())
	{
		Slider(font, U"Radius", radius, -80, 140, { 20, 20 });
		Slider(font, U"Tail width", tailWidth, 0, 300, { 540, 20 });
		Slider(font, U"Frame", thickness, 0, 160, { 20, 65 });
		Slider(font, U"Opacity", opacity, 0.05, 1.0, { 540, 65 });
		if (SimpleGUI::Button(U"Pointer", { 20, 115 })) follow = true;
		if (SimpleGUI::Button(U"Short", { 160, 115 })) { follow = false; target = { 352, 360 }; }
		if (SimpleGUI::Button(U"Sharp", { 290, 115 })) { follow = false; target = { 480, 360 }; tailWidth = 8; }
		if (SimpleGUI::Button(U"Diagonal", { 420, 115 })) { follow = false; target = { 470, 220 }; }
		if (SimpleGUI::Button(U"Inside", { 580, 115 })) { follow = false; target = { 230, 360 }; }
		if (SimpleGUI::Button(U"Reset", { 720, 115 }))
		{
			radius = 24; tailWidth = 48; thickness = 8; opacity = 0.5; follow = true;
		}
		if (follow && RectF{ 10, 180, 490, 390 }.mouseOver()) target = Cursor::PosF();

		font(U"Fill: move the pointer here").draw(30, 175);
		font(U"Frame: same relative target").draw(550, 175);
		const RoundRect body{ 110, 290, 240, 140, radius };
		const ColorF ink{ 0.2, 0.75, 0.95, opacity };
		body.rect.drawFrame(1, ColorF{ 0.3 });
		body.drawBalloon(target, tailWidth, ink);
		Circle{ target, 3 }.draw(Palette::Orange);
		{
			const Transformer2D shift{ Mat3x2::Translate(520, 0) };
			body.rect.drawFrame(1, ColorF{ 0.3 });
			body.drawBalloonFrame(target, tailWidth, thickness, ink);
			Circle{ target, 3 }.draw(Palette::Orange);
		}

		font(U"Rotation and reflection").draw(30, 590);
		const RoundRect small{ -100, -40, 200, 80, radius };
		{
			const Transformer2D transform{ Mat3x2::Rotate(0.2).translated(240, 710) };
			small.drawBalloon(Vec2{ 140, 70 }, tailWidth, ink);
		}
		{
			const Transformer2D transform{ Mat3x2::Scale(-1.2, 0.8).translated(770, 710) };
			small.drawBalloonFrame(Vec2{ 140, 70 }, tailWidth, thickness, ink);
		}
		font(U"r={:.1f}  tail={:.1f}  frame={:.1f}  alpha={:.2f}"_fmt(radius, tailWidth, thickness, opacity)).draw(30, 815);
	}
}
```

## Automated checks and measurements

From the repository root on macOS:

```sh
./macOS/run-tests.sh '--test-case=~[benchmark]RoundRectBalloon.*'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=RoundRectBalloon.submission_benchmark'
```

On Windows use the corresponding `WindowsDesktop/run-tests.ps1` runner with
`-Configuration Release -TestArguments '--test-case=RoundRectBalloon.submission_benchmark'`.
The [dedicated tests](../Test_RoundRectBalloon.cpp) check placement, radius and
width boundaries, contour area and triangle winding, buffer guards, failed
allocations, translucent pixels, transforms, and exact body-only fallbacks.

The opt-in benchmark uses 128 draws per sample, 8 warm-up and 21 measured rounds,
rotating method order. It compares direct drawing, Polygon construction plus
drawing, and cached Polygon drawing for a fixed shape and a sequence of changing
radii/tips. The Polygon uses the same sampled outline, with validation disabled;
the cached changing sequence is a preparation-excluded lower bound, not the cost
of rebuilding geometry as it animates. The reference tessellation is internal
test code; this feature adds no public balloon-to-Polygon API.

It reports wall/thread CPU nanoseconds per call, p10/p90 wall times, mean fill
vertices, triangles per call, and batch draw calls. Frame submission and VSync
waits occur outside the timer. These are CPU costs, not GPU time or FPS.

The [upload diagnostic](../../docs/renderer2d/README.md#measuring-cpu-writes-to-upload-buffers)
additionally covers ordinary, square, clamped, and maximum-detail balloons in
RAM and both Metal CPU cache modes, verifying buffer contents after timing.
Do not run the benchmark concurrently with builds or other performance tests.
