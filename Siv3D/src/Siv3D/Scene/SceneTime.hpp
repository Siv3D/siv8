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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Scene.hpp>

namespace s3d
{
	class SceneTime
	{
	public:

		void update();

		int32 getFrameCount() const noexcept;

		double getDeltaTimeSec() const noexcept;

		double getTimeSec() const noexcept;

		void setMaxDeltaTimeSec(double timeSec) noexcept;

		double getMaxDeltaTimeSec() const noexcept;

	private:

		int32 m_userFrameCount = 0;

		double m_timeSec = 0.0;

		double m_deltaTimeSec = 0.0;

		double m_maxDeltaTimeSec = Scene::DefaultMaxDeltaTime;

		int64 m_previousFrameTimeNanosec = 0;
	};
}
