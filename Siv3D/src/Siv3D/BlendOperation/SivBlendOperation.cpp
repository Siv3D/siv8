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
# include <Siv3D/BlendOperation.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array BlendOperationStrings =
		{
			U""_sv,
			U"Add"_sv,
			U"Subtract"_sv,
			U"ReverseSubtract"_sv,
			U"Min"_sv,
			U"Max"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const BlendOperation value)
	{
		formatData.string.append(BlendOperationStrings[FromEnum(value)]);
	}
}
