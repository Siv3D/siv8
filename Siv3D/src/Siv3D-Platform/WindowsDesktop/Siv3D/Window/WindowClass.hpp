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

# pragma once
# include <Siv3D/String.hpp>
# include <Siv3D/Windows/Windows.hpp>

namespace s3d
{
	struct WindowClass
	{
		std::wstring name;

		void registerClass(HMODULE moduleHandle);

		void unregisterClass(HMODULE moduleHandle);
	};
}
