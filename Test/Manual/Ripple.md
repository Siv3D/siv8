# Ripple

A 1280 x 780 gallery of concentric-circle backgrounds. Runs on Windows (D3D11)
and macOS (Metal); see the [design notes](../../docs/renderer2d/ripple.md).
This focused sample is separate from the planned gallery of all Pattern types.

## Execution

1. Paste the complete program into a separate Siv3D application built with this
   revision. Keep the repository's platform test entry point intact.
2. Run with the standard engine assets; no extra images are needed.
   On Windows, include the standard `App/Resource.rc` and place the executable
   beside the `engine/` assets. [D3D11 verification](../../docs/renderer2d/d3d11-new-patterns.md#verification)
   also describes separate sample builds.
3. Space toggles outward radial motion; R restores the initial state.
4. Down/Up change object scale (0.3 to 1). Left/Right rotate the objects;
   zoom out first to leave space between rotated cards. N toggles nonuniform scale.
5. Optional `--capture` saves `ripple.png` in the application's working directory
   and exits. `--animate --capture` captures after 90 frames. Remove these optional
   retained images when no longer needed.

## Expected results

- Nine cards show distant arcs, visible centers, broad bands, thin rings, two
  layered combinations, and zero/full-thickness endpoints. Only names appear as text.
- Zero offset includes a filled central disk. Eclipse starts half a pitch out,
  leaving the center empty. Off-card centers create broad flowing arcs.
- Animation expands the rings without moving their centers. Offsets wrap every
  pitch without a visible jump. Pausing holds the phase; resuming continues it.
- Geometry and pattern scale/rotate together. Nonuniform scale produces ellipses.
- Empty and Solid remain uniform throughout animation. Orbit uses two phases
  at the same center; Interference uses two different centers and transparent overlays.
- Edges remain filtered at the center. Very thin or strongly minified rings can
  lose contrast or alias; the filter does not integrate many rings over a pixel.

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
		Pattern::Ripple paint;
		Optional<Pattern::Ripple> overlay = none;
	};

	const std::array<Preset, 9> Presets{{
		{ U"Porcelain", ColorF{ 0.16, 0.30, 0.46 },
			{ ColorF{ 0.62, 0.77, 0.90 }, ColorF{ 0.96, 0.98, 1 }, 16, 2, { 430, 320 } } },
		{ U"Eclipse", ColorF{ 0.9, 0.95, 1 },
			{ ColorF{ 0.20, 0.32, 0.49 }, ColorF{ 0.045, 0.07, 0.12 }, 28, 9, { 280, 55 }, 14 } },
		{ U"Sunrise", ColorF{ 0.30, 0.25, 0.08 },
			{ ColorF{ 0.96, 0.69, 0.20 }, ColorF{ 1, 0.95, 0.74 }, 36, 16, { 190, 240 } } },
		{ U"Coral", ColorF{ 0.47, 0.16, 0.18 },
			{ ColorF{ 0.98, 0.50, 0.44 }, ColorF{ 1, 0.88, 0.80 }, 42, 18, { -80, -90 } } },
		{ U"Orbit", ColorF{ 0.34, 0.22, 0.44 },
			{ ColorF{ 0.73, 0.59, 0.84 }, ColorF{ 0.96, 0.92, 0.98 }, 32, 7, { 265, 68 } },
			Pattern::Ripple{ ColorF{ 0.48, 0.32, 0.62, 0.7 }, ColorF{ 0, 0 }, 32, 2, { 265, 68 }, 9 } },
		{ U"Sonar", ColorF{ 0.80, 1, 0.92 },
			{ ColorF{ 0.13, 0.60, 0.42 }, ColorF{ 0.025, 0.13, 0.12 }, 18, 2, { 275, 65 } } },
		{ U"Interference", ColorF{ 0.86, 0.95, 1 },
			{ ColorF{ 0.18, 0.32, 0.49 }, ColorF{ 0.055, 0.10, 0.18 }, 22, 5, { 55, 60 } },
			Pattern::Ripple{ ColorF{ 0.40, 0.69, 0.79, 0.5 }, ColorF{ 0, 0 }, 22, 3, { 315, 60 } } },
		{ U"Empty", ColorF{ 0.26, 0.35, 0.36 },
			{ ColorF{ 0.10, 0.65, 0.5 }, ColorF{ 0.84, 0.92, 0.90 }, 24, 0, { 190, 100 } } },
		{ U"Solid", ColorF{ 0.92, 0.92, 1 },
			{ ColorF{ 0.29, 0.26, 0.48 }, ColorF{ 0.95 }, 24, 24, { 190, 100 } } },
	}};
}

void Main()
{
	Window::Resize(1280, 780);
	Window::SetTitle(U"Ripple");
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
			ScreenCapture::GetFrame().save(U"ripple.png");
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
			paint.radiusOffset += std::fmod(time * 12.0, paint.pitch);
			shape.draw(paint);
			if (preset.overlay)
			{
				auto overlay = *preset.overlay;
				overlay.radiusOffset += std::fmod(time * 12.0, overlay.pitch);
				shape.draw(overlay);
			}
			shape.drawFrame(0, 1.5, ColorF{ 0.85, 0.92, 1, 0.7 });
			label(preset.name).draw(24, 144, preset.text);
		}
		if (capture && (++frames == (animate ? 90 : 3))) { ScreenCapture::RequestCurrentFrame(); }
	}
}
```
