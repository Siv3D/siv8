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
# include <Siv3D/JSONValueType.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array JSONValueTypeStrings =
		{
			U"Null"_sv,
			U"Object"_sv,
			U"Array"_sv,
			U"String"_sv,
			U"Bool"_sv,
			U"NumberInt"_sv,
			U"NumberUint"_sv,
			U"NumberFloat"_sv,
			U"Binary"_sv,
			U"Invalid"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const JSONValueType value)
	{
		formatData.string.append(JSONValueTypeStrings[FromEnum(value)]);
	}
}
