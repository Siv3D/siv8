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
# include <Siv3D/AACQuality.hpp>
# include "AACEncoder_MF.hpp"

constexpr GUID CLSID_AACMFTEncoder = { 0x93AF0C51, 0x2275, 0x45d2, { 0xA3, 0x5B, 0xF2, 0xBA, 0x21, 0xCA, 0xED, 0x00 } };

namespace s3d
{
	bool AACEncoder_MF::EncodeWaveToAAC(const StereoSampleFloat32* wave, const size_t frameCount, const uint32 sampleRate, IWriter& writer, const AACQuality quality, const size_t inputChunkFrames) noexcept
	{
		return MF::EncodeWaveToCompressedAudio(wave, frameCount, sampleRate, writer, Codec(), GetAllCapabilities(), ToAudioQuality(quality), inputChunkFrames);
	}

	Array<uint32> AACEncoder_MF::GetAvailableSampleRates(const uint32 channels) noexcept
	{
		return MF::GetAvailableSampleRates(Codec(), GetAllCapabilities(), channels);
	}

	bool AACEncoder_MF::IsSampleRateSupported(const uint32 sampleRate, const uint32 channels) noexcept
	{
		return MF::IsSampleRateSupported(Codec(), GetAllCapabilities(), sampleRate, channels);
	}

	MF::AudioQuality AACEncoder_MF::ToAudioQuality(const AACQuality quality) noexcept
	{
		switch (quality)
		{
		case AACQuality::Low:    return MF::AudioQuality::Low;
		case AACQuality::Medium: return MF::AudioQuality::Medium;
		case AACQuality::High:   return MF::AudioQuality::High;
		}

		return MF::AudioQuality::High;
	}

	const MF::AudioEncoderCodecConfig& AACEncoder_MF::Codec()
	{
		static const MF::AudioEncoderCodecConfig codec =
		{
			.name = "AACEncoder",
			.encoderClsid = CLSID_AACMFTEncoder,
			.outputSubtype = MFAudioFormat_AAC,
			.channels = 2,
			.defaultInputChunkFrames = 1024 * 4,
			.configureOutputType = [](IMFMediaType* type)
			{
				MF::ThrowIfFailed(type->SetUINT32(MF_MT_AAC_PAYLOAD_TYPE, 1), "Set AAC payload type");
			},
		};

		return codec;
	}

	const Array<MF::AudioEncoderCapability>& AACEncoder_MF::GetAllCapabilities()
	{
		static const Array<MF::AudioEncoderCapability> capabilities = []
		{
			AutoComMtaScope autoCom;
			return MF::EnumerateEncoderCapabilities(Codec());
		}();

		return capabilities;
	}
}
