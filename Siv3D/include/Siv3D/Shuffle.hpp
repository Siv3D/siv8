﻿//-----------------------------------------------
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
# include "Common.hpp"
# include "Random.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Shuffle
	//
	////////////////////////////////////////////////////////////////

	/// @brief 範囲の要素をランダムにシャッフルします。
	/// @tparam Range 範囲の型
	/// @param range 範囲
	template <class Range> requires std::permutable<std::ranges::iterator_t<Range>>
	void Shuffle(Range&& range);

	/// @brief 範囲の要素をランダムにシャッフルします。
	/// @tparam Range 範囲の型
	/// @param range 範囲
	/// @param urbg 乱数エンジン
	template <class Range> requires std::permutable<std::ranges::iterator_t<Range>>
	void Shuffle(Range&& range, Concept::UniformRandomBitGenerator auto&& urbg);

	/// @brief 指定した範囲の要素をランダムにシャッフルします。
	/// @tparam RandomIt イテレータの型
	/// @param first 範囲の開始位置を指すイテレータ
	/// @param last 範囲の終端位置を指すイテレータ
	template <class RandomIt> requires std::permutable<RandomIt>
	void Shuffle(RandomIt first, RandomIt last);

	/// @brief 指定した範囲の要素をランダムにシャッフルします。
	/// @tparam RandomIt イテレータの型
	/// @param first 範囲の開始位置を指すイテレータ
	/// @param last 範囲の終端位置を指すイテレータ
	/// @param urbg 乱数エンジン
	template <class RandomIt> requires std::permutable<RandomIt>
	void Shuffle(RandomIt first, RandomIt last, Concept::UniformRandomBitGenerator auto&& urbg);
}

# include "detail/Shuffle.ipp"
