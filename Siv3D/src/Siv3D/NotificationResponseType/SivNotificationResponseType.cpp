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
# include <Siv3D/NotificationResponseType.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array NotificationResponseTypeStrings =
		{
			U"Unknown"_sv,
			U"DefaultActivated"_sv,
			U"ActionActivated"_sv,
			U"Dismissed"_sv,
			U"Failed"_sv,
		};
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const NotificationResponseType value)
	{
		formatData.string.append(NotificationResponseTypeStrings[FromEnum(value)]);
	}
}
