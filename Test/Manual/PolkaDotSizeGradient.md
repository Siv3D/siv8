# Polka-dot size gradient

An interactive comparison of `Pattern::PolkaDotSizeGradient` in drawing
coordinates and under an object transform. The first preset uses a square
lattice rotated by 45 degrees. Radius varies per dot, using a smoothstep profile.

## Execution

1. Paste the complete program below into a separate Siv3D application built with
   this revision. Keep the repository's platform test entry point intact.
2. Run on macOS (Metal) or Windows (D3D11). On Windows, include the standard
   Siv3D application resources and place the executable beside the `engine/`
   assets, as in `WindowsDesktop/App/`. See the
   [D3D11 integration guide](../../docs/renderer2d/d3d11-polka-dot-size-gradient-handoff.md)
   for automated verification and image-comparison criteria.
3. Compare the presets, then drag the controls. `Start` and `End` are fractions
   of the 340-unit reference span. `Field direction` rotates the transition
   independently of `Lattice angle`. Both previews use identical paint parameters.
4. Enable `Guides` to see the start (cyan), end (orange), and origin (pink).
   Move `Start`, then move `Origin X/Y`: only the latter moves dot centers.
5. Enable `Split rectangle` to draw the same region as a rectangle and two
   triangles. Adjust object zoom and rotation in the right preview.
6. Optionally launch with `--capture`. It saves `pattern-size-gradient.png` in the
   application's working directory and exits. This optional retained screenshot
   is not an automated-test fixture; remove it when no longer needed.
   A separate Windows sample build and retained comparison captures can live in
   `WindowsDesktop/Intermediate/Manual/PolkaDotSizeGradient/`; keep generated
   source and project files out of the platform test entry point.

## Expected results

- The default forms staggered rows from a rotated square lattice, with four
  nearest neighbors per center. Dot radii grow downward, while spacing stays fixed.
- Start/end and direction do not move centers. Origin and lattice angle do not
  move the transition boundaries. Each dot remains circular before the object
  transform; the radius is sampled at its center, not separately at each pixel.
- Radius zero gives exactly the background. `Min radius == Max radius` gives
  constant-size dots. Maximum radius is limited to half the pitch so circles
  stay within their respective cells.
- Transition boundaries classify dot centers; neighboring nonzero circles can
  extend across the start line. They are not clipped there.
- The object transform carries the lattice and transition together. Splitting
  the rectangle does not restart the pattern or introduce a seam.
- Background alpha reveals the substrate. Subpixel dots fade out; strong
  minification can still alias because this shader is not an area-integrated
  filter for an arbitrarily large number of dots.

## Complete Main.cpp

