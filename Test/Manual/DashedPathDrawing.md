# LineStyle: Line and path comparison

The canonical gallery for `draw(LineStyle, ...)` and
`LineString::drawClosed(LineStyle, ...)` compares every existing LineStyle preset
with the original Line renderer. The overview shows all 13 presets at once:
Square, Round, Flat, the five patterned presets, and their five Flat variants.
Each row keeps the preset's original cap; the program changes only its phase.

The seven columns are Line, a collinear LineString, a polyline with right-angle
corners, Bezier2, Bezier3, Spline2D, and a closed LineString. The first two have
identical geometry, including endpoints; the others share their horizontal span
but have different path lengths. Use the 2.1x detail view to inspect a selected
preset. Width, phase and opacity are shared across every sample.

## Run

1. Paste the complete program below into a separate Siv3D application's Main.cpp
   built with this revision. Keep the repository's platform entry point intact.
2. Run on Metal or D3D11 with the standard engine assets; no extra assets are
   required. The resizable window displays a fixed 1400 x 900 logical canvas.
3. Start with the overview, then click a row and press Tab for its detail view.
   The initial state is paused, width 6, phase 0, and alpha 0.5.

| Key | Action |
|---|---|
| Tab | Toggle overview / selected-preset detail |
| Up / Down | Select the previous / next preset; clicking a row also selects it |
| Space | Pause / resume phase animation |
| Left / Right | Pause and step phase by 0.1; hold Shift for 0.01 |
| W / S | Increase / decrease width within 1–16 source units |
| A | Toggle alpha 0.5 / 1 |
| G | Toggle gray centerlines, green starts, and red ends; closed paths have only a start marker |
| R | Reset width, phase, opacity, guides and animation; retain the selected preset and view |

Optional command-line arguments:

- `--detail` starts in the enlarged view.
- `--preset=RoundDotFlat` selects a preset by its exact name.
- `--width=12` and `--phase=1.25` set the starting width and phase. Width is
  clamped to 1–16; phase wraps by 20, a common period of all five patterns.
- `--capture` saves the selected view after four frames and exits. Images are
  `line-style-overview.png` or `line-style-<preset>.png` in the application's
  working directory. Repeated captures of the same view overwrite that file.
  Remove retained images when no longer needed; avoid input during capture.

## Expected results

- Line and the straight LineString should agree on visible intervals and phase.
  Look at endpoints in the non-Flat and Flat rows. Cut-edge antialiasing can differ
  between the Line shader and the path's geometric cuts.
- Square, Round and Flat are solid styles and ignore phase. Their open endpoints
  differ; closed paths have no endpoint caps.
- Patterned non-Flat presets extend the whole open path by half the width. Each
  dash still has flat cuts. RoundDot places round points and uses Line's open
  non-Flat pitch fitting; RoundDotFlat does not fit the pitch.
- Curves keep spacing along the sampled centerline without restarting at each
  vertex. Different path lengths produce different dash counts and terminal
  pieces. The detail view preserves source geometry; adaptive sampling may become
  finer under its larger transform.
- The closed path keeps its period instead of fitting it to the perimeter.
  Stroked pieces spanning the seam join; RoundDot retains whole dots with centers
  within one cycle. Seam spacing can differ from the regular pitch.
- Near a corner, a dash ending just after the vertex can produce a distorted
  outline or darker overlap. Set width 12–16, enable alpha 0.5, and step phase in
  the detail view to inspect this. Tight curves and thick strokes can also
  overlap. These are accepted rendering limits, not artifact-free guarantees.

See the [drawing guide](../../docs/geometry2d/dashed-drawing.md) for design and
measurement details. The [automated tests](../Test_DashedPath.cpp) cover placement,
boundaries, rendering and CPU submission. To run correctness tests on macOS:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=~[benchmark]DashedPath.*'
```

For Windows and the isolated Release benchmark, use the
[drawing guide's instructions](../../docs/geometry2d/dashed-drawing.md#implementation-and-measurement).

## Complete program

```cpp
# include <Siv3D.hpp>
# include <array>
# include <cmath>

namespace
{
	struct Preset
	{
		StringView name;
		LineStyle::Parameters parameters;
	};

