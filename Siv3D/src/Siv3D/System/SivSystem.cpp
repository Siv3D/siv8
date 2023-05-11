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

# include <Siv3D/System.hpp>
# include <Siv3D/System/ISystem.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace System
	{
		bool Update()
		{
			return SIV3D_ENGINE(System)->update();
		}

		void Sleep(const Duration& duration)
		{
			const int32 milliSec = static_cast<int32>(duration.count() * 1000);

			Sleep(milliSec);
		}
	}
}
