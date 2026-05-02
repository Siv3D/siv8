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
	//	erase_all
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Array<Type, Allocator>::size_type Array<Type, Allocator>::erase_all(const value_type& value)
	{
		return std::erase(m_container, value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase_if
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr typename Array<Type, Allocator>::size_type Array<Type, Allocator>::erase_if(Fty f)
		requires std::predicate<Fty&, const value_type&>
	{
		return std::erase_if(m_container, detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	reduce
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty, class R>
	constexpr auto Array<Type, Allocator>::reduce(Fty f, R init) const
	{
		return std::reduce(m_container.begin(), m_container.end(), init, f);
	}

	////////////////////////////////////////////////////////////////
	//
	//	stable_unique, stable_uniqued
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::stable_unique() &
	{
		return (*this = stable_uniqued());
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::stable_unique() &&
	{
		return stable_uniqued();
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::stable_uniqued() const
	{
		Array result(Arg::reserve = m_container.size());

		detail::ArrayStableUniqueHelper<value_type> pred;
		pred.reserve(m_container.size());

		std::copy_if(m_container.begin(), m_container.end(), std::back_inserter(result), std::ref(pred));

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
		m_container.erase(std::unique(m_container.begin(), m_container.end()), m_container.end());
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
		std::unique_copy(m_container.begin(), m_container.end(), std::back_inserter(result));
		return result;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::uniqued_consecutive() && noexcept
	{
		return std::move(unique_consecutive());
	}

	////////////////////////////////////////////////////////////////
	//
	//	reduce
	//
	////////////////////////////////////////////////////////////////

	template <class Allocator>
	template <class Fty, class R>
	constexpr auto Array<bool, Allocator>::reduce(Fty f, R init) const
	{
		return std::reduce(m_container.begin(), m_container.end(), init, f);
	}

	////////////////////////////////////////////////////////////////
	//
	//	unique_consecutive, uniqued_consecutive
	//
	////////////////////////////////////////////////////////////////

	template <class Allocator>
	constexpr Array<bool, Allocator>& Array<bool, Allocator>::unique_consecutive() & noexcept
	{
		m_container.erase(std::unique(m_container.begin(), m_container.end()), m_container.end());
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
		std::unique_copy(m_container.begin(), m_container.end(), std::back_inserter(result));
		return result;
	}

	template <class Allocator>
	constexpr Array<bool, Allocator> Array<bool, Allocator>::uniqued_consecutive() && noexcept
	{
		return std::move(unique_consecutive());
	}
}
