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
	inline std::span<const Vec2> PolygonHolesView::getHole(const size_t index) const
	{
		return m_holeFunction(m_holes, index);
	}

	constexpr PolygonHolesView::iterator::iterator(const PolygonHolesView* view, const size_t index) noexcept
		: m_view{ view }
		, m_index{ index } {}

	inline PolygonHolesView::iterator::reference PolygonHolesView::iterator::operator*() const
	{
		return m_view->getHole(m_index);
	}

	inline PolygonHolesView::iterator& PolygonHolesView::iterator::operator++() noexcept
	{
		++m_index;
		return *this;
	}

	inline PolygonHolesView::iterator PolygonHolesView::iterator::operator++(int) noexcept
	{
		auto result = *this;
		++*this;
		return result;
	}

	template <class Rings>
	PolygonHolesView::PolygonHolesView(const Rings& rings) noexcept
		: m_holes{ std::addressof(rings) }
		, m_sizeFunction{
			[](const void* p) -> size_t
			{
				return std::ranges::size(*static_cast<const Rings*>(p));
			}
		}
		, m_holeFunction{
			[](const void* p, const size_t index) -> std::span<const Vec2>
			{
				const auto& holes = *static_cast<const Rings*>(p);
				const auto& hole = (*(std::ranges::begin(holes) + static_cast<std::ptrdiff_t>(index)));

				using Point = std::remove_cv_t<std::ranges::range_value_t<std::remove_reference_t<decltype(hole)>>>;
				static_assert(std::same_as<Point, Vec2>);

				return{ std::ranges::data(hole), std::ranges::size(hole) };
			}
		} {}

	inline PolygonHolesView::iterator PolygonHolesView::begin() const noexcept
	{
		return iterator{ this, 0 };
	}

	inline PolygonHolesView::iterator PolygonHolesView::end() const
	{
		return iterator{ this, size() };
	}

	inline size_t PolygonHolesView::size() const
	{
		return m_holes ? m_sizeFunction(m_holes) : 0;
	}

	inline bool PolygonHolesView::empty() const
	{
		return (size() == 0);
	}

	inline std::span<const Vec2> PolygonHolesView::operator[](const size_t index) const
	{
		return getHole(index);
	}

	inline Array<Array<Vec2>> PolygonHolesView::asArray() const
	{
		return std::ranges::to<Array<Array<Vec2>>>(*this);
	}
}
