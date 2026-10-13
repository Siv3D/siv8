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

# include "Siv3DTest.hpp"
# include <ThirdParty/nanobench/nanobench.h>
# include <functional>

namespace
{
	using Draw = std::function<void(Image&, Color, bool, EnableAntialiasing, Vec2)>;
	struct Drawing
	{
		const char* name;
		Draw draw;
	};

	Array<Drawing> Drawings()
	{
		return {
			{ "Point", [](Image& image, Color color, bool paint, EnableAntialiasing, Vec2 offset)
				{
					const auto shape = Point{ 11, 12 } + offset.asPoint();
					if (paint) shape.paint(image, color);
					else shape.overwrite(image, color);
				} },
			{ "Vec2", [](Image& image, Color color, bool paint, EnableAntialiasing, Vec2 offset)
				{
					const auto shape = Vec2{ 11.3, 12.7 } + offset;
					if (paint) shape.paint(image, color);
					else shape.overwrite(image, color);
				} },
			{ "Float2", [](Image& image, Color color, bool paint, EnableAntialiasing, Vec2 offset)
				{
					const auto shape = Float2{ 11.3f, 12.7f } + Float2{ offset };
					if (paint) shape.paint(image, color);
					else shape.overwrite(image, color);
				} },
			{ "Rect", [](Image& image, Color color, bool paint, EnableAntialiasing, Vec2 offset)
				{
					const auto shape = Rect{ 5, 6, 24, 18 }.movedBy(offset.asPoint());
					if (paint) shape.paint(image, color);
					else shape.overwrite(image, color);
				} },
			{ "Rect.frame", [](Image& image, Color color, bool paint, EnableAntialiasing, Vec2 offset)
				{
					const auto shape = Rect{ 5, 6, 24, 18 }.movedBy(offset.asPoint());
					if (paint) shape.paintFrame(image, 2, 3, color);
					else shape.overwriteFrame(image, 2, 3, color);
				} },
			{ "RectF", [](Image& image, Color color, bool paint, EnableAntialiasing, Vec2 offset)
				{
					const auto shape = RectF{ 5.25, 6.75, 24.5, 18.25 }.movedBy(offset);
					if (paint) shape.paint(image, color);
					else shape.overwrite(image, color);
				} },
			{ "RectF.frame", [](Image& image, Color color, bool paint, EnableAntialiasing, Vec2 offset)
				{
					const auto shape = RectF{ 5.25, 6.75, 24.5, 18.25 }.movedBy(offset);
					if (paint) shape.paintFrame(image, 2, 3, color);
					else shape.overwriteFrame(image, 2, 3, color);
				} },
			{ "Circle", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Circle{ 17.25, 15.75, 11.5 }.movedBy(offset);
					if (paint) shape.paint(image, color, aa);
					else shape.overwrite(image, color, aa);
				} },
			{ "Circle.frame", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Circle{ 17.25, 15.75, 11.5 }.movedBy(offset);
					if (paint) shape.paintFrame(image, 4.0, color, aa);
					else shape.overwriteFrame(image, 4.0, color, aa);
				} },
			{ "SuperEllipse.n=0.5", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const SuperEllipse shape{ Vec2{ 17.25, 15.75 } + offset, 13, 9, 0.5 };
					if (paint) shape.paint(image, color, aa);
					else shape.overwrite(image, color, aa);
				} },
			{ "SuperEllipse.n=1.0", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const SuperEllipse shape{ Vec2{ 17.25, 15.75 } + offset, 13, 9, 1.0 };
					if (paint) shape.paint(image, color, aa);
					else shape.overwrite(image, color, aa);
				} },
			{ "SuperEllipse.n=2.0", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const SuperEllipse shape{ Vec2{ 17.25, 15.75 } + offset, 13, 9, 2.0 };
					if (paint) shape.paint(image, color, aa);
					else shape.overwrite(image, color, aa);
				} },
			{ "SuperEllipse.n=4.0", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const SuperEllipse shape{ Vec2{ 17.25, 15.75 } + offset, 13, 9, 4.0 };
					if (paint) shape.paint(image, color, aa);
					else shape.overwrite(image, color, aa);
				} },
			{ "SuperEllipse.n=16.0", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const SuperEllipse shape{ Vec2{ 17.25, 15.75 } + offset, 13, 9, 16.0 };
					if (paint) shape.paint(image, color, aa);
					else shape.overwrite(image, color, aa);
				} },
			{ "Ellipse", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Ellipse{ 17.25, 15.75, 13, 7 }.movedBy(offset);
					if (paint) shape.paint(image, color, aa);
					else shape.overwrite(image, color, aa);
				} },
			{ "Ellipse.frame", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Ellipse{ 17.25, 15.75, 13, 7 }.movedBy(offset);
					if (paint) shape.paintFrame(image, 4.0, color, aa);
					else shape.overwriteFrame(image, 4.0, color, aa);
				} },
			{ "RoundRect", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = RoundRect{ 5.25, 6.75, 24.5, 18.25, 5.5 }.movedBy(offset);
					if (paint) shape.paint(image, color, aa);
					else shape.overwrite(image, color, aa);
				} },
			{ "RoundRect.frame", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = RoundRect{ 5.25, 6.75, 24.5, 18.25, 5.5 }.movedBy(offset);
					if (paint) shape.paintFrame(image, 4.0, color, aa);
					else shape.overwriteFrame(image, 4.0, color, aa);
				} },
			{ "Triangle", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Triangle{ 3.25, 4.75, 30.5, 7.25, 17.75, 27.5 }.movedBy(offset);
					if (paint) shape.paint(image, color, aa);
					else shape.overwrite(image, color, aa);
				} },
			{ "Triangle.frame", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Triangle{ 3.25, 4.75, 30.5, 7.25, 17.75, 27.5 }.movedBy(offset);
					if (paint) shape.paintFrame(image, 4.0, color, aa);
					else shape.overwriteFrame(image, 4.0, color, aa);
				} },
			{ "Quad", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Quad{ 3.25, 4.75, 30.5, 7.25, 28.75, 27.5, 6.5, 24.75 }.movedBy(offset);
					if (paint) shape.paint(image, color, aa);
					else shape.overwrite(image, color, aa);
				} },
			{ "Quad.frame", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Quad{ 3.25, 4.75, 30.5, 7.25, 28.75, 27.5, 6.5, 24.75 }.movedBy(offset);
					if (paint) shape.paintFrame(image, 4.0, color, aa);
					else shape.overwriteFrame(image, 4.0, color, aa);
				} },
			{ "Polygon", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Polygon{ { Vec2{ 3.25, 4.75 }, Vec2{ 30.5, 7.25 }, Vec2{ 20.75, 14.5 }, Vec2{ 28.75, 27.5 }, Vec2{ 6.5, 24.75 } } }.movedBy(offset);
					if (paint) shape.paint(image, color, aa);
					else shape.overwrite(image, color, aa);
				} },
			{ "Polygon.frame", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Polygon{ { Vec2{ 3.25, 4.75 }, Vec2{ 30.5, 7.25 }, Vec2{ 20.75, 14.5 }, Vec2{ 28.75, 27.5 }, Vec2{ 6.5, 24.75 } } }.movedBy(offset);
					if (paint) shape.paintFrame(image, 4.0, color, aa);
					else shape.overwriteFrame(image, 4.0, color, aa);
				} },
			{ "Shape2D", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Shape2D::Star(13, Vec2{ 18, 16 } + offset);
					if (paint) shape.paint(image, color, aa);
					else shape.overwrite(image, color, aa);
				} },
			{ "Shape2D.frame", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Shape2D::Star(13, Vec2{ 18, 16 } + offset);
					if (paint) shape.paintFrame(image, 4.0, color, aa);
					else shape.overwriteFrame(image, 4.0, color, aa);
				} },
			{ "MultiPolygon", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = MultiPolygon{ RectF{ 3, 4, 10, 20 }.movedBy(offset).asPolygon(), RectF{ 22, 6, 10, 18 }.movedBy(offset).asPolygon() };
					if (paint) shape.paint(image, color, aa);
					else shape.overwrite(image, color, aa);
				} },
			{ "MultiPolygon.frame", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = MultiPolygon{ RectF{ 3, 4, 10, 20 }.movedBy(offset).asPolygon(), RectF{ 22, 6, 10, 18 }.movedBy(offset).asPolygon() };
					if (paint) shape.paintFrame(image, 4.0, color, aa);
					else shape.overwriteFrame(image, 4.0, color, aa);
				} },
			{ "Line.Square", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Line{ 4.25, 6.75, 30.5, 24.5 }.movedBy(offset);
					if (paint) shape.paint(image, LineCap::Square, 5.0, color, aa);
					else shape.overwrite(image, LineCap::Square, 5.0, color, aa);
				} },
			{ "Line.Round", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Line{ 4.25, 6.75, 30.5, 24.5 }.movedBy(offset);
					if (paint) shape.paint(image, LineCap::Round, 5.0, color, aa);
					else shape.overwrite(image, LineCap::Round, 5.0, color, aa);
				} },
			{ "Line.Flat", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Line{ 4.25, 6.75, 30.5, 24.5 }.movedBy(offset);
					if (paint) shape.paint(image, LineCap::Flat, 5.0, color, aa);
					else shape.overwrite(image, LineCap::Flat, 5.0, color, aa);
				} },
			{ "Line.arrow", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Line{ 4.25, 6.75, 30.5, 24.5 }.movedBy(offset);
					if (paint) shape.paintArrow(image, 5.0, 10.0, color, aa);
					else shape.overwriteArrow(image, 5.0, 10.0, color, aa);
				} },
			{ "Line.doubleArrow", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Line{ 4.25, 6.75, 30.5, 24.5 }.movedBy(offset);
					if (paint) shape.paintDoubleHeadedArrow(image, 5.0, 10.0, color, aa);
					else shape.overwriteDoubleHeadedArrow(image, 5.0, 10.0, color, aa);
				} },
			{ "LineString.Square", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = LineString{ Vec2{ 4, 6 } + offset, Vec2{ 28, 7 } + offset, Vec2{ 10, 24 } + offset, Vec2{ 26, 24 } + offset };
					if (paint) shape.paint(image, LineCap::Square, 5.0, color, aa);
					else shape.overwrite(image, LineCap::Square, 5.0, color, aa);
				} },
			{ "LineString.Round", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = LineString{ Vec2{ 4, 6 } + offset, Vec2{ 28, 7 } + offset, Vec2{ 10, 24 } + offset, Vec2{ 26, 24 } + offset };
					if (paint) shape.paint(image, LineCap::Round, 5.0, color, aa);
					else shape.overwrite(image, LineCap::Round, 5.0, color, aa);
				} },
			{ "LineString.Flat", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = LineString{ Vec2{ 4, 6 } + offset, Vec2{ 28, 7 } + offset, Vec2{ 10, 24 } + offset, Vec2{ 26, 24 } + offset };
					if (paint) shape.paint(image, LineCap::Flat, 5.0, color, aa);
					else shape.overwrite(image, LineCap::Flat, 5.0, color, aa);
				} },
			{ "LineString.closed", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = LineString{ Vec2{ 4, 6 } + offset, Vec2{ 28, 7 } + offset, Vec2{ 10, 24 } + offset, Vec2{ 26, 24 } + offset };
					if (paint) shape.paintClosed(image, 5.0, color, aa);
					else shape.overwriteClosed(image, 5.0, color, aa);
				} },
			{ "Bezier2.Square", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Bezier2{ Vec2{ 4, 6 } + offset, Vec2{ 29, 3 } + offset, Vec2{ 25, 25 } + offset };
					if (paint) shape.paint(image, LineCap::Square, 5.0, color, aa);
					else shape.overwrite(image, LineCap::Square, 5.0, color, aa);
				} },
			{ "Bezier2.Round", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Bezier2{ Vec2{ 4, 6 } + offset, Vec2{ 29, 3 } + offset, Vec2{ 25, 25 } + offset };
					if (paint) shape.paint(image, LineCap::Round, 5.0, color, aa);
					else shape.overwrite(image, LineCap::Round, 5.0, color, aa);
				} },
			{ "Bezier2.Flat", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Bezier2{ Vec2{ 4, 6 } + offset, Vec2{ 29, 3 } + offset, Vec2{ 25, 25 } + offset };
					if (paint) shape.paint(image, LineCap::Flat, 5.0, color, aa);
					else shape.overwrite(image, LineCap::Flat, 5.0, color, aa);
				} },
			{ "Bezier3.Square", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Bezier3{ Vec2{ 4, 6 } + offset, Vec2{ 29, 3 } + offset, Vec2{ 3, 25 } + offset, Vec2{ 29, 25 } + offset };
					if (paint) shape.paint(image, LineCap::Square, 5.0, color, aa);
					else shape.overwrite(image, LineCap::Square, 5.0, color, aa);
				} },
			{ "Bezier3.Round", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Bezier3{ Vec2{ 4, 6 } + offset, Vec2{ 29, 3 } + offset, Vec2{ 3, 25 } + offset, Vec2{ 29, 25 } + offset };
					if (paint) shape.paint(image, LineCap::Round, 5.0, color, aa);
					else shape.overwrite(image, LineCap::Round, 5.0, color, aa);
				} },
			{ "Bezier3.Flat", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Bezier3{ Vec2{ 4, 6 } + offset, Vec2{ 29, 3 } + offset, Vec2{ 3, 25 } + offset, Vec2{ 29, 25 } + offset };
					if (paint) shape.paint(image, LineCap::Flat, 5.0, color, aa);
					else shape.overwrite(image, LineCap::Flat, 5.0, color, aa);
				} },
			{ "Circle.pie", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Circle{ 17.25, 15.75, 11.5 }.movedBy(offset);
					if (paint) shape.paintPie(image, 0.3, 4.5, color, aa);
					else shape.overwritePie(image, 0.3, 4.5, color, aa);
				} },
			{ "Circle.arc", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Circle{ 17.25, 15.75, 11.5 }.movedBy(offset);
					if (paint) shape.paintArc(image, 0.3, 4.5, 2.0, 3.0, color, aa);
					else shape.overwriteArc(image, 0.3, 4.5, 2.0, 3.0, color, aa);
				} }
		};
	}

	Image Background(const Size size)
	{
		Image image{ size };
		size_t i = 0;
		for (auto& pixel : image)
		{
			pixel = Color{ uint8(i * 73), uint8(i * 31), uint8(i * 17), uint8(i * 23) };
			++i;
		}
		return image;
	}

	// Independent scalar straight-alpha oracle, including the specified byte rounding.
	Color Composite(Color src, Color dst, uint8 coverage, bool paint)
	{
		if (!paint)
		{
			const auto channel = [&](uint32 s, uint32 d) { return (s * coverage + d * (255 - coverage) + 127) / 255; };
			return Color{ channel(src.r, dst.r), channel(src.g, dst.g), channel(src.b, dst.b), channel(src.a, dst.a) };
		}
		const uint32 a = (src.a * coverage + 127) / 255;
		if (a == 0) return dst;
		const uint32 denominator = a * 255 + dst.a * (255 - a);
		const auto channel = [&](uint32 s, uint32 d) { return (s * a * 255 + d * dst.a * (255 - a) + denominator / 2) / denominator; };
		return Color{ channel(src.r, dst.r), channel(src.g, dst.g), channel(src.b, dst.b), (denominator + 127) / 255 };
	}

	bool Inside(const Vec2 p, const Array<Array<Vec2>>& contours)
	{
		bool inside = false;
		for (const auto& points : contours)
		{
			for (size_t i = 0, j = points.size() - 1; i < points.size(); j = i++)
			{
				const Vec2 a = points[i], b = points[j];
				if (((a.y > p.y) != (b.y > p.y)) && (p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x)) inside = !inside;
			}
		}
		return inside;
	}

	Image PolygonMask(const Size size, const Array<Array<Vec2>>& contours, const bool aa)
	{
		Image image{ size, Color{ 0, 0 } };
		const int32 samples = aa ? 4 : 1;
		for (int32 y = 0; y < size.y; ++y)
		{
			for (int32 x = 0; x < size.x; ++x)
			{
				int32 count = 0;
				for (int32 sy = 0; sy < samples; ++sy)
					for (int32 sx = 0; sx < samples; ++sx)
						count += Inside(Vec2{ x + (sx + 0.5) / samples, y + (sy + 0.5) / samples }, contours);
				const uint8 c = uint8((count * 255 + samples * samples / 2) / (samples * samples));
				image[y][x] = Color{ c, c };
			}
		}
		return image;
	}
}

