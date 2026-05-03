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

# include <Siv3D/WaveSample.hpp>
# include <Siv3D/IWriter.hpp>
# include <Siv3D/AACQuality.hpp>
# include "AACEncoder_CA.hpp"

namespace s3d
{
	namespace
	{
		static uint8 AACSampleRateIndex(const uint32 sampleRate)
		{
			switch (sampleRate)
			{
			case 96000: return 0;
			case 88200: return 1;
			case 64000: return 2;
			case 48000: return 3;
			case 44100: return 4;
			case 32000: return 5;
			case 24000: return 6;
			case 22050: return 7;
			case 16000: return 8;
			case 12000: return 9;
			case 11025: return 10;
			case 8000:  return 11;
			case 7350:  return 12;
			default:
				throw Error{ fmt::format("Unsupported AAC ADTS sample rate: {}", sampleRate) };
			}
		}

		static void WriteBytes(IWriter& writer, const void* data, const uint32 sizeBytes)
		{
			if (sizeBytes == 0)
			{
				return;
			}

			const int64 written = writer.write(data, static_cast<int64>(sizeBytes));
			if (written != static_cast<int64>(sizeBytes))
			{
				throw Error{ fmt::format(
					"Failed to write ADTS AAC bytes to writer (requested={}, written={}).",
					sizeBytes,
					written) };
			}
		}


		static Array<AudioValueRange> GetApplicableEncodeBitRateRanges(AudioConverterRef converter)
		{
			UInt32 size = 0;
			const OSStatus infoStatus = ::AudioConverterGetPropertyInfo(
				converter,
				kAudioConverterApplicableEncodeBitRates,
				&size,
				nullptr);

			if ((infoStatus != noErr) || (size == 0) || (size % sizeof(AudioValueRange) != 0))
			{
				return {};
			}

			Array<AudioValueRange> ranges(size / sizeof(AudioValueRange));
			const OSStatus getStatus = ::AudioConverterGetProperty(
				converter,
				kAudioConverterApplicableEncodeBitRates,
				&size,
				ranges.data());

			if (getStatus != noErr)
			{
				return {};
			}

			return ranges;
		}

		static uint32 ChooseBitRateFromRanges(const Array<AudioValueRange>& ranges, const CA::AudioQuality quality)
		{
			Array<uint32> candidates;
			for (const auto& range : ranges)
			{
				const uint32 minimum = static_cast<uint32>(range.mMinimum);
				const uint32 maximum = static_cast<uint32>(range.mMaximum);
				const uint32 midpoint = static_cast<uint32>((range.mMinimum + range.mMaximum) * 0.5);
				if ((minimum != 0) && (not candidates.contains(minimum))) candidates << minimum;
				if ((midpoint != 0) && (not candidates.contains(midpoint))) candidates << midpoint;
				if ((maximum != 0) && (not candidates.contains(maximum))) candidates << maximum;
			}

			if (candidates.isEmpty())
			{
				return 0;
			}

			candidates.sort();
			switch (quality)
			{
			case CA::AudioQuality::Low:
				return candidates.front();
			case CA::AudioQuality::Medium:
				return candidates[candidates.size() / 2];
			case CA::AudioQuality::High:
			default:
				return candidates.back();
			}
		}

		static void ConfigureAACConverter(
			AudioConverterRef converter,
			const uint32,
			const uint32,
			const CA::AudioQuality quality,
			const Array<CA::AudioEncoderCapability>&)
		{
			const Array<AudioValueRange> applicableRanges = GetApplicableEncodeBitRateRanges(converter);
			const uint32 selectedBitRate = ChooseBitRateFromRanges(applicableRanges, quality);

			if (selectedBitRate == 0)
			{
				return;
			}

			UInt32 propertySize = sizeof(selectedBitRate);
			const OSStatus setStatus = ::AudioConverterSetProperty(
				converter,
				kAudioConverterEncodeBitRate,
				propertySize,
				&selectedBitRate);

			if (setStatus != noErr)
			{
				return;
			}
		}

