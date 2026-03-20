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

# include <Siv3D/AudioFormat/MP3Decoder.hpp>
# include <Siv3D/IReader.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	StringView MP3Decoder::name() const noexcept
	{
		return U"MP3"_sv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	audioFormat
	//
	////////////////////////////////////////////////////////////////

	AudioFormat MP3Decoder::audioFormat() const noexcept
	{
		return AudioFormat::MP3;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isHeader
	//
	////////////////////////////////////////////////////////////////

	bool MP3Decoder::isHeader(const uint8(&bytes)[RequiredHeaderBytes]) const noexcept
	{
		static_assert(3 <= RequiredHeaderBytes);
		static constexpr uint8 MPEG1_SIGN[] = { 0xFF, 0xFB };
		static constexpr uint8 MP3_SIGN[] = { 0x49, 0x44, 0x33 };
		return (std::memcmp(bytes, MPEG1_SIGN, sizeof(MPEG1_SIGN)) == 0)
			|| (std::memcmp(bytes, MP3_SIGN, sizeof(MP3_SIGN)) == 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	possibleExtensions
	//
	////////////////////////////////////////////////////////////////

	std::span<const StringView> MP3Decoder::possibleExtensions() const noexcept
	{
		static constexpr std::array<StringView, 1> Extensions = { U"mp3" };
		return Extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	decode
	//
	////////////////////////////////////////////////////////////////

	Wave MP3Decoder::decode(const FilePathView path) const
	{
		return IAudioDecoder::decode(path);
	}

	Wave MP3Decoder::decode(std::unique_ptr<IReader> reader, [[maybe_unused]] const FilePathView pathHint) const
	{
		if ((not reader) || (not reader->isOpen()))
		{
			return{};
		}

		Wave wave;

		return wave;
	}
}
