# Triangle and Quad GPU joins

This gallery checks GPU joins, alpha coverage, winding reversal, and
zoomed round corners. It does not exercise Image painting or polygon buffers.
Run it as a separate Siv3D application with the current engine, on Metal and D3D11.
Do not replace the repository test runner's `Main.cpp`.

## Steps and expected results

1. Run the complete program below. Columns are Bevel, Round, Miter.
   Rows are an ordinary triangle, convex quad, concave quad, crossing quad, acute
   triangle, and a triangle whose inner hole disappears at the selected width.
2. Hold Space to reverse vertex order. All styles should retain their shape.
   Use Up/Down to change width and Left/Right to change zoom (about each cell).
   Press F to cycle centered / inward-only / outward-only / inner:outer 1:3.
   Width is the total of the two sides. Inward-only frames stay inside the original
   contour and fill it when the hole disappears; outward-only frames stay outside.
3. Press P to switch between a translucent solid and a translucent pattern.
   There should be no dark/bright seams or doubled alpha where edges meet corners.
   The pattern should continue through each corner without a coordinate reset.
4. Inspect corners at 4x zoom. Round should follow a circular outline; Miter can
   extend far from acute vertices. Bevel retains Siv3D's historical clipped-tip
   shape, which is not a simple chord between the offset edges.
5. Use width 1, 12, 40, and 100; inspect the disappearance of inner holes, concave
   regions, crossing regions, and the transition between mesh and fallback paths.
   Compare with the baseline gallery on the same scene settings if evaluating a
   future change to the join contract.

6. Press T to cycle from the gallery to a square inset-collapse check (width 21)
   and a short-edge quad check (width 12.360679775), then back. Select centered mode with F for these transition widths. In these modes,
   Up/Down changes width by 0.001, with initial zoom 4x. Cross each boundary in
   both directions. Outer Round corners should retain the same subdivision
   quality when the direct path switches to fallback. The usual subdivision
   changes at radius/angle quality thresholds are separate from this check.

Record backend, scene MSAA/resolution settings, width, zoom, and inspected cases.
The automated checks in [Test_Renderer2DJoinStyle.cpp](../Test_Renderer2DJoinStyle.cpp)
cover captured pixels and mesh topology; this gallery is for interactive inspection.

```cpp
# include <Siv3D.hpp>

void Main()
{
    Window::Resize(900, 900);
    Scene::SetBackground(ColorF{ 0.12 });
    const Font font{ 18 };
    const Array<JoinStyle> styles{
        JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter };
    const Array<Array<Vec2>> shapes{
        {{-70,-45}, {75,-35}, {-10,45}},
        {{-70,-45}, {70,-30}, {60,45}, {-60,35}},
        {{-70,-45}, {70,-30}, {0,0}, {-60,45}},
        {{-70,-45}, {70,45}, {-70,45}, {70,-45}},
        {{-75,0}, {60,-10}, {60,10}},
        {{-28,-20}, {28,-20}, {0,28}},
    };
    double width = 12, zoom = 1;
    bool patterned = false;
    int transition = 0, frameMode = 0;
    while (System::Update())
    {
        if (KeyF.down()) frameMode = (frameMode + 1) % 4;
        if (KeyT.down())
        {
            transition = (transition + 1) % 3;
            width = (transition == 1 ? 20.999 : transition == 2 ? 12.359679775 : 12);
            zoom = (transition ? 4 : 1);
        }
        const double step = (transition ? 0.001 : 4);
        if (KeyUp.down()) width = Min(width + step, 100.0);
        if (KeyDown.down()) width = Max(width - step, 1.0);
        if (KeyRight.down()) zoom = Min(zoom * 2, 4.0);
        if (KeyLeft.down()) zoom = Max(zoom * 0.5, 0.5);
        if (KeyP.down()) patterned = !patterned;
        font(U"Width: {:.6f}  Zoom: {}  Space: reverse  P: pattern  T: transition  F: mode {}"_fmt(width, zoom, frameMode)).draw(16, 8);
        const auto draw = [&](const auto& shape, const JoinStyle style)
        {
            const auto apply = [&](const auto& fill)
            {
                if (frameMode == 0) shape.drawFrame(width, fill, style);
                else
                {
                    const double inner = (frameMode == 1 ? width : frameMode == 2 ? 0 : width * 0.25);
                    shape.drawFrame(inner, width - inner, fill, style);
                }
            };
            if (patterned)
            {
                apply(Pattern::PolkaDot{
                    .primary = ColorF{ 0.3, 0.8, 1.0, 0.5 },
                    .background = ColorF{ 1.0, 0.3, 0.2, 0.5 } });
            }
            else apply(ColorF{ 0.3, 0.8, 1.0, 0.5 });
        };
        if (transition)
        {
            Array<Vec2> points = (transition == 1)
                ? Array<Vec2>{{-10.5,-10.5}, {10.5,-10.5}, {10.5,10.5}, {-10.5,10.5}}
                : Array<Vec2>{{-30,-20}, {30,-20}, {30,-10}, {-30,20}};
            if (KeySpace.pressed()) points.reverse();
            const Transformer2D transform{ Mat3x2::Scale(zoom).translated(450,450) };
            draw(Quad{points[0], points[1], points[2], points[3]}, JoinStyle::Round);
            continue;
        }
        for (size_t column = 0; column < styles.size(); ++column)
        {
            font(Format(styles[column])).draw(column * 300 + 16, 38);
            for (size_t row = 0; row < shapes.size(); ++row)
            {
                auto points = shapes[row];
                if (KeySpace.pressed()) points.reverse();
                const Transformer2D transform{
                    Mat3x2::Scale(zoom).translated(column * 300 + 150, row * 130 + 130) };
                if (points.size() == 3) draw(Triangle{ points[0], points[1], points[2] }, styles[column]);
                else draw(Quad{ points[0], points[1], points[2], points[3] }, styles[column]);
            }
        }
    }
}
```
