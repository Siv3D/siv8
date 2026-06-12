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
	namespace detail
	{
		template <class Type>
		class ArrayStableUniqueHelper
		{
		public:

			[[nodiscard]]
			bool operator()(const Type& value)
			{
				return m_set.insert(value).second;
			}

			void reserve(size_t n)
			{
				m_set.reserve(n);
			}

		private:

			HashSet<Type> m_set;
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_first
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr bool Array<Type, Allocator>::erase_first(const value_type& value)
	{
		if (const auto it = std::ranges::find(m_container, value);
			it != m_container.end())
		{
			m_container.erase(it);
			return true;
		}
		else
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_first_unstable
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr bool Array<Type, Allocator>::erase_first_unstable(const value_type& value)
	{
		if (const auto it = std::ranges::find(m_container, value);
			it != m_container.end())
		{
			const auto last = (m_container.end() - 1);

			if (it != last)
			{
				std::ranges::iter_swap(it, last);
			}

			m_container.pop_back();

			return true;
		}
		else
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_first_if
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr bool Array<Type, Allocator>::erase_first_if(Fty f)
		requires std::predicate<Fty&, const value_type&>
	{
		if (const auto it = std::ranges::find_if(m_container, detail::PassFunction(std::forward<Fty>(f)));
			it != m_container.end())
		{
			m_container.erase(it);
			return true;
		}
		else
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_first_if_unstable
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr bool Array<Type, Allocator>::erase_first_if_unstable(Fty f)
		requires std::predicate<Fty&, const value_type&>
	{
		if (const auto it = std::ranges::find_if(m_container, detail::PassFunction(std::forward<Fty>(f)));
			it != m_container.end())
		{
			const auto last = (m_container.end() - 1);

			if (it != last)
			{
				std::ranges::iter_swap(it, last);
			}

			m_container.pop_back();

			return true;
		}
		else
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	fold_left
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class R, class Fty>
	constexpr auto Array<Type, Allocator>::fold_left(R init, Fty f) const
	{
		return std::ranges::fold_left(m_container, init, detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	stable_unique, stable_uniqued
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::stable_unique() &
	{
		detail::ArrayStableUniqueHelper<value_type> pred;
		pred.reserve(m_container.size());

		const auto removed = std::ranges::remove_if(m_container, [&pred](const value_type& v) { return (not pred(v)); });
		m_container.erase(removed.begin(), removed.end());
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::stable_unique() &&
	{
		return std::move(stable_unique());
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::stable_uniqued() const
	{
		Array result(Arg::reserve = m_container.size());

		detail::ArrayStableUniqueHelper<value_type> pred;
		pred.reserve(m_container.size());

		std::ranges::copy_if(m_container, std::back_inserter(result), std::ref(pred));
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	unique_consecutive, uniqued_consecutive
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::unique_consecutive() &
	{
		auto result = std::ranges::unique(m_container);
		m_container.erase(result.begin(), result.end());
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::unique_consecutive() &&
	{
		return std::move(unique_consecutive());
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::uniqued_consecutive() const&
	{
		Array result;
		std::ranges::unique_copy(m_container, std::back_inserter(result));
		return result;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::uniqued_consecutive() &&
		noexcept(std::is_nothrow_move_assignable_v<value_type>)
	{
		return std::move(unique_consecutive());
	}

	////////////////////////////////////////////////////////////////
	//
	//	fold_left
	//
	////////////////////////////////////////////////////////////////

	template <class Allocator>
	template <class R, class Fty>
	constexpr auto Array<bool, Allocator>::fold_left(R init, Fty f) const
	{
		return std::ranges::fold_left(m_container.begin(), m_container.end(), init, detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	unique_consecutive, uniqued_consecutive
	//
	////////////////////////////////////////////////////////////////

	template <class Allocator>
	constexpr Array<bool, Allocator>& Array<bool, Allocator>::unique_consecutive() & noexcept
	{
		auto result = std::ranges::unique(m_container);
		m_container.erase(result.begin(), result.end());
		return *this;
	}

	template <class Allocator>
	constexpr Array<bool, Allocator> Array<bool, Allocator>::unique_consecutive() && noexcept
	{
		return std::move(unique_consecutive());
	}

	template <class Allocator>
	constexpr Array<bool, Allocator> Array<bool, Allocator>::uniqued_consecutive() const&
	{
		Array result;
		std::ranges::unique_copy(m_container, std::back_inserter(result));
		return result;
	}

	template <class Allocator>
	constexpr Array<bool, Allocator> Array<bool, Allocator>::uniqued_consecutive() && noexcept
	{
		return std::move(unique_consecutive());
	}
}
