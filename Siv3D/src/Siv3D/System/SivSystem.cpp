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

# include <Siv3D/System.hpp>
# include <Siv3D/System/ISystem.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>


namespace s3d
{
	namespace System
	{
		////////////////////////////////////////////////////////////////
		//
		//	Update
		//
		////////////////////////////////////////////////////////////////

		bool Update()
		{
			// [Siv3D ToDo] VSync を実装したら外す
		# if SIV3D_PLATFORM(MACOS) || SIV3D_PLATFORM(LINUX)
		
			Sleep(16);

		# endif

			return SIV3D_ENGINE(System)->update();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Sleep
		//
		////////////////////////////////////////////////////////////////

		void Sleep(const Duration& duration)
		{
			const int32 milliSec = static_cast<int32>(duration.count() * 1000);

			Sleep(milliSec);
		}
	}
}
