# LineStyle: Line, path and arc comparison

The canonical gallery for `draw(LineStyle, ...)` and
`LineString::drawClosed(LineStyle, ...)` compares every existing LineStyle preset
with the original Line renderer. The overview shows all 13 presets at once:
Square, Round, Flat, the five patterned presets, and their five Flat variants.
Press C to switch between paths and analytic arcs; F selects rounded frames. Each row keeps the preset's original cap; the program changes only its phase.

The path page has seven columns: Line, a collinear LineString, a polyline with right-angle
corners, Bezier2, Bezier3, Spline2D, and a closed LineString. The first two have
identical geometry, including endpoints; the others share their horizontal span
but have different path lengths. The arc page compares Line, an open Circle arc,
an open Ellipse arc, its reverse, full Circle and Ellipse arcs, and a flat Ellipse.
Use the 2.1x detail view to inspect a selected
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
| C | Toggle paths / analytic arcs |
| F | Toggle the rounded-frame page |
| H | Toggle the closure comparison page |
| P | Pause / resume sweep animation (350° through 362° and back) |
| J / K | Pause and step sweep by 1°; Shift: 0.01° |
| V | Reverse the sweep direction |
| O | Toggle variable / full-reference display in the enlarged seam card |
| I | Toggle symmetric / asymmetric arc/frame widths (inner 25%, outer 75%) |
| Tab | Toggle overview / selected-preset detail |
| Up / Down | Select the previous / next preset; clicking a row also selects it |
| Space | Pause / resume phase animation |
| Left / Right | Pause and step phase by 0.1; hold Shift for 0.01 |
| W / S | Increase / decrease width within 1–16 source units |
| A | Toggle alpha 0.5 / 1 |
| G | Toggle gray centerlines, green starts, and red ends; closed paths have only a start marker |
| R | Reset width, phase, opacity, guides and animation; retain the selected preset and view |

Optional command-line arguments:

- `--frames` selects rounded frames, including radius 0, a small radius, capsules, a circle and a narrow frame.
- `--arcs` selects the analytic arc page; `--asymmetric` selects unequal inner/outer widths.
- `--detail` starts in the enlarged view.
- `--closure` compares a variable sweep with a fixed 360° reference and a 4x seam view.
  `--sweep=359.9`, `--sweep-play`, and `--reverse` set its initial angle, animation and direction.
- `--preset=RoundDotFlat` selects a preset by its exact name.
- `--width=12` and `--phase=1.25` set the starting width and phase. Width is
  clamped to 1–16; phase wraps by 20, a common period of all five patterns.
- `--capture` saves the selected view after four frames and exits. Images are
  `line-style-overview.png` or `line-style-<preset>.png` in the application's
  working directory; arc-page names include `arcs-`, frame-page names include `frames-`; closure captures include `closure-<angle>-<preset>`, and unequal-width arc/frame captures include `asymmetric-`. Repeated captures of the same view overwrite that file.
  Remove retained images when no longer needed; avoid input during capture.

## Expected results

- Line and the straight LineString should agree on visible intervals and phase.
  Look at endpoints in the non-Flat and Flat rows. Cut-edge antialiasing can differ
  between the Line shader and the path's geometric cuts.
- Square, Round and Flat are solid styles and ignore phase. Their open endpoints
  differ; closed LineStrings have no endpoint caps. Solid arcs retain ordinary
  `drawArc()` behavior, including the existing Round circle cap overlap visible
  at the full-circle seam.
- On the path page, patterned non-Flat presets extend the whole open path by half the width. Each
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

- Arc dashes use analytic normals, without polyline joins. Circle spacing uses
  exact central arc length; ellipse spacing uses an approximate length table.
  Reverse arcs advance the pattern from their own start. Every patterned arc
  fits an integer number of periods to its full circumference, regardless of the
  displayed sweep. Round dots keep their diameter. Non-Flat extensions shorten
  as the uncovered gap closes; no phase shift occurs at 360°.
- On the H page, pause phase animation (Space), then press P to animate the sweep.
  Inspect 359.99° → 360° → 360.01° and the reverse direction (V), all ten patterned
  presets, phase -0.5 (a dot centered on the seam), width 16 and asymmetric widths.
  Existing dots and dashes should remain stationary. The last cut closes without
  a global hop or a darker duplicate of the seam dot. The right card enlarges
  the seam; O alternates its variable sweep and full-circle reference.
  Solid styles retain ordinary cap behavior and are not period-fitted.
