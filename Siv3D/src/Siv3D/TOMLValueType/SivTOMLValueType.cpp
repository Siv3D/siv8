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
# include <Siv3D/TOMLValueType.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array TOMLValueTypeStrings =
		{
			U"Invalid"_sv,
			U"Bool"_sv,
			U"Int"_sv,
			U"Float"_sv,
			U"String"_sv,
			U"OffsetDateTime"_sv,
			U"LocalDateTime"_sv,
			U"LocalDate"_sv,
			U"LocalTime"_sv,
			U"Array"_sv,
			U"Table"_sv,
		};
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const TOMLValueType value)
	{
		formatData.string.append(TOMLValueTypeStrings[FromEnum(value)]);
	}
}
