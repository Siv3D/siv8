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

# include <array>
# include <Siv3D/AudioFormat.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array AudioFormatStrings =
		{
			U"Unknown"_sv,
			U"WAV"_sv,
			U"MP3"_sv,
			U"AAC"_sv,
			U"Ogg Vorbis"_sv,
			U"Opus"_sv,
			U"WMA"_sv,
			U"AIFF"_sv,
			U"FLAC"_sv,
			U"MIDI"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const AudioFormat value)
	{
		formatData.string.append(AudioFormatStrings[FromEnum(value)]);
	}
}
