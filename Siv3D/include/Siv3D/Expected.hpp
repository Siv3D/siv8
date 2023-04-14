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

# pragma once
# include <iostream>
# define TL_MONOSTATE_INPLACE_MUTEX
# include <ThirdParty/expected/expected.hpp>
# include "FormatData.hpp"

namespace s3d
{
	template <class E>
	using Unexpected = tl::unexpected<E>;

	template <class E>
	using BadExpectedAccess = tl::bad_expected_access<E>;

	using Unexpect_t = tl::unexpect_t;

	inline constexpr Unexpect_t unexpect{ tl::unexpect };

	template <class Type, class E>
	using Expected = tl::expected<Type, E>;

	template <class Type, class E>
	void Formatter(FormatData& formatData, const Expected<Type, E>& value);
}

# include "detail/Expected.ipp"
