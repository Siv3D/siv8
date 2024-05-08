﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Profiler.hpp>
# include <Siv3D/Profiler/IProfiler.hpp>
//# include <Siv3D/AssetMonitor/IAssetMonitor.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace Profiler
	{
		int32 FPS() noexcept
		{
			return SIV3D_ENGINE(Profiler)->getFPS();
		}

		//void EnableAssetCreationWarning(const bool enabled)
		//{
		//	SIV3D_ENGINE(AssetMonitor)->setWarningEnabled(enabled);
		//}

		const ProfilerStat& GetStat()
		{
			return SIV3D_ENGINE(Profiler)->getStat();
		}
	}
}
