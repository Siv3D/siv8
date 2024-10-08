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
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr LineString::LineString(const container_type& other)
		: m_points{ other } {}

	constexpr LineString::LineString(container_type&& other)
		: m_points{ std::move(other) } {}

	constexpr LineString::LineString(const size_type count, const value_type& value)
		: m_points(count, value) {}

	constexpr LineString::LineString(const size_type count)
		: m_points(count) {}

	template <class Iterator>
	constexpr LineString::LineString(Iterator first, Iterator last)
		: m_points(first, last) {}

	LineString::LineString(const Array<Point>& points)
		: m_points(points.begin(), points.end()) {}

	constexpr LineString::LineString(const HasAsArray auto& a)
		: m_points{ a.asArray() } {}

	constexpr LineString::LineString(HasAsArray auto&& a)
		: m_points{ std::forward<decltype(a)>(a).asArray() } {}

	constexpr LineString::LineString(std::initializer_list<value_type> list)
		: m_points(list) {}

# ifdef __cpp_lib_containers_ranges

	template <Concept::ContainerCompatibleRange<Vec2> Range>
	constexpr LineString::LineString(std::from_range_t, Range&& range)
		: m_points(std::from_range, std::forward<Range>(range)) {}

	template <Concept::ContainerCompatibleRange<Point> Range>
	constexpr LineString::LineString(std::from_range_t, Range&& range)
		: m_points(std::from_range, std::forward<Range>(range)) {}

# endif

	constexpr LineString::LineString(Arg::reserve_<size_type> size)
		: m_points{ size } {}

}
