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
# include <Siv3D/TextureAddressMode.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array TextureAddressModeStrings =
		{
			U""_sv,
			U"Repeat"_sv,
			U"Mirror"_sv,
			U"MirrorClamp"_sv,
			U"Clamp"_sv,
			U"BorderColor"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const TextureAddressMode value)
	{
		formatData.string.append(TextureAddressModeStrings[FromEnum(value)]);
	}
}