- Use the flat ellipse and I at width 12–16 to inspect tight-curvature limits.
  Its inward offset can fold over, and neighboring round dots on tight bends can
  overlap. Compare alpha 0.5 and 1. Gray guides show the reference curve; green
  and red markers show its angular endpoints, before the asymmetric width shift.

On the F page, all patterned frames fit their period to the full midpoint
perimeter. Phase advances clockwise from the upper-left corner's top contact.
Space animates phase through straight/arc boundaries; compare Flat/non-Flat
variants and phase -0.5 at alpha 0.5 to inspect the seam. I changes the width
offset. Round dots keep their diameter and can overlap at small radii. A frame
whose inner rectangle collapses fills its outer shape, matching ordinary frames.
The separate count/ratio-based `drawDashedFrame()` API keeps its existing behavior.

See the [drawing guide](../../docs/geometry2d/dashed-drawing.md) for design and
measurement details. The [automated tests](../Test_DashedPath.cpp) cover placement,
boundaries, rendering and CPU submission. [Arc tests](../Test_StyledArc.cpp) also
check analytic placement, ellipse length accuracy, upload bounds and public rendering.
[Frame tests](../Test_StyledRoundRect.cpp) cover whole-perimeter placement, joins,
round dots, collapsed interiors, buffer bounds and public rendering. To run correctness tests on macOS:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=~[benchmark]DashedPath.*,~[benchmark]StyledArc.*,~[benchmark]StyledRoundRect.*'
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

	constexpr std::array<StringView, 7> ArcTitles{
		U"Line", U"Circle", U"Ellipse", U"Ellipse", U"Circle", U"Ellipse", U"Ellipse"
	};
	constexpr std::array<StringView, 7> ArcSubtitles{
		U"reference", U"open arc", U"open arc", U"reverse arc", U"full arc", U"full arc", U"flat / full"
	};

	Vec2 ArcEndpoint(const size_t kind, const bool end)
	{
		if (kind == 0) return { end ? 132.0 : 0.0, 0 };
		if (kind == 1)
		{
			const double t = end ? 40_deg : -40_deg;
			return Vec2{ 66, 90 } + Vec2{ std::sin(t), -std::cos(t) } * 100;
		}
		if (kind >= 4) return { 66, kind == 6 ? -5.0 : -18.0 };
		const double t = (end != (kind == 3)) ? 80_deg : -80_deg;
		const Vec2 direction{ std::sin(t), -std::cos(t) };
		return Vec2{ 66, 18 } + direction * (64 * 32 / std::hypot(32 * direction.x, 64 * direction.y));
	}

	void DrawArcGeometry(const size_t kind, const LineStyle& style, const double width, const ColorF& color, const bool asymmetric)
	{
		const double inner = width * (asymmetric ? 0.25 : 0.5), outer = (width - inner);
		switch (kind)
		{
		case 0: Line{ 0, 0, 132, 0 }.draw(style, width, color); break;
		case 1: Circle{ 66, 90, 100 }.drawArc(style, -40_deg, 80_deg, inner, outer, color); break;
		case 2: Ellipse{ 66, 18, 64, 32 }.drawArc(style, -80_deg, 160_deg, inner, outer, color); break;
		case 3: Ellipse{ 66, 18, 64, 32 }.drawArc(style, 80_deg, -160_deg, inner, outer, color); break;
		case 4: Circle{ 66, 0, 18 }.drawArc(style, 0, 360_deg, inner, outer, color); break;
		case 5: Ellipse{ 66, 0, 58, 18 }.drawArc(style, 0, 360_deg, inner, outer, color); break;
		case 6: Ellipse{ 66, 0, 60, 5 }.drawArc(style, 0, 360_deg, inner, outer, color); break;
		}
	}

	constexpr std::array<StringView, 7> FrameTitles{
		U"Line", U"RoundRect", U"RoundRect", U"RoundRect", U"RoundRect", U"RoundRect", U"RoundRect"
	};
	constexpr std::array<StringView, 7> FrameSubtitles{
		U"reference", U"radius 12", U"capsule", U"radius 0", U"radius 2", U"circle limit", U"narrow / thick"
	};

	RoundRect FrameShape(const size_t kind)
	{
		if (kind == 5) return RoundRect{ 48, -18, 36, 36, 18 };
		if (kind == 6) return RoundRect{ 0, -4, 132, 8, 4 };
		return RoundRect{ 0, -18, 132, 36, kind == 1 ? 12.0 : kind == 2 ? 18.0 : kind == 3 ? 0.0 : 2.0 };
	}

	void DrawFrameGeometry(const size_t kind, const LineStyle& style, const double width, const ColorF& color, const bool asymmetric)
	{
		if (kind == 0) Line{ 0, 0, 132, 0 }.draw(style, width, color);
		else
		{
			const double inner = width * (asymmetric ? 0.25 : 0.5);
			FrameShape(kind).drawFrame(style, inner, (width - inner), color);
		}
	}

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
		const ColorF& color, const bool guides, const Vec2 origin, const bool arcs, const bool asymmetric, const bool roundFrames, const double zoom = 1.0)
	{
		const Transformer2D transform{ Mat3x2::Scale(zoom).translated(origin) };
		if (roundFrames)
		{
			if (guides) DrawFrameGeometry(kind, LineStyle::Flat, 0.75, ColorF{ 0.70 }, false);
			DrawFrameGeometry(kind, style, width, color, asymmetric);
		}
		else if (arcs)
		{
			if (guides) DrawArcGeometry(kind, LineStyle::Flat, 0.75, ColorF{ 0.70 }, false);
			DrawArcGeometry(kind, style, width, color, asymmetric);
		}
		else
		{
			if (guides) DrawGeometry(kind, LineStyle::Flat, 0.75, ColorF{ 0.70 });
			DrawGeometry(kind, style, width, color);
		}
		if (guides && roundFrames)
		{
			const auto rect = FrameShape(kind);
			Circle{ kind == 0 ? Vec2{ 0, 0 } : rect.rect.tl() + Vec2{ rect.r, 0 }, 2 }.draw(ColorF{ 0.05, 0.55, 0.32 });
		}
		else if (guides)
		{
			Circle{ arcs ? ArcEndpoint(kind, false) : Starts[kind], 2 }.draw(ColorF{ 0.05, 0.55, 0.32 });
			if (arcs ? kind < 4 : kind < Ends.size()) Circle{ arcs ? ArcEndpoint(kind, true) : Ends[kind], 2 }.draw(ColorF{ 0.85, 0.25, 0.15 });
		}
	}

	void DrawClosure(const Font& ui, const Font& small, const LineStyle& style, const double width,
		const ColorF ink, const double degrees, const bool reverse, const bool asymmetric, const bool guides, const bool referenceZoom)
	{
		const ColorF text{ 0.15, 0.19, 0.25 };
		const double start = 35_deg, direction = (reverse ? -1.0 : 1.0);
		const double inner = width * (asymmetric ? 0.25 : 0.5), outer = (width - inner);
		for (int32 row = 0; row < 2; ++row)
		for (int32 column = 0; column < 3; ++column)
		{
			const Vec2 pos{ 16 + column * 460.0, 110 + row * 330.0 };
			RectF{ pos, 448, 310 }.draw(ColorF{ 1.0 });
			const bool full = (column == 1 || (column == 2 && referenceZoom));
			const double sweep = direction * (full ? 360_deg : Math::ToRadians(degrees));
			ui(row ? U"Ellipse" : U"Circle", U" / ", full ? U"360.00 deg reference" : U"{:.2f} deg"_fmt(degrees)).draw(pos + Vec2{ 12, 10 }, text);
			if (column == 2) small(U"4x seam / O: alternate with reference").draw(pos + Vec2{ 12, 36 }, text);
			const Ellipse ellipse{ 0, 0, row ? 130.0 : 85.0, row ? 65.0 : 85.0 };
			const Vec2 radial{ std::sin(start), -std::cos(start) };
			const Vec2 endpoint = radial * (ellipse.a * ellipse.b / std::hypot(ellipse.b * radial.x, ellipse.a * radial.y));
			const double zoom = (column == 2 ? 4.0 : 1.0);
			const Vec2 origin = pos + Vec2{ 224, 175 } - (column == 2 ? endpoint * zoom : Vec2{ 0, 0 });
			const ScopedScissorRect2D clip{ Rect{ static_cast<int32>(pos.x), static_cast<int32>(pos.y + 60), 448, 244 } };
			const Transformer2D transform{ Mat3x2::Scale(zoom).translated(origin) };
			if (guides) ellipse.drawArc(0, 360_deg, 0, 0.5, ColorF{ 0.75 });
			if (row) ellipse.drawArc(style, start, sweep, inner, outer, ink);
			else Circle{ 0, 0, ellipse.a }.drawArc(style, start, sweep, inner, outer, ink);
			if (guides) Circle{ endpoint, 1.5 }.draw(ColorF{ 0.05, 0.55, 0.32 });
		}
	}

}

