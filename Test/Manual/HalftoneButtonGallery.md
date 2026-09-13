# Halftone button backgrounds

Eight button-background recipes using only `Pattern::Halftone`.
The first six use one pattern draw. Prism and Champagne use a second pattern
with a transparent background. No custom shader or external image is needed.

## Execution

1. Paste the complete code below into a separate Siv3D application using a
   backend that implements `Halftone`.
2. Run the application and compare the eight backgrounds. Click a button to
   select it. Press Space to toggle slow lattice motion while keeping each
   gradient's endpoints fixed.
3. Each recipe is defined for a 420 x 104 local rectangle. The gallery places
   it using `Transformer2D`; the paint's origin and endpoints need no per-button
   world-coordinate adjustments. Adapt the local endpoints when changing size.
4. Optional: run with `--capture` to save `halftone-buttons.png` in the working
   directory and exit. This retained screenshot can be deleted after inspection.

## Expected results

- Glacier: a low-contrast blue frost concentrated toward the lower edge.
- Carbon: tiny dots with a nonzero minimum radius and a quiet diagonal change.
- Acid: large dark dots entering from the lower-right side of a lime surface.
- Violet: purple dots growing toward the right, leaving room for the label.
- Coral: light dots against a saturated background, reversing the visual weight.
- Blueprint: a fine unrotated lattice whose dots remain visible across the surface.
- Prism: cyan and translucent pink layers with different phases and directions.
- Champagne: opposing gold fields that leave the middle comparatively plain.

All dot radii satisfy `0 <= minRadius <= maxRadius <= pitch/2`. Overlays use
ordinary alpha compositing, not an additional shader feature. Small dots may
appear softer at non-integral display scales. Selection/hover frames and labels
are ordinary drawing operations separate from the background recipes.

## Complete Main.cpp

