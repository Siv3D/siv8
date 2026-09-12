# Renderer2D RMS scaling

This visual check covers tessellation and pattern scaling under rotation followed
by a nonuniform scale. It uses public APIs and creates no output files.

## Execution

1. Build a separate Siv3D v0.8 application with this revision and paste the complete
   code below into its Main.cpp. Keep the repository test entry points intact.
2. Run on Windows (D3D11) and macOS (Metal). Press Space to animate or pause rotation.
3. Inspect the three columns, which differ by 45 degrees of rotation before an
   X scale of 4 and a Y scale of 1. Compare paused and animated frames.
4. Change `Mat3x2::Scale(4.0f, 1.0f)` to `Mat3x2::Scale(2.0f)` and repeat to check
   uniform scaling.

## Expected results

- The displayed RMS scale stays approximately 2.91548 in every column as the angle
  changes. Under the uniform scale it stays at 2.
- The first row has ellipses with the same ideal outline. The tessellation density
  should not oscillate with rotation. Small changes in polygon vertex placement
  are expected; RMS is a quality estimate, not a bound on screen-space error.
- The next three rows show round rectangles, round-ended lines, and polygons made
  from rectangles with unequal corner radii. Their shapes rotate and distort, but
  there should be no periodic jumps in subdivision density caused by scale changes.
- The final row shows patterned circles. On Metal, the pattern follows the full
  drawing transform: dots rotate and stretch with the geometry. On D3D11, the
  pattern stays aligned to the render target and its size follows the constant
  RMS scale. It should not pulsate as the angle changes. The backends use different
  pattern coordinate models until the D3D11 port.
- Dividing line thickness by RMS compensates uniform scaling exactly. With the
  nonuniform scale, thickness depends on orientation and is only approximated by
  this scalar. D3D11 and Metal should show equivalent line-width behavior.

## Complete code

```cpp
# include <Siv3D.hpp>

void Main()
{
	Window::Resize(1000, 680);
	Scene::SetBackground(ColorF{ 0.12 });
	const Font font{ 18 };
	bool animate = false;
	double angle = 0.0;

	while (System::Update())
	{
		if (KeySpace.down())
		{
			animate = not animate;
		}
		if (animate)
		{
			angle += Scene::DeltaTime() * 30_deg;
		}

		font(U"Space: animate / pause").draw(20, 15);
		font(U"Circle / round rectangle / line / corner polygon / pattern").draw(20, 42);
		for (int32 column = 0; column < 3; ++column)
		{
			const double x = (180 + column * 320);
			const Mat3x2 linear = (Mat3x2::Rotate(angle + column * 45_deg)
				* Mat3x2::Scale(4.0f, 1.0f));
			font(U"RMS: ", linear.rmsScaling()).draw(x - 100, 85);
			for (int32 row = 0; row < 5; ++row)
			{
				const Transformer2D transform{ linear.translated(x, 160 + row * 105) };
				switch (row)
				{
				case 0:
					Circle{ 0, 0, 26 }.draw(ColorF{ 0.3, 0.8, 1.0 });
					break;
				case 1:
					RectF{ -24, -18, 48, 36 }.rounded(8).draw(ColorF{ 0.9, 0.6, 0.2 });
					break;
				case 2:
					Line{ -24, 0, 24, 0 }.draw(LineCap::Round, LineCap::Round,
						12.0 / Graphics2D::GetRMSScaling(), ColorF{ 0.4, 1.0, 0.5 });
					break;
				case 3:
					RectF{ -24, -18, 48, 36 }.rounded(4, 8, 12, 16).draw(ColorF{ 0.8, 0.5, 1.0 });
					break;
				case 4:
					Circle{ 0, 0, 26 }.draw(Pattern::PolkaDot{
						.primary = ColorF{ 1.0 }, .background = ColorF{ 0.2, 0.4, 0.7 }, .scale = 14.0 });
					break;
				}
			}
		}
	}
}
```
