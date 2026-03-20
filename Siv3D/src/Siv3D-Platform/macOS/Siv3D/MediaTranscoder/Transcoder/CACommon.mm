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

# include <Siv3D/IWriter.hpp>
# include <Siv3D/WaveSample.hpp>
# include <algorithm>
# include <array>
# include <utility>
# include "CACommon.hpp"

namespace s3d
{
	namespace
	{
		class AudioConverterHolder
		{
		public:

			AudioConverterHolder() = default;

			AudioConverterHolder(const AudioConverterHolder&) = delete;

			AudioConverterHolder(AudioConverterHolder&& other) noexcept
				: m_converter(std::exchange(other.m_converter, nullptr)) {}

			~AudioConverterHolder()
			{
				reset();
			}

			AudioConverterHolder& operator =(const AudioConverterHolder&) = delete;

			AudioConverterHolder& operator =(AudioConverterHolder&& other) noexcept
			{
				if (this != &other)
				{
					reset();
					m_converter = std::exchange(other.m_converter, nullptr);
				}

				return *this;
			}

			[[nodiscard]]
			AudioConverterRef get() const noexcept
			{
				return m_converter;
			}

			[[nodiscard]]
			AudioConverterRef* put() noexcept
			{
				reset();
				return &m_converter;
			}

			void reset() noexcept
			{
				if (m_converter)
				{
					::AudioConverterDispose(m_converter);
					m_converter = nullptr;
				}
			}

		private:

			AudioConverterRef m_converter = nullptr;
		};

		struct AudioEncodingProfile
		{
			uint32 bitRate = 0;
		};

		struct InputSource
		{
			const StereoSampleFloat32* wave = nullptr;
			size_t frameCount = 0;
			size_t frameOffset = 0;
			uint32 channels = 2;
			uint32 maxFramesPerCallback = 1024 * 4;
		};

		static constexpr std::array<uint32, 15> CommonAudioSampleRates =
		{
			7350, 8000, 11025, 12000, 16000, 22050, 24000, 32000,
			44100, 48000, 64000, 88200, 96000, 176400, 192000,
		};

		static Array<AudioValueRange> GetFormatValueRanges(
			const AudioFormatPropertyID propertyID,
			const AudioStreamBasicDescription& specifier,
			const char* what)
		{
			UInt32 size = 0;
			CA::ThrowIfFailed(
				::AudioFormatGetPropertyInfo(propertyID, sizeof(specifier), &specifier, &size),
				what);

			if ((size == 0) || (size % sizeof(AudioValueRange) != 0))
			{
				return {};
			}

			Array<AudioValueRange> result(size / sizeof(AudioValueRange));
			CA::ThrowIfFailed(
				::AudioFormatGetProperty(propertyID, sizeof(specifier), &specifier, &size, result.data()),
				what);
			return result;
		}

		static bool IsInAnyRange(const Float64 value, const Array<AudioValueRange>& ranges) noexcept
		{
			for (const auto& range : ranges)
			{
				if ((range.mMinimum <= value) && (value <= range.mMaximum))
				{
					return true;
				}
			}

			return false;
		}

		static Array<uint32> GetAvailableEncodeSampleRatesByFormat(const AudioFormatID formatID)
		{
			AudioStreamBasicDescription specifier = {};
			specifier.mFormatID = formatID;

			const Array<AudioValueRange> ranges = GetFormatValueRanges(
				kAudioFormatProperty_AvailableEncodeSampleRates,
				specifier,
				"AudioFormatGetPropertyInfo/Property(kAudioFormatProperty_AvailableEncodeSampleRates)");

			Array<uint32> result;

			for (const uint32 sampleRate : CommonAudioSampleRates)
			{
				if (IsInAnyRange(sampleRate, ranges))
				{
					result << sampleRate;
				}
			}

			return result;
		}

