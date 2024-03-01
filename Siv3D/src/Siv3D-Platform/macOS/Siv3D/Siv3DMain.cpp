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
# include <Siv3D/Engine/Siv3DEngine.hpp>
void Main();

int main(int, char**)
{
	s3d::Siv3DEngine engine;
	
	std::clog << "OpenSiv3D for macOS\n";
	
	Main();

	return 0;
}
