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
# include "Wave.hpp"

namespace s3d
{
	namespace WaveProcessing
	{
		[[nodiscard]]
		Wave Resample(const Wave& wave, int32 newSampleRate);
	}
}