TEST_CASE("ImageDraw.shape_compositing_and_empty_destinations")
{
	for (const auto& drawing : Drawings())
	{
		CAPTURE(drawing.name);
		for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		{
			CAPTURE(static_cast<bool>(aa));
			Image mask{ 37, 31, Color{ 0, 0 } };
			drawing.draw(mask, Palette::White, false, aa, Vec2{ 0, 0 });
			REQUIRE(std::any_of(mask.begin(), mask.end(), [](Color c) { return c.a != 0; }));
			for (const bool paint : { false, true })
			{
				CAPTURE(paint);
				for (const uint32 alpha : { 0u, 113u, 255u })
				{
					CAPTURE(alpha);
					const Color color{ 203, 47, 119, alpha };
					const Image before = Background(mask.size());
					Image expected = before, actual = before;
					for (size_t i = 0; i < mask.size_bytes() / sizeof(Color); ++i)
						expected.data()[i] = Composite(color, before.data()[i], mask.data()[i].a, paint);
					drawing.draw(actual, color, paint, aa, Vec2{ 0, 0 });
					CHECK((actual == expected));
					Image empty;
					drawing.draw(empty, color, paint, aa, Vec2{ 0, 0 });
					CHECK(empty.isEmpty());
				}
			}
		}
	}
}

