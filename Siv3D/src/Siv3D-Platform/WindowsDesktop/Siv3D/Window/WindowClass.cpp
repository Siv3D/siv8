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

# include "WindowClass.hpp"
# include "WindowProc.hpp"
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>

namespace s3d
{
	void WindowClass::registerClass(const HMODULE moduleHandle)
	{
		// WindowClass の名前を生成
		name = FileSystem::ModulePath().toWstr();

		const WNDCLASSEX windowClass
		{
			.cbSize			= sizeof(WNDCLASSEX),
			.style			= (CS_HREDRAW | CS_VREDRAW),
			.lpfnWndProc	= WindowProc,
			.hInstance		= moduleHandle,
			.hIcon			= ::LoadIconW(moduleHandle, MAKEINTRESOURCEW(100)),
			.hCursor		= nullptr,
			.hbrBackground	= static_cast<HBRUSH>(::GetStockObject(DKGRAY_BRUSH)),
			.lpszClassName	= name.c_str(),
		};

		LOG_DEBUG("RegisterClassExW()");

		if (not ::RegisterClassExW(&windowClass))
		{
			throw InternalEngineError{ "RegisterClassExW() failed" };
		}
	}
	
	void WindowClass::unregisterClass(const HMODULE moduleHandle)
	{
		LOG_DEBUG("UnregisterClassW()");
		
		[[maybe_unused]] const BOOL b = ::UnregisterClassW(name.c_str(), moduleHandle);
		
		name.clear();

		LOG_DEBUG(fmt::format("UnregisterClassW() -> {}", static_cast<bool>(b)));
	}
}
