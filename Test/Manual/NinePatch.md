# NinePatch gallery

This program uses a generated atlas, so it needs no external image files. It
compares a source region, stretch/tile drawing, frames, reflected input, and
animated destinations. See the [guide](../../docs/renderer2d/nine-patch.md).

## Run

1. Copy the complete program into an application's `Main.cpp`, build, and run.
   Do not leave a temporary replacement in the repository's platform Main.cpp.
2. Use the sliders to change border scale and opacity; try border scale 0.
3. Toggle horizontal edges, vertical edges, and center tiling independently.
   Disable all three to compare with stretch drawing. Inspect partial last tiles.
4. Toggle Nearest/Linear and inspect the reflected examples. Pause the animation.
5. Observe the animated width reaching 0 and crossing the minimum border width.
   Move the window between displays with different pixel densities.
6. Set a very small positive border scale. The pattern stops getting finer when
   repetition would exceed 128 tiles along an axis; border placement still follows
   the slider. At exactly zero, only the stretched center remains.

## Expected results

- The top panels keep corner proportions; the frame panel leaves the center empty.
- Enabled regions repeat from their top-left corner and crop the right/bottom
  remainder. Disabled regions stretch. Border scale changes the pattern size too.
- The yellow inner rectangle follows the center patch. It is a layout guide,
  not part of the NinePatch drawing.
- Small destinations shrink all corners together without an abrupt switch to
  the center image. At zero width there is no draw.
- With half opacity, shared patch boundaries have no dark overlap or bright gap.
- The reflected panel keeps the source's visible orientation. The lower bar
  uses three-slice drawing, and the last panel uses zero border scale.
- Nearest preserves the pixel pattern. Linear smooths it but can mix neighboring
  slice colors at repeated boundaries. The atlas has one extruded outer border
  texel and no mipmaps; this does not isolate its internal slices from filtering.

## Program

```cpp
# include <Siv3D.hpp>

Image MakePanelAtlas()
{
    Image atlas{ 96, 80, Palette::Magenta };
    const Color corners[]{ Palette::Skyblue, Palette::Orange, Palette::Pink, Palette::Limegreen };
    for (int32 y = 7; y <= 72; ++y)
    for (int32 x = 15; x <= 80; ++x)
    {
        const int32 px = Clamp(x - 16, 0, 63), py = Clamp(y - 8, 0, 63);
        const bool side = (px < 16 || px >= 48), cap = (py < 16 || py >= 48);
        Color color = ((px / 4 + py / 4) % 2) ? Color{ 48, 64, 80 } : Color{ 56, 72, 88 };
        if (side || cap) color = Color{ 96, 136, 168 };
        if (side && cap) color = corners[(px >= 48) + 2 * (py >= 48)];
        if ((px % 16 == 7) && (py % 16 == 7)) color = Palette::White;
        atlas[y][x] = color;
    }
    return atlas;
}

void Slider(const Font& font, const StringView label, double& value,
    const double maximum, const Vec2 pos)
{
    const RectF track{ pos.x + 90, pos.y + 7, 180, 20 };
    if (track.mouseOver() && MouseL.pressed())
    {
        value = maximum * Clamp((Cursor::PosF().x - track.x) / track.w, 0.0, 1.0);
    }
    font(label).draw(pos);
    track.draw(ColorF{ 0.25 });
    RectF{ track.pos, track.w * value / maximum, track.h }.draw(ColorF{ 0.2, 0.6, 0.8 });
}

void Main()
{
    Window::Resize(1040, 720);
    Scene::SetBackground(ColorF{ 0.12 });
    const Font font{ 18 };
    const Texture atlas{ MakePanelAtlas(), TextureDesc::NoMipmap };
    const TextureRegion source = atlas(16, 8, 64, 64);
    NinePatch panel{ source, 16 };
    NinePatch mirror{ source.mirrored(), 16 };
    NinePatch asymmetric{ source.mirrored(), NinePatch::Slice{ 8, 12, 20, 16 } };
    NinePatch bar{ source, NinePatch::Slice{ .right = 16, .left = 16 } };
    NinePatch center{ source, 16 };
    center.setBorderScale(0);
    double scale = 1, opacity = 0.5, time = 0;
    bool linear = false, paused = false;
    auto tiling = NinePatch::Tiling::All();

    while (System::Update())
    {
        Slider(font, U"Border", scale, 3.0, { 20, 20 });
        Slider(font, U"Opacity", opacity, 1.0, { 360, 20 });
        SimpleGUI::CheckBox(linear, U"Linear", { 680, 20 });
        SimpleGUI::CheckBox(paused, U"Pause", { 840, 20 });
        SimpleGUI::CheckBox(tiling.topBottom, U"Tile horizontal edges", { 20, 60 });
        SimpleGUI::CheckBox(tiling.leftRight, U"Tile vertical edges", { 360, 60 });
        SimpleGUI::CheckBox(tiling.center, U"Tile center", { 700, 60 });
        if (not paused) time += Scene::DeltaTime();
        panel.setBorderScale(scale).setTiling(tiling);
        mirror.setBorderScale(scale).setTiling(tiling);
        asymmetric.setBorderScale(scale).setTiling(tiling);
        bar.setBorderScale(scale).setTiling(tiling);
        center.setTiling(tiling);
        const ScopedRenderStates2D sampler{ linear ? SamplerState::ClampLinear : SamplerState::ClampNearest };
        const ColorF tint{ 1.0, opacity };

        font(U"Source (3x)").draw(24, 110);
        source.scaled(3).draw(24, 150);
        font(U"Panel + innerRect").draw(260, 110);
        const RectF main{ 260, 150, 400, 180 };
        panel.draw(main, tint);
        const RectF inner = panel.innerRect(main);
        if (inner.w > 0 && inner.h > 0) inner.drawFrame(1, Palette::Yellow);
        font(U"Frame only").draw(700, 110);
        panel.drawFrame({ 700, 150, 300, 180 }, tint);

        font(U"Animated width, including zero").draw(260, 370);
        const double width = Max(0.0, 180.0 + 200.0 * Math::Sin(time));
        panel.draw({ 260, 410, width, 100 }, tint);
        font(U"Mirrored, asymmetric slice").draw(700, 370);
        asymmetric.draw({ 700, 410, 300, 100 }, tint);

        font(U"Three-slice").draw(24, 540);
        bar.draw({ 24, 580, 340, 64 }, tint);
        font(U"Reflected frame").draw(400, 540);
        mirror.drawFrame({ 400, 580, 270, 64 }, tint);
        font(U"Center only (scale 0)").draw(700, 540);
        center.draw({ 700, 580, 300, 64 }, tint);
    }
}
```
