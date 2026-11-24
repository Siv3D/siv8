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
# include <Siv3D/MSAASampleCount.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array MSAASampleCountStrings =
		{
			U""_sv,
			U"NoMSAA"_sv,
			U"X2MSAA"_sv,
			U""_sv,
			U"X4MSAA"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const MSAASampleCount value)
	{
		formatData.string.append(MSAASampleCountStrings[FromEnum(value)]);
	}
}
