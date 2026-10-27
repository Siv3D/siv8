# Crescent and teardrop gallery

This program compares direct fills, generated Shape2D fills and Polygon fills.
The geometry definitions and implementation notes are in the
[circular shape guide](../../docs/geometry2d/circular-shapes.md).
Direct frame methods are not provided; **F** draws the generated Shape2D outline.

## Run

1. Use the complete program below as `Main.cpp` in a separate Siv3D application.
   Keep the repository's test entry point unchanged.
2. **C** switches shape family; **D** cycles direct, Shape2D and Polygon drawing.
   **A** toggles translucency, **Space** animates, **G** toggles guides, and **F**
   toggles a Shape2D outline overlay.
3. Compare the three drawing methods, particularly thin crescents and tips just
   outside the circle. Repeat on Metal and D3D11.

Expected results:

- A crescent opens in the selected direction, with 0 degrees pointing up and
  90 degrees pointing right. Increasing width turns it into a full circle at
  twice the radius. Thin crescents keep connected tips without crossed strips.
- A teardrop joins the outside tip to the circle tangentially. A tip inside or
  on the circle leaves only the circle. Only the tip is sharp.
- Translucent fills remain uniformly colored; they are not composed by painting
  a background-colored cutout or overlapping a circle and a triangle.
- For equal quality, nondegenerate direct and generated fills match. The ordinary
  Circle fallback can have small edge-rasterization differences from the stored
  boundary fan. Geometry is generated each frame in this visual program; it is
  not a cached-shape performance demonstration.
- The generated outline has no internal spokes. Thick frames and sharp tips use
  the existing polyline join behavior and can overlap.

```cpp
# include <Siv3D.hpp>

void Main()
{
    Window::Resize(1200, 900);
    Scene::SetBackground(ColorF{ 0.10, 0.12, 0.15 });
    const Font font{ 18 };
    const std::array<String, 3> modes{ U"Direct fill", U"Shape2D fill", U"Polygon fill" };
    const std::array angles{ -60_deg, 0_deg, 90_deg };
    const std::array widths{ 0.6, 0.02, 1.99, 2.1 };
    const std::array distances{ 1.8, 1.001, 2.4, 0.7 };
    const std::array<String, 4> rows{ U"Ordinary", U"Thin / near boundary", U"Near circle / long tip", U"Circle fallback" };
    bool crescent = true, translucent = true, animate = false, guides = true, frame = false;
    int32 mode = 0;
    double phase = 0;
    while (System::Update())
    {
        if (KeyC.down()) crescent = !crescent;
        if (KeyD.down()) mode = ((mode + 1) % 3);
        if (KeyA.down()) translucent = !translucent;
        if (KeySpace.down()) animate = !animate;
        if (KeyG.down()) guides = !guides;
        if (KeyF.down()) frame = !frame;
        if (animate) phase += Scene::DeltaTime();
        font(U"C: shape   D: drawing method   A: alpha   Space: animate   G: guides   F: Shape2D frame").draw(20, 12);
        font((crescent ? U"Crescent / " : U"Teardrop / "), modes[mode]).draw(20, 38);
        const ColorF color{ 0.3, 0.8, 1.0, (translucent ? 0.5 : 1.0) };
        for (int32 row = 0; row < 4; ++row)
        for (int32 column = 0; column < 3; ++column)
        {
            const Transformer2D cell{ Mat3x2::Translate(column * 400, 76 + row * 204) };
            font(rows[row], U" / ", Math::ToDegrees(angles[column]), U" deg").draw(16, 0);
            const Circle circle{ 220, 118, (crescent ? 60 : 40) };
            const double angle = angles[column] + (animate ? 0.2 * Math::Sin(phase) : 0);
            const Vec2 direction{ Math::Sin(angle), -Math::Cos(angle) };
            const double factor = (animate ? (1 + 0.15 * Math::Sin(phase * 0.7)) : 1);
            const double width = circle.r * widths[row] * factor;
            const Vec2 tip = circle.center + direction * (circle.r * distances[row] * factor);
            const auto makeShape = [&]
            {
                const auto quality = QualityFactor::FromSceneScaling();
                return crescent ? Shape2D::Crescent(circle, angle, width, quality)
                                : Shape2D::Teardrop(circle, tip, quality);
            };
            if (mode == 0)
            {
                if (crescent) circle.drawCrescent(angle, width, color);
                else circle.drawTeardrop(tip, color);
            }
            else if (mode == 1) makeShape().draw(color);
            else Polygon{ makeShape() }.draw(color);
            if (frame) makeShape().drawFrame(3, Palette::Orange);
            if (guides)
            {
                circle.drawFrame(1, ColorF{ 0.6 });
                circle.center.asCircle(2).draw(Palette::Orange);
                if (crescent) Line{ circle.center, circle.center + direction * 36 }.drawArrow(1, SizeF{ 5, 8 }, Palette::Orange);
                else tip.asCircle(3).draw(Palette::Orange);
            }
        }
    }
}
```

## Automated checks and performance

On macOS, run from the repository root:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=~[benchmark]CircularShapes.*,Renderer2DUpload.*'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=CircularShapes.submission_benchmark'
```

The dedicated [tests](../Test_CircularShapes.cpp) check arc membership, tangency,
independent analytic area, triangle winding and single coverage, matching stored
and direct meshes, index offsets, allocation failure, negative/zero radii, circle
fallbacks, quality limits, Image outlines and translucent transformed rendering.

The benchmark compares direct fills, per-call Shape2D construction plus drawing,
cached Shape2Ds and cached Polygons. It uses radii 64 and 1000, both fixed geometry
and a repeating 32-shape changing sequence, eight warmup rounds and 21 measured
rounds of 128 offscreen draws. The method order rotates. All methods use the same
sampled contour and triangle indices, and the benchmark checks triangle and draw
counts. Scene updates and GPU execution are excluded; on macOS thread CPU time
is also reported. VSync and thread QoS are restored afterward.

Cached changing sequences exclude preparation and represent a reuse lower bound,
not the cost of constructing arbitrary animated geometry. The direct path avoids
owned intermediate arrays but still evaluates geometry each call. Use the
[upload diagnostic](../../tools/renderer2d-index-upload.mm), following the
[upload guide](../../docs/renderer2d/README.md#measuring-cpu-writes-to-upload-buffers),
to compare RAM, Metal default-cache and write-combined memory at `-Os` and `-O3`.
