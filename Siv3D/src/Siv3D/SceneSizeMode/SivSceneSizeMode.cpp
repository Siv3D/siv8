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
# include <Siv3D/SceneSizeMode.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const SceneSizeMode value)
	{
		static constexpr std::array names{ U"Fixed"_sv, U"Window"_sv, U"WindowPixels"_sv };
		formatData.string.append(names[FromEnum(value)]);
	}
}
