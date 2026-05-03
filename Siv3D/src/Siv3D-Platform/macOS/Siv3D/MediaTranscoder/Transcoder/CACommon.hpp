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
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/EngineLog.hpp>
# include <AudioToolbox/AudioToolbox.h>

namespace s3d
{
	class IWriter;
	struct StereoSampleFloat32;

	namespace CA
	{
		enum class AudioQuality
		{
			Low,
			Medium,
			High,
		};

		struct AudioEncoderCapability
		{
			uint32 sampleRate = 0;
			uint32 channels = 0;
			uint32 bitRate = 0;
		};

		struct AudioEncoderCodecConfig
		{
			const char* name = "";
			AudioFormatID outputFormatID = 0;
			UInt32 outputFormatFlags = 0;
			uint32 channels = 2;
			size_t defaultInputChunkFrames = 1024 * 4;

			void (*configureConverter)(AudioConverterRef converter, uint32 sampleRate, uint32 channels, AudioQuality quality, const Array<AudioEncoderCapability>& capabilities) = nullptr;
			void (*writePacket)(IWriter& writer, uint32 sampleRate, uint32 channels, const void* data, uint32 sizeBytes, const AudioStreamPacketDescription* packetDesc) = nullptr;
		};

		void ThrowIfFailed(OSStatus status, const char* what);

		Array<AudioClassDescription> GetEncoderDescriptions(AudioFormatID formatID);

		Array<AudioEncoderCapability> EnumerateEncoderCapabilities(const AudioEncoderCodecConfig& codec);

		bool EncodeWaveToCompressedAudio(
			const StereoSampleFloat32* wave, size_t frameCount, uint32 sampleRate, IWriter& writer,
			const AudioEncoderCodecConfig& codec, const Array<AudioEncoderCapability>& capabilities,
			AudioQuality quality, size_t inputChunkFrames) noexcept;

		Array<uint32> GetAvailableSampleRates(const AudioEncoderCodecConfig& codec, const Array<AudioEncoderCapability>& capabilities, uint32 channels) noexcept;

		bool IsSampleRateSupported(const AudioEncoderCodecConfig& codec, const Array<AudioEncoderCapability>& capabilities, uint32 sampleRate, uint32 channels) noexcept;
	}
}
