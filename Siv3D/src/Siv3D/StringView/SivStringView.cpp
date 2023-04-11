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

# include <iostream>
# include <Siv3D/StringView.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Hash.hpp>
//# include <Siv3D/FormatData.hpp>

namespace s3d
{
	std::string StringView::toUTF8() const
	{
		return Unicode::ToUTF8(*this);
	}

	std::wstring StringView::toWstr() const
	{
		return Unicode::ToWstring(*this);
	}

	//////////////////////////////////////////////////
	//
	//	operator <<
	//
	//////////////////////////////////////////////////

	std::ostream& operator <<(std::ostream& os, const StringView& value)
	{
		return (os << Unicode::ToUTF8(value));
	}

	std::wostream& operator <<(std::wostream& os, const StringView& value)
	{
		return (os << Unicode::ToWstring(value));
	}

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const StringView& value)
	{
		return os.write(value.data(), value.size());
	}

	//////////////////////////////////////////////////
	//
	//	Formatter
	//
	//////////////////////////////////////////////////

	//void Formatter(FormatData& formatData, const StringView s)
	//{
	//	formatData.string.append(s);
	//}
}
