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

# include <Siv3D/Time.hpp>
# include "SceneTime.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void SceneTime::update()
	{
		++m_userFrameCount;

		const int64 currentNanoSec = Time::GetNanosec();

		const double deltaTimeSec = (m_previousFrameTimeNanosec ?
			((currentNanoSec - m_previousFrameTimeNanosec) / 1'000'000'000.0) : 0.0);

		m_deltaTimeSec = Min(deltaTimeSec, m_maxDeltaTimeSec);

		m_timeSec += m_deltaTimeSec;

		m_previousFrameTimeNanosec = currentNanoSec;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getFrameCount
	//
	////////////////////////////////////////////////////////////////

	int32 SceneTime::getFrameCount() const noexcept
	{
		return m_userFrameCount;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getDeltaTimeSec
	//
	////////////////////////////////////////////////////////////////

	double SceneTime::getDeltaTimeSec() const noexcept
	{
		return m_deltaTimeSec;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getTimeSec
	//
	////////////////////////////////////////////////////////////////

	double SceneTime::getTimeSec() const noexcept
	{
		return m_timeSec;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setMaxDeltaTimeSec
	//
	////////////////////////////////////////////////////////////////

	void SceneTime::setMaxDeltaTimeSec(const double timeSec) noexcept
	{
		m_deltaTimeSec = Max(timeSec, 0.0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMaxDeltaTimeSec
	//
	////////////////////////////////////////////////////////////////

	double SceneTime::getMaxDeltaTimeSec() const noexcept
	{
		return m_maxDeltaTimeSec;
	}
}
