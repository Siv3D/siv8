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

# include "CSystem.hpp"
# include <ThirdParty/Oniguruma/oniguruma.h>
# include <Siv3D/EngineResource/IEngineResource.hpp>
# include <Siv3D/UserAction/IUserAction.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/ImageDecoder/IImageDecoder.hpp>
# include <Siv3D/ImageEncoder/IImageEncoder.hpp>
# include <Siv3D/Emoji/IEmoji.hpp>
# include <Siv3D/Cursor/ICursor.hpp>
# include <Siv3D/CursorStyle/ICursorStyle.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	CSystem::~CSystem()
	{
		LOG_SCOPED_DEBUG("CSystem::~CSystem()");
	}

	void CSystem::init()
	{
		LOG_SCOPED_DEBUG("CSystem::init()");

		{
			OnigEncoding encodings = ONIG_ENCODING_UTF32_LE;
			onig_initialize(&encodings, 1);
		}

		SIV3D_ENGINE(EngineResource)->init();

		SIV3D_ENGINE(Window)->init();

		SIV3D_ENGINE(ImageDecoder)->init();

		SIV3D_ENGINE(ImageEncoder)->init();
		
		SIV3D_ENGINE(Emoji)->init();
		
		SIV3D_ENGINE(Cursor)->init();

		SIV3D_ENGINE(CursorStyle)->init();

		LOG_INFO("✅ Siv3D engine has initialized");
	}
	
	bool CSystem::update()
	{
		if (m_shouldTerminate)
		{
			return false;
		}

		if (SIV3D_ENGINE(UserAction)->shouldTerminate())
		{
			m_shouldTerminate = true;
			
			return false;
		}
		
		//
		//	previous frame
		//
		//-----------------------------------
		//
		//	current frame
		//
		
		SIV3D_ENGINE(Window)->update();
		SIV3D_ENGINE(Cursor)->update();

		return true;
	}
}
