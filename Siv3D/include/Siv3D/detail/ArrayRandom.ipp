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
	//	choice
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	typename Array<Type, Allocator>::value_type& Array<Type, Allocator>::choice()
	{
		return choice(GetDefaultRNG());
	}

	template <class Type, class Allocator>
	const typename Array<Type, Allocator>::value_type& Array<Type, Allocator>::choice() const
	{
		return choice(GetDefaultRNG());
	}

	template <class Type, class Allocator>
	typename Array<Type, Allocator>::value_type& Array<Type, Allocator>::choice(Concept::UniformRandomBitGenerator auto&& rbg)
	{
		const size_t size = m_container.size();

		if (size == 0)
		{
			detail::ThrowArrayChoiceOutOfRange();
		}

		return m_container[RandomClosedOpen<size_t>(0, size, std::forward<decltype(rbg)>(rbg))];
	}

	template <class Type, class Allocator>
	const typename Array<Type, Allocator>::value_type& Array<Type, Allocator>::choice(Concept::UniformRandomBitGenerator auto&& rbg) const
	{
		const size_t size = m_container.size();

		if (size == 0)
		{
			detail::ThrowArrayChoiceOutOfRange();
		}

		return m_container[RandomClosedOpen<size_t>(0, size, std::forward<decltype(rbg)>(rbg))];
	}

	template <class Type, class Allocator>
	Array<Type, Allocator> Array<Type, Allocator>::choice(const Concept::Integral auto n) const
	{
		return choice(n, GetDefaultRNG());
	}

	template <class Type, class Allocator>
	Array<Type, Allocator> Array<Type, Allocator>::choice(const Concept::Integral auto n, Concept::UniformRandomBitGenerator auto&& rbg) const
	{
		Array result(Arg::reserve = Min<size_t>(n, m_container.size()));

		std::sample(m_container.begin(), m_container.end(), std::back_inserter(result), n, std::forward<decltype(rbg)>(rbg));

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	shuffle, shuffled
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::shuffle()&
	{
		Shuffle(m_container.begin(), m_container.end(), GetDefaultRNG());
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::shuffle()&&
	{
		return std::move(shuffle());
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::shuffled() const&
	{
		Array result(*this);
		result.shuffle();
		return result;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::shuffled()&&
	{
		return std::move(shuffle());
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator>& Array<Type, Allocator>::shuffle(Concept::UniformRandomBitGenerator auto&& rbg)&
	{
		Shuffle(m_container.begin(), m_container.end(), std::forward<decltype(rbg)>(rbg));
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::shuffle(Concept::UniformRandomBitGenerator auto&& rbg)&&
	{
		return std::move(shuffle(std::forward<decltype(rbg)>(rbg)));
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::shuffled(Concept::UniformRandomBitGenerator auto&& rbg) const&
	{
		Array result(*this);
		result.shuffle(std::forward<decltype(rbg)>(rbg));
		return result;
	}

	template <class Type, class Allocator>
	constexpr Array<Type, Allocator> Array<Type, Allocator>::shuffled(Concept::UniformRandomBitGenerator auto&& rbg)&&
	{
		return std::move(shuffle(std::forward<decltype(rbg)>(rbg)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	choice
	//
	////////////////////////////////////////////////////////////////
	
	template <class Allocator>
	typename Array<bool, Allocator>::value_type& Array<bool, Allocator>::choice()
	{
		return choice(GetDefaultRNG());
	}

	template <class Allocator>
	const typename Array<bool, Allocator>::value_type& Array<bool, Allocator>::choice() const
	{
		return choice(GetDefaultRNG());
	}

	template <class Allocator>
	typename Array<bool, Allocator>::value_type& Array<bool, Allocator>::choice(Concept::UniformRandomBitGenerator auto&& rbg)
	{
		const size_t size = m_container.size();

		if (size == 0)
		{
			detail::ThrowArrayChoiceOutOfRange();
		}

		return m_container[RandomClosedOpen<size_t>(0, size, std::forward<decltype(rbg)>(rbg))];
	}

	template <class Allocator>
	const typename Array<bool, Allocator>::value_type& Array<bool, Allocator>::choice(Concept::UniformRandomBitGenerator auto&& rbg) const
	{
		const size_t size = m_container.size();

		if (size == 0)
		{
			detail::ThrowArrayChoiceOutOfRange();
		}

		return m_container[RandomClosedOpen<size_t>(0, size, std::forward<decltype(rbg)>(rbg))];
	}

	template <class Allocator>
	Array<bool, Allocator> Array<bool, Allocator>::choice(Concept::Integral auto n) const
	{
		return choice(n, GetDefaultRNG());
	}

	template <class Allocator>
	Array<bool, Allocator> Array<bool, Allocator>::choice(Concept::Integral auto n, Concept::UniformRandomBitGenerator auto&& rbg) const
	{
		Array result(Arg::reserve = Min<size_t>(n, m_container.size()));

		std::sample(m_container.begin(), m_container.end(), std::back_inserter(result), n, std::forward<decltype(rbg)>(rbg));

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	shuffle, shuffled
	//
	////////////////////////////////////////////////////////////////

	template <class Allocator>
	constexpr Array<bool, Allocator>& Array<bool, Allocator>::shuffle()&
	{
		Shuffle(m_container.begin(), m_container.end(), GetDefaultRNG());
		return *this;
	}

	template <class Allocator>
	constexpr Array<bool, Allocator> Array<bool, Allocator>::shuffle()&&
	{
		return std::move(shuffle());
	}

	template <class Allocator>
	constexpr Array<bool, Allocator> Array<bool, Allocator>::shuffled() const&
	{
		Array result(*this);
		result.shuffle();
		return result;
	}

	template <class Allocator>
	constexpr Array<bool, Allocator> Array<bool, Allocator>::shuffled()&&
	{
		return std::move(shuffle());
	}

	template <class Allocator>
	constexpr Array<bool, Allocator>& Array<bool, Allocator>::shuffle(Concept::UniformRandomBitGenerator auto&& rbg)&
	{
		Shuffle(m_container.begin(), m_container.end(), std::forward<decltype(rbg)>(rbg));
		return *this;
	}

	template <class Allocator>
	constexpr Array<bool, Allocator> Array<bool, Allocator>::shuffle(Concept::UniformRandomBitGenerator auto&& rbg)&&
	{
		return std::move(shuffle(std::forward<decltype(rbg)>(rbg)));
	}

	template <class Allocator>
	constexpr Array<bool, Allocator> Array<bool, Allocator>::shuffled(Concept::UniformRandomBitGenerator auto&& rbg) const&
	{
		Array result(*this);
		result.shuffle(std::forward<decltype(rbg)>(rbg));
		return result;
	}

	template <class Allocator>
	constexpr Array<bool, Allocator> Array<bool, Allocator>::shuffled(Concept::UniformRandomBitGenerator auto&& rbg)&&
	{
		return std::move(shuffle(std::forward<decltype(rbg)>(rbg)));
	}
}
