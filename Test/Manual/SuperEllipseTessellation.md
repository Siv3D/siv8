# SuperEllipse drawing tessellation

This program compares fill and frame contours while changing the exponent and
zoom. The cyan overlay is a dense analytic reference with exact axis endpoints;
it does not call `outer()` or `asPolygon()`. It concentrates angular samples near
the axes so that large exponents are represented without the axis rounding of a
uniform-angle reference.

## Run

1. Copy the complete program below into a separate Siv3D application.
2. Use keys **1–8** to select exponents 0.25, 0.5, 1, 2, 4, 8, 32, and 128.
   Hold **Left/Right** for continuous changes. **Space** animates zoom.
3. Toggle **R** for the reference, **H** for display-resolution rendering,
   **A** for nonuniform scaling, and **T** for a thick frame.
   **B** switches to 512x magnification; use **Up/Down** to inspect different
   positions between an axis tip and the diagonal. Use `n = 2` or `n = 4` to
   exercise the 256-interval cap, and toggle R to inspect chord error.
4. Inspect the four axis tips, the diagonal regions, and changes during zoom.
   Repeat on Metal and Direct3D 11. Display resolution requires a high-DPI display
   to differ from scene resolution.

## Expected results

- Fill and frame follow the reference for both concave and convex exponents.
  At `n = 1` the contour is an exact diamond. The four axis points remain present.
- Zoom and display resolution increase detail without changing the logical size.
  The contour may change its segment count during animation. At most 256
  intervals are used around the whole contour. In the magnified view, chord
  error may exceed the target, but no contour tail should be disproportionately
  coarse because the subdivision budget was exhausted earlier in traversal.
- The frame uses the existing line joins. Wide frames at concave tips can exhibit
  the existing join and overlap behavior; the reference overlay marks the stroke
  centerline, not its inner or outer offset.
- Nonuniform transforms use an RMS scalar, so the error target is not a strict
  screen-space bound along the most stretched direction. The fixed subdivision
  budget also takes precedence over the target at very large magnifications.

Automated geometry, gradient, pixel, transformation, resolution, and generated
Polygon compatibility checks are in
[Test_Renderer2DSuperEllipse.cpp](../Test_Renderer2DSuperEllipse.cpp).

```cpp
# include <Siv3D.hpp>
# include <cmath>

LineString Reference(const double n)
{
	Array<Vec2> quarter;
	for (int32 i = 0; i <= 2048; ++i)
	{
		const double angle = (Math::QuarterPi * std::pow(i / 2048.0, 8));
		quarter.push_back({ std::pow(std::cos(angle), 2 / n), std::pow(std::sin(angle), 2 / n) });
	}
	for (int32 i = 2047; i >= 0; --i)
	{
		const Vec2 p = quarter[i];
		quarter.push_back({ p.y, p.x });
	}
	LineString result;
	const size_t end = (quarter.size() - 1);
	for (size_t i = 0; i < end; ++i) result.push_back(quarter[i]);
	for (size_t i = end; i > 0; --i) result.push_back({ -quarter[i].x, quarter[i].y });
	for (size_t i = 0; i < end; ++i) result.push_back(-quarter[i]);
	for (size_t i = end; i > 0; --i) result.push_back({ quarter[i].x, -quarter[i].y });
	for (auto& point : result) point *= Vec2{ 92, 72 };
	return result;
}

void Main()
{
	Window::Resize(1100, 700);
	Scene::Configure(SceneConfig::Fixed({ 1100, 700 }, SceneResolution::SceneSize));
	Scene::SetBackground(ColorF{ 0.08 });
	const Font font{ 18 };
	const Array<Input> keys{ Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8 };
	const Array<double> exponents{ 0.25, 0.5, 1, 2, 4, 8, 32, 128 };
	double n = 4, previousN = 0, time = 0, inspectAngle = Math::QuarterPi;
	bool animate = false, reference = false, display = false, stretched = false, thick = false, magnified = false;
	LineString outline;
	while (System::Update())
	{
		for (size_t i = 0; i < keys.size(); ++i) if (keys[i].down()) n = exponents[i];
		n = Clamp(n * std::exp((int(KeyRight.pressed()) - int(KeyLeft.pressed())) * Scene::DeltaTime()), 0.25, 128.0);
		if (KeySpace.down()) animate = !animate;
		if (KeyR.down()) reference = !reference;
		if (KeyA.down()) stretched = !stretched;
		if (KeyT.down()) thick = !thick;
		if (KeyB.down()) magnified = !magnified;
		inspectAngle = Clamp(inspectAngle + (int(KeyUp.pressed()) - int(KeyDown.pressed())) * Scene::DeltaTime() * 0.02,
			0.0, Math::QuarterPi);
		if (KeyH.down())
		{
			display = !display;
			Scene::Configure(SceneConfig::Fixed({ 1100, 700 },
				display ? SceneResolution::Display : SceneResolution::SceneSize));
		}
		if (n != previousN) { outline = Reference(n); previousN = n; }
		if (animate) time += Scene::DeltaTime();
		const double zoom = ((magnified ? 512 : 1) * (1 + 0.65 * std::sin(time)));
		const Vec2 focus = magnified ? Vec2{ 92 * std::pow(std::cos(inspectAngle), 2 / n),
			72 * std::pow(std::sin(inspectAngle), 2 / n) } : Vec2{ 0, 0 };
		for (int32 column = 0; column < 3; ++column)
		{
			const ScopedScissorRect2D clip{ 20 + column * 360, 200, 340, 480 };
			const Transformer2D transform{ Mat3x2::Translate(-focus) * Mat3x2::Scale(zoom, zoom * (stretched ? 0.5 : 1))
				.translated(190 + column * 360, 390) };
			const SuperEllipse shape{ 0, 0, 92, 72, n };
			const Pattern::PolkaDot pattern{ .primary = Palette::Orange, .background = ColorF{ 0.25 } };
			if (column == 0) shape.draw(Arg::top(ColorF{ 0.9, 0.3, 0.1 }), Arg::bottom(ColorF{ 0.4, 0.1, 0.6 }));
			if (column == 1) shape.drawFrame((thick ? 20 : 3) / (magnified ? zoom : 1), Palette::Orange);
			if (column == 2) shape.drawFrame((thick ? 20 : 3) / (magnified ? zoom : 1), pattern);
			if (reference) outline.drawClosed(0.75 / zoom, Palette::Cyan);
		}
		font(U"n={:.4f}  zoom={:.2f}  render={}"_fmt(n, zoom, Scene::RenderResolution())).draw(20, 20);
		font(U"1-8: exponent  Left/Right: adjust  Space: zoom animation").draw(20, 50);
		font(U"R: analytic reference  H: resolution  A: stretch  T: thick frame").draw(20, 80);
		font(U"B: magnified view  Up/Down: inspect contour").draw(20, 110);
		font(U"Gradient fill").drawAt(190, 150);
		font(U"Solid frame").drawAt(550, 150);
		font(U"Pattern frame").drawAt(910, 150);
	}
}
```

## CPU vertex-generation benchmark

From the repository root on macOS:

```sh
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=Renderer2D.superellipse_vertices_benchmark'
```

This hidden benchmark covers ordinary and budget-saturating sizes, reuses fixed
vertex/index buffers, and measures the fill
builder, including tessellation and gradient vertex writes. It excludes renderer
submission, frame joins, GPU execution, and allocation. Compare the same cases
and build configuration when evaluating another tessellator; triangle counts
alone do not establish a CPU speedup.
