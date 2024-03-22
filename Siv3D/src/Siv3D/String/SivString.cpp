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
# include <Siv3D/HashSet.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/Char.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/Array.hpp>

namespace s3d
{
	namespace detail
	{
		[[nodiscard]]
		inline static constexpr bool IsTrimmable(const char32 ch) noexcept
		{
			return (ch <= 0x20) || ((ch - 0x7F) <= (0x9F - 0x7F));
		}

		class StringStableUniqueHelper
		{
		public:

			[[nodiscard]]
			bool operator()(const char32 value)
			{
				return m_set.insert(value).second;
			}

		private:

			HashSet<char32> m_set;
		};
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
		auto first1 = begin(), last1 = end();
		auto first2 = s.begin(), last2 = s.end();

		for (; (first1 != last1) && (first2 != last2); ++first1, ++first2)
		{
			const int32 c = CaseInsensitiveCompare(*first1, *first2);

			if (c != 0)
			{
				return c;
			}
		}

		if ((first1 == last1) && (first2 != last2))
		{
			return -1;
		}
		else if ((first1 != last1) && (first2 == last2))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	case_insensitive_equals
	//
	////////////////////////////////////////////////////////////////

	bool String::case_insensitive_equals(const StringView s) const noexcept
	{
		if (m_string.size() != s.size())
		{
			return false;
		}

		auto first1 = m_string.begin(), last1 = m_string.end();
		auto first2 = s.begin(), last2 = s.end();

		for (; (first1 != last1) && (first2 != last2); ++first1, ++first2)
		{
			const int32 c = CaseInsensitiveCompare(*first1, *first2);

			if (c != 0)
			{
				return false;
			}
		}

		return true;
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
		String result{ *this };
		result.capitalize();
		return result;
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
		int64 count = 0;

		for (auto it = begin();; ++it, ++count)
		{
			it = std::search(it, end(), s.begin(), s.end());

			if (it == end())
			{
				return count;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	expandTabs
	//
	////////////////////////////////////////////////////////////////

	String String::expandTabs(const size_type tabSize) const
	{
		String expanded;

		for (auto ch : m_string)
		{
			if (ch == U'\t')
			{
				expanded.m_string.append(tabSize, U' ');
			}
			else
			{
				expanded.m_string.push_back(ch);
			}
		}

		return expanded;
	}

	////////////////////////////////////////////////////////////////
	//
	//	layout
	//
	////////////////////////////////////////////////////////////////

	String String::layout(size_type width) const
	{
		if (width == 0)
		{
			width = 1;
		}

		String result;
		result.reserve(m_string.length() + (m_string.length() / width));

		size_t count = 0;

		for (auto ch : m_string)
		{
			if (ch == U'\n')
			{
				count = 0;
			}
			else if (ch == U'\r')
			{
				continue;
			}
			else
			{
				++count;
			}

			if (width < count)
			{
				result.push_back(U'\n');
				count = 1;
			}

			result.push_back(ch);
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftPad, leftPadded
	//
	////////////////////////////////////////////////////////////////

	String& String::leftPad(const size_type length, const value_type fillChar) & noexcept
	{
		if (length <= m_string.length())
		{
			return *this;
		}

		m_string.insert(m_string.begin(), (length - m_string.length()), fillChar);

		return *this;
	}

	String String::leftPad(const size_type length, const value_type fillChar) && noexcept
	{
		return std::move(leftPad(length, fillChar));
	}

	String String::leftPadded(const size_type length, const value_type fillChar) const&
	{
		String result;
		result.reserve(Max(m_string.length(), length));

		result.append((length - m_string.length()), fillChar);
		result.append(m_string);

		return result;
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
		String result{ *this };
		result.lowercase();
		return result;
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
		m_string.erase(m_string.begin(), std::find_if(m_string.begin(), m_string.end(), detail::IsTrimmable));
		return *this;
	}

	String String::leftTrim() && noexcept
	{
		return std::move(leftTrim());
	}

	String String::leftTrimmed() const&
	{
		auto it = std::find_if(m_string.begin(), m_string.end(), detail::IsTrimmable);
		return String(it, m_string.end());
	}

	String String::leftTrimmed() && noexcept
	{
		return std::move(leftTrim());
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
		String result;
		result.reserve(m_string.length());

		for (auto c : m_string)
		{
			if (c != ch)
			{
				result.push_back(c);
			}
		}

		return result;
	}

	String String::removed(const StringView s) const&
	{
		if (s.isEmpty())
		{
			return *this;
		}

		String result;

		const auto itEnd = m_string.end();
		auto itCurrent = m_string.begin();
		auto itNext = std::search(itCurrent, itEnd, s.begin(), s.end());

		while (itNext != itEnd)
		{
			result.append(itCurrent, itNext);
			itCurrent = (itNext + s.length());
			itNext = std::search(itCurrent, itEnd, s.begin(), s.end());
		}

		result.append(itCurrent, itNext);

		return result;
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
		if (m_string.size() <= index)
		{
			return *this;
		}

		String result;
		result.reserve(m_string.length() - 1);
		result.append(m_string.begin(), (m_string.begin() + index));
		result.append((m_string.begin() + index + 1), m_string.end());

		return result;
	}

	String String::removed_at(const size_type index) && noexcept
	{
		return std::move(remove_at(index));
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
		String result{ *this };
		result.replace(oldChar, newChar);
		return result;
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
		String result;

		if (oldStr.length() <= newStr.length())
		{
			result.reserve(m_string.length());
		}

		const auto itEnd = m_string.end();
		auto itCurrent = m_string.begin();
		auto itNext = std::search(itCurrent, itEnd, oldStr.begin(), oldStr.end());

		while (itNext != itEnd)
		{
			result.append(itCurrent, itNext);
			result.append(newStr);
			itCurrent = (itNext + oldStr.length());
			itNext = std::search(itCurrent, itEnd, oldStr.begin(), oldStr.end());
		}

		result.append(itCurrent, itNext);

		return result;
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
		if (length <= m_string.length())
		{
			return *this;
		}

		m_string.append((length - m_string.length()), fillChar);

		return *this;
	}

	String String::rightPad(const size_type length, const value_type fillChar) && noexcept
	{
		return std::move(rightPad(length, fillChar));
	}

	String String::rightPadded(const size_type length, const value_type fillChar) const&
	{
		String result;
		result.reserve(Max(m_string.length(), length));

		result.append(m_string);
		result.append((length - m_string.length()), fillChar);

		return result;
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
		m_string.erase(std::find_if_not(m_string.rbegin(), m_string.rend(), detail::IsTrimmable).base(), m_string.end());
		return *this;
	}

	String String::rightTrim() && noexcept
	{
		return std::move(rightTrim());
	}

	String String::rightTrimmed() const&
	{
		auto it = std::find_if_not(m_string.rbegin(), m_string.rend(), detail::IsTrimmable).base();
		return String(m_string.begin(), it);
	}

	String String::rightTrimmed() && noexcept
	{
		return std::move(rightTrim());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate, rotated
	//
	////////////////////////////////////////////////////////////////

	String& String::rotate(const size_type index) & noexcept
	{
		if (m_string.length() <= index)
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
		if (m_string.length() <= index)
		{
			return *this;
		}

		String result;
		result.reserve(m_string.length());

		result.append((m_string.begin() + index), m_string.end());
		result.append(m_string.begin(), (m_string.begin() + index));

		return result;
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
		String result{ *this };
		result.rsort();
		return result;
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
		String result{ *this };
		result.sort();
		return result;
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
		if (m_string.empty())
		{
			return{};
		}

		Array<String> result;

		size_t start = 0;

		for (size_t i = 0; i < m_string.length(); ++i)
		{
			if (m_string[i] == ch)
			{
				result.push_back(m_string.substr(start, (i - start)));
				start = (i + 1);
			}
		}

		result.push_back(m_string.substr(start));

		return result;
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
		if (m_string.empty())
		{
			return{};
		}

		Array<String> result(1);

		const value_type* pSrc = m_string.data();
		const value_type* const pSrcEnd = (pSrc + m_string.length());
		String* currentLine = &result.back();

		while (pSrc != pSrcEnd)
		{
			if (*pSrc == U'\n')
			{
				result.emplace_back();
				currentLine = &result.back();
			}
			else if (*pSrc != U'\r')
			{
				currentLine->push_back(*pSrc);
			}

			++pSrc;
		}

		return result;
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
		String result{ *this };
		result.swapcase();
		return result;
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

	String String::trimmed() const&
	{
		const char32* start = m_string.data();
		const char32* end = (start + m_string.size());

		while ((start < end) && detail::IsTrimmable(*start))
		{
			++start;
		}

		while ((start < end) && detail::IsTrimmable(*(end - 1)))
		{
			--end;
		}

		return String(start, end);
	}

	String String::trimmed() && noexcept
	{
		return std::move(trim());
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
		String result{ *this };
		result.uppercase();
		return result;
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
		String result;

		detail::StringStableUniqueHelper pred;

		std::copy_if(m_string.begin(), m_string.end(), std::back_inserter(result), std::ref(pred));

		return result;
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
		String result{ *this };
		result.sort_and_unique();
		return result;
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
		String result;
		std::unique_copy(m_string.begin(), m_string.end(), std::back_inserter(result));
		return result;
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

		for (auto index : indices)
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
		String result;
		result.reserve(m_string.length());

		for (const auto ch : m_string)
		{
			switch (ch)
			{
			case U'&':
				result.append(U"&amp;");
				break;
			case U'<':
				result.append(U"&lt;");
				break;
			case U'>':
				result.append(U"&gt;");
				break;
			case U'\"':
				result.append(U"&quot;");
				break;
			case U'\'':
				result.append(U"&apos;");
				break;
			default:
				result.push_back(ch);
				break;
			}
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	std::ostream& operator <<(std::ostream& os, const String& value)
	{
		return (os << Unicode::ToUTF8(value));
	}

	std::wostream& operator <<(std::wostream& os, const String& value)
	{
		return (os << Unicode::ToWstring(value));
	}

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const String& value)
	{
		return os.write(value.data(), value.size());
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
	//	(private functions)
	//
	////////////////////////////////////////////////////////////////

	void String::ThrowAtOutOfRange()
	{
		throw std::out_of_range{ "String::at(): index out of range" };
	}

	void String::ThrowValuesAtOutOfRange()
	{
		throw std::out_of_range{ "String::values_at(): index out of range" };
	}
}
