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
# include <utility>
# include <tuple>
# include "Formatter.hpp"
# include "EscapedFormatter.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief タプルを文字列に変換します。
	/// @tparam ...Ts タプルの要素の型
	/// @param formatData 文字列バッファ
	/// @param tuple タプル
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	template <class... Ts>
	void Formatter(FormatData& formatData, const std::tuple<Ts...>& tuple);

	/// @brief ペアを文字列に変換します。
	/// @tparam First ペアの 1 つ目の要素の型
	/// @tparam Second ペアの 2 つ目の要素の型
	/// @param formatData 文字列バッファ
	/// @param pair ペア
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	template <class First, class Second>
	void Formatter(FormatData& formatData, const std::pair<First, Second>& pair);
}

# include "detail/TupleFormatter.ipp"
