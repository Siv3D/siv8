# Pattern coordinates and transforms

An interactive check of pattern attachment, continuity across primitives, and the
difference between moving geometry and moving its coordinate system. No external
assets or output files are required. Use the [gallery](PatternGallery.md) to edit
individual pattern parameters.

## Execution

1. Paste the complete code below into the `Main.cpp` of a separate application
   built with this Siv3D revision. Keep the repository test entry points intact.
2. Run on Windows (D3D11) or macOS (Metal). Select each of the six pattern types,
   then each transform mode.
3. Toggle `Camera` to apply the same matrix as a camera transform instead of a
   local transform. Press Space or use `Animate` to pause/resume; `Reset` returns
   the animation to its starting point.
4. Compare the left and middle panels. The middle rectangle is assembled from
   three primitives and has a cyan marker at the seam. Compare pattern features
   on either side of that marker.
5. Compare the left and right panels during translation. Both outlines move to
   the same place, but only the left panel moves the pattern's coordinate system.

## Expected results

- The left and middle panels show the same pattern phase and spacing.
  Dividing a shape into separate rectangles and triangles does not restart the
  pattern at each primitive's origin. Ignore the middle panel's cyan seam marker.
- Translation, rotation, nonuniform scale, reflection, and shear move or deform
  the pattern with its geometry in the left and middle panels. Local and camera
  modes produce the same result. Clipping to each viewport does not shift the
  pattern origin or affect the labels and neighboring panels.
- In the right panel, the moving geometry samples different parts of a pattern
  that stays anchored to the panel's drawing coordinates. This difference is
  intentional: changing shape coordinates does not create a new pattern origin.
- Pause animation to compare individual dots, grid intersections, or triangle
  corners. Narrow features may change antialiasing under deformation; they should
  not visibly slide across the geometry in the left/middle panels.

## Complete Main.cpp

```cpp
# include <Siv3D.hpp>
# include <array>
# include <cmath>

namespace
{
	PatternParameters MakePattern(const size_t type)
	{
		const ColorF primary{ 0.3, 0.9, 0.75 };
		const ColorF background{ 0.08, 0.15, 0.23 };
		const Vec2 offset{ 0.13, 0.23 };
		switch (type)
		{
		case 0: return Pattern::PolkaDot{ primary, background, 24, 0.5, 17_deg, offset };
		case 1: return Pattern::Stripe{ primary, background, 24, 0.4, 17_deg, offset };
		case 2: return Pattern::Grid{ primary, background, 24, 0.3, 17_deg, offset };
		case 3: return Pattern::Checker{ primary, background, 24, 1, 1, 17_deg, offset };
		case 4: return Pattern::Triangle{ primary, background, 24, 17_deg, offset };
		default: return Pattern::HexGrid{ primary, background, 24, 0.4, 17_deg, offset };
		}
	}
}

void Main()
{
	Window::Resize(1100, 700);
	Scene::SetBackground(ColorF{ 0.07, 0.09, 0.13 });
	const Font title{ 24, Typeface::Bold };
	const Font font{ 16 };
	const std::array<StringView, 6> types{
		U"PolkaDot", U"Stripe", U"Grid", U"Checker", U"Triangle", U"HexGrid" };
	const std::array<StringView, 5> modes{
		U"Translate", U"Rotate", U"Stretch", U"Reflect", U"Shear" };
	size_t type = 0, mode = 0;
	bool animate = true, camera = false;
	double time = 0.0;

	while (System::Update())
	{
		title(U"Pattern coordinates").draw(20, 12);
		font(U"Same geometry; compare where the pattern is anchored.").draw(290, 23);
		for (size_t i = 0; i < types.size(); ++i)
		{
			const Vec2 pos{ 20 + i * 180, 55 };
			if (SimpleGUI::Button(types[i], pos, 164.0))
			{
				type = i;
			}
			if (type == i)
			{
				RectF{ pos.x, 93, 164, 3 }.draw(Palette::Aquamarine);
			}
		}
		for (size_t i = 0; i < modes.size(); ++i)
		{
			const Vec2 pos{ 20 + i * 132, 110 };
			if (SimpleGUI::Button(modes[i], pos, 122.0))
			{
				mode = i;
			}
			if (mode == i)
			{
				RectF{ pos.x, 148, 122, 3 }.draw(Palette::Aquamarine);
			}
		}
		SimpleGUI::CheckBox(animate, U"Animate", Vec2{ 700, 110 });
		SimpleGUI::CheckBox(camera, U"Camera", Vec2{ 840, 110 });
		if (SimpleGUI::Button(U"Reset", Vec2{ 980, 110 }, 100.0))
		{
			time = 0.0;
		}
		if (KeySpace.down())
		{
			animate = not animate;
		}
		if (animate)
		{
			time += Scene::DeltaTime();
		}
		const Vec2 shift{ 35 * std::sin(time), 20 * std::cos(time * 0.7) };
		Mat3x2 linear = Mat3x2::Identity();
		switch (mode)
		{
		case 1: linear = Mat3x2::Rotate(time * 25_deg); break;
		case 2: linear = Mat3x2::Scale(1.5, 0.65).rotated(time * 20_deg); break;
		case 3: linear = Mat3x2::Scale(-1.0, 1.0).rotated(time * 20_deg); break;
		case 4: linear = Mat3x2::ShearX(0.7 * std::sin(time)); break;
		default: break;
		}
		const auto pattern = MakePattern(type);
		const std::array<StringView, 3> labels{
			U"Move drawing coordinates", U"Same, split into 3 primitives", U"Move geometry coordinates" };
		for (int32 column = 0; column < 3; ++column)
		{
			const int32 x = 20 + column * 360;
			font(labels[column]).draw(x, 174);
			RectF{ x, 205, 340, 390 }.draw(ColorF{ 0.16, 0.18, 0.22 });
			{
				const ScopedViewport2D viewport{ x, 205, 340, 390 };
				// Geometry translation is deliberately before the same linear transform.
				const Vec2 geometryShift = (column == 2) ? shift : Vec2{ 0, 0 };
				const Mat3x2 matrix = ((column == 2) ? linear
					: (Mat3x2::Translate(shift) * linear)).translated(170, 195);
				const Transformer2D transform{ matrix, camera
					? Transformer2D::Target::SetCamera : Transformer2D::Target::SetLocal };
				if (column == 1)
				{
					RectF{ -70, -50, 87, 100 }.draw(pattern);
					Triangle{ 17, -50, 70, -50, 17, 50 }.draw(pattern);
					Triangle{ 70, -50, 70, 50, 17, 50 }.draw(pattern);
					Line{ 17, -55, 17, 55 }.draw(1, Palette::Cyan);
				}
				else
				{
					RectF{ Vec2{ -70, -50 } + geometryShift, 140, 100 }.draw(pattern);
				}
				Circle{ Vec2{ 0, 85 } + geometryShift, 20 }.draw(pattern);
			}
			font(column == 2 ? U"Pattern origin stays in the panel."
				: U"Pattern follows the coordinate transform.").draw(x, 610);
		}
		font(U"Space: pause / resume. Pattern angle and offset are fixed in this sample.")
			.draw(20, 654);
	}
}
```
