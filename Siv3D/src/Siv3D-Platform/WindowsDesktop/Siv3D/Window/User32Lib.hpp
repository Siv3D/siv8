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

# pragma once
# include <Siv3D/DLL.hpp>

namespace s3d
{
	struct User32Lib
	{
		LibraryHandle library = nullptr;

		decltype(GetSystemMetricsForDpi)* pGetSystemMetricsForDpi = nullptr;

		decltype(AdjustWindowRectExForDpi)* pAdjustWindowRectExForDpi = nullptr;

		decltype(SetWindowFeedbackSetting)* pSetWindowFeedbackSetting = nullptr;

		void load();

		void unload();
	};
}
