//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <chrono>
# include <cstdio>

namespace
{
	Bezier3 Straight(const Vec2 a, const Vec2 b)
	{
		return Bezier3::FromHermite(a, (b - a), b, (b - a));
	}

	void CheckPoint(const Vec2 actual, const Vec2 expected, const double error = 1e-7)
	{
		CHECK(actual.distanceFrom(expected) <= error);
	}

	// Independent nonuniform Catmull-Rom evaluation (recursive interpolation).
	Vec2 CentripetalPoint(const std::array<Vec2, 4>& p, const double u)
	{
		std::array<double, 4> t{};
		for (size_t i = 1; i < 4; ++i)
		{
			t[i] = (t[i - 1] + std::sqrt(p[i].distanceFrom(p[i - 1])));
		}
		const double x = (t[1] + (t[2] - t[1]) * u);
		const auto Mix = [&](const Vec2 a, const Vec2 b, const double ta, const double tb)
		{
			return (((tb - x) * a + (x - ta) * b) / (tb - ta));
		};
		const Vec2 a0 = Mix(p[0], p[1], t[0], t[1]);
		const Vec2 a1 = Mix(p[1], p[2], t[1], t[2]);
		const Vec2 a2 = Mix(p[2], p[3], t[2], t[3]);
		return Mix(Mix(a0, a1, t[0], t[2]), Mix(a1, a2, t[1], t[3]), t[1], t[2]);
	}

	double PolylineDistance(const LineString& line, const Vec2 point)
	{
		double best = Math::Inf;
		for (size_t i = 1; i < line.size(); ++i)
		{
			const Vec2 d = (line[i] - line[i - 1]);
			const double t = ((d.lengthSq() == 0) ? 0.0 : Clamp((point - line[i - 1]).dot(d) / d.lengthSq(), 0.0, 1.0));
			best = Min(best, point.distanceFrom(line[i - 1] + d * t));
		}
		return best;
	}
}

TEST_CASE("Spline2D.EmptyAndInvalidInput")
{
	Spline2D empty;
	CHECK(empty.isEmpty());
	CHECK(not empty.isClosed());
	CHECK(empty.segmentCount() == 0);
	const Optional<SplineClosestPoint2D> closest = empty.computeClosestPoint({ 1, 2 });
	CHECK(not closest);
	CHECK(not empty.computeBoundingRect());
	CHECK(empty.toLineString().isEmpty());
	CHECK(empty.toLineStringAdaptive().isEmpty());
	CHECK_THROWS_AS(empty.segment(0), std::out_of_range);
	CHECK_THROWS_AS(empty.pointAt({ 0, 0 }), std::out_of_range);
	CHECK(Spline2D::FromCatmullRom({}).isEmpty());
	CHECK(Spline2D::FromBezierSegments({}, CloseRing::Yes).isEmpty());
	const Array<Vec2> same{ { 3, 4 }, { 3, 4 }, { 3, 4 } };
	CHECK(Spline2D::FromCatmullRom(same).isEmpty());
	CHECK_THROWS_AS(Spline2D::FromCatmullRom(same, CloseRing::Yes), std::invalid_argument);
	CHECK_THROWS_AS(Spline2D::FromCatmullRom({}, CloseRing::Yes), std::invalid_argument);
	const Array<Bezier3> disconnected{ Straight({ 0, 0 }, { 1, 0 }), Straight({ 2, 0 }, { 3, 0 }) };
	CHECK_THROWS_AS(Spline2D::FromBezierSegments(disconnected), std::invalid_argument);
	const Array<Bezier3> open{ Straight({ 0, 0 }, { 1, 0 }) };
	CHECK_THROWS_AS(Spline2D::FromBezierSegments(open, CloseRing::Yes), std::invalid_argument);
	LineString output{ { 9, 8 }, { 7, 6 } };
	const LineString saved = output;
	CHECK_THROWS_AS(empty.toLineStringAdaptive(output, 0), std::invalid_argument);
	CHECK_THROWS_AS(empty.toLineStringAdaptive(output, -1), std::invalid_argument);
	CHECK_THROWS_AS(empty.toLineStringAdaptive(output, 1, -1), std::invalid_argument);
	CHECK_THROWS_AS(empty.toLineStringAdaptive(output, 1, 21), std::invalid_argument);
	CHECK(output == saved);
	empty.toLineString(output);
	CHECK(output.isEmpty());
}

