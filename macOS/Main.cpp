
# include <Siv3D.hpp> // Siv3D v0.8.0
# include "../Test/Siv3DTest.hpp"

void Main()
{
	//RunTest();
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
	//Window::SetStyle(WindowStyle::Sizable);
	//Scene::SetResizeMode(ResizeMode::Keep);

	while (System::Update())
	{
		RectF{ 100, 100, 100 }.draw();
		RectF{ 200, 200, 100 }.draw(ColorF{ 0.8, 0.9, 1.0 });
		Circle{ Cursor::Pos(), 100 }.draw(ColorF{ 1.0, 0.0, 0.0, 0.5 });
	}
}
