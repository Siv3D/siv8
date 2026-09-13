# Weave

A 1280 x 780 gallery of plain-weave backgrounds, from fine cloth to broad basket
strips. Runs on Windows (D3D11) and macOS (Metal); see the
[design notes](../../docs/renderer2d/weave.md).

## Execution

1. Paste the complete program into a separate Siv3D application built with this
   revision. Keep the repository's platform test entry point intact.
2. Run with the standard engine assets. No extra images are needed.
   On Windows, include the standard `App/Resource.rc` and place the executable
   beside the `engine/` assets. [D3D11 verification](../../docs/renderer2d/d3d11-new-patterns.md#verification)
   also describes separate sample builds.
3. Space toggles motion of the weave origin; R restores the initial state.
4. Down/Up adjust object scale (0.3 to 1). Left/Right rotate; zoom out first to
   leave space between rotated cards. N toggles nonuniform scale.
5. Optional `--capture` saves `weave.png` in the application's working directory
   and exits. `--animate --capture` captures after 90 frames. Remove these optional
   retained images when no longer needed.

## Expected results

- Nine framed cards show fine, broad, rotated, zero-gap, zero-width, and full-width
  examples. Names are the only on-screen text.
- At the origin, the vertical strip crosses above the horizontal one. The order
  alternates at each adjacent crossing, including in negative coordinates.
- Side gaps reveal the background and distinguish which strip passes underneath.
  Basket uses the maximum gap, so its cuts extend to the half-cell boundary.
- Grid has zero gap and no cuts. Empty and Solid remain uniform during animation.
- Animation moves the weave without moving the card. Pausing holds the phase;
  resuming continues it. Object transforms carry both geometry and weave together.
- Nonuniform scale stretches the strips and gaps. Thin strips or strong
  minification can lose contrast or alias; the filter is a local approximation.

## Complete Main.cpp

```cpp
# include <Siv3D.hpp>
# include <array>
# include <cmath>

namespace
{
	struct Preset
	{
		StringView name;
		ColorF text;
		Pattern::Weave paint;
	};

	const std::array<Preset, 9> Presets{{
		{ U"Linen", ColorF{ 0.35, 0.29, 0.22 },
			{ ColorF{ 0.77, 0.70, 0.59 }, ColorF{ 0.96, 0.94, 0.88 }, 20, 7, 2 } },
		{ U"Cobalt", ColorF{ 0.89, 0.95, 1 },
			{ ColorF{ 0.23, 0.39, 0.62 }, ColorF{ 0.045, 0.09, 0.17 }, 30, 16, 3, 15_deg } },
		{ U"Basket", ColorF{ 0.39, 0.24, 0.09 },
			{ ColorF{ 0.79, 0.55, 0.26 }, ColorF{ 0.99, 0.90, 0.72 }, 36, 20, 8 } },
		{ U"Rose", ColorF{ 0.47, 0.20, 0.27 },
			{ ColorF{ 0.87, 0.62, 0.68 }, ColorF{ 1, 0.91, 0.91 }, 28, 14, 3, 45_deg } },
		{ U"Carbon", ColorF{ 0.88, 0.93, 0.98 },
			{ ColorF{ 0.22, 0.27, 0.34 }, ColorF{ 0.075, 0.095, 0.13 }, 20, 12, 2, 45_deg } },
		{ U"Oblique", ColorF{ 0.11, 0.33, 0.32 },
			{ ColorF{ 0.35, 0.66, 0.59 }, ColorF{ 0.88, 0.96, 0.89 }, 34, 18, 4, -25_deg } },
		{ U"Grid", ColorF{ 0.34, 0.23, 0.45 },
			{ ColorF{ 0.72, 0.59, 0.84 }, ColorF{ 0.96, 0.92, 0.98 }, 24, 6, 0, 12_deg } },
		{ U"Empty", ColorF{ 0.26, 0.35, 0.36 },
			{ ColorF{ 0.10, 0.65, 0.5 }, ColorF{ 0.84, 0.92, 0.90 }, 24, 0, 6 } },
		{ U"Solid", ColorF{ 0.92, 0.92, 1 },
			{ ColorF{ 0.29, 0.26, 0.48 }, ColorF{ 0.95 }, 24, 24, 0 } },
	}};
}

void Main()
{
	Window::Resize(1280, 780);
	Window::SetTitle(U"Weave");
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(1280, 780);
	Scene::SetBackground(ColorF{ 0.075, 0.085, 0.105 });
	const Font label{ 28, Typeface::Bold };
	const auto shape = RectF{ 0, 0, 380, 200 }.rounded(20);
	const bool capture = System::GetCommandLineArgs().contains(U"--capture");
	bool animate = System::GetCommandLineArgs().contains(U"--animate");
	bool nonuniform = false;
	double time = 0, zoom = 1, angle = 0;
	int32 frames = 0;

	while (System::Update())
	{
		if (capture && ScreenCapture::HasNewFrame())
		{
			ScreenCapture::GetFrame().save(U"weave.png");
			return;
		}
		if (KeySpace.down()) { animate = !animate; }
		if (KeyN.down()) { nonuniform = !nonuniform; }
		if (KeyR.down()) { animate = nonuniform = false; time = angle = 0; zoom = 1; }
		if (animate) { time += Scene::DeltaTime(); }
		if (KeyUp.pressed()) { zoom = Min(zoom + Scene::DeltaTime() * 0.3, 1.0); }
		if (KeyDown.pressed()) { zoom = Max(zoom - Scene::DeltaTime() * 0.3, 0.3); }
		if (KeyLeft.pressed()) { angle -= Scene::DeltaTime() * 15_deg; }
		if (KeyRight.pressed()) { angle += Scene::DeltaTime() * 15_deg; }

		for (size_t i = 0; i < Presets.size(); ++i)
		{
			const auto& preset = Presets[i];
			const Vec2 position{ 40 + (i % 3) * 410, 30 + (i / 3) * 245 };
			const Transformer2D transform{ Mat3x2::Translate(-190, -100)
				.scaled(zoom, zoom * (nonuniform ? 0.6 : 1.0)).rotated(angle)
				.translated(position + Vec2{ 190, 100 }) };
			auto paint = preset.paint;
			paint.origin += Vec2{ 28 * std::sin(time * 0.5), 18 * std::sin(time * 0.3) };
			shape.draw(paint);
			shape.drawFrame(0, 1.5, ColorF{ 0.85, 0.92, 1, 0.7 });
			label(preset.name).draw(24, 144, preset.text);
		}
		if (capture && (++frames == (animate ? 90 : 3))) { ScreenCapture::RequestCurrentFrame(); }
	}
}
```
