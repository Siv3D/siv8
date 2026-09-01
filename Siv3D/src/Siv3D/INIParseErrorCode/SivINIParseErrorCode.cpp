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
# include <Siv3D/INIParseErrorCode.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/Utility.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array INIParseErrorCodeStrings =
		{
			U"InvalidReader"_sv,
			U"InvalidEncoding"_sv,
			U"BareCR"_sv,
			U"MissingClosingBracket"_sv,
			U"EmptySectionName"_sv,
			U"UnexpectedCharacterAfterSection"_sv,
			U"MissingAssignment"_sv,
			U"EmptyKey"_sv,
			U"DuplicateSection"_sv,
			U"DuplicateKey"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const INIParseErrorCode value)
	{
		formatData.string.append(INIParseErrorCodeStrings[FromEnum(value)]);
	}
}

