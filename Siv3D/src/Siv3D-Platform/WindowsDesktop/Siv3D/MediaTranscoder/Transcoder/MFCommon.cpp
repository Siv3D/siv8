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
# include <Siv3D/IWriter.hpp>
# include <Siv3D/WaveSample.hpp>
# include <Siv3D/Windows/ComPtr.hpp>
# include "MFCommon.hpp"

namespace s3d
{
	namespace
	{
		struct AudioEncodingProfile
		{
			uint32 avgBytesPerSec = 0;
		};

		static Array<int16> InterleavePcm16(const StereoSampleFloat32* samples, size_t frameCount, uint32 channels)
		{
			Array<int16> pcm;

			if (channels == 1)
			{
				pcm.resize(frameCount);

				for (size_t i = 0; i < frameCount; ++i)
				{
					const StereoSampleInt16 sample16 = samples[i].toInt16();
					const int32 mixed = ((static_cast<int32>(sample16.left) + static_cast<int32>(sample16.right)) / 2);
					pcm[i] = static_cast<int16>(Clamp(mixed, static_cast<int32>(INT16_MIN), static_cast<int32>(INT16_MAX)));
				}
			}
			else if (channels == 2)
			{
				pcm.resize(frameCount * 2);

				for (size_t i = 0; i < frameCount; ++i)
				{
					const StereoSampleInt16 sample16 = samples[i].toInt16();
					pcm[i * 2 + 0] = sample16.left;
					pcm[i * 2 + 1] = sample16.right;
				}
			}
			else
			{
				throw Error{ "Audio encoder supports only mono or stereo input." };
			}

			return pcm;
		}

		static ComPtr<IMFSample> CreateSampleFromBytes(const void* data, DWORD size)
		{
			ComPtr<IMFMediaBuffer> buffer;
			MF::ThrowIfFailed(MFCreateMemoryBuffer(size, buffer.GetAddressOf()), "MFCreateMemoryBuffer");

			BYTE* dst = nullptr;
			DWORD maxLen = 0;
			DWORD curLen = 0;
			MF::ThrowIfFailed(buffer->Lock(&dst, &maxLen, &curLen), "IMFMediaBuffer::Lock");

			std::memcpy(dst, data, size);

			MF::ThrowIfFailed(buffer->SetCurrentLength(size), "IMFMediaBuffer::SetCurrentLength");
			MF::ThrowIfFailed(buffer->Unlock(), "IMFMediaBuffer::Unlock");

			ComPtr<IMFSample> sample;
			MF::ThrowIfFailed(MFCreateSample(sample.GetAddressOf()), "MFCreateSample");
			MF::ThrowIfFailed(sample->AddBuffer(buffer.Get()), "IMFSample::AddBuffer");

			return sample;
		}

		static void WriteSampleToWriter(IMFSample* sample, IWriter& writer)
		{
			ComPtr<IMFMediaBuffer> contiguous;
			MF::ThrowIfFailed(sample->ConvertToContiguousBuffer(contiguous.GetAddressOf()), "ConvertToContiguousBuffer");

			BYTE* p = nullptr;
			DWORD maxLen = 0;
			DWORD curLen = 0;
			MF::ThrowIfFailed(contiguous->Lock(&p, &maxLen, &curLen), "Output buffer Lock");

			const int64 written = writer.write(p, static_cast<int64>(curLen));
			const HRESULT unlockHr = contiguous->Unlock();

			if (written != static_cast<int64>(curLen))
			{
				if (FAILED(unlockHr))
				{
					MF::ThrowIfFailed(unlockHr, "Output buffer Unlock");
				}

				throw Error{
					fmt::format(
						"Failed to write encoded audio bytes to writer (requested={}, written={}).",
						static_cast<uint32>(curLen),
						written)
				};
			}

			MF::ThrowIfFailed(unlockHr, "Output buffer Unlock");
		}

