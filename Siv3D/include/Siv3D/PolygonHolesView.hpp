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
# include <ranges>
# include <span>
# include "PointVector.hpp"

namespace s3d
{
	class PolygonHolesView: public std::ranges::view_interface<PolygonHolesView>
	{
	public:

		class iterator
		{
		public:

			using iterator_concept  = std::forward_iterator_tag;
			using iterator_category = std::forward_iterator_tag;
			using value_type        = std::span<const Vec2>;
			using difference_type   = std::ptrdiff_t;
			using reference         = value_type;

			constexpr iterator() noexcept = default;

			[[nodiscard]]
			reference operator*() const;
			
			iterator& operator++() noexcept;
			
			iterator operator++(int) noexcept;

			friend bool operator==(const iterator&, const iterator&) noexcept = default;

		private:

			const PolygonHolesView* m_view = nullptr;

			size_t m_index = 0;

			friend class PolygonHolesView;

			constexpr iterator(const PolygonHolesView* view, size_t index) noexcept;
		};

		constexpr PolygonHolesView() noexcept = default;

		template <class Rings>
		explicit PolygonHolesView(const Rings& rings) noexcept;

		[[nodiscard]]
		iterator begin() const noexcept;
		
		[[nodiscard]]
		iterator end() const;
		
		[[nodiscard]]
		size_t size() const;
		
		[[nodiscard]]
		bool empty() const;
		
		[[nodiscard]]
		std::span<const Vec2> operator[](size_t index) const;

		[[nodiscard]]
		Array<Array<Vec2>> asArray() const;

	private:

		using SizeFunction = size_t(*)(const void*);
		using HoleFunction = std::span<const Vec2>(*)(const void*, size_t);

		const void* m_holes = nullptr;

		SizeFunction m_sizeFunction = nullptr;

		HoleFunction m_holeFunction = nullptr;

		[[nodiscard]]
		std::span<const Vec2> getHole(size_t index) const;
	};
}

# include "detail/PolygonHolesView.ipp"
