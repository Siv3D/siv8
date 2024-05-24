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
# include <Siv3D/CompareFunction.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array CompareFunctionStrings =
		{
			U""_sv,
			U"Never"_sv,
			U"Less"_sv,
			U"Equal"_sv,
			U"LessEqual"_sv,
			U"Greater"_sv,
			U"NotEqual"_sv,
			U"GreaterEqual"_sv,
			U"Always"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const CompareFunction value)
	{
		formatData.string.append(CompareFunctionStrings[FromEnum(value)]);
	}
}