		static bool TryProcessAllAvailableOutput(IMFTransform* encoder, IWriter& writer)
		{
			bool producedAny = false;

			for (;;)
			{
				MFT_OUTPUT_STREAM_INFO streamInfo = {};
				MF::ThrowIfFailed(encoder->GetOutputStreamInfo(0, &streamInfo), "GetOutputStreamInfo");

				ComPtr<IMFSample> outSample;
				ComPtr<IMFMediaBuffer> outBuffer;

				MFT_OUTPUT_DATA_BUFFER output = {};
				DWORD status = 0;

				if (streamInfo.dwFlags & MFT_OUTPUT_STREAM_PROVIDES_SAMPLES)
				{
					output.pSample = nullptr;
				}
				else
				{
					DWORD bufferSize = streamInfo.cbSize;
					if (bufferSize == 0)
					{
						bufferSize = 64 * 1024;
					}

					MF::ThrowIfFailed(MFCreateSample(outSample.GetAddressOf()), "MFCreateSample(output)");
					MF::ThrowIfFailed(MFCreateMemoryBuffer(bufferSize, outBuffer.GetAddressOf()), "MFCreateMemoryBuffer(output)");
					MF::ThrowIfFailed(outSample->AddBuffer(outBuffer.Get()), "AddBuffer(output)");
					output.pSample = outSample.Get();
				}

				const HRESULT hr = encoder->ProcessOutput(0, 1, &output, &status);

				if (hr == MF_E_TRANSFORM_NEED_MORE_INPUT)
				{
					break;
				}

				MF::ThrowIfFailed(hr, "ProcessOutput");

				if (output.pSample)
				{
					WriteSampleToWriter(output.pSample, writer);
					producedAny = true;
				}

				if (output.pEvents)
				{
					output.pEvents->Release();
				}
			}

			return producedAny;
		}

		static Array<AudioEncodingProfile> GetEncodingProfiles(const Array<MF::AudioEncoderCapability>& capabilities, const uint32 sampleRate, const uint32 channels)
		{
			Array<AudioEncodingProfile> profiles;

			for (const auto& cap : capabilities)
			{
				if ((cap.sampleRate == sampleRate) && (cap.channels == channels))
				{
					profiles << AudioEncodingProfile{ cap.avgBytesPerSec };
				}
			}

			return profiles;
		}

		static const AudioEncodingProfile& SelectEncodingProfile(const Array<AudioEncodingProfile>& profiles, const MF::AudioQuality quality)
		{
			switch (quality)
			{
			case MF::AudioQuality::Low:
				return profiles[1];

			case MF::AudioQuality::Medium:
				return profiles[profiles.size() / 2];

			case MF::AudioQuality::High:
			default:
				return profiles.back();
			}
		}

