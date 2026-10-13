//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Window.hpp>
# include <Siv3D/Window/WindowUtility.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include "WindowMisc.hpp"

namespace s3d
{
	namespace WindowMisc
	{
		bool CenterWindowOnCurrentScreen()
		{
			return CenterToScreen(*SIV3D_ENGINE(Window), none);
		}

		bool CenterWindowOnScreen(const size_t monitorIndex)
		{
			return CenterToScreen(*SIV3D_ENGINE(Window), monitorIndex);
		}
	}

	namespace Platform::Windows::Window
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetHWND
		//
		////////////////////////////////////////////////////////////////

		void* GetHWND()
		{
			return SIV3D_ENGINE(Window)->getHandle();
		}
	}
}
