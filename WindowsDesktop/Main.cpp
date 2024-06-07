
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

	while (System::Update())
	{
		RectF{ 100, 100, 100 }.draw();
		RectF{ 200, 200, 100 }.draw(ColorF{ 0.8, 0.9, 1.0 });
		Line{ 300, 200, 400, 100 }.draw(10);
		Line{ 300, 300, 400, 400 }.drawUncapped(10);
		Rect{ 400, 400, 200, 100 }.draw(Arg::left(0.3, 0.6, 0.9), Arg::right(0.5, 0.8, 1.0));
		Triangle{ 600, 100, 700, 200, 500, 200 }.draw(ColorF{ 0.8, 0.6, 0.2 });
		Circle{ Cursor::Pos(), 100 }.draw(ColorF{ 1.0, 0.0, 0.0, 0.5 });
	}
}