TEST_CASE("ImageDraw.clipping_matches_translated_larger_canvas")
{
	for (const auto& drawing : Drawings())
	{
		CAPTURE(drawing.name);
		for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
			for (const bool paint : { false, true })
				for (const Vec2 offset : { Vec2{ -15, -12 }, Vec2{ 20, 19 }, Vec2{ -100, -100 } })
				{
					const Color background{ 30, 70, 110, 61 }, color{ 200, 50, 80, 127 };
					Image clipped{ 37, 31, background }, large{ 101, 95, background };
					drawing.draw(clipped, color, paint, aa, offset);
					drawing.draw(large, color, paint, aa, offset + Vec2{ 32, 32 });
					bool equal = true;
					for (int32 y = 0; y < 31; ++y)
						for (int32 x = 0; x < 37; ++x)
							equal &= (clipped[y][x] == large[y + 32][x + 32]);
					CHECK(equal);
				}
	}
}

TEST_CASE("ImageDraw.polygon_sampling_winding_holes_and_crossings")
{
	const Array<Array<Vec2>> shapes{
		{ { 2, 3 }, { 27, 3 }, { 27, 23 }, { 2, 23 } },
		{ { -4.25, 2.75 }, { 28.125, 5.875 }, { 8.5, 10.25 }, { 23.75, 27.5 }, { 1.25, 20.75 } },
		{ { 3.125, 2.875 }, { 27.875, 25.125 }, { 3.125, 25.125 }, { 27.875, 2.875 } },
		{ { 2.5, 2.5 }, { 28.5, 2.5 }, { 13.5, 25.5 } },
		{ { 5, 6 }, { 5, 6 }, { 20, 6 }, { 20, 20 }, { 5, 20 }, { 5, 6 } }
	};
	for (const auto& points : shapes)
		for (const bool aa : { false, true })
		{
			const auto antialiasing = EnableAntialiasing{ aa };
			const Image expected = PolygonMask(Size{ 32, 29 }, { points }, aa);
			Image actual{ 32, 29, Color{ 0, 0 } };
			ImageDraw::FillPolygon(actual, points, Palette::White, ImagePixel::BlendMode::Overwrite, antialiasing);
			CHECK((actual == expected));
			actual.fill(Color{ 0, 0 });
			ImageDraw::FillPolygon(actual, points.reversed(), Palette::White, ImagePixel::BlendMode::Overwrite, antialiasing);
			CHECK((actual == expected));
		}
	const Array<Vec2> outer{ { -3, -2 }, { 30, -2 }, { 30, 28 }, { -3, 28 } };
	const Array<Vec2> hole{ { 7.25, 6.25 }, { 7.25, 19.75 }, { 22.75, 19.75 }, { 22.75, 6.25 } };
	const Polygon polygon{ outer, { hole } };
	REQUIRE(polygon);
	for (const bool aa : { false, true })
	{
		Image actual{ 32, 29, Color{ 0, 0 } };
		polygon.overwrite(actual, Palette::White, EnableAntialiasing{ aa });
		CHECK(actual == PolygonMask(actual.size(), { outer, hole }, aa));
	}
}

