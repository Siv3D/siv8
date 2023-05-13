//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <sstream>
# include <Siv3D/Int128.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/Unicode.hpp>

namespace s3d
{
	String ToString(const int128 value)
	{
		std::ostringstream os;
		os << value;
		return Unicode::WidenAscii(os.str());
	}

	String ToString(const uint128 value)
	{
		std::ostringstream os;
		os << value;
		return Unicode::WidenAscii(os.str());
	}

	void Formatter(FormatData& formatData, const int128 value)
	{
		formatData.string.append(ToString(value));
	}

	void Formatter(FormatData& formatData, const uint128 value)
	{
		formatData.string.append(ToString(value));
	}
}
