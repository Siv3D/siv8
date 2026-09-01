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
# include <Siv3D.hpp>

// ベンチマークを実行する場合 1, 実行しない場合 0
# define SIV3D_RUN_BENCHMARK 0

// 所用時間の長いテストを実行する場合 1, 実行しない場合 0
# define SIV3D_RUN_HEAVY_TEST 0

# if SIV3D_RUN_BENCHMARK

	//
	// nanobench
	//
	# include <ThirdParty/nanobench/nanobench.h>
	using ankerl::nanobench::Bench;
	using ankerl::nanobench::doNotOptimizeAway;

# endif

//
// doctest
//
# if SIV3D_COMPILER(APPLE_CLANG) // Xcode では色付けを無効化
#   define DOCTEST_CONFIG_COLORS_NONE
# endif
# include <ThirdParty/doctest/doctest.h>

int32 RunTest();

namespace s3d
{
	class ScopedLogSilencer
	{
	public:

		ScopedLogSilencer()
		{
			Logger.setOutputLevel(LogLevel::Error);
		}

		~ScopedLogSilencer()
		{
			Logger.setOutputLevel(m_oldOutputLevel);
		}

	private:

		LogLevel m_oldOutputLevel = Logger.getOutputLevel();
	};
}
