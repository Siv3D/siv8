# Render texture operations gallery

This gallery compares a cropped source, Gaussian blur, Dual Kawase blur, and
quarter-resolution downsampling followed by Gaussian blur. All processing uses
explicit physical pixel regions. The magenta area around the source is poison:
it must never enter the processed images.

Run the complete program below as the application's `Main`. Drag the sliders to
change the active width, Gaussian sigma, and Dual Kawase radius. Keep the program
running while the width decreases and grows again. Press **S** to save a normal
screen capture. The two blur strength controls use different scales.

Expected results:

- The source's magenta surroundings never appear inside a filtered panel.
- Shrinking and regrowing the active region leaves no stale bands or edge seams.
- Sigma zero and radius zero reproduce the cropped source.
- Dual Kawase changes continuously around radii 1, 3, 7, 15, and 31. Its texture
  and directional character can differ from Gaussian blur.
- The last panel is an intentionally cheaper approximation; thin lines may lose
  detail. Its sigma is divided by four to roughly preserve the apparent scale.
- Window or Scene presentation scaling does not change the RT processing sizes.

```cpp
# include <Siv3D.hpp>

void Main()
{
	Window::Resize(1120, 500);
	Scene::Configure(SceneConfig::Fixed({ 1120, 500 }));
	Scene::SetBackground(ColorF{ 0.12 });
	const Font label{ FontMethod::MSDF, 20 };
	const Size capacity{ 256, 192 };
	const RenderTexture atlas{ Size{ 288, 224 } };
	const RenderTexture gaussian{ capacity }, kawase{ capacity }, approximate{ capacity };
	const RenderTexture small{ Size{ 64, 48 } }, smallBlurred{ small.size() };
	RenderTextureOps::Workspace work;
	double width = 256, sigma = 8, radius = 3;

	while (System::Update())
	{
		const Size active{ static_cast<int32>(width), 192 };
		const Rect input{ 16, 16, active }, output{ active };
		const Rect reduced{ Max(1, active.x / 4), 48 };
		{
			const ScopedRenderTarget2D target{ atlas };
			const ScopedScissorRect2D clip{ input };
			atlas.clear(Palette::Magenta);
			input.draw(ColorF{ 0.18, 0.23, 0.3 });
			for (int32 y = 16; y < 208; y += 12)
			{
				for (int32 x = 16; x < 272; x += 12)
				{
					if (((x / 12 + y / 12) & 1) == 0) { Rect{ x, y, 12, 12 }.draw(ColorF{ 0.3 }); }
				}
			}
			Circle{ 100, 100, 48 }.draw(ColorF{ 0.15, 0.65, 1.0, 0.8 });
			Rect{ 157, 48, 34, 112 }.draw(Palette::Orange);
			Line{ 24, 32, 248, 184 }.draw(2, Palette::White);
			Circle{ 220, 175, 2 }.draw(Palette::White);
		}

		RenderTextureOps::GaussianBlur(atlas(input), gaussian, output, sigma, work);
		RenderTextureOps::DualKawaseBlur(atlas(input), kawase, output, radius, work);
		RenderTextureOps::Downsample(atlas(input), small, reduced, work);
		RenderTextureOps::GaussianBlur(small(reduced), smallBlurred, reduced, sigma / 4, work);
		RenderTextureOps::Resample(smallBlurred(reduced), approximate, output);
		{
			const ScopedRenderStates2D sampler{ SamplerState::ClampNearest };
			atlas(input).draw(16, 50);
			gaussian(output).draw(296, 50);
			kawase(output).draw(576, 50);
			approximate(output).draw(856, 50);
		}
		label(U"Source ROI").draw(16, 18);
		label(U"Gaussian").draw(296, 18);
		label(U"Dual Kawase").draw(576, 18);
		label(U"1/4 size + Gaussian").draw(856, 18);
		const auto slider = [&](const StringView name, double& value, const double minimum, const double maximum, const double y)
		{
			if (MouseL.pressed() && RectF{ 20, y, 400, 30 }.mouseOver())
			{
				value = minimum + Clamp((Cursor::PosF().x - 20) / 400, 0.0, 1.0) * (maximum - minimum);
			}
			RectF{ 20, y + 10, 400, 8 }.draw(ColorF{ 0.3 });
			Circle{ 20 + (value - minimum) / (maximum - minimum) * 400, y + 14, 8 }.draw(Palette::Skyblue);
			label(U"{}: {:.2f}"_fmt(name, value)).draw(445, y);
		};
		slider(U"Active width", width, 64.0, 256.0, 280);
		slider(U"Gaussian sigma", sigma, 0.0, 32.0, 330);
		slider(U"Kawase radius", radius, 0.0, 32.0, 380);
		label(U"S: screenshot | Resize and vary blur strengths to check reuse").draw(20, 450);
		if (KeyS.down()) { ScreenCapture::SaveCurrentFrame(); }
	}
}
```
