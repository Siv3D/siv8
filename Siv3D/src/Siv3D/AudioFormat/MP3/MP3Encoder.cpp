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

# include <Siv3D/AudioFormat/MP3Encoder.hpp>
# include <Siv3D/BinaryFileWriter.hpp>
# include <Siv3D/MemoryWriter.hpp>
# include <Siv3D/MediaTranscoder/IMediaTranscoder.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	StringView MP3Encoder::name() const noexcept
	{
		return U"MP3"_sv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	audioFormat
	//
	////////////////////////////////////////////////////////////////

	AudioFormat MP3Encoder::audioFormat() const noexcept
	{
		return AudioFormat::MP3;
	}

	////////////////////////////////////////////////////////////////
	//
	//	possibleExtensions
	//
	////////////////////////////////////////////////////////////////

	std::span<const StringView> MP3Encoder::possibleExtensions() const noexcept
	{
		static constexpr std::array<StringView, 1> Extensions = { U"mp3" };
		return Extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool MP3Encoder::save(const Wave& wave, const FilePathView path) const
	{
		return save(wave, path, MP3Quality::High);
	}

	bool MP3Encoder::save(const Wave& wave, const FilePathView path, const MP3Quality quality) const
	{
		std::unique_ptr<BinaryFileWriter> writer = std::make_unique<BinaryFileWriter>();

		if (not writer->open(path))
		{
			LOG_FAIL(U"❌ MP3Encoder::save(): Failed to open the file: {}"_fmt(path));
			return false;
		}

		return encode(wave, *writer, quality);
	}

	////////////////////////////////////////////////////////////////
	//
	//	encode
	//
	////////////////////////////////////////////////////////////////

	bool MP3Encoder::encode(const Wave& wave, IWriter& writer) const
	{
		return encode(wave, writer, MP3Quality::High);
	}

	bool MP3Encoder::encode(const Wave& wave, IWriter& writer, const MP3Quality quality) const
	{
		if ((not wave) || (not writer.isOpen()))
		{
			return false;
		}

		return SIV3D_ENGINE(MediaTranscoder)->encodeMP3(wave, writer, quality);
	}

	Blob MP3Encoder::encode(const Wave& wave) const
	{
		return encode(wave, MP3Quality::High);
	}

	Blob MP3Encoder::encode(const Wave& wave, const MP3Quality quality) const
	{
		MemoryWriter writer;

		if (not encode(wave, writer, quality))
		{
			return{};
		}

		return writer.extractBlob();
	}

	////////////////////////////////////////////////////////////////
	//
	//	availableSampleRates
	//
	////////////////////////////////////////////////////////////////

	Array<uint32> MP3Encoder::availableSampleRates() const
	{
		return SIV3D_ENGINE(MediaTranscoder)->getAvailableMP3SampleRates();
	}
}
