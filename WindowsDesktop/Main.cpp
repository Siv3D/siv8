# include <Siv3D.hpp> // Siv3D v0.8.0
# include "../Test/Siv3DTest.hpp"
//SIV3D_SET(EngineOption::D3D11Driver::WARP);

void Main()
{
	//RunTest();
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
	//Window::SetStyle(WindowStyle::Sizable);
	//Graphics::SetLowLatencyMode(true);
	//Scene::SetResizeMode(ResizeMode::Keep);

	const Texture texture{ U"example/windmill.png" };
	const Texture emoji{ U"🔥"_emoji, 200 };

	while (System::Update())
	{
		Scene::Rect().draw(Pattern::Checker{ ColorF{ 0.2 }, ColorF{ 0.25 }, 40 });

		texture.draw(10, 10);
		emoji.scaled(1.0 + Periodic::Sine1_1(4s) * 0.2).drawAt(700, 100);

		const double t = Scene::Time();
		RectF{ 40, 40, 120 }.draw();
		RectF{ 200, 40, 120 }.draw(Pattern::PolkaDot{ ColorF{ 0.2, 1.0, 0.5 }, ColorF{ 0.2, 0.2, 0.8 }, 18, 0.8, 45_deg, Vec2::All(t) });
		Circle{ 420, 100, 60 }.draw(Pattern::Grid{ ColorF{ 0.2, 1.0, 0.5 }, ColorF{ 0.2, 0.2, 0.8 }, 20.0, 0.4, (t * 10_deg) });
		Line{ 40, 220, 360, 200 }.draw(LineStyle::Round, 6, ColorF{ 0.2, 0.2, 0.8 }, ColorF{ 0.2, 1.0, 0.5 });
		Line{ 40, 260, 360, 240 }.draw(LineStyle::Dashed(t * 3), 6, ColorF{ 0.2, 0.2, 0.8 }, ColorF{0.2, 1.0, 0.5});
		Line{ 40, 300, 360, 280 }.draw(LineStyle::DashDot(t * 3), 6, ColorF{ 0.2, 0.2, 0.8 }, ColorF{ 0.2, 1.0, 0.5 });

		Circle{ 160, 440, 100 }.drawArc(LineCap::Round, (t * 90_deg), 240_deg, 20, 20, Arg::start(0.2, 0.2, 0.8), Arg::end(0.2, 1.0, 0.5));
		Circle{ 460, 440, 100 }.draw(Arg::left(0.2, 1.0, 0.5), Arg::right(0.2, 0.2, 0.8));

		Circle{ Cursor::Pos(), 100 }.draw(ColorF{ 1.0, 0.0, 0.0, 0.5 });
	}
}
