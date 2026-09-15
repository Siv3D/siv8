//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# include "Siv3DTest.hpp"

namespace
{
	void CheckBuffer(const Polygon& polygon)
	{
		REQUIRE(polygon);
		CHECK(polygon.holeCount() == 0);
		CHECK(Polygon::Validate(polygon.outer()) == PolygonFailureType::Ok);
		CHECK(Geometry2D::IsClockwise(polygon.outer()));
		CHECK(polygon.outer().front() != polygon.outer().back());
		CHECK(polygon.triangleCount() == (polygon.outer().size() - 2));
		for (const auto& index : polygon.indices())
		{
			REQUIRE(index.i0 < polygon.outer().size());
			REQUIRE(index.i1 < polygon.outer().size());
			REQUIRE(index.i2 < polygon.outer().size());
			const Vec2 a = polygon.outer()[index.i0];
			const Vec2 b = polygon.outer()[index.i1];
			const Vec2 c = polygon.outer()[index.i2];
			CHECK((b - a).cross(c - a) > 0.0);
		}
		const auto parsed = Polygon::Parse(Format(polygon));
		REQUIRE(parsed);
		CHECK(parsed->outer() == polygon.outer());
	}
}

TEST_CASE("Line.buffer.nonpositive_distance")
{
	for (const Line line : { Line{ 0, 0, 10, 5 }, Line{ 3, 4, 3, 4 } })
	{
		for (const double distance : { 0.0, -1.0 })
		{
			CHECK(line.computeMiterBufferPolygon(distance).isEmpty());
			CHECK(line.computeRoundBufferPolygon(distance).isEmpty());
		}
	}
}

TEST_CASE("Line.buffer.zero_length")
{
	const Line line{ 3, 4, 3, 4 };
	for (const double radius : { 0.25, 2.0, 10.0 })
	{
		CHECK(line.computeMiterBufferPolygon(radius).isEmpty());
		for (const double quality : { 0.0, 0.25, 1.0, 3.0 })
		{
			CAPTURE(radius, quality);
			const Polygon result = line.computeRoundBufferPolygon(radius, QualityFactor{ quality });
			const Polygon circle = Circle{ line.start, radius }.asPolygon(QualityFactor{ quality });
			CheckBuffer(result);
			CHECK(result.outer() == circle.outer());
			CHECK(result.area() == circle.area());
			CHECK(result.boundingRect() == circle.boundingRect());
		}
	}
}

TEST_CASE("Line.buffer.miter")
{
	const Array<Line> lines{
		{ 0, 0, 10, 0 }, { 10, 0, 0, 0 }, { 2, 3, 2, 12 }, { 2, 12, 2, 3 },
		{ -3, 2, 4, 9 }, { -3, 2, -7, -3 }, { 0, 0, 1e-8, 0 },
	};
	for (const auto& line : lines)
	{
		for (const double distance : { 0.125, 2.5, 8.0 })
		{
			CAPTURE(line, distance);
			const Polygon result = line.computeMiterBufferPolygon(distance);
			const Quad quad = line.withThickness(distance * 2);
			CheckBuffer(result);
			CHECK(result.outer() == Array<Vec2>{ quad.p0, quad.p1, quad.p2, quad.p3 });
			CHECK(result.area() == Test::Approx(line.length() * distance * 2));
			CHECK(result.boundingRect() == quad.boundingRect());
		}
	}
}

TEST_CASE("Line.buffer.round_geometry_and_quality")
{
	const Array<Line> lines{
		{ 0, 0, 10, 0 }, { 2, 3, 2, 12 }, { -3, 2, 4, 9 }, { -3, 2, -7, -3 },
		{ 0, 0, 1e-8, 0 },
	};
	for (const auto& line : lines)
	{
		for (const double radius : { 0.25, 2.0, 2.01, 4.0, 4.01, 7.99, 8.0, 16.0 })
		{
			for (const double quality : { 0.0, 0.5, 1.0, 2.0 })
			{
				CAPTURE(line, radius, quality);
				const uint32 n = QualityFactor{ quality }.toPointsPerCircle(radius).value();
				const Polygon result = line.computeRoundBufferPolygon(radius, QualityFactor{ quality });
				CheckBuffer(result);
				CHECK(result.outer().size() == (n + 2));
				const double expectedArea = ((2 * radius * line.length()) + (n * radius * radius * std::sin(Math::TwoPi / n) * 0.5));
				CHECK(result.area() == Test::Approx(expectedArea));
				for (const auto& point : result.outer())
				{
					CHECK(line.distanceFrom(point) == Test::Approx(radius).epsilon(1e-12));
				}
			}
		}
	}
}

TEST_CASE("Line.buffer.round_reversed")
{
	const Line line{ -3, 2, 4, 9 };
	const Polygon forward = line.computeRoundBufferPolygon(4.0);
	const Polygon backward = line.reversed().computeRoundBufferPolygon(4.0);
	REQUIRE(forward.outer().size() == backward.outer().size());
	const size_t half = (forward.outer().size() / 2);
	for (size_t i = 0; i < forward.outer().size(); ++i)
	{
		CHECK(forward.outer()[(i + half) % forward.outer().size()].distanceFrom(backward.outer()[i]) < 1e-12);
	}
	CHECK(forward.area() == Test::Approx(backward.area()));
}

TEST_CASE("Line.buffer.round_quality_limit")
{
	const Line line{ 0, 0, 10, 0 };
	const Polygon result = line.computeRoundBufferPolygon(16.0, QualityFactor{ 1000.0 });
	CheckBuffer(result);
	CHECK(result.outer().size() == 254);
}