TEST_CASE("ImageDraw.ellipse_frame_matches_boundary_distance")
{
	for (const Vec2 axes : { Vec2{ 13, 2 }, Vec2{ 2, 13 }, Vec2{ 12, 7 } })
	{
		const Ellipse ellipse{ Vec2{ 16.5, 16.5 }, axes };
		Image image{ 33, 33, Color{ 0, 0 } };
		ellipse.overwriteFrame(image, 1.25, 0.75, Palette::White);
		for (int32 y = 0; y < 33; ++y)
			for (int32 x = 0; x < 33; ++x)
			{
				const double sd = Geometry2D::SignedDistance(ellipse, Vec2{ x + 0.5, y + 0.5 });
				const auto coverage = [](double d) { return int32(Clamp(0.5 - d, 0.0, 1.0) * 255 + 0.5); };
				const int32 expected = coverage(sd - 0.75) - coverage(sd + 1.25);
				if (Abs(int32(image[y][x].a) - expected) > 1)
				{
					CAPTURE(axes, x, y, sd, expected, image[y][x]);
					FAIL_CHECK("Ellipse frame differs from its boundary-distance contract");
					break;
				}
			}
	}
}

TEST_CASE("ImageDraw.benchmark", "[.benchmark]")
{
	using ankerl::nanobench::doNotOptimizeAway;
	ankerl::nanobench::Bench bench;
	bench.title("ImageDraw public API").unit("draw").epochs(7).minEpochIterations(2);
	for (const int32 size : { 32, 256, 1024 })
	{
		Image image{ size, Color{ 40, 70, 100, 255 } };
		const double r = size * 0.4;
		const Vec2 center{ size * 0.5, size * 0.5 };
		const Polygon polygon = Shape2D::Star(r, center).toPolygon();
		const Ellipse ellipse{ center, r, r * 0.6 };
		const Circle circle{ center, r };
		const SuperEllipse superEllipse{ center, r, r * 0.6, 4 };
		const LineString line = polygon.outline();
		for (const bool aa : { false, true })
			for (const bool paint : { false, true })
			{
				const Color color{ 210, 60, 120, 113 };
				const std::string suffix = "/" + std::to_string(size) + (aa ? "/AA" : "/NoAA") + (paint ? "/paint" : "/overwrite");
				const auto run = [&](const char* name, auto&& draw)
				{
					bench.run(name + suffix, [&] { draw(); doNotOptimizeAway(image.data()); });
					CHECK(std::any_of(image.begin(), image.end(), [](Color c) { return c.r != 40; }));
				};
				run("Polygon", [&] { if (paint) polygon.paint(image, color, EnableAntialiasing{ aa }); else polygon.overwrite(image, color, EnableAntialiasing{ aa }); });
				run("SuperEllipse", [&] { if (paint) superEllipse.paint(image, color, EnableAntialiasing{ aa }); else superEllipse.overwrite(image, color, EnableAntialiasing{ aa }); });
				run("Ellipse", [&] { if (paint) ellipse.paint(image, color, EnableAntialiasing{ aa }); else ellipse.overwrite(image, color, EnableAntialiasing{ aa }); });
				run("Circle", [&] { if (paint) circle.paint(image, color, EnableAntialiasing{ aa }); else circle.overwrite(image, color, EnableAntialiasing{ aa }); });
				run("LineString", [&] { if (paint) line.paint(image, 3.5, color, EnableAntialiasing{ aa }); else line.overwrite(image, 3.5, color, EnableAntialiasing{ aa }); });
			}
	}
}

