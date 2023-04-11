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
# include <optional>
# include <iostream>

namespace s3d
{
	template <class Type>
	using Optional = std::optional<Type>;

	using None_t = std::nullopt_t;

	inline constexpr None_t none{ std::nullopt };

	inline constexpr None_t unspecified{ std::nullopt };

	template <class CharType>
	inline std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const None_t&)
	{
		constexpr CharType no[] = { 'n','o','n','e','\0' };
		return output << no;
	}
}
