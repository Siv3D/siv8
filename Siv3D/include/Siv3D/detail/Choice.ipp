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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Choice
	//
	////////////////////////////////////////////////////////////////

	template <std::ranges::random_access_range Range>
	auto& Choice(Range& range)
	{
		return Choice(std::ranges::begin(range), std::ranges::end(range), DefaultRNG());
	}

	template <std::ranges::random_access_range Range>
	const auto& Choice(const Range& range)
	{
		return Choice(std::ranges::begin(range), std::ranges::end(range), DefaultRNG());
	}

	template <std::ranges::random_access_range Range>
	auto Choice(Range&& range)
	{
		return Choice(std::ranges::begin(range), std::ranges::end(range), DefaultRNG());
	}

	template <class Type>
	Type Choice(std::initializer_list<Type> list)
	{
		return Choice(list.begin(), list.end(), DefaultRNG());
	}

	template <std::ranges::random_access_range Range>
	auto& Choice(Range& range, Concept::UniformRandomBitGenerator auto&& rbg)
	{
		return Choice(std::ranges::begin(range), std::ranges::end(range), std::forward<decltype(rbg)>(rbg));
	}

	template <std::ranges::random_access_range Range>
	const auto& Choice(const Range& range, Concept::UniformRandomBitGenerator auto&& rbg)
	{
		return Choice(std::ranges::begin(range), std::ranges::end(range), std::forward<decltype(rbg)>(rbg));
	}

	template <std::ranges::random_access_range Range>
	auto Choice(Range&& range, Concept::UniformRandomBitGenerator auto&& rbg)
	{
		return Choice(std::ranges::begin(range), std::ranges::end(range), std::forward<decltype(rbg)>(rbg));
	}

	template <class Type>
	auto Choice(std::initializer_list<Type> list, Concept::UniformRandomBitGenerator auto&& rbg)
	{
		return Choice(list.begin(), list.end(), std::forward<decltype(rbg)>(rbg));
	}

	template <std::random_access_iterator RandomIt, std::sentinel_for<RandomIt> Sentinel>
	auto& Choice(RandomIt first, Sentinel last)
	{
		return Choice(first, last, DefaultRNG());
	}

	template <std::random_access_iterator RandomIt, std::sentinel_for<RandomIt> Sentinel>
	const auto& Choice(RandomIt first, Sentinel last)
	{
		return Choice(first, last, DefaultRNG());
	}

	template <std::random_access_iterator RandomIt, std::sentinel_for<RandomIt> Sentinel>
	auto& Choice(RandomIt first, Sentinel last, Concept::UniformRandomBitGenerator auto&& rbg)
	{
		const auto size = (last - first);
		const auto randomIndex = RandomClosedOpen<size_t>(0, size, std::forward<decltype(rbg)>(rbg));
		return *(first + randomIndex);
	}
}