TEST_CASE("ImageDraw.circle_integer_mask_is_composited_once")
{
	for (const int32 radius : { 1, 2, 3, 5, 12, 19 })
	{
		// The historical integer raster mask, independently accumulated without blending.
		Image mask{ 43, 43, Color{ 0, 0 } };
		const auto span = [&](int32 y, int32 x0, int32 x1)
		{
			for (int32 x = x0; x <= x1; ++x) mask[y][x] = Palette::White;
		};
		int32 x = radius, y = 0, error = 0, plus = 1, minus = radius * 2 - 1;
		while (x >= y)
		{
			span(21 - y, 21 - x, 21 + x);
			span(21 + y, 21 - x, 21 + x);
			span(21 - x, 21 - y, 21 + y);
			span(21 + x, 21 - y, 21 + y);
			++y;
			error += plus;
			plus += 2;
			if (error > 0) { error -= minus; --x; minus -= 2; }
		}
		Image actual{ mask.size(), Color{ 0, 0 } };
		Circle{ 21.2, 20.8, radius + 0.2 }.paint(actual, Color{ 255, 113 }, EnableAntialiasing::No);
		for (size_t i = 0; i < actual.size_bytes() / sizeof(Color); ++i)
			mask.data()[i] = mask.data()[i].a ? Color{ 255, 113 } : Color{ 0, 0 };
		CHECK((actual == mask));
	}
}

