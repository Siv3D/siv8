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

# pragma once
# include <Siv3D.hpp>

// ベンチマークを実行する場合 1, 実行しない場合 0
# define SIV3D_RUN_BENCHMARK 1

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
# if SIV3D_COMPILER(APPLECLANG) // Xcode では色付けを無効化
#   define DOCTEST_CONFIG_COLORS_NONE
# endif
# include <ThirdParty/doctest/doctest.h>

void RunTest();

class ScopedLogSilencer
{
public:

	ScopedLogSilencer()
	{
		Logger.setOutputLevel(LogType::Error);
	}

	~ScopedLogSilencer()
	{
		Logger.setOutputLevel(m_oldOutputLevel);
	}

private:

	LogType m_oldOutputLevel = Logger.getOutputLevel();
};
