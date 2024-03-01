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
	//	Shuffle
	//
	////////////////////////////////////////////////////////////////

	template <class Range> requires std::permutable<std::ranges::iterator_t<Range>>
	void Shuffle(Range&& range)
	{
		Shuffle(std::ranges::begin(range), std::ranges::end(range), DefaultRNG());
	}

	template <class Range, Concept::UniformRandomBitGenerator URBG> requires std::permutable<std::ranges::iterator_t<Range>>
	void Shuffle(Range&& range, URBG&& urbg)
	{
		Shuffle(std::ranges::begin(range), std::ranges::end(range), std::forward<URBG>(urbg));
	}

	template <class RandomIt> requires std::permutable<RandomIt>
	void Shuffle(RandomIt first, RandomIt last)
	{
		Shuffle(first, last, DefaultRNG());
	}

	template <class RandomIt, Concept::UniformRandomBitGenerator URBG> requires std::permutable<RandomIt>
	void Shuffle(RandomIt first, RandomIt last, URBG&& urbg)
	{
		if (first == last)
		{
			return;
		}

		using difference_type = typename std::iterator_traits<RandomIt>::difference_type;

		for (auto it = (first + 1); it < last; ++it)
		{
			const uint64 n = static_cast<uint64>(it - first);
			std::iter_swap(it, first + static_cast<difference_type>(Random<uint64>(0, n, std::forward<URBG>(urbg))));
		}
	}
}
