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

# include <iostream>
# include <stdexcept>
# include <Siv3D/StringView.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	toUTF8
	//
	////////////////////////////////////////////////////////////////

	std::string StringView::toUTF8() const
	{
		return Unicode::ToUTF8(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	toUTF16
	//
	////////////////////////////////////////////////////////////////

	std::u16string StringView::toUTF16() const
	{
		return Unicode::ToUTF16(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	toWstr
	//
	////////////////////////////////////////////////////////////////

	std::wstring StringView::toWstr() const
	{
		return Unicode::ToWstring(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	toString
	//
	////////////////////////////////////////////////////////////////

	String StringView::toString() const
	{
		return String{ m_view };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	String operator +(const StringView lhs, const StringView rhs)
	{
		String s;
		s.reserve(lhs.size() + rhs.size());
		s.append(lhs).append(rhs);
		return s;
	}

	String operator +(const StringView::value_type* lhs, const StringView rhs)
	{
		return (StringView{ lhs } + rhs);
	}

	String operator +(const StringView::value_type lhs, const StringView rhs)
	{
		String s;
		s.reserve(1 + rhs.size());
		s.append(lhs).append(rhs);
		return s;
	}

	String operator +(const StringView lhs, const StringView::value_type* rhs)
	{
		return (lhs + StringView{ rhs });
	}

	String operator +(const StringView lhs, const StringView::value_type rhs)
	{
		String s;
		s.reserve(lhs.size() + 1);
		s.append(lhs).append(rhs);
		return s;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const StringView s)
	{
		formatData.string.append(s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private member functions)
	//
	////////////////////////////////////////////////////////////////

	void StringView::ThrowAtOutOfRange()
	{
		throw std::out_of_range{ "StringView::at(): index out of range" };
	}

	void StringView::ThrowSubstrOutOfRange()
	{
		throw std::out_of_range{ "StringView::substr(): index out of range" };
	}
}
