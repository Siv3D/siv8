//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <array>
# include <Siv3D/WebBrowser.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array WebBrowserStrings =
		{
			U"Chrome"_sv,
			U"Firefox"_sv,
			U"Edge"_sv,
			U"Safari"_sv,
			U"Opera"_sv,
			U"Brave"_sv,
			U"Vivaldi"_sv,
		};
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const WebBrowser value)
	{
		formatData.string.append(WebBrowserStrings[FromEnum(value)]);
	}
}
