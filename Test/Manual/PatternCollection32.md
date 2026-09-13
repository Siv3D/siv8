# Pattern collection

A 1680 x 1050 gallery of 32 button backgrounds. Only the patterns, rounded
frames, and names appear on screen. The first eight preserve the Halftone
button recipes; sixteen more cover PolkaDot, Stripe, Grid, Checker, Triangle,
and HexGrid. The final eight explore layered combinations of those patterns.

## Execution

1. Paste the complete code below into a separate Siv3D application built with
   this revision. The repository's platform Main.cpp remains the normal
   development/test entry point.
2. Run on Metal or D3D11. The scene stays at 1680 x 1050 and requires no external
   image assets. Animation starts paused.
3. Press Space to toggle animation. Pausing holds the current phase; resuming
   continues from that phase. Press R to pause and restore the original layout.
   These controls add no on-screen text.
4. Optional command-line arguments:
   - `--animate` starts animation immediately.
   - `--capture` saves `pattern-collection.png` in the working directory and exits.
   - `--animate --capture` captures after 90 frames to show a moved lattice.
   Delete these optional retained images when no longer needed.

## Expected results

- Four columns and eight rows fit within the scene without clipping.
- The top two rows retain Glacier, Carbon, Acid, Violet, Coral, Blueprint, Prism,
  and Champagne. Their original 420 x 104 paint coordinates and parameters are
  preserved; each card is uniformly scaled by 0.9 for layout.
- Subsequent rows show three PolkaDot, three Stripe, three Grid, three Checker,
  two Triangle, and two HexGrid recipes. Graphite combines fine and coarse grids.
- The final two rows add Orbit (nested dots forming rings), Pearl Circuit
  (dots at grid intersections), Tartan (crossing broad stripes), Interference
  (two slightly different stripe frequencies/angles), Ember Mesh (HexGrid plus
  Halftone), Kaleidoscope (offset triangle layers), Solar Static (Halftone plus
  fine stripes), and Petal Cut (background-colored stripes covering half-dots).
- Prism, Champagne, Graphite, and the last eight recipes use a transparent second
  pattern layer. Orbit and Petal Cut paint the base background color over parts
  of the first layer; they do not punch holes through the composed background.
- Interference intentionally produces larger bands from overlapping fine lines;
  its appearance can change with framebuffer scale.
- Animation moves each lattice along a slow bounded path, while cards, frames,
  and names stay fixed. Layers use the same displacement so Orbit, Pearl Circuit,
  and Petal Cut retain their aligned constructions. Halftone's linear radius
  field stays fixed in card coordinates while dot centers move through it.
- All presets remain immutable; each frame derives its paint from the originals
  rather than accumulating changes to their matrices or field coefficients.
- Labels, borders, and a restrained shadow accompany each pattern. No heading,
  explanatory caption, arrow, selection indicator, or control hint is displayed.

