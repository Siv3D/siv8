# Bezier arrow gallery

This program compares quadratic and cubic arrows with small and large heads,
short curves, and heads too narrow to join. It uses the public drawing API.
The adopted design and API links are in the
[curve drawing guide](../../docs/geometry2d/bezier-drawing.md#curved-arrows).

## Run

1. Use the complete program below as `Main.cpp` in a separate Siv3D application.
   Do not replace the repository's test entry point.
2. Press **A** to toggle translucent drawing, **C** to cycle the start cap,
   **Space** to animate, **G** to toggle the thin original curve, and **B** to
   switch between 256 segments and a deliberately coarse one-segment budget.
3. Inspect each head length with and without animation. Repeat on Metal and D3D11.

Expected results:

- The tip stays at the right endpoint; the head's base center lies on the original
  curve. Larger heads change direction according to that base-to-tip chord.
- The short-curve row shrinks oversized heads while retaining shaft thickness.
- The narrow-head row omits the triangle and displays the full original curve.
- Ordinary joins remain uniformly translucent. Sharp turns, short terminal
  segments, and intersections may darken where geometry overlaps.
- Caps change only the start. Budget changes affect the shaft approximation and
  can cause head omission, but do not shift the candidate head placement.

```cpp
# include <Siv3D.hpp>

void Main()
{
    Window::Resize(1200, 900);
    Scene::SetBackground(ColorF{ 0.10, 0.12, 0.15 });
    const Font font{ 18 };
    bool translucent = false, animate = false, guides = true, coarse = false;
    int32 capIndex = 0;
    const std::array caps{ LineCap::Flat, LineCap::Square, LineCap::Round };
    const std::array<String, 3> capNames{ U"Flat", U"Square", U"Round" };
    const std::array<String, 4> labels{ U"Bezier2", U"Bezier3", U"Short curve", U"Narrow head" };
    const std::array lengths{ 24.0, 70.0, 140.0 };
    double phase = 0;
    while (System::Update())
    {
        if (KeyA.down()) translucent = !translucent;
        if (KeyC.down()) capIndex = ((capIndex + 1) % 3);
        if (KeySpace.down()) animate = !animate;
        if (KeyG.down()) guides = !guides;
        if (KeyB.down()) coarse = !coarse;
        if (animate) phase += Scene::DeltaTime();
        const double bend = (30 * Math::Sin(phase));
        const ColorF color{ 0.3, 0.8, 1.0, (translucent ? 0.5 : 1.0) };
        const int32 budget = (coarse ? 1 : 256);
        font(U"A: alpha   C: cap   Space: animate   G: guides   B: budget").draw(20, 12);
        font(U"Cap: ", capNames[capIndex], U"    segments: ", budget).draw(20, 38);
        for (int32 row = 0; row < 4; ++row)
        for (int32 column = 0; column < 3; ++column)
        {
            const Transformer2D cell{ Mat3x2::Translate(column * 400, 76 + row * 204) };
            const double length = lengths[column];
            font(labels[row], U" / length ", length).draw(16, 0);
            const Bezier2 quadratic{ { 24, 156 }, { 150, 4 + bend }, { 360, 156 } };
            const Bezier3 cubic{ { 24, 156 }, { 80, -40 + bend }, { 250, 156 }, { 360, 156 } };
            const Bezier2 shortCurve{ { 120, 130 }, { 145, 85 + bend }, { 200, 130 } };
            const Bezier2 narrow{ { 24, 156 }, { 24, 10 + bend }, { 360, 156 } };
            const auto draw = [&](const auto& curve, const double extraWidth)
            {
                if (guides) curve.drawAdaptive(1, ColorF{ 0.5 });
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
./macOS/run-tests.sh '--test-case=BezierArrow.*'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=BezierArrow.placement_benchmark'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=BezierArrow.submission_benchmark'
```

The opt-in benchmark reports CPU nanoseconds per placement for an ordinary cubic
and a collinear reversing cubic. It excludes tessellation, mesh building, command
submission, and GPU work. Treat the values as host-dependent measurements, not a
frame-rate guarantee. The normal suite validates rendering without writing images.
The submission benchmark compares `drawAdaptive` with `drawArrow` for both degrees,
using 256 offscreen curves per frame, two warmup rounds and nine measured rounds
in alternating order. It reports median CPU time including mesh and command
submission, but excluding frame updates and GPU work.
