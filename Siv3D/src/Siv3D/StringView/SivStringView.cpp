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
# include <algorithm>
# include <ranges>
# include <Siv3D/Array.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/Char.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Random.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/HashSet.hpp>

namespace s3d
{
	namespace
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
	//	case_insensitive_compare
	//
	////////////////////////////////////////////////////////////////

	int32 StringView::case_insensitive_compare(const StringView s) const noexcept
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

	bool StringView::case_insensitive_equals(const StringView s) const noexcept
	{
		if (m_view.size() != s.size())
		{
			return false;
		}

		auto first1 = m_view.begin(), last1 = m_view.end();
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
	//	capitalized
	//
	////////////////////////////////////////////////////////////////

	String StringView::capitalized() const
	{
		String result{ m_view };

		for (auto& ch : result)
		{
			if (IsAlpha(ch))
			{
				ch = ToUpper(ch);
				break;
			}
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	count
	//
	////////////////////////////////////////////////////////////////

	int64 StringView::count(const StringView s) const
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

	String StringView::expandTabs(const size_type tabSize) const
	{
		String expanded;

		for (auto ch : m_view)
		{
			if (ch == U'\t')
			{
				expanded.append(tabSize, U' ');
			}
			else
			{
				expanded.push_back(ch);
			}
		}

		return expanded;
	}


	////////////////////////////////////////////////////////////////
	//
	//	layout
	//
	////////////////////////////////////////////////////////////////

	String StringView::layout(size_type width) const
	{
		if (width == 0)
		{
			width = 1;
		}

		String result;
		result.reserve(m_view.size() + (m_view.size() / width));

		size_t count = 0;

		for (auto ch : m_view)
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
	//	leftPadded
	//
	////////////////////////////////////////////////////////////////

	String StringView::leftPadded(const size_type length, const value_type fillChar) const
	{
		String result;
		result.reserve(Max(m_view.size(), length));

		result.append((length - m_view.size()), fillChar);
		result.append(m_view);

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	lowercased
	//
	////////////////////////////////////////////////////////////////

	String StringView::lowercased() const
	{
		String result{ m_view };

		for (auto& c : result)
		{
			c = ToLower(c);
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftTrim, leftTrimmed
	//
	////////////////////////////////////////////////////////////////

	String StringView::leftTrimmed() const
	{
		auto it = std::find_if(m_view.begin(), m_view.end(), IsTrimmable);
		return String(it, m_view.end());
	}


	////////////////////////////////////////////////////////////////
	//
	//	remove, removed
	//
	////////////////////////////////////////////////////////////////

	String StringView::removed(const value_type ch) const
	{
		String result;
		result.reserve(m_view.size());

		for (auto c : m_view)
		{
			if (c != ch)
			{
				result.push_back(c);
			}
		}

		return result;
	}

	String StringView::removed(const StringView s) const
	{
		if (s.isEmpty())
		{
			return{};
		}

		String result;

		const auto itEnd = m_view.end();
		auto itCurrent = m_view.begin();
		auto itNext = std::search(itCurrent, itEnd, s.begin(), s.end());

		while (itNext != itEnd)
		{
			result.append(itCurrent, itNext);
			itCurrent = (itNext + s.size());
			itNext = std::search(itCurrent, itEnd, s.begin(), s.end());
		}

		result.append(itCurrent, itNext);

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	repeat
	//
	////////////////////////////////////////////////////////////////

	String StringView::repeat(const size_t count) const
	{
		const size_t blockLength = m_view.size();
		const size_t blockMemorySize = (blockLength * sizeof(value_type));

		String result((blockLength * count), U'\0');
		value_type* pDst = result.data();
		const value_type* pSrc = m_view.data();

		for (size_t i = 0; i < count; ++i)
		{
			std::memcpy(pDst, pSrc, blockMemorySize);
			pDst += blockLength;
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	replace, replaced
	//
	////////////////////////////////////////////////////////////////

	String StringView::replaced(const value_type oldChar, const value_type newChar) const
	{
		String result{ m_view };
		result.replace(oldChar, newChar);
		return result;
	}

	String StringView::replaced(const StringView oldStr, const StringView newStr) const
	{
		String result;

		if (oldStr.size() <= newStr.size())
		{
			result.reserve(m_view.size());
		}

		const auto itEnd = m_view.end();
		auto itCurrent = m_view.begin();
		auto itNext = std::search(itCurrent, itEnd, oldStr.begin(), oldStr.end());

		while (itNext != itEnd)
		{
			result.append(itCurrent, itNext);
			result.append(newStr);
			itCurrent = (itNext + oldStr.size());
			itNext = std::search(itCurrent, itEnd, oldStr.begin(), oldStr.end());
		}

		result.append(itCurrent, itNext);

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	removed_at
	//
	////////////////////////////////////////////////////////////////

	String StringView::removed_at(const size_type index) const
	{
		if (m_view.size() <= index)
		{
			return{};
		}

		String result;
		result.reserve(m_view.size() - 1);
		result.append(m_view.begin(), (m_view.begin() + index));
		result.append((m_view.begin() + index + 1), m_view.end());

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	reversed
	//
	////////////////////////////////////////////////////////////////

	String StringView::reversed() const
	{
		return String{ m_view.rbegin(), m_view.rend() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightPadded
	//
	////////////////////////////////////////////////////////////////

	String StringView::rightPadded(const size_type length, const value_type fillChar) const
	{
		String result;
		result.reserve(Max(m_view.size(), length));

		result.append(m_view);
		result.append((length - m_view.size()), fillChar);

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightTrim, rightTrimmed
	//
	////////////////////////////////////////////////////////////////

	String StringView::rightTrimmed() const
	{
		auto it = std::find_if_not(m_view.rbegin(), m_view.rend(), IsTrimmable).base();
		return String(m_view.begin(), it);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate, rotated
	//
	////////////////////////////////////////////////////////////////

	String StringView::rotated(const size_type index) const
	{
		if (m_view.size() <= index)
		{
			return{};
		}

		String result;
		result.reserve(m_view.size());

		result.append((m_view.begin() + index), m_view.end());
		result.append(m_view.begin(), (m_view.begin() + index));

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	rsorted
	//
	////////////////////////////////////////////////////////////////

	String StringView::rsorted() const
	{
		String result{ m_view };
		result.rsort();
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	sorted
	//
	////////////////////////////////////////////////////////////////

	String StringView::sorted() const
	{
		String result{ m_view };
		result.sort();
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	split
	//
	////////////////////////////////////////////////////////////////

	Array<String> StringView::split(const value_type ch) const
	{
		auto split_views = m_view | std::views::split(ch) | std::views::transform([](auto&& part) { return String{ part.begin(), part.end() }; });

		return Array<String>(split_views.begin(), split_views.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	splitView
	//
	////////////////////////////////////////////////////////////////

	Array<StringView> StringView::splitView(const value_type ch) const SIV3D_LIFETIMEBOUND
	{
		auto split_views = m_view | std::views::split(ch) | std::views::transform([](auto&& part) { return StringView{ part }; });

		return Array<StringView>(split_views.begin(), split_views.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	split_at
	//
	////////////////////////////////////////////////////////////////

	std::pair<StringView, StringView> StringView::split_at(const size_type pos) const
	{
		return{ substr(0, pos), substr(pos) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	split_lines
	//
	////////////////////////////////////////////////////////////////

	Array<String, std::allocator<String>> StringView::split_lines() const
	{
		if (m_view.empty())
		{
			return{};
		}

		Array<String> result(1);

		const value_type* pSrc = m_view.data();
		const value_type* const pSrcEnd = (pSrc + m_view.size());
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

	String StringView::swapcased() const
	{
		String result{ m_view };

		for (auto& ch : result)
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

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	take
	//
	////////////////////////////////////////////////////////////////

	String StringView::take(const size_t n) const
	{
		return String(m_view.begin(), (m_view.begin() + Min(n, m_view.size())));
	}

	////////////////////////////////////////////////////////////////
	//
	//	trimmed
	//
	////////////////////////////////////////////////////////////////

	String StringView::trimmed() const
	{
		const char32* start = m_view.data();
		const char32* end = (start + m_view.size());

		while ((start < end) && IsTrimmable(*start))
		{
			++start;
		}

		while ((start < end) && IsTrimmable(*(end - 1)))
		{
			--end;
		}

		return String(start, end);
	}

	////////////////////////////////////////////////////////////////
	//
	//	uppercased
	//
	////////////////////////////////////////////////////////////////

	String StringView::uppercased() const
	{
		String result{ m_view };
		
		for (auto& c : result)
		{
			c = ToUpper(c);
		}
		
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	stable_uniqued
	//
	////////////////////////////////////////////////////////////////

	String StringView::stable_uniqued() const
	{
		String result;

		StringStableUniqueHelper pred;

		std::copy_if(m_view.begin(), m_view.end(), std::back_inserter(result), std::ref(pred));

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	sorted_and_uniqued
	//
	////////////////////////////////////////////////////////////////

	String StringView::sorted_and_uniqued() const
	{
		String result{ m_view };
		result.sort_and_unique();
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	uniqued_consecutive
	//
	////////////////////////////////////////////////////////////////

	String StringView::uniqued_consecutive() const
	{
		String result;
		std::unique_copy(m_view.begin(), m_view.end(), std::back_inserter(result));
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	values_at
	//
	////////////////////////////////////////////////////////////////

	String StringView::values_at(const std::initializer_list<size_type> indices) const
	{
		String result;
		result.reserve(indices.size());

		for (const auto index : indices)
		{
			if (index < m_view.size())
			{
				result.push_back(m_view[index]);
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
	//	xml_escaped
	//
	////////////////////////////////////////////////////////////////

	String StringView::xml_escaped() const
	{
		String result;
		result.reserve(m_view.size());

		for (const auto ch : m_view)
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

	std::ostream& operator <<(std::ostream& output, const StringView& value)
	{
		return (output << Unicode::ToUTF8(value));
	}

	std::wostream& operator <<(std::wostream& output, const StringView& value)
	{
		return (output << Unicode::ToWstring(value));
	}

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& output, const StringView& value)
	{
		return output.write(value.data(), value.size());
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

	void StringView::ThrowChoiceOutOfRange()
	{
		throw std::out_of_range{ "StringView::choice(): StringView is empty" };
	}

	void StringView::ThrowValuesAtOutOfRange()
	{
		throw std::out_of_range{ "StringView::values_at(): index out of range" };
	}
}
