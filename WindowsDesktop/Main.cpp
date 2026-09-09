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
