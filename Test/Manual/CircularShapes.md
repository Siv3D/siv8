# Crescent, lens and teardrop gallery

This program compares direct fills, generated Shape2D fills and Polygon fills.
The geometry definitions and implementation notes are in the
[circular shape guide](../../docs/geometry2d/circular-shapes.md).
Direct frame methods are not provided; **F** draws the generated Shape2D outline.

## Run

1. Use the complete program below as `Main.cpp` in a separate Siv3D application.
   Keep the repository's test entry point unchanged.
2. **C** cycles crescent, teardrop and lens; **D** cycles direct, Shape2D and Polygon drawing.
   **A** toggles translucency, **Space** animates, **G** toggles guides, and **F**
   toggles a Shape2D outline overlay. **T** switches lens frames between 3 and 12 pixels.
   The lens page compares Shape2D, Polygon and Image in three columns; D has no effect there.
3. Compare the three drawing methods, particularly thin crescents and tips just
   outside the circle. Repeat on Metal and D3D11.

Optional arguments: `--lens`, `--frame`, `--thick`, `--rotated` and `--capture`.
Capture saves `circular-shapes-<family>-<fill|frame|thick>.png` in the application's
working directory after four frames, then exits. Repeated captures overwrite that
file; remove retained images when no longer needed. Avoid input during capture.

Expected results:

- A crescent opens in the selected direction, with 0 degrees pointing up and
  90 degrees pointing right. Increasing width turns it into a full circle at
  twice the radius. Thin crescents keep connected tips without crossed strips.
- A lens has two symmetric circular arcs, exact tips and central half-width.
  Its rows show half-widths 24, 0.1, 79.999 and 80 for a tip distance of 160.
  The last row is a circle. Image and GPU edges may rasterize differently.
  A 12-pixel frame around a sharp, thin lens can overlap according to the existing
  frame implementation; fill remains a single non-overlapping mesh.
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
    const auto& args = System::GetCommandLineArgs();
    int32 family = args.contains(U"--lens") ? 2 : 0;
    bool translucent = true, animate = false, guides = true;
    bool frame = args.contains(U"--frame"), thick = args.contains(U"--thick");
    const bool capture = args.contains(U"--capture"), rotated = args.contains(U"--rotated");
    std::array<DynamicTexture, 4> lensTextures;
    const std::array<String, 3> familyNames{ U"crescent", U"teardrop", U"lens" };
    int32 frames = 0;
    int32 mode = 0;
    double phase = 0;
    while (System::Update())
    {
        if (capture && ScreenCapture::HasNewFrame())
        {
            const FilePath path = Format(U"circular-shapes-", familyNames[family], U"-", frame ? (thick ? U"thick" : U"frame") : U"fill", U".png");
            const bool saved = ScreenCapture::GetFrame().save(path);
            System::Exit(saved ? 0 : 1);
            return;
        }
        if (KeyC.down()) family = ((family + 1) % 3);
        if (KeyT.down()) thick = !thick;
        if (KeyD.down()) mode = ((mode + 1) % 3);
        if (KeyA.down()) translucent = !translucent;
        if (KeySpace.down()) animate = !animate;
        if (KeyG.down()) guides = !guides;
        if (KeyF.down()) frame = !frame;
        if (animate) phase += Scene::DeltaTime();
        const bool crescent = (family == 0);
        font(U"C: shape   D: drawing method   A: alpha   Space: animate   G: guides   F: frame   T: thick frame").draw(20, 12);
        font(family == 2 ? U"Lens / Shape2D | Polygon | Image" : Format(crescent ? U"Crescent / " : U"Teardrop / ", modes[mode])).draw(20, 38);
        const ColorF color{ 0.3, 0.8, 1.0, (translucent ? 0.5 : 1.0) };
        for (int32 row = 0; row < 4; ++row)
        for (int32 column = 0; column < 3; ++column)
        {
            const Transformer2D cell{ Mat3x2::Translate(column * 400, 76 + row * 204) };
            if (family == 2)
            {
                const std::array halfWidths{ 24.0, 0.1, 79.999, 80.0 };
                const std::array<StringView, 3> labels{ U"Shape2D", U"Polygon", U"Image" };
                const double angle = (rotated ? 0.4 : 0.0) + (animate ? 0.35 * Math::Sin(phase) : 0);
                const Vec2 axis{ Math::Cos(angle), Math::Sin(angle) }, center{ 220, 110 };
                const Vec2 from = center - axis * 80, to = center + axis * 80;
                const double h = Min(halfWidths[row], from.distanceFrom(to) * 0.5);
                const auto lens = Shape2D::Lens(from, to, h, QualityFactor::FromSceneScaling());
                const double thickness = (thick ? 12 : 3);
                if (column == 0)
                {
                    lens.draw(color);
                    if (frame) lens.drawFrame(thickness, ColorF{ 1.0, 0.6, 0.1, 0.5 });
                }
                else if (column == 1)
                {
                    const Polygon polygon{ lens };
                    polygon.draw(color);
                    if (frame) polygon.drawFrame(thickness, ColorF{ 1.0, 0.6, 0.1, 0.5 });
                }
                else
                {
                    Image image{ 400, 204, ColorF{ 0.10, 0.12, 0.15 }.toColor() };
                    lens.paint(image, color.toColor());
                    if (frame) lens.paintFrame(image, thickness, ColorF{ 1.0, 0.6, 0.1, 0.5 }.toColor());
                    if (lensTextures[row].fill(image)) lensTextures[row].draw();
                }
                font(labels[column], U" / h = ", h, U" / ", lens.vertices().size(), U" vertices").draw(16, 0);
                if (guides)
                {
                    Line{ from, to }.draw(1, ColorF{ 0.6, 0.4 });
                    from.asCircle(2).draw(Palette::Orange); to.asCircle(2).draw(Palette::Orange);
                }
                continue;
            }
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
        if (capture && (++frames == 4)) ScreenCapture::RequestCurrentFrame();
    }
}
```

## Automated checks and performance

On macOS, run from the repository root:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=~[benchmark]CircularShapes.*,Renderer2DUpload.*'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=CircularShapes.submission_benchmark'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=CircularShapes.lens_submission_benchmark'
```

The dedicated [tests](../Test_CircularShapes.cpp) check arc membership, tangency,
independent analytic area, triangle winding and single coverage, matching stored
and direct meshes, index offsets, allocation failure, negative/zero radii, circle
fallbacks, quality limits, Image outlines and translucent transformed rendering.
Lens cases also check both reflection symmetries, reversed tips, the thin/circle
limits, independent disk-intersection area, known triangulation and Shape2D /
Polygon / Image consistency.

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

The lens benchmark separates construction alone, construction plus fill, cached
Shape2D / Polygon fills, and their cached frames. It uses half-length/half-width
80/24, 80/0.1, 80/79.999 and 500/500, four warmups and 21 rotating rounds of
128 calls. Before each timed batch it warms the CPU for 5 ms. The reported wall
medians and p10/p90 exclude scene updates and GPU execution. Construction includes
two owned arrays and their destruction; cached draws reuse both arrays. The
[upload diagnostic](../../tools/renderer2d-index-upload.mm) uses the same contours
with existing Polygon fill and Shape2D frame builders in RAM and both Metal
cache modes.
