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

# pragma once
# include <optional>

namespace s3d
{
	template <class Type>
	using Optional = std::optional<Type>;

	inline constexpr auto none = std::nullopt;
}
