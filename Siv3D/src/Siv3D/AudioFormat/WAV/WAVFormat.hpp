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
# include <Siv3D/Types.hpp>

namespace s3d
{
	struct RIFFHeader
	{
		uint8 riff[4];
		uint32 fileSize;
		uint8 type[4];

		static constexpr uint8 RIFFSignature[] = { 'R', 'I', 'F', 'F' };
		static constexpr uint8 WAVEFormat[] = { 'W', 'A', 'V', 'E' };
	};
	static_assert(sizeof(RIFFHeader) == 12);

	struct ChunkHeader
	{
		uint8 chunkID[4];
		uint32 chunkSize;

		static constexpr uint8 fmtChunkID[] = { 'f', 'm', 't', ' ' };
		static constexpr uint8 dataChunkID[] = { 'd', 'a', 't', 'a' };
	};
	static_assert(sizeof(ChunkHeader) == 8);

	struct FormatHeader
	{
		uint16 formatID;
		uint16 channels;
		uint32 sampleRate;
		uint32 bytePerSec;
		uint16 blockAlign;
		uint16 bitsPerSample;
	};
	static_assert(sizeof(FormatHeader) == 16);

	struct MonoSampleUint8
	{
		uint8 value = 128;
	};

	struct StereoSampleUint8
	{
		uint8 left = 128;
		uint8 right = 128;
	};

	struct MonoSampleInt16
	{
		int16 value = 0;
	};

	struct PCMInt24
	{
		uint8 b[3];
	};

	struct MonoSampleInt24
	{
		PCMInt24 value;
	};

	struct StereoSampleInt24
	{
		PCMInt24 left;
		PCMInt24 right;
	};

	struct MonoSampleInt32
	{
		int32 value = 0;
	};
	
	struct StereoSampleInt32
	{
		int32 left = 0;
		int32 right = 0;
	};

	struct StereoSampleFloat64
	{
		double left = 0.0;
		double right = 0.0;
	};

	template <size_t Size>
	[[nodiscard]]
	bool MemEqual(const uint8(&lhs)[Size], const uint8(&rhs)[Size]) noexcept
	{
		return (std::memcmp(lhs, rhs, Size) == 0);
	}
}
