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
# include <Siv3D/MessageBoxResult.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/Utility.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array MessageBoxResultStrings =
		{
			U"Ok"_sv,
			U"Cancel"_sv,
			U"Yes"_sv,
			U"No"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const MessageBoxResult value)
	{
		formatData.string.append(MessageBoxResultStrings[FromEnum(value)]);
	}
}
