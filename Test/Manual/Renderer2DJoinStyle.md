# Triangle and Quad GPU joins

This gallery checks explicit GPU joins, alpha coverage, winding reversal, and
zoomed round corners. It does not exercise Image painting or polygon buffers.
Run it as a separate Siv3D application with the current engine, on Metal and D3D11.
Do not replace the repository test runner's `Main.cpp`.

## Steps and expected results

1. Run the complete program below. Columns are Default, Bevel, Round, Miter.
   Rows are an ordinary triangle, convex quad, concave quad, crossing quad, acute
   triangle, and a triangle whose inner hole disappears at the selected width.
2. Hold Space to reverse vertex order. Explicit joins should retain their shape.
   Use Up/Down to change width and Left/Right to change zoom (about each cell).
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

Record backend, scene MSAA/resolution settings, width, zoom, and inspected cases.
The automated checks in [Test_Renderer2DJoinStyle.cpp](../Test_Renderer2DJoinStyle.cpp)
cover captured pixels and mesh topology; this gallery is for interactive inspection.

```cpp
# include <Siv3D.hpp>

void Main()
{
    Window::Resize(1200, 900);
    Scene::SetBackground(ColorF{ 0.12 });
    const Font font{ 18 };
    const Array<JoinStyle> styles{
        JoinStyle::Default, JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter };
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
    while (System::Update())
    {
        if (KeyUp.down()) width = Min(width + 4, 100.0);
        if (KeyDown.down()) width = Max(width - 4, 1.0);
        if (KeyRight.down()) zoom = Min(zoom * 2, 4.0);
        if (KeyLeft.down()) zoom = Max(zoom * 0.5, 0.5);
        if (KeyP.down()) patterned = !patterned;
        font(U"Width: {}  Zoom: {}  Space: reverse  P: pattern"_fmt(width, zoom)).draw(16, 8);
        for (size_t column = 0; column < styles.size(); ++column)
        {
            font(Format(styles[column])).draw(column * 300 + 16, 38);
            for (size_t row = 0; row < shapes.size(); ++row)
            {
                auto points = shapes[row];
                if (KeySpace.pressed()) points.reverse();
                const Transformer2D transform{
                    Mat3x2::Scale(zoom).translated(column * 300 + 150, row * 130 + 130) };
                const auto draw = [&](const auto& shape)
                {
                    if (patterned)
                    {
                        shape.drawFrame(width, Pattern::PolkaDot{
                            .primary = ColorF{ 0.3, 0.8, 1.0, 0.5 },
                            .background = ColorF{ 1.0, 0.3, 0.2, 0.5 } }, styles[column]);
                    }
                    else shape.drawFrame(width, ColorF{ 0.3, 0.8, 1.0, 0.5 }, styles[column]);
                };
                if (points.size() == 3) draw(Triangle{ points[0], points[1], points[2] });
                else draw(Quad{ points[0], points[1], points[2], points[3] });
            }
        }
    }
}
```
