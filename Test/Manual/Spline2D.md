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
6. Press Q to cycle targets 0.25 / 0.5 / 1 pixel and B to switch between 256 and
   four subdivisions per Bezier interval. Hold Up/Down to zoom. G switches between
   `path.draw()` and a cached coordinate-space outline; H switches scene/display
   render resolution. Drag across interval boundaries and inspect the closing seam.

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

- Direct drawing adapts to zoom and render resolution. The cached outline retains
  its coordinate-space subdivision until edited. A four-segment per-curve budget
  may show facets but retains the knots. Quality controls do not change movement
  measurements or the path's geometry.

The numerical and storage checks are in [Test_Spline2D.cpp](../Test_Spline2D.cpp).

## Complete Main.cpp

```cpp
# include <Siv3D.hpp>

void Main()
{
    Window::Resize(1000, 700);
    Scene::Configure(SceneConfig::Fixed({ 1000, 700 }, SceneResolution::SceneSize));
    Scene::SetBackground(ColorF{ 0.10, 0.12, 0.16 });
    const Font font{ 18 };
    Array<Vec2> knots{
        { 100, 350 }, { 140, 300 }, { 220, 170 },
        { 720, 180 }, { 760, 430 }, { 340, 560 }
    };
    bool closed = true;
    bool uniform = false;
    bool paused = false;
    bool dirty = true, outlineDirty = true;
    bool cached = false, limited = false, display = false;
    double zoom = 1.0;
    const Array<double> quality{ 0.25, 0.5, 1.0 };
    size_t qualityIndex = 1;
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

        if (KeyQ.down()) { qualityIndex = (qualityIndex + 1) % quality.size(); outlineDirty = true; }
        if (KeyB.down()) { limited = not limited; outlineDirty = true; }
        if (KeyG.down()) { cached = not cached; }
        if (KeyH.down())
        {
            display = not display;
            Scene::Configure(SceneConfig::Fixed({ 1000, 700 },
                display ? SceneResolution::Display : SceneResolution::SceneSize));
        }
        zoom = Clamp(zoom + (int(KeyUp.pressed()) - int(KeyDown.pressed())) * Scene::DeltaTime(), 0.5, 2.0);
        {
            const Transformer2D transform{ Mat3x2::Scale(zoom, Vec2{ 500, 350 }), TransformCursor::Yes };
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
                outlineDirty = true;
                dirty = false;
            }

            if (outlineDirty)
            {
                path.toLineStringAdaptive(outline, quality[qualityIndex], limited ? 4 : 256);
                outlineDirty = false;
            }
            if (not cached) { path.draw(3, ColorF{ 0.75 }, quality[qualityIndex], limited ? 4 : 256); }
            else if (closed) { outline.drawClosed(3, ColorF{ 0.75 }); }
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

        }

        font(U"Drag knots | U: interpolation | C: close | Space: pause | R: restart")
            .draw(20, 16);
        font(uniform ? U"Uniform" : U"Centripetal").draw(20, 44);
        font(closed ? U"Closed / Wrap" : U"Open / Wrap").draw(20, 72);
        font(U"Q: target={}  B: budget={}  G: {}  Up/Down: zoom={:.2f}  H: render {}"_fmt(
            quality[qualityIndex], limited ? 4 : 256, cached ? U"cached" : U"direct", zoom, Scene::RenderResolution())).draw(20, 100);
    }
}
```