	constexpr std::array<Preset, 13> Presets{{
		{ U"Square", LineStyle::Square },
		{ U"Round", LineStyle::Round },
		{ U"Flat", LineStyle::Flat },
		{ U"Dotted", LineStyle::Dotted },
		{ U"Dashed", LineStyle::Dashed },
		{ U"LongDash", LineStyle::LongDash },
		{ U"DashDot", LineStyle::DashDot },
		{ U"RoundDot", LineStyle::RoundDot },
		{ U"DottedFlat", LineStyle::DottedFlat },
		{ U"DashedFlat", LineStyle::DashedFlat },
		{ U"LongDashFlat", LineStyle::LongDashFlat },
		{ U"DashDotFlat", LineStyle::DashDotFlat },
		{ U"RoundDotFlat", LineStyle::RoundDotFlat },
	}};

	constexpr std::array<StringView, 7> Titles{
		U"Line", U"LineString", U"LineString", U"Bezier2", U"Bezier3", U"Spline2D", U"drawClosed"
	};
	constexpr std::array<StringView, 7> Subtitles{
		U"reference", U"straight", U"corners", U"quadratic", U"cubic", U"Catmull-Rom", U"LineString"
	};
	constexpr std::array<Vec2, 7> Starts{{
		{ 0, 0 }, { 0, 0 }, { 0, 12 }, { 0, 10 }, { 0, 12 }, { 0, 0 }, { 0, -12 }
	}};
	constexpr std::array<Vec2, 6> Ends{{
		{ 132, 0 }, { 132, 0 }, { 132, -12 }, { 132, 10 }, { 132, -12 }, { 132, 0 }
	}};

	void DrawGeometry(const size_t kind, const LineStyle& style, const double width, const ColorF& color)
	{
		static const LineString straight{ { 0, 0 }, { 44, 0 }, { 88, 0 }, { 132, 0 } };
		static const LineString corners{ { 0, 12 }, { 66, 12 }, { 66, -12 }, { 132, -12 } };
		static const Spline2D spline = Spline2D::FromCatmullRom(
			LineString{ { 0, 0 }, { 33, -12 }, { 66, 12 }, { 99, -12 }, { 132, 0 } });
		static const LineString ring{ { 0, -12 }, { 132, -12 }, { 132, 12 }, { 0, 12 } };
		switch (kind)
		{
		case 0: Line{ 0, 0, 132, 0 }.draw(style, width, color); break;
		case 1: straight.draw(style, width, color); break;
		case 2: corners.draw(style, width, color); break;
		case 3: Bezier2{ { 0, 10 }, { 66, -32 }, { 132, 10 } }.draw(style, width, color); break;
		case 4: Bezier3{ { 0, 12 }, { 30, -44 }, { 102, 44 }, { 132, -12 } }.draw(style, width, color); break;
		case 5: spline.draw(style, width, color); break;
		case 6: ring.drawClosed(style, width, color); break;
		}
	}

	void DrawSample(const size_t kind, const LineStyle& style, const double width,
		const ColorF& color, const bool guides, const Vec2 origin, const double zoom = 1.0)
	{
		const Transformer2D transform{ Mat3x2::Scale(zoom).translated(origin) };
		if (guides) DrawGeometry(kind, LineStyle::Flat, 0.75, ColorF{ 0.70 });
		DrawGeometry(kind, style, width, color);
		if (guides)
		{
			Circle{ Starts[kind], 2 }.draw(ColorF{ 0.05, 0.55, 0.32 });
			if (kind < Ends.size()) Circle{ Ends[kind], 2 }.draw(ColorF{ 0.85, 0.25, 0.15 });
		}
	}
}

