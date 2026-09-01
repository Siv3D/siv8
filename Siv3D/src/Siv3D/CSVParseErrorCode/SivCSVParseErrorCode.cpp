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
# include <Siv3D/CSVParseErrorCode.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/Utility.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array CSVParseErrorCodeStrings =
		{
			U"InvalidReader"_sv,
			U"InvalidEncoding"_sv,
			U"UnexpectedQuoteInUnquotedField"_sv,
			U"UnexpectedCharacterAfterClosingQuote"_sv,
			U"UnterminatedQuotedField"_sv,
			U"BareCR"_sv,
			U"BareLF"_sv,
			U"InconsistentColumns"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const CSVParseErrorCode value)
	{
		formatData.string.append(CSVParseErrorCodeStrings[FromEnum(value)]);
	}
}

