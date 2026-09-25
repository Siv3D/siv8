//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <cmath>
# include <limits>

namespace
{
	void CheckMapping(const Mat3x3& matrix, const Quad& from, const Quad& to, const double tolerance = 1e-3)
	{
		const Vec2 sources[]{ from.p0, from.p1, from.p2, from.p3 };
		const Vec2 targets[]{ to.p0, to.p1, to.p2, to.p3 };
		for (size_t i = 0; i < 4; ++i)
		{
			const Vec2 actual = matrix.transformPoint(sources[i]);
			CHECK(std::abs(actual.x - targets[i].x) <= tolerance);
			CHECK(std::abs(actual.y - targets[i].y) <= tolerance);
		}
	}
}

TEST_CASE("Mat3x3.composition")
{
	constexpr Mat3x2 affine{ 2, 1, -1, 3, 10, -5 };
	constexpr Mat3x3 a{ affine };
	constexpr Mat3x3 b{ 1, 0, 0.01f, 0, 1, 0.02f, 0, 0, 1 };
	static_assert(a * Mat3x3::Identity() == a);
	static_assert(Mat3x3::Identity() * a == a);
	const Vec2 p{ 20, 30 };
	CHECK(a.transformPoint(p) == affine.transformPoint(p));
	CHECK((a * b).transformPoint(p).distanceFrom(b.transformPoint(a.transformPoint(p))) < 1e-5);
	CHECK((a * b).inverse().transformPoint((a * b).transformPoint(p)).distanceFrom(p) < 1e-4);
}

TEST_CASE("Mat3x3.Homography")
{
	const RectF rect{ 20, 30, 200, 100 };
	const Quad source = rect.asQuad();
	const Quad target{ 10, 40, 240, 20, 200, 190, 40, 160 };
	const auto h = Mat3x3::TryHomography(source, target);
	REQUIRE(h);
	CheckMapping(*h, source, target);
	CheckMapping(Mat3x3::Homography(source, target), source, target);
	CheckMapping(Mat3x3::Homography(rect, target), source, target);
	CheckMapping(Mat3x3::Homography(target, rect), target, source);
	CheckMapping(Mat3x3::Homography(rect, rect), source, source);
	CheckMapping(Mat3x3::Homography(target), RectF{ 0, 0, 1, 1 }.asQuad(), target);
	const std::array<Float2, 4> points{ target.p0, target.p1, target.p2, target.p3 };
	CheckMapping(Mat3x3::Homography(points), RectF{ 0, 0, 1, 1 }.asQuad(), target);
	REQUIRE(Mat3x3::TryHomography(points));
	REQUIRE(Mat3x3::TryHomography(target));
	REQUIRE(Mat3x3::TryHomography(rect, rect));
	REQUIRE(Mat3x3::TryHomography(rect, target));
	const auto inverse = Mat3x3::TryHomography(target, rect);
	REQUIRE(inverse);
	for (const Vec2 p : { Vec2{ 20, 30 }, Vec2{ 100, 70 }, Vec2{ 220, 130 } })
	{
		CHECK(inverse->transformPoint(h->transformPoint(p)).distanceFrom(p) < 1e-3);
	}

	const Quad reflected{ target.p1, target.p0, target.p3, target.p2 };
	const auto reflection = Mat3x3::TryHomography(source, reflected);
	REQUIRE(reflection);
	CheckMapping(*reflection, source, reflected);
	const auto clockwiseSource = Mat3x3::TryHomography(reflected, target);
	REQUIRE(clockwiseSource);
	CheckMapping(*clockwiseSource, reflected, target);

	// The unchecked factory retains general four-point projective mappings.
	const Quad concave{ 0, 0, 100, 0, 25, 25, 0, 100 };
	CheckMapping(Mat3x3::Homography(source, concave), source, concave);
}

TEST_CASE("Mat3x3.Homography_scale_and_zero_m33")
{
	for (const double scale : { 1e-12, 1.0, 1e12 })
	{
		const Quad from{ Vec2{ 2, 3 } * scale, Vec2{ 8, 3 } * scale, Vec2{ 8, 7 } * scale, Vec2{ 2, 7 } * scale };
		const Quad to{ Vec2{ -1, 2 } * scale, Vec2{ 10, 1 } * scale, Vec2{ 9, 8 } * scale, Vec2{ 0, 6 } * scale };
		const auto h = Mat3x3::TryHomography(from, to);
		REQUIRE(h);
		CheckMapping(*h, from, to, scale * 1e-4);
	}
	const Quad source = RectF{ 2, 1, 2, 2 }.asQuad();
	// x' = 1/x, y' = y/x. The origin is outside the source; _33 is exactly zero.
	const Mat3x3 expected{ 0, 0, 1, 0, 1, 0, 1, 0, 0 };
	const Quad target{ expected.transformPoint(source.p0), expected.transformPoint(source.p1),
		expected.transformPoint(source.p2), expected.transformPoint(source.p3) };
	const auto h = Mat3x3::TryHomography(source, target);
	REQUIRE(h);
	CHECK(std::abs(h->_33) < 1e-6);
	CheckMapping(*h, source, target);
	for (const Vec2 p : { source.p0, source.p1, source.p2, source.p3 })
	{
		CHECK((h->_13 * p.x + h->_23 * p.y + h->_33) > 0);
	}
}

TEST_CASE("Mat3x3.TryHomography_invalid")
{
	const Quad valid = RectF{ 0, 0, 100, 100 }.asQuad();
	const double nan = std::numeric_limits<double>::quiet_NaN();
	const double inf = std::numeric_limits<double>::infinity();
	for (const Quad invalid : {
		Quad{ 0, 0, 0, 0, 100, 100, 0, 100 },
		Quad{ 0, 0, 50, 0, 100, 0, 0, 100 },
		Quad{ 0, 0, 100, 100, 100, 0, 0, 100 },
		Quad{ 0, 0, 100, 0, 25, 25, 0, 100 },
		Quad{ 0, 0, 100, 0, 100, nan, 0, 100 },
		Quad{ inf, 0, 100, 0, 100, 100, 0, 100 },
		RectF{ 0, 0, 100, 1e-15 }.asQuad(),
		RectF{ 0, 0, 0, 100 }.asQuad() })
	{
		CHECK_FALSE(Mat3x3::TryHomography(valid, invalid));
		CHECK_FALSE(Mat3x3::TryHomography(invalid, valid));
	}
	// All corners are finite doubles, but their distinction is lost in the float matrix.
	CHECK_FALSE(Mat3x3::TryHomography(valid, RectF{ 1e15, 1e15, 1, 1 }.asQuad()));
}
