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

# include "RegExpDetail.hpp"
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/Unicode.hpp>

namespace s3d
{
	namespace detail
	{
		static MatchResults MatchOrSearch(regex_t* regex, const bool search, const StringView s)
		{
			if (regex == nullptr)
			{
				return{};
			}

			const UChar* pString = reinterpret_cast<const UChar*>(s.data());
			const size_t stringLength = (s.size() * sizeof(char32));

			const unsigned char* pStart = pString;
			const unsigned char* pEnd = pString + stringLength;
			const unsigned char* pRange = pEnd;

			OnigRegion* region = ::onig_region_new();
			ScopeExit cleanup = [=]() { ::onig_region_free(region, 1); };

			const int r = search ? ::onig_match(regex, pString, pEnd, pStart, region, ONIG_OPTION_NONE)
				: ::onig_search(regex, pString, pEnd, pStart, pRange, region, ONIG_OPTION_NONE);

			if (r >= 0)
			{
				Array<StringView> matches;

				for (int32 i = 0; i < region->num_regs; ++i)
				{
					if ((region->beg[i] == ONIG_REGION_NOTPOS)
						|| (region->end[i] == ONIG_REGION_NOTPOS))
					{
						matches.emplace_back();
						continue;
					}

					const size_t begIndex = (region->beg[i] / sizeof(char32));
					const size_t endIndex = (region->end[i] / sizeof(char32));
					const size_t length = (endIndex - begIndex);
					matches.push_back(s.substr(begIndex, length));
				}

				return MatchResults(std::move(matches));
			}
			else
			{
				return{};
			}
		}
	}

	RegExp::RegExpDetail::RegExpDetail(const StringView pattern)
	{
		const UChar* pPattern = reinterpret_cast<const UChar*>(pattern.data());
		const size_t patternLength = (pattern.size() * sizeof(char32));

		OnigErrorInfo error;
		if (const int r = ::onig_new(&m_regex, pPattern, pPattern + patternLength,
			ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF32_LE, ONIG_SYNTAX_RUBY, &error);
			r != ONIG_NORMAL)
		{
			char s[ONIG_MAX_ERROR_MESSAGE_LEN];
			::onig_error_code_to_str(reinterpret_cast<UChar*>(s), r, &error);
			LOG_FAIL(fmt::format("❌ RegExp: Failed to create a regex object ({0})", s));
			return;
		}
	}

	RegExp::RegExpDetail::~RegExpDetail()
	{
		if (not isValid())
		{
			return;
		}

		::onig_free(m_regex);
		m_regex = nullptr;
	}

	bool RegExp::RegExpDetail::isValid() const noexcept
	{
		return (m_regex != nullptr);
	}

	bool RegExp::RegExpDetail::fullMatch(const StringView s) const
	{
		if (not isValid())
		{
			return false;
		}

		const UChar* pString = reinterpret_cast<const UChar*>(s.data());
		const size_t stringLength = (s.size() * sizeof(char32));

		const unsigned char* pStart = pString;
		const unsigned char* pEnd = pString + stringLength;

		OnigRegion* region = ::onig_region_new();
		ScopeExit cleanup = [=]() { ::onig_region_free(region, 1); };

		const int r = ::onig_match(m_regex, pString, pEnd, pStart, region, ONIG_OPTION_NONE);

		if (0 <= r)
		{
			const size_t begIndex = (region->beg[0] / sizeof(char32));
			const size_t endIndex = (region->end[0] / sizeof(char32));
			return ((begIndex == 0) && (endIndex == s.size()));
		}
		else
		{
			return false;
		}
	}

	MatchResults RegExp::RegExpDetail::match(const StringView s) const
	{
		return detail::MatchOrSearch(m_regex, false, s);
	}

	MatchResults RegExp::RegExpDetail::search(const StringView s) const
	{
		return detail::MatchOrSearch(m_regex, true, s);
	}

	Array<MatchResults> RegExp::RegExpDetail::findAll(const StringView s) const
	{
		if (not isValid())
		{
			return{};
		}

		const UChar* pString = reinterpret_cast<const UChar*>(s.data());
		const size_t stringLength = (s.size() * sizeof(char32));

		const unsigned char* pStart = pString;
		const unsigned char* pPreviousStart = pStart;
		const unsigned char* pEnd = pString + stringLength;
		const unsigned char* pRange = pEnd;

		OnigRegion* region = ::onig_region_new();
		ScopeExit cleanup = [=]() { ::onig_region_free(region, 1); };

		Array<MatchResults> results;

		for (;;)
		{
			Array<StringView> matches;

			const int r = ::onig_search(m_regex, pString, pEnd, pStart, pRange, region, ONIG_OPTION_NONE);

			if (0 <= r)
			{
				for (int32 i = 0; i < region->num_regs; ++i)
				{
					if ((region->beg[i] == ONIG_REGION_NOTPOS)
						|| (region->end[i] == ONIG_REGION_NOTPOS))
					{
						matches.emplace_back();
						continue;
					}

					const size_t begIndex = (region->beg[i] / sizeof(char32));
					const size_t endIndex = (region->end[i] / sizeof(char32));
					const size_t length = (endIndex - begIndex);
					matches.push_back(s.substr(begIndex, length));
				}

				results.push_back(MatchResults{ std::move(matches) });

				const size_t begIndex0 = (region->beg[0] / sizeof(char32));
				const size_t endIndex0 = (region->end[0] / sizeof(char32));
				const size_t length0 = (endIndex0 - begIndex0);
				pStart = pString + (begIndex0 + length0) * sizeof(char32);

				if (pStart == pPreviousStart)
				{
					pStart += sizeof(char32);
				}

				pPreviousStart = pStart;
			}
			else
			{
				return results;
			}
		}
	}

