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
	//	all
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr bool String::all(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return std::all_of(m_string.begin(), m_string.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	any
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr bool String::any(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return std::any_of(m_string.begin(), m_string.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr bool String::contains_if(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return any(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	count
	//
	////////////////////////////////////////////////////////////////

	constexpr int64 String::count(const value_type ch) const noexcept
	{
		return std::count(m_string.begin(), m_string.end(), ch);
	}

	////////////////////////////////////////////////////////////////
	//
	//	count_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr int64 String::count_if(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return std::count_if(m_string.begin(), m_string.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	each
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr void String::each(Fty f) requires std::invocable<Fty&, value_type&>
	{
		std::for_each(m_string.begin(), m_string.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	template <class Fty>
	constexpr void String::each(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		std::for_each(m_string.begin(), m_string.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	each_index
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr void String::each_index(Fty f) requires std::invocable<Fty&, size_t, value_type&>
	{
		for (size_t i = 0; auto& ch : m_string)
		{
			f(i++, ch);
		}
	}

	template <class Fty>
	constexpr void String::each_index(Fty f) const requires std::invocable<Fty&, size_t, const value_type&>
	{
		for (size_t i = 0; auto ch : m_string)
		{
			f(i++, ch);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	each_sindex
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr void String::each_sindex(Fty f) requires std::invocable<Fty&, isize, value_type&>
	{
		for (isize i = 0; auto& ch : m_string)
		{
			f(i++, ch);
		}
	}

	template <class Fty>
	constexpr void String::each_sindex(Fty f) const requires std::invocable<Fty&, isize, const value_type&>
	{
		for (isize i = 0; auto ch : m_string)
		{
			f(i++, ch);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	fetch
	//
	////////////////////////////////////////////////////////////////

	constexpr String::value_type String::fetch(const size_type index, const value_type defaultValue) const noexcept
	{
		if (m_string.size() <= index)
		{
			return defaultValue;
		}

		return m_string[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	fill
	//
	////////////////////////////////////////////////////////////////

	constexpr void String::fill(const value_type value)
	{
		return std::fill(m_string.begin(), m_string.end(), value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	filter
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr String String::filter(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		String result;

		for (const auto ch : m_string)
		{
			if (f(ch))
			{
				result.push_back(ch);
			}
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isSorted
	//
	////////////////////////////////////////////////////////////////

	constexpr bool String::isSorted() const noexcept
	{
		return std::is_sorted(m_string.begin(), m_string.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	keep_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr String& String::keep_if(Fty f) requires std::predicate<Fty&, const value_type&>
	{
		m_string.erase(std::remove_if(m_string.begin(), m_string.end(), std::not_fn(std::forward<Fty>(f))), m_string.end());
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	map
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr auto String::map(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		using result_value_type = std::decay_t<std::invoke_result_t<Fty&, const value_type&>>;

		Array<result_value_type, std::allocator<result_value_type>> result;

		result.reserve(m_string.size());

		for (const auto ch : m_string)
		{
			result.push_back(f(ch));
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	none
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr bool String::none(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return std::none_of(m_string.begin(), m_string.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove_if / removed_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr String& String::remove_if(Fty f) & noexcept requires std::predicate<Fty&, const value_type&>
	{
		m_string.erase(std::remove_if(m_string.begin(), m_string.end(), detail::PassFunction(std::forward<Fty>(f))), m_string.end());
		return *this;
	}

	template <class Fty>
	[[nodiscard]]
	constexpr String String::remove_if(Fty f) && noexcept requires std::predicate<Fty&, const value_type&>
	{
		return std::move(remove_if(std::forward<Fty>(f)));
	}

	template <class Fty>
	[[nodiscard]]
	constexpr String String::removed_if(Fty f) const& requires std::predicate<Fty&, const value_type&>
	{
		String result;

		for (const auto ch : m_string)
		{
			if (not f(ch))
			{
				result.push_back(ch);
			}
		}

		return result;
	}

	template <class Fty>
	[[nodiscard]]
	constexpr String String::removed_if(Fty f) && noexcept requires std::predicate<Fty&, const value_type&>
	{
		return std::move(remove_if(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	repeat
	//
	////////////////////////////////////////////////////////////////

	constexpr String String::repeat(const size_t count) const
	{
		const size_t blockLength = m_string.size();
		const size_t blockMemorySize = (blockLength * sizeof(value_type));

		String result((blockLength * count), U'\0');
		value_type* pDst = result.data();
		const value_type* pSrc = m_string.data();

		for (size_t i = 0; i < count; ++i)
		{
			std::memcpy(pDst, pSrc, blockMemorySize);
			pDst += blockLength;
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	replace_if / replaced_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr String& String::replace_if(Fty f, const value_type newChar) & noexcept requires std::predicate<Fty&, const value_type&>
	{
		for (auto& c : m_string)
		{
			if (f(c))
			{
				c = newChar;
			}
		}

		return *this;
	}

	template <class Fty>
	constexpr String String::replace_if(Fty f, const value_type newChar) && noexcept requires std::predicate<Fty&, const value_type&>
	{
		return std::move(replace_if(f, newChar));
	}

	template <class Fty>
	constexpr String String::replaced_if(Fty f, const value_type newChar) const& requires std::predicate<Fty&, const value_type&>
	{
		String result{ *this };
		result.replace_if(f, newChar);
		return result;
	}

	template <class Fty>
	constexpr String String::replaced_if(Fty f, const value_type newChar) && noexcept requires std::predicate<Fty&, const value_type&>
	{
		return std::move(replace_if(f, newChar));
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse_each
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr void String::reverse_each(Fty f) requires std::invocable<Fty&, value_type&>
	{
		auto it = m_string.rbegin();
		const auto itEnd = m_string.rend();

		while (it != itEnd)
		{
			f(*it++);
		}
	}

	template <class Fty>
	constexpr void String::reverse_each(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		auto it = m_string.rbegin();
		const auto itEnd = m_string.rend();

		while (it != itEnd)
		{
			f(*it++);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	sort_by / sorted_by
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	String& String::sort_by(Fty f) & noexcept requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		std::sort(m_string.begin(), m_string.end(), detail::PassFunction(std::forward<Fty>(f)));
		return *this;
	}

	template <class Fty>
	String String::sort_by(Fty f) && noexcept requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		std::sort(m_string.begin(), m_string.end(), detail::PassFunction(std::forward<Fty>(f)));
		return std::move(*this);
	}

	template <class Fty>
	String String::sorted_by(Fty f) const& requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		String result{ *this };
		result.sort_by(std::forward<Fty>(f));
		return result;
	}

	template <class Fty>
	String String::sorted_by(Fty f) && noexcept requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		return std::move(sort_by(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	take_while
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	String String::take_while(Fty f) const& requires std::predicate<Fty&, const value_type&>
	{
		return String(m_string.begin(), std::find_if_not(m_string.begin(), m_string.end(), detail::PassFunction(std::forward<Fty>(f))));
	}

	template <class Fty>
	String String::take_while(Fty f) && noexcept requires std::predicate<Fty&, const value_type&>
	{
		const auto it = std::find_if_not(m_string.begin(), m_string.end(), f);
		m_string.erase(it, m_string.end());
		return std::move(*this);
	}
}