TEST_CASE("ImageDraw.small_curves_match_distance_coverage")
{
	const auto coverage = [](double sd) { return uint8(Clamp(0.5 - sd, 0.0, 1.0) * 255 + 0.5); };
	for (const double radius : { 0.2, 0.5, 1.0, 2.75, 7.5 })
	{
		const Circle circle{ 10.25, 10.75, radius };
		Image actual{ 23, 23, Color{ 0, 0 } }, expected = actual;
		circle.overwrite(actual, Palette::White);
		for (int32 y = 0; y < 23; ++y)
			for (int32 x = 0; x < 23; ++x)
			{
				const auto c = coverage(Vec2{ x + 0.5, y + 0.5 }.distanceFrom(circle.center) - radius);
				expected[y][x] = Color{ c, c };
			}
		CHECK((actual == expected));
	}
	for (const RectF rect : { RectF{ 5.25, 6.75, 12, 9 }, RectF{ 10.2, 9.7, 0.6, 0.4 }, RectF{ 4.2, 6.8, 12, 0.6 } })
		for (const double radius : { 0.1, 2.0, 50.0 })
		{
			CAPTURE(rect, radius);
			const RoundRect shape{ rect, radius };
			Image actual{ 23, 23, Color{ 0, 0 } }, expected = actual;
			shape.overwrite(actual, Palette::White);
			const double r = Min({ radius, rect.w * 0.5, rect.h * 0.5 });
			for (int32 y = 0; y < 23; ++y)
				for (int32 x = 0; x < 23; ++x)
				{
					const Vec2 delta = Vec2{ x + 0.5, y + 0.5 } - rect.center();
					const Vec2 q{ Abs(delta.x) - rect.w * 0.5 + r, Abs(delta.y) - rect.h * 0.5 + r };
					const auto c = coverage(Vec2{ Max(q.x, 0.0), Max(q.y, 0.0) }.length() + Min(Max(q.x, q.y), 0.0) - r);
					expected[y][x] = Color{ c, c };
				}
			CHECK((actual == expected));
		}
}

TEST_CASE("ImageDraw.degenerate_shapes_and_nonpositive_strokes")
{
	const Color bg{ 20, 40, 60, 73 }, color{ 210, 80, 40, 117 };
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
	{
		Image image{ 31, 27, bg };
		Rect{ 4, 5, 0, 10 }.paint(image, color);
		RectF{ 4, 5, 10, -1 }.overwrite(image, color);
		Circle{ 15, 13, 0 }.paint(image, color, aa);
		Ellipse{ 15, 13, 0, 10 }.overwrite(image, color, aa);
		RoundRect{ 4, 5, 0, 10, 3 }.paint(image, color, aa);
		Polygon{}.paint(image, color, aa);
		Shape2D{}.overwrite(image, color, aa);
		MultiPolygon{}.paint(image, color, aa);
		LineString{ Vec2{ 15, 13 } }.paint(image, color, aa);
		for (const double thickness : { -1.0, 0.0 })
		{
			Line{ 2, 3, 28, 24 }.paint(image, thickness, color, aa);
			LineString{ Vec2{ 2, 3 }, Vec2{ 28, 24 } }.overwrite(image, thickness, color, aa);
			Triangle{ 3, 3, 28, 3, 15, 24 }.paintFrame(image, thickness, color, aa);
			Circle{ 15, 13, 8 }.overwriteFrame(image, thickness, color, aa);
			Ellipse{ 15, 13, 8, 4 }.paintFrame(image, thickness, color, aa);
			RoundRect{ 4, 5, 20, 15, 3 }.overwriteFrame(image, thickness, color, aa);
		}
		Circle{ 15, 13, 8 }.paintPie(image, 0.5, 0.0, color, aa);
		Circle{ 15, 13, 8 }.overwriteArc(image, 0.5, 0.0, 2, 3, color, aa);
		CHECK((image == Image{ 31, 27, bg }));
	}
}

TEST_CASE("ImageDraw.circle_sweep_direction_and_full_sweeps")
{
	const Circle circle{ 17.25, 15.75, 11.5 };
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
	{
		Image fill{ 37, 31, Color{ 0, 0 } }, pie = fill, frame = fill, arc = fill;
		const Color color{ 200, 50, 110, 127 };
		circle.paint(fill, color, aa);
		circle.paintPie(pie, 0.3, Math::TwoPi, color, aa);
		circle.paintFrame(frame, 2, 3, color, aa);
		circle.paintArc(arc, 0.3, -Math::TwoPi, 2, 3, color, aa);
		CHECK((fill == pie));
		CHECK((frame == arc));
		for (const double sweep : { 0.01, 1.2, 4.5 })
		{
			pie.fill(Color{ 0, 0 }); arc.fill(Color{ 0, 0 });
			circle.overwritePie(pie, 0.25, sweep, color, aa);
			circle.overwritePie(arc, 0.25 + sweep, -sweep, color, aa);
			CHECK((pie == arc));
			pie.fill(Color{ 0, 0 }); arc.fill(Color{ 0, 0 });
			circle.paintArc(pie, 0.25, sweep, 2, 3, color, aa);
			circle.paintArc(arc, 0.25 + sweep, -sweep, 2, 3, color, aa);
			CHECK((pie == arc));
		}
	}
}

