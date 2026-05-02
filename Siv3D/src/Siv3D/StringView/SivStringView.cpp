//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
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
# include <Siv3D/FormatData.hpp>
# include <Siv3D/HashSet.hpp>
# include <Siv3D/RegExp.hpp>

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

			void reserve(const size_t size)
			{
				m_set.reserve(size);
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
		if (s.isEmpty())
		{
			return 0;
		}

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
		const size_t tabs = std::ranges::count(m_view, U'\t');
		const size_t extra = (tabs * (tabSize - 1));

		String expanded;
		expanded.reserve(m_view.size() + extra);

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
		if (length <= m_view.size())
		{
			return String{ m_view };
		}

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
		auto it = std::ranges::find_if_not(m_view, IsTrimmable);
		return String(it, m_view.end());
	}

	String StringView::leftTrimmed(const StringView chars) const
	{
		auto it = m_view.begin();

		while (it != m_view.end() && chars.contains(*it))
		{
			++it;
		}

		return String(it, m_view.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	repeat
	//
	////////////////////////////////////////////////////////////////

	String StringView::repeat(const size_t count) const
	{
		const size_t blockLength = m_view.size();

		if ((blockLength == 0) || (count == 0))
		{
			return{};
		}

		String result;

		if ((result.max_size() / blockLength) < count)
		{
			ThrowRepeatLengthError();
		}

		const size_t totalLength = (blockLength * count);
		result.resize(totalLength);

		value_type* pDst = result.data();
		const value_type* pSrc = m_view.data();
		const size_t bytes = (blockLength * sizeof(value_type));

		for (size_t i = 0; i < count; ++i)
		{
			std::memcpy(pDst, pSrc, bytes);
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
		if (oldStr.isEmpty())
		{
			return String{ m_view };
		}

		String result;
		result.reserve(m_view.size());

		size_type currentPos = 0;
		size_type foundPos = m_view.find(oldStr.m_view, currentPos);

		while (foundPos != npos)
		{
			result.append(m_view.begin() + currentPos, m_view.begin() + foundPos);
			result.append(newStr);

			currentPos = (foundPos + oldStr.size());
			foundPos = m_view.find(oldStr.m_view, currentPos);
		}

		result.append(m_view.begin() + currentPos, m_view.end());

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
	//	replacedFirst
	//
	////////////////////////////////////////////////////////////////

	String StringView::replacedFirst(const RegExp& regexp, const StringView replacement) const
	{
		return regexp.replaceFirst(*this, replacement);
	}

	////////////////////////////////////////////////////////////////
	//
	//	replacedAll
	//
	////////////////////////////////////////////////////////////////

	String StringView::replacedAll(const RegExp& regexp, const StringView replacement) const
	{
		return regexp.replaceAll(*this, replacement);
	}

	String StringView::replacedAll(const RegExp& regexp, FunctionRef<String(const MatchResults&)> replacementFunc) const
	{
		return regexp.replaceAll(*this, replacementFunc);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightPadded
	//
	////////////////////////////////////////////////////////////////

	String StringView::rightPadded(const size_type length, const value_type fillChar) const
	{
		if (length <= m_view.size())
		{
			return String{ m_view };
		}

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

	String StringView::rightTrimmed(const StringView chars) const
	{
		auto it = m_view.rbegin();
		
		while (it != m_view.rend() && chars.contains(*it))
		{
			++it;
		}
		
		return String(m_view.begin(), it.base());
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
			return String{ m_view };
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
	//	splitAt
	//
	////////////////////////////////////////////////////////////////

	std::pair<StringView, StringView> StringView::splitAt(const size_type pos) const SIV3D_LIFETIMEBOUND
	{
		if (m_view.size() <= pos)
		{
			return{ *this, StringView{} };
		}

		return{ substr(0, pos), substr(pos) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	splitLines
	//
	////////////////////////////////////////////////////////////////

	Array<StringView> StringView::splitLines() const SIV3D_LIFETIMEBOUND
	{
		return splitView(U'\n');
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

	String StringView::trimmed(const StringView chars) const
	{
		const char32* start = m_view.data();
		const char32* end = (start + m_view.size());
		
		while ((start < end) && chars.contains(*start))
		{
			++start;
		}
		
		while ((start < end) && chars.contains(*(end - 1)))
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
		result.reserve(m_view.size());

		StringStableUniqueHelper pred;
		pred.reserve(m_view.size());

		std::ranges::copy_if(m_view, std::back_inserter(result), std::ref(pred));

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
		result.reserve(m_view.size());
		std::ranges::unique_copy(m_view, std::back_inserter(result));
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
	//	without
	//
	////////////////////////////////////////////////////////////////

	String StringView::without(const value_type ch) const
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

	String StringView::without(const StringView s) const
	{
		if (s.isEmpty())
		{
			return String{ m_view };
		}

		String result;
		result.reserve(m_view.size());

		size_type currentPos = 0;
		size_type foundPos = m_view.find(s.m_view, currentPos);

		while (foundPos != npos)
		{
			result.append(m_view.begin() + currentPos, m_view.begin() + foundPos);

			currentPos = (foundPos + s.size());
			foundPos = m_view.find(s.m_view, currentPos);
		}

		result.append(m_view.begin() + currentPos, m_view.end());

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	without_at
	//
	////////////////////////////////////////////////////////////////

	String StringView::without_at(const size_type index) const
	{
		if (m_view.size() <= index)
		{
			ThrowWithoutAtOutOfRange();
		}

		String result;
		result.reserve(m_view.size() - 1);
		result.append(m_view.begin(), (m_view.begin() + index));
		result.append((m_view.begin() + index + 1), m_view.end());

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

	void StringView::ThrowWithoutAtOutOfRange()
	{
		throw std::out_of_range{ "StringView::without_at(): index out of range" };
	}

	void StringView::ThrowRepeatLengthError()
	{
		throw std::length_error{ "StringView::repeat(): result too long" };
	}
}
