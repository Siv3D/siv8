//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <iostream>
# include <Siv3D/Engine/Siv3DEngine.hpp>

void Main();

int main(int, char**)
{
	using namespace s3d;
	
    std::clog << "OpenSiv3D for macOS\n";

	Siv3DEngine engine;

	Main();

	return 0;
}