TEST_CASE("ImageDraw.stroke_repeated_vertices_and_crossings_compose_once")
{
	const LineString points{ Vec2{ 4, 4 }, Vec2{ 27, 25 }, Vec2{ 4, 25 }, Vec2{ 27, 4 } };
	const LineString repeated{ points[0], points[0], points[1], points[1], points[2], points[3], points[3] };
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		for (const LineCap cap : { LineCap::Flat, LineCap::Square, LineCap::Round })
		{
			Image a{ 32, 30, Color{ 0, 0 } }, b = a;
			points.paint(a, cap, 6, Color{ 255, 113 }, aa);
			repeated.paint(b, cap, 6, Color{ 255, 113 }, aa);
			CHECK((a == b));
			CHECK(std::all_of(a.begin(), a.end(), [](Color c) { return c.a <= 113; }));
			a.fill(Color{ 0, 0 }); b.fill(Color{ 0, 0 });
			points.paintClosed(a, 6, Color{ 255, 113 }, aa);
			repeated.paintClosed(b, 6, Color{ 255, 113 }, aa);
			CHECK((a == b));
			CHECK(std::all_of(a.begin(), a.end(), [](Color c) { return c.a <= 113; }));
		}
}

TEST_CASE("ImageDraw.polygon_random_sampling_reference")
{
	// Local deterministic generator: no change to the engine's random state.
	uint32 state = 0xA37291B5;
	const auto next = [&]() { state = state * 1664525u + 1013904223u; return state; };
	for (int32 trial = 0; trial < 60; ++trial)
	{
		Array<Vec2> points;
		for (int32 i = 0, n = 3 + int32(next() % 10); i < n; ++i)
			points.emplace_back((int32(next() % 1600) - 300) / 32.0, (int32(next() % 1400) - 250) / 32.0);
		Image actual{ 32, 29, Color{ 0, 0 } };
		ImageDraw::FillPolygon(actual, points, Palette::White, ImagePixel::BlendMode::Overwrite);
		const Image expected = PolygonMask(actual.size(), { points }, true);
		CAPTURE(trial);
		CHECK((actual == expected));
	}
}

TEST_CASE("ImageDraw.ellipse_fill_matches_boundary_distance")
{
	for (const Vec2 axes : { Vec2{ 13, 2 }, Vec2{ 2, 13 }, Vec2{ 12, 7 }, Vec2{ 0.2, 0.4 }, Vec2{ 9, 0.3 } })
	{
		const Ellipse ellipse{ Vec2{ 16.25, 16.75 }, axes };
		Image actual{ 33, 33, Color{ 0, 0 } }, expected = actual;
		ellipse.overwrite(actual, Palette::White);
		for (int32 y = 0; y < 33; ++y)
			for (int32 x = 0; x < 33; ++x)
			{
				const double sd = Geometry2D::SignedDistance(ellipse, Vec2{ x + 0.5, y + 0.5 });
				const uint8 c = uint8(Clamp(0.5 - sd, 0.0, 1.0) * 255 + 0.5);
				expected[y][x] = Color{ c, c };
			}
		CAPTURE(axes);
		CHECK((actual == expected));
	}
}

