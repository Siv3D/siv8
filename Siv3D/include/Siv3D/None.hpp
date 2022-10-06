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
# include <ostream>
# include <optional>
# include "Common.hpp"

namespace s3d
{
	struct FormatData;

	/// @brief 無効値の型
	using None_t = std::nullopt_t;

	std::ostream& operator <<(std::ostream& os, const None_t&);

	std::wostream& operator <<(std::wostream& os, const None_t&);

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const None_t&);

	void Formatter(FormatData& formatData, None_t);

	/// @brief 無効値
	inline constexpr None_t none = std::nullopt;
}