TEST_CASE("Spline2D.CatmullRom.OpenAndParameterization")
{
	const Array<Vec2> points{ { 0, 0 }, { 2, 7 }, { 60, -10 }, { 63, 5 } };
	const auto centripetal = Spline2D::FromCatmullRom(points);
	const auto uniform = Spline2D::FromCatmullRom(points, CloseRing::No, CatmullRomParameterization::Uniform);
	CHECK(centripetal.segmentCount() == 3);
	CHECK(not centripetal.isClosed());
	for (size_t i = 0; i < 3; ++i)
	{
		CHECK(centripetal.pointAt({ i, 0 }) == points[i]);
		CHECK(centripetal.pointAt({ i, 1 }) == points[i + 1]);
		const Vec2 previous = ((i == 0) ? (2 * points[0] - points[1]) : points[i - 1]);
		const Vec2 next = ((i == 2) ? (2 * points[3] - points[2]) : points[i + 2]);
		for (const double t : { 0.1, 0.3, 0.5, 0.8 })
		{
			CheckPoint(centripetal.pointAt({ i, t }), CentripetalPoint({ previous, points[i], points[i + 1], next }, t));
			CheckPoint(uniform.pointAt({ i, t }), Spline::CatmullRom(previous, points[i], points[i + 1], next, t));
		}
	}
	CHECK(centripetal.pointAt({ 1, 0.5 }).distanceFrom(uniform.pointAt({ 1, 0.5 })) > 0.1);
	const Array<Vec2> two{ { 2, 3 }, { 2, 3 }, { 20, 30 }, { 20, 30 } };
	const auto line = Spline2D::FromCatmullRom(two);
	CHECK(line.segmentCount() == 1);
	for (const double t : { 0.0, 0.25, 0.5, 1.0 })
	{
		CheckPoint(line.pointAt({ 0, t }), Vec2{ 2, 3 }.lerp({ 20, 30 }, t));
	}
}

TEST_CASE("Spline2D.CatmullRom.ClosedAndRepeatedKnots")
{
	const Array<Vec2> points{ { 0, 0 }, { 30, 10 }, { 20, 40 }, { -10, 10 } };
	const Array<Vec2> repeated{ { 0, 0 }, { 0, 0 }, { 30, 10 }, { 20, 40 }, { 20, 40 }, { -10, 10 }, { 0, 0 }, { 0, 0 } };
	const auto ring = Spline2D::FromCatmullRom(repeated, CloseRing::Yes);
	REQUIRE(ring.segmentCount() == points.size());
	CHECK(ring.isClosed());
	for (size_t i = 0; i < points.size(); ++i)
	{
		const size_t next = ((i + 1) % points.size());
		CHECK(ring.segment(i).p0 == points[i]);
		CHECK(ring.segment(i).p3 == points[next]);
		CheckPoint(ring.pointAt({ i, 0.37 }), CentripetalPoint({ points[(i + 3) % 4], points[i], points[next], points[(i + 2) % 4] }, 0.37));
		CheckPoint(ring.segment(i).tangentAt(1), ring.segment(next).tangentAt(0));
	}
	const auto vertices = ring.toLineString(1);
	CHECK(vertices.size() == 4);
	CHECK(vertices.front() != vertices.back());
	const auto polyline = ring.toLineStringAdaptive();
	CHECK(polyline.front() != polyline.back());
}

TEST_CASE("Spline2D.ClosestBoundsAndReverse")
{
	const Array<Bezier3> curves{ Straight({ 0, 0 }, { 10, 0 }), Straight({ 10, 0 }, { 10, 20 }) };
	const auto spline = Spline2D::FromBezierSegments(curves);
	const Optional<SplineClosestPoint2D> nearest = spline.computeClosestPoint({ 13, 7 });
	REQUIRE(nearest);
	CHECK(nearest->location.segment == 1);
	CheckPoint(nearest->point, { 10, 7 });
	CHECK(Abs(nearest->distanceSq - 9) < 1e-10);
	CHECK(spline.pointAt(nearest->location) == nearest->point);
	const Optional<SplineClosestPoint2D> join = spline.computeClosestPoint({ 10, 0 });
	REQUIRE(join);
	CHECK(join->location.segment == 0);
	CHECK(join->location.t == 1);
	const auto bounds = spline.computeBoundingRect();
	REQUIRE(bounds);
	CHECK(*bounds == RectF{ 0, 0, 10, 20 });
	auto reverse = spline.reversed();
	for (size_t i = 0; i < 2; ++i)
	{
		for (const double t : { 0.0, 0.37, 1.0 })
		{
			CheckPoint(reverse.pointAt({ 1 - i, 1 - t }), spline.pointAt({ i, t }));
		}
	}
	reverse.reverse();
	CHECK(reverse.toLineString() == spline.toLineString());
	reverse.clear();
	CHECK(reverse.isEmpty());
	CHECK(not reverse.isClosed());
	Spline2D copy = spline;
	copy.swap(reverse);
	CHECK(copy.isEmpty());
	CHECK(reverse.segmentCount() == 2);

	const Array<Bezier3> loops{ Bezier3{ { 0, 0 }, { 100, 200 }, { -100, 200 }, { 0, 0 } } };
	const auto loop = Spline2D::FromBezierSegments(loops, CloseRing::Yes);
	const auto closest = loop.computeClosestPoint({ 0, 151 });
	REQUIRE(closest);
	CheckPoint(closest->point, { 0, 150 });
	const auto loopBounds = loop.computeBoundingRect();
	REQUIRE(loopBounds);
	CHECK(Abs(loopBounds->h - 150) < 1e-10);
	CHECK(loop.reversed().isClosed());
}

