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
# ifndef __cpp_lib_ranges_cartesian_product

	namespace detail
	{
		class Step2D
		{
		public:

			class Iterator
			{
			public:

				[[nodiscard]]
				constexpr Iterator() noexcept
					: m_countX{ 0 }
					, m_startX{ 0 }
					, m_step_counter{ 0, 0 }
					, m_counter{ 0, 0 }
					, m_stride{ 0, 0 } {}

				[[nodiscard]]
				constexpr Iterator(Size step_count, Point start, Size stride) noexcept
					: m_countX{ step_count.x }
					, m_startX{ start.x }
					, m_step_counter{ step_count }
					, m_counter{ start }
					, m_stride{ stride } {}

				constexpr Iterator& operator ++() noexcept
				{
					if (m_step_counter.x == 1)
					{
						m_step_counter.x = m_countX;

						--m_step_counter.y;

						m_counter.x = m_startX;

						m_counter.y = (m_counter.y + m_stride.y);
					}
					else
					{
						--m_step_counter.x;

						m_counter.x = (m_counter.x + m_stride.x);
					}

					return *this;
				}

				constexpr Iterator operator ++(int) noexcept
				{
					Iterator it = *this;

					++(*this);

					return it;
				}

				[[nodiscard]]
				constexpr const Point& operator *() const noexcept
				{
					return m_counter;
				}

				[[nodiscard]]
				constexpr const Point* operator ->() const noexcept
				{
					return &m_counter;
				}

				[[nodiscard]]
				constexpr bool operator ==(const Iterator& r) const noexcept
				{
					return (m_step_counter == r.m_step_counter);
				}

				[[nodiscard]]
				constexpr bool operator !=(const Iterator& r) const noexcept
				{
					return !(*this == r);
				}

			private:

				int32 m_countX;

				int32 m_startX;

				Size m_step_counter;

				Point m_counter;

				Size m_stride;
			};

			using iterator = Iterator;

			[[nodiscard]]
			constexpr Step2D(Point start, Size step_count, Size stride) noexcept
				: m_start{ start }
				, m_step_count{ step_count }
				, m_stride{ stride } {}

			[[nodiscard]]
			constexpr iterator begin() const noexcept
			{
				if ((m_step_count.x <= 0) || (m_step_count.y <= 0))
				{
					return end();
				}

				return iterator{ m_step_count, m_start, m_stride };
			}

			[[nodiscard]]
			constexpr iterator end() const noexcept
			{
				return iterator{ Size{ m_step_count.x, 0 }, Point{ 0, 0 }, Size{ 0, 0 } };
			}

		private:

			Point m_start;

			Size m_step_count;

			Size m_stride;
		};
	}

# endif

	////////////////////////////////////////////////////////////////
	//
	//	step
	//
	////////////////////////////////////////////////////////////////

# ifdef __cpp_lib_ranges_cartesian_product

	constexpr auto step(const Size n) noexcept
	{
		const auto x_view = std::views::iota(0, n.x);
		const auto y_view = std::views::iota(0, n.y);

		return (std::views::cartesian_product(y_view, x_view)
			| std::views::transform([](const auto& p) -> Point
				{ return{ std::get<1>(p), std::get<0>(p) }; }));
	}

	constexpr auto step(const Point start, const Size n) noexcept
	{
		const auto x_view = std::views::iota(start.x, (start.x + n.x));
		const auto y_view = std::views::iota(start.y, (start.y + n.y));

		return (std::views::cartesian_product(y_view, x_view)
			| std::views::transform([](const auto& p) -> Point
				{ return{ std::get<1>(p), std::get<0>(p) }; }));
	}

	constexpr auto step(const Point start, const Size n, const Size stride) noexcept
	{
		const auto x_view = std::views::iota(0, n.x);
		const auto y_view = std::views::iota(0, n.y);

		return (std::views::cartesian_product(y_view, x_view)
			| std::views::transform([=](const auto& p) -> Point
				{
					return{
						(start.x + std::get<1>(p) * stride.x),
						(start.y + std::get<0>(p) * stride.y)
					};
				}));
	}

# else

	constexpr auto step(const Size n) noexcept
	{
		return detail::Step2D{ Point{ 0, 0 }, n, Size{ 1, 1 } };
	}

	constexpr auto step(const Point start, const Size n) noexcept
	{
		return detail::Step2D{ start, n, Size{ 1, 1 } };
	}

	constexpr auto step(const Point start, const Size n, const Size stride) noexcept
	{
		return detail::Step2D{ start, n, stride };
	}

# endif

	////////////////////////////////////////////////////////////////
	//
	//	step_backward
	//
	////////////////////////////////////////////////////////////////

# ifdef __cpp_lib_ranges_cartesian_product

	constexpr auto step_backward(const Size n) noexcept
	{
		const auto x_view = std::views::iota(0, n.x);
		const auto y_view = std::views::iota(0, n.y);

		return (std::views::cartesian_product(y_view, x_view)
			| std::views::reverse
			| std::views::transform([](const auto& p) -> Point
				{ return{ std::get<1>(p), std::get<0>(p) }; }));
	}

# else

	constexpr auto step_backward(const Size n) noexcept
	{
		return detail::Step2D{ Point{ (n.x - 1), (n.y - 1) }, n, Size{ -1, -1 } };
	}

# endif
}