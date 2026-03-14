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
# include "Types.hpp"

namespace s3d
{
	enum class WAVSaveFormat
	{
		PCM8Stereo,
	
		PCM16Stereo,
	
		PCM24Stereo,
	
		PCM32Stereo,

		Float32Stereo,

		Default = PCM16Stereo,
	};
}
