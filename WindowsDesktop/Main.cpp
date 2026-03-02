# include <Siv3D.hpp> // Siv3D v0.8.0
# include "../Test/Siv3DTest.hpp"
//SIV3D_SET(EngineOption::D3D11Driver::WARP);



void Main()
{
	String port;

	for (const auto& info : System::EnumerateSerialPorts())
	{
		Print << info.description;
		Print << info.port;
		Print << info.hardwareID;
	}

	Serial::Config config;
	config.timeout = Serial::Timeout::Blocking();

	Serial serial{ U"COM3", 9600, config };
	Print << serial.isOpen();

	while (System::Update())
	{
		//ClearPrint();
		//Print << Profiler::FPS();
		//Print << serial.available();
		//if (const auto available = serial.available())
		//{
		//	Print << U"Available: " << available;

		//	int32 n = 0;
		//	if (serial.read(n))
		//	{
		//		Print << n;
		//	}
		//}

		if (Key1.down())
		{
			serial.writeln(U"One");
		}

		if (Key2.down())
		{
			serial.writeln(U"Two");
		}

		if (Key3.down())
		{
			serial.writeln(U"Three");
		}

		for (String line; serial.readLine(line); )
		{
			Print << U"[{}] {} ({})"_fmt(Scene::FrameCount(), line, line.size());
		}

		//if (serial.readLine()



		//if (const auto available = serial.available())
		//{
		//	Print << available;

		//	if (4 <= available)
		//	{
		//		int32 t = -1;
		//		serial.read(&t, 4);
		//		Print << U"t: " << t;
		//	}
		//}

	}
}