TEST_CASE("Spline2D.FlatteningAndStorageReuse")
{
	const Array<Bezier3> curves{
		Bezier3{ { 0, 0 }, { 60, 150 }, { -80, 120 }, { 0, 0 } },
		Bezier3{ { 0, 0 }, { 100, 0 }, { -50, 0 }, { 20, 0 } }
	};
	const auto spline = Spline2D::FromBezierSegments(curves);
	LineString output;
	output.reserve(4096);
	const auto* storage = output.data();
	spline.toLineString(output, 5);
	CHECK(output.size() == 11);
	CHECK(output == spline.toLineString(5));
	CHECK(output.data() == storage);
	spline.toLineString(output, -1);
	CHECK(output.size() == 3);
	spline.toLineStringAdaptive(output, 0.1, 14);
	CHECK(output.data() == storage);
	CHECK(output == spline.toLineStringAdaptive(0.1, 14));
	CHECK(output.front() == curves.front().p0);
	CHECK(output.back() == curves.back().p3);
	for (const Bezier3& curve : curves)
	{
		for (int i = 0; i <= 1000; ++i)
		{
			CHECK(PolylineDistance(output, curve.pointAt(i / 1000.0)) <= 0.100001);
		}
	}
	const auto coarse = spline.toLineStringAdaptive(0.1, 0);
	CHECK(coarse.size() <= 3);
	CHECK(output.size() > coarse.size());
	Spline2D{}.toLineStringAdaptive(output);
	CHECK(output.isEmpty());
	CHECK(output.data() == storage);
}

TEST_CASE("Spline2DMeasure.DistanceModesAndZeroLengthSegments")
{
	const Array<Bezier3> curves{
		Straight({ 0, 0 }, { 0, 0 }), Straight({ 0, 0 }, { 10, 0 }),
		Straight({ 10, 0 }, { 10, 0 }), Straight({ 10, 0 }, { 110, 0 }), Straight({ 110, 0 }, { 110, 0 })
	};
	const Spline2DMeasure measure{ Spline2D::FromBezierSegments(curves) };
	CHECK(Abs(measure.length() - 110) < 1e-10);
	CHECK(measure.locationAtDistance(-1) == SplineLocation{ 0, 0 });
	CHECK(measure.locationAtDistance(measure.length()) == SplineLocation{ 4, 1 });
	const auto boundary = measure.locationAtDistance(measure.distanceAt({ 1, 1 }));
	CHECK(boundary == SplineLocation{ 3, 0 });
	for (const double d : { 0.0, 1.0, 9.0, 10.0, 50.0, 110.0 })
	{
		CheckPoint(measure.pointAtDistance(d), { d, 0 });
		CHECK(Abs(measure.distanceAt(measure.locationAtDistance(d)) - d) < 1e-7);
	}
	CheckPoint(measure.pointAtDistance(-5), { 0, 0 });
	CheckPoint(measure.pointAtDistance(500), { 110, 0 });
	CheckPoint(measure.pointAtDistance(335, DistanceMode::Wrap), { 5, 0 });
	CheckPoint(measure.pointAtDistance(-335, DistanceMode::Wrap), { 105, 0 });
	CHECK(measure.locationAtDistance(measure.length(), DistanceMode::Wrap) == SplineLocation{ 0, 0 });
	CHECK(measure.locationAtDistance(-measure.length(), DistanceMode::Wrap) == SplineLocation{ 0, 0 });
	CHECK_THROWS_AS(measure.distanceAt({ 5, 0 }), std::out_of_range);

	const Array<Bezier3> constant{ Straight({ 7, 9 }, { 7, 9 }) };
	const Spline2DMeasure zero{ Spline2D::FromBezierSegments(constant, CloseRing::Yes) };
	CHECK(zero.length() == 0);
	CHECK(not zero.isEmpty());
	CHECK(zero.distanceAt({ 0, 0.7 }) == 0);
	for (const double d : { -20.0, 0.0, 40.0 })
	{
		CHECK(zero.pointAtDistance(d) == Vec2{ 7, 9 });
		CHECK(zero.locationAtDistance(d, DistanceMode::Wrap) == SplineLocation{ 0, 0 });
	}
	const Spline2DMeasure empty;
	CHECK(empty.isEmpty());
	CHECK(empty.length() == 0);
	CHECK_THROWS_AS(empty.locationAtDistance(0), std::out_of_range);
	CHECK_THROWS_AS(empty.pointAtDistance(0), std::out_of_range);
	CHECK_THROWS_AS(empty.distanceAt({ 0, 0 }), std::out_of_range);
}

