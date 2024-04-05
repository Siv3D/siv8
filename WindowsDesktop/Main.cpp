
# include <Siv3D.hpp> // Siv3D v0.8.0
# include "../Test/Siv3DTest.hpp"

void Main()
{
	//RunTest();

	int32 count = 0;

	while (System::Update())
	{
		Window::SetTitle(U"Siv3D App: frame {}"_fmt(++count));
	}
}