		static Array<uint32> GetAvailableEncodeChannelCountsByFormat(const AudioFormatID formatID)
		{
			AudioStreamBasicDescription specifier = {};
			specifier.mFormatID = formatID;

			UInt32 size = 0;
			CA::ThrowIfFailed(
				::AudioFormatGetPropertyInfo(
					kAudioFormatProperty_AvailableEncodeNumberChannels,
					sizeof(specifier),
					&specifier,
					&size),
				"AudioFormatGetPropertyInfo(kAudioFormatProperty_AvailableEncodeNumberChannels)");

			if ((size == 0) || (size % sizeof(UInt32) != 0))
			{
				return {};
			}

			Array<UInt32> values(size / sizeof(UInt32));
			CA::ThrowIfFailed(
				::AudioFormatGetProperty(
					kAudioFormatProperty_AvailableEncodeNumberChannels,
					sizeof(specifier),
					&specifier,
					&size,
					values.data()),
				"AudioFormatGetProperty(kAudioFormatProperty_AvailableEncodeNumberChannels)");

			Array<uint32> result;
			for (const UInt32 value : values)
			{
				if (value == 0xFFFFFFFFu)
				{
					result << 1;
					result << 2;
					continue;
				}

				result << static_cast<uint32>(value);
			}

			result.sort();
			result.erase(std::unique(result.begin(), result.end()), result.end());
			return result;
		}

		static void AppendUniqueBitRate(Array<uint32>& result, const uint32 bitRate)
		{
			if ((bitRate != 0) && (not result.contains(bitRate)))
			{
				result << bitRate;
			}
		}

		static Array<uint32> GetRepresentativeEncodeBitRates(
			const AudioFormatID formatID,
			const uint32 sampleRate,
			const uint32 channels)
		{
			AudioStreamBasicDescription specifier = {};
			specifier.mFormatID = formatID;
			specifier.mSampleRate = static_cast<Float64>(sampleRate);
			specifier.mChannelsPerFrame = channels;

			const Array<AudioValueRange> ranges = GetFormatValueRanges(
				kAudioFormatProperty_AvailableEncodeBitRates,
				specifier,
				"AudioFormatGetPropertyInfo/Property(kAudioFormatProperty_AvailableEncodeBitRates)");

			Array<uint32> result;

			for (const auto& range : ranges)
			{
				const uint32 minimum = static_cast<uint32>(range.mMinimum);
				const uint32 maximum = static_cast<uint32>(range.mMaximum);
				const uint32 midpoint = static_cast<uint32>((range.mMinimum + range.mMaximum) * 0.5);

				AppendUniqueBitRate(result, minimum);
				AppendUniqueBitRate(result, midpoint);
				AppendUniqueBitRate(result, maximum);
			}

			result.sort();
			return result;
		}

		static AudioStreamBasicDescription CreateFloat32InputFormat(const uint32 sampleRate, const uint32 channels)
		{
			AudioStreamBasicDescription format = {};
			format.mSampleRate = static_cast<Float64>(sampleRate);
			format.mFormatID = kAudioFormatLinearPCM;
			format.mFormatFlags = (kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked);
			format.mBytesPerPacket = (channels * sizeof(float));
			format.mFramesPerPacket = 1;
			format.mBytesPerFrame = (channels * sizeof(float));
			format.mChannelsPerFrame = channels;
			format.mBitsPerChannel = 32;
			return format;
		}

		static AudioStreamBasicDescription CreateCompressedOutputFormat(const AudioFormatID formatID, const UInt32 formatFlags, const uint32 sampleRate, const uint32 channels)
		{
			AudioStreamBasicDescription format = {};
			format.mSampleRate = static_cast<Float64>(sampleRate);
			format.mFormatID = formatID;
			format.mFormatFlags = formatFlags;
			format.mChannelsPerFrame = channels;

			UInt32 size = sizeof(format);
			CA::ThrowIfFailed(
				::AudioFormatGetProperty(kAudioFormatProperty_FormatInfo, 0, nullptr, &size, &format),
				"AudioFormatGetProperty(kAudioFormatProperty_FormatInfo)");
			return format;
		}

