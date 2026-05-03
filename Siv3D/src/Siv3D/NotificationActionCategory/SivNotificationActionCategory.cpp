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
# include <Siv3D/NotificationActionCategory.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array NotificationActionCategoryStrings =
		{
			U"Default"_sv,
			U"Open"_sv,
			U"OpenDismiss"_sv,
			U"OK"_sv,
			U"OKCancel"_sv,
			U"YesNo"_sv,
			U"YesNoCancel"_sv,
			U"AcceptDecline"_sv,
			U"AcceptDeclineLater"_sv,
			U"RetryCancel"_sv,
			U"ViewDismiss"_sv,
		};
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const NotificationActionCategory value)
	{
		formatData.string.append(NotificationActionCategoryStrings[FromEnum(value)]);
	}
}
