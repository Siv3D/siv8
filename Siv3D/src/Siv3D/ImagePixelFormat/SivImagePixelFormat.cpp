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
# include <Siv3D/ImagePixelFormat.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array ImagePixelFormatStrings =
		{
			U"Unknown"_sv,
			U"R8G8B8"_sv,
			U"R8G8B8X8"_sv,
			U"R8G8B8A8"_sv,
			U"Gray8"_sv,
			U"Gray16"_sv,
			U"Gray8A8"_sv,
			U"Gray16A16"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const ImagePixelFormat value)
	{
		formatData.string.append(ImagePixelFormatStrings[FromEnum(value)]);
	}
}
