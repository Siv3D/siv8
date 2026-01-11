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

# pragma once
# include <array>
# include <Siv3D/FrameMetrics.hpp>
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

		const FrameMetrics& getFrameMetrics() const override;

		void reportEvent(ProfilerEvent event) override;

		void reportStat(ProfilerStat stat, int64 delta) override;

	private:

		struct FPSCounter
		{
			int32 frameCount = 0;

			int32 currentFPS = 1;

			int64 timeStampMillisec = 0;
		
		} m_fpsCounter;

		FrameMetrics m_frameMetrics{ .frameIndex = -1 };

		std::array<uint64, 8> m_timestamps{};

		std::array<int64, 2> m_stats{};
	};
}
