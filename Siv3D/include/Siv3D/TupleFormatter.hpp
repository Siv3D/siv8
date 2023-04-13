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
# include <utility>
# include <tuple>
# include "Formatter.hpp"

namespace s3d
{
	template <class... Ts>
	void Formatter(FormatData& formatData, const std::tuple<Ts...>& tuple);

	template <class First, class Second>
	void Formatter(FormatData& formatData, const std::pair<First, Second>& pair);
}

# include "detail/TupleFormatter.ipp"