The [editable Pattern gallery](PatternGallery.md) provides parameter controls.
The [Halftone button gallery](HalftoneButtonGallery.md) preserves the earlier
optional animation example.

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
		PatternParameters paint;
		Optional<PatternParameters> overlay = none;
	};

	// Paints use the original 420 x 104 button coordinates.
	const std::array<Preset, 32> Presets{{
		{ U"Glacier", ColorF{ 0.15, 0.24, 0.38 },
			Pattern::Halftone{ ColorF{ 0.66, 0.81, 0.97 }, ColorF{ 0.95, 0.97, 1.0 },
				9, 0, 4.5, 45_deg, { 0, 0 }, { 0, 34 }, { 0, 110 } } },
		{ U"Carbon", ColorF{ 0.88, 0.94, 1.0 },
			Pattern::Halftone{ ColorF{ 0.18, 0.25, 0.34 }, ColorF{ 0.055, 0.075, 0.105 },
				7, 0.8, 2.7, 0_deg, { 2, 1 }, { 0, -80 }, { 420, 130 } } },
		{ U"Acid", ColorF{ 0.06, 0.12, 0.08 },
			Pattern::Halftone{ ColorF{ 0.09, 0.18, 0.12 }, ColorF{ 0.82, 0.98, 0.28 },
				16, 0, 8, 45_deg, { 0, 0 }, { 235, 5 }, { 325, 110 } } },
		{ U"Violet", ColorF{ 0.97, 0.92, 1.0 },
			Pattern::Halftone{ ColorF{ 0.63, 0.40, 0.98 }, ColorF{ 0.13, 0.075, 0.24 },
				11, 0, 5.5, 15_deg, { 12, 8 }, { 170, 15 }, { 410, 95 } } },
		{ U"Coral", ColorF{ 0.23, 0.06, 0.10 },
			Pattern::Halftone{ ColorF{ 1.0, 0.92, 0.84 }, ColorF{ 0.97, 0.40, 0.37 },
				18, 0, 9, 45_deg, { 4, 0 }, { 240, 5 }, { 25, 125 } } },
		{ U"Blueprint", ColorF{ 0.96, 0.98, 1.0 },
			Pattern::Halftone{ ColorF{ 0.37, 0.64, 0.94 }, ColorF{ 0.10, 0.28, 0.66 },
				10, 0.5, 2.4, 0_deg, { 5, 4 }, { 0, -35 }, { 0, 145 } } },
		{ U"Prism", ColorF{ 0.95, 0.98, 1.0 },
			Pattern::Halftone{ ColorF{ 0.20, 0.90, 0.93, 0.85 }, ColorF{ 0.07, 0.11, 0.20 },
				13, 0, 6.5, 45_deg, { 0, 0 }, { 160, 0 }, { 390, 120 } },
			Pattern::Halftone{
				ColorF{ 1.0, 0.32, 0.65, 0.70 }, ColorF{ 0, 0 },
				13, 0, 6.5, 45_deg, { 6.5, 0 }, { 220, -5 }, { -10, 125 } } },
		{ U"Champagne", ColorF{ 0.31, 0.22, 0.13 },
			Pattern::Halftone{ ColorF{ 0.71, 0.51, 0.25 }, ColorF{ 0.99, 0.96, 0.88 },
				12, 0, 6, 45_deg, { 0, 0 }, { 125, 0 }, { -45, 65 } },
			Pattern::Halftone{
				ColorF{ 0.71, 0.51, 0.25 }, ColorF{ 0, 0 },
				12, 0, 6, 45_deg, { 0, 0 }, { 295, 0 }, { 465, 65 } } },
		{ U"Rose Dots", ColorF{ 0.34, 0.13, 0.20 },
			Pattern::PolkaDot{ ColorF{ 0.90, 0.63, 0.70 }, ColorF{ 1.0, 0.91, 0.93 }, 15, 0.70, 45_deg } },
		{ U"Noir Dots", ColorF{ 0.94, 0.92, 0.84 },
			Pattern::PolkaDot{ ColorF{ 0.36, 0.32, 0.24 }, ColorF{ 0.10, 0.10, 0.095 }, 12, 0.38, 0_deg } },
		{ U"Soda", ColorF{ 0.08, 0.29, 0.36 },
			Pattern::PolkaDot{ ColorF{ 0.62, 0.88, 0.92 }, ColorF{ 0.88, 0.98, 0.98 }, 30, 1.0, 30_deg } },
		{ U"Silk", ColorF{ 0.36, 0.23, 0.15 },
			Pattern::Stripe{ ColorF{ 0.88, 0.79, 0.65 }, ColorF{ 0.99, 0.94, 0.84 }, 13, 0.28, 32_deg } },
		{ U"Racing", ColorF{ 1.0, 0.95, 0.86 },
			Pattern::Stripe{ ColorF{ 0.74, 0.23, 0.15 }, ColorF{ 0.89, 0.34, 0.22 }, 38, 0.90, -35_deg } },
		{ U"Pinstripe", ColorF{ 0.92, 0.96, 1.0 },
			Pattern::Stripe{ ColorF{ 0.27, 0.38, 0.52 }, ColorF{ 0.09, 0.15, 0.23 }, 10, 0.14, 0_deg } },
		{ U"Graphite", ColorF{ 0.88, 0.94, 0.96 },
			Pattern::Grid{ ColorF{ 0.22, 0.29, 0.33 }, ColorF{ 0.07, 0.10, 0.12 }, 24, 0.09, 0_deg },
			Pattern::Grid{ ColorF{ 0.39, 0.50, 0.55, 0.30 }, ColorF{ 0, 0 }, 96, 0.06, 0_deg } },
		{ U"Mint Grid", ColorF{ 0.10, 0.32, 0.26 },
			Pattern::Grid{ ColorF{ 0.51, 0.78, 0.65 }, ColorF{ 0.87, 0.97, 0.90 }, 23, 0.12, 0_deg } },
		{ U"Amethyst", ColorF{ 0.97, 0.92, 1.0 },
			Pattern::Grid{ ColorF{ 0.45, 0.31, 0.60 }, ColorF{ 0.19, 0.12, 0.30 }, 30, 0.16, 45_deg } },
		{ U"Gingham", ColorF{ 0.19, 0.29, 0.48 },
			Pattern::Checker{ ColorF{ 0.65, 0.76, 0.91 }, ColorF{ 0.95, 0.97, 1.0 }, 14, 0.5, 0.5 } },
		{ U"Matcha", ColorF{ 0.14, 0.24, 0.09 },
			Pattern::Checker{ ColorF{ 0.66, 0.76, 0.46 }, ColorF{ 0.83, 0.89, 0.65 }, 24, 1, 1, 45_deg } },
		{ U"Copper Weave", ColorF{ 1.0, 0.90, 0.76 },
			Pattern::Checker{ ColorF{ 0.47, 0.28, 0.19 }, ColorF{ 0.18, 0.12, 0.10 }, 8, 0.65, 0.35, 45_deg } },
		{ U"Origami", ColorF{ 0.32, 0.21, 0.40 },
			Pattern::Triangle{ ColorF{ 0.81, 0.70, 0.87 }, ColorF{ 0.95, 0.89, 0.98 }, 32, 15_deg } },
		{ U"Tidal", ColorF{ 0.90, 1.0, 0.98 },
			Pattern::Triangle{ ColorF{ 0.11, 0.42, 0.43 }, ColorF{ 0.07, 0.27, 0.32 }, 23, 30_deg } },
		{ U"Honeycomb", ColorF{ 0.35, 0.22, 0.08 },
			Pattern::HexGrid{ ColorF{ 0.77, 0.53, 0.21 }, ColorF{ 1.0, 0.90, 0.62 }, 18, 0.20, 0_deg } },
		{ U"Ion", ColorF{ 0.86, 1.0, 0.94 },
			Pattern::HexGrid{ ColorF{ 0.20, 0.58, 0.44 }, ColorF{ 0.055, 0.14, 0.15 }, 17, 0.32, 30_deg } },
		{ U"Orbit", ColorF{ 0.91, 0.98, 1.0 },
			Pattern::PolkaDot{ ColorF{ 0.24, 0.69, 0.78 }, ColorF{ 0.065, 0.15, 0.22 }, 26, 1.12, 45_deg },
			Pattern::PolkaDot{ ColorF{ 0.065, 0.15, 0.22 }, ColorF{ 0, 0 }, 26, 0.76, 45_deg } },
		{ U"Pearl Circuit", ColorF{ 0.35, 0.24, 0.25 },
			Pattern::Grid{ ColorF{ 0.80, 0.67, 0.66 }, ColorF{ 0.99, 0.93, 0.90 }, 28, 0.13 },
			Pattern::PolkaDot{ ColorF{ 0.65, 0.42, 0.43 }, ColorF{ 0, 0 }, 28, 0.28 } },
		{ U"Tartan", ColorF{ 0.99, 0.94, 0.79 },
			Pattern::Stripe{ ColorF{ 0.43, 0.16, 0.22 }, ColorF{ 0.09, 0.14, 0.20 }, 46, 1.0, 0_deg },
			Pattern::Stripe{ ColorF{ 0.84, 0.63, 0.35, 0.44 }, ColorF{ 0, 0 }, 29, 0.75, 90_deg } },
		{ U"Interference", ColorF{ 0.91, 0.97, 1.0 },
			Pattern::Stripe{ ColorF{ 0.14, 0.47, 0.51 }, ColorF{ 0.07, 0.12, 0.20 }, 9, 0.55, 30_deg },
			Pattern::Stripe{ ColorF{ 0.70, 0.36, 0.85, 0.72 }, ColorF{ 0, 0 }, 9.4, 0.50, 35_deg } },
		{ U"Ember Mesh", ColorF{ 1.0, 0.91, 0.77 },
			Pattern::HexGrid{ ColorF{ 0.38, 0.24, 0.22 }, ColorF{ 0.14, 0.09, 0.14 }, 24, 0.28, 0_deg },
			Pattern::Halftone{ ColorF{ 1.0, 0.47, 0.18, 0.85 }, ColorF{ 0, 0 },
				14, 0, 6, 30_deg, { 0, 0 }, { 170, 5 }, { 400, 110 } } },
		{ U"Kaleidoscope", ColorF{ 0.23, 0.22, 0.40 },
			Pattern::Triangle{ ColorF{ 0.80, 0.67, 0.93 }, ColorF{ 0.96, 0.91, 0.99 }, 31, 0_deg },
			Pattern::Triangle{ ColorF{ 0.20, 0.65, 0.73, 0.32 }, ColorF{ 0, 0 }, 31, 60_deg, { 0.5, 0.25 } } },
		{ U"Solar Static", ColorF{ 1.0, 0.95, 0.84 },
			Pattern::Halftone{ ColorF{ 0.99, 0.73, 0.27 }, ColorF{ 0.15, 0.13, 0.23 },
				17, 0, 8.5, 45_deg, { 0, 0 }, { 150, 0 }, { 395, 110 } },
			Pattern::Stripe{ ColorF{ 0.87, 0.77, 1.0, 0.24 }, ColorF{ 0, 0 }, 9, 0.25, -45_deg } },
		{ U"Petal Cut", ColorF{ 0.36, 0.16, 0.17 },
			Pattern::PolkaDot{ ColorF{ 0.91, 0.55, 0.47 }, ColorF{ 1.0, 0.91, 0.81 }, 28, 1.25, 0_deg },
			Pattern::Stripe{ ColorF{ 1.0, 0.91, 0.81 }, ColorF{ 0, 0 }, 28, 1.0, 90_deg, { 0.25, 0 } } },
	}};

	PatternParameters ScrollPattern(PatternParameters paint, const Float2 displacement)
	{
		const auto& m = paint.uvTransform;
		const Float2 shift{
			m._11 * displacement.x + m._21 * displacement.y,
			m._12 * displacement.x + m._22 * displacement.y };
		paint.uvTransform._31 -= shift.x;
		paint.uvTransform._32 -= shift.y;
		if (paint.type == PatternType::Halftone)
		{
			// Keep the drawing-space radius field fixed while moving the lattice.
			paint.extraParams.z += (paint.extraParams.x * shift.x + paint.extraParams.y * shift.y);
		}
		return paint;
	}
}

