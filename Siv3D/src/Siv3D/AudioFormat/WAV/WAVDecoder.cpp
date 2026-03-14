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

# include <Siv3D/AudioFormat/WAVDecoder.hpp>
# include <Siv3D/IReader.hpp>
# include <Siv3D/EngineLog.hpp>
# include "WAVFormat.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr uint32 GetPaddedChunkSize(const uint32 chunkSize) noexcept
		{
			return (chunkSize + (chunkSize & 1u));
		}

		[[nodiscard]]
		static constexpr float PCMUint8ToFloat32(const uint8 value) noexcept
		{
			return ((static_cast<float>(value) - 128.0f) / 128.0f);
		}

		[[nodiscard]]
		static constexpr float PCMInt16ToFloat32(const int16 value) noexcept
		{
			return ((value < 0) ? (static_cast<float>(value) / 32768.0f) : (static_cast<float>(value) / 32767.0f));
		}

		[[nodiscard]]
		static constexpr int32 PCMInt24ToInt32(const PCMInt24& value) noexcept
		{
			const uint32 u = (static_cast<uint32>(value.b[0]))
				| (static_cast<uint32>(value.b[1]) << 8)
				| (static_cast<uint32>(value.b[2]) << 16);

			return ((u & 0x00800000u)
				? static_cast<int32>(u | 0xFF000000u)
				: static_cast<int32>(u));
		}

		[[nodiscard]]
		static constexpr float PCMInt24ToFloat32(const PCMInt24& value) noexcept
		{
			const int32 s = PCMInt24ToInt32(value);
			return ((s < 0) ? (static_cast<float>(s) / 8388608.0f) : (static_cast<float>(s) / 8388607.0f));
		}

		[[nodiscard]]
		static constexpr float PCMInt32ToFloat32(const int32 value) noexcept
		{
			return ((value < 0) ? (static_cast<float>(value) / 2147483648.0f) : (static_cast<float>(value) / 2147483647.0f));
		}

		static constexpr size_t BufferFrameCount = 4096;

		static void ReadMono8(IReader& reader, size_t numFrames, StereoSampleFloat32* pDst)
		{
			Array<MonoSampleUint8> buffer(BufferFrameCount);

			while (0 < numFrames)
			{
				const size_t framesToRead = Min(numFrames, BufferFrameCount);
				reader.read(buffer.data(), (framesToRead * sizeof(MonoSampleUint8)));

				const MonoSampleUint8* pSrc = buffer.data();
				const MonoSampleUint8* const pSrcEnd = (pSrc + framesToRead);

				while (pSrc != pSrcEnd)
				{
					const float s = PCMUint8ToFloat32(pSrc->value);
					pDst->left = s;
					pDst->right = s;
					++pSrc;
					++pDst;
				}

				numFrames -= framesToRead;
			}
		}

		static void ReadStereo8(IReader& reader, size_t numFrames, StereoSampleFloat32* pDst)
		{
			Array<StereoSampleUint8> buffer(BufferFrameCount);

			while (0 < numFrames)
			{
				const size_t framesToRead = Min(numFrames, BufferFrameCount);
				reader.read(buffer.data(), (framesToRead * sizeof(StereoSampleUint8)));

				const StereoSampleUint8* pSrc = buffer.data();
				const StereoSampleUint8* const pSrcEnd = (pSrc + framesToRead);

				while (pSrc != pSrcEnd)
				{
					pDst->left = PCMUint8ToFloat32(pSrc->left);
					pDst->right = PCMUint8ToFloat32(pSrc->right);
					++pSrc;
					++pDst;
				}

				numFrames -= framesToRead;
			}
		}

		static void ReadMono16(IReader& reader, size_t numFrames, StereoSampleFloat32* pDst)
		{
			Array<MonoSampleInt16> buffer(BufferFrameCount);

			while (0 < numFrames)
			{
				const size_t framesToRead = Min(numFrames, BufferFrameCount);
				reader.read(buffer.data(), (framesToRead * sizeof(MonoSampleInt16)));

				const MonoSampleInt16* pSrc = buffer.data();
				const MonoSampleInt16* const pSrcEnd = (pSrc + framesToRead);

				while (pSrc != pSrcEnd)
				{
					const float s = PCMInt16ToFloat32(pSrc->value);
					pDst->left = s;
					pDst->right = s;
					++pSrc;
					++pDst;
				}

				numFrames -= framesToRead;
			}
		}

		static void ReadStereo16(IReader& reader, size_t numFrames, StereoSampleFloat32* pDst)
		{
			Array<StereoSampleInt16> buffer(BufferFrameCount);

			while (0 < numFrames)
			{
				const size_t framesToRead = Min(numFrames, BufferFrameCount);
				reader.read(buffer.data(), (framesToRead * sizeof(StereoSampleInt16)));

				const StereoSampleInt16* pSrc = buffer.data();
				const StereoSampleInt16* const pSrcEnd = (pSrc + framesToRead);

				while (pSrc != pSrcEnd)
				{
					pDst->left = PCMInt16ToFloat32(pSrc->left);
					pDst->right = PCMInt16ToFloat32(pSrc->right);
					++pSrc;
					++pDst;
				}

				numFrames -= framesToRead;
			}
		}

		static void ReadMono24(IReader& reader, size_t numFrames, StereoSampleFloat32* pDst)
		{
			Array<MonoSampleInt24> buffer(BufferFrameCount);

			while (0 < numFrames)
			{
				const size_t framesToRead = Min(numFrames, BufferFrameCount);
				reader.read(buffer.data(), (framesToRead * sizeof(MonoSampleInt24)));

				const MonoSampleInt24* pSrc = buffer.data();
				const MonoSampleInt24* const pSrcEnd = (pSrc + framesToRead);

				while (pSrc != pSrcEnd)
				{
					const float s = PCMInt24ToFloat32(pSrc->value);
					pDst->left = s;
					pDst->right = s;
					++pSrc;
					++pDst;
				}

				numFrames -= framesToRead;
			}
		}

		static void ReadStereo24(IReader& reader, size_t numFrames, StereoSampleFloat32* pDst)
		{
			Array<StereoSampleInt24> buffer(BufferFrameCount);

			while (0 < numFrames)
			{
				const size_t framesToRead = Min(numFrames, BufferFrameCount);
				reader.read(buffer.data(), (framesToRead * sizeof(StereoSampleInt24)));

				const StereoSampleInt24* pSrc = buffer.data();
				const StereoSampleInt24* const pSrcEnd = (pSrc + framesToRead);

				while (pSrc != pSrcEnd)
				{
					pDst->left = PCMInt24ToFloat32(pSrc->left);
					pDst->right = PCMInt24ToFloat32(pSrc->right);
					++pSrc;
					++pDst;
				}

				numFrames -= framesToRead;
			}
		}

		static void ReadMono32(IReader& reader, size_t numFrames, StereoSampleFloat32* pDst)
		{
			Array<MonoSampleInt32> buffer(BufferFrameCount);

			while (0 < numFrames)
			{
				const size_t framesToRead = Min(numFrames, BufferFrameCount);
				reader.read(buffer.data(), (framesToRead * sizeof(MonoSampleInt32)));

				const MonoSampleInt32* pSrc = buffer.data();
				const MonoSampleInt32* const pSrcEnd = (pSrc + framesToRead);
				
				while (pSrc != pSrcEnd)
				{
					const float s = PCMInt32ToFloat32(pSrc->value);
					pDst->left = s;
					pDst->right = s;
					++pSrc;
					++pDst;
				}
				
				numFrames -= framesToRead;
			}
		}

		static void ReadStereo32(IReader& reader, size_t numFrames, StereoSampleFloat32* pDst)
		{
			Array<StereoSampleInt32> buffer(BufferFrameCount);

			while (0 < numFrames)
			{
				const size_t framesToRead = Min(numFrames, BufferFrameCount);
				reader.read(buffer.data(), (framesToRead * sizeof(StereoSampleInt32)));
				
				const StereoSampleInt32* pSrc = buffer.data();
				const StereoSampleInt32* const pSrcEnd = (pSrc + framesToRead);
				
				while (pSrc != pSrcEnd)
				{
					pDst->left = PCMInt32ToFloat32(pSrc->left);
					pDst->right = PCMInt32ToFloat32(pSrc->right);
					++pSrc;
					++pDst;
				}
				
				numFrames -= framesToRead;
			}
		}

		static void ReadMonoFloat32(IReader& reader, size_t numFrames, StereoSampleFloat32* pDst)
		{
			Array<float> buffer(BufferFrameCount);

			while (0 < numFrames)
			{
				const size_t framesToRead = Min(numFrames, BufferFrameCount);
				reader.read(buffer.data(), (framesToRead * sizeof(float)));

				const float* pSrc = buffer.data();
				const float* const pSrcEnd = (pSrc + framesToRead);

				while (pSrc != pSrcEnd)
				{
					const float s = *pSrc;
					pDst->left = s;
					pDst->right = s;
					++pSrc;
					++pDst;
				}

				numFrames -= framesToRead;
			}
		}

		static void ReadStereoFloat32(IReader& reader, size_t numFrames, StereoSampleFloat32* pDst)
		{
			reader.read(pDst, (numFrames * sizeof(StereoSampleFloat32)));
		}

		static void ReadMonoFloat64(IReader& reader, size_t numFrames, StereoSampleFloat32* pDst)
		{
			Array<double> buffer(BufferFrameCount);

			while (0 < numFrames)
			{
				const size_t framesToRead = Min(numFrames, BufferFrameCount);
				reader.read(buffer.data(), (framesToRead * sizeof(double)));
				
				const double* pSrc = buffer.data();
				const double* const pSrcEnd = (pSrc + framesToRead);
				
				while (pSrc != pSrcEnd)
				{
					const float s = static_cast<float>(*pSrc);
					pDst->left = s;
					pDst->right = s;
					++pSrc;
					++pDst;
				}
				
				numFrames -= framesToRead;
			}
		}

		static void ReadStereoFloat64(IReader& reader, size_t numFrames, StereoSampleFloat32* pDst)
		{
			Array<StereoSampleFloat64> buffer(BufferFrameCount);

			while (0 < numFrames)
			{
				const size_t framesToRead = Min(numFrames, BufferFrameCount);
				reader.read(buffer.data(), (framesToRead * sizeof(StereoSampleFloat64)));
				
				const StereoSampleFloat64* pSrc = buffer.data();
				const StereoSampleFloat64* const pSrcEnd = (pSrc + framesToRead);
				
				while (pSrc != pSrcEnd)
				{
					pDst->left = static_cast<float>(pSrc->left);
					pDst->right = static_cast<float>(pSrc->right);
					++pSrc;
					++pDst;
				}
				
				numFrames -= framesToRead;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	StringView WAVDecoder::name() const noexcept
	{
		return U"WAV"_sv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	audioFormat
	//
	////////////////////////////////////////////////////////////////

	AudioFormat WAVDecoder::audioFormat() const noexcept
	{
		return AudioFormat::WAV;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isHeader
	//
	////////////////////////////////////////////////////////////////

	bool WAVDecoder::isHeader(const uint8(&bytes)[RequiredHeaderBytes]) const noexcept
	{
		static_assert(12 <= RequiredHeaderBytes);
		return (std::memcmp(bytes, RIFFHeader::RIFFSignature, sizeof(RIFFHeader::RIFFSignature)) == 0)
			&& (std::memcmp((bytes + 8), RIFFHeader::WAVEFormat, sizeof(RIFFHeader::WAVEFormat)) == 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	possibleExtensions
	//
	////////////////////////////////////////////////////////////////

	std::span<const StringView> WAVDecoder::possibleExtensions() const noexcept
	{
		static constexpr std::array<StringView, 1> Extensions = { U"wav" };
		return Extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	decode
	//
	////////////////////////////////////////////////////////////////

	Wave WAVDecoder::decode(const FilePathView path) const
	{
		return IAudioDecoder::decode(path);
	}

	Wave WAVDecoder::decode(std::unique_ptr<IReader> reader, [[maybe_unused]] const FilePathView pathHint) const
	{
		if ((not reader) || (not reader->isOpen()))
		{
			return{};
		}

		// RIFF ヘッダを読み込む
		{
			RIFFHeader riffHeader{};
			if (not reader->read(riffHeader))
			{
				return{};
			}

			if ((not MemEqual(riffHeader.riff, RIFFHeader::RIFFSignature))
				|| (not MemEqual(riffHeader.type, RIFFHeader::WAVEFormat)))
			{
				return{};
			}
		}

		// fmt チャンクを探す
		ChunkHeader chunkHeader{};
		for (;;)
		{
			if (not reader->read(chunkHeader))
			{
				return{};
			}

			if (MemEqual(chunkHeader.chunkID, ChunkHeader::fmtChunkID))
			{
				break;
			}
			else
			{
				reader->skip(GetPaddedChunkSize(chunkHeader.chunkSize));
			}
		}

		// fmt チャンク全体をバッファに読み込む
		const uint32 paddedFmtSize = GetPaddedChunkSize(chunkHeader.chunkSize);
		Array<uint8> fmtChunk(paddedFmtSize);
		if (reader->read(fmtChunk.data(), fmtChunk.size()) != static_cast<int64>(fmtChunk.size_bytes()))
		{
			return{};
		}

		if (chunkHeader.chunkSize < sizeof(FormatHeader))
		{
			LOG_FAIL("Invalid WAV: fmt chunk is too small");
			return{};
		}

		FormatHeader formatHeader{};
		std::memcpy(&formatHeader, fmtChunk.data(), sizeof(FormatHeader));

		uint16 actualFormatID = formatHeader.formatID;
		if ((actualFormatID == 0xFFFE) && (40 <= chunkHeader.chunkSize))
		{
			uint16 cbSize = 0;
			std::memcpy(&cbSize, fmtChunk.data() + 16, 2);
			if (22 <= cbSize)
			{
				// SubFormat GUID の先頭2バイトが実際の formatID
				std::memcpy(&actualFormatID, (fmtChunk.data() + 24), 2);
			}
		}

		// data チャンクを探す
		for (;;)
		{
			if (not reader->read(chunkHeader))
			{
				return{};
			}

			if (MemEqual(chunkHeader.chunkID, ChunkHeader::dataChunkID))
			{
				break;
			}
			else
			{
				reader->skip(GetPaddedChunkSize(chunkHeader.chunkSize));
			}
		}

		if ((formatHeader.channels == 0) || (formatHeader.bitsPerSample == 0))
		{
			LOG_FAIL("Invalid WAV: zero channels or bitsPerSample");
			return{};
		}

		if ((formatHeader.bitsPerSample % 8) != 0)
		{
			LOG_FAIL("Invalid WAV: bitsPerSample is not byte-aligned");
			return{};
		}

		const uint32 size_bytes = chunkHeader.chunkSize;
		const size_t numFrames = (size_bytes / (formatHeader.channels * (formatHeader.bitsPerSample / 8)));
		const uint32 sampleRate = formatHeader.sampleRate;
		const uint16 bitsPerSample = formatHeader.bitsPerSample;
		const uint16 channels = formatHeader.channels;

		const bool isPCM = (actualFormatID == 0x0001);
		const bool isFloat = (actualFormatID == 0x0003);

		Wave wave{ numFrames, Arg::sampleRate = sampleRate };
		StereoSampleFloat32* pDst = wave.data();

		if (isPCM && (bitsPerSample == 8) && (channels == 1))
		{
			ReadMono8(*reader, numFrames, pDst);
		}
		else if (isPCM && (bitsPerSample == 8) && (channels == 2))
		{
			ReadStereo8(*reader, numFrames, pDst);
		}
		else if (isPCM && (bitsPerSample == 16) && (channels == 1))
		{
			ReadMono16(*reader, numFrames, pDst);
		}
		else if (isPCM && (bitsPerSample == 16) && (channels == 2))
		{
			ReadStereo16(*reader, numFrames, pDst);
		}
		else if (isPCM && (bitsPerSample == 24) && (channels == 1))
		{
			ReadMono24(*reader, numFrames, pDst);
		}
		else if (isPCM && (bitsPerSample == 24) && (channels == 2))
		{
			ReadStereo24(*reader, numFrames, pDst);
		}
		else if (isPCM && (bitsPerSample == 32) && (channels == 1))
		{
			ReadMono32(*reader, numFrames, pDst);
		}
		else if (isPCM && (bitsPerSample == 32) && (channels == 2))
		{
			ReadStereo32(*reader, numFrames, pDst);
		}
		else if (isFloat && (bitsPerSample == 32) && (channels == 1))
		{
			ReadMonoFloat32(*reader, numFrames, pDst);
		}
		else if (isFloat && (bitsPerSample == 32) && (channels == 2))
		{
			ReadStereoFloat32(*reader, numFrames, pDst);
		}
		else if (isFloat && (bitsPerSample == 64) && (channels == 1))
		{
			ReadMonoFloat64(*reader, numFrames, pDst);
		}
		else if (isFloat && (bitsPerSample == 64) && (channels == 2))
		{
			ReadStereoFloat64(*reader, numFrames, pDst);
		}
		else
		{
			LOG_FAIL(fmt::format("Unsupported WAV format: formatID = {0}, channels = {1}, bitsPerSample = {2}", actualFormatID, channels, bitsPerSample));
			return{};
		}

		return wave;
	}
}
