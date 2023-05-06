# include <Siv3D.hpp> // OpenSiv3D v0.8.0
# include "../Test/Siv3DTest.hpp"

void Main()
{
	RunTest();

	Console << DateTime::Now();

	Console << (Time::GetUptimeSec() / 60);

	std::system("pause");
}
