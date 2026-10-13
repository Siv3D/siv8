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
3. Inspect the first row (`paint`), second row (`overwrite`), third row (paint
   alpha), fourth row (overwrite alpha), and the GPU row at the bottom.
4. Record the platform, exponent, shape modes, AA setting, and observed result. The program
   writes no files and does not replace the repository test application's Main.

## Expected results

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
- The last column has a Polygon with a hole and overlapping wide frames. The CPU
  combines the frames before applying color; the overlap must not become darker.
  The GPU may composite the contours separately.
- SuperEllipse changes from concave to diamond to ellipse to a rounded box.
  Its image fill at exponent 2 matches Ellipse. Other fills use 4x4 samples,
  so very small features can disappear between sample positions. Frames are
  centered on an approximation of the boundary and have rounded corners. Check
  horizontal/vertical symmetry and that wide concave frames have no darker overlaps.
- Paint raises background alpha. Overwrite replaces alpha in fully covered
  pixels. Both preserve pixels outside the shape. The grayscale views distinguish
  these effects from ordinary display blending.
- The GPU row has the same geometry and color intent; edge coverage and frame
  joins may differ. See the [API inventory](../../docs/geometry2d/image-writing.md)
  for differences that require separate API work.

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
	bool aa = true;
	bool dirty = true;
	Array<Texture> textures;
	const Polygon holed{ RectF{ 20.25, 16.75, 152, 96 }.outer(),
		{ RectF{ 31.25, 27.75, 130, 74 }.outer().reversed() } };

	const auto cpu = [&](Image& image, const int32 kind, const bool paint)
	{
		const EnableAntialiasing antialiasing{ aa };
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
		font(U"A: AA = {}   Space: exponent = {}   F: {}"_fmt(aa, exponents[exponentIndex], modes[superEllipseMode])).draw(20, 12);
		font(U"C: {}   L: {}   R: {:.2f}   T: {}   P: {}"_fmt(circleModes[circleMode], arcCaps[arcCapIndex], arcSweeps[arcSweepIndex], arcWidths[arcThicknessIndex], ellipseModes[ellipseMode])).draw(20, 32);
		const Array<String> labels{ U"paint", U"overwrite", U"paint alpha", U"overwrite alpha", U"GPU draw" };
		for (int32 row = 0; row < 5; ++row)
		{
			font(labels[row]).draw(10, 80 + row * 148);
			for (int32 kind = 0; kind < 5; ++kind)
			{
				const Vec2 pos{ 160.0 + kind * 204, 54.0 + row * 148 };
				RectF{ pos, 192, 128 }.draw(ColorF{ 0.3 });
				if (row < 4) textures[row * 5 + kind].draw(pos);
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
