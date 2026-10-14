# Shape Image writing gallery

This program compares CPU `paint` and `overwrite` results with GPU `draw` for
fractional, translucent geometry. It also displays CPU alpha as grayscale so that
duplicate compositing and unexpected seams remain visible on transparent images.
The two rasterizers use different AA approximations; pixel identity is not the
acceptance criterion.

## Run

1. Use the complete program below in a separate Siv3D application.
2. Press **A** to toggle CPU antialiasing. Press **Space** to cycle the
   SuperEllipse exponent through 0.5, 1, 2, 4, and 16. Press **F** to cycle its
   fill, thin frame, and wide frame. Press **P** to switch the Ellipse column
   between its frame, minor pie, major pie, negative pie, and full pie. Press **C**
   to switch Circle between fill, shallow segment, semicircle, major segment,
   negative-angle segment, full segment, and arc. In arc mode, press **L** to
   cycle Flat / Round / Square and **R** to cycle short / major / negative / zero /
   full sweeps. Press **T** to cycle subpixel / thin / wide arcs. For Circle frame
   checks, select arc mode and a full sweep, then vary **A** and **T**.
   Press **S** to cycle the shape, stroke, arrow, spline, polygon-frame, and centered-join pages. The stroke page has
   Line, LineString, Bezier2, Bezier3, and a short Line. On this page, **J** and **K** independently cycle the
   start and end caps through Flat / Round / Square. **D** changes the last Line
   length between zero, 1.5, and 20 pixels. **Q** cycles Bezier quality through
   0.25 / 0.5 / 1.5 pixels; **B** switches its budget between 256 and four segments.
   The GPU Bezier columns use `drawAdaptive` with the same numeric settings.
   On the arrow page, **J** selects the
   tail cap, **T** cycles the shaft width, and **D** changes the short arrow.
   The columns show horizontal, diagonal, short, clipped, and zero-length arrows.
   On the spline page, **J** / **K** select CPU end caps, **T** changes width,
   **Q** changes quality, and **B** switches between 256 and one segment per cubic.
   Its columns are open, closed, self-crossed with a collapsed interval, clipped,
   and all-coincident paths. Native GPU Spline drawing retains Square ends;
   **J** / **K** do not affect that row.
   On the polygon-frame page, **F** selects balanced / inward / outward widths,
   **T** cycles total width through 0.5 / 8 / 80 pixels, **B** reverses winding,
   and **J** selects Bevel / Round / Miter.
   Columns show Triangle, a clipped convex Quad, concave Quad, crossed Quad, and
   a collinear Quad; the last column must remain empty for both rasterizers.
   On the centered-join page, **J** cycles Bevel / Round / Miter, **T**
   changes width, and **B** reverses winding. The first column is an acute Triangle;
   the remaining columns reuse the Quads from the polygon-frame page.
3. Inspect the first row (`paint`), second row (`overwrite`), third row (paint
   alpha), fourth row (overwrite alpha), and the GPU row at the bottom.
4. Record the platform, exponent, shape modes, AA setting, and observed result. The program
   writes no files and does not replace the repository test application's Main.

## Expected results

- Bezier quality and budget controls affect both paint and overwrite, with the
  same centerline subdivision. Four segments deliberately produces facets.
  Image quality is in image pixels, while GPU quality includes render resolution;
  compare at scene-size resolution when inspecting the same numeric target.

- AA edges have intermediate coverage; disabling AA removes intermediate coverage
  from these shape operations. RectF has no AA toggle and is not in this gallery.
- Translucent circle fills and thin rounded rectangles have no darker bands.
  Circle segments have a straight chord and a circular arc. Shallow segments
  exclude the center; major segments include it. Height-based segments point
  30 degrees clockwise from up. The negative-angle segment follows a -4.5-radian
  arc from that direction. Full segments match ordinary circle fills. Check
  chord/arc intersections for missing pixels or repeated compositing.
- Circle arcs retain Flat ends unless a cap is selected. Round ends are circular;
  Square ends extend along the tangent by half the visible radial width. On short
  and almost-full arcs, overlapping CPU caps must not become darker. Zero-angle
  Round and Square arcs draw the cap alone; full CPU arcs match circle frames.
  GPU Square arcs also extend along the tangent, draw a square at zero angle, and
  omit caps on full sweeps. GPU caps may darken overlaps.