TEST_CASE("Spline2DMeasure.NonlinearStraightAndRetracingCurve")
{
	const Array<Bezier3> nonlinear{ Bezier3{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 100, 0 } } };
	const Spline2DMeasure measure{ Spline2D::FromBezierSegments(nonlinear) };
	CHECK(measure.spline().toLineStringAdaptive().size() == 2);
	for (const double d : { 0.0001, 0.01, 1.0, 12.5, 50.0, 99.0 })
	{
		CAPTURE(d);
		CheckPoint(measure.pointAtDistance(d), { d, 0 });
		CHECK(Abs(measure.locationAtDistance(d).t - std::cbrt(d / 100)) < 1e-6);
	}
	const Array<Bezier3> retracing{ Bezier3{ { 0, 0 }, { 100, 0 }, { -100, 0 }, { 0, 0 } } };
	const Spline2DMeasure backtrack{ Spline2D::FromBezierSegments(retracing) };
	const double extremum = (50.0 / std::sqrt(3.0));
	CHECK(Abs(backtrack.length() - 4 * extremum) < 1e-7);
	CheckPoint(backtrack.pointAtDistance(extremum), { extremum, 0 });
	CheckPoint(backtrack.pointAtDistance(2 * extremum), { 0, 0 });
	CheckPoint(backtrack.pointAtDistance(3 * extremum), { -extremum, 0 });
}

TEST_CASE("Spline2DMeasure.RoundTripOwnershipAndReverse")
{
	const Array<Vec2> points{ { 0, 0 }, { 30, 80 }, { 300, 70 }, { 310, -40 } };
	auto spline = Spline2D::FromCatmullRom(points, CloseRing::Yes);
	const Spline2DMeasure measured{ spline };
	const Spline2DMeasure reverse{ spline.reversed() };
	CHECK(Abs(measured.length() - reverse.length()) < 1e-7);
	for (size_t i = 0; i < spline.segmentCount(); ++i)
	{
		for (const double t : { 0.0, 0.01, 0.13, 0.5, 0.8, 1.0 })
		{
			const double distance = measured.distanceAt({ i, t });
			CheckPoint(measured.pointAtDistance(distance), spline.pointAt({ i, t }));
			CheckPoint(reverse.pointAtDistance(reverse.length() - distance), spline.pointAt({ i, t }));
		}
	}
	const Vec2 saved = measured.pointAtDistance(10);
	spline.clear();
	CHECK(measured.pointAtDistance(10) == saved);
	Spline2DMeasure copy = measured;
	Spline2DMeasure moved{ std::move(copy) };
	CHECK(moved.pointAtDistance(10) == saved);
	copy = moved;
	CHECK(copy.pointAtDistance(10) == saved);
	const Spline2DMeasure* alias = &copy;
	copy = *alias;
	CHECK(copy.pointAtDistance(10) == saved);
	Spline2DMeasure empty;
	empty.swap(copy);
	CHECK(copy.isEmpty());
	CHECK(empty.pointAtDistance(10) == saved);
	copy = std::move(empty);
	CHECK(copy.pointAtDistance(10) == saved);
}

TEST_CASE("Spline2DMeasure.Benchmark", "[.benchmark]")
{
	const Array<Vec2> points{ { 0, 0 }, { 30, 80 }, { 300, 70 }, { 310, -40 } };
	const auto spline = Spline2D::FromCatmullRom(points);
	const Spline2DMeasure measure{ spline };
	const Bezier3& first = spline.segment(0);
	const double length = first.computeLength();
	Vec2 reference{ 0, 0 }, cached{ 0, 0 };
	constexpr int count = 10000;
	const auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < count; ++i)
	{
		reference += first.computePointAtDistance(length * ((i % 997) + 0.5) / 997.0);
	}
	const auto middle = std::chrono::steady_clock::now();
	for (int i = 0; i < count; ++i)
	{
		cached += measure.pointAtDistance(length * ((i % 997) + 0.5) / 997.0);
	}
	const auto end = std::chrono::steady_clock::now();
	CheckPoint(cached, reference, 1e-5);
	std::printf("Spline distance queries (%d): one-shot %.3f ms, cached %.3f ms\n", count,
		std::chrono::duration<double, std::milli>(middle - start).count(),
		std::chrono::duration<double, std::milli>(end - middle).count());
}
