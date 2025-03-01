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
# include "IProfiler.hpp"

namespace s3d
{
	class CProfiler final : public ISiv3DProfiler
	{
	public:

		~CProfiler() override;

		void init() override;

		void beginFrame() override;

		void endFrame() override;

		int32 getFPS() const noexcept override;

		String getSimpleStatistics() const override;

		const ProfilerStat& getStat() const override;

	private:

		struct FPSCounter
		{
			int32 frameCount = 0;

			int32 currentFPS = 1;

			int64 timeStampMillisec = 0;
		
		} m_fpsCounter;

		ProfilerStat m_stat;
	};
}
