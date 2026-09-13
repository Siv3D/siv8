# Truchet

A 1280 x 780 gallery of connected quarter-circle tiles. Runs on Windows (D3D11)
and macOS (Metal); see the [design notes](../../docs/renderer2d/truchet.md).

## Execution

1. Paste the complete program into a separate Siv3D application built with this
   revision. Keep the repository's platform test entry point intact.
2. Run with the standard engine assets. No extra image files are needed.
   On Windows, include the standard `App/Resource.rc` and place the executable
   beside the `engine/` assets. [D3D11 verification](../../docs/renderer2d/d3d11-new-patterns.md#verification)
   also describes separate sample builds.
3. Space toggles tiling motion; S changes the random seeds; R restores the initial
   state. Seed changes should not change either regular layout.
4. Down/Up adjust object scale (0.3 to 1). Left/Right rotate; zoom out first to
   leave room between rotated cards. N toggles nonuniform scale.
5. Optional `--capture` saves `truchet.png` in the application's working directory
   and exits. `--animate --capture` captures after 90 frames. Remove these optional
   retained images when no longer needed.

## Expected results

- Nine cards include Uniform (Porcelain/Jade), Alternating (Loops/Pearls), Random
  (Midnight/Citrus/Labyrinth), and zero/full-width endpoints. Names are the only text.
- Uniform creates repeated flowing paths. Alternating forms closed rings. Random
  creates connected meanders and loops, not disconnected arc fragments.
- At each edge midpoint the arcs meet with matching tangents, regardless of the
  neighboring orientation. Negative coordinates obey the same placement rules.
- Changing the seed rearranges only random layouts. Restarting with the same seed
  restores the same arrangement, including seeds with high bits set.
- Animation translates the tiling and its arrangement together, without reshuffling
  tiles. Pausing holds the phase; resuming continues it. Empty/Solid remain uniform.
- Object transforms carry the pattern with the card. Nonuniform scale deforms
  circular arcs. Wide arcs can merge; very small tiles can lose detail or alias.

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
		Pattern::Truchet paint;
	};

	using Layout = Pattern::Truchet::Layout;
	const std::array<Preset, 9> Presets{{
		{ U"Porcelain", ColorF{ 0.16, 0.30, 0.46 },
			{ ColorF{ 0.62, 0.77, 0.90 }, ColorF{ 0.96, 0.98, 1 }, 40, 3, 0_deg, {}, Layout::Uniform } },
		{ U"Midnight", ColorF{ 0.9, 0.95, 1 },
			{ ColorF{ 0.22, 0.36, 0.55 }, ColorF{ 0.045, 0.07, 0.12 }, 42, 10, 15_deg, {}, Layout::Random, 1 } },
		{ U"Citrus", ColorF{ 0.19, 0.28, 0.07 },
			{ ColorF{ 0.69, 0.84, 0.20 }, ColorF{ 0.97, 0.98, 0.74 }, 48, 16, 0_deg, {}, Layout::Random, 17 } },
		{ U"Loops", ColorF{ 0.47, 0.20, 0.27 },
			{ ColorF{ 0.87, 0.58, 0.67 }, ColorF{ 1, 0.91, 0.91 }, 34, 7, 0_deg, {}, Layout::Alternating } },
		{ U"Labyrinth", ColorF{ 0.88, 0.93, 0.98 },
			{ ColorF{ 0.25, 0.32, 0.40 }, ColorF{ 0.06, 0.085, 0.12 }, 24, 6, 45_deg, {}, Layout::Random, 0x7FC00001u } },
		{ U"Jade", ColorF{ 0.11, 0.33, 0.32 },
			{ ColorF{ 0.35, 0.66, 0.59 }, ColorF{ 0.88, 0.96, 0.89 }, 54, 13, -25_deg, {}, Layout::Uniform } },
		{ U"Pearls", ColorF{ 0.34, 0.23, 0.45 },
			{ ColorF{ 0.72, 0.59, 0.84 }, ColorF{ 0.96, 0.92, 0.98 }, 30, 12, 45_deg, {}, Layout::Alternating } },
		{ U"Empty", ColorF{ 0.26, 0.35, 0.36 },
			{ ColorF{ 0.10, 0.65, 0.5 }, ColorF{ 0.84, 0.92, 0.90 }, 32, 0 } },
		{ U"Solid", ColorF{ 0.92, 0.92, 1 },
			{ ColorF{ 0.29, 0.26, 0.48 }, ColorF{ 0.95 }, 32, 32 } },
	}};
}

void Main()
{
	Window::Resize(1280, 780);
	Window::SetTitle(U"Truchet");
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
	uint32 seedOffset = 0;

	while (System::Update())
	{
		if (capture && ScreenCapture::HasNewFrame())
		{
			ScreenCapture::GetFrame().save(U"truchet.png");
			return;
		}
		if (KeySpace.down()) { animate = !animate; }
		if (KeyN.down()) { nonuniform = !nonuniform; }
		if (KeyS.down()) { ++seedOffset; }
		if (KeyR.down()) { animate = nonuniform = false; time = angle = 0; zoom = 1; seedOffset = 0; }
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
			paint.seed += seedOffset;
			paint.origin += Vec2{ 28 * std::sin(time * 0.5), 18 * std::sin(time * 0.3) };
			shape.draw(paint);
			shape.drawFrame(0, 1.5, ColorF{ 0.85, 0.92, 1, 0.7 });
			label(preset.name).draw(24, 144, preset.text);
		}
		if (capture && (++frames == (animate ? 90 : 3))) { ScreenCapture::RequestCurrentFrame(); }
	}
}
```
