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


# include <Siv3D/RegExp.hpp>
# include "RegExpDetail.hpp"

namespace s3d
{
	RegExp::RegExp()
		: pImpl{ std::make_shared<RegExpDetail>() } {}

	RegExp::RegExp(const StringView pattern)
		: pImpl{ std::make_shared<RegExpDetail>(pattern) } {}

	bool RegExp::isValid() const noexcept
	{
		return pImpl->isValid();
	}

	RegExp::operator bool() const noexcept
	{
		return pImpl->isValid();
	}

	bool RegExp::fullMatch(const StringView s) const
	{
		return pImpl->fullMatch(s);
	}

	MatchResults RegExp::match(const StringView s) const
	{
		return pImpl->match(s);
	}

	MatchResults RegExp::search(const StringView s) const
	{
		return pImpl->search(s);
	}

	Array<MatchResults> RegExp::findAll(const StringView s) const
	{
		return pImpl->findAll(s);
	}

	String RegExp::replaceFirst(const StringView s, const StringView replacement) const
	{
		return pImpl->replaceFirst(s, replacement);
	}

	String RegExp::replaceAll(const StringView s, const StringView replacement) const
	{
		return pImpl->replaceAll(s, replacement);
	}

	inline namespace Literals
	{
		inline namespace RegExpLiterals
		{
			RegExp operator ""_re(const char32* s, const size_t length) noexcept
			{
				return RegExp{ StringView{ s, length } };
			}
		}
	}
}