```cpp
# include <Siv3D.hpp>
# include <cmath>

namespace
{
	struct Settings
	{
		double pitch = 36, minRadius = 0, maxRadius = 12;
		double angle = 45, originX = 0, originY = 0;
		double start = 0.25, end = 1, direction = 0;
		double zoom = 1.2, rotation = 12, backgroundAlpha = 1;
	};

	void Slider(const Font& font, const int32 id, int32& active, const StringView label,
		double& value, const double minimum, const double maximum, const double y)
	{
		const RectF hit{ 1010, y, 150, 28 };
		if (hit.leftClicked()) { active = id; }
		if ((active == id) && MouseL.pressed())
		{
			value = minimum + Clamp((Cursor::PosF().x - 1020) / 130, 0.0, 1.0) * (maximum - minimum);
		}
		const double t = (maximum > minimum) ? (value - minimum) / (maximum - minimum) : 0;
		font(label, U" ", U"{:.2f}"_fmt(value)).draw(820, y + 3, ColorF{ 0.88 });
		Line{ 1020, y + 14, 1150, y + 14 }.draw(4, ColorF{ 0.25 });
		Circle{ 1020 + t * 130, y + 14, 6 }.draw(ColorF{ 0.3, 0.8, 1 });
	}

	Pattern::PolkaDotSizeGradient MakePattern(const Settings& s)
	{
		const Vec2 direction{ std::sin(s.direction * 1_deg), std::cos(s.direction * 1_deg) };
		return {
			.primary = ColorF{ 0.16, 0.38, 0.70 },
			.background = ColorF{ 0.96, 0.97, 1, s.backgroundAlpha },
			.pitch = s.pitch, .minRadius = s.minRadius, .maxRadius = s.maxRadius,
			.angle = s.angle * 1_deg, .origin = { s.originX, s.originY },
			.start = direction * (-170 + 340 * s.start),
			.end = direction * (-170 + 340 * s.end),
		};
	}

	void Preview(const int32 x, const Settings& s, const bool transformed,
		const bool split, const bool guides)
	{
		const ScopedViewport2D viewport{ x, 112, 380, 470 };
		RectF{ 0, 0, 380, 470 }.draw(ColorF{ 0.17, 0.21, 0.27 });
		const Transformer2D transform{ Mat3x2::Scale(transformed ? s.zoom : 1.0)
			.rotated(transformed ? s.rotation * 1_deg : 0.0).translated(190, 235) };
		const RectF rect{ -140, -170, 280, 340 };
		rect.draw(ColorF{ 0.35, 0.50, 0.42 });
		RectF{ 0, -170, 140, 340 }.draw(ColorF{ 0.55, 0.38, 0.40 });
		const auto pattern = MakePattern(s);
		if (split)
		{
			RectF{ -140, -170, 113, 340 }.draw(pattern);
			Triangle{ -27, -170, 140, -170, -27, 170 }.draw(pattern);
			Triangle{ 140, -170, 140, 170, -27, 170 }.draw(pattern);
		}
		else { rect.draw(pattern); }
		if (guides)
		{
			const Vec2 tangent{ std::cos(s.direction * 1_deg), -std::sin(s.direction * 1_deg) };
			Line{ pattern.start - tangent * 170, pattern.start + tangent * 170 }
				.draw(1.5, ColorF{ 0, 0.8, 0.9 });
			Line{ pattern.end - tangent * 170, pattern.end + tangent * 170 }
				.draw(1.5, ColorF{ 1, 0.6, 0.1 });
			Circle{ pattern.origin, 4 }.draw(ColorF{ 1, 0.2, 0.5 });
		}
	}
}

void Main()
{
	Window::Resize(1180, 760);
	Window::SetTitle(U"Polka-dot size gradient");
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(1180, 760);
	Scene::SetBackground(ColorF{ 0.07, 0.09, 0.13 });
	const Font title{ 25, Typeface::Bold };
	const Font font{ 16 };
	Settings s;
	bool guides = false, split = false;
	int32 active = -1, frames = 0;
	const bool capture = System::GetCommandLineArgs().contains(U"--capture");

	while (System::Update())
	{
		if (capture && ScreenCapture::HasNewFrame())
		{
			ScreenCapture::GetFrame().save(U"pattern-size-gradient.png");
			return;
		}
		if (not MouseL.pressed()) { active = -1; }
		title(U"Polka-dot size gradient").draw(20, 15);
		font(U"Independent lattice and transition controls").draw(410, 23);
		if (SimpleGUI::Button(U"45 deg / grow", Vec2{ 20, 58 }, 180)) { s = Settings{}; }
		if (SimpleGUI::Button(U"0 deg / grow", Vec2{ 220, 58 }, 180)) { s = Settings{}; s.angle = 0; }
		if (SimpleGUI::Button(U"Horizontal", Vec2{ 420, 58 }, 180)) { s = Settings{}; s.direction = 90; }
		if (SimpleGUI::Button(U"Shrink down", Vec2{ 620, 58 }, 180)) { s = Settings{}; s.direction = 180; }
		font(U"Drawing coordinates").draw(20, 90);
		font(U"Same pattern + object transform").draw(420, 90);

		Slider(font, 0, active, U"Pitch", s.pitch, 16, 64, 120);
		s.maxRadius = Min(s.maxRadius, s.pitch / 2);
		Slider(font, 1, active, U"Max radius", s.maxRadius, 0, s.pitch / 2, 154);
		s.minRadius = Min(s.minRadius, s.maxRadius);
		Slider(font, 2, active, U"Min radius", s.minRadius, 0, s.maxRadius, 188);
		Slider(font, 3, active, U"Lattice angle", s.angle, -90, 90, 222);
		Slider(font, 4, active, U"Origin X", s.originX, -64, 64, 256);
		Slider(font, 5, active, U"Origin Y", s.originY, -64, 64, 290);
		Slider(font, 6, active, U"Start", s.start, 0, 0.8, 324);
		Slider(font, 7, active, U"End", s.end, 0.85, 1.2, 358);
		Slider(font, 8, active, U"Field direction", s.direction, -180, 180, 392);
		Slider(font, 9, active, U"Object zoom", s.zoom, 0.5, 1.6, 426);
		Slider(font, 10, active, U"Object angle", s.rotation, -45, 45, 460);
		Slider(font, 11, active, U"Background alpha", s.backgroundAlpha, 0, 1, 494);
		SimpleGUI::CheckBox(guides, U"Guides", Vec2{ 820, 544 });
		SimpleGUI::CheckBox(split, U"Split rectangle", Vec2{ 950, 544 });
		Preview(20, s, false, split, guides);
		Preview(420, s, true, split, guides);
		font(U"Start/end change size, without moving dot centers.").draw(20, 610);
		font(U"Lattice angle changes placement, without rotating the transition.").draw(20, 640);
		font(U"Cyan: start | Orange: end | Pink: a dot center (origin)").draw(20, 670);
		font(U"Circles may extend across start/end lines. These lines do not clip the dots.").draw(20, 708);
		if (capture && (++frames == 3)) { ScreenCapture::RequestCurrentFrame(); }
	}
}
```
