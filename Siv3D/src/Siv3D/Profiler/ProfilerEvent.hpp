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

namespace s3d
{
	enum class ProfilerEvent : uint8
	{
		GPUWait_Start,

		GPUWait_End,

		EngineBegin_Start,

		EngineBegin_End,

		UserUpdate_Start,

		UserUpdate_End,

		EngineEnd_Start,

		EngineEnd_End,
	};
}
