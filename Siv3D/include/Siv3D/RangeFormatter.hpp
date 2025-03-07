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
# include <ranges>
# include "Formatter.hpp"
# include "EscapedFormatter.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Concept::FormattableRange
	//
	////////////////////////////////////////////////////////////////

	namespace Concept
	{
		template <class Range>
		concept FormattableRange = std::ranges::input_range<Range>
			&& Formattable<std::ranges::range_reference_t<const Range>>;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief Range を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param range Range
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, const Concept::FormattableRange auto& range);
}

# include "detail/RangeFormatter.ipp"
