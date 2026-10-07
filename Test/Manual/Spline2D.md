# Spline2D path gallery

This program shows editable Catmull-Rom paths, distance-based movement, equally
spaced markers, and the closest point to the mouse. It needs no external assets
and does not write output files.

## Execution

1. Paste the complete code below into a separate application's `Main.cpp` using
   this Siv3D revision. Keep the repository test entry points intact.
2. Run on macOS (Metal) or Windows (D3D11).
3. Drag a white knot with the left mouse button. Press U to switch between
   centripetal and uniform interpolation, and C to open or close the path.
4. Press Space to pause movement; press R to restart from the beginning.
5. Move the mouse near the curve and around a self-crossing to inspect projection.

## Expected results

- The curve passes through every white knot. Uneven knot spacing produces
  visibly different shapes in the two interpolation modes.
- The orange disk travels at approximately 120 path units per second. It should
  not visibly accelerate on short segments or at ordinary segment boundaries.
- Blue markers are spaced by 40 units along the curve, not by equal parameter
  increments. The final gap of a closed path need not be exactly 40 units.
- A closed path joins smoothly at the seam. On an open path, wrapping deliberately
  jumps from the last knot to the first; C changes geometry, not the movement mode.
- The magenta marker is on the curve at a closest position to the mouse. It may
  jump between branches near a crossing where the closest branch changes.
- Pausing leaves the moving disk stationary. Editing rebuilds the geometry and
  measurement, so its position may change after a knot is dragged.

The numerical and storage checks are in [Test_Spline2D.cpp](../Test_Spline2D.cpp).

## Complete Main.cpp

```cpp
# include <Siv3D.hpp>

void Main()
{
    Window::Resize(1000, 700);
    Scene::SetBackground(ColorF{ 0.10, 0.12, 0.16 });
    const Font font{ 18 };
    Array<Vec2> knots{
        { 100, 350 }, { 140, 300 }, { 220, 170 },
        { 720, 180 }, { 760, 430 }, { 340, 560 }
    };
    bool closed = true;
    bool uniform = false;
    bool paused = false;
    bool dirty = true;
    Optional<size_t> dragged;
    double distance = 0.0;
    Spline2D path;
    Spline2DMeasure measured;
    LineString outline;

    while (System::Update())
    {
        if (KeyU.down()) { uniform = not uniform; dirty = true; }
        if (KeyC.down()) { closed = not closed; dirty = true; }
        if (KeySpace.down()) { paused = not paused; }
        if (KeyR.down()) { distance = 0.0; }

        const Vec2 cursor = Cursor::PosF();
        if (MouseL.down())
        {
            for (size_t i = 0; i < knots.size(); ++i)
            {
                if (knots[i].distanceFrom(cursor) <= 12)
                {
                    dragged = i;
                    break;
                }
            }
        }
        if (MouseL.pressed() && dragged)
        {
            // Keep neighboring knots distinct for this editing demonstration.
            bool distinct = true;
            for (size_t i = 0; i < knots.size(); ++i)
            {
                if ((i != *dragged) && (knots[i].distanceFrom(cursor) < 2))
                {
                    distinct = false;
                }
            }
            if (distinct)
            {
                knots[*dragged] = cursor;
                dirty = true;
            }
        }
        if (MouseL.up()) { dragged.reset(); }

        if (dirty)
        {
            path = Spline2D::FromCatmullRom(knots,
                (closed ? CloseRing::Yes : CloseRing::No),
                (uniform ? CatmullRomParameterization::Uniform
                         : CatmullRomParameterization::Centripetal));
            measured = Spline2DMeasure{ path };
            path.toLineStringAdaptive(outline, 0.35);
            dirty = false;
        }

        if (closed) { outline.drawClosed(3, ColorF{ 0.75 }); }
        else { outline.draw(3, ColorF{ 0.75 }); }

        for (double s = 0; s < measured.length(); s += 40)
        {
            Circle{ measured.pointAtDistance(s), 4 }.draw(Palette::Skyblue);
        }
        if (not paused) { distance += 120.0 * Scene::DeltaTime(); }
        Circle{ measured.pointAtDistance(distance, DistanceMode::Wrap), 9 }
            .draw(Palette::Orange);

        if (const auto closest = path.computeClosestPoint(cursor))
        {
            Line{ cursor, closest->point }.draw(1, Palette::Magenta);
            Circle{ closest->point, 5 }.draw(Palette::Magenta);
        }
        for (const Vec2 knot : knots) { Circle{ knot, 7 }.draw(Palette::White); }

        font(U"Drag knots | U: interpolation | C: close | Space: pause | R: restart")
            .draw(20, 16);
        font(uniform ? U"Uniform" : U"Centripetal").draw(20, 44);
        font(closed ? U"Closed / Wrap" : U"Open / Wrap").draw(20, 72);
    }
}
```
