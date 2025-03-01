//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "User32Lib.hpp"

namespace s3d
{
	void User32Lib::load()
	{
		library						= DLL::LoadSystemLibrary(L"user32.dll");
		pGetSystemMetricsForDpi		= DLL::GetFunctionNoThrow{ library, "GetSystemMetricsForDpi" };
		pAdjustWindowRectExForDpi	= DLL::GetFunctionNoThrow{ library, "AdjustWindowRectExForDpi" };
	}

	void User32Lib::unload()
	{
		DLL::Unload(library);
	}
}