	String RegExp::RegExpDetail::replaceFirst(const StringView s, const StringView replacement) const
	{
		if (not isValid())
		{
			return{};
		}

		const UChar* pString = reinterpret_cast<const UChar*>(s.data());
		const size_t stringLength = (s.size() * sizeof(char32));

		const unsigned char* pStart = pString;
		const unsigned char* pEnd = pString + stringLength;
		const unsigned char* pRange = pEnd;

		OnigRegion* region = ::onig_region_new();
		ScopeExit cleanup = [=]() { ::onig_region_free(region, 1); };

		const int r = ::onig_search(m_regex, pString, pEnd, pStart, pRange, region, ONIG_OPTION_NONE);

		if (0 <= r)
		{
			const size_t begIndex = (region->beg[0] / sizeof(char32));
			const size_t endIndex = (region->end[0] / sizeof(char32));
			const size_t length = (endIndex - begIndex);

			String result = s.substr(0, begIndex).toString();
			result.append(replacement);
			result.append(s.substr(begIndex + length));

			return result;
		}
		else
		{
			return s.toString();
		}
	}

	String RegExp::RegExpDetail::replaceAll(const StringView s, const StringView replacement) const
	{
		if (not isValid())
		{
			return{};
		}

		const UChar* pString = reinterpret_cast<const UChar*>(s.data());
		const size_t stringLength = (s.size() * sizeof(char32));

		const unsigned char* pStart = pString;
		const unsigned char* pPreviousStart = pStart;
		const unsigned char* pEnd = pString + stringLength;
		const unsigned char* pRange = pEnd;

		OnigRegion* region = ::onig_region_new();
		ScopeExit cleanup = [=]() { ::onig_region_free(region, 1); };

		String result;

		for (;;)
		{
			const int r = ::onig_search(m_regex, pString, pEnd, pStart, pRange, region, ONIG_OPTION_NONE);

			if (0 <= r)
			{
				const size_t begIndex = (region->beg[0] / sizeof(char32));
				const size_t endIndex = (region->end[0] / sizeof(char32));
				const size_t length = (endIndex - begIndex);

				result.append(s.substr(((pPreviousStart - pString) / sizeof(char32)), (begIndex - (pPreviousStart - pString) / sizeof(char32))));
				result.append(replacement);

				pStart = (pString + (begIndex + length) * sizeof(char32));

				if (pStart == pPreviousStart)
				{
					pStart += sizeof(char32);
				}

				pPreviousStart = pStart;
			}
			else
			{
				result.append(s.substr((pPreviousStart - pString) / sizeof(char32)));
				return result;
			}
		}
	}

	String RegExp::RegExpDetail::replaceAll(const StringView s, const FunctionRef<String(const MatchResults&)> replacementFunc) const
	{
		if (not isValid())
		{
			return{};
		}

		const UChar* pString = reinterpret_cast<const UChar*>(s.data());
		const size_t stringLength = (s.size() * sizeof(char32));

		const unsigned char* pStart = pString;
		const unsigned char* pPreviousStart = pStart;
		const unsigned char* pEnd = pString + stringLength;
		const unsigned char* pRange = pEnd;

		OnigRegion* region = ::onig_region_new();
		ScopeExit cleanup = [=]() { ::onig_region_free(region, 1); };

		String result;

		for (;;)
		{
			const int r = ::onig_search(m_regex, pString, pEnd, pStart, pRange, region, ONIG_OPTION_NONE);

			if (0 <= r)
			{
				Array<StringView> matches;

				for (int32 i = 0; i < region->num_regs; ++i)
				{
					if ((region->beg[i] == ONIG_REGION_NOTPOS)
						|| (region->end[i] == ONIG_REGION_NOTPOS))
					{
						matches.emplace_back();
						continue;
					}

					const size_t begIndex = (region->beg[i] / sizeof(char32));
					const size_t endIndex = (region->end[i] / sizeof(char32));
					const size_t length = (endIndex - begIndex);
					matches.push_back(s.substr(begIndex, length));
				}

				const MatchResults matchResults{ std::move(matches) };

				result.append(s.substr(((pPreviousStart - pString) / sizeof(char32)), (region->beg[0] / sizeof(char32) - (pPreviousStart - pString) / sizeof(char32))));
				result.append(replacementFunc(matchResults));

				const size_t begIndex = (region->beg[0] / sizeof(char32));
				const size_t endIndex = (region->end[0] / sizeof(char32));
				const size_t length = (endIndex - begIndex);
				pStart = (pString + (begIndex + length) * sizeof(char32));

				if (pStart == pPreviousStart)
				{
					pStart += sizeof(char32);
				}

				pPreviousStart = pStart;
			}
			else
			{
				result.append(s.substr((pPreviousStart - pString) / sizeof(char32)));
				return result;
			}
		}
	}
}
