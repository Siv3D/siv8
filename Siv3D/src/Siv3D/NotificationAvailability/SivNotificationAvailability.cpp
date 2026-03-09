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
# include <Siv3D/NotificationAvailability.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array NotificationAvailabilityStrings =
		{
			U"Unavailable"_sv,
			U"Denied"_sv,
			U"Available"_sv,
		};
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const NotificationAvailability value)
	{
		formatData.string.append(NotificationAvailabilityStrings[FromEnum(value)]);
	}
}
