# Pattern gallery and zoom comparison

An interactive gallery for the six constant-parameter `Pattern` shaders. Each type has six
parameter presets, editable controls, and two simultaneous uniform-zoom previews.
The program uses public APIs and requires no external assets or output files.
For rotation, nonuniform scale, reflection, shear, and primitive continuity,
use the [coordinate comparison](PatternTransforms.md). For independently varying
dot radii, use the [size-gradient sample](PolkaDotSizeGradient.md).

## Execution

1. Paste the complete code below into `Main.cpp` of a separate Siv3D v0.8
   application built with this revision. Keep the repository's platform test
   entry points intact.
2. Run on macOS (Metal) or Windows (D3D11). Select a pattern with the top buttons,
   then click a preset card. Drag the controls at the right to edit that preset's
   copy in the bottom previews; the gallery cards remain fixed references.
3. Try scale, radius/thickness, angle, offset, and background alpha. Checker also
   has independent horizontal/vertical intensity controls. Disabled controls do
   not apply to the current type. Enable `Animate offset` to scroll the pattern.
4. Set `Object zoom` to 0.5, 1, and 2. Compare the two bottom previews. `Zoom = 1`
   resets only this zoom; `Reset` restores the editable pattern and stops animation.
   Disable `Animate offset` and enable `Pan shapes` to observe translation:
   the pattern stays attached as the previews move horizontally.
5. Keep object zoom fixed and use the window-size buttons or resize the window by
   hand. The scene stays at 1200 x 800 (`ResizeMode::Keep`). The footer reports the
   actual framebuffer/scene presentation scale, including HiDPI and letterboxing.

## Expected results

- Every type shows six variations. Checker varies its two intensities in the
  Bold / H-V and Rotated presets; Triangle varies size, angle, offset, and alpha
  because it has no radius/thickness parameter. Transparent areas reveal the
  colored substrate behind the pattern.
- Increasing `scale` makes the pattern coarser. `scale` is a pattern coordinate
  scale, not a universal cell diameter: the repeat geometry differs by type.
- Under uniform object zoom `z`, the left preview uses the entered scale `s` and
  has effective pattern scale `s * z` in scene pixels. The right uses `s / z`, so
  its effective pattern scale stays `s`. Both shapes and their white 100-unit
  rulers grow with `z`; the cyan 100-scene-pixel rulers remain fixed.
- Resizing the window scales the entire fixed-resolution scene, including both
  previews, their rulers, and the UI. Even the right preview then changes size in
  framebuffer pixels. Its compensation applies to object zoom only. Texture
  filtering can soften the presentation at non-integral scales.
- Both backends evaluate patterns in drawing coordinates before the local/camera
  transforms. The two previews have the same phase at zoom 1, and moving either
  preview with `Pan shapes` carries its pattern with it. Offset scrolls the
  pattern in its own coordinates. At other zoom values the two previews use
  different pattern scales; their features need not align.
- There is no per-shape origin. The rectangle and circle in a preview share one
  pattern coordinate system. Changing their geometry coordinates directly would
  move their outlines within that pattern; the pan uses `Transformer2D` instead.

## Complete Main.cpp

