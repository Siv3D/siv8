# Trimmed circle arcs

This gallery compares angle-based gaps with parallel separators on a textured
background, then exercises a single trimmed sector. It writes no files.

## Run

1. Build a separate Siv3D application with the complete program below.
2. Hold **Left/Right** to adjust trim, **Down/Up** for the inner radius, and
   **Q/E** for the sweep. **Z** sets zero sweep and **F** sets a full turn.
   Press **Space** to reverse the sweeps, **A** to animate the trim, and **H**
   to switch render resolution.
3. Compare the upper menus, then inspect the lower sector near a disappearing
   inner arc, zero sweep, half a turn, and a full turn.

## Expected results

- The left menu's gaps widen toward the outer edge. The right menu has parallel
  straight sides separated by twice the trim value. Background checks remain
  visible through the gaps and through the half-transparent fill.
- The right menu retains the same inner and outer radii while trim changes.
  With a small inner radius, each item converges to a tip. Large trims can remove
  narrow items completely without inverted triangles or stray pixels.
- The lower sector has no darker or brighter seam at its bisector. Reversing
  the sweep changes its direction, with the same geometry reflected about the
  starting ray.
- Increasing the sweep keeps one connected sector, with no extra cut on the
  opposite side of the circle. When trim reaches the inner radius, sectors
  wider than half a turn join their straight sides with a rounded inner arc.
  Exactly one full turn ignores trim and draws the complete ring or disk.
- A zero trim matches an ordinary flat arc; a zero sweep draws nothing.
- Switching render resolution changes drawing detail, not the logical sizes.

```cpp
# include <Siv3D.hpp>

void Main()
{
    Window::Resize(960, 760);
    Scene::Configure(SceneConfig::Fixed({ 960, 760 }, SceneResolution::SceneSize));
    const Font font{ 18 };
    double trim = 6, innerRadius = 64, sweep = 240;
    bool reverse = false, animate = false, highResolution = false;
    double time = 0;

    while (System::Update())
    {
        if (KeySpace.down()) reverse = !reverse;
        if (KeyA.down()) animate = !animate;
        if (KeyH.down())
        {
            highResolution = !highResolution;
            Scene::Configure(SceneConfig::Fixed({ 960, 760 },
                highResolution ? SceneResolution::Display : SceneResolution::SceneSize));
        }
        const double dt = Scene::DeltaTime();
        time += dt;
        trim = Clamp(trim + (int32(KeyRight.pressed()) - int32(KeyLeft.pressed())) * 30 * dt, 0.0, 90.0);
        innerRadius = Clamp(innerRadius + (int32(KeyUp.pressed()) - int32(KeyDown.pressed())) * 60 * dt, 0.0, 150.0);
        sweep = Clamp(sweep + (int32(KeyE.pressed()) - int32(KeyQ.pressed())) * 60 * dt, 0.0, 360.0);
        if (KeyZ.down()) sweep = 0;
        if (KeyF.down()) sweep = 360;
        for (int32 y = 0; y < 760; y += 24)
            for (int32 x = 0; x < 960; x += 24)
                Rect{ x, y, 24, 24 }.draw(ColorF{ ((x / 24 + y / 24) % 2) ? 0.16 : 0.22 });

        const double amount = (animate ? 42 * (1 + std::sin(time)) : trim);
        const double sign = (reverse ? -1.0 : 1.0);
        const double angularGap = (2 * std::asin(Min(amount / 160, 1.0)));
        for (int32 i = 0; i < 6; ++i)
        {
            const ColorF color = ColorF{ HSV{ i * 60.0 }, 0.5 };
            Circle{ 240, 220, 160 }.drawArc(sign * (i * 60_deg + angularGap / 2),
                sign * Max(60_deg - angularGap, 0.0), 160 - innerRadius, 0, color);
            Circle{ 720, 220, 160 }.drawTrimmedArc(sign * i * 60_deg,
                sign * 60_deg, 160 - innerRadius, 0, amount, color);
        }
        font(U"Angular gaps").draw(32, 24);
        font(U"Parallel sides / trim = {:.1f}"_fmt(amount)).draw(512, 24);

        Circle{ 240, 570, 130 }.drawTrimmedArc(0, sign * Math::ToRadians(sweep),
            130 - Min(innerRadius, 130.0), 0, amount, ColorF{ 0.3, 0.8, 1.0, 0.5 });
        font(U"Single sector").draw(32, 400);
        font(U"Left / Right: trim = {:.1f}"_fmt(trim)).draw(500, 440);
        font(U"Down / Up: inner radius = {:.1f}"_fmt(innerRadius)).draw(500, 490);
        font(U"Q / E: sweep = {:.1f} deg"_fmt(sweep)).draw(500, 540);
        font(U"Z: zero sweep | F: full turn").draw(500, 590);
        font(U"Space: reverse | A: animate | H: resolution").draw(32, 724);
    }
}
```
