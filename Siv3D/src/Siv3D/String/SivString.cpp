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

# include <iostream>
# include <stdexcept>
# include <Siv3D/String.hpp>
# include <Siv3D/Char.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/RegExp.hpp>
# include <Siv3D/StringRandom.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		inline static constexpr bool IsTrimmable(const char32 ch) noexcept
		{
			return (ch <= 0x20) || ((ch - 0x7F) <= (0x9F - 0x7F));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	toUTF8
	//
	////////////////////////////////////////////////////////////////

	std::string String::toUTF8() const
	{
		return Unicode::ToUTF8(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	toUTF16
	//
	////////////////////////////////////////////////////////////////

	std::u16string String::toUTF16() const
	{
		return Unicode::ToUTF16(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	toWstr
	//
	////////////////////////////////////////////////////////////////

	std::wstring String::toWstr() const
	{
		return Unicode::ToWstring(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	case_insensitive_compare
	//
	////////////////////////////////////////////////////////////////

	int32 String::case_insensitive_compare(const StringView s) const noexcept
	{
		return StringView{ m_string }.case_insensitive_compare(s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	case_insensitive_equals
	//
	////////////////////////////////////////////////////////////////

	bool String::case_insensitive_equals(const StringView s) const noexcept
	{
		return StringView{ m_string }.case_insensitive_equals(s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	capitalize, capitalized
	//
	////////////////////////////////////////////////////////////////

	String& String::capitalize() & noexcept
	{
		for (auto& c : m_string)
		{
			if (IsAlpha(c))
			{
				c = ToUpper(c);
				break;
			}
		}

		return *this;
	}

	String String::capitalize() && noexcept
	{
		return std::move(capitalize());
	}

	String String::capitalized() const&
	{
		return StringView{ m_string }.capitalized();
	}

	String String::capitalized()&& noexcept
	{
		return std::move(capitalize());
	}

	////////////////////////////////////////////////////////////////
	//
	//	count
	//
	////////////////////////////////////////////////////////////////

	int64 String::count(const StringView s) const
	{
		return StringView{ m_string }.count(s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	expandTabs
	//
	////////////////////////////////////////////////////////////////

	String String::expandTabs(const size_type tabSize) const
	{
		return StringView{ m_string }.expandTabs(tabSize);
	}

	////////////////////////////////////////////////////////////////
	//
	//	layout
	//
	////////////////////////////////////////////////////////////////

	String String::layout(size_type width) const
	{
		return StringView{ m_string }.layout(width);
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftPad, leftPadded
	//
	////////////////////////////////////////////////////////////////

	String& String::leftPad(const size_type length, const value_type fillChar) & noexcept
	{
		if (length <= m_string.size())
		{
			return *this;
		}

		m_string.insert(m_string.begin(), (length - m_string.size()), fillChar);

		return *this;
	}

	String String::leftPad(const size_type length, const value_type fillChar) && noexcept
	{
		return std::move(leftPad(length, fillChar));
	}

	String String::leftPadded(const size_type length, const value_type fillChar) const&
	{
		return StringView{ m_string }.leftPadded(length, fillChar);
	}

	String String::leftPadded(const size_type length, const value_type fillChar) && noexcept
	{
		return std::move(leftPad(length, fillChar));
	}

	////////////////////////////////////////////////////////////////
	//
	//	lowercase, lowercased
	//
	////////////////////////////////////////////////////////////////

	String& String::lowercase()& noexcept
	{
		for (auto& c : m_string)
		{
			c = ToLower(c);
		}

		return *this;
	}

	String String::lowercase()&& noexcept
	{
		return std::move(lowercase());
	}

	String String::lowercased() const&
	{
		return StringView{ m_string }.lowercased();
	}

	String String::lowercased()&& noexcept
	{
		return std::move(lowercase());
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftTrim, leftTrimmed
	//
	////////////////////////////////////////////////////////////////

	String& String::leftTrim() & noexcept
	{
		m_string.erase(m_string.begin(), std::find_if_not(m_string.begin(), m_string.end(), IsTrimmable));
		return *this;
	}

	String String::leftTrim() && noexcept
	{
		return std::move(leftTrim());
	}

	String& String::leftTrim(const StringView chars) & noexcept
	{
		auto it = m_string.begin();

		while (it != m_string.end() && chars.contains(*it))
		{
			++it;
		}

		m_string.erase(m_string.begin(), it);

		return *this;
	}

	String String::leftTrim(const StringView chars) && noexcept
	{
		return std::move(leftTrim(chars));
	}

	String String::leftTrimmed() const&
	{
		return StringView{ m_string }.leftTrimmed();
	}

	String String::leftTrimmed() && noexcept
	{
		return std::move(leftTrim());
	}

	String String::leftTrimmed(const StringView chars) const&
	{
		return StringView{ m_string }.leftTrimmed(chars);
	}

	String String::leftTrimmed(const StringView chars) && noexcept
	{
		return std::move(leftTrim(chars));
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove, removed
	//
	////////////////////////////////////////////////////////////////

	String& String::remove(const value_type ch) & noexcept
	{
		m_string.erase(std::remove(m_string.begin(), m_string.end(), ch), m_string.end());
		return *this;
	}

	String& String::remove(const StringView s) & noexcept
	{
		return (*this = removed(s));
	}

	String String::remove(const value_type ch) && noexcept
	{
		return std::move(remove(ch));
	}

	String String::remove(const StringView s) && noexcept
	{
		return std::move(remove(s));
	}

	String String::removed(const value_type ch) const&
	{
		return StringView{ m_string }.removed(ch);
	}

	String String::removed(const StringView s) const&
	{
		return StringView{ m_string }.removed(s);
	}

	String String::removed(const value_type ch) && noexcept
	{
		return std::move(remove(ch));
	}

	String String::removed(const StringView s) && noexcept
	{
		return std::move(remove(s));
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove_at, removed_at
	//
	////////////////////////////////////////////////////////////////

	String& String::remove_at(const size_type index) & noexcept
	{
		if (m_string.size() <= index)
		{
			return *this;
		}

		m_string.erase(index, 1);

		return *this;
	}

	String String::remove_at(const size_type index) && noexcept
	{
		return std::move(remove_at(index));
	}

	String String::removed_at(const size_type index) const&
	{
		return StringView{ m_string }.removed_at(index);
	}

	String String::removed_at(const size_type index) && noexcept
	{
		return std::move(remove_at(index));
	}

	////////////////////////////////////////////////////////////////
	//
	//	repeat
	//
	////////////////////////////////////////////////////////////////

	String String::repeat(const size_t count) const
	{
		return StringView{ m_string }.repeat(count);
	}

	////////////////////////////////////////////////////////////////
	//
	//	replace, replaced
	//
	////////////////////////////////////////////////////////////////

	String& String::replace(const value_type oldChar, const value_type newChar) & noexcept
	{
		std::replace(m_string.begin(), m_string.end(), oldChar, newChar);
		return *this;
	}

	String String::replace(const value_type oldChar, const value_type newChar) && noexcept
	{
		return std::move(replace(oldChar, newChar));
	}

	String String::replaced(const value_type oldChar, const value_type newChar) const&
	{
		return StringView{ m_string }.replaced(oldChar, newChar);
	}

	String String::replaced(const value_type oldChar, const value_type newChar) && noexcept
	{
		return std::move(replace(oldChar, newChar));
	}

	String& String::replace(const StringView oldStr, const StringView newStr)&
	{
		return (*this = replaced(oldStr, newStr));
	}

	String String::replace(const StringView oldStr, const StringView newStr)&&
	{
		return replaced(oldStr, newStr);
	}

	String String::replaced(const StringView oldStr, const StringView newStr) const&
	{
		return StringView{ m_string }.replaced(oldStr, newStr);
	}

	////////////////////////////////////////////////////////////////
	//
	//	replaceFirst, replacedFirst
	//
	////////////////////////////////////////////////////////////////

	String& String::replaceFirst(const RegExp& regexp, const StringView replacement)
	{
		return (*this = regexp.replaceFirst(*this, replacement));
	}

	String String::replacedFirst(const RegExp& regexp, const StringView replacement) const
	{
		return regexp.replaceFirst(*this, replacement);
	}

	////////////////////////////////////////////////////////////////
	//
	//	replaceAll, replacedAll
	//
	////////////////////////////////////////////////////////////////

	String& String::replaceAll(const RegExp& regexp, const StringView replacement)
	{
		return (*this = regexp.replaceAll(*this, replacement));
	}

	String& String::replaceAll(const RegExp& regexp, FunctionRef<String(const MatchResults&)> replacementFunc)
	{
		return (*this = regexp.replaceAll(*this, replacementFunc));
	}

	String String::replacedAll(const RegExp& regexp, const StringView replacement) const
	{
		return regexp.replaceAll(*this, replacement);
	}

	String String::replacedAll(const RegExp& regexp, FunctionRef<String(const MatchResults&)> replacementFunc) const
	{
		return regexp.replaceAll(*this, replacementFunc);
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse, reversed
	//
	////////////////////////////////////////////////////////////////

	String& String::reverse() & noexcept
	{
		std::reverse(m_string.begin(), m_string.end());
		return *this;
	}

	String String::reverse() && noexcept
	{
		return std::move(reverse());
	}

	String String::reversed() const&
	{
		return String{ m_string.rbegin(), m_string.rend() };
	}

	String String::reversed() && noexcept
	{
		return std::move(reverse());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightPad, rightPadded
	//
	////////////////////////////////////////////////////////////////

	String& String::rightPad(const size_type length, const value_type fillChar) & noexcept
	{
		if (length <= m_string.size())
		{
			return *this;
		}

		m_string.append((length - m_string.size()), fillChar);

		return *this;
	}

	String String::rightPad(const size_type length, const value_type fillChar) && noexcept
	{
		return std::move(rightPad(length, fillChar));
	}

	String String::rightPadded(const size_type length, const value_type fillChar) const&
	{
		return StringView{ m_string }.rightPadded(length, fillChar);
	}

	String String::rightPadded(const size_type length, const value_type fillChar) && noexcept
	{
		return std::move(rightPad(length, fillChar));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightTrim, rightTrimmed
	//
	////////////////////////////////////////////////////////////////

	String& String::rightTrim() & noexcept
	{
		m_string.erase(std::find_if_not(m_string.rbegin(), m_string.rend(), IsTrimmable).base(), m_string.end());
		return *this;
	}

	String String::rightTrim() && noexcept
	{
		return std::move(rightTrim());
	}

	String& String::rightTrim(const StringView chars) & noexcept
	{
		auto it = m_string.rbegin();
		
		while (it != m_string.rend() && chars.contains(*it))
		{
			++it;
		}
		
		m_string.erase(it.base(), m_string.end());
		
		return *this;
	}

	String String::rightTrim(const StringView chars) && noexcept
	{
		return std::move(rightTrim(chars));
	}

	String String::rightTrimmed() const&
	{
		return StringView{ m_string }.rightTrimmed();
	}

	String String::rightTrimmed() && noexcept
	{
		return std::move(rightTrim());
	}

	String String::rightTrimmed(const StringView chars) const&
	{
		return StringView{ m_string }.rightTrimmed(chars);
	}

	String String::rightTrimmed(const StringView chars) && noexcept
	{
		return std::move(rightTrim(chars));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate, rotated
	//
	////////////////////////////////////////////////////////////////

	String& String::rotate(const size_type index) & noexcept
	{
		if (m_string.size() <= index)
		{
			return *this;
		}

		std::rotate(m_string.begin(), (m_string.begin() + index), m_string.end());

		return *this;
	}

	String String::rotate(const size_type index) && noexcept
	{
		return std::move(rotate(index));
	}

	String String::rotated(const size_type index) const&
	{
		return StringView{ m_string }.rotated(index);
	}

	String String::rotated(const size_type index) && noexcept
	{
		return std::move(rotate(index));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rsort, rsorted
	//
	////////////////////////////////////////////////////////////////

	String& String::rsort() & noexcept
	{
		std::sort(m_string.begin(), m_string.end(), std::greater<value_type>());
		return *this;
	}

	String String::rsort() && noexcept
	{
		return std::move(rsort());
	}

	String String::rsorted() const&
	{
		return StringView{ m_string }.rsorted();
	}

	String String::rsorted() && noexcept
	{
		return std::move(rsort());
	}

	////////////////////////////////////////////////////////////////
	//
	//	shuffle, shuffled
	//
	////////////////////////////////////////////////////////////////

	String& String::shuffle() & noexcept
	{
		return shuffle(GetDefaultRNG());
	}

	String String::shuffle() && noexcept
	{
		return std::move(*this).shuffle(GetDefaultRNG());
	}

	String String::shuffled() const&
	{
		return shuffled(GetDefaultRNG());
	}

	String String::shuffled() && noexcept
	{
		return std::move(*this).shuffled(GetDefaultRNG());
	}

	////////////////////////////////////////////////////////////////
	//
	//	sort, sorted
	//
	////////////////////////////////////////////////////////////////

	String& String::sort() & noexcept
	{
		std::sort(m_string.begin(), m_string.end());
		return *this;
	}

	String String::sort() && noexcept
	{
		return std::move(sort());
	}

	String String::sorted() const&
	{
		return StringView{ m_string }.sorted();
	}

	String String::sorted() && noexcept
	{
		return std::move(sort());
	}

	////////////////////////////////////////////////////////////////
	//
	//	split
	//
	////////////////////////////////////////////////////////////////

	Array<String> String::split(const value_type ch) const
	{
		return StringView{ m_string }.split(ch);
	}

	////////////////////////////////////////////////////////////////
	//
	//	splitView
	//
	////////////////////////////////////////////////////////////////

	Array<StringView> String::splitView(const value_type ch) const SIV3D_LIFETIMEBOUND
	{
		return StringView{ m_string }.splitView(ch);
	}

	////////////////////////////////////////////////////////////////
	//
	//	split_at
	//
	////////////////////////////////////////////////////////////////

	std::pair<String, String> String::split_at(const size_type pos) const
	{
		return{ substr(0, pos), substr(pos) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	split_lines
	//
	////////////////////////////////////////////////////////////////

	Array<String, std::allocator<String>> String::split_lines() const
	{
		return StringView{ m_string }.split_lines();
	}

	////////////////////////////////////////////////////////////////
	//
	//	swapcase, swapcased
	//
	////////////////////////////////////////////////////////////////

	String& String::swapcase() & noexcept
	{
		for (auto& ch : m_string)
		{
			if (IsLower(ch))
			{
				ch -= ('a' - 'A');
			}
			else if (IsUpper(ch))
			{
				ch += ('a' - 'A');
			}
		}

		return *this;
	}

	String String::swapcase() && noexcept
	{
		return std::move(swapcase());
	}

	String String::swapcased() const&
	{
		return StringView{ m_string }.swapcased();
	}

	String String::swapcased() && noexcept
	{
		return std::move(swapcase());
	}

	////////////////////////////////////////////////////////////////
	//
	//	take
	//
	////////////////////////////////////////////////////////////////

	String String::take(const size_t n) const&
	{
		return String(m_string.begin(), (m_string.begin() + Min(n, m_string.size())));
	}

	String String::take(const size_t n) && noexcept
	{
		if (n < m_string.size())
		{
			m_string.resize(n);
		}

		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	trim, trimmed
	//
	////////////////////////////////////////////////////////////////

	String& String::trim() & noexcept
	{
		return rightTrim().leftTrim();
	}

	String String::trim() && noexcept
	{
		return std::move(trim());
	}

	String& String::trim(const StringView chars) & noexcept
	{
		return rightTrim(chars).leftTrim(chars);
	}

	String String::trim(const StringView chars) && noexcept
	{
		return std::move(trim(chars));
	}

	String String::trimmed() const&
	{
		return StringView{ m_string }.trimmed();
	}

	String String::trimmed() && noexcept
	{
		return std::move(trim());
	}

	String String::trimmed(const StringView chars) const&
	{
		return StringView{ m_string }.trimmed(chars);
	}

	String String::trimmed(const StringView chars) && noexcept
	{
		return std::move(trim(chars));
	}

	////////////////////////////////////////////////////////////////
	//
	//	uppercase, uppercased
	//
	////////////////////////////////////////////////////////////////

	String& String::uppercase() & noexcept
	{
		for (auto& c : m_string)
		{
			c = ToUpper(c);
		}

		return *this;
	}

	String String::uppercase() && noexcept
	{
		return std::move(uppercase());
	}

	String String::uppercased() const&
	{
		return StringView{ m_string }.uppercased();
	}

	String String::uppercased() && noexcept
	{
		return std::move(uppercase());
	}

	////////////////////////////////////////////////////////////////
	//
	//	stable_unique, stable_uniqued
	//
	////////////////////////////////////////////////////////////////

	String& String::stable_unique() & noexcept
	{
		return (*this = stable_uniqued());
	}

	String String::stable_unique() &&
	{
		return stable_uniqued();
	}

	String String::stable_uniqued() const
	{
		return StringView{ m_string }.stable_uniqued();
	}

	////////////////////////////////////////////////////////////////
	//
	//	sort_and_unique, sorted_and_uniqued
	//
	////////////////////////////////////////////////////////////////

	String& String::sort_and_unique() & noexcept
	{
		std::sort(m_string.begin(), m_string.end());
		m_string.erase(std::unique(m_string.begin(), m_string.end()), m_string.end());
		return *this;
	}

	String String::sort_and_unique() && noexcept
	{
		return std::move(sort_and_unique());
	}

	String String::sorted_and_uniqued() const&
	{
		return StringView{ m_string }.sorted_and_uniqued();
	}

	String String::sorted_and_uniqued() && noexcept
	{
		return std::move(sort_and_unique());
	}

	////////////////////////////////////////////////////////////////
	//
	//	unique_consecutive, uniqued_consecutive
	//
	////////////////////////////////////////////////////////////////

	String& String::unique_consecutive() & noexcept
	{
		m_string.erase(std::unique(m_string.begin(), m_string.end()), m_string.end());
		return *this;
	}

	String String::unique_consecutive() && noexcept
	{
		return std::move(unique_consecutive());
	}

	String String::uniqued_consecutive() const&
	{
		return StringView{ m_string }.uniqued_consecutive();
	}

	String String::uniqued_consecutive() && noexcept
	{
		return std::move(unique_consecutive());
	}

	////////////////////////////////////////////////////////////////
	//
	//	values_at
	//
	////////////////////////////////////////////////////////////////

	String String::values_at(const std::initializer_list<size_type> indices) const
	{
		String result;
		result.reserve(indices.size());

		for (const auto index : indices)
		{
			if (index < m_string.size())
			{
				result.push_back(m_string[index]);
			}
			else
			{
				ThrowValuesAtOutOfRange();
			}
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	xml_escape, xml_escaped
	//
	////////////////////////////////////////////////////////////////

	String& String::xml_escape()
	{
		return (*this = xml_escaped());
	}

	String String::xml_escaped() const
	{
		return StringView{ m_string }.xml_escaped();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	std::ostream& operator <<(std::ostream& output, const String& value)
	{
		return (output << Unicode::ToUTF8(value));
	}

	std::wostream& operator <<(std::wostream& output, const String& value)
	{
		return (output << Unicode::ToWstring(value));
	}

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& output, const String& value)
	{
		return output.write(value.data(), value.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator >>
	//
	////////////////////////////////////////////////////////////////

	std::istream& operator >>(std::istream& input, String& value)
	{
		std::string s;
		input >> s;
		value = Unicode::FromUTF8(s);
		return input;
	}

	std::wistream& operator >>(std::wistream& input, String& value)
	{
		std::wstring s;
		input >> s;
		value = Unicode::FromWstring(s);
		return input;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const String& s)
	{
		formatData.string.append(s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void String::ThrowAtOutOfRange()
	{
		throw std::out_of_range{ "String::at(): index out of range" };
	}

	void String::ThrowChoiceOutOfRange()
	{
		throw std::out_of_range{ "String::choice(): String is empty" };
	}

	void String::ThrowValuesAtOutOfRange()
	{
		throw std::out_of_range{ "String::values_at(): index out of range" };
	}
}
