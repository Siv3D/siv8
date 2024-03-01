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

# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

// ユーザが実装するエントリーポイント | User-defined entry point
void Main();

// Windows アプリケーションのエントリーポイント | Entry point for Windows applications
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	// タイマーの解像度を上げることで Sleep() の精度を上げる
	::timeBeginPeriod(1);

	if (FAILED(::CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		::OutputDebugStringW(L"CoInitializeEx() failed\n");
	}

	s3d::Siv3DEngine engine;

	Main();

	::CoUninitialize();

	// タイマーの解像度を戻す
	::timeEndPeriod(1);
	
	return 0;
}
