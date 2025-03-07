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
		[[nodiscard]]
		constexpr Size ValidGridSizeOrEmpty(const Size size) noexcept
		{
			if ((size.x < 0) || (size.y < 0))
			{
				return{ 0, 0 };
			}
			else
			{
				return size;
			}
		}

		[[noreturn]]
		void ThrowGridAtOutOfRange();
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(const size_type w, const size_type h)
		: m_size{ w, h }
		, m_container(w * h) {}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(const size_type w, const size_type h, const value_type& value)
		: m_size{ w, h } 
		, m_container((w * h), value) {}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(const Size size)
		: m_size{ detail::ValidGridSizeOrEmpty(size) }
		, m_container(m_size.x * m_size.y) {}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(const Size size, const value_type& value)
		: m_size{ detail::ValidGridSizeOrEmpty(size) }
		, m_container((m_size.x * m_size.y), value) {}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(const size_type w, const size_type h, const Array<value_type>& data)
		: m_size{ w, h }
		, m_container(data)
	{
		m_container.resize(w * h);
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(const size_type w, const size_type h, Array<value_type>&& data)
		: m_size{ w, h }
		, m_container(std::move(data))
	{
		m_container.resize(w * h);
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(Size size, const Array<value_type>& data)
		: m_size{ detail::ValidGridSizeOrEmpty(size) }
		, m_container(data)
	{
		m_container.resize(m_size.x * m_size.y);
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(Size size, Array<value_type>&& data)
		: m_size{ detail::ValidGridSizeOrEmpty(size) }
		, m_container(std::move(data))
	{
		m_container.resize(m_size.x * m_size.y);
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(const std::initializer_list<std::initializer_list<value_type>>& set)
		: Grid(std::max_element(set.begin(), set.end(),
			[](auto& lhs, auto& rhs) { return (lhs.size() < rhs.size()); })->size(), set.size())
	{
		auto dst = m_container.begin();

		for (const auto& a : set)
		{
			std::copy(a.begin(), a.end(), dst);
			dst += m_size.x;
		}
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(const size_type w, const size_type h, Arg::generator_<FunctionRef<value_type()>> generator)
		: Grid(Size{ w, h }, generator) {}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(const size_type w, const size_type h, Arg::generator_<FunctionRef<value_type(int32, int32)>> generator)
		: Grid(Size{ w, h }, generator) {}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(const size_type w, const size_type h, Arg::generator_<FunctionRef<value_type(Point)>> generator)
		: Grid(Size{ w, h }, generator) {}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(const Size size, Arg::generator_<FunctionRef<value_type()>> generator)
		: m_size{ detail::ValidGridSizeOrEmpty(size) }
		, m_container(m_size.x * m_size.y)
	{
		const auto& f = *generator;

		value_type* pDst = m_container.data();
		const value_type* const pDstEnd = (pDst + m_container.size());

		while (pDst != pDstEnd)
		{
			*pDst++ = f();
		}
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(const Size size, Arg::generator_<FunctionRef<value_type(int32, int32)>> generator)
		: m_size{ detail::ValidGridSizeOrEmpty(size) }
		, m_container(m_size.x * m_size.y)
	{
		const auto& f = *generator;

		value_type* pDst = m_container.data();

		for (int32 y = 0; y < m_size.y; ++y)
		{
			for (int32 x = 0; x < m_size.x; ++x)
			{
				*pDst++ = f(x, y);
			}
		}
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::Grid(const Size size, Arg::generator_<FunctionRef<value_type(Point)>> generator)
		: m_size{ detail::ValidGridSizeOrEmpty(size) }
		, m_container(m_size.x * m_size.y)
	{
		const auto& f = *generator;

		value_type* pDst = m_container.data();

		for (int32 y = 0; y < m_size.y; ++y)
		{
			for (int32 x = 0; x < m_size.x; ++x)
			{
				*pDst++ = f(Point{ x, y });
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	assign
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::assign(const size_type w, const size_type h, const value_type& value)
	{
		m_size = { w, h };
		m_container.assign((m_size.x * m_size.y), value);
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::assign(const Size size, const value_type& value)
	{
		m_size = detail::ValidGridSizeOrEmpty(size);
		m_container.assign((m_size.x * m_size.y), value);
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::assign(const std::initializer_list<std::initializer_list<value_type>>& set)
	{
		m_container.clear();

		resize(std::max_element(set.begin(), set.end(),
			[](auto& lhs, auto& rhs) { return lhs.size() < rhs.size(); })->size(), set.size());

		auto dst = m_container.begin();

		for (const auto& a : set)
		{
			std::copy(a.begin(), a.end(), dst);
			dst += m_size.x;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	get_allocator
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::allocator_type Grid<Type, Allocator>::get_allocator() const noexcept
	{
		return m_container.get_allocator();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getContainer
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr const typename Grid<Type, Allocator>::container_type& Grid<Type, Allocator>::getContainer() const& noexcept
	{
		return m_container;
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::container_type Grid<Type, Allocator>::getContainer() && noexcept
	{
		return std::move(m_container);
	}

	////////////////////////////////////////////////////////////////
	//
	//	at
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::reference Grid<Type, Allocator>::at(const size_type y, const size_type x)&
	{
		if (not indexInBounds(y, x))
		{
			detail::ThrowGridAtOutOfRange();
		}

		return *(m_container.data() + (y * m_size.x + x));
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_reference Grid<Type, Allocator>::at(const size_type y, const size_type x) const&
	{
		if (not indexInBounds(y, x))
		{
			detail::ThrowGridAtOutOfRange();
		}

		return *(m_container.data() + (y * m_size.x + x));
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::value_type Grid<Type, Allocator>::at(const size_type y, const size_type x)&&
	{
		if (not indexInBounds(y, x))
		{
			detail::ThrowGridAtOutOfRange();
		}

		return std::move(*(m_container.data() + (y * m_size.x + x)));
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::reference Grid<Type, Allocator>::at(const Point pos)&
	{
		if (not indexInBounds(pos))
		{
			detail::ThrowGridAtOutOfRange();
		}

		return *(m_container.data() + (pos.y * m_size.x + pos.x));
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_reference Grid<Type, Allocator>::at(const Point pos) const&
	{
		if (not indexInBounds(pos))
		{
			detail::ThrowGridAtOutOfRange();
		}

		return *(m_container.data() + (pos.y * m_size.x + pos.x));
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::value_type Grid<Type, Allocator>::at(const Point pos)&&
	{
		if (not indexInBounds(pos))
		{
			detail::ThrowGridAtOutOfRange();
		}

		return std::move(*(m_container.data() + (pos.y * m_size.x + pos.x)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::pointer Grid<Type, Allocator>::operator [](const size_type y)
	{
		assert(y < static_cast<size_type>(m_size.y));
		return (m_container.data() + (y * m_size.x));
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_pointer Grid<Type, Allocator>::operator [](const size_type y) const
	{
		assert(y < static_cast<size_type>(m_size.y));
		return (m_container.data() + (y * m_size.x));
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::reference Grid<Type, Allocator>::operator [](const Point pos)&
	{
		return *(m_container.data() + (pos.y * m_size.x + pos.x));
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_reference Grid<Type, Allocator>::operator [](const Point pos) const&
	{
		assert(indexInBounds(pos));
		return *(m_container.data() + (pos.y * m_size.x + pos.x));
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::value_type Grid<Type, Allocator>::operator [](const Point pos)&&
	{
		assert(indexInBounds(pos));
		return *(m_container.data() + (pos.y * m_size.x + pos.x));
	}

# ifdef __cpp_multidimensional_subscript

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::reference Grid<Type, Allocator>::operator [](const size_type y, const size_type x)&
	{
		assert(indexInBounds(y, x));
		return *(m_container.data() + (y * m_size.x + x));
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_reference Grid<Type, Allocator>::operator [](const size_type y, const size_type x) const&
	{
		assert(indexInBounds(y, x));
		return *(m_container.data() + (y * m_size.x + x));
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::value_type Grid<Type, Allocator>::operator [](const size_type y, const size_type x)&&
	{
		assert(indexInBounds(y, x));
		return *(m_container.data() + (y * m_size.x + x));
	}

# endif

	////////////////////////////////////////////////////////////////
	//
	//	front
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::reference Grid<Type, Allocator>::front() & noexcept
	{
		return m_container.front();
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_reference Grid<Type, Allocator>::front() const& noexcept
	{
		return m_container.front();
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::value_type Grid<Type, Allocator>::front() && noexcept
	{
		return std::move(m_container.front());
	}

	////////////////////////////////////////////////////////////////
	//
	//	back
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::reference Grid<Type, Allocator>::back() & noexcept
	{
		return m_container.back();
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_reference Grid<Type, Allocator>::back() const& noexcept
	{
		return m_container.back();
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::value_type Grid<Type, Allocator>::back() && noexcept
	{
		return std::move(m_container.back());
	}

	////////////////////////////////////////////////////////////////
	//
	//	indexInBounds
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr bool Grid<Type, Allocator>::indexInBounds(const size_type y, const size_type x) const noexcept
	{
		return ((y < static_cast<size_type>(m_size.y)) && (x < static_cast<size_type>(m_size.x)));
	}

	template <class Type, class Allocator>
	constexpr bool Grid<Type, Allocator>::indexInBounds(const Point pos) const noexcept
	{
		return (InRange(pos.x, 0, (m_size.x - 1)) && InRange(pos.y, 0, (m_size.y - 1)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	data
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::pointer Grid<Type, Allocator>::data() noexcept
	{
		return m_container.data();
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_pointer Grid<Type, Allocator>::data() const noexcept
	{
		return m_container.data();
	}

	////////////////////////////////////////////////////////////////
	//
	//	begin, end
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::iterator Grid<Type, Allocator>::begin() noexcept
	{
		return m_container.begin();
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::iterator Grid<Type, Allocator>::end() noexcept
	{
		return m_container.end();
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_iterator Grid<Type, Allocator>::begin() const noexcept
	{
		return m_container.begin();
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_iterator Grid<Type, Allocator>::end() const noexcept
	{
		return m_container.end();
	}

	////////////////////////////////////////////////////////////////
	//
	//	cbegin, cend
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_iterator Grid<Type, Allocator>::cbegin() const noexcept
	{
		return m_container.cbegin();
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_iterator Grid<Type, Allocator>::cend() const noexcept
	{
		return m_container.cend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	rbegin, rend
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::reverse_iterator Grid<Type, Allocator>::rbegin() noexcept
	{
		return m_container.rbegin();
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::reverse_iterator Grid<Type, Allocator>::rend() noexcept
	{
		return m_container.rend();
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_reverse_iterator Grid<Type, Allocator>::rbegin() const noexcept
	{
		return m_container.rbegin();
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_reverse_iterator Grid<Type, Allocator>::rend() const noexcept
	{
		return m_container.rend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	crbegin, crend
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_reverse_iterator Grid<Type, Allocator>::crbegin() const noexcept
	{
		return m_container.crbegin();
	}

	template <class Type, class Allocator>
	constexpr typename Grid<Type, Allocator>::const_reverse_iterator Grid<Type, Allocator>::crend() const noexcept
	{
		return m_container.crend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	empty
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr bool Grid<Type, Allocator>::empty() const noexcept
	{
		return m_container.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr bool Grid<Type, Allocator>::isEmpty() const noexcept
	{
		return m_container.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>::operator bool() const noexcept
	{
		return (not m_container.empty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	width, height
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr int32 Grid<Type, Allocator>::width() const noexcept
	{
		return m_size.x;
	}

	template <class Type, class Allocator>
	constexpr int32 Grid<Type, Allocator>::height() const noexcept
	{
		return m_size.y;
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Size Grid<Type, Allocator>::size() const noexcept
	{
		return m_size;
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_elements
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr size_t Grid<Type, Allocator>::num_elements() const noexcept
	{
		return m_container.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	size_bytes
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr size_t Grid<Type, Allocator>::size_bytes() const noexcept requires (Concept::TriviallyCopyable<value_type>)
	{
		return (m_container.size() * sizeof(value_type));
	}

	////////////////////////////////////////////////////////////////
	//
	//	reserve
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::reserve(const size_type w, const size_type h)
	{
		m_container.reserve(w * h);
	}

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::reserve(const Size size)
	{
		const Size newSize = detail::ValidGridSizeOrEmpty(size);
		m_container.reserve(newSize.x * newSize.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	capacity
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr size_t Grid<Type, Allocator>::capacity() const noexcept
	{
		return m_container.capacity();
	}

	////////////////////////////////////////////////////////////////
	//
	//	shrink_to_fit
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::shrink_to_fit()
	{
		m_container.shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::clear() noexcept
	{
		m_size.clear();
		m_container.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::release()
	{
		clear();
		shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::swap(Grid& other) noexcept
	{
		std::ranges::swap(m_size, other.m_size);
		m_container.swap(other.m_container);
	}




	////////////////////////////////////////////////////////////////
	//
	//	row
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr auto Grid<Type, Allocator>::row(const size_type y) noexcept
	{
		assert(y < static_cast<size_type>(m_size.y));
		return std::span((m_container.data() + (y * m_size.x)), m_size.x);
	}

	template <class Type, class Allocator>
	constexpr auto Grid<Type, Allocator>::row(const size_type y) const noexcept
	{
		assert(y < static_cast<size_type>(m_size.y));
		return std::span((m_container.data() + (y * m_size.x)), m_size.x);
	}

	////////////////////////////////////////////////////////////////
	//
	//	column
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr auto Grid<Type, Allocator>::column(const size_type x) noexcept
	{
		assert(x < static_cast<size_type>(m_size.x));
		return (m_container | std::views::drop(x) | std::views::stride(m_size.x));
	}

	template <class Type, class Allocator>
	constexpr auto Grid<Type, Allocator>::column(const size_type x) const noexcept
	{
		assert(x < static_cast<size_type>(m_size.x));
		return (m_container | std::views::drop(x) | std::views::stride(m_size.x));
	}









	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	void Formatter(FormatData& formatData, const Grid<Type, Allocator>& v)
	{
		formatData.string.push_back(U'[');

		bool isFirst = true;

		for (int32 y = 0; y < v.height(); ++y)
		{
			if (isFirst)
			{
				isFirst = false;
			}
			else
			{
				formatData.string.append(U",\n ");
			}

			Formatter(formatData, std::span<const Type>(v[y], v[y] + v.width()));
		}

		formatData.string.push_back(U']');
	}



}
