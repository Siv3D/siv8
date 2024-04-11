﻿//-----------------------------------------------
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
# include <Siv3D/RegExp.hpp>
# include <ThirdParty/Oniguruma/oniguruma.h>

namespace s3d
{
	class RegExp::RegExpDetail
	{
	public:

		RegExpDetail();

		explicit RegExpDetail(StringView pattern);

		~RegExpDetail();

		[[nodiscard]]
		bool isValid() const noexcept;

		[[nodiscard]]
		bool fullMatch(StringView s) const;

		[[nodiscard]]
		MatchResults match(StringView s) const;

		[[nodiscard]]
		MatchResults search(StringView s) const;

		[[nodiscard]]
		Array<MatchResults> findAll(StringView s) const;

	private:

		regex_t* m_regex = nullptr;
	};
}
