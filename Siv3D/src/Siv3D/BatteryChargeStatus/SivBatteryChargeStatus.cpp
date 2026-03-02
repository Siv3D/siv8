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
# include <Siv3D/BatteryChargeStatus.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array BatteryChargeStatusStrings =
		{
			U"Unknown"_sv,
			U"Discharging"_sv,
			U"Charging"_sv,
			U"NotCharging"_sv,
			U"Full"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const BatteryChargeStatus value)
	{
		formatData.string.append(BatteryChargeStatusStrings[FromEnum(value)]);
	}
}
