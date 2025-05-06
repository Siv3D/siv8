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
# include <Siv3D/FontStyle.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array FontStyleStrings =
		{
			U"Normal"_sv,
			U"Bold"_sv,
			U"Italic"_sv,
			U"BoldItalic"_sv,
			U"Bitmap"_sv,
			U"BoldBitmap"_sv,
			U"ItalicBitmap"_sv,
			U"BoldItalicBitmap"_sv,
		};
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const FontStyle value)
	{
		formatData.string.append(FontStyleStrings[FromEnum(value)]);
	}
}
