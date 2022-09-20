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

# include <iostream>
# include <Siv3D/String.hpp>
# include <Siv3D/Hash.hpp>
# include <Siv3D/Unicode.hpp>

namespace s3d
{
	//////////////////////////////////////////////////
	//
	//	substr / substrView
	//
	//////////////////////////////////////////////////

	String String::substr(const size_t offset, const size_t count) const
	{
		return String(m_string, offset, count);
	}

	StringView String::substrView(const size_t offset, const size_t count) const&
	{
		if (size() < offset)
		{
			throw std::out_of_range{ "String::substrView(): index out of range" };
		}

		return StringView((m_string.data() + offset), Min(count, (m_string.size() - offset)));
	}

	//////////////////////////////////////////////////
	//
	//	narrow / toWstr / toUTF8 / toUTF16 / toUTF32
	//
	//////////////////////////////////////////////////

	std::string String::narrow() const
	{
		return Unicode::Narrow(m_string);
	}

	std::wstring String::toWstr() const
	{
		return Unicode::ToWstring(m_string);
	}

	std::string String::toUTF8() const
	{
		return Unicode::ToUTF8(m_string);
	}

	std::u16string String::toUTF16() const
	{
		return Unicode::ToUTF16(m_string);
	}

	const std::u32string& String::toUTF32() const noexcept
	{
		return m_string;
	}

	//////////////////////////////////////////////////
	//
	//	hash
	//
	//////////////////////////////////////////////////

	uint64 String::hash() const noexcept
	{
		return Hash(m_string.data(), size_bytes());
	}

	//////////////////////////////////////////////////
	//
	//	operator == / operator <=>
	//
	//////////////////////////////////////////////////

	bool operator ==(const String& lhs, const String::value_type* rhs)
	{
		return (lhs.m_string == rhs);
	}

	std::strong_ordering operator <=>(const String& lhs, const String::value_type* rhs)
	{
		return (lhs.m_string.compare(rhs) <=> 0);
	}

	//////////////////////////////////////////////////
	//
	//	operator <<
	//
	//////////////////////////////////////////////////

	std::ostream& operator <<(std::ostream& output, const String& value)
	{
		return (output << value.narrow());
	}

	std::wostream& operator <<(std::wostream& output, const String& value)
	{
		return (output << value.toWstr());
	}

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& output, const String& value)
	{
		return output.write(value.data(), value.size());
	}

	//////////////////////////////////////////////////
	//
	//	operator +
	//
	//////////////////////////////////////////////////

	String operator +(const String::value_type lhs, const String& rhs)
	{
		String result;
		result.reserve(1 + rhs.size());
		result.push_back(lhs);
		result.append(rhs);
		return result;
	}

	String operator +(const String::value_type lhs, String&& rhs)
	{
		rhs.push_front(lhs);
		return std::move(rhs);
	}

	String operator +(const String::value_type* lhs, const String& rhs)
	{
		return (StringView{ lhs } + rhs);
	}

	String operator +(const String::value_type* lhs, String&& rhs)
	{
		return (StringView{ lhs } + std::move(rhs));
	}

	String operator +(const StringView lhs, const String& rhs)
	{
		String result;
		result.reserve(lhs.size() + rhs.size());
		result.append(lhs);
		result.append(rhs);
		return result;
	}

	String operator +(const StringView lhs, String&& rhs)
	{
		rhs.insert(0, lhs);
		return std::move(rhs);
	}

	String operator +(const String& lhs, const String::value_type rhs)
	{
		String result;
		result.reserve(lhs.size() + 1);
		result.append(lhs);
		result.push_back(rhs);
		return result;
	}

	String operator +(const String& lhs, const String::value_type* rhs)
	{
		return (lhs + StringView{ rhs });
	}

	String operator +(const String& lhs, const StringView rhs)
	{
		String result;
		result.reserve(lhs.size() + rhs.size());
		result.append(lhs);
		result.append(rhs);
		return result;
	}

	String operator +(const String& lhs, const String& rhs)
	{
		String result;
		result.reserve(lhs.size() + rhs.size());
		result.append(lhs);
		result.append(rhs);
		return result;
	}

	String operator +(const String& lhs, String&& rhs)
	{
		rhs.insert(0, lhs);
		return std::move(rhs);
	}

	String operator +(String&& lhs, const String::value_type rhs)
	{
		lhs.push_back(rhs);
		return std::move(lhs);
	}

	String operator +(String&& lhs, const String::value_type* rhs)
	{
		lhs.append(rhs);
		return std::move(lhs);
	}

	String operator +(String&& lhs, const StringView rhs)
	{
		lhs.append(rhs);
		return std::move(lhs);
	}

	String operator +(String&& lhs, const String& rhs)
	{
		lhs.append(rhs);
		return std::move(lhs);
	}

	String operator +(String&& lhs, String&& rhs)
	{
		const size_t newSize = (lhs.size() + rhs.size());

		if (newSize <= rhs.capacity()) // rhs にアロケーション無しで追加可能
		{
			rhs.insert(0, lhs);
			return std::move(rhs);
		}
		else
		{
			lhs.append(rhs);
			return std::move(rhs);
		}
	}

	//////////////////////////////////////////////////
	//
	//	operator >>
	//
	//////////////////////////////////////////////////

	std::istream& operator >>(std::istream& input, String& value)
	{
		std::string s;
		input >> s;
		value = Unicode::Widen(s);
		return input;
	}

	std::wistream& operator >>(std::wistream& input, String& value)
	{
		std::wstring s;
		input >> s;
		value = Unicode::FromWstring(s);
		return input;
	}

	//////////////////////////////////////////////////
	//
	//	operator +
	//
	//////////////////////////////////////////////////

	String operator +(const String::value_type lhs, const StringView rhs)
	{
		String result;
		result.reserve(1 + rhs.size());
		result.push_back(lhs);
		result.append(rhs);
		return result;
	}

	String operator +(const String::value_type* lhs, const StringView rhs)
	{
		return (StringView{ lhs } + rhs);
	}

	String operator +(const StringView lhs, const String::value_type* rhs)
	{
		return (lhs + StringView{ rhs });
	}

	String operator +(const StringView lhs, const StringView rhs)
	{
		String result;
		result.reserve(lhs.size() + rhs.size());
		result.append(lhs);
		result.append(rhs);
		return result;
	}

	String operator +(const StringView lhs, const String::value_type rhs)
	{
		String result;
		result.reserve(lhs.size() + 1);
		result.append(lhs);
		result.push_back(rhs);
		return result;
	}
}
