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
# include <Siv3D/AnimatedImageDecodeError.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/Utility.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array AnimatedImageDecodeErrorStrings =
		{
			U"None"_sv,
			U"InvalidFormat"_sv,
			U"NotAnimated"_sv,
			U"InvalidData"_sv,
			U"TruncatedInput"_sv,
			U"ReadError"_sv,
			U"UnsupportedFeature"_sv,
			U"TooManyFrames"_sv,
			U"DecodedBytesLimitExceeded"_sv,
			U"WorkingMemoryLimitExceeded"_sv,
			U"OutOfMemory"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const AnimatedImageDecodeError value)
	{
		formatData.string.append(AnimatedImageDecodeErrorStrings[FromEnum(value)]);
	}
}

