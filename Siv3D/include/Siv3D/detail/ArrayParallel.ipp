//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	parallel_count_if
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	isize Array<Type, Allocator>::parallel_count_if(Fty f) const
		requires std::predicate<Fty&, const value_type&>
	{
		// The same callable object is shared by all worker threads.
		// Concurrent access to the callable and its referenced state must be synchronized by the caller.
		const auto function = std::ref(f);

	# if SIV3D_PLATFORM(WINDOWS)

		return std::count_if(std::execution::par, m_container.begin(), m_container.end(), function);

	# else

		if (m_container.empty())
		{
			return 0;
		}

		const size_t numThreads = Threading::GetConcurrency();

		if (numThreads <= 1)
		{
			return std::count_if(m_container.begin(), m_container.end(), function);
		}

		const size_t containerSize = m_container.size();
		const size_t countPerThread = ((containerSize / numThreads)
			+ static_cast<size_t>((containerSize % numThreads) != 0));

		Array<std::future<isize>> tasks;
		tasks.reserve(numThreads - 1);

		auto it = m_container.begin();
		size_t countLeft = containerSize;

		for (size_t i = 0; i < (numThreads - 1); ++i)
		{
			const size_t n = Min(countPerThread, countLeft);

			if (n == 0)
			{
				break;
			}

			const auto first = it;
			const auto last = (it + n);

			tasks.emplace_back(std::async(std::launch::async, [first, last, function]()
			{
				return std::count_if(first, last, function);
			}));

			it = last;
			countLeft -= n;
		}

		isize result = 0;
		
		if (countLeft)
		{
			result = std::count_if(it, (it + countLeft), function);
		}

		for (auto& task : tasks)
		{
			result += task.get();
		}

		return result;

	# endif
	}

	////////////////////////////////////////////////////////////////
	//
	//	parallel_each
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	void Array<Type, Allocator>::parallel_each(Fty f)
		requires std::invocable<Fty&, value_type&>
	{
		// The same callable object is shared by all worker threads.
		// Concurrent access to the callable and its referenced state must be synchronized by the caller.
		const auto function = std::ref(f);

	# if SIV3D_PLATFORM(WINDOWS)

		std::for_each(std::execution::par, m_container.begin(), m_container.end(), function);

	# else

		if (m_container.empty())
		{
			return;
		}

		const size_t numThreads = Threading::GetConcurrency();

		if (numThreads <= 1)
		{
			std::for_each(m_container.begin(), m_container.end(), function);
			return;
		}

		const size_t containerSize = size();
		const size_t countPerThread = ((containerSize / numThreads)
			+ static_cast<size_t>((containerSize % numThreads) != 0));

		Array<std::future<void>> tasks;
		tasks.reserve(numThreads - 1);

		auto it = m_container.begin();
		size_t countLeft = containerSize;

		for (size_t i = 0; i < (numThreads - 1); ++i)
		{
			const size_t n = Min(countPerThread, countLeft);

			if (n == 0)
			{
				break;
			}

			const auto first = it;
			const auto last = (it + n);

			tasks.emplace_back(std::async(std::launch::async, [first, last, function]()
			{
				std::for_each(first, last, function);
			}));

			it = last;
			countLeft -= n;
		}

		if (countLeft)
		{
			std::for_each(it, (it + countLeft), function);
		}

		for (auto& task : tasks)
		{
			task.get();
		}

	# endif
	}

	template <class Type, class Allocator>
	template <class Fty>
	void Array<Type, Allocator>::parallel_each(Fty f) const
		requires std::invocable<Fty&, const value_type&>
	{
		// The same callable object is shared by all worker threads.
		// Concurrent access to the callable and its referenced state must be synchronized by the caller.
		const auto function = std::ref(f);

	# if SIV3D_PLATFORM(WINDOWS)

		std::for_each(std::execution::par, m_container.begin(), m_container.end(), function);

	# else

		if (m_container.empty())
		{
			return;
		}

		const size_t numThreads = Threading::GetConcurrency();

		if (numThreads <= 1)
		{
			std::for_each(m_container.begin(), m_container.end(), function);
			return;
		}

		const size_t containerSize = size();
		const size_t countPerThread = ((containerSize / numThreads)
			+ static_cast<size_t>((containerSize % numThreads) != 0));

		Array<std::future<void>> tasks;
		tasks.reserve(numThreads - 1);

		auto it = m_container.begin();
		size_t countLeft = containerSize;

		for (size_t i = 0; i < (numThreads - 1); ++i)
		{
			const size_t n = Min(countPerThread, countLeft);

			if (n == 0)
			{
				break;
			}

			const auto first = it;
			const auto last = (it + n);

			tasks.emplace_back(std::async(std::launch::async, [first, last, function]()
			{
				std::for_each(first, last, function);
			}));

			it = last;
			countLeft -= n;
		}

		if (countLeft)
		{
			std::for_each(it, (it + countLeft), function);
		}

		for (auto& task : tasks)
		{
			task.get();
		}

	# endif
	}

	////////////////////////////////////////////////////////////////
	//
	//	parallel_map
	//
	////////////////////////////////////////////////////////////////

	namespace detail
	{
		template <class Source, class Fty>
		auto ArrayParallelMap(const Source& source, Fty& f)
		{
			using Result = std::decay_t<std::invoke_result_t<Fty&, const typename Source::value_type&>>;
			const size_t size = source.size();
			const size_t workers = Min(size, Threading::GetConcurrency());

			if (workers <= 1)
			{
				return source.map(std::ref(f));
			}

			// All workers share f. Futures are destroyed before any referenced local storage.
			const auto function = std::ref(f);
			const size_t batchSize = (size / workers);
			const size_t extra = (size % workers);

			if constexpr (std::is_trivially_default_constructible_v<Result> && std::is_move_assignable_v<Result>)
			{
				// Keep the single-allocation path for scalar and other cheap result types.
				Array<Result> result(size);
				Array<std::future<void>> tasks;
				tasks.reserve(workers - 1);
				auto src = source.begin();
				auto dst = result.begin();

				for (size_t i = 0; i < (workers - 1); ++i)
				{
					const size_t count = (batchSize + static_cast<size_t>(i < extra));
					tasks.emplace_back(std::async(std::launch::async, [src, dst, count, function]() mutable
					{
						for (size_t j = 0; j < count; ++j)
						{
							*dst++ = function(*src++);
						}
					}));
					src += count;
					dst += count;
				}

				while (src != source.end())
				{
					*dst++ = function(*src++);
				}
				for (auto& task : tasks)
				{
					task.get();
				}
				return result;
			}
			else
			{
				Array<Result> result(Arg::reserve = size);
				Array<std::future<Array<Result>>> tasks;
				tasks.reserve(workers - 1);
				auto src = source.begin();

				const auto buildChunk = [function](auto first, const size_t count)
				{
					Array<Result> chunk(Arg::reserve = count);
					for (size_t j = 0; j < count; ++j)
					{
						chunk.push_back(function(*first++));
					}
					return chunk;
				};

				for (size_t i = 0; i < (workers - 1); ++i)
				{
					const size_t count = (batchSize + static_cast<size_t>(i < extra));
					tasks.emplace_back(std::async(std::launch::async, buildChunk, src, count));
					src += count;
				}

				auto tail = buildChunk(src, static_cast<size_t>(source.end() - src));
				for (auto& task : tasks)
				{
					auto chunk = task.get();
					for (auto& value : chunk)
					{
						result.push_back(std::move(value));
					}
				}
				for (auto& value : tail)
				{
					result.push_back(std::move(value));
				}
				return result;
			}
		}
	}

	template <class Type, class Allocator>
	template <class Fty>
	auto Array<Type, Allocator>::parallel_map(Fty f) const
		requires detail::ArrayMapFunction<Fty, value_type>
	{
		return detail::ArrayParallelMap(*this, f);
	}

}
