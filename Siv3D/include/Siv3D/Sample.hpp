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
# include <iterator>
# include <type_traits>
# include <ranges>
# include "Concepts.hpp"
# include "Random.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Sample
	//
	////////////////////////////////////////////////////////////////

	/// @brief 指定された個数の要素を母集団からランダムに抽出します。
	/// @tparam SampleIt 出力イテレータ型
	/// @tparam URBG 乱数生成器の型
	/// @tparam PopIt 入力イテレータ型
	/// @tparam Diff 抽出数を表す整数型
	/// @param first 母集団の先頭イテレータ
	/// @param last 母集団の終端イテレータ
	/// @param out 出力先の先頭
	/// @param count 抽出する要素数
	/// @param rng 乱数生成器
	/// @return 書き込み後の出力イテレータ
	template <std::input_iterator PopIt, class SampleIt, std::integral Diff, Concept::UniformRandomBitGenerator URBG>
	SampleIt Sample(PopIt first, PopIt last, SampleIt out, Diff count, URBG& rng);

	/// @brief 指定された個数の要素を母集団からランダムに抽出します。
	/// @tparam SampleIt 出力イテレータ型
	/// @tparam R 入力レンジ型
	/// @tparam Diff 抽出数を表す整数型
	/// @tparam URBG 乱数生成器の型
	/// @param r 母集団レンジ
	/// @param out 出力先の先頭
	/// @param count 抽出する要素数
	/// @param rng 乱数生成器
	/// @return 書き込み後の出力イテレータ
	template <std::ranges::input_range R, class SampleIt, std::integral Diff, Concept::UniformRandomBitGenerator URBG>
	SampleIt Sample(R&& r, SampleIt out, Diff count, URBG& rng);
}

# include "detail/Sample.ipp"
