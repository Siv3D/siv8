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
# include <Siv3D/Array.hpp>
# include "MFCommon.hpp"

namespace s3d
{
	struct StereoSampleFloat32;

	class MP3Encoder_MF
	{
	public:

		static bool EncodeWaveToMP3(const StereoSampleFloat32* wave, size_t frameCount, uint32 sampleRate, IWriter& writer, MP3Quality quality, size_t inputChunkFrames = Codec().defaultInputChunkFrames) noexcept;

		static Array<uint32> GetAvailableSampleRates(uint32 channels = 2) noexcept;

		static bool IsSampleRateSupported(uint32 sampleRate, uint32 channels = 2) noexcept;

	private:

		static MF::AudioQuality ToAudioQuality(MP3Quality quality) noexcept;

		static const MF::AudioEncoderCodecConfig& Codec();

		static const Array<MF::AudioEncoderCapability>& GetAllCapabilities();
	};
}
