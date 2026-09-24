//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <type_traits>
# include <utility>
# include <Siv3D/Ray2D.hpp>
# include <Siv3D/RaycastHit2D.hpp>
# include <Siv3D/Geometry2D/Raycast.hpp>

namespace
{
	template <class Shape, class = void>
	struct HasGeometry2DRaycast : std::false_type {};

	template <class Shape>
	struct HasGeometry2DRaycast<Shape, std::void_t<decltype(
		Geometry2D::Raycast(std::declval<const Ray2D&>(), std::declval<const Shape&>()))>>
		: std::is_same<decltype(Geometry2D::Raycast(
			std::declval<const Ray2D&>(), std::declval<const Shape&>())), Optional<RaycastHit2D>> {};

	template <class Shape, class = void>
	struct HasGeometry2DRaycastRange : std::false_type {};

	template <class Shape>
	struct HasGeometry2DRaycastRange<Shape, std::void_t<decltype(
		Geometry2D::Raycast(std::declval<const Ray2D&>(), std::declval<const Shape&>(), 0.0, 1.0))>>
		: std::is_same<decltype(Geometry2D::Raycast(
			std::declval<const Ray2D&>(), std::declval<const Shape&>(), 0.0, 1.0)), Optional<RaycastHit2D>> {};

	template <class Shape, class = void>
	struct HasReversedGeometry2DRaycast : std::false_type {};

	template <class Shape>
	struct HasReversedGeometry2DRaycast<Shape, std::void_t<decltype(
		Geometry2D::Raycast(std::declval<const Shape&>(), std::declval<const Ray2D&>()))>>
		: std::true_type {};
}

TEST_CASE("Geometry2D.Raycast.FreeFunctionAvailability")
{
	static_assert(not HasGeometry2DRaycast<Point>::value);
	static_assert(not HasGeometry2DRaycast<Vec2>::value);
	static_assert(not HasGeometry2DRaycast<Line>::value);
	static_assert(not HasGeometry2DRaycast<LineString>::value);
	static_assert(not HasGeometry2DRaycast<Bezier2>::value);
	static_assert(not HasGeometry2DRaycast<Bezier3>::value);

	static_assert(HasGeometry2DRaycast<Rect>::value);
	static_assert(HasGeometry2DRaycast<RectF>::value);
	static_assert(HasGeometry2DRaycast<Circle>::value);
	static_assert(HasGeometry2DRaycast<Ellipse>::value);
	static_assert(HasGeometry2DRaycast<SuperEllipse>::value);
	static_assert(HasGeometry2DRaycast<Triangle>::value);
	static_assert(HasGeometry2DRaycast<Quad>::value);
	static_assert(HasGeometry2DRaycast<RoundRect>::value);
	static_assert(HasGeometry2DRaycast<Polygon>::value);
	static_assert(HasGeometry2DRaycast<MultiPolygon>::value);

	static_assert(not HasReversedGeometry2DRaycast<Rect>::value);
	static_assert(not HasReversedGeometry2DRaycast<Circle>::value);
	static_assert(not HasReversedGeometry2DRaycast<Polygon>::value);
	static_assert(not HasReversedGeometry2DRaycast<MultiPolygon>::value);
}

TEST_CASE("Geometry2D.Raycast.RangeAvailability")
{
	static_assert(not HasGeometry2DRaycastRange<Point>::value);
	static_assert(not HasGeometry2DRaycastRange<Vec2>::value);
	static_assert(not HasGeometry2DRaycastRange<Line>::value);
	static_assert(not HasGeometry2DRaycastRange<LineString>::value);
	static_assert(not HasGeometry2DRaycastRange<Bezier2>::value);
	static_assert(not HasGeometry2DRaycastRange<Bezier3>::value);
	static_assert(HasGeometry2DRaycastRange<Rect>::value);
	static_assert(HasGeometry2DRaycastRange<RectF>::value);
	static_assert(HasGeometry2DRaycastRange<Circle>::value);
	static_assert(HasGeometry2DRaycastRange<Ellipse>::value);
	static_assert(HasGeometry2DRaycastRange<SuperEllipse>::value);
	static_assert(HasGeometry2DRaycastRange<Triangle>::value);
	static_assert(HasGeometry2DRaycastRange<Quad>::value);
	static_assert(HasGeometry2DRaycastRange<RoundRect>::value);
	static_assert(HasGeometry2DRaycastRange<Polygon>::value);
	static_assert(HasGeometry2DRaycastRange<MultiPolygon>::value);
}
