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

# include <Siv3D/Keyboard.hpp>
# include <Siv3D/Keyboard/IKeyboard.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace Keyboard
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetAllInputs
		//
		////////////////////////////////////////////////////////////////

		const Array<Input>& GetAllInputs() noexcept
		{
			return SIV3D_ENGINE(Keyboard)->getAllInput();
		}

		//Array<KeyEvent> GetEvents()
		//{
		//	return SIV3D_ENGINE(Keyboard)->getEvents();
		//}
	}
}
