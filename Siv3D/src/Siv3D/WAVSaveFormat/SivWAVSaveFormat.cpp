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
# include <Siv3D/WAVSaveFormat.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array WAVSaveFormatStrings =
		{
			U"PCM8Stereo"_sv,
			U"PCM16Stereo"_sv,
			U"PCM24Stereo"_sv,
			U"PCM32Stereo"_sv,
			U"Float32Stereo"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const WAVSaveFormat value)
	{
		formatData.string.append(WAVSaveFormatStrings[FromEnum(value)]);
	}
}
