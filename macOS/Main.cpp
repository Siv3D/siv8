
# include <Siv3D.hpp> // OpenSiv3D v0.8.0
# include "../Test/Siv3DTest.hpp"

void Main()
{
	//RunTest();
	
	//Scene::SetResizeMode(ResizeMode::Keep);
	//Scene::SetResizeMode(ResizeMode::Actual);
	//Window::SetStyle(WindowStyle::Sizable);
	
	while (System::Update())
	{
		Scene::SetBackground(HSV{ (Scene::Time() * 60), 0.8, 1.0});
	}
}
