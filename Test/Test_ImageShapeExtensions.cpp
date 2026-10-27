// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.

# include "ImageDrawingTestHelper.hpp"
# include <functional>
# include <ThirdParty/nanobench/nanobench.h>

using namespace s3d::ImageDrawingTest;

namespace
{
	bool InRounded(const Vec2 p, const RectF rect, const std::array<double, 4>& radii)
	{
		if ((p.x < rect.x) || (rect.x + rect.w <= p.x) || (p.y < rect.y) || (rect.y + rect.h <= p.y)) return false;
		const Vec2 corners[]{ rect.tl(), rect.tr(), rect.br(), rect.bl() };
		const Vec2 signs[]{ {1, 1}, {-1, 1}, {-1, -1}, {1, -1} };
		for (size_t i = 0; i < 4; ++i)
		{
			const Vec2 q = (p - corners[i]) * signs[i];
			if ((q.x < radii[i]) && (q.y < radii[i]) && ((q - Vec2::All(radii[i])).lengthSq() > radii[i] * radii[i])) return false;
		}
		return true;
	}

	template <class Draw, class Contains>
	void CheckSamples(const Draw& draw, const Contains& contains)
	{
		for (const auto aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		{
			const Image mask = SampleMask({ 83, 67 }, aa == EnableAntialiasing::Yes, contains);
			for (const bool paint : { false, true })
				for (const Color ink : { Color{ 49, 178, 215, 127 }, Color{ 200, 30, 90, 0 }, Color{ 20, 90, 130, 255 } })
				{
					Image actual = Background(mask.size()), expected = actual;
					draw(actual, ink, paint, aa);
					CompositeMask(expected, mask, ink, paint);
					CHECK_THAT(actual, ImageEquals(expected, 1));
				}
		}
	}
}

TEST_CASE("ImageShapeExtensions.rounded_fill_and_frame", "[Image][Geometry2D]")
{
	for (const RectF rect : { RectF{ 12.3, 8.7, 51.2, 43.6 }, RectF{ -18.2, 35.7, 51.2, 43.6 } })
	{
		const std::array<double, 4> radii{ 4, 16, 0, 23 };
		CheckSamples([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
		{
			if (paint) rect.paintRounded(image, 4, 16, 0, 23, ink, aa);
			else rect.overwriteRounded(image, 4, 16, 0, 23, ink, aa);
		}, [&](Vec2 p) { return InRounded(p, rect, radii); });
		CheckSamples([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
		{
			if (paint) rect.paintRoundedFrame(image, 4, 16, 0, 23, 3, 5, ink, aa);
			else rect.overwriteRoundedFrame(image, 4, 16, 0, 23, 3, 5, ink, aa);
		}, [&](Vec2 p) { return InRounded(p, rect.stretched(5), {9, 21, 5, 28}) && !InRounded(p, rect.stretched(-3), {1, 13, 0, 20}); });
	}
}

TEST_CASE("ImageShapeExtensions.rounded_normalization_and_degeneracy", "[Image][Geometry2D]")
{
	const RectF rect{ 10.3, 7.2, 40, 30 };
	// The left side (60 + 20) sets the uniform scale 30 / 80.
	CheckSamples([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
	{
		if (paint) rect.paintRounded(image, 60, 8, -4, 20, ink, aa);
		else rect.overwriteRounded(image, 60, 8, -4, 20, ink, aa);
	}, [&](Vec2 p) { return InRounded(p, rect, {22.5, 3, 0, 7.5}); });
	CheckSamples([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
	{
		if (paint) rect.paintRoundedFrame(image, 4, 8, 0, 11, 25, 2, ink, aa);
		else rect.overwriteRoundedFrame(image, 4, 8, 0, 11, 25, 2, ink, aa);
	}, [&](Vec2 p) { return InRounded(p, rect.stretched(2), {6, 10, 2, 13}); });
	Image image = Background({ 83, 67 }); const Image original = image;
	RectF{0, 0, 0, 20}.paintRounded(image, 1, 2, 3, 4, Palette::White);
	rect.overwriteRoundedFrame(image, 1, 2, 3, 4, -1, 3, Palette::White);
	rect.paintRoundedFrame(image, 1, 2, 3, 4, 0, Palette::White);
	CHECK(image == original);
	Image empty;
	rect.paintRounded(empty, 1, 2, 3, 4, Palette::White);
	CHECK(empty.isEmpty());
}

TEST_CASE("ImageShapeExtensions.rounded_overloads", "[Image][Geometry2D]")
{
	const Rect rect{ 12, 14, 40, 30 };
	for (const auto aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
	{
		Image a = Background({ 83, 67 }), b = a;
		rect.paintRounded(a, 5, 5, 5, 5, Color{80, 110}, aa);
		RoundRect{ rect, 5 }.paint(b, Color{80, 110}, aa);
		CHECK(a == b);
		rect.overwriteRoundedFrame(a, 2, 6, 10, 0, 8, Color{110, 90}, aa);
		RectF{ rect }.overwriteRoundedFrame(b, 2, 6, 10, 0, 4, 4, Color{110, 90}, aa);
		CHECK(a == b);
		rect.paintFrame(a, 5.0, Color{80, 110});
		rect.paintFrame(b, 2.5, 2.5, Color{80, 110});
		CHECK(a == b);
	}
}

TEST_CASE("ImageShapeExtensions.balloon_union", "[Image][Geometry2D]")
{
	const RoundRect body{ RectF{ 20.3, 15.7, 35, 27 }, 5 };
	// Centered attachment on each side gives independently known roots.
	const Vec2 tips[]{ {37.8, 3.2}, {74.8, 29.2}, {37.8, 59.2}, {3.8, 29.2} };
	const Triangle tails[]{ { Vec2{32.8,15.7}, tips[0], Vec2{42.8,15.7} },
		{ Vec2{55.3,24.2}, tips[1], Vec2{55.3,34.2} },
		{ Vec2{42.8,42.7}, tips[2], Vec2{32.8,42.7} },
		{ Vec2{20.3,34.2}, tips[3], Vec2{20.3,24.2} } };
	for (size_t i = 0; i < 4; ++i)
	{
		CheckSamples([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
		{
			if (paint) body.paintBalloon(image, tips[i], 10, ink, aa);
			else body.overwriteBalloon(image, tips[i], 10, ink, aa);
		}, [&](Vec2 p) { return InRounded(p, body.rect, {5,5,5,5}) || tails[i].intersects(p); });
		Image frame{ 83, 67, Color{0,0} };
		body.paintBalloonFrame(frame, tips[i], 10, 12, Color{255,127});
		uint8 maximum = 0;
		for (Color pixel : frame) maximum = Max(maximum, pixel.a);
		CHECK(maximum == 127);
	}
	for (Vec2 target : { body.center(), Vec2{37.8, 3.2} })
	{
		Image a = Background({83,67}), b = a;
		body.paintBalloon(a, target, 0, Color{150,100}); body.paint(b, Color{150,100});
		body.overwriteBalloonFrame(a, target, 0, 6, Color{120,80}); body.overwriteFrame(b, 6, Color{120,80});
		CHECK(a == b);
	}
}

TEST_CASE("ImageShapeExtensions.rounded_benchmark", "[.benchmark][Image]")
{
	Image image{ 640, 480, Color{ 80, 120 } };
	const RectF rect{ 170.25, 130.75, 260, 170 };
	const Polygon cached = rect.rounded(12, 40, 0, 70);
	ankerl::nanobench::Bench bench;
	bench.epochs(11).warmup(20).minEpochIterations(20).unit("shape");
	bench.run("rounded.direct.paint.AA", [&] { rect.paintRounded(image, 12, 40, 0, 70, Color{130,100}); });
	bench.run("rounded.cached_polygon.paint.AA", [&] { cached.paint(image, Color{130,100}); });
	bench.run("rounded.construct_polygon.paint.AA", [&] { rect.rounded(12, 40, 0, 70).paint(image, Color{130,100}); });
	bench.run("rounded.frame.direct.paint.AA", [&] { rect.paintRoundedFrame(image, 12, 40, 0, 70, 6, Color{130,100}); });
	bench.run("rounded.frame.cached_polygon.paint.AA", [&] { cached.paintFrame(image, 6, Color{130,100}); });
	const RoundRect balloon{ rect, 24 };
	bench.run("balloon.fill.paint.AA", [&] { balloon.paintBalloon(image, {480,320}, 40, Color{130,100}); });
	bench.run("balloon.frame.paint.AA", [&] { balloon.paintBalloonFrame(image, {480,320}, 40, 6, Color{130,100}); });
}

TEST_CASE("ImageShapeExtensions.circular_shapes", "[Image][Geometry2D]")
{
	const Circle circle{ 37.3, 28.7, 21.2 };
	for (const double angle : { 0.0, 1.1, 3.4, -2.7 })
	{
		const double width = 12.3;
		const Circle cut{ circle.center + Vec2{ std::sin(angle), -std::cos(angle) } * width, circle.r };
		CheckSamples([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
		{
			if (paint) circle.paintCrescent(image, angle, width, ink, aa);
			else circle.overwriteCrescent(image, angle, width, ink, aa);
		}, [&](Vec2 p) { return circle.intersects(p) && !cut.intersects(p); });
	}
	for (const Vec2 tip : { Vec2{72.1, 14.3}, Vec2{-14.1, 59.4}, Vec2{36.8, -12.7} })
	{
		const Vec2 axis = (tip - circle.center).normalized();
		const double angle = std::acos(circle.r / tip.distanceFrom(circle.center));
		const Triangle triangle{ tip, circle.center + axis.rotated(angle) * circle.r, circle.center + axis.rotated(-angle) * circle.r };
		CheckSamples([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
		{
			if (paint) circle.paintTeardrop(image, tip, ink, aa);
			else circle.overwriteTeardrop(image, tip, ink, aa);
		}, [&](Vec2 p) { return circle.intersects(p) || triangle.intersects(p); });
	}
	for (const auto aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
	{
		Image a = Background({83, 67}), b = a;
		circle.paintCrescent(a, 1.2, 2 * circle.r, Color{40, 127}, aa);
		circle.paint(b, Color{40, 127}, aa);
		CHECK(a == b);
		circle.overwriteTeardrop(a, circle.center, Color{90, 127}, aa);
		circle.overwrite(b, Color{90, 127}, aa);
		CHECK(a == b);
		circle.paintCrescent(a, 0, 0, Palette::White, aa);
		Circle{circle.center, 0}.paintTeardrop(a, {80, 80}, Palette::White, aa);
		CHECK(a == b);
	}
}

namespace
{
	template <class Draw>
	void CheckShapeComposition(const Draw& draw)
	{
		for (const auto aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		{
			Image mask{83, 67, Color{0,0}};
			draw(mask, Palette::White, false, aa);
			for (const bool paint : { false, true })
				for (Color ink : { Color{39, 152, 210, 0}, Color{39, 152, 210, 117}, Color{39, 152, 210, 255} })
				{
					Image actual = Background(mask.size()), expected = actual;
					draw(actual, ink, paint, aa);
					CompositeMask(expected, mask, ink, paint);
					CHECK_THAT(actual, ImageEquals(expected, 1));
				}
		}
	}
}

TEST_CASE("ImageShapeExtensions.rotated_ellipse", "[Image][Geometry2D]")
{
	const Ellipse ellipse{ 32.4, 29.7, 28.2, 12.3 };
	for (const double rotation : {0.35, 1.2, -0.7})
	{
		CheckSamples([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
		{
			if (paint) ellipse.paintRotated(image, rotation, ink, aa);
			else ellipse.overwriteRotated(image, rotation, ink, aa);
		}, [&](Vec2 p) { return ellipse.intersects(ellipse.center + (p - ellipse.center).rotated(-rotation)); });
		CheckShapeComposition([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
		{
			if (paint) ellipse.paintFrameRotated(image, rotation, 4, 2, ink, aa);
			else ellipse.overwriteFrameRotated(image, rotation, 4, 2, ink, aa);
		});
	}
	for (const auto aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
	{
		Image a = Background({83,67}), b = a;
		ellipse.paintRotated(a, 0, Color{210,137}, aa); ellipse.paint(b, Color{210,137}, aa);
		CHECK(a == b);
		ellipse.overwriteFrameRotated(a, 0, 8, Color{90,137}, aa); ellipse.overwriteFrame(b, 4, 4, Color{90,137}, aa);
		CHECK(a == b);
		ellipse.paintFrameRotated(a, Math::HalfPi, 4, 2, Color{80,137}, aa);
		Ellipse{ellipse.center, ellipse.b, ellipse.a}.paintFrame(b, 4, 2, Color{80,137}, aa);
		CHECK(a == b);
		ellipse.paintFrameRotated(a, 0.7, -1, 2, Palette::White, aa);
		Ellipse{ellipse.center, 0, 10}.overwriteRotated(a, 0.7, Palette::White, aa);
		CHECK(a == b);
	}
}

TEST_CASE("ImageShapeExtensions.arc_caps_and_reversal", "[Image][Geometry2D]")
{
	const Circle circle{ 35.2, 31.7, 23.4 };
	const Ellipse ellipse{ circle.center, 29.4, 14.1 }, circular{circle.center, circle.r, circle.r};
	const Color ink{110,30,220,127};
	for (const auto aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		for (const LineCap startCap : {LineCap::Flat, LineCap::Round, LineCap::Square})
			for (const LineCap endCap : {LineCap::Flat, LineCap::Round, LineCap::Square})
				for (const double angle : { 0.0, 0.4, 4.7, -3.6, Math::TwoPi, -9.0 })
				{
					INFO("sweep=" << angle << " caps=" << static_cast<int>(startCap) << "," << static_cast<int>(endCap));
					Image a = Background({83,67}), b = a, c = a;
					circle.paintArc(a, startCap, endCap, 0.63, angle, 4, 3, ink, aa);
					circular.paintArc(b, startCap, endCap, 0.63, angle, 4, 3, ink, aa);
					CHECK(a == b);
					if ((0 < Abs(angle)) && (Abs(angle) < Math::TwoPi))
					{
						circle.paintArc(c, endCap, startCap, 0.63 + angle, -angle, 4, 3, ink, aa);
						CHECK(a == c);
					}
					else if (Math::TwoPi <= Abs(angle))
					{
						circle.paintFrame(c, 4, 3, ink, aa); CHECK(a == c);
					}
					if (startCap == endCap)
					{
						Image d = Background({83,67}); circle.paintArc(d, startCap, 0.63, angle, 4, 3, ink, aa); CHECK(a == d);
					}
					a = b = c = Background({83,67});
					ellipse.overwriteArc(a, startCap, endCap, 0.63, angle, 4, 3, ink, aa);
					if ((angle == 0) && (startCap != endCap)) b = a;
					else if (Abs(angle) < Math::TwoPi) ellipse.overwriteArc(b, endCap, startCap, 0.63 + angle, -angle, 4, 3, ink, aa);
					else ellipse.overwriteFrame(b, 4, 3, ink, aa);
					CHECK(a == b);
					if (startCap == endCap)
					{
						ellipse.overwriteArc(c, startCap, 0.63, angle, 4, 3, ink, aa); CHECK(a == c);
					}
				}
	for (double width : {0.4, 5.0, 25.0})
	{
		CheckShapeComposition([&](Image& image, Color color, bool paint, EnableAntialiasing aa)
		{
			if (paint) ellipse.paintArc(image, LineCap::Round, LineCap::Square, 0.4, 5.9, width, width, color, aa);
			else ellipse.overwriteArc(image, LineCap::Round, LineCap::Square, 0.4, 5.9, width, width, color, aa);
		});
	}
	Image a{83,67,Color{0,0}}, b = a;
	ellipse.paintArc(a, 0, Math::HalfPi, 3, 2, Palette::White);
	ellipse.paintArc(b, LineCap::Flat, LineCap::Flat, 0, Math::HalfPi, 3, 2, Palette::White);
	CHECK(a == b);
	CHECK(a[18][54].a > 0);
	CHECK(a[45][54].a == 0);
	CHECK(a[32][35].a == 0);
}

TEST_CASE("ImageShapeExtensions.straight_arrows", "[Image][Geometry2D]")
{
	const Vec2 from{10.3, 25.7}, to{70.3, 25.7};
	const LineString line{from, from, {39.1,25.7}, to, to};
	const Bezier2 q{from, from.lerp(to, 0.5), to};
	const Bezier3 c{from, from.lerp(to, 1.0/3), from.lerp(to, 2.0/3), to};
	const Spline2D spline = Spline2D::FromBezierSegments(Array<Bezier3>{c});
	const auto check = [&](const auto& path)
	{
		for (const bool doubleHead : {false,true})
			for (const LineCap cap : {LineCap::Flat, LineCap::Square, LineCap::Round})
			{
				const double width = 6.2, length = 15.3, headWidth = 26.6;
				const Vec2 endBase{to.x-length,to.y}, startBase{from.x+length,from.y};
				const Triangle endHead{to, endBase+Vec2{0,headWidth/2}, endBase-Vec2{0,headWidth/2}};
				const Triangle startHead{from, startBase-Vec2{0,headWidth/2}, startBase+Vec2{0,headWidth/2}};
				CheckSamples([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
				{
					if (doubleHead)
					{
						if (paint) path.paintDoubleHeadedArrow(image, width, {20.4,length}, ink, aa);
						else path.overwriteDoubleHeadedArrow(image, width, {20.4,length}, ink, aa);
					}
					else if (paint) path.paintArrow(image, cap, width, {20.4,length}, ink, aa);
					else path.overwriteArrow(image, cap, width, {20.4,length}, ink, aa);
				}, [&](Vec2 p)
				{
					if (endHead.intersects(p) || (doubleHead && startHead.intersects(p))) return true;
					const double left = doubleHead ? startBase.x : (cap == LineCap::Square ? from.x-width/2 : from.x);
					return ((left <= p.x) && (p.x <= endBase.x) && (Abs(p.y-from.y) <= width/2))
						|| (!doubleHead && cap == LineCap::Round && from.distanceFromSq(p) <= width*width/4);
				});
			}
	};
	check(line); check(q); check(c); check(spline);
}

TEST_CASE("ImageShapeExtensions.curve_arrow_compositing_and_limits", "[Image][Geometry2D]")
{
	const Bezier2 q{ Vec2{3.2,54.7}, Vec2{46.8,-20.3}, Vec2{77.1,53.4} };
	const Bezier3 c{ Vec2{3.2,54.7}, Vec2{90.8,-12.3}, Vec2{-15.2,2.9}, Vec2{77.1,53.4} };
	const LineString path{{4.2,8.7},{76.8,56.4},{8.8,56.4},{75.1,8.7},{75.1,8.7}};
	const auto spline = Spline2D::FromCatmullRom(path, CloseRing::Yes);
	const auto check = [&](const auto& curve)
	{
		for (const SizeF size : { SizeF{13,17}, SizeF{50,110} })
		{
			CheckShapeComposition([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
			{
				if (paint) curve.paintArrow(image, LineCap::Round, 10, size, ink, aa);
				else curve.overwriteArrow(image, LineCap::Round, 10, size, ink, aa);
			});
			CheckShapeComposition([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
			{
				if (paint) curve.paintDoubleHeadedArrow(image, 10, size, ink, aa);
				else curve.overwriteDoubleHeadedArrow(image, 10, size, ink, aa);
			});
		}
		Image a = Background({83,67}), b = a;
		curve.paintDoubleHeadedArrow(a, 5, {15,17}, Color{80,127});
		curve.reversed().paintDoubleHeadedArrow(b, 5, {15,17}, Color{80,127});
		CHECK_THAT(a, ImageEquals(b, 1));
		b = a;
		curve.paintArrow(a, 0, {15,17}, Palette::White);
		curve.overwriteDoubleHeadedArrow(a, 5, {0,17}, Palette::White);
		CHECK(a == b);
		Image empty; curve.paintArrow(empty, 5, {15,17}, Palette::White); CHECK(empty.isEmpty());
	};
	check(q); check(c); check(path); check(spline);
	Image image = Background({83,67}), original = image;
	Bezier2{{22,33},{22,33},{22,33}}.paintArrow(image, 5, {15,17}, Palette::White);
	LineString{{22,33},{22,33}}.overwriteDoubleHeadedArrow(image, 5, {15,17}, Palette::White);
	Spline2D{}.paintArrow(image, 5, {15,17}, Palette::White);
	CHECK(image == original);
	CHECK_THROWS_AS(spline.paintArrow(image, 5, {15,17}, Palette::White, EnableAntialiasing::Yes, 0.0), std::invalid_argument);
	CHECK_THROWS_AS(spline.overwriteDoubleHeadedArrow(image, 5, {15,17}, Palette::White, EnableAntialiasing::Yes, 0.5, 0), std::invalid_argument);
}

TEST_CASE("ImageShapeExtensions.arc_arrows", "[Image][Geometry2D]")
{
	const Circle circle{38.4,32.7,23.2};
	for (const double angle : {0.1, 3.5, -4.7, Math::TwoPi, -Math::TwoPi})
	{
		CheckShapeComposition([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
		{
			if (paint) circle.paintArcArrow(image, LineCap::Round, 0.3, angle, 8, {16,18}, ink, aa);
			else circle.overwriteArcArrow(image, LineCap::Round, 0.3, angle, 8, {16,18}, ink, aa);
		});
		CheckShapeComposition([&](Image& image, Color ink, bool paint, EnableAntialiasing aa)
		{
			if (paint) circle.paintArcDoubleHeadedArrow(image, 0.3, angle, 8, {16,18}, ink, aa);
			else circle.overwriteArcDoubleHeadedArrow(image, 0.3, angle, 8, {16,18}, ink, aa);
		});
	}
	Image a = Background({83,67}), b = a;
	circle.paintArcArrow(a, 0.3, Math::TwoPi, 8, {16,18}, Color{80,127});
	circle.paintArcArrow(b, LineCap::Flat, 0.3, 9, 8, {16,18}, Color{80,127});
	CHECK(a == b);
	circle.overwriteArcDoubleHeadedArrow(a, 0.3, -Math::TwoPi, 8, {16,18}, Color{30,90});
	circle.overwriteArcDoubleHeadedArrow(b, 0.3, -9, 8, {16,18}, Color{30,90});
	CHECK(a == b);
	circle.paintArcArrow(a, 0.3, 0, 8, {16,18}, Palette::White);
	CHECK(a == b);
}

TEST_CASE("ImageShapeExtensions.arrow_fallback_and_short_heads", "[Image][Geometry2D]")
{
	const Bezier2 curve{{10,50},{10,5},{75,50}};
	const LineString elbow{{10,20},{60,20},{60,27.5}};
	const auto straight = [](Vec2 a, Vec2 b) { return Bezier3::FromHermite(a,b-a,b,b-a); };
	const auto spline = Spline2D::FromBezierSegments(Array<Bezier3>{straight(elbow[0],elbow[1]),straight(elbow[1],elbow[2])});
	const auto check = [&](const auto& shape, double headLength)
	{
		for (const auto aa : {EnableAntialiasing::No,EnableAntialiasing::Yes})
		{
			Image actual = Background({83,67}), expected = actual;
			shape.paintArrow(actual,LineCap::Round,5,{0.001,headLength},Color{90,127},aa);
			shape.paint(expected,LineCap::Round,LineCap::Flat,5,Color{90,127},aa);
			CHECK(actual == expected);
		}
	};
	check(curve,40);check(elbow,25);check(spline,25);
	const LineString shortPath{{30.3,25.7},{34.3,25.7}};
	// A four-pixel path consumes the entire requested 16-pixel head length.
	const Triangle head{{34.3,25.7},{30.3,30.7},{30.3,20.7}};
	CheckSamples([&](Image& image,Color ink,bool paint,EnableAntialiasing aa)
	{
		if (paint) shortPath.paintArrow(image,5,{20,16},ink,aa);
		else shortPath.overwriteArrow(image,5,{20,16},ink,aa);
	},[&](Vec2 p){return head.intersects(p);});
}

TEST_CASE("ImageShapeExtensions.extensions_benchmark", "[.benchmark][Image]")
{
	Image image{640,480,Color{80,120}};
	const Color ink{130,80,220,100};
	const Circle circle{300.25,220.75,90};
	const Vec2 tip{440.25,120.75};
	const Shape2D crescent = Shape2D::Crescent(circle,0.7,50), teardrop = Shape2D::Teardrop(circle,tip);
	const Ellipse ellipse{300.25,220.75,130,70};
	const Polygon rotated = ellipse.asPolygon().rotatedAt(ellipse.center,0.6);
	const Bezier2 q{{160.25,290.75},{300.25,50.75},{440.25,290.75}};
	const Bezier3 c{{160.25,290.75},{440.25,80.75},{160.25,80.75},{440.25,290.75}};
	const LineString path{{160.25,150.75},{440.25,290.75},{160.25,290.75},{440.25,150.75}};
	const Spline2D spline = Spline2D::FromCatmullRom(path);
	ankerl::nanobench::Bench bench;
	bench.epochs(11).warmup(20).minEpochIterations(20).unit("shape");
	bench.run("crescent.direct.paint.AA",[&]{circle.paintCrescent(image,0.7,50,ink);});
	bench.run("crescent.cached_shape.paint.AA",[&]{crescent.paint(image,ink);});
	bench.run("crescent.construct_shape.paint.AA",[&]{Shape2D::Crescent(circle,0.7,50).paint(image,ink);});
	bench.run("teardrop.direct.paint.AA",[&]{circle.paintTeardrop(image,tip,ink);});
	bench.run("teardrop.cached_shape.paint.AA",[&]{teardrop.paint(image,ink);});
	bench.run("ellipse.rotated.direct.paint.AA",[&]{ellipse.paintRotated(image,0.6,ink);});
	bench.run("ellipse.rotated.cached_polygon.paint.AA",[&]{rotated.paint(image,ink);});
	bench.run("ellipse.rotated_frame.direct.paint.AA",[&]{ellipse.paintFrameRotated(image,0.6,6,ink);});
	bench.run("ellipse.unrotated_frame.paint.AA",[&]{ellipse.paintFrame(image,6,ink);});
	bench.run("ellipse.rotated_frame.cached_polygon.paint.AA",[&]{rotated.paintFrame(image,6,ink);});
	bench.run("ellipse.arc.Flat.paint.AA",[&]{ellipse.paintArc(image,0.4,4.5,3,3,ink);});
	bench.run("ellipse.arc.mixed.paint.AA",[&]{ellipse.paintArc(image,LineCap::Round,LineCap::Square,0.4,4.5,3,3,ink);});
	bench.run("circle.arc.Round.paint.AA",[&]{circle.paintArc(image,LineCap::Round,0.4,4.5,3,3,ink);});
	bench.run("circle.arc.mixed.paint.AA",[&]{circle.paintArc(image,LineCap::Round,LineCap::Square,0.4,4.5,3,3,ink);});
	bench.run("bezier2.arrow.paint.AA",[&]{q.paintArrow(image,6,{24,30},ink);});
	bench.run("bezier2.double_arrow.paint.AA",[&]{q.paintDoubleHeadedArrow(image,6,{24,30},ink);});
	bench.run("bezier3.arrow.paint.AA",[&]{c.paintArrow(image,6,{24,30},ink);});
	bench.run("bezier3.stroke.paint.AA",[&]{c.paint(image,6,ink);});
	bench.run("polyline.arrow.paint.AA",[&]{path.paintArrow(image,6,{24,30},ink);});
	bench.run("spline.arrow.paint.AA",[&]{spline.paintArrow(image,6,{24,30},ink);});
	bench.run("spline.double_arrow.paint.AA",[&]{spline.paintDoubleHeadedArrow(image,6,{24,30},ink);});
	bench.run("circle.arc_arrow.paint.AA",[&]{circle.paintArcArrow(image,0.4,4.5,6,{24,30},ink);});
	bench.run("circle.arc_double_arrow.paint.AA",[&]{circle.paintArcDoubleHeadedArrow(image,0.4,4.5,6,{24,30},ink);});
	bench.run("circle.full_arc_double_arrow.paint.AA",[&]{circle.paintArcDoubleHeadedArrow(image,0.4,Math::TwoPi,6,{24,30},ink);});
	bench.run("bezier3.arrow.overwrite.noAA",[&]{c.overwriteArrow(image,6,{24,30},ink,EnableAntialiasing::No);});
	bench.run("ellipse.arc.overwrite.noAA",[&]{ellipse.overwriteArc(image,0.4,4.5,3,3,ink,EnableAntialiasing::No);});
}

TEST_CASE("ImageShapeExtensions.ellipse_frame_distance_bounds", "[Image][Geometry2D]")
{
	for (const Vec2 axes : {Vec2{24,13},Vec2{26,1.2},Vec2{2.3,27},Vec2{18,18}})
		for (double rotation : {0.0,0.63,Math::HalfPi})
			for (Vec2 widths : {Vec2{0.15,0.35},Vec2{3,0},Vec2{0,2.5},Vec2{30,4}})
				for (auto aa : {EnableAntialiasing::No,EnableAntialiasing::Yes})
				{
					const Ellipse ellipse{{18.7,17.3},axes};
					Image actual{53,47,Color{0,0}}, expected = actual;
					ellipse.overwriteFrameRotated(actual,rotation,widths.x,widths.y,Palette::White,aa);
					const auto coverage = [](double distance) { return int32(Clamp(0.5-distance,0.0,1.0)*255+0.5); };
					const bool hole = (widths.x < Min(axes.x,axes.y));
					for(int32 y=0;y<expected.height();++y)for(int32 x=0;x<expected.width();++x)
					{
						const double r = (axes.x == axes.y ? 0.0 : rotation), c = std::cos(r), sn = std::sin(r);
						const double dx = x+0.5-ellipse.x, dy = y+0.5-ellipse.y;
						const Vec2 p{c*dx+sn*dy,-sn*dx+c*dy};
						// SignedDistance snaps near-boundary points to zero; Image's existing
						// non-AA contract uses the unsnapped distance and normalized-radius sign.
						const double distance = p.distanceFrom(*Geometry2D::ClosestPointOnBoundary(Ellipse{{0,0},axes},p));
						const double sd = ((p/axes).lengthSq() <= 1 ? -distance : distance);
						const int32 value = aa == EnableAntialiasing::Yes ? coverage(sd-widths.y)-(hole?coverage(sd+widths.x):0)
							: (sd <= widths.y && (!hole || -widths.x <= sd) ? 255 : 0);
						expected[y][x] = Color{value,value};
					}
					INFO("axes=" << axes << " rotation=" << rotation << " widths=" << widths);
					CHECK_THAT(actual,ImageEquals(expected));
				}
}

TEST_CASE("ImageShapeExtensions.normalized_balloon_and_clipping", "[Image][Geometry2D]")
{
	const RoundRect negative{15.3,12.7,40,30,-9};
	for (const auto aa : {EnableAntialiasing::No,EnableAntialiasing::Yes})
	{
		Image a=Background({83,67}),b=a;
		negative.paintBalloon(a,negative.center(),20,Color{80,127},aa);
		RoundRect{negative.rect,9}.paint(b,Color{80,127},aa);
		CHECK(a==b);
		negative.overwriteBalloonFrame(a,{70,60},0,8,Color{40,127},aa);
		RoundRect{negative.rect,9}.overwriteFrame(b,8,Color{40,127},aa);
		CHECK(a==b);
		const Ellipse circular{{35.3,28.7},19.4,19.4};
		circular.paintRotated(a,0.7,Color{90,127},aa);circular.paint(b,Color{90,127},aa);
		CHECK(a==b);
	}
	const auto compare=[&](const auto& draw)
	{
		for(auto aa:{EnableAntialiasing::No,EnableAntialiasing::Yes})
		{
			Image small{83,67,Color{0,0}},large{223,207,Color{0,0}};
			draw(small,Vec2{0,0},aa);draw(large,Vec2{70,70},aa);
			for(int32 y=0;y<67;++y)for(int32 x=0;x<83;++x) CHECK(small[y][x]==large[y+70][x+70]);
		}
	};
	compare([](Image& image,Vec2 offset,EnableAntialiasing aa)
	{
		Ellipse{Vec2{8.3,35.7}+offset,36.2,18.6}.paintArc(image,LineCap::Square,LineCap::Round,0.43,5.3,9,7,Color{30,127},aa);
	});
	compare([](Image& image,Vec2 offset,EnableAntialiasing aa)
	{
		Bezier3{{-20.3,80.7},{90.4,-70.2},{-30.2,25.7},{103.6,72.4}}.movedBy(offset)
			.paintDoubleHeadedArrow(image,16,{27,43},Color{30,127},aa,0.2,256);
	});
	compare([](Image& image,Vec2 offset,EnableAntialiasing aa)
	{
		RoundRect{RectF{Vec2{-12.3,12.7}+offset,70.2,35.4},11}.paintBalloon(image,Vec2{106.4,82.3}+offset,25,Color{30,127},aa);
	});
}
