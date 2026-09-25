# ScopedQuadWarp2D visual check

This program applies one projective transform to a texture, patterned geometry,
curves, and MSDF text. It also shows a nested transform and an unwarped overlay.
See the [renderer guide](../../docs/renderer2d/quad-warp.md) for the shader interface.

## Steps and expected results

Run the complete program below in the platform sample application. Move the mouse
to adjust the upper-right target corner. Convex targets render the whole panel;
invalid targets skip it. The texture grid and shapes should remain aligned through
the perspective change, with no diagonal texture seam. Curves can show their chosen
tessellation at large magnifications. The overlay stays in screen coordinates.
The fixed scissor rectangle clips the warped panel in screen coordinates.

```cpp
# include <Siv3D.hpp>

void Main()
{
    Scene::SetBackground(ColorF{ 0.08 });
    const Font font{ FontMethod::MSDF, 32 };
    Image pixels{ 128, 128, Palette::White };
    for (int32 y = 0; y < 128; ++y)
    {
        for (int32 x = 0; x < 128; ++x)
        {
            pixels[y][x] = (((x / 16) + (y / 16)) % 2)
                ? Palette::Orange : Palette::Skyblue;
        }
    }
    const Texture texture{ pixels };
    const RectF source{ 0, 0, 400, 280 };

    while (System::Update())
    {
        const Quad target{ Vec2{ 80, 100 }, Cursor::PosF(), Vec2{ 620, 490 }, Vec2{ 120, 440 } };
        if (const auto h = Mat3x3::TryHomography(source, target))
        {
            const ScopedScissorRect2D scissor{ Rect{ 40, 80, 680, 440 } };
            const ScopedQuadWarp2D warp{ *h };
            source.draw(ColorF{ 0.18 });
            texture.draw(20, 20);
            Circle{ 270, 85, 55 }.draw(Palette::Orange);
            RectF{ 180, 155, 180, 55 }.draw(Pattern::Checker{});
            font(U"Projective 2D").draw(20, 225);
            {
                const ScopedQuadWarp2D inner{ Mat3x3{ Mat3x2::Translate(160, 0) } };
                RectF{ 0, 0, 25, 25 }.draw(Palette::Lime);
            }
            {
                const ScopedQuadWarp2D overlay{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
                RectF{ 50, 90, 20, 20 }.draw(Palette::Red);
            }
        }
        font(U"Move the mouse to warp the panel").draw(20, 20);
    }
}
```
