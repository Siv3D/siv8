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
# include <vector>
# include <array>
# include <initializer_list>
# include "Formatter.hpp"

namespace s3d
{
	template <std::forward_iterator ForwardIt>
	void Formatter(FormatData& formatData, ForwardIt first, ForwardIt last);

	template <class Type, size_t N>
	void Formatter(FormatData& formatData, const Type(&values)[N]);

	template <class Type, size_t N>
	void Formatter(FormatData& formatData, const std::array<Type, N>& v);

	template <class Type, class Allocator = std::allocator<Type>>
	void Formatter(FormatData& formatData, const std::vector<Type, Allocator>& v);

	template <class Type>
	void Formatter(FormatData& formatData, const std::initializer_list<Type>& ilist);
}

# include "detail/RangeFormatter.ipp"
