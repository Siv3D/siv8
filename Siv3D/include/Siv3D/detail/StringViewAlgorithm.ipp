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
	////////////////////////////////////////////////////////////////
	//
	//	all
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr bool StringView::all(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return std::all_of(m_view.begin(), m_view.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	any
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr bool StringView::any(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return std::any_of(m_view.begin(), m_view.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr bool StringView::contains_if(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return any(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	count
	//
	////////////////////////////////////////////////////////////////

	constexpr int64 StringView::count(const value_type ch) const noexcept
	{
		return std::count(m_view.begin(), m_view.end(), ch);
	}

	////////////////////////////////////////////////////////////////
	//
	//	count_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr int64 StringView::count_if(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return std::count_if(m_view.begin(), m_view.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	each
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr void StringView::each(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		std::for_each(m_view.begin(), m_view.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	each_index
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr void StringView::each_index(Fty f) const requires std::invocable<Fty&, size_t, const value_type&>
	{
		for (size_t i = 0; auto ch : m_view)
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
	constexpr void StringView::each_sindex(Fty f) const requires std::invocable<Fty&, isize, const value_type&>
	{
		for (isize i = 0; auto ch : m_view)
		{
			f(i++, ch);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	fetch
	//
	////////////////////////////////////////////////////////////////

	constexpr StringView::value_type StringView::fetch(const size_type index, const value_type defaultValue) const noexcept
	{
		if (m_view.size() <= index)
		{
			return defaultValue;
		}

		return m_view[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	filter
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr String StringView::filter(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		String result;

		for (const auto ch : m_view)
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

	constexpr bool StringView::isSorted() const noexcept
	{
		return std::is_sorted(m_view.begin(), m_view.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	map
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr auto StringView::map(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		using result_value_type = std::decay_t<std::invoke_result_t<Fty&, const value_type&>>;

		Array<result_value_type, std::allocator<result_value_type>> result;

		result.reserve(m_view.size());

		for (const auto ch : m_view)
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
	constexpr bool StringView::none(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return std::none_of(m_view.begin(), m_view.end(), detail::PassFunction(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove_if / removed_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr String StringView::removed_if(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		String result;

		for (const auto ch : m_view)
		{
			if (not f(ch))
			{
				result.push_back(ch);
			}
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	replaced_if
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr String StringView::replaced_if(Fty f, const value_type newChar) const requires std::predicate<Fty&, const value_type&>
	{
		String result{ m_view };

		for (auto& c : result)
		{
			if (f(c))
			{
				c = newChar;
			}
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse_each
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	constexpr void StringView::reverse_each(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		auto it = m_view.rbegin();
		const auto itEnd = m_view.rend();

		while (it != itEnd)
		{
			f(*it++);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	sort_by, sorted_by
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	String StringView::sorted_by(Fty f) const requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		String result{ m_view };
		result.sort_by(std::forward<Fty>(f));
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	take_while
	//
	////////////////////////////////////////////////////////////////

	template <class Fty>
	String StringView::take_while(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return String(m_view.begin(), std::find_if_not(m_view.begin(), m_view.end(), detail::PassFunction(std::forward<Fty>(f))));
	}
}
