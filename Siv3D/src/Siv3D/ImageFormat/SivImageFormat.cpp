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
# include <Siv3D/ImageFormat.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array ImageFormatStrings =
		{
			U"Unknown"_sv,
			U"DDS"_sv,
			U"PNG"_sv,
			U"JPEG"_sv,
			U"JPEG2000"_sv,
			U"JPEG_XL"_sv,
			U"BMP"_sv,
			U"WebP"_sv,
			U"GIF"_sv,
			U"TIFF"_sv,
			U"TGA"_sv,
			U"PPM"_sv,
			U"SVG"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const ImageFormat value)
	{
		formatData.string.append(ImageFormatStrings[FromEnum(value)]);
	}
}
