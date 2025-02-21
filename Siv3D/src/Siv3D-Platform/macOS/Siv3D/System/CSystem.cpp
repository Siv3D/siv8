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
# include <Siv3D/LicenseManager/ILicenseManager.hpp>
# include <Siv3D/RegExp/IRegExp.hpp>
# include <Siv3D/EngineResource/IEngineResource.hpp>
# include <Siv3D/Profiler/IProfiler.hpp>
# include <Siv3D/AssetMonitor/IAssetMonitor.hpp>
# include <Siv3D/UserAction/IUserAction.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Scene/CScene.hpp>
# include <Siv3D/ImageDecoder/IImageDecoder.hpp>
# include <Siv3D/ImageEncoder/IImageEncoder.hpp>
# include <Siv3D/Emoji/IEmoji.hpp>
# include <Siv3D/Cursor/ICursor.hpp>
# include <Siv3D/CursorStyle/ICursorStyle.hpp>
# include <Siv3D/Keyboard/IKeyboard.hpp>
# include <Siv3D/Mouse/IMouse.hpp>
# include <Siv3D/Renderer/IRenderer.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/ScreenCapture/IScreenCapture.hpp>
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

		SIV3D_ENGINE(RegExp)->init();
		SIV3D_ENGINE(EngineResource)->init();
		SIV3D_ENGINE(Profiler)->init();
		SIV3D_ENGINE(Window)->init();
		SIV3D_ENGINE(ImageDecoder)->init();
		SIV3D_ENGINE(ImageEncoder)->init();
		SIV3D_ENGINE(Emoji)->init();
		SIV3D_ENGINE(Cursor)->init();
		SIV3D_ENGINE(CursorStyle)->init();
		SIV3D_ENGINE(Keyboard)->init();
		SIV3D_ENGINE(Mouse)->init();
		SIV3D_ENGINE(Renderer)->init();
		SIV3D_ENGINE(Renderer2D)->init();
		SIV3D_ENGINE(ScreenCapture)->init();

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
	
		SIV3D_ENGINE(Renderer)->flush();
		SIV3D_ENGINE(Profiler)->endFrame();
		SIV3D_ENGINE(Renderer)->present();
		SIV3D_ENGINE(ScreenCapture)->update();
	
		//
		//	previous frame
		//
		//-----------------------------------
		//
		//	current frame
		//
		
		SIV3D_ENGINE(Profiler)->beginFrame();

		if (not SIV3D_ENGINE(AssetMonitor)->update())
		{
			m_shouldTerminate = true;
			return false;
		}

		SIV3D_ENGINE(Window)->update();
		SIV3D_ENGINE(Scene)->update();
		SIV3D_ENGINE(Renderer)->beginFrame();
		SIV3D_ENGINE(Cursor)->update();
		SIV3D_ENGINE(CursorStyle)->update();
		SIV3D_ENGINE(Keyboard)->update();
		SIV3D_ENGINE(Mouse)->update();
		SIV3D_ENGINE(LicenseManager)->update();

		return true;
	}
}
