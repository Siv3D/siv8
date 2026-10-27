# Curved and polyline arrow gallery

This program compares Bezier, circle, LineString, and Spline2D arrows with small
and large heads, short curves, and heads too narrow to join. It uses the public drawing API.
The adopted design and API links are in the
[curve drawing guide](../../docs/geometry2d/bezier-drawing.md#curved-arrows).

## Run

1. Use the complete program below as `Main.cpp` in a separate Siv3D application.
   Do not replace the repository's test entry point.
2. Press **E** to cycle through Bezier, circle, LineString, and Spline2D arrows.
   Press **N** to toggle narrow heads in the circle and path views. Press **A** to toggle translucent drawing, **C** to cycle the start cap,
   **Space** to animate, **G** to toggle the thin original curve, and **B** to
   switch between 256 segments and a deliberately coarse one-segment budget
   per Bezier interval in Bezier and Spline2D arrows. Circle arcs use automatic
   bounded subdivision; LineString uses its original vertices.
3. Inspect each head length with and without animation. Repeat on Metal and D3D11.

Expected results:

- The tip stays at the source endpoint; the head's base center lies on the original
  curve. Larger heads change direction according to that base-to-tip chord.
- The short-curve row shrinks oversized heads while retaining shaft thickness.
- The narrow-head row omits the triangle and displays the full original curve.
- Ordinary joins remain uniformly translucent. Sharp turns, short terminal
  segments, and intersections may darken where geometry overlaps.
- Caps change only the start. Budget changes affect the Bezier/Spline shaft approximation
  and can cause head omission, but do not shift the candidate head placement.
- In the circle view, the full-turn row always uses Flat. With guides on, the
  marker at the start also marks the head tip. Both sweep directions follow the
  source circle. A large head on an arc longer than a semicircle may be omitted
  when its base reaches the antipodal point. Narrow heads may also be omitted.
- The path views include reversed paths, a closed path with Flat start, and a
  short path. The closed row puts the tip at the start marker. Large spline heads
  can start in an earlier Bezier interval without jumping to an interval boundary.

```cpp
# include <Siv3D.hpp>

void Main()
{
    Window::Resize(1200, 900);
    Scene::SetBackground(ColorF{ 0.10, 0.12, 0.15 });
    const Font font{ 18 };
    bool translucent = false, animate = false, guides = true, coarse = false;
    bool narrowHeads = false;
    int32 family = 0;
    int32 capIndex = 0;
    const std::array caps{ LineCap::Flat, LineCap::Square, LineCap::Round };
    const std::array<String, 3> capNames{ U"Flat", U"Square", U"Round" };
    const std::array<String, 4> labels{ U"Bezier2", U"Bezier3", U"Short curve", U"Narrow head" };
    const std::array<String, 4> families{ U"Bezier", U"Circle", U"LineString", U"Spline2D" };
    const std::array<String, 4> pathLabels{ U"Forward", U"Reversed", U"Closed / Flat", U"Short / shrink" };
    const std::array lengths{ 24.0, 70.0, 140.0 };
    double phase = 0;
    while (System::Update())
    {
        if (KeyE.down()) family = ((family + 1) % 4);
        if (KeyN.down()) narrowHeads = !narrowHeads;
        if (KeyA.down()) translucent = !translucent;
        if (KeyC.down()) capIndex = ((capIndex + 1) % 3);
        if (KeySpace.down()) animate = !animate;
        if (KeyG.down()) guides = !guides;
        if (KeyB.down()) coarse = !coarse;
        if (animate) phase += Scene::DeltaTime();
        const double bend = (30 * Math::Sin(phase));
        const ColorF color{ 0.3, 0.8, 1.0, (translucent ? 0.5 : 1.0) };
        const int32 budget = (coarse ? 1 : 256);
        font(U"E: family   A: alpha   C: cap   Space: animate   G: guides   B: Bezier/Spline budget   N: narrow heads").draw(20, 12);
        font(families[family], U"    Cap: ", capNames[capIndex], U"    Bezier/Spline budget: ", budget).draw(20, 38);
        for (int32 row = 0; row < 4; ++row)
        for (int32 column = 0; column < 3; ++column)
        {
            const Transformer2D cell{ Mat3x2::Translate(column * 400, 76 + row * 204) };
            const double length = lengths[column];
            if (family == 1)
            {
                const std::array<String, 4> names{ U"Clockwise", U"Counterclockwise", U"Full turn / Flat", U"Short arc / shrink" };
                font(names[row], U" / length ", length).draw(16, 0);
                const Circle circle{ 185, 108, 65 };
                const double start = (row == 2 ? 0.0 : -60_deg);
                const double sweep = (row == 2 ? 360_deg : (row == 3 ? 35_deg : (row == 1 ? -250_deg : 250_deg)));
                const double headLength = (length + (animate ? bend * 0.3 : 0));
                circle.drawArcArrow((row == 2 ? LineCap::Flat : caps[capIndex]), start, sweep,
                    8, SizeF{ (narrowHeads ? 0.001 : length * 0.9), headLength }, color);
                if (guides)
                {
                    circle.drawArc(start, sweep, 0.5, 0.5, ColorF{ 0.5 });
                    circle.getPointByAngle(start).asCircle(3).draw(Palette::Orange);
                }
                continue;
            }
            if (family >= 2)
            {
                font(pathLabels[row], U" / length ", length).draw(16, 0);
                LineString path{ { 30, 150 }, { 95, 60 + bend }, { 170, 150 }, { 240, 60 }, { 340, 150 } };
                if (row == 1) path.reverse();
                if (row == 2) path = { { 125, 60 }, { 270, 80 }, { 280, 150 }, { 115, 150 } };
                if (row == 3) path = { { 140, 130 }, { 165, 105 + bend }, { 200, 130 } };
                const LineCap cap = (row == 2 ? LineCap::Flat : caps[capIndex]);
                const SizeF head{ (narrowHeads ? 0.001 : length * 0.65), length };
                if (family == 2)
                {
                    if (row == 2) path.push_back(path.front());
                    path.drawArrow(cap, 8, head, color);
                    if (guides) path.draw(1, ColorF{ 0.5 });
                }
                else
                {
                    const auto spline = Spline2D::FromCatmullRom(path, row == 2 ? CloseRing::Yes : CloseRing::No);
                    spline.drawArrow(cap, 8, head, color, 0.5, budget);
                    if (guides) spline.draw(1, ColorF{ 0.5 });
                }
                if (guides) path.front().asCircle(3).draw(Palette::Orange);
                continue;
            }
            font(labels[row], U" / length ", length).draw(16, 0);
            const Bezier2 quadratic{ { 24, 156 }, { 150, 4 + bend }, { 360, 156 } };
            const Bezier3 cubic{ { 24, 156 }, { 80, -40 + bend }, { 250, 156 }, { 360, 156 } };
            const Bezier2 shortCurve{ { 120, 130 }, { 145, 85 + bend }, { 200, 130 } };
            const Bezier2 narrow{ { 24, 156 }, { 24, 10 + bend }, { 360, 156 } };
            const auto draw = [&](const auto& curve, const double extraWidth)
            {
                if (guides) curve.draw(1, ColorF{ 0.5 });
                curve.drawArrow(caps[capIndex], 8, SizeF{ extraWidth, length }, color, 0.5, budget);
            };
            if (row == 0) draw(quadratic, length * 0.9);
            if (row == 1) draw(cubic, length * 0.9);
            if (row == 2) draw(shortCurve, length * 0.9);
            if (row == 3) draw(narrow, 0.001);
        }
    }
}
```

## Automated checks and placement timing

On macOS, from the repository root:

```sh
./macOS/run-tests.sh '--test-case=BezierArrow.*,CircleArcArrow.*,PathArrow.*'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=BezierArrow.placement_benchmark'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=BezierArrow.submission_benchmark'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=CircleArcArrow.submission_benchmark'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=PathArrow.submission_benchmark'
```

The opt-in benchmark reports CPU nanoseconds per placement for an ordinary cubic
and a collinear reversing cubic. It excludes tessellation, mesh building, command
submission, and GPU work. Treat the values as host-dependent measurements, not a
frame-rate guarantee. The normal suite validates rendering without writing images.
The submission benchmark compares `draw` with `drawArrow` for both degrees,
using 256 offscreen curves per frame, two warmup rounds and nine measured rounds
in alternating order. It reports median CPU time including mesh and command
submission, but excluding frame updates and GPU work.

The circle benchmark uses eight warmup rounds and 21 alternating measured rounds
of 128 offscreen calls per method. It compares ordinary arcs, short arcs, full
turns, omitted heads, and maximum subdivision against `Circle::drawArc()`, and
reports triangle and batch counts. These APIs perform different geometry work;
the ratio is an added CPU submission cost, not a GPU or frame-rate ratio.

The path benchmark uses the same eight/21/128 warmup, sample, and draw counts.
It compares retained LineStrings with 33 or 257 points and retained Spline2Ds with
4 or 32 cubic intervals, using ordinary and oversized heads. Spline `draw()` uses
its usual Square caps; arrow drawing uses Flat. Construction, frame updates,
and GPU work are outside the timed section. On macOS it reports both elapsed
and thread CPU time with median and p10/p90. VSync and benchmark thread QoS are
restored afterward. Oversized heads may remove most or all of the shaft, so
compare triangle counts along with submission time.
