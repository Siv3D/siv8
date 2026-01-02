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

namespace s3d
{
	namespace detail
	{
		template <std::forward_iterator ForwardIt, class OutIt, std::integral Diff, Concept::UniformRandomBitGenerator URBG>
			requires std::output_iterator<OutIt, std::iter_reference_t<ForwardIt>>
		[[nodiscard]]
		OutIt SampleSelectionStable(ForwardIt first, ForwardIt last, OutIt out, Diff count, URBG& rng)
		{
			if (count <= 0)
			{
				return out;
			}

			using pop_diff_t = std::iter_difference_t<ForwardIt>;
			using CT = std::common_type_t<pop_diff_t, Diff>;

			const pop_diff_t pop_size_raw = std::ranges::distance(first, last);
			CT pop_size = static_cast<CT>(pop_size_raw);
			CT k = static_cast<CT>(count);

			if (pop_size < k)
			{
				k = pop_size; // match std::sample behavior
			}

			// selection sampling: for i from pop_size down to 1,
			// pick current element with probability k / i
			for (; 0 < pop_size; ++first, --pop_size)
			{
				if (k == 0)
				{
					break;
				}

				// rand in [0, pop_size-1]
				const CT r = RandomClosed<CT>(0, (pop_size - 1), rng);

				if (r < k)
				{
					*out++ = *first;
					--k;
				}
			}

			return out;
		}

		template <std::input_iterator InputIt, std::random_access_iterator RandomAccessOutIt, std::integral Diff, Concept::UniformRandomBitGenerator URBG>
			requires std::indirectly_writable<RandomAccessOutIt, std::iter_reference_t<InputIt>>
		[[nodiscard]]
		RandomAccessOutIt SampleReservoir(InputIt first, InputIt last, RandomAccessOutIt out, Diff count, URBG& rng)
		{
			if (count <= 0)
			{
				return out;
			}

			using pop_diff_t = std::iter_difference_t<InputIt>;
			using out_diff_t = std::iter_difference_t<RandomAccessOutIt>;
			using CT = std::common_type_t<pop_diff_t, Diff, out_diff_t>;

			const CT k = static_cast<CT>(count);

			CT i = 0;

			// fill initial reservoir [0, k)
			for (; i < k; ++i, ++first)
			{
				if (first == last)
				{
					return (out + static_cast<out_diff_t>(i)); // fewer than k elements
				}

				*(out + static_cast<out_diff_t>(i)) = *first;
			}

			// Now i == k, and we've consumed k elements.
			// For each next element (t = k+1, k+2, ...), choose j in [0, t-1] and replace if j < k.
			CT t = k;

			for (; first != last; ++first)
			{
				++t; // t = number of items seen so far
				const CT j = RandomClosed<CT>(0, (t - 1), rng);

				if (j < k)
				{
					*(out + static_cast<out_diff_t>(j)) = *first;
				}
			}

			return out + static_cast<out_diff_t>(k);
		}
	}

	template <std::input_iterator PopIt, class SampleIt, std::integral Diff, Concept::UniformRandomBitGenerator URBG>
	SampleIt Sample(PopIt first, PopIt last, SampleIt out, Diff count, URBG& rng)
	{
		if constexpr (std::forward_iterator<PopIt>)
		{
			static_assert(std::output_iterator<SampleIt, std::iter_reference_t<PopIt>>,
				"For forward iterators, output must accept assignment from *first");
			return detail::SampleSelectionStable(first, last, out, count, rng);
		}
		else
		{
			// input-only: require random-access output and valid storage [out, out+count)
			static_assert(std::random_access_iterator<SampleIt>,
				"For input-only iterators, output must be random access (reservoir sampling)");
			static_assert(std::indirectly_writable<SampleIt, std::iter_reference_t<PopIt>>,
				"Output must be writable from *first");
			return detail::SampleReservoir(first, last, out, count, rng);
		}
	}

	template <std::ranges::input_range R, class SampleIt, std::integral Diff, Concept::UniformRandomBitGenerator URBG>
	SampleIt Sample(R&& r, SampleIt out, Diff count, URBG& rng)
	{
		return Sample(std::ranges::begin(r), std::ranges::end(r), out, count, rng);
	}
}
