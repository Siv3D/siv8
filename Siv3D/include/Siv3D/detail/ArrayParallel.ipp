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

	template <class Type, class Allocator>
	template <class Fty>
	auto Array<Type, Allocator>::parallel_map(Fty f) const
		requires std::invocable<Fty&, const value_type&>
	{
		using result_value_type = std::decay_t<std::invoke_result_t<Fty&, const value_type&>>;

		if (m_container.empty())
		{
			return Array<result_value_type>{};
		}

		// The same callable object is shared by all worker threads.
		// Concurrent access to the callable and its referenced state must be synchronized by the caller.
		const auto function = std::ref(f);

		const size_t numThreads = Threading::GetConcurrency();

		if (numThreads <= 1)
		{
			return map(function);
		}

		const size_t containerSize = m_container.size();
		Array<result_value_type> result(containerSize);

		const size_t countPerThread = ((containerSize / numThreads)
			+ static_cast<size_t>((containerSize % numThreads) != 0));

		Array<std::future<void>> tasks;
		tasks.reserve(numThreads - 1);

		auto itDst = result.begin();
		auto itSrc = m_container.begin();
		size_t countLeft = containerSize;

		for (size_t i = 0; i < (numThreads - 1); ++i)
		{
			const size_t n = Min(countPerThread, countLeft);

			if (n == 0)
			{
				break;
			}

			const auto dstBegin = itDst;
			const auto srcBegin = itSrc;

			tasks.emplace_back(std::async(std::launch::async, [dstBegin, srcBegin, n, function]() mutable
			{
				auto dst = dstBegin;
				auto src = srcBegin;
				const auto srcEnd = (src + n);

				while (src != srcEnd)
				{
					*dst++ = function(*src++);
				}
			}));

			itDst += n;
			itSrc += n;
			countLeft -= n;
		}

		if (countLeft)
		{
			const auto itSrcEnd = m_container.end();

			while (itSrc != itSrcEnd)
			{
				*itDst++ = function(*itSrc++);
			}
		}

		for (auto& task : tasks)
		{
			task.get();
		}

		return result;
	}
}
