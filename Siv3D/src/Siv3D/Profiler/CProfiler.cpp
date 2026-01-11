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

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CProfiler::init()
	{
		LOG_SCOPED_DEBUG("CProfiler::init()");

		m_fpsCounter.timeStampMillisec = Time::GetMillisec();
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginFrame
	//
	////////////////////////////////////////////////////////////////

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

		// Stat
		{
			++m_frameMetrics.frameIndex;

			m_frameMetrics.drawCalls = std::exchange(m_stats[FromEnum(ProfilerStat::Renderer2D_DrawCalls)], 0);
			m_frameMetrics.triangleCount = std::exchange(m_stats[FromEnum(ProfilerStat::Renderer2D_TriangleCount)], 0);

			m_frameMetrics.engineBeginTimeUs = (m_timestamps[FromEnum(ProfilerEvent::EngineBegin_End)]
				- m_timestamps[FromEnum(ProfilerEvent::EngineBegin_Start)]);

			m_frameMetrics.userUpdateTimeUs = (m_timestamps[FromEnum(ProfilerEvent::UserUpdate_End)]
				- m_timestamps[FromEnum(ProfilerEvent::UserUpdate_Start)]);

			m_frameMetrics.engineEndTimeUs = (m_timestamps[FromEnum(ProfilerEvent::EngineEnd_End)]
				- m_timestamps[FromEnum(ProfilerEvent::EngineEnd_Start)]);

			m_frameMetrics.gpuWaitTimeUs = (m_timestamps[FromEnum(ProfilerEvent::GPUWait_End)]
				- m_timestamps[FromEnum(ProfilerEvent::GPUWait_Start)]);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	endFrame
	//
	////////////////////////////////////////////////////////////////

	void CProfiler::endFrame() {}

	////////////////////////////////////////////////////////////////
	//
	//	getFPS
	//
	////////////////////////////////////////////////////////////////

	int32 CProfiler::getFPS() const noexcept
	{
		return m_fpsCounter.currentFPS;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSimpleStatistics
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	getFrameMetrics
	//
	////////////////////////////////////////////////////////////////

	const FrameMetrics& CProfiler::getFrameMetrics() const
	{
		return m_frameMetrics;
	}

	////////////////////////////////////////////////////////////////
	//
	//	reportEvent
	//
	////////////////////////////////////////////////////////////////

	void CProfiler::reportEvent(const ProfilerEvent event)
	{
		const uint64 timestamp = Time::GetMicrosec();
		m_timestamps[FromEnum(event)] = timestamp;
	}

	////////////////////////////////////////////////////////////////
	//
	//	reportStat
	//
	////////////////////////////////////////////////////////////////

	void CProfiler::reportStat(const ProfilerStat stat, const int64 delta)
	{
		m_stats[FromEnum(stat)] += delta;
	}
}
