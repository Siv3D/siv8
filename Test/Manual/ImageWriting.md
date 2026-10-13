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
   fill, thin frame, and wide frame.
3. Inspect the first row (`paint`), second row (`overwrite`), third row (paint
   alpha), fourth row (overwrite alpha), and the GPU row at the bottom.
4. Record the platform, exponent, fill/frame mode, AA setting, and observed result. The program
   writes no files and does not replace the repository test application's Main.

## Expected results

- AA edges have intermediate coverage; disabling AA removes intermediate coverage
  from these shape operations. RectF has no AA toggle and is not in this gallery.
- Translucent circle fills and thin rounded rectangles have no darker bands.
- The ellipse frame is continuous, including along the long axis.
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
			if (paint) shape.paint(image, ink, antialiasing);
			else shape.overwrite(image, ink, antialiasing);
		}
		else if (kind == 1)
		{
			const Ellipse shape{ 96.5, 64.5, 79, 11 };
			if (paint) shape.paintFrame(image, 3.25, ink, antialiasing);
			else shape.overwriteFrame(image, 3.25, ink, antialiasing);
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
		const Array<String> labels{ U"paint", U"overwrite", U"paint alpha", U"overwrite alpha", U"GPU draw" };
		for (int32 row = 0; row < 5; ++row)
		{
			font(labels[row]).draw(10, 80 + row * 148);
			for (int32 kind = 0; kind < 5; ++kind)
			{
				const Vec2 pos{ 160.0 + kind * 204, 54.0 + row * 148 };
				RectF{ pos, 192, 128 }.draw(ColorF{ 0.3 });
				if (row < 4) textures[row * 5 + kind].draw(pos);
				else if (kind == 0) Circle{ pos + Vec2{ 96.25, 64.75 }, 49.5 }.draw(ink);
				else if (kind == 1) Ellipse{ pos + Vec2{ 96.5, 64.5 }, 79, 11 }.drawFrame(3.25, ink);
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
