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
	struct FormatData;

	enum class WAVSaveFormat
	{
		PCM8Mono,

		PCM8Stereo,

		PCM16Mono,
	
		PCM16Stereo,
	
		PCM24Mono,

		PCM24Stereo,
	
		PCM32Mono,

		PCM32Stereo,

		Float32Mono,

		Float32Stereo,

		Default = PCM16Stereo,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const WAVSaveFormat value);
}