		static Array<AudioEncodingProfile> GetEncodingProfiles(const Array<CA::AudioEncoderCapability>& capabilities, const uint32 sampleRate, const uint32 channels)
		{
			Array<AudioEncodingProfile> profiles;

			for (const auto& capability : capabilities)
			{
				if ((capability.sampleRate == sampleRate)
					&& (capability.channels == channels)
					&& (capability.bitRate != 0)
					&& ([&]()
					{
						for (const auto& profile : profiles)
						{
							if (profile.bitRate == capability.bitRate)
							{
								return false;
							}
						}

						return true;
					})())
				{
					profiles << AudioEncodingProfile{ capability.bitRate };
				}
			}

			profiles.sort_by([](const AudioEncodingProfile& a, const AudioEncodingProfile& b)
			{
				return a.bitRate < b.bitRate;
			});
			return profiles;
		}


		static AudioConverterHolder CreateConfiguredAudioConverter(
			const CA::AudioEncoderCodecConfig& codec,
			const Array<CA::AudioEncoderCapability>& capabilities,
			const uint32 sampleRate,
			const uint32 channels,
			const CA::AudioQuality quality)
		{
			if (not ((channels == 1) || (channels == 2)))
			{
				throw Error{ "Audio encoder supports only mono or stereo." };
			}

			const Array<AudioEncodingProfile> profiles = GetEncodingProfiles(capabilities, sampleRate, channels);
			if (profiles.isEmpty())
			{
				throw Error{ fmt::format(
					"Audio encoder does not support sampleRate={} Hz, channels={} (codec={}).",
					sampleRate,
					channels,
					codec.name) };
			}


			const Array<AudioClassDescription> encoderDescriptions = CA::GetEncoderDescriptions(codec.outputFormatID);
			if (encoderDescriptions.isEmpty())
			{
				throw Error{ fmt::format("No installed encoder was found for codec={}", codec.name) };
			}

			const AudioStreamBasicDescription inputFormat = CreateFloat32InputFormat(sampleRate, channels);
			const AudioStreamBasicDescription outputFormat = CreateCompressedOutputFormat(codec.outputFormatID, codec.outputFormatFlags, sampleRate, channels);

			AudioConverterHolder converter;
			CA::ThrowIfFailed(
				::AudioConverterNewSpecific(
					&inputFormat,
					&outputFormat,
					static_cast<UInt32>(encoderDescriptions.size()),
					encoderDescriptions.data(),
					converter.put()),
				"AudioConverterNewSpecific");

			if (codec.configureConverter)
			{
				codec.configureConverter(converter.get(), sampleRate, channels, quality, capabilities);
			}

			return converter;
		}

