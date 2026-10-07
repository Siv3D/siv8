//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"

TEST_CASE("Bezier3.ArcLength.DistanceAndStationaryPoints")
{
	const Bezier3 nonlinear{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 100, 0 } };
	CHECK(Abs(nonlinear.computeLength() - 100) < 1e-10);
	CHECK(nonlinear.computeTAtDistance(-10) == 0);
	CHECK(nonlinear.computeTAtDistance(110) == 1);
	for (const double distance : { 0.001, 1.0, 12.5, 50.0, 99.0 })
	{
		CAPTURE(distance);
		CHECK(nonlinear.computePointAtDistance(distance).distanceFrom({ distance, 0 }) < 1e-7);
	}
	const Bezier3 constant{ { 3, 7 }, { 3, 7 }, { 3, 7 }, { 3, 7 } };
	CHECK(constant.computeLength() == 0);
	CHECK(constant.computeTAtDistance(50) == 0);
	CHECK(constant.computePointAtDistance(50) == Vec2{ 3, 7 });
	const Bezier3 reversing{ { 0, 0 }, { 100, 0 }, { -100, 0 }, { 0, 0 } };
	const double extremum = (50 / std::sqrt(3.0));
	CHECK(Abs(reversing.computeLength() - 4 * extremum) < 1e-7);
	for (const double distance : { extremum, 2 * extremum, 3 * extremum })
	{
		CAPTURE(distance);
		const double x = ((distance <= extremum) ? distance : ((distance <= 3 * extremum) ? (2 * extremum - distance) : (distance - 4 * extremum)));
		CHECK(reversing.computePointAtDistance(distance).distanceFrom({ x, 0 }) < 1e-7);
	}
	const auto rotated = Bezier3{ reversing.p0.rotated(0.7), reversing.p1.rotated(0.7), reversing.p2.rotated(0.7), reversing.p3.rotated(0.7) };
	CHECK(rotated.computePointAtDistance(extremum).distanceFrom(Vec2{ extremum, 0 }.rotated(0.7)) < 1e-7);
}

TEST_CASE("Bezier3.Flattening.LoopsAndDepthLimit")
{
	const Bezier3 loop{ { 0, 0 }, { 100, 100 }, { -100, 100 }, { 0, 0 } };
	const auto polyline = loop.toLineStringAdaptive(0.1, 12);
	CHECK(polyline.size() > 20);
	CHECK(polyline.front() == loop.p0);
	CHECK(polyline.back() == loop.p3);
	CHECK(loop.toLineStringAdaptive(0.1, 0).size() == 1);
	CHECK(loop.toLineStringAdaptive(0.1, -1).size() == 1);
	const Bezier3 nonlinear{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 100, 0 } };
	CHECK(nonlinear.toLineStringAdaptive().size() == 2);
}

namespace
{
	template <class Bezier>
	void CheckClosest(const Bezier& curve, const Vec2& query, const Vec2& expected, const double tolerance = 1e-10)
	{
		const double t = curve.computeClosestT(query);
		CHECK(0.0 <= t);
		CHECK(t <= 1.0);
		const Vec2 point = curve.computeClosestPoint(query);
		CHECK(point == curve.pointAt(t));
		CHECK(point.distanceFrom(expected) <= tolerance);
		CHECK(curve.reversed().computeClosestPoint(query).distanceFrom(expected) <= tolerance);
	}
}

TEST_CASE("Bezier3.ClosestPoint.ClosedAndCollinear")
{
	CheckClosest(Bezier3{ { 0, 0 }, { 100, 200 }, { -100, 200 }, { 0, 0 } }, { 0, 151 }, { 0, 150 });
	const Bezier3 nearlyClosed{ { 0, 0 }, { 100, 200 }, { -100, 200 }, { 1e-7, 0 } };
	CheckClosest(nearlyClosed, (nearlyClosed.pointAt(0.5) + Vec2{ 0, 1 }), nearlyClosed.pointAt(0.5));
	CheckClosest(Bezier3{ { 0, 0 }, { 100, 0 }, { -50, 0 }, { 50, 0 } }, { 10, 1 }, { 10, 0 });
	CheckClosest(Bezier3{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 100, 0 } }, { 10, 1 }, { 10, 0 });
	CheckClosest(Bezier3{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 100, 0 } }, { 0.0001, 1 }, { 0.0001, 0 });
	CheckClosest(Bezier3{ { 0, 0 }, { 100, 0 }, { -100, 0 }, { 0, 0 } },
		{ 40, 2 }, { (50.0 / std::sqrt(3.0)), 0 });
	CheckClosest(Bezier3{ { 0, 0 }, { 100, 0 }, { -100, 0 }, { 0, 0 } },
		{ -40, 2 }, { (-50.0 / std::sqrt(3.0)), 0 });
}

TEST_CASE("Bezier3.ClosestPoint.EndpointsAndDegeneracy")
{
	const Bezier3 curve{ { 0, 0 }, { 20, 90 }, { 70, -20 }, { 100, 40 } };
	CHECK(curve.computeClosestT({ -10, -10 }) == 0.0);
	CHECK(curve.computeClosestT({ 110, 40 }) == 1.0);
	CheckClosest(curve, { -10, -10 }, curve.p0);
	CheckClosest(curve, { 110, 40 }, curve.p3);
	CheckClosest(Bezier3{ { 3, 4 }, { 3, 4 }, { 3, 4 }, { 3, 4 } }, { 7, -2 }, { 3, 4 });
	CheckClosest(Bezier3{ { 0, 0 }, { 30, 10 }, { 60, 20 }, { 90, 30 } }, { 29, 13 }, { 30, 10 });
	for (const double t : { 0.0, 1e-4, 0.123, 0.5, 0.99, 1.0 })
	{
		CAPTURE(t);
		CheckClosest(curve, curve.pointAt(t), curve.pointAt(t));
		const Bezier3 stationaryStart{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 100, 20 } };
		CheckClosest(stationaryStart, stationaryStart.pointAt(t), stationaryStart.pointAt(t));
	}
}

