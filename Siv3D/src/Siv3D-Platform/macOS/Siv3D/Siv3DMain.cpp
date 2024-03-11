//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <iostream>
# include <Siv3D/System/ISystem.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

void Main();

int main(int, char**)
{
	using namespace s3d;
	
	std::clog << "OpenSiv3D for macOS\n";
	
	Siv3DEngine engine;
	
	try
	{
		SIV3D_ENGINE(System)->init();
	}
	catch (const Error& error)
	{
		std::cerr << error << '\n';
		return -1;
	}

	LOG_DEBUG("Main() ---");

	Main();
	
	LOG_DEBUG("--- Main()");
}