		static OSStatus AudioConverterInputProc(
			AudioConverterRef,
			UInt32* ioNumberDataPackets,
			AudioBufferList* ioData,
			AudioStreamPacketDescription** outDataPacketDescription,
			void* inUserData)
		{
			auto* source = static_cast<InputSource*>(inUserData);

			if ((source == nullptr) || (ioNumberDataPackets == nullptr) || (ioData == nullptr))
			{
				return paramErr;
			}

			const size_t remainingFrames = (source->frameCount - source->frameOffset);
			const size_t cappedByRequest = Min<size_t>(remainingFrames, static_cast<size_t>(*ioNumberDataPackets));
			const size_t cappedByChunk = Min<size_t>(cappedByRequest, static_cast<size_t>(source->maxFramesPerCallback));
			const UInt32 framesToProvide = static_cast<UInt32>(cappedByChunk);

			ioData->mNumberBuffers = 1;
			ioData->mBuffers[0].mNumberChannels = source->channels;

			if (framesToProvide == 0)
			{
				*ioNumberDataPackets = 0;
				ioData->mBuffers[0].mDataByteSize = 0;
				ioData->mBuffers[0].mData = nullptr;
				if (outDataPacketDescription)
				{
					*outDataPacketDescription = nullptr;
				}
				return noErr;
			}

			ioData->mBuffers[0].mDataByteSize = (framesToProvide * source->channels * sizeof(float));
			ioData->mBuffers[0].mData = const_cast<StereoSampleFloat32*>(source->wave + source->frameOffset);
			*ioNumberDataPackets = framesToProvide;
			source->frameOffset += framesToProvide;

			if (outDataPacketDescription)
			{
				*outDataPacketDescription = nullptr;
			}

			return noErr;
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
					"Failed to write encoded audio bytes to writer (requested={}, written={}).",
					sizeBytes,
					written) };
			}
		}
	}

	namespace CA
	{
		void ThrowIfFailed(const OSStatus status, const char* what)
		{
			if (status != noErr)
			{
				throw Error{ fmt::format("{} (OSStatus={})", what, static_cast<int32>(status)) };
			}
		}

		Array<AudioClassDescription> GetEncoderDescriptions(const AudioFormatID formatID)
		{
			UInt32 size = 0;
			ThrowIfFailed(
				::AudioFormatGetPropertyInfo(kAudioFormatProperty_Encoders, sizeof(formatID), &formatID, &size),
				"AudioFormatGetPropertyInfo(kAudioFormatProperty_Encoders)");

			if ((size == 0) || (size % sizeof(AudioClassDescription) != 0))
			{
				return {};
			}

			Array<AudioClassDescription> result(size / sizeof(AudioClassDescription));
			ThrowIfFailed(
				::AudioFormatGetProperty(kAudioFormatProperty_Encoders, sizeof(formatID), &formatID, &size, result.data()),
				"AudioFormatGetProperty(kAudioFormatProperty_Encoders)");
			return result;
		}

		Array<AudioEncoderCapability> EnumerateEncoderCapabilities(const AudioEncoderCodecConfig& codec)
		{
			const Array<uint32> sampleRates = GetAvailableEncodeSampleRatesByFormat(codec.outputFormatID);
			const Array<uint32> channelCounts = GetAvailableEncodeChannelCountsByFormat(codec.outputFormatID);

			Array<AudioEncoderCapability> capabilities;
			for (const uint32 channels : channelCounts)
			{
				for (const uint32 sampleRate : sampleRates)
				{
					const Array<uint32> bitRates = GetRepresentativeEncodeBitRates(codec.outputFormatID, sampleRate, channels);
					for (const uint32 bitRate : bitRates)
					{
						capabilities << AudioEncoderCapability{ sampleRate, channels, bitRate };
					}
				}
			}

			capabilities.sort_by([](const AudioEncoderCapability& a, const AudioEncoderCapability& b)
			{
				if (a.channels != b.channels)
				{
					return a.channels < b.channels;
				}

				if (a.sampleRate != b.sampleRate)
				{
					return a.sampleRate < b.sampleRate;
				}

				return a.bitRate < b.bitRate;
			});

			return capabilities;
		}

		bool EncodeWaveToCompressedAudio(
			const StereoSampleFloat32* wave,
			const size_t frameCount,
			const uint32 sampleRate,
			IWriter& writer,
			const AudioEncoderCodecConfig& codec,
			const Array<AudioEncoderCapability>& capabilities,
			const AudioQuality quality,
			const size_t inputChunkFrames) noexcept
		{
			try
			{
				if (not writer)
				{
					LOG_FAIL(fmt::format("{}: writer is not open", codec.name));
					return false;
				}

				if (wave == nullptr)
				{
					LOG_FAIL(fmt::format("{}: wave is null", codec.name));
					return false;
				}

				AudioConverterHolder converter = CreateConfiguredAudioConverter(
					codec,
					capabilities,
					sampleRate,
					codec.channels,
					quality);

				UInt32 maxOutputPacketSize = 0;
				UInt32 propertySize = sizeof(maxOutputPacketSize);
				ThrowIfFailed(
					::AudioConverterGetProperty(
						converter.get(),
						kAudioConverterPropertyMaximumOutputPacketSize,
						&propertySize,
						&maxOutputPacketSize),
					"AudioConverterGetProperty(kAudioConverterPropertyMaximumOutputPacketSize)");

				if (maxOutputPacketSize == 0)
				{
					maxOutputPacketSize = 4096;
				}

				const UInt32 packetsPerCall = static_cast<UInt32>(Max<size_t>(1, (inputChunkFrames + 1023) / 1024));
				Array<uint8> outputBuffer(maxOutputPacketSize * packetsPerCall);
				Array<AudioStreamPacketDescription> packetDescriptions(packetsPerCall);

				InputSource source;
				source.wave = wave;
				source.frameCount = frameCount;
				source.channels = codec.channels;
				source.maxFramesPerCallback = static_cast<uint32>(Max<size_t>(1, inputChunkFrames));

				for (;;)
				{
					UInt32 outputPacketCount = packetsPerCall;
					std::fill(packetDescriptions.begin(), packetDescriptions.end(), AudioStreamPacketDescription{});

					AudioBufferList outputData = {};
					outputData.mNumberBuffers = 1;
					outputData.mBuffers[0].mNumberChannels = codec.channels;
					outputData.mBuffers[0].mDataByteSize = static_cast<UInt32>(outputBuffer.size() * sizeof(uint8));
					outputData.mBuffers[0].mData = outputBuffer.data();

					ThrowIfFailed(
						::AudioConverterFillComplexBuffer(
							converter.get(),
							&AudioConverterInputProc,
							&source,
							&outputPacketCount,
							&outputData,
							packetDescriptions.data()),
						"AudioConverterFillComplexBuffer");

					if (outputPacketCount == 0)
					{
						break;
					}

					for (UInt32 i = 0; i < outputPacketCount; ++i)
					{
						const AudioStreamPacketDescription& packetDesc = packetDescriptions[i];

						if ((packetDesc.mDataByteSize == 0) && (outputPacketCount != 1))
						{
							throw Error{ "AudioConverter did not return packet sizes for multi-packet output." };
						}

						const size_t offset = packetDesc.mDataByteSize ? packetDesc.mStartOffset : 0;
						const uint32 sizeBytes = packetDesc.mDataByteSize ? packetDesc.mDataByteSize : outputData.mBuffers[0].mDataByteSize;
						const auto* packetData = (static_cast<const uint8*>(outputData.mBuffers[0].mData) + offset);

						if (codec.writePacket)
						{
							codec.writePacket(writer, sampleRate, codec.channels, packetData, sizeBytes, &packetDesc);
						}
						else
						{
							WriteBytes(writer, packetData, sizeBytes);
						}
					}
				}

				return true;
			}
			catch (const Error& e)
			{
				LOG_FAIL(fmt::format("{}: {}", codec.name, e.messageUTF8()));
				return false;
			}
			catch (const std::exception& e)
			{
				LOG_FAIL(fmt::format("{}: {}", codec.name, e.what()));
				return false;
			}
			catch (...)
			{
				LOG_FAIL(fmt::format("{}: unknown error", codec.name));
				return false;
			}
		}

		Array<uint32> GetAvailableSampleRates(const AudioEncoderCodecConfig& codec, const Array<AudioEncoderCapability>& capabilities, const uint32 channels) noexcept
		{
			try
			{
				Array<uint32> result;

				for (const auto& capability : capabilities)
				{
					if ((capability.channels == channels) && (not result.contains(capability.sampleRate)))
					{
						result << capability.sampleRate;
					}
				}

				return result;
			}
			catch (const Error& e)
			{
				LOG_FAIL(fmt::format("{} GetAvailableSampleRates(): {}", codec.name, e.messageUTF8()));
				return {};
			}
			catch (const std::exception& e)
			{
				LOG_FAIL(fmt::format("{} GetAvailableSampleRates(): {}", codec.name, e.what()));
				return {};
			}
			catch (...)
			{
				LOG_FAIL(fmt::format("{} GetAvailableSampleRates(): unknown error", codec.name));
				return {};
			}
		}

		bool IsSampleRateSupported(const AudioEncoderCodecConfig& codec, const Array<AudioEncoderCapability>& capabilities, const uint32 sampleRate, const uint32 channels) noexcept
		{
			try
			{
				for (const auto& capability : capabilities)
				{
					if ((capability.sampleRate == sampleRate) && (capability.channels == channels))
					{
						return true;
					}
				}

				return false;
			}
			catch (const Error& e)
			{
				LOG_FAIL(fmt::format("{} IsSampleRateSupported(): {}", codec.name, e.messageUTF8()));
				return false;
			}
			catch (const std::exception& e)
			{
				LOG_FAIL(fmt::format("{} IsSampleRateSupported(): {}", codec.name, e.what()));
				return false;
			}
			catch (...)
			{
				LOG_FAIL(fmt::format("{} IsSampleRateSupported(): unknown error", codec.name));
				return false;
			}
		}
	}
}
