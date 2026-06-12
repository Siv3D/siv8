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

# include <Siv3D/AudioFormat/WAVEncoder.hpp>
# include <Siv3D/BinaryFileWriter.hpp>
# include <Siv3D/MemoryWriter.hpp>
# include <Siv3D/EngineLog.hpp>
# include "WAVFormat.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr float ClampSample(const float value) noexcept
		{
			if ((-1.0f <= value) && (value <= 1.0f))
			{
				return value;
			}
	
			if (value < -1.0f)
			{
				return -1.0f;
			}

			if (1.0f < value)
			{
				return 1.0f;
			}
			
			return 0.0f; // NaN の場合は無音
		}

		[[nodiscard]]
		static constexpr uint8 Float32ToPCMUint8(const float value) noexcept
		{
			const float x = ClampSample(value);
			const float scaled = ((x * 127.5f) + 128.0f);
			return static_cast<uint8>((scaled <= 0.0f) ? 0 : ((255.0f <= scaled) ? 255 : static_cast<int32>(scaled)));
		}

		[[nodiscard]]
		static constexpr int16 Float32ToPCMInt16(const float value) noexcept
		{
			return StereoSampleFloat32::Float32ToInt16(ClampSample(value));
		}

		[[nodiscard]]
		static constexpr int32 Float32ToPCMInt24(const float value) noexcept
		{
			const float x = ClampSample(value);
			if (x < 0.0f)
			{
				return static_cast<int32>(x * 8388608.0f);
			}
			else
			{
				return static_cast<int32>(x * 8388607.0f);
			}
		}

		[[nodiscard]]
		static constexpr int32 Float32ToPCMInt32(const float value) noexcept
		{
			const float x = ClampSample(value);
			if (x < 0.0f)
			{
				return static_cast<int32>(x * 2147483648.0);
			}
			else
			{
				return static_cast<int32>(x * 2147483647.0);
			}
		}

		[[nodiscard]]
		static constexpr PCMInt24 Int32ToPCMInt24(const int32 value) noexcept
		{
			const uint32 u = static_cast<uint32>(value);
			return{ { static_cast<uint8>(u & 0xFF), static_cast<uint8>((u >> 8) & 0xFF), static_cast<uint8>((u >> 16) & 0xFF) } };
		}

		[[nodiscard]]
		static constexpr uint32 GetBitsPerSample(const WAVSaveFormat format) noexcept
		{
			switch (format)
			{
			case WAVSaveFormat::PCM8Mono:
			case WAVSaveFormat::PCM8Stereo:
				return 8;

			case WAVSaveFormat::PCM16Mono:
			case WAVSaveFormat::PCM16Stereo:
				return 16;

			case WAVSaveFormat::PCM24Mono:
			case WAVSaveFormat::PCM24Stereo:
				return 24;

			case WAVSaveFormat::PCM32Mono:
			case WAVSaveFormat::PCM32Stereo:
			case WAVSaveFormat::Float32Mono:
			case WAVSaveFormat::Float32Stereo:
				return 32;

			default:
				return 16;
			}
		}

		[[nodiscard]]
		static constexpr uint16 GetChannels(const WAVSaveFormat format) noexcept
		{
			switch (format)
			{
			case WAVSaveFormat::PCM8Mono:
			case WAVSaveFormat::PCM16Mono:
			case WAVSaveFormat::PCM24Mono:
			case WAVSaveFormat::PCM32Mono:
			case WAVSaveFormat::Float32Mono:
				return 1;
			default:
				return 2;
			}
		}

		[[nodiscard]]
		static constexpr float ToMono(const StereoSampleFloat32& s) noexcept
		{
			return ((s.left + s.right) * 0.5f);
		}

		[[nodiscard]]
		static constexpr uint16 GetFormatID(const WAVSaveFormat format) noexcept
		{
			return ((format == WAVSaveFormat::Float32Stereo) ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	StringView WAVEncoder::name() const noexcept
	{
		return U"WAV"_sv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	audioFormat
	//
	////////////////////////////////////////////////////////////////

	AudioFormat WAVEncoder::audioFormat() const noexcept
	{
		return AudioFormat::WAV;
	}

	////////////////////////////////////////////////////////////////
	//
	//	possibleExtensions
	//
	////////////////////////////////////////////////////////////////

	std::span<const StringView> WAVEncoder::possibleExtensions() const noexcept
	{
		static constexpr std::array<StringView, 1> Extensions = { U"wav" };
		return Extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool WAVEncoder::save(const Wave& wave, const FilePathView path) const
	{
		return save(wave, path, WAVSaveFormat::Default);
	}

	bool WAVEncoder::save(const Wave& wave, const FilePathView path, const WAVSaveFormat format) const
	{
		if (not wave)
		{
			LOG_FAIL("❌ WAVEncoder::save(): wave is empty");
			return false;
		}

		std::unique_ptr<BinaryFileWriter> writer = std::make_unique<BinaryFileWriter>();

		if (not writer->open(path))
		{
			LOG_FAIL(fmt::format("❌ WAVEncoder::save(): Failed to open the file: {}", path.toUTF8()));
			return false;
		}

		return encode(wave, *writer, format);
	}

	////////////////////////////////////////////////////////////////
	//
	//	encode
	//
	////////////////////////////////////////////////////////////////

	bool WAVEncoder::encode(const Wave& wave, IWriter& writer) const
	{
		return encode(wave, writer, WAVSaveFormat::Default);
	}

	bool WAVEncoder::encode(const Wave& wave, IWriter& writer, const WAVSaveFormat format) const
	{
		if ((not wave) || (not writer.isOpen()))
		{
			return false;
		}

		const uint16 channels = GetChannels(format);
		const uint16 bitsPerSample = static_cast<uint16>(GetBitsPerSample(format));
		const uint16 bytesPerSample = static_cast<uint16>(bitsPerSample / 8);
		const uint16 blockAlign = static_cast<uint16>(channels * bytesPerSample);
		const uint32 bytePerSec = (wave.sampleRate() * blockAlign);
		const uint32 dataChunkSize = static_cast<uint32>(wave.samples() * blockAlign);
		const uint32 riffFileSize = (4 + sizeof(ChunkHeader) + sizeof(FormatHeader) + sizeof(ChunkHeader) + dataChunkSize);

		const RIFFHeader riffHeader
		{
			.riff = { 'R', 'I', 'F', 'F' },
			.fileSize = riffFileSize,
			.type = { 'W', 'A', 'V', 'E' },
		};

		const ChunkHeader fmtChunkHeader
		{
			.chunkID = { 'f', 'm', 't', ' ' },
			.chunkSize = sizeof(FormatHeader),
		};

		const FormatHeader formatHeader
		{
			.formatID = GetFormatID(format),
			.channels = channels,
			.sampleRate = wave.sampleRate(),
			.bytePerSec = bytePerSec,
			.blockAlign = blockAlign,
			.bitsPerSample = bitsPerSample,
		};

		const ChunkHeader dataChunkHeader
		{
			.chunkID = { 'd', 'a', 't', 'a' },
			.chunkSize = dataChunkSize,
		};

		if ((not writer.write(riffHeader))
			|| (not writer.write(fmtChunkHeader))
			|| (not writer.write(formatHeader))
			|| (not writer.write(dataChunkHeader)))
		{
			return false;
		}

		const StereoSampleFloat32* pSrc = wave.data();
		const StereoSampleFloat32* const pSrcEnd = (pSrc + wave.samples());
		constexpr size_t FramesPerChunk = 4096;

		switch (format)
		{
		case WAVSaveFormat::PCM8Mono:
			{
				auto buffer = std::make_unique_for_overwrite<MonoSampleUint8[]>(FramesPerChunk);

				while (pSrc < pSrcEnd)
				{
					const size_t framesToWrite = Min<size_t>(FramesPerChunk, static_cast<size_t>(pSrcEnd - pSrc));

					for (size_t i = 0; i < framesToWrite; ++i)
					{
						buffer[i] = MonoSampleUint8{ Float32ToPCMUint8(ToMono(pSrc[i])) };
					}

					if (const size_t bytesToWrite = (framesToWrite * sizeof(MonoSampleUint8));
						writer.write(buffer.get(), bytesToWrite) != static_cast<int64>(bytesToWrite))
					{
						return false;
					}

					pSrc += framesToWrite;
				}

				break;
			}
		case WAVSaveFormat::PCM8Stereo:
			{
				auto buffer = std::make_unique_for_overwrite<StereoSampleUint8[]>(FramesPerChunk);

				while (pSrc < pSrcEnd)
				{
					const size_t framesToWrite = Min<size_t>(FramesPerChunk, static_cast<size_t>(pSrcEnd - pSrc));

					for (size_t i = 0; i < framesToWrite; ++i)
					{
						buffer[i] = StereoSampleUint8{ Float32ToPCMUint8(pSrc[i].left), Float32ToPCMUint8(pSrc[i].right) };
					}

					if (const size_t bytesToWrite = (framesToWrite * sizeof(StereoSampleUint8));
						writer.write(buffer.get(), bytesToWrite) != static_cast<int64>(bytesToWrite))
					{
						return false;
					}

					pSrc += framesToWrite;
				}

				break;
			}
		case WAVSaveFormat::PCM16Mono:
			{
				auto buffer = std::make_unique_for_overwrite<MonoSampleInt16[]>(FramesPerChunk);

				while (pSrc < pSrcEnd)
				{
					const size_t framesToWrite = Min<size_t>(FramesPerChunk, static_cast<size_t>(pSrcEnd - pSrc));

					for (size_t i = 0; i < framesToWrite; ++i)
					{
						buffer[i] = MonoSampleInt16{ Float32ToPCMInt16(ToMono(pSrc[i])) };
					}

					if (const size_t bytesToWrite = (framesToWrite * sizeof(MonoSampleInt16));
						writer.write(buffer.get(), bytesToWrite) != static_cast<int64>(bytesToWrite))
					{
						return false;
					}

					pSrc += framesToWrite;
				}

				break;
			}
		case WAVSaveFormat::PCM16Stereo:
			{
				auto buffer = std::make_unique_for_overwrite<StereoSampleInt16[]>(FramesPerChunk);

				while (pSrc < pSrcEnd)
				{
					const size_t framesToWrite = Min<size_t>(FramesPerChunk, static_cast<size_t>(pSrcEnd - pSrc));

					for (size_t i = 0; i < framesToWrite; ++i)
					{
						buffer[i] = StereoSampleInt16{ Float32ToPCMInt16(pSrc[i].left), Float32ToPCMInt16(pSrc[i].right) };
					}

					if (const size_t bytesToWrite = (framesToWrite * sizeof(StereoSampleInt16));
						writer.write(buffer.get(), bytesToWrite) != static_cast<int64>(bytesToWrite))
					{
						return false;
					}

					pSrc += framesToWrite;
				}

				break;
			}
		case WAVSaveFormat::PCM24Mono:
			{
				auto buffer = std::make_unique_for_overwrite<MonoSampleInt24[]>(FramesPerChunk);

				while (pSrc < pSrcEnd)
				{
					const size_t framesToWrite = Min<size_t>(FramesPerChunk, static_cast<size_t>(pSrcEnd - pSrc));

					for (size_t i = 0; i < framesToWrite; ++i)
					{
						buffer[i] = MonoSampleInt24
						{
							.value = Int32ToPCMInt24(Float32ToPCMInt24(ToMono(pSrc[i]))),
						};
					}

					if (const size_t bytesToWrite = (framesToWrite * sizeof(MonoSampleInt24));
						writer.write(buffer.get(), bytesToWrite) != static_cast<int64>(bytesToWrite))
					{
						return false;
					}

					pSrc += framesToWrite;
				}

				break;
			}
		case WAVSaveFormat::PCM24Stereo:
			{
				auto buffer = std::make_unique_for_overwrite<StereoSampleInt24[]>(FramesPerChunk);

				while (pSrc < pSrcEnd)
				{
					const size_t framesToWrite = Min<size_t>(FramesPerChunk, static_cast<size_t>(pSrcEnd - pSrc));

					for (size_t i = 0; i < framesToWrite; ++i)
					{
						buffer[i] = StereoSampleInt24
						{
							.left	= Int32ToPCMInt24(Float32ToPCMInt24(pSrc[i].left)),
							.right	= Int32ToPCMInt24(Float32ToPCMInt24(pSrc[i].right)),
						};
					}

					if (const size_t bytesToWrite = (framesToWrite * sizeof(StereoSampleInt24));
						writer.write(buffer.get(), bytesToWrite) != static_cast<int64>(bytesToWrite))
					{
						return false;
					}

					pSrc += framesToWrite;
				}

				break;
			}
		case WAVSaveFormat::PCM32Mono:
			{
				auto buffer = std::make_unique_for_overwrite<MonoSampleInt32[]>(FramesPerChunk);

				while (pSrc < pSrcEnd)
				{
					const size_t framesToWrite = Min<size_t>(FramesPerChunk, static_cast<size_t>(pSrcEnd - pSrc));

					for (size_t i = 0; i < framesToWrite; ++i)
					{
						buffer[i] = MonoSampleInt32{ Float32ToPCMInt32(ToMono(pSrc[i])) };
					}

					if (const size_t bytesToWrite = (framesToWrite * sizeof(MonoSampleInt32));
						writer.write(buffer.get(), bytesToWrite) != static_cast<int64>(bytesToWrite))
					{
						return false;
					}

					pSrc += framesToWrite;
				}

				break;
			}
		case WAVSaveFormat::PCM32Stereo:
			{
				auto buffer = std::make_unique_for_overwrite<StereoSampleInt32[]>(FramesPerChunk);

				while (pSrc < pSrcEnd)
				{
					const size_t framesToWrite = Min<size_t>(FramesPerChunk, static_cast<size_t>(pSrcEnd - pSrc));

					for (size_t i = 0; i < framesToWrite; ++i)
					{
						buffer[i] = StereoSampleInt32{ Float32ToPCMInt32(pSrc[i].left), Float32ToPCMInt32(pSrc[i].right) };
					}

					if (const size_t bytesToWrite = (framesToWrite * sizeof(StereoSampleInt32));
						writer.write(buffer.get(), bytesToWrite) != static_cast<int64>(bytesToWrite))
					{
						return false;
					}

					pSrc += framesToWrite;
				}

				break;
			}
		case WAVSaveFormat::Float32Mono:
			{
				auto buffer = std::make_unique_for_overwrite<float[]>(FramesPerChunk);

				while (pSrc < pSrcEnd)
				{
					const size_t framesToWrite = Min<size_t>(FramesPerChunk, static_cast<size_t>(pSrcEnd - pSrc));

					for (size_t i = 0; i < framesToWrite; ++i)
					{
						buffer[i] = ClampSample(ToMono(pSrc[i]));
					}

					if (const size_t bytesToWrite = (framesToWrite * sizeof(float));
						writer.write(buffer.get(), bytesToWrite) != static_cast<int64>(bytesToWrite))
					{
						return false;
					}

					pSrc += framesToWrite;
				}

				break;
			}
		case WAVSaveFormat::Float32Stereo:
			{
				auto buffer = std::make_unique_for_overwrite<StereoSampleFloat32[]>(FramesPerChunk);

				while (pSrc < pSrcEnd)
				{
					const size_t framesToWrite = Min<size_t>(FramesPerChunk, static_cast<size_t>(pSrcEnd - pSrc));

					for (size_t i = 0; i < framesToWrite; ++i)
					{
						buffer[i] = StereoSampleFloat32{ ClampSample(pSrc[i].left), ClampSample(pSrc[i].right) };
					}

					if (const size_t bytesToWrite = (framesToWrite * sizeof(StereoSampleFloat32));
						writer.write(buffer.get(), bytesToWrite) != static_cast<int64>(bytesToWrite))
					{
						return false;
					}

					pSrc += framesToWrite;
				}

				break;
			}
		default:
			{
				return false;
			}
		}

		return true;
	}

	Blob WAVEncoder::encode(const Wave& wave) const
	{
		return encode(wave, WAVSaveFormat::Default);
	}

	Blob WAVEncoder::encode(const Wave& wave, const WAVSaveFormat format) const
	{
		MemoryWriter writer;

		if (not encode(wave, writer, format))
		{
			return{};
		}

		return writer.extractBlob();
	}
}
