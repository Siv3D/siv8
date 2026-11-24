# Rotated ellipses, arc caps and Bezier gradients

This gallery shows direct rotated ellipse drawing, normal-width ellipse arcs,
independent Circle caps and start/end gradients on adaptive Bezier strokes.
It writes no files and does not replace the repository test application's Main.

## Run

1. Build a separate Siv3D application with the complete program below.
2. Press **Space** to reverse the arc sweeps, **H** to switch scene-size/display
   render resolution, and **R** to pause/resume the ellipse rotation.
3. Inspect the flat ellipse tips, the two different arc ends, and the gradient
   continuity between strokes and caps. Record the platform, resolution mode
   and any visible artifacts.

## Expected results

- The upper-left Bezier strokes transition from blue to orange, or fade to
  transparent, along the drawn length. Their different cap combinations remain
  visible at the endpoints.
- The upper-right Circle has a Flat start and a Round end. Reversing the sweep
  keeps these styles attached to the specified start and end. The gradient's
  blue end remains at the start.
- The lower-left ellipse fill and frame rotate around their centers without
  changing the geometry object. The vertical gradient rotates with the fill.
- The lower-right ellipse arcs use a constant normal-direction width. Their
  Round starts and Square ends follow the local tangent. The flat ellipse's tip
  should remain rounded on its outward side. The sample uses only an outward
  offset there to avoid folded inward offsets at high curvature.
- Changing render resolution keeps the logical sizes and positions while
  adjusting subdivision density. Display mode can look identical on a 1x display.

```cpp
# include <Siv3D.hpp>

void Main()
{
    Window::Resize(960, 640);
    Scene::Configure(SceneConfig::Fixed({ 960, 640 }, SceneResolution::SceneSize));
    Scene::SetBackground(ColorF{ 0.08, 0.10, 0.14 });
    const Font font{ 18 };
    const ColorF blue{ 0.15, 0.65, 1.0 };
    const ColorF orange{ 1.0, 0.45, 0.1 };
    bool reverse = false, highResolution = false, animate = true;
    double rotation = 0.0;

    while (System::Update())
    {
        if (KeySpace.down()) reverse = !reverse;
        if (KeyR.down()) animate = !animate;
        if (KeyH.down())
        {
            highResolution = !highResolution;
            Scene::Configure(SceneConfig::Fixed({ 960, 640 },
                highResolution ? SceneResolution::Display : SceneResolution::SceneSize));
        }
        if (animate) rotation += Scene::DeltaTime() * 0.4;
        const double sweep = (reverse ? -240_deg : 240_deg);

        font(U"Bezier gradients").draw(24, 20);
        Bezier2{ { 40, 220 }, { 160, 40 }, { 420, 180 } }.draw(
            LineCap::Flat, LineCap::Round, 18, Arg::start = blue, Arg::end = orange);
        Bezier3{ { 40, 250 }, { 170, 150 }, { 280, 330 }, { 420, 220 } }.draw(
            LineCap::Round, 12, Arg::start = blue, Arg::end = ColorF{ 1.0, 0.0 });

        font(U"Circle: Flat / Round").draw(500, 20);
        Circle{ 720, 180, 85 }.drawFrame(1, ColorF{ 0.3 });
        Circle{ 720, 180, 85 }.drawArc(LineCap::Flat, LineCap::Round,
            -60_deg, sweep, 10, 10, Arg::start = blue, Arg::end = orange);

        font(U"Rotated fill and frame").draw(24, 340);
        Ellipse{ 240, 470, 140, 55 }.drawRotated(rotation,
            Arg::top = blue, Arg::bottom = orange);
        Ellipse{ 240, 470, 155, 70 }.drawFrameRotated(rotation, 4, Palette::White);

        font(U"Ellipse: Round / Square").draw(500, 340);
        Ellipse{ 720, 440, 145, 50 }.drawArc(LineCap::Round, LineCap::Square,
            -60_deg, sweep, 4, 10, Arg::start = blue, Arg::end = orange);
        Ellipse{ 720, 550, 145, 6 }.drawArc(LineCap::Round, LineCap::Square,
            -60_deg, sweep, 0, 8, blue, orange);

        font(U"Space: reverse  |  H: render resolution  |  R: rotation").draw(24, 610);
    }
}
```