```cpp
# include <Siv3D.hpp>
# include <array>

namespace
{
	struct Settings
	{
		double scale = 28.0;
		double amount = 0.25;
		double horizontal = 1.0;
		double vertical = 1.0;
		double degrees = 0.0;
		Vec2 offset{ 0, 0 };
		double backgroundAlpha = 1.0;
	};

	struct Preset
	{
		StringView name;
		Settings settings;
	};

	const std::array<StringView, 6> Names{
		U"PolkaDot", U"Stripe", U"Grid", U"Checker", U"Triangle", U"HexGrid"
	};

	const std::array<Preset, 6> Presets{{
		{ U"Baseline", {} },
		{ U"Fine", { .scale = 12.0 } },
		{ U"Large", { .scale = 56.0 } },
		{ U"Bold / H-V", { .amount = 0.75, .vertical = 0.25,
			.degrees = 15.0, .offset = { 0.5, 0.0 } } },
		{ U"Rotated", { .amount = 0.4, .horizontal = 0.25, .degrees = 45.0 } },
		{ U"Transparent", { .scale = 36.0, .amount = 0.35,
			.horizontal = 0.65, .vertical = 0.65, .degrees = 20.0,
			.offset = { 0.25, 0.5 }, .backgroundAlpha = 0.0 } },
	}};

	PatternParameters MakePattern(const size_t type, const Settings& s,
		const double scaleDivisor = 1.0)
	{
		const ColorF primary{ HSV{ (190.0 + type * 37.0), 0.55, 1.0 } };
		const ColorF background{ 0.08, 0.12, 0.20, s.backgroundAlpha };
		const double scale = (s.scale / scaleDivisor);
		const double angle = (s.degrees * 1_deg);
		switch (type)
		{
		case 0:
			return Pattern::PolkaDot{ primary, background, scale, s.amount, angle, s.offset };
		case 1:
			return Pattern::Stripe{ primary, background, scale, s.amount, angle, s.offset };
		case 2:
			return Pattern::Grid{ primary, background, scale, s.amount, angle, s.offset };
		case 3:
			return Pattern::Checker{ primary, background, scale,
				s.horizontal, s.vertical, angle, s.offset };
		case 4:
			return Pattern::Triangle{ primary, background, scale, angle, s.offset };
		default:
			return Pattern::HexGrid{ primary, background, scale, s.amount, angle, s.offset };
		}
	}

	// This revision's SimpleGUI has buttons/checkboxes; use a small local slider.
	void Slider(const Font& font, const int32 id, int32& active,
		const StringView label, double& value, const double minimum,
		const double maximum, const double y, const bool enabled = true)
	{
		const RectF hit{ 975, y, 198, 26 };
		if (enabled && hit.leftClicked())
		{
			active = id;
		}
		if (enabled && (active == id) && MouseL.pressed())
		{
			value = (minimum + Clamp((Cursor::PosF().x - 985.0) / 178.0, 0.0, 1.0)
				* (maximum - minimum));
		}
		const ColorF color = enabled ? ColorF{ 0.85 } : ColorF{ 0.38 };
		font(label, U" ", U"{:.2f}"_fmt(value)).draw(782, y + 3, color);
		Line{ 985, y + 13, 1163, y + 13 }.draw(4, ColorF{ 0.25 });
		Circle{ 985 + (value - minimum) / (maximum - minimum) * 178,
			y + 13, 7 }.draw(color);
	}

	void DrawSubstrate(const RectF& rect)
	{
		rect.draw(ColorF{ 0.24, 0.30, 0.38 });
		RectF{ rect.x, rect.y, rect.w / 2, rect.h }.draw(ColorF{ 0.44, 0.24, 0.28 });
	}

	void DrawPreview(const Font& font, const size_t type, const Settings& settings,
		const double zoom, const double x, const bool compensate, const double pan)
	{
		font(compensate ? U"B: scale / zoom (fixed spacing in scene)"
			: U"A: scale unchanged (spacing follows zoom)").draw(x, 500);
		const RectF panel{ x, 530, 560, 204 };
		DrawSubstrate(panel);
		{
			const Transformer2D transform{
				Mat3x2::Scale(zoom).translated(x + 280 + pan, 621) };
			const auto pattern = MakePattern(type, settings, compensate ? zoom : 1.0);
			RectF{ -112, -36, 112, 72 }.rounded(12).draw(pattern);
			Circle{ 70, 0, 36 }.draw(pattern);
			// The ruler's length, unlike its stroke width, follows object zoom.
			Line{ -50, 45, 50, 45 }.draw(2.0 / zoom, Palette::White);
		}
		Line{ x + 18, 719, x + 118, 719 }.draw(2, ColorF{ 0.2, 0.9, 1.0 });
		font(U"100 scene px").draw(x + 126, 705, ColorF{ 0.2, 0.9, 1.0 });
		font(U"effective scale: ", U"{:.1f}"_fmt(settings.scale * (compensate ? 1.0 : zoom)),
			U"  | white ruler: 100 object units").draw(x, 738);
	}
}

void Main()
{
	Window::Resize(1200, 800);
	Window::SetTitle(U"Pattern gallery - variations and zoom");
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(1200, 800);
	Scene::SetBackground(ColorF{ 0.07, 0.09, 0.13 });
	const Font font{ 16 };
	const Font title{ 25, Typeface::Bold };
	size_t type = 0;
	Settings settings;
	double zoom = 1.0;
	double animation = 0.0;
	bool animate = false;
	bool panShapes = false;
	int32 activeSlider = -1;

	while (System::Update())
	{
		if (not MouseL.pressed())
		{
			activeSlider = -1;
		}
		title(U"Pattern gallery").draw(20, 14);
		font(U"Click a preset, then edit it below. Cyan ruler = scene; white ruler = object.")
			.draw(260, 23);
		for (size_t i = 0; i < Names.size(); ++i)
		{
			const double x = (20 + i * 195);
			if (SimpleGUI::Button(Names[i], Vec2{ x, 57 }, 180.0))
			{
				type = i;
				settings = Settings{};
				animation = 0.0;
			}
			if (type == i)
			{
				RectF{ x, 94, 180, 3 }.draw(ColorF{ 0.2, 0.9, 1.0 });
			}
		}

		for (size_t i = 0; i < Presets.size(); ++i)
		{
			const double x = (20 + (i % 3) * 250);
			const double y = (111 + (i / 3) * 188);
			const RectF card{ x, y, 236, 176 };
			const RectF swatch{ x + 8, y + 32, 220, 92 };
			const auto& preset = Presets[i];
			const auto& s = preset.settings;
			card.draw(ColorF{ 0.13, 0.16, 0.21 });
			font(preset.name).draw(x + 8, y + 5);
			DrawSubstrate(swatch);
			swatch.draw(MakePattern(type, s));
			font(U"s=", s.scale, U"  angle=", s.degrees, U"  alpha=", s.backgroundAlpha)
				.draw(x + 8, y + 127);
			if (type == 3)
			{
				font(U"H=", s.horizontal, U"  V=", s.vertical).draw(x + 8, y + 150);
			}
			else if (type == 4)
			{
				font(U"offset=", s.offset).draw(x + 8, y + 150);
			}
			else
			{
				font((type == 0) ? U"radiusScale=" : U"thicknessScale=", s.amount)
					.draw(x + 8, y + 150);
			}
			if (card.mouseOver())
			{
				card.drawFrame(1, 0, ColorF{ 0.2, 0.9, 1.0 });
			}
			if (card.leftClicked())
			{
				settings = s;
				animation = 0.0;
			}
		}

		Slider(font, 0, activeSlider, U"scale", settings.scale, 8, 80, 110);
		Slider(font, 1, activeSlider, (type == 0) ? U"radiusScale" : U"thicknessScale",
			settings.amount, 0.02, 1, 142, (type != 3) && (type != 4));
		Slider(font, 2, activeSlider, U"angle (deg)", settings.degrees, 0, 180, 174);
		Slider(font, 3, activeSlider, U"offset X", settings.offset.x, -2, 2, 206);
		Slider(font, 4, activeSlider, U"offset Y", settings.offset.y, -2, 2, 238);
		Slider(font, 5, activeSlider, U"horizontal", settings.horizontal, 0, 1, 270, type == 3);
		Slider(font, 6, activeSlider, U"vertical", settings.vertical, 0, 1, 302, type == 3);
		Slider(font, 7, activeSlider, U"background alpha", settings.backgroundAlpha, 0, 1, 334);
		Slider(font, 8, activeSlider, U"Object zoom", zoom, 0.5, 2, 366);
		SimpleGUI::CheckBox(animate, U"Animate offset", Vec2{ 782, 405 });
		SimpleGUI::CheckBox(panShapes, U"Pan shapes", Vec2{ 1000, 405 });
		if (SimpleGUI::Button(U"Reset", Vec2{ 782, 450 }, 112.0))
		{
			settings = Settings{};
			animation = 0.0;
			animate = false;
			panShapes = false;
		}
		if (SimpleGUI::Button(U"Zoom = 1", Vec2{ 914, 450 }, 130.0))
		{
			zoom = 1.0;
		}
		if (animate)
		{
			animation += (Scene::DeltaTime() * 0.3);
		}
		Settings preview = settings;
		preview.offset += Vec2{ animation, animation * 0.4 };
		const double pan = panShapes ? (25.0 * Periodic::Sine1_1(6s)) : 0.0;
		DrawPreview(font, type, preview, zoom, 20, false, pan);
		DrawPreview(font, type, preview, zoom, 620, true, pan);

		const Size framebuffer = Window::GetState().frameBufferSize;
		const double presentation = Min(framebuffer.x / 1200.0, framebuffer.y / 800.0);
		font(U"Scene 1200x800 | framebuffer ", framebuffer,
			U" | presentation x", U"{:.2f}"_fmt(presentation)).draw(20, 774);
		font(U"Window:").draw(760, 774);
		if (SimpleGUI::Button(U"75%", Vec2{ 840, 760 }, 100.0))
		{
			Window::Resize(900, 600);
		}
		if (SimpleGUI::Button(U"100%", Vec2{ 960, 760 }, 100.0))
		{
			Window::Resize(1200, 800);
		}
		if (SimpleGUI::Button(U"125%", Vec2{ 1080, 760 }, 100.0))
		{
			Window::Resize(1500, 1000);
		}
	}
}
```
