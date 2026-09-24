//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

namespace
{
	template <class... Types>
	struct TypeList {};

	using AreaShapes = TypeList<Rect, RectF, Circle, Ellipse, SuperEllipse,
		Triangle, Quad, RoundRect, Polygon, MultiPolygon>;
	using NonAreaShapes = TypeList<Point, Vec2, Line, LineString, Bezier2, Bezier3>;

	template <class A, class B>
	concept HasMemberContains = requires(const A& a, const B& b)
	{
		{ a.contains(b) } -> std::same_as<bool>;
	};

	template <class A, class B>
	concept HasMemberOverlaps = requires(const A& a, const B& b)
	{
		{ a.overlaps(b) } -> std::same_as<bool>;
	};

	template <class A, class B>
	concept HasFreeContains = requires(const A& a, const B& b)
	{
		{ Geometry2D::Contains(a, b) } -> std::same_as<bool>;
	};

	template <class A, class B>
	concept HasFreeOverlaps = requires(const A& a, const B& b)
	{
		{ Geometry2D::Overlaps(a, b) } -> std::same_as<bool>;
	};

	struct CircleProxy
	{
		Circle value;

		operator Circle() const { return value; }
	};

	struct ExplicitCircleProxy
	{
		explicit operator Circle() const;
	};

	struct MutableCircleProxy
	{
		operator Circle();
	};

	struct AmbiguousShapeProxy
	{
		operator Circle() const;
		operator Ellipse() const;
	};

	struct Unrelated {};

	template <class A, class... Bs>
	consteval bool AllContainable(TypeList<Bs...>)
	{
		return (HasMemberContains<A, Bs> && ...);
	}

	template <class A, class... Bs>
	consteval bool AllOverlappable(TypeList<Bs...>)
	{
		return (HasMemberOverlaps<A, Bs> && ...);
	}

	template <class A, class... Bs>
	consteval bool NoneOverlappable(TypeList<Bs...>)
	{
		return ((!HasMemberOverlaps<A, Bs>) && ...);
	}

	template <class A, class... Bs>
	consteval bool MatchesFreeFunctions(TypeList<Bs...>)
	{
		return (((HasMemberContains<A, Bs> == HasFreeContains<A, Bs>)
			&& (HasMemberOverlaps<A, Bs> == HasFreeOverlaps<A, Bs>)) && ...);
	}

	template <class A>
	consteval bool CheckAreaShape()
	{
		return AllContainable<A>(AreaShapes{})
			&& AllContainable<A>(TypeList<Point, Vec2, Line, LineString, Float2, CircleProxy>{})
			&& !HasMemberContains<A, Bezier2> && !HasMemberContains<A, Bezier3>
			&& AllOverlappable<A>(AreaShapes{}) && HasMemberOverlaps<A, CircleProxy>
			&& NoneOverlappable<A>(NonAreaShapes{})
			&& !HasMemberContains<A, ExplicitCircleProxy> && !HasMemberOverlaps<A, ExplicitCircleProxy>
			&& !HasMemberContains<A, MutableCircleProxy> && !HasMemberOverlaps<A, MutableCircleProxy>
			&& !HasMemberContains<A, AmbiguousShapeProxy> && !HasMemberOverlaps<A, AmbiguousShapeProxy>
			&& !HasMemberContains<A, Unrelated> && !HasMemberOverlaps<A, Unrelated>
			&& !HasMemberContains<A, int> && !HasMemberOverlaps<A, int>
			&& MatchesFreeFunctions<A>(AreaShapes{})
			&& MatchesFreeFunctions<A>(NonAreaShapes{})
			&& MatchesFreeFunctions<A>(TypeList<Float2, CircleProxy, ExplicitCircleProxy,
				MutableCircleProxy, AmbiguousShapeProxy, Unrelated, int>{});
	}

	template <class... As>
	consteval bool CheckAreaShapes(TypeList<As...>)
	{
		return (CheckAreaShape<As>() && ...);
	}

	template <class A, class B>
	Optional<bool> TryContains(const A& a, const B& b)
	{
		if constexpr (HasMemberContains<A, B>)
		{
			return a.contains(b);
		}
		else
		{
			return none;
		}
	}

	template <class A, class B>
	Optional<bool> TryOverlaps(const A& a, const B& b)
	{
		if constexpr (HasMemberOverlaps<A, B>)
		{
			return a.overlaps(b);
		}
		else
		{
			return none;
		}
	}
}

TEST_CASE("Geometry2D.ContainsOverlaps.MemberAvailability")
{
	static_assert(CheckAreaShapes(AreaShapes{}));
	static_assert(HasMemberContains<LineString, Vec2>);
	static_assert(!HasMemberContains<LineString, Line>);
	static_assert(!HasMemberOverlaps<LineString, Rect>);
	SUCCEED();
}

TEST_CASE("Geometry2D.ContainsOverlaps.GenericDispatch")
{
	const Circle circle{ 0, 0, 5 };
	CHECK(TryContains(circle, Circle{ 0, 0, 1 }) == Optional<bool>{ true });
	CHECK(TryContains(circle, Circle{ 10, 0, 1 }) == Optional<bool>{ false });
	CHECK(TryContains(circle, Bezier2{}) == none);
	CHECK(TryContains(circle, Bezier3{}) == none);
	CHECK(TryOverlaps(circle, Circle{ 0, 0, 1 }) == Optional<bool>{ true });
	CHECK(TryOverlaps(circle, Circle{ 10, 0, 1 }) == Optional<bool>{ false });
	CHECK(TryOverlaps(circle, Line{ -1, 0, 1, 0 }) == none);
	CHECK(TryOverlaps(circle, Vec2{ 0, 0 }) == none);
	CHECK(TryContains(circle, AmbiguousShapeProxy{}) == none);
	CHECK(TryOverlaps(circle, AmbiguousShapeProxy{}) == none);
}

TEST_CASE("Geometry2D.ContainsOverlaps.ImplicitConversions")
{
	const Circle circle{ 0, 0, 5 };
	const CircleProxy inside{ Circle{ 0, 0, 1 } };
	const CircleProxy outside{ Circle{ 10, 0, 1 } };
	CHECK(circle.contains(inside));
	CHECK_FALSE(circle.contains(outside));
	CHECK(circle.overlaps(inside));
	CHECK_FALSE(circle.overlaps(outside));
	CHECK(circle.contains(Float2{ 0, 0 }));
	CHECK_FALSE(circle.contains(Float2{ 10, 0 }));
}