TEST_CASE("ImageDraw.superellipse_implicit_sampling_and_axis_signs")
{
	for (const double n : { 0.5, 1.0, 1.5, 4.0, 16.0 })
		for (const bool aa : { false, true })
		{
			const SuperEllipse shape{ 15.31, 13.73, 12.4, 9.2, n };
			Image expected{ 33, 29, Color{ 0, 0 } }, actual = expected;
			const int32 samples = aa ? 4 : 1;
			for (int32 y = 0; y < 29; ++y)
				for (int32 x = 0; x < 33; ++x)
				{
					int32 count = 0;
					for (int32 sy = 0; sy < samples; ++sy)
						for (int32 sx = 0; sx < samples; ++sx)
						{
							const long double px = x + (sx + 0.5L) / samples;
							const long double py = y + (sy + 0.5L) / samples;
							count += (std::pow(std::abs((px - shape.x) / shape.a), n)
								+ std::pow(std::abs((py - shape.y) / shape.b), n) < 1.0L);
						}
					const uint8 coverage = uint8((count * 255 + samples * samples / 2) / (samples * samples));
					expected[y][x] = Color{ coverage, coverage };
				}
			CAPTURE(n, aa);
			CHECK(&shape.overwrite(actual, Palette::White, EnableAntialiasing{ aa }) == &shape);
			CHECK((actual == expected));
			for (const Vec2 signs : { Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
			{
				actual.fill(Color{ 0, 0 });
				SuperEllipse{ shape.center, shape.axes * signs, n }.overwrite(actual, Palette::White, EnableAntialiasing{ aa });
				CHECK((actual == expected));
			}
		}
}

TEST_CASE("ImageDraw.superellipse_special_cases")
{
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
	{
		const Image background = Background(Size{ 33, 29 });
		const Color color{ 213, 47, 112, 117 };
		Image actual = background, expected = background;
		const SuperEllipse shape{ 15.25, 13.75, -12, 8, 2 };
		CHECK(&shape.paint(actual, color, aa) == &shape);
		Ellipse{ shape.center, 12, 8 }.paint(expected, color, aa);
		CHECK((actual == expected));
		shape.overwrite(actual, color, aa);
		Ellipse{ shape.center, 12, 8 }.overwrite(expected, color, aa);
		CHECK((actual == expected));
		actual = expected = background;
		SuperEllipse{ 16, 14, 12, 8, 1 }.overwrite(actual, color, aa);
		Quad{ 16, 6, 28, 14, 16, 22, 4, 14 }.overwrite(expected, color, aa);
		CHECK((actual == expected));
		actual = background;
		for (const auto invalid : { SuperEllipse{ 16, 14, 0, 8, 4 }, SuperEllipse{ 16, 14, 12, 0, 4 },
			SuperEllipse{ 16, 14, 12, 8, 0 }, SuperEllipse{ 16, 14, 12, 8, -1 } })
		{
			invalid.paint(actual, color, aa);
			invalid.overwrite(actual, color, aa);
		}
		CHECK((actual == background));
	}
	// A shape smaller than a pixel can still cover AA sample positions.
	Image tiny{ 3, 3, Color{ 0, 0 } };
	SuperEllipse{ 1.5, 1.5, 0.2, 0.2, 4 }.overwrite(tiny, Palette::White);
	CHECK(tiny[1][1].a == 64);
	CHECK(tiny[0][0].a == 0);
}

TEST_CASE("ImageDraw.quad_concave_and_crossed_contours")
{
	for (const Quad quad : { Quad{ 2, 2, 16, 28, 30, 2, 16, 16 }, Quad{ 2, 2, 30, 28, 2, 28, 30, 2 } })
		for (const bool aa : { false, true })
		{
			Image actual{ 33, 31, Color{ 0, 0 } };
			quad.overwrite(actual, Palette::White, EnableAntialiasing{ aa });
			const Image expected = PolygonMask(actual.size(), { { quad.p0, quad.p1, quad.p2, quad.p3 } }, aa);
			CAPTURE(quad, aa);
			const auto mismatch = std::mismatch(actual.begin(), actual.end(), expected.begin());
			if (mismatch.first != actual.end())
			{
				const auto index = std::distance(actual.begin(), mismatch.first);
				CAPTURE(index, *mismatch.first, *mismatch.second);
				CHECK((actual == expected));
			}
			else CHECK((actual == expected));
		}
}

TEST_CASE("ImageDraw.narrow_destinations_and_offset_overloads")
{
	for (const auto& drawing : Drawings())
		for (const Size size : { Size{ 1, 1 }, Size{ 1, 31 }, Size{ 37, 1 } })
			for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
			{
				const Color bg{ 31, 71, 113, 67 }, color{ 197, 91, 171, 103 };
				Image actual{ size, bg }, large{ 64, 64, bg };
				drawing.draw(actual, color, true, aa, Vec2{ -16, -13 });
				drawing.draw(large, color, true, aa, Vec2{ 0, 0 });
				bool equal = true;
				for (int32 y = 0; y < size.y; ++y)
					for (int32 x = 0; x < size.x; ++x)
						equal &= (actual[y][x] == large[y + 13][x + 16]);
				CAPTURE(drawing.name, size);
				CHECK(equal);
			}
	const Polygon polygon{ { Vec2{ 2, 2 }, Vec2{ 29, 2 }, Vec2{ 29, 26 }, Vec2{ 2, 26 } },
		{ { Vec2{ 8, 8 }, Vec2{ 8, 18 }, Vec2{ 21, 18 }, Vec2{ 21, 8 } } } };
	const Vec2 offset{ -3.25, 4.75 };
	const Color color{ 211, 71, 137, 103 };
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		for (const bool paint : { false, true })
		{
			Image actual = Background(Size{ 33, 31 }), expected = actual;
			if (paint)
			{
				polygon.paint(actual, offset, color, aa);
				polygon.movedBy(offset).paint(expected, color, aa);
				polygon.paintFrame(actual, offset, 3.25, color, aa);
				polygon.movedBy(offset).paintFrame(expected, 3.25, color, aa);
			}
			else
			{
				polygon.overwrite(actual, offset, color, aa);
				polygon.movedBy(offset).overwrite(expected, color, aa);
				polygon.overwriteFrame(actual, offset, 3.25, color, aa);
				polygon.movedBy(offset).overwriteFrame(expected, 3.25, color, aa);
			}
			CHECK((actual == expected));
		}
}

TEST_CASE("ImageDraw.pixel_compositing_bulk_tails_and_opaque_destination")
{
	for (const size_t count : { 0u, 1u, 3u, 4u, 7u, 15u, 16u, 17u, 31u })
		for (const uint32 alpha : { 0u, 1u, 113u, 254u, 255u })
			for (const bool paint : { false, true })
			{
				std::array<Color, 35> actual, expected;
				std::array<uint8, 35> coverage;
				const Color src{ 203, 71, 127, alpha };
				for (size_t i = 0; i < actual.size(); ++i)
				{
					actual[i] = expected[i] = Color{ uint8(i * 73), uint8(i * 31), uint8(i * 11), uint8(i * 17) };
					coverage[i] = uint8(i * 51);
				}
				for (size_t i = 1; i <= count; ++i)
					expected[i] = Composite(src, expected[i], coverage[i], paint);
				ImagePixel::ApplyWithCoverage(paint ? ImagePixel::BlendMode::SourceOver : ImagePixel::BlendMode::Overwrite,
					src, actual.data() + 1, coverage.data() + 1, count);
				CHECK((actual == expected));
			}
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		for (const double n : { 0.5, 2.0, 4.0 })
		{
			const SuperEllipse shape{ 13.25, 14.75, 12, 9, n };
			const Color color{ 213, 79, 151, 117 };
			Image actual{ 31, 29, Color{ 30, 70, 110 } }, expected = actual;
			ImageDraw::Fill(actual, shape, color, ImagePixel::BlendMode::SourceOver, aa, ImageDraw::DstAlpha::AssumeOpaque);
			shape.paint(expected, color, aa);
			CHECK((actual == expected));
		}
}
