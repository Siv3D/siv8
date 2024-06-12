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

# include <Siv3D/Stopwatch.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	format
	//
	////////////////////////////////////////////////////////////////

	String Stopwatch::format(const StringView format) const
	{
		return FormatTime(elapsed(), format);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	std::ostream& operator <<(std::ostream& os, const Stopwatch& value)
	{
		return (os << value.format());
	}

	std::wostream& operator <<(std::wostream& os, const Stopwatch& value)
	{
		return (os << value.format());
	}

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const Stopwatch& value)
	{
		return (os << value.format());
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 
	/// @param formatData 
	/// @param value 
	void Formatter(FormatData& formatData, const Stopwatch& value)
	{
		formatData.string.append(value.format());
	}
}
