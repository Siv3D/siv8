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
# include <Siv3D/WaveSample.hpp>
# include <Siv3D/IWriter.hpp>
# include <Siv3D/MP3Quality.hpp>
# include "MP3Encoder_MF.hpp"

constexpr GUID CLSID_MP3ACMCodecWrapper = { 0x11103421, 0x354c, 0x4cca, { 0xa7, 0xa3, 0x1a, 0xff, 0x9a, 0x5b, 0x67, 0x01 } };

namespace s3d
{
	bool MP3Encoder_MF::EncodeWaveToMP3(const StereoSampleFloat32* wave, const size_t frameCount, const uint32 sampleRate, IWriter& writer, const MP3Quality quality, const size_t inputChunkFrames) noexcept
	{
		return MF::EncodeWaveToCompressedAudio(wave, frameCount, sampleRate, writer, Codec(), GetAllCapabilities(), ToAudioQuality(quality), inputChunkFrames);
	}

	Array<uint32> MP3Encoder_MF::GetAvailableSampleRates(const uint32 channels) noexcept
	{
		return MF::GetAvailableSampleRates(Codec(), GetAllCapabilities(), channels);
	}

	bool MP3Encoder_MF::IsSampleRateSupported(const uint32 sampleRate, const uint32 channels) noexcept
	{
		return MF::IsSampleRateSupported(Codec(), GetAllCapabilities(), sampleRate, channels);
	}

	MF::AudioQuality MP3Encoder_MF::ToAudioQuality(const MP3Quality quality) noexcept
	{
		switch (quality)
		{
		case MP3Quality::Low:    return MF::AudioQuality::Low;
		case MP3Quality::Medium: return MF::AudioQuality::Medium;
		case MP3Quality::High:   return MF::AudioQuality::High;
		}

		return MF::AudioQuality::High;
	}

	const MF::AudioEncoderCodecConfig& MP3Encoder_MF::Codec()
	{
		static const MF::AudioEncoderCodecConfig codec =
		{
			.name = "MP3Encoder",
			.encoderClsid = CLSID_MP3ACMCodecWrapper,
			.outputSubtype = MFAudioFormat_MP3,
			.channels = 2,
			.defaultInputChunkFrames = 1152 * 4,
			.configureOutputType = {},
		};

		return codec;
	}

	const Array<MF::AudioEncoderCapability>& MP3Encoder_MF::GetAllCapabilities()
	{
		static const Array<MF::AudioEncoderCapability> capabilities = []
		{
			AutoComMtaScope autoCom;
			return MF::EnumerateEncoderCapabilities(Codec());
		}();

		return capabilities;
	}
}
