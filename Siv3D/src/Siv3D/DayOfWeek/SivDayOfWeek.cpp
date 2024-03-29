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
# include <Siv3D/DayOfWeek.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array DayOfWeekStrings =
		{
			U"Sunday"_sv,
			U"Monday"_sv,
			U"Tuesday"_sv,
			U"Wednesday"_sv,
			U"Thursday"_sv,
			U"Friday"_sv,
			U"Saturday"_sv,
		};
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const DayOfWeek value)
	{
		formatData.string.append(DayOfWeekStrings[FromEnum(value)]);
	}
}
