# Wave

A gallery of sine-wave backgrounds, using drawing-unit spacing, thickness,
amplitude, wavelength, and origin. Runs on Windows (D3D11) and macOS (Metal);
see the [design notes](../../docs/renderer2d/wave.md).

## Execution

1. Paste the complete program into a separate Siv3D application built with this
   revision. Keep the repository's normal platform test entry point intact.
2. Run with the standard engine assets. No additional images are required.
   On Windows, include the standard `App/Resource.rc` and place the executable
   beside the `engine/` assets. [D3D11 verification](../../docs/renderer2d/d3d11-new-patterns.md#verification)
   also describes separate sample builds.
3. Space toggles motion of the pattern origin; R restores the initial state.
4. Up/Down adjust object scale (0.3 to 1). Left/Right adjust object rotation.
   N toggles nonuniform scale. Geometry and pattern should move together.
5. Optional `--capture` saves `wave.png` in the working directory and exits.
   `--animate --capture` records a moving phase after 90 frames. Remove these
   optional retained images when no longer needed.

## Expected results

- Twelve framed cards show fine, broad, rotated, layered, straight, empty, and
  full-coverage variants. Names are the only on-screen text.
- The pattern origin moves while the card geometry remains still during animation.
  Pausing keeps the current phase; resuming continues it.
- Object scale changes wave size together with each card. Nonuniform scale
  deliberately deforms both. The pattern does not remain fixed to the screen.
- Straight has amplitude zero. Empty shows only its background; Solid shows only
  its primary color. The two overlay cards retain transparency between strokes.
- Slope compensation keeps ordinary wave strokes visually similar in thickness.
  Sharp curves and wide strokes are approximate, and neighboring bands can merge.
  Strong minification can alias; this is not a many-period area filter.

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
		Pattern::Wave paint;
		Optional<Pattern::Wave> overlay = none;
	};

	const std::array<Preset, 12> Presets{{
		{ U"Porcelain", ColorF{ 0.16, 0.30, 0.46 },
			{ ColorF{ 0.65, 0.79, 0.90 }, ColorF{ 0.96, 0.98, 1 }, 18, 2, 4, 110 } },
		{ U"Midnight", ColorF{ 0.9, 0.95, 1 },
			{ ColorF{ 0.16, 0.27, 0.42 }, ColorF{ 0.045, 0.07, 0.12 }, 27, 8, 9, 100, -20_deg } },
		{ U"Citrus", ColorF{ 0.15, 0.23, 0.08 },
			{ ColorF{ 0.67, 0.84, 0.19 }, ColorF{ 0.95, 0.98, 0.72 }, 38, 15, 14, 100, -15_deg } },
		{ U"Lilac", ColorF{ 0.32, 0.21, 0.46 },
			{ ColorF{ 0.72, 0.59, 0.88 }, ColorF{ 0.96, 0.92, 1 }, 22, 5, 7, 70, 30_deg } },
		{ U"Coral", ColorF{ 0.47, 0.16, 0.18 },
			{ ColorF{ 0.98, 0.50, 0.44 }, ColorF{ 1, 0.88, 0.80 }, 42, 18, 12, 125, 15_deg } },
		{ U"Lagoon", ColorF{ 0.85, 1, 0.97 },
			{ ColorF{ 0.12, 0.58, 0.52 }, ColorF{ 0.02, 0.22, 0.25 }, 30, 8, 10, 90, -35_deg } },
		{ U"Gold Thread", ColorF{ 1, 0.90, 0.62 },
			{ ColorF{ 0.60, 0.44, 0.20 }, ColorF{ 0.11, 0.095, 0.09 }, 14, 1.5, 3, 110, 12_deg } },
		{ U"Cross Current", ColorF{ 0.88, 0.95, 1 },
			{ ColorF{ 0.18, 0.32, 0.49 }, ColorF{ 0.055, 0.10, 0.18 }, 32, 7, 7, 100, 20_deg },
			Pattern::Wave{ ColorF{ 0.40, 0.69, 0.79, 0.5 }, ColorF{ 0, 0 }, 32, 3, 7, 100, -20_deg } },
		{ U"Echo", ColorF{ 0.34, 0.22, 0.44 },
			{ ColorF{ 0.75, 0.60, 0.81 }, ColorF{ 0.96, 0.92, 0.97 }, 36, 8, 9, 100 },
			Pattern::Wave{ ColorF{ 0.48, 0.32, 0.62, 0.65 }, ColorF{ 0, 0 }, 36, 2, 9, 100, 0_deg, { 0, 9 } } },
		{ U"Straight", ColorF{ 0.89, 0.94, 1 },
			{ ColorF{ 0.22, 0.33, 0.49 }, ColorF{ 0.10, 0.16, 0.27 }, 22, 5, 0, 80, -25_deg } },
		{ U"Empty", ColorF{ 0.26, 0.35, 0.36 },
			{ ColorF{ 0.10, 0.65, 0.5 }, ColorF{ 0.84, 0.92, 0.90 }, 24, 0, 6, 80 } },
		{ U"Solid", ColorF{ 0.92, 0.92, 1 },
			{ ColorF{ 0.29, 0.26, 0.48 }, ColorF{ 0.95 }, 24, 24, 6, 80 } },
	}};
}

void Main()
{
	Window::Resize(1680, 900);
	Window::SetTitle(U"Wave");
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(1680, 900);
	Scene::SetBackground(ColorF{ 0.075, 0.085, 0.105 });
	const Font label{ 28, Typeface::Bold };
	const auto shape = RectF{ 0, 0, 380, 230 }.rounded(20);
	const bool capture = System::GetCommandLineArgs().contains(U"--capture");
	bool animate = System::GetCommandLineArgs().contains(U"--animate");
	bool nonuniform = false;
	double time = 0, zoom = 1, angle = 0;
	int32 frames = 0;

	while (System::Update())
	{
		if (capture && ScreenCapture::HasNewFrame())
		{
			ScreenCapture::GetFrame().save(U"wave.png");
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
		const Vec2 displacement{ 30 * std::sin(time * 0.6), 12 * std::sin(time * 0.4) };

		for (size_t i = 0; i < Presets.size(); ++i)
		{
			const auto& preset = Presets[i];
			const Vec2 position{ 47 + (i % 4) * 402, 40 + (i / 4) * 285 };
			const Transformer2D transform{ Mat3x2::Translate(-190, -115)
				.scaled(zoom, zoom * (nonuniform ? 0.6 : 1.0)).rotated(angle)
				.translated(position + Vec2{ 190, 115 }) };
			auto paint = preset.paint;
			paint.origin += displacement;
			shape.draw(paint);
			if (preset.overlay)
			{
				auto overlay = *preset.overlay;
				overlay.origin += displacement;
				shape.draw(overlay);
			}
			shape.drawFrame(0, 1.5, ColorF{ 0.85, 0.92, 1, 0.7 });
			label(preset.name).draw(24, 168, preset.text);
		}
		if (capture && (++frames == (animate ? 90 : 3))) { ScreenCapture::RequestCurrentFrame(); }
	}
}
```
