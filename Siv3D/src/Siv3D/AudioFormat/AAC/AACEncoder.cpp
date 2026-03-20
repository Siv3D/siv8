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

# include <Siv3D/AudioFormat/AACEncoder.hpp>
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

	StringView AACEncoder::name() const noexcept
	{
		return U"AAC"_sv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	audioFormat
	//
	////////////////////////////////////////////////////////////////

	AudioFormat AACEncoder::audioFormat() const noexcept
	{
		return AudioFormat::AAC;
	}

	////////////////////////////////////////////////////////////////
	//
	//	possibleExtensions
	//
	////////////////////////////////////////////////////////////////

	std::span<const StringView> AACEncoder::possibleExtensions() const noexcept
	{
		static constexpr std::array<StringView, 1> Extensions = { U"aac" };
		return Extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool AACEncoder::save(const Wave& wave, const FilePathView path) const
	{
		return save(wave, path, AACQuality::High);
	}

	bool AACEncoder::save(const Wave& wave, const FilePathView path, const AACQuality quality) const
	{
		std::unique_ptr<BinaryFileWriter> writer = std::make_unique<BinaryFileWriter>();

		if (not writer->open(path))
		{
			LOG_FAIL(U"❌ AACEncoder::save(): Failed to open the file: {}"_fmt(path));
			return false;
		}

		return encode(wave, *writer, quality);
	}

	////////////////////////////////////////////////////////////////
	//
	//	encode
	//
	////////////////////////////////////////////////////////////////

	bool AACEncoder::encode(const Wave& wave, IWriter& writer) const
	{
		return encode(wave, writer, AACQuality::High);
	}

	bool AACEncoder::encode(const Wave& wave, IWriter& writer, const AACQuality quality) const
	{
		if ((not wave) || (not writer.isOpen()))
		{
			return false;
		}

		return SIV3D_ENGINE(MediaTranscoder)->encodeAAC(wave, writer, quality);
	}

	Blob AACEncoder::encode(const Wave& wave) const
	{
		return encode(wave, AACQuality::High);
	}

	Blob AACEncoder::encode(const Wave& wave, const AACQuality quality) const
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

	Array<uint32> AACEncoder::availableSampleRates() const
	{
		return SIV3D_ENGINE(MediaTranscoder)->getAvailableAACSampleRates();
	}
}
