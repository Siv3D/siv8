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

		[[noreturn]]
		void ThrowGridPopBackRowOutOfRange();

		[[noreturn]]
		void ThrowGridPopBackColumnOutOfRange();

		[[noreturn]]
		void ThrowGridInsertRowOutOfRange();

		[[noreturn]]
		void ThrowGridInsertRowsOutOfRange();

		[[noreturn]]
		void ThrowGridInsertColumnOutOfRange();
	
		[[noreturn]]
		void ThrowGridInsertColumnsOutOfRange();

		[[noreturn]]
		void ThrowGridRemoveRowOutOfRange();

		[[noreturn]]
		void ThrowGridRemoveRowsOutOfRange();

		[[noreturn]]
		void ThrowGridRemoveColumnOutOfRange();

		[[noreturn]]
		void ThrowGridRemoveColumnsOutOfRange();

		[[noreturn]]
		void ThrowGridSwapColumnsOutOfRange();

		[[noreturn]]
		void ThrowGridSwapRowsOutOfRange();
		
		[[noreturn]]
		void ThrowGridValuesAtOutOfRange();
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

# if defined(__cpp_lib_ranges_stride)

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

# endif

	////////////////////////////////////////////////////////////////
	//
	//	push_back_row
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::push_back_row(const value_type& value)
	{
		m_container.insert(m_container.end(), m_size.x, value);
		++m_size.y;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back_row
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::pop_back_row()
	{
		if (m_size.y == 0)
		{
			detail::ThrowGridPopBackRowOutOfRange();
		}
		
		m_container.resize(m_container.size() - m_size.x);
		--m_size.y;
	}

	////////////////////////////////////////////////////////////////
	//
	//	push_back_column
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::push_back_column(const value_type& value)
	{
		if (m_size.y == 0)
		{
			++m_size.x;
			return;
		}

		const int32 oldWidth = m_size.x;
		const int32 newWidth = (m_size.x + 1);

		m_container.resize(m_size.y * newWidth);

		for (int32 row = (m_size.y - 1); 0 <= row; --row)
		{
			const auto srcBegin = (m_container.begin() + row * oldWidth);
			const auto srcEnd = (srcBegin + oldWidth);
			const auto dstEnd = (m_container.begin() + row * newWidth + oldWidth);
			
			std::move_backward(srcBegin, srcEnd, dstEnd);
			m_container[row * newWidth + oldWidth] = value;
		}
		
		m_size.x = newWidth;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back_column
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::pop_back_column()
	{
		if (m_size.x == 0)
		{
			detail::ThrowGridPopBackColumnOutOfRange();
		}

		if (m_size.y == 0)
		{
			--m_size.x;
			return;
		}
		
		const int32 newWidth = (m_size.x - 1);

		for (int32 row = 0; row < m_size.y; ++row)
		{
			const auto srcBegin = (m_container.begin() + row * m_size.x);
			const auto srcEnd = (srcBegin + newWidth);
			const auto dstBegin = (m_container.begin() + row * newWidth);

			std::move(srcBegin, srcEnd, dstBegin);
		}

		m_container.resize(m_size.y * newWidth);
		m_size.x = newWidth;
	}

	////////////////////////////////////////////////////////////////
	//
	//	insert_row
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::insert_row(const size_type y, const value_type& value)
	{
		if (static_cast<size_type>(m_size.y) < y)
		{
			detail::ThrowGridInsertRowOutOfRange();
		}

		const auto it = (m_container.begin() + (y * m_size.x));

		m_container.insert(it, m_size.x, value);

		++m_size.y;
	}

	////////////////////////////////////////////////////////////////
	//
	//	insert_rows
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::insert_rows(const size_type y, const size_type count, const value_type& value)
	{
		if (static_cast<size_type>(m_size.y) < y)
		{
			detail::ThrowGridInsertRowsOutOfRange();
		}

		const auto it = (m_container.begin() + (y * m_size.x));

		m_container.insert(it, (m_size.x * count), value);
		
		m_size.y += count;
	}

	////////////////////////////////////////////////////////////////
	//
	//	insert_column
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::insert_column(const size_type x, const value_type& value)
	{
		if (static_cast<size_type>(m_size.x) < x)
		{
			detail::ThrowGridInsertColumnOutOfRange();
		}
		
		if (m_size.y == 0)
		{
			++m_size.x;
			return;
		}
		
		const int32 oldWidth = m_size.x;
		const int32 newWidth = oldWidth + 1;

		m_container.resize(m_size.y * newWidth);

		for (int32 row = (m_size.y - 1); 0 <= row; --row)
		{
			const int32 oldRowStart = (row * oldWidth);
			const int32 newRowStart = (row * newWidth);

			std::move_backward((m_container.begin() + oldRowStart + x),
				(m_container.begin() + oldRowStart + oldWidth),
				(m_container.begin() + newRowStart + newWidth));

			m_container[newRowStart + x] = value;

			if (newRowStart != oldRowStart)
			{
				std::copy(m_container.begin() + oldRowStart,
					m_container.begin() + oldRowStart + x,
					m_container.begin() + newRowStart);
			}
		}

		m_size.x = newWidth;
	}

	////////////////////////////////////////////////////////////////
	//
	//	insert_columns
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::insert_columns(const size_type x, const size_type n, const value_type& value)
	{
		if (static_cast<size_type>(m_size.x) < x)
		{
			detail::ThrowGridInsertColumnsOutOfRange();
		}

		if (n == 0)
		{
			return;
		}

		if (m_size.y == 0)
		{
			m_size.x += static_cast<int32>(n);
			return;
		}

		const int32 oldWidth = m_size.x;
		const int32 newWidth = (oldWidth + static_cast<int32>(n));

		m_container.resize(m_size.y * newWidth);

		for (int32 row = (m_size.y - 1); 0 <= row; --row)
		{
			const int32 oldRowStart = (row * oldWidth);
			const int32 newRowStart = (row * newWidth);

			std::move_backward(
				(m_container.begin() + oldRowStart + x),
				(m_container.begin() + oldRowStart + oldWidth),
				(m_container.begin() + newRowStart + newWidth)
			);

			std::fill(
				(m_container.begin() + newRowStart + x),
				(m_container.begin() + newRowStart + x + n),
				value
			);

			if (newRowStart != oldRowStart)
			{
				std::move_backward(
					(m_container.begin() + oldRowStart),
					(m_container.begin() + oldRowStart + x),
					(m_container.begin() + newRowStart + x)
				);
			}
		}

		m_size.x = newWidth;
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove_row
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::remove_row(const size_type y)
	{
		if (static_cast<size_type>(m_size.y) <= y)
		{
			detail::ThrowGridRemoveRowOutOfRange();
		}

		const auto it = (m_container.begin() + (y * m_size.x));
		
		m_container.erase(it, (it + m_size.x));
		
		--m_size.y;
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove_rows
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::remove_rows(const size_type y, const size_type count)
	{
		if (static_cast<size_type>(m_size.y) < (y + count))
		{
			detail::ThrowGridRemoveRowsOutOfRange();
		}

		const auto it = (m_container.begin() + (y * m_size.x));

		m_container.erase(it, (it + m_size.x * count));

		m_size.y -= static_cast<int32>(count);
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove_column
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::remove_column(const size_type x)
	{
		if (static_cast<size_type>(m_size.x) <= x)
		{
			detail::ThrowGridRemoveColumnOutOfRange();
		}

		if (m_size.y == 0)
		{
			--m_size.x;
			return;
		}

		const int32 oldWidth = m_size.x;
		const int32 newWidth = (oldWidth - 1);

		for (int32 row = 0; row < m_size.y; ++row)
		{
			const int32 srcRowStart = (row * oldWidth);
			const int32 destRowStart = (row * newWidth);

			if (0 < x)
			{
				std::move((m_container.begin() + srcRowStart),
					(m_container.begin() + srcRowStart + x),
					(m_container.begin() + destRowStart));
			}

			if (x < (oldWidth - 1))
			{
				std::move((m_container.begin() + srcRowStart + x + 1),
					(m_container.begin() + srcRowStart + oldWidth),
					(m_container.begin() + destRowStart + x));
			}
		}

		m_container.resize(m_size.y * newWidth);
		m_size.x = newWidth;
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove_columns
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::remove_columns(const size_type x, const size_type n)
	{
		if (static_cast<size_type>(m_size.x) < (x + n))
		{
			detail::ThrowGridRemoveColumnsOutOfRange();
		}

		if (n == 0)
		{
			return;
		}

		if (m_size.y == 0)
		{
			m_size.x -= static_cast<int32>(n);
			return;
		}

		const int32 oldWidth = m_size.x;
		const int32 newWidth = (oldWidth - static_cast<int32>(n));

		if (newWidth == 0)
		{
			m_container.clear();
			m_size.x = 0;
			return;
		}

		for (int32 row = 0; row < m_size.y; ++row)
		{
			const int32 srcRowStart = (row * oldWidth);
			const int32 destRowStart = (row * newWidth);
			
			if (0 < x)
			{
				std::move((m_container.begin() + srcRowStart),
					(m_container.begin() + srcRowStart + x),
					(m_container.begin() + destRowStart));
			}
			
			if (x < (oldWidth - n))
			{
				std::move((m_container.begin() + srcRowStart + x + n),
					(m_container.begin() + srcRowStart + oldWidth),
					(m_container.begin() + destRowStart + x));
			}
		}

		m_container.resize(m_size.y * newWidth);
		m_size.x = newWidth;
	}

	////////////////////////////////////////////////////////////////
	//
	//	resizeWidth
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::resizeWidth(const size_type w)
	{
		resizeWidth(w, value_type{});
	}

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::resizeWidth(const size_type w, const value_type& value)
	{
		const int32 newWidth = static_cast<int32>(w);

		if (m_size.x == newWidth)
		{
			return;
		}

		if (m_size.x == 0)
		{
			m_container.clear();
			m_size.x = newWidth;
			return;
		}

		if (newWidth < m_size.x) // shrink
		{
			remove_columns(newWidth, (m_size.x - newWidth));
		}
		else if (m_size.x < newWidth) // expand
		{
			insert_columns(m_size.x, (newWidth - m_size.x), value);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	resizeHeight
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::resizeHeight(const size_type h)
	{
		resizeHeight(h, value_type{});
	}

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::resizeHeight(const size_type h, const value_type& value)
	{
		m_container.resize((m_size.x * h), value);
		m_size.y = static_cast<int32>(h);
	}

	////////////////////////////////////////////////////////////////
	//
	//	resize
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::resize(const size_type w, const size_type h)
	{
		resize(w, h, value_type{});
	}

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::resize(const Size size)
	{
		resize(size.x, size.y, value_type{});
	}

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::resize(const size_type w, const size_type h, const value_type& value)
	{
		const int32 oldHeight = m_size.y;
		const int32 newWidth = static_cast<int32>(w);
		const int32 newHeight = static_cast<int32>(h);

		if ((newWidth == 0) || (newHeight == 0))
		{
			m_container.clear();
			m_size.x = newWidth;
			m_size.y = newHeight;
			return;
		}

		if (m_container.isEmpty())
		{
			m_container.resize((newWidth * newHeight), value);
			m_size.x = newWidth;
			m_size.y = newHeight;
			return;
		}

		if (newHeight < oldHeight)
		{
			resizeHeight(newHeight, value);
			resizeWidth(newWidth, value);
		}
		else
		{
			m_container.reserve(newWidth * newHeight);
			resizeWidth(newWidth, value);
			resizeHeight(newHeight, value);
		}
	}

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::resize(const Size size, const value_type& value)
	{
		resize(size.x, size.y, value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate90
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	void Grid<Type, Allocator>::rotate90()
	{
		constexpr size_t BlockSize = 8;
		const size_t oldWidth = m_size.x;
		const size_t oldHeight = m_size.y;

		if (oldWidth == oldHeight)
		{
			value_type* const pData = data();
			const size_t halfHeight = (oldHeight / 2);
			const size_t halfWidth = (halfHeight + oldHeight % 2);

			for (size_t b = 0; b < halfWidth; b += BlockSize)
			{
				for (size_t y = 0; y < halfHeight; ++y)
				{
					value_type* p1 = (pData + y * oldWidth + b);
					value_type* p2 = (pData + (oldHeight - b - 1) * oldWidth + y);
					value_type* p3 = (pData + (oldHeight - y) * oldWidth - b - 1);
					value_type* p4 = (pData + (b + 1) * oldWidth - y - 1);
					const size_t w = Min((halfWidth - b), BlockSize);

					for (size_t x = 0; x < w; ++x)
					{
						value_type tmp = std::move(*p1);
						*p1 = std::move(*p2);
						*p2 = std::move(*p3);
						*p3 = std::move(*p4);
						*p4 = std::move(tmp);
						++p1;
						p2 -= oldWidth;
						--p3;
						p4 += oldWidth;
					}
				}
			}
		}
		else
		{
			Grid newGrid(oldHeight, oldWidth);

			value_type* const pSrcBase = m_container.data();
			value_type* const pDstBase = newGrid.data();

			for (size_t b = 0; b < oldWidth; b += BlockSize)
			{
				value_type* const pDstLine = (pDstBase + b * oldHeight);
				const size_t w = Min((oldWidth - b), BlockSize);

				for (size_t y = 0; y < oldHeight; ++y)
				{
					value_type* pSrc = (pSrcBase + (y * oldWidth + b));
					value_type* pDst = (pDstLine + (oldHeight - y - 1));

					for (size_t x = 0; x < w; ++x)
					{
						*pDst = std::move(*pSrc++);
						pDst += oldHeight;
					}
				}
			}
			
			swap(newGrid);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate180
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::rotate180() noexcept
	{
		std::reverse(m_container.begin(), m_container.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate270
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	void Grid<Type, Allocator>::rotate270()
	{
		constexpr size_t BlockSize = 8;
		const size_t oldWidth = m_size.x;
		const size_t oldHeight = m_size.y;

		if (oldWidth == oldHeight)
		{
			value_type* const pData = data();
			const size_t halfHeight = (oldHeight / 2);
			const size_t halfWidth = (halfHeight + oldHeight % 2);

			for (size_t b = 0; b < halfWidth; b += BlockSize)
			{
				for (size_t y = 0; y < halfHeight; ++y)
				{
					value_type* p1 = (pData + y * oldWidth + b);
					value_type* p2 = (pData + (oldHeight - b - 1) * oldWidth + y);
					value_type* p3 = (pData + (oldHeight - y) * oldWidth - b - 1);
					value_type* p4 = (pData + (b + 1) * oldWidth - y - 1);
					const size_t w = Min((halfWidth - b), BlockSize);

					for (size_t x = 0; x < w; ++x)
					{
						value_type tmp = std::move(*p1);
						*p1 = std::move(*p4);
						*p4 = std::move(*p3);
						*p3 = std::move(*p2);
						*p2 = std::move(tmp);
						++p1;
						p2 -= oldWidth;
						--p3;
						p4 += oldWidth;
					}
				}
			}
		}
		else
		{
			Grid newGrid(oldHeight, oldWidth);

			value_type* const pSrcBase = m_container.data();
			value_type* const pDstBase = newGrid.data();

			for (size_t b = 0; b < oldWidth; b += BlockSize)
			{
				value_type* const pDstLine = (pDstBase + (oldWidth - b - 1) * oldHeight);
				const size_t w = Min((oldWidth - b), BlockSize);

				for (size_t y = 0; y < oldHeight; ++y)
				{
					value_type* pSrc = (pSrcBase + (y * oldWidth + b));
					value_type* pDst = (pDstLine + y);

					for (size_t x = 0; x < w; ++x)
					{
						*pDst = std::move(*pSrc++);
						pDst -= oldHeight;
					}
				}
			}

			swap(newGrid);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	mirror
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::mirror() noexcept
	{
		auto it = m_container.begin();

		for (int32 y = 0; y < m_size.y; ++y)
		{
			std::reverse(it, (it + m_size.x));
			it += m_size.x;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	flip
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr void Grid<Type, Allocator>::flip() noexcept
	{
		for (int32 y = 0; y < (m_size.y / 2); ++y)
		{
			const auto rowTop = (m_container.begin() + (y * m_size.x));
			const auto rowBottom = (m_container.begin() + ((m_size.y - 1 - y) * m_size.x));

			std::swap_ranges(rowTop, (rowTop + m_size.x), rowBottom);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	transpose
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	void Grid<Type, Allocator>::transpose()
	{
		constexpr size_t BlockSize = 8;

		if (m_size.x == m_size.y)
		{
			value_type* const pData = data();

			for (size_t i = 0; i < m_size.x; i += BlockSize)
			{
				const size_t iMax = Min<size_t>((i + BlockSize), m_size.x);

				for (size_t y = i; y < iMax; ++y)
				{
					for (size_t x = i; x < y; ++x)
					{
						value_type* p1 = (pData + y * m_size.x + x);
						value_type* p2 = (pData + x * m_size.x + y);

						value_type tmp = std::move(*p1);
						*p1 = std::move(*p2);
						*p2 = std::move(tmp);
					}
				}

				for (size_t k = i + BlockSize; k < m_size.x; k += BlockSize)
				{
					const size_t kMax = Min<size_t>((k + BlockSize), m_size.x);

					for (size_t y = i; y < iMax; ++y)
					{
						for (size_t x = k; x < kMax; ++x)
						{
							value_type* p1 = (pData + y * m_size.x + x);
							value_type* p2 = (pData + x * m_size.x + y);

							value_type tmp = std::move(*p1);
							*p1 = std::move(*p2);
							*p2 = std::move(tmp);
						}
					}
				}
			}
		}
		else
		{
			Grid newGrid(m_size.y, m_size.x);

			value_type* const pSrcBase = data();
			value_type* const pDstBase = newGrid.data();

			for (size_t b = 0; b < m_size.x; b += BlockSize)
			{
				value_type* const pDstLine = (pDstBase + b * m_size.y);
				const size_t w = Min<size_t>((m_size.x - b), BlockSize);

				for (size_t y = 0; y < m_size.y; ++y)
				{
					value_type* pSrc = (pSrcBase + (y * m_size.x + b));
					value_type* pDst = (pDstLine + y);

					for (size_t x = 0; x < w; ++x)
					{
						*pDst = std::move(*pSrc++);
						pDst += m_size.y;
					}
				}
			}

			swap(newGrid);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	all
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr bool Grid<Type, Allocator>::all(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return m_container.all(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	any
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr bool Grid<Type, Allocator>::any(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return m_container.any(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr bool Grid<Type, Allocator>::contains(const value_type& value) const
	{
		return m_container.contains(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains_if
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr bool Grid<Type, Allocator>::contains_if(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return m_container.contains_if(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	count
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr isize Grid<Type, Allocator>::count(const value_type& value) const
	{
		return m_container.count(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	count_if
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr isize Grid<Type, Allocator>::count_if(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return m_container.count_if(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	each
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr void Grid<Type, Allocator>::each(Fty f) requires std::invocable<Fty&, value_type&>
	{
		m_container.each(std::forward<Fty>(f));
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr void Grid<Type, Allocator>::each(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		m_container.each(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	fetch
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class U>
	constexpr typename Grid<Type, Allocator>::value_type Grid<Type, Allocator>::fetch(const size_type y, const size_type x, U&& defaultValue) const noexcept(std::is_nothrow_constructible_v<value_type, U>) requires std::constructible_from<value_type, U>
	{
		if (not indexInBounds(y, x))
		{
			return value_type(std::forward<U>(defaultValue));
		}

		return m_container[y * m_size.x + x];
	}

	template <class Type, class Allocator>
	template <class U>
	constexpr typename Grid<Type, Allocator>::value_type Grid<Type, Allocator>::fetch(const Point pos, U&& defaultValue) const noexcept(std::is_nothrow_constructible_v<value_type, U>) requires std::constructible_from<value_type, U>
	{
		return fetch(pos.y, pos.x, std::forward<U>(defaultValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	fill
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::fill(const value_type& value)
	{
		m_container.fill(value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isSorted
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr bool Grid<Type, Allocator>::isSorted() const requires Concept::LessThanComparable<value_type>
	{
		return m_container.isSorted();
	}

	////////////////////////////////////////////////////////////////
	//
	//	map
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr auto Grid<Type, Allocator>::map(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		using result_value_type = std::decay_t<std::invoke_result_t<Fty&, const value_type&>>;

		Grid<result_value_type> result(m_size);

		auto itDst = result.begin();

		for (const auto& value : m_container)
		{
			*itDst++ = f(value);
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	none
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr bool Grid<Type, Allocator>::none(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return m_container.none(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	replace, replaced
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::replace(const value_type& oldValue, const value_type& newValue)&
	{
		m_container.replace(oldValue, newValue);
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::replace(const value_type& oldValue, const value_type& newValue)&&
	{
		return std::move(replace(oldValue, newValue));
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::replaced(const value_type& oldValue, const value_type& newValue) const&
	{
		Grid result(*this);

		result.replace(oldValue, newValue);

		return result;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::replaced(const value_type& oldValue, const value_type& newValue)&&
	{
		return std::move(replace(oldValue, newValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	replace_if, replaced_if
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::replace_if(Fty f, const value_type& newValue)& requires std::predicate<Fty&, const value_type&>
	{
		m_container.replace_if(std::forward<Fty>(f), newValue);
		return *this;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::replace_if(Fty f, const value_type& newValue) && requires std::predicate<Fty&, const value_type&>
	{
		return std::move(replace_if(f, newValue));
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::replaced_if(Fty f, const value_type& newValue) const& requires std::predicate<Fty&, const value_type&>
	{
		Grid result(*this);

		result.replace_if(std::forward<Fty>(f), newValue);

		return result;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::replaced_if(Fty f, const value_type& newValue) && requires std::predicate<Fty&, const value_type&>
	{
		return std::move(replace_if(f, newValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse, reversed
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::reverse()&
	{
		m_container.reverse();
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::reverse()&&
	{
		return std::move(reverse());
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::reversed() const&
	{
		Grid result;
		result.m_container.assign(m_container.rbegin(), m_container.rend());
		result.m_size = m_size;

		return result;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::reversed()&&
	{
		return std::move(reverse());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate_columns, rotated_columns
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::rotate_columns(const size_type middle)&
	{
		auto it = m_container.begin();

		for (int32 y = 0; y < m_size.y; ++y)
		{
			std::rotate(it, (it + middle), (it + m_size.x));
			it += m_size.x;
		}

		return *this;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::rotate_columns(const size_type middle)&&
	{
		return std::move(rotate_columns(middle));
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::rotated_columns(const size_type middle) const&
	{
		Grid result(m_size);
		auto itSrc = m_container.begin();
		auto itDst = result.m_container.begin();

		for (int32 y = 0; y < m_size.y; ++y)
		{
			std::copy((itSrc + middle), (itSrc + m_size.x), itDst);
			itDst += (m_size.x - middle);
			std::copy(itSrc, (itSrc + middle), itDst);
			itDst += middle;
			itSrc += m_size.x;
		}

		return result;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::rotated_columns(const size_type middle)&&
	{
		return std::move(rotate_columns(middle));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate_rows, rotated_rows
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::rotate_rows(const size_type middle)&
	{
		const auto itMiddle = (m_container.begin() + (middle * m_size.x));

		std::rotate(m_container.begin(), itMiddle, m_container.end());
		
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::rotate_rows(const size_type middle)&&
	{
		return std::move(rotate_rows(middle));
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::rotated_rows(const size_type middle) const&
	{
		Grid result(m_size);

		const auto itMiddle = (m_container.begin() + (middle * m_size.x));
		std::copy(itMiddle, m_container.end(), result.m_container.begin());
		std::copy(m_container.begin(), itMiddle, (result.m_container.begin() + (m_size.y - middle) * m_size.x));

		return result;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::rotated_rows(const size_type middle)&&
	{
		return std::move(rotate_rows(middle));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rsort, rsorted
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::rsort()& requires Concept::LessThanComparable<value_type>
	{
		m_container.rsort();
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::rsort() && requires Concept::LessThanComparable<value_type>
	{
		return std::move(rsort());
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::rsorted() const& requires Concept::LessThanComparable<value_type>
	{
		Grid result(*this);
		result.rsort();
		return result;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::rsorted() && requires Concept::LessThanComparable<value_type>
	{
		return std::move(rsort());
	}

	////////////////////////////////////////////////////////////////
	//
	//	shuffle, shuffled
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::shuffle()&
	{
		Shuffle(m_container.begin(), m_container.end(), GetDefaultRNG());
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::shuffle()&&
	{
		return std::move(shuffle());
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::shuffled() const&
	{
		Grid result(*this);
		result.shuffle();
		return result;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::shuffled()&&
	{
		return std::move(shuffle());
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::shuffle(Concept::UniformRandomBitGenerator auto&& rbg)&
	{
		Shuffle(m_container.begin(), m_container.end(), std::forward<decltype(rbg)>(rbg));
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::shuffle(Concept::UniformRandomBitGenerator auto&& rbg)&&
	{
		return std::move(shuffle(std::forward<decltype(rbg)>(rbg)));
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::shuffled(Concept::UniformRandomBitGenerator auto&& rbg) const&
	{
		Grid result(*this);
		result.shuffle(std::forward<decltype(rbg)>(rbg));
		return result;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::shuffled(Concept::UniformRandomBitGenerator auto&& rbg)&&
	{
		return std::move(shuffle(std::forward<decltype(rbg)>(rbg)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	sort, sorted
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::sort()& requires Concept::LessThanComparable<value_type>
	{
		m_container.sort();
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::sort() && requires Concept::LessThanComparable<value_type>
	{
		return std::move(sort());
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::sorted() const& requires Concept::LessThanComparable<value_type>
	{
		Grid result(*this);
		result.sort();
		return result;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::sorted() && requires Concept::LessThanComparable<value_type>
	{
		return std::move(sort());
	}

	////////////////////////////////////////////////////////////////
	//
	//	sort_by, sorted_by
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::sort_by(Fty f)& requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		m_container.sort_by(std::forward<Fty>(f));
		return *this;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::sort_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		return std::move(sort_by(std::forward<Fty>(f)));
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::sorted_by(Fty f) const& requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		Grid result(*this);
		result.sort_by(std::forward<Fty>(f));
		return result;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::sorted_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		return std::move(sort_by(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	stable_sort, stable_sorted
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::stable_sort()& requires Concept::LessThanComparable<value_type>
	{
		m_container.stable_sort();
		return *this;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::stable_sort() && requires Concept::LessThanComparable<value_type>
	{
		return std::move(stable_sort());
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::stable_sorted() const& requires Concept::LessThanComparable<value_type>
	{
		Grid result(*this);
		result.stable_sort();
		return result;
	}

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::stable_sorted() && requires Concept::LessThanComparable<value_type>
	{
		return std::move(stable_sort());
	}

	////////////////////////////////////////////////////////////////
	//
	//	stable_sort_by, stable_sorted_by
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::stable_sort_by(Fty f)& requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		m_container.stable_sort_by(std::forward<Fty>(f));
		return *this;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::stable_sort_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		return std::move(stable_sort_by(std::forward<Fty>(f)));
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::stable_sorted_by(Fty f) const& requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		Grid result(*this);
		result.stable_sort_by(std::forward<Fty>(f));
		return result;
	}

	template <class Type, class Allocator>
	template <class Fty>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::stable_sorted_by(Fty f) && requires std::predicate<Fty&, const value_type&, const value_type&>
	{
		return std::move(stable_sort_by(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	sum
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr auto Grid<Type, Allocator>::sum() const requires (Concept::Addable<value_type> || Concept::AddAssignable<value_type>)
	{
		return m_container.sum();
	}

	////////////////////////////////////////////////////////////////
	//
	//	sumF
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr auto Grid<Type, Allocator>::sumF() const requires Concept::FloatingPoint<value_type>
	{
		return m_container.sumF();
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap_columns
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::swap_columns(const size_type a, const size_type b)
	{
		if ((static_cast<size_type>(m_size.x) <= a) || (static_cast<size_type>(m_size.x) <= b))
		{
			detail::ThrowGridSwapColumnsOutOfRange();
		}

		size_t aBase = a;
		size_t bBase = b;

		for (int32 y = 0; y < m_size.y; ++y)
		{
			using std::swap;
			swap(m_container[aBase], m_container[bBase]);
			aBase += m_size.x;
			bBase += m_size.x;
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap_rows
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::swap_rows(const size_type a, const size_type b)
	{
		if ((static_cast<size_type>(m_size.y) <= a) || (static_cast<size_type>(m_size.y) <= b))
		{
			detail::ThrowGridSwapRowsOutOfRange();
		}

		auto itA = (m_container.begin() + (a * m_size.x));
		auto itB = (m_container.begin() + (b * m_size.x));

		std::swap_ranges(itA, (itA + m_size.x), itB);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	values_at
	//
	////////////////////////////////////////////////////////////////
	
	template <class Type, class Allocator>
	constexpr Array<Type> Grid<Type, Allocator>::values_at(const std::initializer_list<Point> indices) const
	{
		Array<value_type> result;
		result.reserve(indices.size());

		for (auto index : indices)
		{
			if (index < m_container.size())
			{
				result.push_back(m_container[index.y * m_size.x + index.x]);
			}
			else
			{
				detail::ThrowGridValuesAtOutOfRange();
			}
		}

		return result;
	}
	
	////////////////////////////////////////////////////////////////
	//
	//	parallel_count_if
	//
	////////////////////////////////////////////////////////////////
	
	template <class Type, class Allocator>
	template <class Fty>
	isize Grid<Type, Allocator>::parallel_count_if(Fty f) const requires std::predicate<Fty&, const value_type&>
	{
		return m_container.parallel_count_if(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	parallel_each
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	void Grid<Type, Allocator>::parallel_each(Fty f) requires std::invocable<Fty&, value_type&>
	{
		m_container.parallel_each(std::forward<Fty>(f));
	}

	template <class Type, class Allocator>
	template <class Fty>
	void Grid<Type, Allocator>::parallel_each(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		m_container.parallel_each(std::forward<Fty>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	parallel_map
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	auto Grid<Type, Allocator>::parallel_map(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		return Grid(m_size, m_container.parallel_map(std::forward<Fty>(f)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator >>
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	template <class Fty>
	constexpr auto Grid<Type, Allocator>::operator >>(Fty f) const requires std::invocable<Fty&, const value_type&>
	{
		using result_value_type = std::decay_t<std::invoke_result_t<Fty&, const value_type&>>;

		if constexpr (std::is_same_v<result_value_type, void>)
		{
			each(std::forward<Fty>(f));
		}
		else
		{
			return map(std::forward<Fty>(f));
		}
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
