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

# include <Siv3D/System.hpp>
# include <Siv3D/System/ISystem.hpp>
# include <Siv3D/UserAction/IUserAction.hpp>
# include <Siv3D/Renderer/IRenderer.hpp>
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
			return SIV3D_ENGINE(System)->update();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Exit
		//
		////////////////////////////////////////////////////////////////

		void Exit() noexcept
		{
			SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::SystemExitCalled);
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetTerminationTriggers, GetTerminationTriggers
		//
		////////////////////////////////////////////////////////////////

		void SetTerminationTriggers(const uint32 userActionFlags) noexcept
		{
			SIV3D_ENGINE(UserAction)->setTerminationTriggers(userActionFlags);
		}

		uint32 GetTerminationTriggers() noexcept
		{
			return SIV3D_ENGINE(UserAction)->getTerminationTriggers();
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetUserActions
		//
		////////////////////////////////////////////////////////////////

		uint32 GetUserActions() noexcept
		{
			return SIV3D_ENGINE(UserAction)->getPreviousUserActions();
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

		////////////////////////////////////////////////////////////////
		//
		//	GetRendererType
		//
		////////////////////////////////////////////////////////////////

		EngineOption::Renderer GetRendererType()
		{
			return SIV3D_ENGINE(Renderer)->getRendererType();
		}
	}
}
