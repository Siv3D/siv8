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

# pragma once

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	template <class CharType>
	std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const None_t&)
	{
		constexpr CharType no[] = { 'n','o','n','e','\0' };
		return output << no;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	void Formatter(FormatData& formatData, const Optional<Type>& value)
	{
		if (value)
		{
			formatData.string.append(U"(Optional)"_sv);
			Formatter(formatData, *value);
		}
		else
		{
			Formatter(formatData, none);
		}
	}
}