```cpp
# include <Siv3D.hpp>
# include <array>

namespace
{
	struct Preset
	{
		StringView name;
		StringView note;
		ColorF text;
		Pattern::Halftone paint;
		Optional<Pattern::Halftone> overlay = none;
	};

	const std::array<Preset, 8> Presets{{
		{ U"Glacier", U"SOFT FROST / 45 DEG", ColorF{ 0.15, 0.24, 0.38 },
			{ ColorF{ 0.66, 0.81, 0.97 }, ColorF{ 0.95, 0.97, 1.0 },
				9, 0, 4.5, 45_deg, { 0, 0 }, { 0, 34 }, { 0, 110 } } },
		{ U"Carbon", U"MICRO TEXTURE / NONZERO MINIMUM", ColorF{ 0.88, 0.94, 1.0 },
			{ ColorF{ 0.18, 0.25, 0.34 }, ColorF{ 0.055, 0.075, 0.105 },
				7, 0.8, 2.7, 0_deg, { 2, 1 }, { 0, -80 }, { 420, 130 } } },
		{ U"Acid", U"BOLD HALFTONE / DIAGONAL", ColorF{ 0.06, 0.12, 0.08 },
			{ ColorF{ 0.09, 0.18, 0.12 }, ColorF{ 0.82, 0.98, 0.28 },
				16, 0, 8, 45_deg, { 0, 0 }, { 235, 5 }, { 325, 110 } } },
		{ U"Violet", U"ELECTRIC PURPLE / SIDE FADE", ColorF{ 0.97, 0.92, 1.0 },
			{ ColorF{ 0.63, 0.40, 0.98 }, ColorF{ 0.13, 0.075, 0.24 },
				11, 0, 5.5, 15_deg, { 12, 8 }, { 170, 15 }, { 410, 95 } } },
		{ U"Coral", U"LIGHT DOTS / REVERSED WEIGHT", ColorF{ 0.23, 0.06, 0.10 },
			{ ColorF{ 1.0, 0.92, 0.84 }, ColorF{ 0.97, 0.40, 0.37 },
				18, 0, 9, 45_deg, { 4, 0 }, { 240, 5 }, { 25, 125 } } },
		{ U"Blueprint", U"FINE GRID / QUIET DEPTH", ColorF{ 0.96, 0.98, 1.0 },
			{ ColorF{ 0.37, 0.64, 0.94 }, ColorF{ 0.10, 0.28, 0.66 },
				10, 0.5, 2.4, 0_deg, { 5, 4 }, { 0, -35 }, { 0, 145 } } },
		{ U"Prism", U"TWO TRANSLUCENT COLOR FIELDS", ColorF{ 0.95, 0.98, 1.0 },
			{ ColorF{ 0.20, 0.90, 0.93, 0.85 }, ColorF{ 0.07, 0.11, 0.20 },
				13, 0, 6.5, 45_deg, { 0, 0 }, { 160, 0 }, { 390, 120 } },
			Pattern::Halftone{
				ColorF{ 1.0, 0.32, 0.65, 0.70 }, ColorF{ 0, 0 },
				13, 0, 6.5, 45_deg, { 6.5, 0 }, { 220, -5 }, { -10, 125 } } },
		{ U"Champagne", U"MIRRORED EDGES / OPEN CENTER", ColorF{ 0.31, 0.22, 0.13 },
			{ ColorF{ 0.71, 0.51, 0.25 }, ColorF{ 0.99, 0.96, 0.88 },
				12, 0, 6, 45_deg, { 0, 0 }, { 125, 0 }, { -45, 65 } },
			Pattern::Halftone{
				ColorF{ 0.71, 0.51, 0.25 }, ColorF{ 0, 0 },
				12, 0, 6, 45_deg, { 0, 0 }, { 295, 0 }, { 465, 65 } } },
	}};

	void DrawButton(const Preset& preset, const Vec2 phase)
	{
		const auto shape = RectF{ 0, 0, 420, 104 }.rounded(20);
		auto paint = preset.paint;
		paint.origin += phase;
		shape.draw(paint);
		if (preset.overlay)
		{
			auto overlay = *preset.overlay;
			overlay.origin += phase;
			shape.draw(overlay);
		}
	}
}

void Main()
{
	Window::Resize(1000, 840);
	Window::SetTitle(U"Halftone button collection");
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(1000, 840);
	Scene::SetBackground(ColorF{ 0.075, 0.085, 0.105 });
	const Font title{ 30, Typeface::Bold };
	const Font label{ 25, Typeface::Bold };
	const Font small{ 14 };
	const Font body{ 16 };
	bool animate = false;
	double phase = 0;
	Optional<size_t> selected;
	int32 frames = 0;
	const bool capture = System::GetCommandLineArgs().contains(U"--capture");

	while (System::Update())
	{
		if (capture && ScreenCapture::HasNewFrame())
		{
			ScreenCapture::GetFrame().save(U"halftone-buttons.png");
			return;
		}
		if (KeySpace.down()) { animate = !animate; }
		if (animate) { phase += Scene::DeltaTime() * 4; }
		title(U"Small dots. Different moods.").draw(48, 28, ColorF{ 0.94 });
		body(U"Eight button backgrounds / one existing Pattern / no custom shaders")
			.draw(48, 72, ColorF{ 0.58, 0.64, 0.73 });

		for (size_t i = 0; i < Presets.size(); ++i)
		{
			const auto& preset = Presets[i];
			const Vec2 position{ 48 + (i % 2) * 484, 145 + (i / 2) * 158 };
			const RectF rect{ position, 420, 104 };
			const bool hovered = rect.rounded(20).mouseOver();
			if (rect.rounded(20).leftClicked()) { selected = i; }
			small(U"0", i + 1, U" / ", preset.note).draw(position.x, position.y - 26,
				ColorF{ 0.53, 0.60, 0.70 });
			{
				const Transformer2D transform{ Mat3x2::Translate(position) };
				RectF{ 0, 5, 420, 104 }.rounded(20).draw(ColorF{ 0, 0.25 });
				DrawButton(preset, { phase, phase * 0.3 });
				label(preset.name).draw(26, 34, preset.text);
				// A simple arrow keeps the background recipe independent of icon assets.
				Line{ 365, 52, 385, 52 }.draw(2, preset.text);
				Line{ 378, 45, 385, 52 }.draw(2, preset.text);
				Line{ 378, 59, 385, 52 }.draw(2, preset.text);
				if ((selected && *selected == i) || hovered)
				{
					RectF{ 0, 0, 420, 104 }.rounded(20).drawFrame(0, 2, ColorF{ 0.85, 0.92, 1, 0.8 });
				}
			}
		}
		body(U"SPACE  ", animate ? U"Motion on" : U"Motion off", U"   /   Click to select")
			.draw(48, 786, ColorF{ 0.62, 0.69, 0.79 });
		if (selected) { body(U"Selected: ", Presets[*selected].name).draw(700, 786, ColorF{ 0.9 }); }
		if (capture && (++frames == 3)) { ScreenCapture::RequestCurrentFrame(); }
	}
}
```