- With AA enabled and a full Circle sweep, inspect both edges at the top,
  bottom, left, and right. Subpixel frames remain partially covered; wide frames
  have a solid band between the AA edges. The hole preserves its background.
- The ellipse frame is continuous, including along the long axis. Its pies use
  angles measured from the center in image coordinates: zero points up and
  positive angles turn clockwise. The first ray is 30 degrees clockwise from up.
  Negative sweeps turn counterclockwise. Major pies have no darker band where
  their regions meet; full CPU and GPU pies each match their regular ellipse fills.
- On the shape page, the last column has a Polygon with a hole and overlapping wide frames. The CPU
  combines the frames before applying color; the overlap must not become darker.
  The GPU may composite the contours separately.
- SuperEllipse changes from concave to diamond to ellipse to a rounded box.
  Its image fill at exponent 2 matches Ellipse. Other fills use 4x4 samples,
  so very small features can disappear between sample positions. Frames are
  centered on an approximation of the boundary and have rounded corners. Check
  horizontal/vertical symmetry and that wide concave frames have no darker overlaps.
- On the stroke page, changing one cap changes only that end. Square extends
  along the tangent; Round adds a rounded end. CPU intersections composite once.
  A short Round/Flat Line keeps its Flat end even when the round cap is wider
  than the line is long. The zero-length CPU Line draws a centered square if either cap is Square, a
  disk if either remaining cap is Round, and nothing for Flat/Flat. GPU collapsed
  caps may differ because they retain a start/end orientation.
- On the spline page, open endpoints use the selected caps. Closed paths ignore
  caps and join the closing seam. The collapsed interval at the crossing path's
  knot must not add a cap or dark spot. CPU knots, intersections, and the seam
  composite once; inspect both alpha rows. A budget of one retains internal knots
  and draws their chords. The all-coincident CPU path draws nothing for all caps.
  GPU rendering can differ at joins, intersections, and degenerate paths.
- On the arrow page, Flat ends at the starting point, Square extends backward by
  half the shaft width, and Round adds an outward semicircle. The tip stays sharp
  even for short arrows with wide shafts. The head shrinks proportionally when it
  is longer than the line. Check the tail/body and shaft/head junctions for seams
  and darkening in both alpha rows. Zero-length CPU arrows draw nothing for every
  cap; GPU Round arrows draw a disk at that point.
- On the polygon-frame page, inward-only bands remain inside the original
  contour except for AA at the boundary; outward-only bands remain outside.
  The selected corners and crossed lobes must not darken at overlaps. Wide
  inward bands consume the interior without flipping it or creating new islands.
  **B** preserves both filled regions. The collinear contour draws nothing.
  CPU AA and GPU boundary sampling can differ; this page is not a pixel-parity check.
- Paint raises background alpha. Overwrite replaces alpha in fully covered
  pixels. Both preserve pixels outside the shape. The grayscale views distinguish
  these effects from ordinary display blending.
- The GPU row has the same geometry and color intent; edge coverage and frame
  joins may differ. See the [API inventory](../../docs/geometry2d/image-writing.md)
  for differences that require separate API work.

- On the centered-join page, Bevel clips the acute tip and Miter extends it;
  Round is curved. Omitting the style selects Bevel for both CPU and GPU. Inspect both alpha rows: crossed
  edges and thick frames must not darken at overlaps, and reversing winding must
  preserve the image. The collinear CPU frame has flat extremities for Bevel/Miter
  and rounded extremities for Round. CPU AA need not match GPU samples.

## Complete program

