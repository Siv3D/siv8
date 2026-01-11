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
# include <Siv3D/Common.hpp>
# include <Siv3D/String.hpp>
# include "ProfilerEvent.hpp"
# include "ProfilerStat.hpp"

namespace s3d
{
	struct FrameMetrics;

	class SIV3D_NOVTABLE ISiv3DProfiler
	{
	public:

		[[nodiscard]]
		static ISiv3DProfiler* Create();

		virtual ~ISiv3DProfiler() = default;

		virtual void init() = 0;

		virtual void beginFrame() = 0;

		virtual void endFrame() = 0;

		virtual int32 getFPS() const noexcept = 0;

		virtual String getSimpleStatistics() const = 0;

		virtual const FrameMetrics& getFrameMetrics() const = 0;

		virtual void reportEvent(ProfilerEvent event) = 0;

		virtual void reportStat(ProfilerStat stat, int64 delta) = 0;
	};
}