void Main()
{
	Window::SetTitle(U"LineStyle: Line, path and arc comparison");
	Window::SetStyle(WindowStyle::Sizable);
	Window::Resize(1200, 780);
	Scene::Configure(SceneConfig::Fixed({ 1400, 900 }));
	Scene::SetBackground(ColorF{ 0.96, 0.97, 0.98 });
	const Font title{ 22, Typeface::Bold }, ui{ 16 }, small{ 14 };
	const ColorF text{ 0.15, 0.19, 0.25 }, muted{ 0.40, 0.44, 0.50 };
	double width = 6, phase = 0, sweepDegrees = 359.9, sweepTime = 0;
	size_t selected = 4;
	bool playing = false, translucent = true, guides = false, detail = false;
	const auto& args = System::GetCommandLineArgs();
	const bool capture = args.contains(U"--capture");
	bool roundFrames = args.contains(U"--frames");
	bool arcs = args.contains(U"--arcs"), asymmetric = args.contains(U"--asymmetric");
	detail = args.contains(U"--detail");
	bool closure = args.contains(U"--closure"), sweepPlaying = args.contains(U"--sweep-play");
	bool reverse = args.contains(U"--reverse"), referenceZoom = false;
	for (const auto& arg : args)
	{
		for (size_t i = 0; i < Presets.size(); ++i)
			if (arg == Format(U"--preset=", Presets[i].name)) selected = i;
		if (arg.starts_with(U"--width="))
			if (const auto value = ParseOpt<double>(arg.substr(8)); value && std::isfinite(*value)) width = Clamp(*value, 1.0, 16.0);
		if (arg.starts_with(U"--sweep="))
			if (const auto value = ParseOpt<double>(arg.substr(8)); value && std::isfinite(*value)) sweepDegrees = Clamp(*value, 0.0, 362.0);
		if (arg.starts_with(U"--phase="))
			if (const auto value = ParseOpt<double>(arg.substr(8)); value && std::isfinite(*value)) phase = std::fmod(*value, 20.0);
	}
	sweepTime = std::asin(Clamp((sweepDegrees - 356) / 6, -1.0, 1.0)) / 0.8;
	int32 frames = 0;
	while (System::Update())
	{
		if (capture && ScreenCapture::HasNewFrame())
		{
			const FilePath path = closure ? Format(U"line-style-closure-", sweepDegrees, U"-", Presets[selected].name, U".png") : Format(U"line-style-", roundFrames ? U"frames-" : arcs ? U"arcs-" : U"", asymmetric ? U"asymmetric-" : U"", detail ? Presets[selected].name : U"overview", U".png");
			const bool saved = ScreenCapture::GetFrame().save(path);
			System::Exit(saved ? 0 : 1);
			return;
		}
		if (KeySpace.down()) playing = !playing;
		if (KeyTab.down()) detail = !detail;
		if (KeyC.down()) { arcs = !arcs; closure = false; roundFrames = false; }
		if (KeyF.down()) { roundFrames = !roundFrames; closure = false; }
		if (KeyH.down()) { closure = !closure; roundFrames = false; }
		if (KeyP.down())
		{
			sweepPlaying = !sweepPlaying;
			if (sweepPlaying) sweepTime = std::asin(Clamp((sweepDegrees - 356) / 6, -1.0, 1.0)) / 0.8;
		}
		if (KeyV.down()) reverse = !reverse;
		if (KeyO.down()) referenceZoom = !referenceZoom;
		const double angleStep = KeyShift.pressed() ? 0.01 : 1.0;
		if (KeyJ.down()) { sweepDegrees = Max(0.0, sweepDegrees - angleStep); sweepPlaying = false; }
		if (KeyK.down()) { sweepDegrees = Min(362.0, sweepDegrees + angleStep); sweepPlaying = false; }
		if (sweepPlaying) { sweepTime += Scene::DeltaTime(); sweepDegrees = 356 + 6 * std::sin(sweepTime * 0.8); }
		if (KeyI.down()) asymmetric = !asymmetric;
		if (KeyUp.down()) selected = (selected + Presets.size() - 1) % Presets.size();
		if (KeyDown.down()) selected = (selected + 1) % Presets.size();
		if (KeyW.down()) width = Min(width + 1, 16.0);
		if (KeyS.down()) width = Max(width - 1, 1.0);
		if (KeyA.down()) translucent = !translucent;
		if (KeyG.down()) guides = !guides;
		if (KeyR.down())
		{
			width = 6; phase = 0; playing = false; translucent = true; guides = false; asymmetric = false; sweepPlaying = false; sweepDegrees = 359.9; sweepTime = 0;
		}
		if (playing) phase += Scene::DeltaTime();
		const double step = KeyShift.pressed() ? 0.01 : 0.1;
		if (KeyLeft.down()) { phase -= step; playing = false; }
		if (KeyRight.down()) { phase += step; playing = false; }
		phase = std::fmod(phase, 20.0); // Common period of all five patterned presets.
		const ColorF ink{ 0.05, 0.35, 0.68, (translucent ? 0.5 : 1.0) };

		const auto& titles = roundFrames ? FrameTitles : arcs ? ArcTitles : Titles;
		const auto& subtitles = roundFrames ? FrameSubtitles : arcs ? ArcSubtitles : Subtitles;
		title(closure ? U"Arc closure / " : roundFrames ? U"Rounded frames / " : arcs ? U"LineStyle arcs / " : U"LineStyle paths / ", (detail || closure) ? Presets[selected].name : U"all 13 presets").draw(16, 12, text);
		ui(U"width: {:.0f}   phase: {:.2f}   alpha: {:.1f}   {}"_fmt(
			width, phase, ink.a, playing ? U"playing" : U"paused")).draw(650, 18, text);
		if (closure)
		{
			ui(U"P: sweep animation   J / K: angle (Shift: fine)   V: reverse   O: seam reference   {:.2f} deg / {}"_fmt(
				sweepDegrees, reverse ? U"CCW" : U"CW")).draw(16, 68, text);
			DrawClosure(ui, small, Presets[selected].parameters(phase), width, ink, sweepDegrees, reverse, asymmetric, guides, referenceZoom);
		}
		else if (not detail)
		{
			for (size_t c = 0; c < Titles.size(); ++c)
			{
				ui(titles[c]).drawAt(266 + c * 172, 68, text);
				small(subtitles[c]).drawAt(266 + c * 172, 86, muted);
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
					DrawSample(c, style, width, ink, guides, { 200 + c * 172, y + 26 }, arcs, asymmetric, roundFrames);
			}
		}
		else
		{
			ui(asymmetric && (arcs || roundFrames) ? U"2.1x view / inner 25%, outer 75%" : U"2.1x view / symmetric widths").draw(16, 68, muted);
			const LineStyle style{ Presets[selected].parameters(phase) };
			for (size_t c = 0; c < Titles.size(); ++c)
			{
				const Vec2 pos{ 16 + (c % 4) * 344.0, 110 + (c / 4) * 326.0 };
				RectF{ pos, 332, 306 }.draw(ColorF{ 1.0 });
				ui(titles[c], U" / ", subtitles[c]).draw(pos + Vec2{ 16, 16 }, text);
				DrawSample(c, style, width, ink, guides, pos + Vec2{ 27.4, 150 }, arcs, asymmetric, roundFrames, 2.1);
			}
			ui(U"Inspect endpoints and corners\nG: centerlines and endpoint markers\nA: reveal overlapping strokes\nLeft / Right: step phase while paused").draw(1064, 470, muted);
		}
		ui(U"H: closure comparison    Tab: overview / detail    Up / Down: preset    Space: phase animation    W / S: width").draw(16, 800, text);
		ui(U"Left / Right: phase 0.1 (Shift: 0.01)    A: alpha    G: guides    C: paths / arcs    F: frames    I: inner / outer widths    R: reset").draw(16, 830, text);
		small(closure ? U"Sweep changes reveal a fixed full-circumference pattern. Hold phase still to inspect closure; Solid keeps ordinary caps."
			: roundFrames ? U"Rounded frames fit the full perimeter. Cap is ignored. Narrow frames fill when the interior collapses."
			: arcs ? U"Analytic arcs / different path lengths. All sweeps fit their period to the full circumference. Flat ellipse shows tight-curvature limits."
			: U"Line and straight LineString share endpoints. Curved paths have different lengths. Guides: green = start, red = end.").draw(16, 864, muted);
		if (capture && (++frames == 4)) ScreenCapture::RequestCurrentFrame();
	}
}
```
