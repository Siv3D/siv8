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

# pragma once
# include <span>
# include <vector>
# include <array>
# include <initializer_list>
# include "Array.hpp"
# include "Formatter.hpp"
# include "EscapedFormatter.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	void Formatter(FormatData& formatData, std::span<const Type> span);

	template <class Type, size_t N>
	void Formatter(FormatData& formatData, const Type(&values)[N]);

	template <class Type, size_t N>
	void Formatter(FormatData& formatData, const std::array<Type, N>& v);

	template <class Type, class Allocator = std::allocator<Type>>
	void Formatter(FormatData& formatData, const std::vector<Type, Allocator>& v);

	template <class Type, class Allocator = std::allocator<Type>>
	void Formatter(FormatData& formatData, const Array<Type, Allocator>& v);

	template <class Type>
	void Formatter(FormatData& formatData, std::initializer_list<Type> list);
}

# include "detail/SequenceFormatter.ipp"
