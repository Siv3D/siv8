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
	//	parallel_count_if
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	isize Array<Type, Allocator>::parallel_count_if(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
	# if SIV3D_PLATFORM(WINDOWS)

		return std::count_if(std::execution::par, m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));

	# else

		if (m_container.empty())
		{
			return 0;
		}

		const size_t numThreads = Threading::GetConcurrency();

		if (numThreads <= 1)
		{
			return std::count_if(m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));
		}

		const size_t countPerthread = Max<size_t>(1, ((m_container.size() + (numThreads - 1)) / numThreads));

		Array<std::future<isize>> tasks;

		auto it = m_container.begin();
		size_t countLeft = m_container.size();

		for (size_t i = 0; i < (numThreads - 1); ++i)
		{
			const size_t n = Min(countPerthread, countLeft);

			if (n == 0)
			{
				break;
			}

			tasks.emplace_back(std::async(std::launch::async, [=, &f]()
			{
				return std::count_if(it, (it + n), f);
			}));

			it += n;
			countLeft -= n;
		}

		isize result = 0;
		
		if (countLeft)
		{
			result = std::count_if(it, (it + countLeft), f);
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
	void Array<Type, Allocator>::parallel_each(Fty f) requires std::invocable<Fty&, value_type&>
	{
	# if SIV3D_PLATFORM(WINDOWS)

		std::for_each(std::execution::par, m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));

	# else

		if (m_container.empty())
		{
			return;
		}

		const size_t numThreads = Threading::GetConcurrency();

		if (numThreads <= 1)
		{
			return each(std::forward<Fty>(f));
		}

		const size_t countPerthread = Max<size_t>(1, ((size() + (numThreads - 1)) / numThreads));

		Array<std::future<void>> tasks;

		auto it = m_container.begin();
		size_t countLeft = size();

		for (size_t i = 0; i < (numThreads - 1); ++i)
		{
			const size_t n = Min(countPerthread, countLeft);

			if (n == 0)
			{
				break;
			}

			tasks.emplace_back(std::async(std::launch::async, [=, &f]()
			{
				std::for_each(it, (it + n), f);
			}));

			it += n;
			countLeft -= n;
		}

		if (countLeft)
		{
			std::for_each(it, (it + countLeft), f);
		}

		for (auto& task : tasks)
		{
			task.get();
		}

	# endif
	}

	template <class Type, class Allocator>
	template <class Fty>
	void Array<Type, Allocator>::parallel_each(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
	# if SIV3D_PLATFORM(WINDOWS)

		std::for_each(std::execution::par, m_container.begin(), m_container.end(), detail::PassFunction(std::forward<Fty>(f)));

	# else

		if (m_container.empty())
		{
			return;
		}

		const size_t numThreads = Threading::GetConcurrency();

		if (numThreads <= 1)
		{
			return each(std::forward<Fty>(f));
		}

		const size_t countPerthread = Max<size_t>(1, ((size() + (numThreads - 1)) / numThreads));

		Array<std::future<void>> tasks;

		auto it = m_container.begin();
		size_t countLeft = size();

		for (size_t i = 0; i < (numThreads - 1); ++i)
		{
			const size_t n = Min(countPerthread, countLeft);

			if (n == 0)
			{
				break;
			}

			tasks.emplace_back(std::async(std::launch::async, [=, &f]()
			{
				std::for_each(it, (it + n), f);
			}));

			it += n;
			countLeft -= n;
		}

		if (countLeft)
		{
			std::for_each(it, (it + countLeft), f);
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
	auto Array<Type, Allocator>::parallel_map(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		using result_value_type = std::decay_t<std::invoke_result_t<Fty&, const value_type&>>;

		if (m_container.empty())
		{
			return Array<result_value_type>{};
		}

		const size_t numThreads = Threading::GetConcurrency();

		if (numThreads <= 1)
		{
			return map(std::forward<Fty>(f));
		}

		Array<result_value_type> result(m_container.size());

		const size_t countPerthread = Max<size_t>(1, ((m_container.size() + (numThreads - 1)) / numThreads));

		Array<std::future<void>> tasks;

		auto itDst = result.begin();
		auto itSrc = m_container.begin();
		size_t countLeft = m_container.size();

		for (size_t i = 0; i < (numThreads - 1); ++i)
		{
			const size_t n = Min(countPerthread, countLeft);

			if (n == 0)
			{
				break;
			}

			tasks.emplace_back(std::async(std::launch::async, [=, &f]() mutable
			{
				const auto itSrcEnd = (itSrc + n);

				while (itSrc != itSrcEnd)
				{
					*itDst++ = f(*itSrc++);
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
				*itDst++ = f(*itSrc++);
			}
		}

		for (auto& task : tasks)
		{
			task.get();
		}

		return result;
	}
}