```cpp
# include <Siv3D.hpp>

void Main()
{
	Window::Resize(1200, 840);
	Scene::SetBackground(ColorF{ 0.16 });
	const Font font{ 18 };
	const Color ink{ 220, 60, 130, 113 };
	const Array<double> exponents{ 0.5, 1, 2, 4, 16 };
	size_t exponentIndex = 3;
	int32 superEllipseMode = 0;
	const Array<String> modes{ U"fill", U"thin frame", U"wide frame" };
	int32 circleMode = 0;
	const Array<String> circleModes{ U"fill", U"shallow segment", U"semicircle", U"major segment", U"negative segment", U"full segment", U"arc" };
	const Array<LineCap> arcCaps{ LineCap::Flat, LineCap::Round, LineCap::Square };
	const Array<double> arcSweeps{ 0.08, Math::TwoPi - 0.08, -4.5, 0, Math::TwoPi };
	size_t arcCapIndex = 0, arcSweepIndex = 0;
	const Array<Vec2> arcThicknesses{ Vec2{ 0.15, 0.35 }, Vec2{ 2, 3 }, Vec2{ 24, 10 } };
	const Array<String> arcWidths{ U"subpixel", U"thin", U"wide" };
	size_t arcThicknessIndex = 1;
	const Array<double> heights{ 0, 0.2, 1.0, 1.8 };
	int32 ellipseMode = 0;
	const Array<String> ellipseModes{ U"frame", U"minor pie", U"major pie", U"negative pie", U"full pie" };
	const Array<double> sweeps{ 0, Math::Pi / 3, Math::Pi * 1.5, -Math::Pi * 1.5, Math::TwoPi };
	constexpr double PieStart = Math::Pi / 6;
	int32 page = 0;
	const Array<String> pages{ U"shapes", U"strokes", U"arrows", U"splines", U"polygon frames", U"centered joins" };
	const Array<JoinStyle> joins{ JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter };
	size_t joinIndex = 0;
	const Array<double> arrowWidths{ 0.6, 6.5, 24.5 };
	size_t startCapIndex = 0, endCapIndex = 1;
	const Line strokeLine{ 30.25, 30.75, 160.25, 96.75 };
	const LineString strokePath{ Vec2{ 30.25, 30.75 }, Vec2{ 160.25, 96.75 }, Vec2{ 30.25, 96.75 }, Vec2{ 160.25, 30.75 } };
	const Bezier2 quadratic{ Vec2{ 30.25, 96.75 }, Vec2{ 96.25, -20.75 }, Vec2{ 160.25, 96.75 } };
	const Bezier3 cubic{ Vec2{ 30.25, 96.75 }, Vec2{ 170.25, -20.75 }, Vec2{ 20.25, -20.75 }, Vec2{ 160.25, 96.75 } };
	const Array<double> shortLengths{ 0.0, 1.5, 20.0 };
	size_t shortLengthIndex = 0;
	const Array<double> curveQuality{ 0.25, 0.5, 1.5 };
	size_t curveQualityIndex = 1;
	bool curveLimited = false;
	const auto shortLine = [&] { return Line{ 96.25, 64.75, 96.25 + shortLengths[shortLengthIndex], 64.75 }; };
	const auto arrowLine = [&](const int32 kind)
	{
		if (kind == 0) return Line{ 30.25, 64.75, 160.25, 64.75 };
		if (kind == 1) return strokeLine;
		if (kind == 2) return shortLine();
		if (kind == 3) return Line{ -4.25, 30.75, 160.25, 96.75 };
		return Line{ 96.25, 64.75, 96.25, 64.75 };
	};
	const auto straight = [](const Vec2 a, const Vec2 b)
	{
		return Bezier3::FromHermite(a, b - a, b, b - a);
	};
	const Array<Vec2> splineKnots{ { 24.25, 90.75 }, { 60.25, 24.75 }, { 120.25, 96.75 }, { 168.25, 36.75 } };
	const Array<Spline2D> splines{
		Spline2D::FromCatmullRom(splineKnots),
		Spline2D::FromCatmullRom(splineKnots, CloseRing::Yes),
		Spline2D::FromBezierSegments(Array<Bezier3>{
			straight({ 30.25, 30.75 }, { 160.25, 96.75 }),
			straight({ 160.25, 96.75 }, { 160.25, 96.75 }),
			straight({ 160.25, 96.75 }, { 30.25, 96.75 }),
			straight({ 30.25, 96.75 }, { 160.25, 30.75 }),
			straight({ 160.25, 30.75 }, { 30.25, 30.75 }) }, CloseRing::Yes),
		Spline2D::FromCatmullRom(Array<Vec2>{ { -12.25, 24.75 }, { 40.25, 100.75 }, { 100.25, 16.75 }, { 200.25, 90.75 } }),
		Spline2D::FromBezierSegments(Array<Bezier3>{ straight({ 96.25, 64.75 }, { 96.25, 64.75 }) }) };
	const Array<Quad> frameQuads{
		Quad{ -8.25, 18.75, 164.25, 28.75, 172.25, 106.75, 20.25, 100.75 },
		Quad{ 24.25, 18.75, 168.25, 24.75, 84.25, 58.75, 40.25, 110.75 },
		Quad{ 24.25, 18.75, 168.25, 106.75, 24.25, 106.75, 168.25, 18.75 },
		Quad{ 24.25, 64.75, 72.25, 64.75, 168.25, 64.75, 120.25, 64.75 } };
	const auto frameTriangle = [&]
	{
		const Triangle t = (page == 5) ? Triangle{ 74.25, 106.75, 96.25, 18.75, 118.25, 106.75 }
			: Triangle{ 24.25, 106.75, 96.25, 18.75, 168.25, 106.75 };
		return curveLimited ? Triangle{ t.p2, t.p1, t.p0 } : t;
	};
	const auto frameQuad = [&](int32 kind)
	{
		const Quad q = frameQuads[kind - 1];
		return curveLimited ? Quad{ q.p3, q.p2, q.p1, q.p0 } : q;
	};
	const auto frameWidths = [&]
	{
		const double width = std::array<double, 3>{ 0.5, 8, 80 }[arcThicknessIndex];
		return superEllipseMode == 1 ? Vec2{ width, 0 } : (superEllipseMode == 2 ? Vec2{ 0, width } : Vec2{ width / 2, width / 2 });
	};
	bool aa = true;
	bool dirty = true;
	Array<Texture> textures;
	const Polygon holed{ RectF{ 20.25, 16.75, 152, 96 }.outer(),
		{ RectF{ 31.25, 27.75, 130, 74 }.outer().reversed() } };

	const auto cpu = [&](Image& image, const int32 kind, const bool paint)
	{
		const EnableAntialiasing antialiasing{ aa };
		if (page == 5)
		{
			const double width = std::array<double, 3>{ 0.5, 8, 80 }[arcThicknessIndex];
			const auto draw = [&](const auto& shape)
			{
				if (paint) shape.paintFrame(image, joins[joinIndex], width, ink, antialiasing);
				else shape.overwriteFrame(image, joins[joinIndex], width, ink, antialiasing);
			};
			if (kind == 0) draw(frameTriangle());
			else draw(frameQuad(kind));
			return;
		}
		if (page == 4)
		{
			const auto [inner, outer] = frameWidths();
			const auto draw = [&](const auto& shape)
			{
				if (paint) shape.paintFrame(image, joins[joinIndex], inner, outer, ink, antialiasing);
				else shape.overwriteFrame(image, joins[joinIndex], inner, outer, ink, antialiasing);
			};
			if (kind == 0) draw(frameTriangle());
			else draw(frameQuad(kind));
			return;
		}
		if (page == 3)
		{
			const auto& path = splines[kind];
			const double width = arrowWidths[arcThicknessIndex];
			if (paint) path.paint(image, arcCaps[startCapIndex], arcCaps[endCapIndex], width, ink,
				antialiasing, curveQuality[curveQualityIndex], curveLimited ? 1 : 256);
			else path.overwrite(image, arcCaps[startCapIndex], arcCaps[endCapIndex], width, ink,
				antialiasing, curveQuality[curveQualityIndex], curveLimited ? 1 : 256);
			return;
		}
		if (page == 2)
		{
			const Line line = arrowLine(kind);
			const double width = arrowWidths[arcThicknessIndex];
			if (paint) line.paintArrow(image, arcCaps[startCapIndex], width, SizeF{ 30, 36 }, ink, antialiasing);
			else line.overwriteArrow(image, arcCaps[startCapIndex], width, SizeF{ 30, 36 }, ink, antialiasing);
			return;
		}
		if (page == 1)
		{
			const auto draw = [&](const auto& shape)
			{
				if (paint) shape.paint(image, arcCaps[startCapIndex], arcCaps[endCapIndex], 18.5, ink, antialiasing);
				else shape.overwrite(image, arcCaps[startCapIndex], arcCaps[endCapIndex], 18.5, ink, antialiasing);
			};
			const auto drawCurve = [&](const auto& shape)
			{
				if (paint) shape.paint(image, arcCaps[startCapIndex], arcCaps[endCapIndex], 18.5, ink,
					antialiasing, curveQuality[curveQualityIndex], curveLimited ? 4 : 256);
				else shape.overwrite(image, arcCaps[startCapIndex], arcCaps[endCapIndex], 18.5, ink,
					antialiasing, curveQuality[curveQualityIndex], curveLimited ? 4 : 256);
			};
			if (kind == 0) draw(strokeLine);
			else if (kind == 1) draw(strokePath);
			else if (kind == 2) drawCurve(quadratic);
			else if (kind == 3) drawCurve(cubic);
			else draw(shortLine());
			return;
		}
		if (kind == 0)
		{
			const Circle shape{ 96.25, 64.75, 49.5 };
			if (circleMode == 0)
			{
				if (paint) shape.paint(image, ink, antialiasing);
				else shape.overwrite(image, ink, antialiasing);
			}
			else if (circleMode < 4)
			{
				if (paint) shape.paintSegment(image, PieStart, shape.r * heights[circleMode], ink, antialiasing);
				else shape.overwriteSegment(image, PieStart, shape.r * heights[circleMode], ink, antialiasing);
			}
			else if (circleMode < 6)
			{
				const double sweep = (circleMode == 4 ? -4.5 : -Math::TwoPi);
				if (paint) shape.paintSegmentFromAngles(image, PieStart, sweep, ink, antialiasing);
				else shape.overwriteSegmentFromAngles(image, PieStart, sweep, ink, antialiasing);
			}
			else
			{
				const auto [inner, outer] = arcThicknesses[arcThicknessIndex];
				if (paint) shape.paintArc(image, arcCaps[arcCapIndex], PieStart, arcSweeps[arcSweepIndex], inner, outer, ink, antialiasing);
				else shape.overwriteArc(image, arcCaps[arcCapIndex], PieStart, arcSweeps[arcSweepIndex], inner, outer, ink, antialiasing);
			}
		}
		else if (kind == 1)
		{
			const Ellipse shape{ 96.5, 64.5, 79, (ellipseMode == 0 ? 11 : 35) };
			if (ellipseMode == 0)
			{
				if (paint) shape.paintFrame(image, 3.25, ink, antialiasing);
				else shape.overwriteFrame(image, 3.25, ink, antialiasing);
			}
			else
			{
				if (paint) shape.paintPie(image, PieStart, sweeps[ellipseMode], ink, antialiasing);
				else shape.overwritePie(image, PieStart, sweeps[ellipseMode], ink, antialiasing);
			}
		}
		else if (kind == 2)
		{
			const RoundRect shape{ 20.25, 62.7, 152, 0.6, 0.1 };
			if (paint) shape.paint(image, ink, antialiasing);
			else shape.overwrite(image, ink, antialiasing);
		}
		else if (kind == 3)
		{
			const SuperEllipse shape{ 96.25, 64.75, 76, 49, exponents[exponentIndex] };
			if (superEllipseMode == 0)
			{
				if (paint) shape.paint(image, ink, antialiasing);
				else shape.overwrite(image, ink, antialiasing);
			}
			else
			{
				const double thickness = (superEllipseMode == 1 ? 3.5 : 24.5);
				if (paint) shape.paintFrame(image, thickness, ink, antialiasing);
				else shape.overwriteFrame(image, thickness, ink, antialiasing);
			}
		}
		else
		{
			if (paint) holed.paintFrame(image, 24.5, ink, antialiasing);
			else holed.overwriteFrame(image, 24.5, ink, antialiasing);
		}
	};

	while (System::Update())
	{
		if (KeyQ.down()) { curveQualityIndex = (curveQualityIndex + 1) % curveQuality.size(); dirty = true; }
		if (KeyB.down()) { curveLimited = !curveLimited; dirty = true; }
		if (KeyD.down()) { shortLengthIndex = (shortLengthIndex + 1) % shortLengths.size(); dirty = true; }
		if (KeyS.down()) { page = (page + 1) % 6; dirty = true; }
		if (KeyJ.down())
		{
			if (page >= 4) joinIndex = (joinIndex + 1) % joins.size();
			else startCapIndex = (startCapIndex + 1) % arcCaps.size();
			dirty = true;
		}
		if (KeyK.down()) { endCapIndex = (endCapIndex + 1) % arcCaps.size(); dirty = true; }
		if (KeyA.down()) { aa = !aa; dirty = true; }
		if (KeySpace.down()) { exponentIndex = (exponentIndex + 1) % exponents.size(); dirty = true; }
		if (KeyF.down()) { superEllipseMode = (superEllipseMode + 1) % 3; dirty = true; }
		if (KeyC.down()) { circleMode = (circleMode + 1) % 7; dirty = true; }
		if (KeyL.down()) { arcCapIndex = (arcCapIndex + 1) % arcCaps.size(); dirty = true; }
		if (KeyR.down()) { arcSweepIndex = (arcSweepIndex + 1) % arcSweeps.size(); dirty = true; }
		if (KeyT.down()) { arcThicknessIndex = (arcThicknessIndex + 1) % arcThicknesses.size(); dirty = true; }
		if (KeyP.down()) { ellipseMode = (ellipseMode + 1) % 5; dirty = true; }
		if (dirty)
		{
			textures.clear();
			for (int32 row = 0; row < 4; ++row)
				for (int32 kind = 0; kind < 5; ++kind)
				{
					Image image{ 192, 128, Color{ 40, 100, 160, 40 } };
					cpu(image, kind, (row % 2) == 0);
					if (row >= 2)
						for (auto& pixel : image) pixel = Color{ pixel.a };
					textures.emplace_back(image);
				}
			dirty = false;
		}
		if (page == 5) font(U"S: centered joins   A: AA = {}   J: join = {}   B: reversed = {}"_fmt(aa, joins[joinIndex], curveLimited)).draw(20, 12);
		else if (page == 4) font(U"S: polygon frames   A: AA = {}   F: widths = {}   J: {}   B: reversed = {}"_fmt(aa, frameWidths(), joins[joinIndex], curveLimited)).draw(20, 12);
		else if (page == 1) font(U"S: strokes   A: AA = {}   Q: Bezier target = {}px   B: budget = {}"_fmt(aa, curveQuality[curveQualityIndex], curveLimited ? 4 : 256)).draw(20, 12);
		else if (page == 3) font(U"S: splines   A: AA = {}   Q: target = {}px   B: per-curve budget = {} (GPU caps: Square)"_fmt(aa, curveQuality[curveQualityIndex], curveLimited ? 1 : 256)).draw(20, 12);
		else font(U"S: {}   A: AA = {}   Space: exponent = {}   F: {}"_fmt(pages[page], aa, exponents[exponentIndex], modes[superEllipseMode])).draw(20, 12);
		if ((page == 4) || (page == 5)) font(U"T: width   triangle / clipped convex / concave / crossed / collinear (GPU omitted)").draw(20, 32);
		else if (page == 1) font(U"J: start = {}   K: end = {}    Line / LineString / Bezier2 / Bezier3 / D: {}"_fmt(arcCaps[startCapIndex], arcCaps[endCapIndex], shortLengths[shortLengthIndex])).draw(20, 32);
		else if (page == 3) font(U"J: start = {}   K: end = {}   T: width = {}    open / closed / crossed / clipped / point"_fmt(arcCaps[startCapIndex], arcCaps[endCapIndex], arrowWidths[arcThicknessIndex])).draw(20, 32);
		else if (page == 2) font(U"J: tail = {}   T: width = {}    horizontal / diagonal / D: {} / clipped / zero"_fmt(arcCaps[startCapIndex], arrowWidths[arcThicknessIndex], shortLengths[shortLengthIndex])).draw(20, 32);
		else font(U"C: {}   L: {}   R: {:.2f}   T: {}   P: {}"_fmt(circleModes[circleMode], arcCaps[arcCapIndex], arcSweeps[arcSweepIndex], arcWidths[arcThicknessIndex], ellipseModes[ellipseMode])).draw(20, 32);
		const Array<String> labels{ U"paint", U"overwrite", U"paint alpha", U"overwrite alpha", U"GPU draw" };
		for (int32 row = 0; row < 5; ++row)
		{
			font(labels[row]).draw(10, 80 + row * 148);
			for (int32 kind = 0; kind < 5; ++kind)
			{
				const Vec2 pos{ 160.0 + kind * 204, 54.0 + row * 148 };
				RectF{ pos, 192, 128 }.draw(ColorF{ 0.3 });
				if (row < 4) textures[row * 5 + kind].draw(pos);
				else if (page == 5)
				{
					const ScopedScissorRect2D clip{ static_cast<int32>(pos.x), static_cast<int32>(pos.y), 192, 128 };
					const double width = std::array<double, 3>{ 0.5, 8, 80 }[arcThicknessIndex];
					if (kind == 0) frameTriangle().movedBy(pos).drawFrame(width, ink, joins[joinIndex]);
					else frameQuad(kind).movedBy(pos).drawFrame(width, ink, joins[joinIndex]);
				}
				else if (page == 4)
				{
					const auto [inner, outer] = frameWidths();
					if (kind == 0) frameTriangle().movedBy(pos).drawFrame(inner, outer, ink, joins[joinIndex]);
					else frameQuad(kind).movedBy(pos).drawFrame(inner, outer, ink, joins[joinIndex]);
				}
				else if (page == 3)
				{
					const Transformer2D transform{ Mat3x2::Translate(pos) };
					splines[kind].draw(arrowWidths[arcThicknessIndex], ink, curveQuality[curveQualityIndex], curveLimited ? 1 : 256);
				}
				else if (page == 2)
				{
					arrowLine(kind).movedBy(pos).drawArrow(arcCaps[startCapIndex], arrowWidths[arcThicknessIndex], SizeF{ 30, 36 }, ink);
				}
				else if (page == 1)
				{
					const auto draw = [&](const auto& shape)
					{
						shape.movedBy(pos).draw(arcCaps[startCapIndex], arcCaps[endCapIndex], 18.5, ink);
					};
					const auto drawCurve = [&](const auto& shape)
					{
						shape.movedBy(pos).drawAdaptive(arcCaps[startCapIndex], arcCaps[endCapIndex], 18.5, ink,
							curveQuality[curveQualityIndex], curveLimited ? 4 : 256);
					};
					if (kind == 0) draw(strokeLine);
					else if (kind == 1) draw(strokePath);
					else if (kind == 2) drawCurve(quadratic);
					else if (kind == 3) drawCurve(cubic);
					else draw(shortLine());
				}
				else if (kind == 0)
				{
					const Circle shape{ pos + Vec2{ 96.25, 64.75 }, 49.5 };
					if (circleMode == 0) shape.draw(ink);
					else if (circleMode < 4) shape.drawSegment(PieStart, shape.r * heights[circleMode], ink);
					else if (circleMode < 6) shape.drawSegmentFromAngles(PieStart, (circleMode == 4 ? -4.5 : -Math::TwoPi), ink);
					else
					{
						const auto [inner, outer] = arcThicknesses[arcThicknessIndex];
						shape.drawArc(arcCaps[arcCapIndex], PieStart, arcSweeps[arcSweepIndex], inner, outer, ink);
					}
				}
				else if (kind == 1)
				{
					const Ellipse shape{ pos + Vec2{ 96.5, 64.5 }, 79, (ellipseMode == 0 ? 11 : 35) };
					if (ellipseMode == 0) shape.drawFrame(3.25, ink);
					else shape.drawPie(PieStart, sweeps[ellipseMode], ink);
				}
				else if (kind == 2) RoundRect{ pos.x + 20.25, pos.y + 62.7, 152, 0.6, 0.1 }.draw(ink);
				else if (kind == 3)
				{
					const SuperEllipse shape{ pos + Vec2{ 96.25, 64.75 }, 76, 49, exponents[exponentIndex] };
					if (superEllipseMode == 0) shape.draw(ink);
					else shape.drawFrame((superEllipseMode == 1 ? 3.5 : 24.5), ink);
				}
				else holed.drawFrame(pos, 24.5, ink);
			}
		}
	}
}
```
