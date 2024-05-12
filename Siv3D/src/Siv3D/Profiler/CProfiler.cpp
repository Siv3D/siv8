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

# include "CProfiler.hpp"
# include <Siv3D/Time.hpp>
# include <Siv3D/WindowState.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Renderer/IRenderer.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CProfiler::~CProfiler()
	{
		LOG_SCOPED_DEBUG("CProfiler::~CProfiler()");
	}

	void CProfiler::init()
	{
		LOG_SCOPED_DEBUG("CProfiler::init()");

		m_fpsCounter.timeStampMillisec = Time::GetMillisec();
	}

	void CProfiler::beginFrame()
	{
		// FPS
		{
			if (const int64 timeStampMillisec = Time::GetMillisec();
				(timeStampMillisec - m_fpsCounter.timeStampMillisec) < 1000)
			{
				++m_fpsCounter.frameCount;
			}
			else
			{
				m_fpsCounter.currentFPS = m_fpsCounter.frameCount;
				m_fpsCounter.frameCount = 1;
				m_fpsCounter.timeStampMillisec = timeStampMillisec;
			}
		}

		//// Stat
		//{
		//	{
		//		const auto stat = SIV3D_ENGINE(Renderer2D)->getStat();
		//		m_stat.drawCalls = stat.drawCalls;
		//		m_stat.triangleCount = stat.triangleCount;
		//	}

		//	m_stat.textureCount = static_cast<uint32>(SIV3D_ENGINE(Texture)->getTextureCount());
		//	m_stat.fontCount = static_cast<uint32>(SIV3D_ENGINE(Font)->getFontCount());
		//	m_stat.audioCount = static_cast<uint32>(SIV3D_ENGINE(Audio)->getAudioCount());
		//	m_stat.activeVoice = static_cast<uint32>(GlobalAudio::GetActiveVoiceCount());
		//}
	}

	void CProfiler::endFrame() {}

	int32 CProfiler::getFPS() const noexcept
	{
		return m_fpsCounter.currentFPS;
	}

	String CProfiler::getSimpleStatistics() const
	{
		const auto& state = SIV3D_ENGINE(Window)->getState();
		const Size& sceneSize = SIV3D_ENGINE(Renderer)->getSceneBufferSize();
		return U"{} | {} FPS | F {}x{} | V {}x{} | S {}x{}"_fmt(
			SIV3D_ENGINE(Renderer)->getName(),
			m_fpsCounter.currentFPS,
			state.frameBufferSize.x, state.frameBufferSize.y,
			state.virtualSize.x, state.virtualSize.y,
			sceneSize.x, sceneSize.y);
	}

	const ProfilerStat& CProfiler::getStat() const
	{
		return m_stat;
	}
}
