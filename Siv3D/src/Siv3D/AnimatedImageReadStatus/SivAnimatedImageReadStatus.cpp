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
# include <Siv3D/AnimatedImageReadStatus.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/Utility.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array AnimatedImageReadStatusStrings =
		{
			U"Frame"_sv,
			U"EndOfStream"_sv,
			U"Error"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const AnimatedImageReadStatus value)
	{
		formatData.string.append(AnimatedImageReadStatusStrings[FromEnum(value)]);
	}
}

