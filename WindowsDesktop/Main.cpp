# include <Siv3D.hpp> // OpenSiv3D v0.8.0
# include "../Test/Siv3DTest.hpp"

void Main()
{
	Console.open();

	const char s[] = "あ";

	std::cout << std::size(s) << '\n';
	std::cout << "日本語🐈日本語\n";

	std::system("pause");
}