		static void WriteAACPacket(
			IWriter& writer,
			const uint32 sampleRate,
			const uint32 channels,
			const void* data,
			const uint32 sizeBytes,
			const AudioStreamPacketDescription*)
		{
			if (channels > 7)
			{
				throw Error{ fmt::format("ADTS AAC does not support channels={} in this implementation.", channels) };
			}

			const uint8 frequencyIndex = AACSampleRateIndex(sampleRate);
			const uint8 channelConfig = static_cast<uint8>(channels);
			const uint32 frameLength = (7 + sizeBytes);
			constexpr uint8 profile = 1; // AAC LC (audioObjectType=2, stored as profile-1)

			uint8 header[7] = {};
			header[0] = 0xFF;
			header[1] = 0xF1;
			header[2] = static_cast<uint8>(((profile & 0x03) << 6)
				| ((frequencyIndex & 0x0F) << 2)
				| ((channelConfig >> 2) & 0x01));
			header[3] = static_cast<uint8>(((channelConfig & 0x03) << 6)
				| ((frameLength >> 11) & 0x03));
			header[4] = static_cast<uint8>((frameLength >> 3) & 0xFF);
			header[5] = static_cast<uint8>(((frameLength & 0x07) << 5) | 0x1F);
			header[6] = 0xFC;

			WriteBytes(writer, header, sizeof(header));
			WriteBytes(writer, data, sizeBytes);
		}
	}

	bool AACEncoder_CA::EncodeWaveToAAC(const StereoSampleFloat32* wave, const size_t frameCount, const uint32 sampleRate, IWriter& writer, const AACQuality quality, const size_t inputChunkFrames) noexcept
	{
		return CA::EncodeWaveToCompressedAudio(wave, frameCount, sampleRate, writer, Codec(), GetAllCapabilities(), ToAudioQuality(quality), inputChunkFrames);
	}

	Array<uint32> AACEncoder_CA::GetAvailableSampleRates(const uint32 channels) noexcept
	{
		return CA::GetAvailableSampleRates(Codec(), GetAllCapabilities(), channels);
	}

	bool AACEncoder_CA::IsSampleRateSupported(const uint32 sampleRate, const uint32 channels) noexcept
	{
		return CA::IsSampleRateSupported(Codec(), GetAllCapabilities(), sampleRate, channels);
	}

	CA::AudioQuality AACEncoder_CA::ToAudioQuality(const AACQuality quality) noexcept
	{
		switch (quality)
		{
		case AACQuality::Low:    return CA::AudioQuality::Low;
		case AACQuality::Medium: return CA::AudioQuality::Medium;
		case AACQuality::High:   return CA::AudioQuality::High;
		}

		return CA::AudioQuality::High;
	}

	const CA::AudioEncoderCodecConfig& AACEncoder_CA::Codec()
	{
		static const CA::AudioEncoderCodecConfig codec =
		{
			.name = "AACEncoder",
			.outputFormatID = kAudioFormatMPEG4AAC,
			.outputFormatFlags = kMPEG4Object_AAC_LC,
			.channels = 2,
			.defaultInputChunkFrames = 1024 * 4,
			.configureConverter = &ConfigureAACConverter,
			.writePacket = &WriteAACPacket,
		};

		return codec;
	}

	const Array<CA::AudioEncoderCapability>& AACEncoder_CA::GetAllCapabilities()
	{
		static const Array<CA::AudioEncoderCapability> capabilities = []()
		{
			try
			{
				return CA::EnumerateEncoderCapabilities(Codec());
			}
			catch (const Error& e)
			{
				LOG_FAIL(fmt::format("{} GetAllCapabilities(): {}", Codec().name, e.messageUTF8()));
				return Array<CA::AudioEncoderCapability>{};
			}
			catch (const std::exception& e)
			{
				LOG_FAIL(fmt::format("{} GetAllCapabilities(): {}", Codec().name, e.what()));
				return Array<CA::AudioEncoderCapability>{};
			}
			catch (...)
			{
				LOG_FAIL(fmt::format("{} GetAllCapabilities(): unknown error", Codec().name));
				return Array<CA::AudioEncoderCapability>{};
			}
		}();
		return capabilities;
	}
}