void Main()
{
	Window::Resize(1680, 1050);
	Window::SetTitle(U"Pattern collection");
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(1680, 1050);
	Scene::SetBackground(ColorF{ 0.075, 0.085, 0.105 });
	const Font label{ 30, Typeface::Bold };
	const auto shape = RectF{ 0, 0, 420, 104 }.rounded(20);
	constexpr double CardScale = 0.9;
	const bool capture = System::GetCommandLineArgs().contains(U"--capture");
	int32 frames = 0;
	bool animate = System::GetCommandLineArgs().contains(U"--animate");
	double animationTime = 0.0;

	while (System::Update())
	{
		if (capture && ScreenCapture::HasNewFrame())
		{
			ScreenCapture::GetFrame().save(U"pattern-collection.png");
			return;
		}

		if (KeySpace.down()) { animate = !animate; }
		if (KeyR.down()) { animate = false; animationTime = 0.0; }
		if (animate) { animationTime += Scene::DeltaTime(); }
		const Float2 displacement{
			static_cast<float>(28.0 * std::sin(animationTime * 0.55)),
			static_cast<float>(18.0 * std::sin(animationTime * 0.31)) };

		for (size_t i = 0; i < Presets.size(); ++i)
		{
			const auto& preset = Presets[i];
			const Vec2 position{ 48 + (i % 4) * 402, 34 + (i / 4) * 127 };
			const Transformer2D transform{ Mat3x2::Scale(CardScale).translated(position) };
			RectF{ 0, 5, 420, 104 }.rounded(20).draw(ColorF{ 0, 0.25 });
			shape.draw(ScrollPattern(preset.paint, displacement));
			if (preset.overlay) { shape.draw(ScrollPattern(*preset.overlay, displacement)); }
			shape.drawFrame(0, 1.5 / CardScale, ColorF{ 0.85, 0.92, 1, 0.7 });
			label(preset.name).draw(30, 30, preset.text);
		}

		if (capture && (++frames == (animate ? 90 : 3))) { ScreenCapture::RequestCurrentFrame(); }
	}
}
```
