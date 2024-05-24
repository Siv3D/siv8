
# include <Siv3D.hpp> // Siv3D v0.8.0
# include "../Test/Siv3DTest.hpp"

//SIV3D_SET(EngineOption::D3D11Driver::WARP);

void Main()
{
	//RunTest();

	//Window::SetStyle(WindowStyle::Sizable);
	//Scene::SetResizeMode(ResizeMode::Keep);

	while (System::Update())
	{
		Scene::SetBackground(HSV{ (Scene::Time() * 60), 0.8, 1.0 });
	}
}
