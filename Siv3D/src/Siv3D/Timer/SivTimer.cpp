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

# include <Siv3D/Timer.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	format
	//
	////////////////////////////////////////////////////////////////

	String Timer::format(const StringView format) const
	{
		return FormatTime(remaining(), format);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	std::ostream& operator <<(std::ostream& output, const Timer& value)
	{
		return (output << value.format());
	}

	std::wostream& operator <<(std::wostream& output, const Timer& value)
	{
		return (output << value.format());
	}

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& output, const Timer& value)
	{
		return (output << value.format());
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Timer& value)
	{
		formatData.string.append(value.format());
	}
}
