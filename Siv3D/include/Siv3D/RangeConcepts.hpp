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

# pragma once
# include <concepts>
# include <ranges>

namespace s3d
{
	namespace Concept
	{
		////////////////////////////////////////////////////////////////
		//
		//	ContainerCompatibleRange
		//
		////////////////////////////////////////////////////////////////

		/// @brief コンテナに対応した Range のコンセプト | Container compatible range concept
		template <class Range, class Elem>
		concept ContainerCompatibleRange = (std::ranges::input_range<Range> && std::convertible_to<std::ranges::range_reference_t<Range>, Elem>);
	}
}
