//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Formatter.hpp>

namespace s3d
{
	constexpr StringView True{ U"true" };
	
	constexpr StringView False{ U"false" };

	void Formatter(FormatData& formatData, const bool value)
	{
		formatData.string.append(value ? True : False);
	}

	//void Formatter(FormatData& formatData, const int8 value)
	//{
	//	Formatter(formatData, static_cast<int32>(value));
	//}

	//void Formatter(FormatData& formatData, const uint8 value)
	//{
	//	Formatter(formatData, static_cast<uint32>(value));
	//}

	//void Formatter(FormatData& formatData, const int16 value)
	//{
	//	Formatter(formatData, static_cast<int32>(value));
	//}

	//void Formatter(FormatData& formatData, const uint16 value)
	//{
	//	Formatter(formatData, static_cast<uint32>(value));
	//}
}
