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
# include "Common.hpp"
# include "Random.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Choice
	//
	////////////////////////////////////////////////////////////////

	/// @brief Range から要素をランダムに返します。
	/// @tparam Range ランダムアクセス可能な Range の型
	/// @param range Range
	/// @return Range からランダムに選択された要素への参照
	template <std::ranges::random_access_range Range>
	[[nodiscard]]
	auto& Choice(Range& range);

	/// @brief Range から要素をランダムに返します。
	/// @tparam Range ランダムアクセス可能な Range の型
	/// @param range Range
	/// @return Range からランダムに選択された要素への参照
	template <std::ranges::random_access_range Range>
	[[nodiscard]]
	const auto& Choice(const Range& range);

	/// @brief Range から要素をランダムに返します。
	/// @tparam Range ランダムアクセス可能な Range の型
	/// @param range Range
	/// @return Range からランダムに選択された要素
	template <std::ranges::random_access_range Range>
	[[nodiscard]]
	auto Choice(Range&& range);

	/// @brief リストから要素をランダムに返します。
	/// @tparam Type 要素の型
	/// @param list リスト
	/// @return リストからランダムに選択された要素
	template <class Type>
	[[nodiscard]]
	Type Choice(std::initializer_list<Type> list);

	/// @brief 指定した乱数エンジンを用いて、Range から要素をランダムに返します。
	/// @tparam Range ランダムアクセス可能な Range の型
	/// @param range Range
	/// @param rbg 乱数エンジン
	/// @return Range からランダムに選択された要素への参照
	template <std::ranges::random_access_range Range>
	[[nodiscard]]
	auto& Choice(Range& range, Concept::UniformRandomBitGenerator auto&& rbg);

	/// @brief 指定した乱数エンジンを用いて、Range から要素をランダムに返します。
	/// @tparam Range ランダムアクセス可能な Range の型
	/// @param range Range
	/// @param rbg 乱数エンジン
	/// @return Range からランダムに選択された要素への参照
	template <std::ranges::random_access_range Range>
	[[nodiscard]]
	const auto& Choice(const Range& range, Concept::UniformRandomBitGenerator auto&& rbg);

	/// @brief 指定した乱数エンジンを用いて、Range から要素をランダムに返します。
	/// @tparam Range ランダムアクセス可能な Range の型
	/// @param range Range
	/// @param rbg 乱数エンジン
	/// @return Range からランダムに選択された要素
	template <std::ranges::random_access_range Range>
	[[nodiscard]]
	auto Choice(Range&& range, Concept::UniformRandomBitGenerator auto&& rbg);

	/// @brief 指定した乱数エンジンを用いて、リストから要素をランダムに返します。
	/// @tparam Type 要素の型
	/// @param list リスト
	/// @param rbg 乱数エンジン
	/// @return リストからランダムに選択された要素
	template <class Type>
	[[nodiscard]]
	auto Choice(std::initializer_list<Type> list, Concept::UniformRandomBitGenerator auto&& rbg);

	/// @brief 指定した範囲から要素をランダムに返します。
	/// @tparam RandomIt 先頭イテレータの型
	/// @tparam Sentinel 終端イテレータの型
	/// @param first 範囲の先頭を指すイテレータ
	/// @param last 範囲の終端を指すイテレータ
	/// @return 範囲からランダムに選択された要素への参照
	template <std::random_access_iterator RandomIt, std::sentinel_for<RandomIt> Sentinel>
	[[nodiscard]]
	auto& Choice(RandomIt first, Sentinel last);

	/// @brief 指定した範囲から要素をランダムに返します。
	/// @tparam RandomIt 先頭イテレータの型
	/// @tparam Sentinel 終端イテレータの型
	/// @param first 範囲の先頭を指すイテレータ
	/// @param last 範囲の終端を指すイテレータ
	/// @return 範囲からランダムに選択された要素への参照
	template <std::random_access_iterator RandomIt, std::sentinel_for<RandomIt> Sentinel>
	[[nodiscard]]
	const auto& Choice(RandomIt first, Sentinel last);

	/// @brief 指定した乱数エンジンを用いて、範囲から要素をランダムに返します。
	/// @tparam RandomIt 先頭イテレータの型
	/// @tparam Sentinel 終端イテレータの型
	/// @param first 範囲の先頭を指すイテレータ
	/// @param last 範囲の終端を指すイテレータ
	/// @param rbg 乱数エンジン
	/// @return 範囲からランダムに選択された要素への参照
	template <std::random_access_iterator RandomIt, std::sentinel_for<RandomIt> Sentinel>
	[[nodiscard]]
	auto& Choice(RandomIt first, Sentinel last, Concept::UniformRandomBitGenerator auto&& rbg);
}

# include "detail/Choice.ipp"
