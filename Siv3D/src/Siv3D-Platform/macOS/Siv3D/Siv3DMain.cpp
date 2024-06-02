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
# include <Siv3D/FreestandingMessageBox/FreestandingMessageBox.hpp>
# include <Siv3D/EngineLog.hpp>

void Main();

namespace s3d::detail::init
{
	void InitCommandLines(int argc, char** argv);
}

int main(int argc, char* argv[])
{
	using namespace s3d;
	
	std::clog << "OpenSiv3D for macOS\n";
	
	detail::init::InitCommandLines(argc, argv);
	
	Siv3DEngine engine;
	
	try
	{
		SIV3D_ENGINE(System)->init();
	}
	catch (const Error& error)
	{
		FreestandingMessageBox::ShowError(error.messageUTF8());
		std::cerr << error << '\n';
		return -1;
	}

	LOG_DEBUG("Main() ---");

	Main();
	
	LOG_DEBUG("--- Main()");
}
