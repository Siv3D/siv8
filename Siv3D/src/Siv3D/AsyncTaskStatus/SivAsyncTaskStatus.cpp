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
# include <Siv3D/AsyncTaskStatus.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/Utility.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array AsyncTaskStatusStrings =
		{
			U"Invalid"_sv,
			U"Running"_sv,
			U"Ready"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const AsyncTaskStatus value)
	{
		formatData.string.append(AsyncTaskStatusStrings[FromEnum(value)]);
	}
}