		static ComPtr<IMFMediaType> SelectOutputMediaType(
			IMFTransform* encoder, const MF::AudioEncoderCodecConfig& codec,
			const uint32 sampleRate, const uint32 channels, const uint32 targetAvgBytesPerSec)
		{
			for (DWORD i = 0;; ++i)
			{
				ComPtr<IMFMediaType> type;
				const HRESULT hr = encoder->GetOutputAvailableType(0, i, type.GetAddressOf());

				if (hr == MF_E_NO_MORE_TYPES)
				{
					break;
				}

				MF::ThrowIfFailed(hr, "GetOutputAvailableType");

				GUID major = {};
				GUID subtype = {};
				UINT32 sr = 0;
				UINT32 ch = 0;
				UINT32 bitsPerSample = 0;
				UINT32 avgBytesPerSec = 0;

				if (FAILED(type->GetGUID(MF_MT_MAJOR_TYPE, &major))) continue;
				if (FAILED(type->GetGUID(MF_MT_SUBTYPE, &subtype))) continue;
				if (FAILED(type->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sr))) continue;
				if (FAILED(type->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &ch))) continue;
				if (FAILED(type->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bitsPerSample))) continue;
				if (FAILED(type->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &avgBytesPerSec))) continue;

				if (major != MFMediaType_Audio) continue;
				if (subtype != codec.outputSubtype) continue;
				if (sr != sampleRate) continue;
				if (ch != channels) continue;
				if (avgBytesPerSec != targetAvgBytesPerSec) continue;

				if (codec.configureOutputType)
				{
					codec.configureOutputType(type.Get());
				}

				return type;
			}

			throw Error{
				fmt::format(
					"Audio output type not found (codec={}, sampleRate={} Hz, channels={}, avgBytesPerSec={}).",
					codec.name,
					sampleRate,
					channels,
					targetAvgBytesPerSec)
			};
		}

		static ComPtr<IMFMediaType> CreatePcm16InputType(const uint32 sampleRate, const uint32 channels)
		{
			ComPtr<IMFMediaType> inType;
			MF::ThrowIfFailed(MFCreateMediaType(inType.GetAddressOf()), "MFCreateMediaType(input)");

			MF::ThrowIfFailed(inType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio), "Set input major");
			MF::ThrowIfFailed(inType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM), "Set input subtype");
			MF::ThrowIfFailed(inType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16), "Set BPS");
			MF::ThrowIfFailed(inType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, sampleRate), "Set SR");
			MF::ThrowIfFailed(inType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, channels), "Set CH");
			MF::ThrowIfFailed(inType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, channels * sizeof(int16)), "Set block align");
			MF::ThrowIfFailed(inType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, sampleRate * channels * sizeof(int16)), "Set avg bytes");
			MF::ThrowIfFailed(inType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE), "Set independent");

			return inType;
		}

		static ComPtr<IMFTransform> CreateConfiguredAudioEncoder(
			const MF::AudioEncoderCodecConfig& codec, const Array<MF::AudioEncoderCapability>& capabilities,
			const uint32 sampleRate, const uint32 channels, const MF::AudioQuality quality)
		{
			if (not ((channels == 1) || (channels == 2)))
			{
				throw Error{ "Audio encoder supports only mono or stereo." };
			}

			const Array<AudioEncodingProfile> profiles = GetEncodingProfiles(capabilities, sampleRate, channels);

			if (profiles.isEmpty())
			{
				throw Error{
					fmt::format(
						"Audio encoder does not support sampleRate={} Hz, channels={} (codec={}).",
						sampleRate,
						channels,
						codec.name)
				};
			}

			const AudioEncodingProfile& selectedProfile = SelectEncodingProfile(profiles, quality);

			LOG_DEBUG(fmt::format(
				"Selected audio encoding profile: codec={}, sampleRate={} Hz, channels={}, avgBytesPerSec={}",
				codec.name,
				sampleRate,
				channels,
				selectedProfile.avgBytesPerSec));

			ComPtr<IMFTransform> encoder;
			MF::ThrowIfFailed(
				::CoCreateInstance(
					codec.encoderClsid,
					nullptr,
					CLSCTX_INPROC_SERVER,
					IID_PPV_ARGS(encoder.GetAddressOf())),
				fmt::format("CoCreateInstance({})", codec.name).c_str());

			{
				ComPtr<IMFMediaType> outType = SelectOutputMediaType(
					encoder.Get(),
					codec,
					sampleRate,
					channels,
					selectedProfile.avgBytesPerSec);

				MF::ThrowIfFailed(encoder->SetOutputType(0, outType.Get(), 0), "SetOutputType");
			}

			{
				ComPtr<IMFMediaType> inType = CreatePcm16InputType(sampleRate, channels);
				MF::ThrowIfFailed(encoder->SetInputType(0, inType.Get(), 0), "SetInputType");
			}

			MF::ThrowIfFailed(encoder->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0), "BEGIN_STREAMING");
			MF::ThrowIfFailed(encoder->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, 0), "START_OF_STREAM");

			return encoder;
		}
	}

	AutoComMtaScope::AutoComMtaScope()
	{
		APTTYPE aptType = APTTYPE_CURRENT;
		APTTYPEQUALIFIER aptQualifier = APTTYPEQUALIFIER_NONE;

		const HRESULT aptHr = ::CoGetApartmentType(&aptType, &aptQualifier);

		if (aptHr == CO_E_NOTINITIALIZED)
		{
			MF::ThrowIfFailed(::CoInitializeEx(nullptr, COINIT_MULTITHREADED), "CoInitializeEx");
			m_needUninitialize = true;
			return;
		}

		MF::ThrowIfFailed(aptHr, "CoGetApartmentType");

		switch (aptType)
		{
		case APTTYPE_MTA:
			return;

		case APTTYPE_NA:
			throw Error{
				"Audio encoder cannot run on a neutral COM apartment thread. "
				"Use an MTA thread or dispatch to an engine-managed worker."
			};

		case APTTYPE_STA:
		case APTTYPE_MAINSTA:
		default:
			throw Error{
				"Current thread is already initialized as a non-MTA COM apartment. "
				"Audio encoder requires MTA on the calling thread, or dispatch to an engine-managed MTA worker."
			};
		}
	}

	AutoComMtaScope::~AutoComMtaScope()
	{
		if (m_needUninitialize)
		{
			::CoUninitialize();
		}
	}

	namespace MF
	{
		void ThrowIfFailed(HRESULT hr, const char* what)
		{
			if (FAILED(hr))
			{
				throw Error{ fmt::format("{} (HRESULT=0x{:08X})", what, static_cast<unsigned>(hr)) };
			}
		}

		Array<AudioEncoderCapability> EnumerateEncoderCapabilities(const AudioEncoderCodecConfig& codec)
		{
			ComPtr<IMFTransform> encoder;
			ThrowIfFailed(::CoCreateInstance(codec.encoderClsid, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(encoder.GetAddressOf())),
				fmt::format("CoCreateInstance({})", codec.name).c_str());

			Array<AudioEncoderCapability> capabilities;

			for (DWORD i = 0;; ++i)
			{
				ComPtr<IMFMediaType> type;
				const HRESULT hr = encoder->GetOutputAvailableType(0, i, type.GetAddressOf());

				if (hr == MF_E_NO_MORE_TYPES)
				{
					break;
				}

				ThrowIfFailed(hr, "GetOutputAvailableType");

				GUID major = {};
				GUID subtype = {};
				UINT32 sr = 0;
				UINT32 ch = 0;
				UINT32 avgBytesPerSec = 0;

				if (FAILED(type->GetGUID(MF_MT_MAJOR_TYPE, &major))) continue;
				if (FAILED(type->GetGUID(MF_MT_SUBTYPE, &subtype))) continue;
				if (FAILED(type->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sr))) continue;
				if (FAILED(type->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &ch))) continue;
				if (FAILED(type->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &avgBytesPerSec))) continue;

				if (major != MFMediaType_Audio) continue;
				if (subtype != codec.outputSubtype) continue;

				capabilities << AudioEncoderCapability{ sr, ch, avgBytesPerSec };
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

				return a.avgBytesPerSec < b.avgBytesPerSec;
			});

			LOG_DEBUG(fmt::format("{} encoder capabilities:", codec.name));
			for (const auto& cap : capabilities)
			{
				LOG_DEBUG(fmt::format("  - {} Hz, {} channels, {} avgBytesPerSec", cap.sampleRate, cap.channels, cap.avgBytesPerSec));
			}

			return capabilities;
		}

		bool EncodeWaveToCompressedAudio(const StereoSampleFloat32* wave, const size_t frameCount, const uint32 sampleRate, IWriter& writer,
			const AudioEncoderCodecConfig& codec, const Array<AudioEncoderCapability>& capabilities, const AudioQuality quality, const size_t inputChunkFrames) noexcept
		{
			try
			{
				if (not writer)
				{
					LOG_FAIL(fmt::format("{}: writer is not open", codec.name));
					return false;
				}

				AutoComMtaScope autoCom;

				ComPtr<IMFTransform> encoder = CreateConfiguredAudioEncoder(
					codec,
					capabilities,
					sampleRate,
					codec.channels,
					quality);

				uint64 framePos = 0;

				while (framePos < frameCount)
				{
					const size_t n = Min(inputChunkFrames, frameCount - static_cast<size_t>(framePos));

					Array<int16> pcm = InterleavePcm16(wave + framePos, n, codec.channels);
					const DWORD byteSize = static_cast<DWORD>(pcm.size() * sizeof(int16));

					ComPtr<IMFSample> inSample = CreateSampleFromBytes(pcm.data(), byteSize);

					const LONGLONG sampleTime =
						static_cast<LONGLONG>((framePos * 10000000ull) / sampleRate);

					const LONGLONG sampleDuration =
						static_cast<LONGLONG>((n * 10000000ull) / sampleRate);

					ThrowIfFailed(inSample->SetSampleTime(sampleTime), "SetSampleTime");
					ThrowIfFailed(inSample->SetSampleDuration(sampleDuration), "SetSampleDuration");

					for (;;)
					{
						const HRESULT hr = encoder->ProcessInput(0, inSample.Get(), 0);

						if (hr == MF_E_NOTACCEPTING)
						{
							if (not TryProcessAllAvailableOutput(encoder.Get(), writer))
							{
								ThrowIfFailed(hr, "ProcessInput returned MF_E_NOTACCEPTING but no output was produced");
							}
							continue;
						}

						ThrowIfFailed(hr, "ProcessInput");
						break;
					}

					TryProcessAllAvailableOutput(encoder.Get(), writer);

					framePos += n;
				}

				ThrowIfFailed(encoder->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, 0), "END_OF_STREAM");
				ThrowIfFailed(encoder->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, 0), "COMMAND_DRAIN");

				while (TryProcessAllAvailableOutput(encoder.Get(), writer))
				{
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
				AutoComMtaScope autoCom;

				Array<uint32> result;

				for (const auto& cap : capabilities)
				{
					if (cap.channels != channels)
					{
						continue;
					}

					if (not result.contains(cap.sampleRate))
					{
						result << cap.sampleRate;
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
				AutoComMtaScope autoCom;

				for (const auto& cap : capabilities)
				{
					if ((cap.sampleRate == sampleRate) && (cap.channels == channels))
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
