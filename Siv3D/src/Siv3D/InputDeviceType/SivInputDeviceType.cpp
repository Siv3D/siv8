//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <array>
# include <Siv3D/InputDeviceType.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array InputDeviceTypeStrings =
		{
			U"Unknown"_sv,
			U"Keyboard"_sv,
			U"Mouse"_sv,
			U"Gamepad"_sv,
			U"XInput"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const InputDeviceType value)
	{
		formatData.string.append(InputDeviceTypeStrings[FromEnum(value)]);
	}
}