void Main()
{
	Window::SetTitle(U"LineStyle: Line and path comparison");
	Window::SetStyle(WindowStyle::Sizable);
	Window::Resize(1200, 780);
	Scene::Configure(SceneConfig::Fixed({ 1400, 900 }));
	Scene::SetBackground(ColorF{ 0.96, 0.97, 0.98 });
	const Font title{ 22, Typeface::Bold }, ui{ 16 }, small{ 14 };
	const ColorF text{ 0.15, 0.19, 0.25 }, muted{ 0.40, 0.44, 0.50 };
	double width = 6, phase = 0;
	size_t selected = 4;
	bool playing = false, translucent = true, guides = false, detail = false;
	const auto& args = System::GetCommandLineArgs();
	const bool capture = args.contains(U"--capture");
	detail = args.contains(U"--detail");
	for (const auto& arg : args)
	{
		for (size_t i = 0; i < Presets.size(); ++i)
			if (arg == Format(U"--preset=", Presets[i].name)) selected = i;
		if (arg.starts_with(U"--width="))
			if (const auto value = ParseOpt<double>(arg.substr(8)); value && std::isfinite(*value)) width = Clamp(*value, 1.0, 16.0);
		if (arg.starts_with(U"--phase="))
			if (const auto value = ParseOpt<double>(arg.substr(8)); value && std::isfinite(*value)) phase = std::fmod(*value, 20.0);
	}
	int32 frames = 0;
	while (System::Update())
	{
		if (capture && ScreenCapture::HasNewFrame())
		{
			const FilePath path = detail ? Format(U"line-style-", Presets[selected].name, U".png") : U"line-style-overview.png";
			const bool saved = ScreenCapture::GetFrame().save(path);
			System::Exit(saved ? 0 : 1);
			return;
		}
		if (KeySpace.down()) playing = !playing;
		if (KeyTab.down()) detail = !detail;
		if (KeyUp.down()) selected = (selected + Presets.size() - 1) % Presets.size();
		if (KeyDown.down()) selected = (selected + 1) % Presets.size();
		if (KeyW.down()) width = Min(width + 1, 16.0);
		if (KeyS.down()) width = Max(width - 1, 1.0);
		if (KeyA.down()) translucent = !translucent;
		if (KeyG.down()) guides = !guides;
		if (KeyR.down())
		{
			width = 6; phase = 0; playing = false; translucent = true; guides = false;
		}
		if (playing) phase += Scene::DeltaTime();
		const double step = KeyShift.pressed() ? 0.01 : 0.1;
		if (KeyLeft.down()) { phase -= step; playing = false; }
		if (KeyRight.down()) { phase += step; playing = false; }
		phase = std::fmod(phase, 20.0); // Common period of all five patterned presets.
		const ColorF ink{ 0.05, 0.35, 0.68, (translucent ? 0.5 : 1.0) };

		title(U"LineStyle / ", detail ? Presets[selected].name : U"all 13 presets").draw(16, 12, text);
		ui(U"width: {:.0f}   phase: {:.2f}   alpha: {:.1f}   {}"_fmt(
			width, phase, ink.a, playing ? U"playing" : U"paused")).draw(650, 18, text);
		if (not detail)
		{
			for (size_t c = 0; c < Titles.size(); ++c)
			{
				ui(Titles[c]).drawAt(266 + c * 172, 68, text);
				small(Subtitles[c]).drawAt(266 + c * 172, 86, muted);
			}
			for (size_t r = 0; r < Presets.size(); ++r)
			{
				const double y = (100 + r * 52);
				const RectF row{ 12, y, 1376, 50 };
				if (row.leftClicked()) selected = r;
				row.draw(r == selected ? ColorF{ 0.86, 0.92, 0.98 } : ColorF{ 1.0 });
				ui(Presets[r].name).draw(22, y + 15, text);
				const LineStyle style{ Presets[r].parameters(phase) };
				for (size_t c = 0; c < Titles.size(); ++c)
					DrawSample(c, style, width, ink, guides, { 200 + c * 172, y + 26 });
			}
		}
		else
		{
			ui(U"2.1x view / same source geometry, width and phase as the overview").draw(16, 68, muted);
			const LineStyle style{ Presets[selected].parameters(phase) };
			for (size_t c = 0; c < Titles.size(); ++c)
			{
				const Vec2 pos{ 16 + (c % 4) * 344.0, 110 + (c / 4) * 326.0 };
				RectF{ pos, 332, 306 }.draw(ColorF{ 1.0 });
				ui(Titles[c], U" / ", Subtitles[c]).draw(pos + Vec2{ 16, 16 }, text);
				DrawSample(c, style, width, ink, guides, pos + Vec2{ 27.4, 150 }, 2.1);
			}
			ui(U"Inspect endpoints and corners\nG: centerlines and endpoint markers\nA: reveal overlapping strokes\nLeft / Right: step phase while paused").draw(1064, 470, muted);
		}
		ui(U"Tab: overview / detail    Up / Down or click row: select    Space: play / pause    W / S: width").draw(16, 800, text);
		ui(U"Left / Right: phase 0.1 (Shift: 0.01)    A: alpha    G: guides    R: reset controls").draw(16, 830, text);
		small(U"Line and straight LineString share endpoints. Curved paths have different lengths. Guides: green = start, red = end.").draw(16, 864, muted);
		if (capture && (++frames == 4)) ScreenCapture::RequestCurrentFrame();
	}
}
```
