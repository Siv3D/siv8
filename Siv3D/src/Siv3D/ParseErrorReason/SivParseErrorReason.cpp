//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <array>
# include <Siv3D/ParseErrorReason.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array ParseErrorReasonStrings =
		{
			U"EmptyInput"_sv,
			U"InvalidRadix"_sv,
			U"InvalidFormat"_sv,
			U"NumericOverflow"_sv,
			U"InvalidValueForType"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, ParseErrorReason value)
	{
		formatData.string.append(ParseErrorReasonStrings[FromEnum(value)]);
	}
}