TEST_CASE("Bezier3.ClosestPoint.ScaleAndRotation")
{
	// x = 100*(t - u), y = 100*(t - u)^2 + 1.
	constexpr double u = 0.37;
	const Vec2 c0{ (-100 * u), (100 * u * u + 1) }, c1{ 100, (-200 * u) }, c2{ 0, 100 };
	const Bezier3 curve{ c0, (c0 + c1 / 3), (c0 + 2 * c1 / 3 + c2 / 3), (c0 + c1 + c2) };
	for (const double scale : { 1e-6, 0.01, 1.0, 100.0 })
	{
		for (const double angle : { 0.0, 0.7, Math::HalfPi })
		{
			CAPTURE(scale, angle);
			const auto Transform = [&](const Vec2& p) { return ((p.rotated(angle) + Vec2{ 7, -11 }) * scale); };
			const Bezier3 transformed{ Transform(curve.p0), Transform(curve.p1), Transform(curve.p2), Transform(curve.p3) };
			CheckClosest(transformed, Transform({ 0, 0 }), Transform({ 0, 1 }), (1e-10 * scale));
			CHECK(Abs(transformed.computeClosestT(Transform({ 0, 0 })) - u) <= 1e-12);
			const Bezier3 closed{ Transform({ 0, 0 }), Transform({ 100, 200 }), Transform({ -100, 200 }), Transform({ 0, 0 }) };
			CheckClosest(closed, Transform({ 0, 151 }), Transform({ 0, 150 }), (1e-10 * scale));
		}
	}
}

TEST_CASE("Bezier3.ClosestPoint.NearbyMinima")
{
	// x = 10000*(t - u)^2, y = 1000*(t - u)^3.
	// A nearby minimum and maximum lie inside one 1/64 interval.
	constexpr double u = 0.504;
	const Vec2 c0{ (10000 * u * u), (-1000 * u * u * u) };
	const Vec2 c1{ (-20000 * u), (3000 * u * u) }, c2{ 10000, (-3000 * u) }, c3{ 0, 1000 };
	const Bezier3 curve{ c0, (c0 + c1 / 3), (c0 + 2 * c1 / 3 + c2 / 3), (c0 + c1 + c2 + c3) };
	const Vec2 query = curve.pointAt(0.5095);
	CheckClosest(curve, query, query, 1e-8);
}

TEST_CASE("Bezier2.ClosestPoint.EndpointsAndDegeneracy")
{
	const Bezier2 curve{ { 0, 0 }, { 30, 100 }, { 100, 0 } };
	CheckClosest(curve, { -10, -10 }, curve.p0);
	CheckClosest(curve, { 110, 0 }, curve.p2);
	CheckClosest(Bezier2{ { 3, 4 }, { 3, 4 }, { 3, 4 } }, { 7, -2 }, { 3, 4 });
	CheckClosest(Bezier2{ { 0, 0 }, { 100, 0 }, { 0, 0 } }, { 60, 2 }, { 50, 0 });
	CheckClosest(Bezier2{ { 0, 0 }, { 0, 0 }, { 100, 0 } }, { 25, 1 }, { 25, 0 });
	CheckClosest(Bezier2{ { 0, 0 }, { 50, 0 }, { 100, 0 } }, { 25, 1 }, { 25, 0 });
	CheckClosest(Bezier2{ { 0, 0 }, { 5e-9, 0 }, { 1e-8, 0 } }, { 7.5e-9, 1e-9 }, { 7.5e-9, 0 }, 1e-20);
	for (const double t : { 0.0, 0.001, 0.123, 0.5, 0.99, 1.0 })
	{
		CAPTURE(t);
		CheckClosest(curve, curve.pointAt(t), curve.pointAt(t));
	}
}

TEST_CASE("Bezier2.ClosestPoint.ScaleAndRotation")
{
	// x = 100*(t - u), y = 100*(t - u)^2 + 1.
	constexpr double u = 0.37;
	const Vec2 c0{ (-100 * u), (100 * u * u + 1) }, c1{ 100, (-200 * u) }, c2{ 0, 100 };
	const Bezier2 curve{ c0, (c0 + c1 / 2), (c0 + c1 + c2) };
	for (const double scale : { 1e-8, 0.01, 1.0, 100.0 })
	{
		for (const double angle : { 0.0, 0.7, Math::HalfPi })
		{
			CAPTURE(scale, angle);
			const auto Transform = [&](const Vec2& p) { return ((p.rotated(angle) + Vec2{ 7, -11 }) * scale); };
			const Bezier2 transformed{ Transform(curve.p0), Transform(curve.p1), Transform(curve.p2) };
			CheckClosest(transformed, Transform({ 0, 0 }), Transform({ 0, 1 }), (1e-10 * scale));
			CHECK(Abs(transformed.computeClosestT(Transform({ 0, 0 })) - u) <= 1e-12);
		}
	}
}
