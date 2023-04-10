//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Windows/MinWindows.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

// ユーザによって実装される | Implemented by the user
void Main();

// Windows 用エントリーポイント | Windows entry point
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	using namespace s3d;

	Siv3DEngine engine;

	Main();

	return 0;
}
