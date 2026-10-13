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
			{ "Circle.segment.minor", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Circle{ 17.25, 15.75, 11.5 }.movedBy(offset);
					if (paint) shape.paintSegment(image, 0.37, 4.5, color, aa);
					else shape.overwriteSegment(image, 0.37, 4.5, color, aa);
				} },
			{ "Circle.segment.major", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Circle{ 17.25, 15.75, 11.5 }.movedBy(offset);
					if (paint) shape.paintSegment(image, 0.37, 18.5, color, aa);
					else shape.overwriteSegment(image, 0.37, 18.5, color, aa);
				} },
			{ "Circle.segment.angle", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Circle{ 17.25, 15.75, 11.5 }.movedBy(offset);
					if (paint) shape.paintSegmentFromAngles(image, 0.37, 1.2, color, aa);
					else shape.overwriteSegmentFromAngles(image, 0.37, 1.2, color, aa);
				} },
			{ "Circle.segment.negative", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Circle{ 17.25, 15.75, 11.5 }.movedBy(offset);
					if (paint) shape.paintSegmentFromAngles(image, 0.37, -4.5, color, aa);
					else shape.overwriteSegmentFromAngles(image, 0.37, -4.5, color, aa);
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
			{ "SuperEllipse.frame.n=0.5", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const SuperEllipse shape{ Vec2{ 17.25, 15.75 } + offset, 13, 9, 0.5 };
					if (paint) shape.paintFrame(image, 5.25, color, aa);
					else shape.overwriteFrame(image, 5.25, color, aa);
				} },
			{ "SuperEllipse.frame.n=1.0", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const SuperEllipse shape{ Vec2{ 17.25, 15.75 } + offset, 13, 9, 1.0 };
					if (paint) shape.paintFrame(image, 5.25, color, aa);
					else shape.overwriteFrame(image, 5.25, color, aa);
				} },
			{ "SuperEllipse.frame.n=2.0", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const SuperEllipse shape{ Vec2{ 17.25, 15.75 } + offset, 13, 9, 2.0 };
					if (paint) shape.paintFrame(image, 5.25, color, aa);
					else shape.overwriteFrame(image, 5.25, color, aa);
				} },
			{ "SuperEllipse.frame.n=4.0", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const SuperEllipse shape{ Vec2{ 17.25, 15.75 } + offset, 13, 9, 4.0 };
					if (paint) shape.paintFrame(image, 5.25, color, aa);
					else shape.overwriteFrame(image, 5.25, color, aa);
				} },
			{ "SuperEllipse.frame.n=16.0", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const SuperEllipse shape{ Vec2{ 17.25, 15.75 } + offset, 13, 9, 16.0 };
					if (paint) shape.paintFrame(image, 5.25, color, aa);
					else shape.overwriteFrame(image, 5.25, color, aa);
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
			{ "Ellipse.pie.minor", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const Ellipse shape{ Vec2{ 17.25, 15.75 } + offset, 13, 7 };
					if (paint) shape.paintPie(image, 0.3, 1.2, color, aa);
					else shape.overwritePie(image, 0.3, 1.2, color, aa);
				} },
			{ "Ellipse.pie.major", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const Ellipse shape{ Vec2{ 17.25, 15.75 } + offset, 13, 7 };
					if (paint) shape.paintPie(image, 0.3, 4.5, color, aa);
					else shape.overwritePie(image, 0.3, 4.5, color, aa);
				} },
			{ "Ellipse.pie.negative", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const Ellipse shape{ Vec2{ 17.25, 15.75 } + offset, 13, 7 };
					if (paint) shape.paintPie(image, 0.3, -4.5, color, aa);
					else shape.overwritePie(image, 0.3, -4.5, color, aa);
				} },
			{ "Circle.arc.round_short", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Circle{ 17.25, 15.75, 11.5 }.movedBy(offset);
					if (paint) shape.paintArc(image, LineCap::Round, 0.37, 0.03, 2, 3, color, aa);
					else shape.overwriteArc(image, LineCap::Round, 0.37, 0.03, 2, 3, color, aa);
				} },
			{ "Circle.arc.round_major", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Circle{ 17.25, 15.75, 11.5 }.movedBy(offset);
					if (paint) shape.paintArc(image, LineCap::Round, 0.37, Math::TwoPi - 0.03, 2, 3, color, aa);
					else shape.overwriteArc(image, LineCap::Round, 0.37, Math::TwoPi - 0.03, 2, 3, color, aa);
				} },
			{ "Circle.arc.square_major", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Circle{ 17.25, 15.75, 11.5 }.movedBy(offset);
					if (paint) shape.paintArc(image, LineCap::Square, 0.37, 4.5, 2, 3, color, aa);
					else shape.overwriteArc(image, LineCap::Square, 0.37, 4.5, 2, 3, color, aa);
				} },
			{ "Circle.arc.square_zero", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Circle{ 17.25, 15.75, 11.5 }.movedBy(offset);
					if (paint) shape.paintArc(image, LineCap::Square, 0.37, 0.0, 2, 3, color, aa);
					else shape.overwriteArc(image, LineCap::Square, 0.37, 0.0, 2, 3, color, aa);
				} },
			{ "Circle.arc.round_zero", [](Image& image, Color color, bool paint, EnableAntialiasing aa, Vec2 offset)
				{
					const auto shape = Circle{ 17.25, 15.75, 11.5 }.movedBy(offset);
					if (paint) shape.paintArc(image, LineCap::Round, 0.37, 0.0, 2, 3, color, aa);
					else shape.overwriteArc(image, LineCap::Round, 0.37, 0.0, 2, 3, color, aa);
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
		const Shape2D shape = Shape2D::Star(r, center);
		const Polygon polygon = shape.toPolygon();
		const Polygon holed{ RectF{ center - Vec2{ r, r }, r * 2, r * 2 }.outer(),
			{ RectF{ center - Vec2{ r * 0.8, r * 0.8 }, r * 1.6, r * 1.6 }.outer().reversed() } };
		REQUIRE(holed);
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
					image.fill(Color{ 40, 70, 100, 255 });
					bench.run(name + suffix, [&] { draw(); doNotOptimizeAway(image.data()); });
					CHECK(std::any_of(image.begin(), image.end(), [](Color c) { return c.r != 40; }));
				};
				run("Polygon", [&] { if (paint) polygon.paint(image, color, EnableAntialiasing{ aa }); else polygon.overwrite(image, color, EnableAntialiasing{ aa }); });
				run("Shape2D", [&] { if (paint) shape.paint(image, color, EnableAntialiasing{ aa }); else shape.overwrite(image, color, EnableAntialiasing{ aa }); });
				run("Shape2DFrame", [&] { if (paint) shape.paintFrame(image, 3.5, color, EnableAntialiasing{ aa }); else shape.overwriteFrame(image, 3.5, color, EnableAntialiasing{ aa }); });
				run("Shape2DOffset", [&] { if (paint) shape.paint(image, Vec2{ 0.25, 0.75 }, color, EnableAntialiasing{ aa }); else shape.overwrite(image, Vec2{ 0.25, 0.75 }, color, EnableAntialiasing{ aa }); });
				run("PolygonFrame", [&] { if (paint) polygon.paintFrame(image, 3.5, color, EnableAntialiasing{ aa }); else polygon.overwriteFrame(image, 3.5, color, EnableAntialiasing{ aa }); });
				run("PolygonFrameHoles", [&] { if (paint) holed.paintFrame(image, r * 0.3, color, EnableAntialiasing{ aa }); else holed.overwriteFrame(image, r * 0.3, color, EnableAntialiasing{ aa }); });
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

TEST_CASE("ImageDraw.benchmark_small_circles", "[.benchmark]")
{
	ankerl::nanobench::Bench bench;
	bench.title("Small non-AA circles").unit("draw").epochs(11).minEpochIterations(100);
	Image image{ 64, 64, Color{ 40, 70, 100, 255 } };
	for (const double radius : { 0.5, 1.0, 2.75, 5.0, 12.0, 24.0 })
		for (const bool clipped : { false, true })
			for (const bool paint : { false, true })
			{
				const Circle circle{ clipped ? Vec2{ 0.25, -0.25 } : Vec2{ 32.25, 31.75 }, radius };
				const std::string name = std::to_string(radius) + (clipped ? "/clipped" : "/inside") + (paint ? "/paint" : "/overwrite");
				bench.run(name, [&]
				{
					if (paint) circle.paint(image, Color{ 210, 60, 120, 113 }, EnableAntialiasing::No);
					else circle.overwrite(image, Color{ 210, 60, 120, 113 }, EnableAntialiasing::No);
					ankerl::nanobench::doNotOptimizeAway(image.data());
				});
			}
}

TEST_CASE("ImageDraw.zero_length_line_matches_repeated_line_string")
{
	for (const Vec2 point : { Vec2{ 12, 13 }, Vec2{ 12.25, 13.75 }, Vec2{ 12.5, 13.5 }, Vec2{ -0.25, 0.75 } })
		for (const double thickness : { 0.4, 1.0, 2.0, 5.5, 12.0 })
			for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
				for (const LineCap cap : { LineCap::Flat, LineCap::Square, LineCap::Round })
					for (const bool paint : { false, true })
					{
						Image actual = Background(Size{ 27, 29 }), expected = actual;
						const Color color{ 211, 83, 147, 113 };
						const Line line{ point, point };
						const LineString repeated{ point, point, point };
						if (paint)
						{
							line.paint(actual, cap, thickness, color, aa);
							repeated.paint(expected, cap, thickness, color, aa);
						}
						else
						{
							line.overwrite(actual, cap, thickness, color, aa);
							repeated.overwrite(expected, cap, thickness, color, aa);
						}
						CAPTURE(point, thickness, bool(aa), cap, paint);
						CHECK((actual == expected));
					}
}

TEST_CASE("ImageDraw.circle_full_sweep_and_collapsed_frame_reuse_fill")
{
	for (const Vec2 center : { Vec2{ 12.25, 13.75 }, Vec2{ -0.25, 0.75 } })
		for (const double radius : { 0.2, 0.5, 1.0, 5.5, 12.0 })
			for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
			{
				const Circle circle{ center, radius };
				const Color color{ 211, 83, 147, 113 };
				Image fill = Background(Size{ 27, 29 }), frame = fill, pie = fill, arc = fill;
				circle.paint(fill, color, aa);
				Circle{ center, radius * 0.5 }.paintFrame(frame, radius, radius * 0.5, color, aa);
				circle.paintPie(pie, 1.2, Math::TwoPi, color, aa);
				circle.paintArc(arc, -0.7, -Math::TwoPi, radius, 0, color, aa);
				CAPTURE(center, radius, bool(aa));
				CHECK((fill == frame));
				CHECK((fill == pie));
				CHECK((fill == arc));
			}
}

TEST_CASE("ImageDraw.polygon_frame_contours_compose_once")
{
	const Array<Vec2> outer = RectF{ 2.25, 3.75, 31, 27 }.outer();
	const Array<Vec2> hole1 = RectF{ 5.25, 6.75, 10, 19 }.outer().reversed();
	const Array<Vec2> hole2 = RectF{ 18.25, 6.75, 11, 19 }.outer().reversed();
	const Polygon polygon{ outer, { hole1, hole2 } };
	REQUIRE(polygon);
	const Size size{ 37, 35 };
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ -8.5, -10.25 } })
		for (const double thickness : { 0.5, 1.0, 6.5, 15.0 })
			for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
			{
				// Combine masks before color compositing, including partially covered overlaps.
				Image coverage{ size, Color{ 0, 0 } };
				for (const auto& contour : { outer, hole1, hole2 })
				{
					Image part{ size, Color{ 0, 0 } };
					ImageDraw::ClosedLineString(part, contour, offset, thickness, Palette::White, ImagePixel::BlendMode::Overwrite, aa);
					for (size_t i = 0; i < part.size_bytes() / sizeof(Color); ++i)
						coverage.data()[i].a = Max(coverage.data()[i].a, part.data()[i].a);
				}
				for (const bool paint : { false, true })
					for (const uint8 alpha : { uint8{ 0 }, uint8{ 113 }, uint8{ 255 } })
					{
						const Color color{ 211, 83, 147, alpha };
						Image actual = Background(size), expected = actual;
						if (paint) polygon.paintFrame(actual, offset, thickness, color, aa);
						else polygon.overwriteFrame(actual, offset, thickness, color, aa);
						for (size_t i = 0; i < actual.size_bytes() / sizeof(Color); ++i)
							expected.data()[i] = Composite(color, expected.data()[i], coverage.data()[i].a, paint);
						CAPTURE(offset, thickness, bool(aa), paint, alpha);
						CHECK((actual == expected));
					}
				Image actual{ size, Color{ 0, 0 } }, expected = actual;
				if (offset == Vec2{ 0, 0 }) polygon.paintFrame(actual, thickness, Color{ 255, 113 }, aa);
				else polygon.paintFrame(actual, offset, thickness, Color{ 255, 113 }, aa);
				polygon.movedBy(offset).paintFrame(expected, thickness, Color{ 255, 113 }, aa);
				CHECK((actual == expected));
			}
}

TEST_CASE("ImageDraw.polygon_frame_defaults_empty_and_single_contour")
{
	const Polygon polygon = Shape2D::Star(12, Vec2{ 17.25, 15.75 }).toPolygon();
	REQUIRE(polygon);
	const Vec2 offset{ -3.25, 4.75 };
	const Color color{ 211, 83, 147, 113 };
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		for (const bool paint : { false, true })
		{
			Image actual = Background(Size{ 37, 35 }), expected = actual;
			if (paint) polygon.paintFrame(actual, color, aa);
			else polygon.overwriteFrame(actual, color, aa);
			ImageDraw::ClosedLineString(expected, polygon.outer(), 1.0, color,
				paint ? ImagePixel::BlendMode::SourceOver : ImagePixel::BlendMode::Overwrite, aa);
			CHECK((actual == expected));
			actual = Background(actual.size()); expected = actual;
			if (paint) polygon.paintFrame(actual, offset, color, aa);
			else polygon.overwriteFrame(actual, offset, color, aa);
			ImageDraw::ClosedLineString(expected, polygon.outer(), offset, 1.0, color,
				paint ? ImagePixel::BlendMode::SourceOver : ImagePixel::BlendMode::Overwrite, aa);
			CHECK((actual == expected));
			actual = Background(actual.size()); expected = actual;
			polygon.paintFrame(actual, 0.0, color, aa);
			polygon.overwriteFrame(actual, offset, -1.0, color, aa);
			Polygon{}.paintFrame(actual, offset, 6.5, color, aa);
			Polygon{}.overwriteFrame(actual, 6.5, color, aa);
			CHECK((actual == expected));
			Image empty;
			polygon.paintFrame(empty, 6.5, color, aa);
			polygon.overwriteFrame(empty, offset, 6.5, color, aa);
			CHECK(empty.isEmpty());
		}
}

TEST_CASE("ImageDraw.multipolygon_preserves_member_compositing")
{
	const Polygon polygon{ RectF{ 3.25, 4.75, 25, 19 }.outer(),
		{ RectF{ 6.25, 7.75, 19, 13 }.outer().reversed() } };
	REQUIRE(polygon);
	const MultiPolygon multi{ polygon, polygon };
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		for (const bool paint : { false, true })
			for (const bool frame : { false, true })
				for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ -3.25, 4.75 } })
				{
					Image actual = Background(Size{ 37, 35 }), expected = actual;
					const Color color{ 211, 83, 147, 113 };
					const auto draw = [&](const auto& shape, Image& image)
					{
						if (frame)
						{
							if (paint) shape.paintFrame(image, offset, 6.5, color, aa);
							else shape.overwriteFrame(image, offset, 6.5, color, aa);
						}
						else
						{
							if (paint) shape.paint(image, offset, color, aa);
							else shape.overwrite(image, offset, color, aa);
						}
					};
					draw(multi, actual);
					draw(polygon, expected);
					draw(polygon, expected);
					CHECK((actual == expected));
				}
}

TEST_CASE("ImageDraw.shape2d_uses_vertex_contour_independently_of_indices")
{
	const Array<Float2> vertices{ { 3.25f, 4.75f }, { 29.5f, 4.75f }, { 29.5f, 25.25f }, { 3.25f, 25.25f } };
	const Array<Array<TriangleIndex>> indexSets{ {}, { { 0, 1, 2 } }, { { 2, 1, 0 } }, { { 0, 1, 2 }, { 0, 2, 3 } } };
	for (const auto& indices : indexSets)
	{
		const Shape2D shape{ vertices, indices };
		for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ -7.125, 2.0625 } })
			for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
				for (const bool paint : { false, true })
				{
					const Size size{ 35, 31 };
					const auto points = vertices.map([&](Float2 p) { return Vec2{ p } + offset; });
					const Image mask = PolygonMask(size, { points }, bool(aa));
					Image actual = Background(size), expected = actual;
					const Color color{ 211, 83, 147, 113 };
					if (offset == Vec2{ 0, 0 })
					{
						if (paint) CHECK(&shape.paint(actual, color, aa) == &shape);
						else CHECK(&shape.overwrite(actual, color, aa) == &shape);
					}
					else
					{
						if (paint) CHECK(&shape.paint(actual, offset, color, aa) == &shape);
						else CHECK(&shape.overwrite(actual, offset, color, aa) == &shape);
					}
					for (size_t i = 0; i < actual.size_bytes() / sizeof(Color); ++i)
						expected.data()[i] = Composite(color, expected.data()[i], mask.data()[i].a, paint);
					CHECK((actual == expected));
				}
	}
}

TEST_CASE("ImageDraw.shape2d_float_vertices_and_offsets_match_contour_path")
{
	const Vec2 center{ 17.25, 15.75 };
	const Array<Shape2D> shapes{
		Shape2D::Star(12, center), Shape2D::Cross(12, 5, center, 0.3),
		Shape2D::Ngon(60, 12, center, 0.2), Shape2D::NStar(16, 12, 4, center, 0.1),
		Shape2D::Heart(12, center, 0.3, QualityFactor{ 1.0 }),
		Shape2D::Squircle(12, center, 24), Shape2D::Astroid(center, 12, 8, 0.2, QualityFactor{ 1.0 }),
		Shape2D{ { { 2.25f, 3.75f }, { 31.5f, 27.25f }, { 2.25f, 27.25f }, { 31.5f, 3.75f } }, {} },
		Shape2D{ { { 2, 3 }, { 2, 3 }, { 31, 3 }, { 31, 27 }, { 2, 27 }, { 2, 3 } }, {} },
		Shape2D{}, Shape2D{ { { 2, 3 } }, {} }, Shape2D{ { { 2, 3 }, { 31, 27 } }, {} }
	};
	for (const auto& shape : shapes)
		for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ -7.125, 2.0625 }, Vec2{ 50, -50 } })
			for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
				for (const bool paint : { false, true })
					for (const bool frame : { false, true })
					{
						Image actual = Background(Size{ 35, 31 }), expected = actual;
						const Color color{ 211, 83, 147, 113 };
						const auto mode = paint ? ImagePixel::BlendMode::SourceOver : ImagePixel::BlendMode::Overwrite;
						const auto points = shape.vertices().map([&](Float2 p) { return Vec2{ p } + offset; });
						if (frame)
						{
							if (offset == Vec2{ 0, 0 })
							{
								if (paint) shape.paintFrame(actual, 5.5, color, aa);
								else shape.overwriteFrame(actual, 5.5, color, aa);
							}
							else
							{
								if (paint) shape.paintFrame(actual, offset, 5.5, color, aa);
								else shape.overwriteFrame(actual, offset, 5.5, color, aa);
							}
							ImageDraw::ClosedLineString(expected, points, 5.5, color, mode, aa);
						}
						else
						{
							if (paint) shape.paint(actual, offset, color, aa);
							else shape.overwrite(actual, offset, color, aa);
							ImageDraw::FillPolygon(expected, points, color, mode, aa);
						}
						CAPTURE(offset, bool(aa), paint, frame, points.size());
						CHECK((actual == expected));
					}
}

TEST_CASE("ImageDraw.new_contour_paths_destination_alpha_and_empty_images")
{
	const Polygon polygon{ RectF{ 3.25, 4.75, 25, 19 }.outer(),
		{ RectF{ 6.25, 7.75, 19, 13 }.outer().reversed() } };
	REQUIRE(polygon);
	const Shape2D shape = Shape2D::Star(12, Vec2{ 17.25, 15.75 });
	for (const auto mode : { ImagePixel::BlendMode::SourceOver, ImagePixel::BlendMode::Overwrite })
		for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
			for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ -7.125, 2.0625 } })
			{
				const Color color{ 211, 83, 147, 113 };
				for (int32 kind = 0; kind < 3; ++kind)
				{
					const auto draw = [&](Image& image, ImageDraw::DstAlpha alpha)
					{
						if (kind == 0) ImageDraw::Fill(image, shape, offset, color, mode, aa, alpha);
						else if (kind == 1) ImageDraw::Shape2DFrame(image, shape, offset, 6.5, color, mode, aa, alpha);
						else ImageDraw::PolygonFrame(image, polygon, offset, 6.5, color, mode, aa, alpha);
					};
					Image actual{ 37, 35, Color{ 40, 70, 100, 255 } }, expected = actual;
					draw(actual, ImageDraw::DstAlpha::AssumeOpaque);
					draw(expected, ImageDraw::DstAlpha::Preserve);
					CHECK((actual == expected));
					Image empty;
					draw(empty, ImageDraw::DstAlpha::Preserve);
					CHECK(empty.isEmpty());
				}
				Image actual = Background(Size{ 37, 35 }), expected = actual;
				shape.paintFrame(actual, offset, 0.0, color, aa);
				shape.overwriteFrame(actual, -1.0, color, aa);
				CHECK((actual == expected));
			}
}

TEST_CASE("ImageDraw.benchmark_polygon_frames", "[.benchmark]")
{
	ankerl::nanobench::Bench bench;
	bench.title("Polygon frames by hole count").unit("draw").epochs(7).minEpochIterations(2);
	for (const int32 size : { 256, 1024 })
		for (const int32 grid : { 1, 4, 8 })
		{
			const double step = size * 0.8 / grid;
			Array<Array<Vec2>> holes;
			for (int32 y = 0; y < grid; ++y)
				for (int32 x = 0; x < grid; ++x)
					holes.push_back(RectF{ size * 0.1 + (x + 0.25) * step, size * 0.1 + (y + 0.25) * step,
						step * 0.5, step * 0.5 }.outer().reversed());
			const Polygon polygon{ RectF{ size * 0.05, size * 0.05, size * 0.9, size * 0.9 }.outer(), holes };
			REQUIRE(polygon);
			Image image{ size, Color{ 40, 70, 100, 255 } };
			for (const double thickness : { 1.5, step * 0.6 })
				for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
					for (const bool combined : { false, true })
					{
						const std::string name = std::to_string(size) + "/holes=" + std::to_string(grid * grid)
							+ (thickness == 1.5 ? "/thin" : "/wide") + (bool(aa) ? "/AA" : "/NoAA")
							+ (combined ? "/combined" : "/sequential");
						image.fill(Color{ 40, 70, 100, 255 });
						bench.run(name, [&]
						{
							const Color color{ 211, 83, 147, 113 };
							if (combined) polygon.paintFrame(image, thickness, color, aa);
							else
							{
								// Cost reference: separate calls repeat compositing where contours overlap.
								ImageDraw::ClosedLineString(image, polygon.outer(), thickness, color, ImagePixel::BlendMode::SourceOver, aa);
								for (const auto& hole : polygon.inners())
									ImageDraw::ClosedLineString(image, hole, thickness, color, ImagePixel::BlendMode::SourceOver, aa);
							}
							ankerl::nanobench::doNotOptimizeAway(image.data());
						});
						CHECK(std::any_of(image.begin(), image.end(), [](Color c) { return c.r != 40; }));
					}
		}
}

TEST_CASE("ImageDraw.round_strokes_match_distance_union")
{
	const Array<LineString> paths{
		{ Vec2{ 3.25, 4.75 }, Vec2{ 31.25, 4.75 }, Vec2{ 31.25, 29.75 }, Vec2{ 3.25, 29.75 } },
		{ Vec2{ -5.25, 2.75 }, Vec2{ 32.5, 27.25 }, Vec2{ 3.25, 28.75 }, Vec2{ 31.75, -4.25 } },
		{ Vec2{ 18.25, 1.75 }, Vec2{ 2.25, 27.25 }, Vec2{ 30.75, 12.25 }, Vec2{ 3.5, 9.25 }, Vec2{ 28.25, 29.75 } },
		{ Vec2{ 13.25, 12.75 }, Vec2{ 13.25, 12.75 }, Vec2{ 13.25, 12.75 } }
	};
	for (size_t pathIndex = 0; pathIndex < paths.size(); ++pathIndex)
		for (const double thickness : { 0.5, 3.25, 11.5 })
			for (const bool closed : { false, true })
			{
				const auto& points = paths[pathIndex];
				Image actual{ 37, 35, Color{ 0, 0 } };
				if (closed) points.overwriteClosed(actual, thickness, Palette::White);
				else points.overwrite(actual, LineCap::Round, thickness, Palette::White);
				// Closed strokes with fewer than three distinct consecutive vertices are empty.
				if (closed && (pathIndex == 3))
				{
					CHECK((actual == Image{ actual.size(), Color{ 0, 0 } }));
					continue;
				}
				int32 largestError = 0;
				for (int32 y = 0; y < actual.height(); ++y)
					for (int32 x = 0; x < actual.width(); ++x)
					{
						const Vec2 p{ x + 0.5, y + 0.5 };
						double distance = Math::Inf;
						for (const Vec2 vertex : points)
							distance = Min(distance, p.distanceFrom(vertex) - thickness * 0.5);
						for (size_t i = 0, count = points.size() - (closed ? 0 : 1); i < count; ++i)
						{
							const Vec2 from = points[i], to = points[(i + 1) % points.size()];
							const double length = from.distanceFrom(to);
							if (length == 0) continue;
							const Vec2 direction = (to - from) / length;
							const Vec2 local = p - (from + to) * 0.5;
							const double qx = Abs(local.dot(direction)) - length * 0.5;
							const double qy = Abs(local.cross(direction)) - thickness * 0.5;
							const double body = Vec2{ Max(qx, 0.0), Max(qy, 0.0) }.length() + Min(Max(qx, qy), 0.0);
							distance = Min(distance, body);
						}
						const int32 coverage = int32(Clamp(0.5 - distance, 0.0, 1.0) * 255 + 0.5);
						largestError = Max(largestError, Abs(int32(actual[y][x].a) - coverage));
					}
				CAPTURE(pathIndex, thickness, closed);
				CHECK(largestError <= 1); // Independent distance arithmetic can round at a coverage tie.
			}
}

TEST_CASE("ImageDraw.superellipse_frame_boundary_approximation")
{
	for (const double n : { 0.25, 0.5, 0.98, 1.0, 1.02, 1.5, 2.0, 4.0, 16.0, 32.0 })
		for (const Vec2 axes : { Vec2{ 15, 10 }, Vec2{ 17, 1.25 }, Vec2{ 0.2, 0.35 } })
		{
			const SuperEllipse shape{ Vec2{ 18.25, 16.75 }, axes, n };
			Grid<double> distances{ 39, 35 };
			for (int32 y = 0; y < 35; ++y)
				for (int32 x = 0; x < 39; ++x)
					distances[y][x] = Abs(Geometry2D::SignedDistance(shape, Vec2{ x + 0.5, y + 0.5 }));
			for (const double thickness : { 1.5, 5.25, 18.5 })
				for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
				{
					Image actual{ 39, 35, Color{ 0, 0 } };
					shape.overwriteFrame(actual, thickness, Palette::White, aa);
					int32 largestExcess = 0, largestDeficit = 0, wrongInterior = 0;
					for (int32 y = 0; y < 35; ++y)
						for (int32 x = 0; x < 39; ++x)
						{
							const double distance = (distances[y][x] - thickness * 0.5);
							const int32 expected = int32(Clamp(0.5 - distance, 0.0, 1.0) * 255 + 0.5);
							largestExcess = Max(largestExcess, int32(actual[y][x].a) - expected);
							largestDeficit = Max(largestDeficit, expected - int32(actual[y][x].a));
							// Binary coverage may differ only within the contour approximation band.
							if (0.126 < Abs(distance)) wrongInterior += ((actual[y][x].a != 0) != (distance < 0));
						}
					CAPTURE(n, axes, thickness, bool(aa));
					if (aa)
					{
						CHECK(largestExcess <= 33); // 1/8 pixel chord error plus byte rounding.
						// A round stroke uses max(rectangle, disk) coverage at each join.
						// Compared with an exact capsule SDF, this can lose up to
						// 0.25 / thickness coverage in the one-pixel AA band (thickness >= 1).
						CHECK(largestDeficit <= (33 + int32(std::ceil(255.0 * 0.25 / thickness))));
					}
					else CHECK(wrongInterior == 0);
				}
		}
}

TEST_CASE("ImageDraw.superellipse_frame_special_cases_and_alpha")
{
	const Color color{ 213, 47, 112, 117 };
	const Image background = Background(Size{ 33, 29 });
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
	{
		Image actual = background, expected = background;
		const SuperEllipse diamond{ 16, 14, 12, 8, 1 };
		CHECK(&diamond.paintFrame(actual, 3.5, color, aa) == &diamond);
		Quad{ 16, 6, 28, 14, 16, 22, 4, 14 }.paintFrame(expected, 3.5, color, aa);
		CHECK((actual == expected));
		CHECK(&diamond.overwriteFrame(actual, 3.5, color, aa) == &diamond);
		Quad{ 16, 6, 28, 14, 16, 22, 4, 14 }.overwriteFrame(expected, 3.5, color, aa);
		CHECK((actual == expected));
		for (const auto invalid : { SuperEllipse{ 16, 14, 0, 8, 4 }, SuperEllipse{ 16, 14, 12, 0, 4 },
			SuperEllipse{ 16, 14, 12, 8, 0 }, SuperEllipse{ 16, 14, 12, 8, -1 } })
		{
			invalid.paintFrame(actual, 5, color, aa);
			invalid.overwriteFrame(actual, 5, color, aa);
		}
		diamond.paintFrame(actual, 0, color, aa);
		diamond.overwriteFrame(actual, -1, color, aa);
		CHECK((actual == expected));
		Image empty;
		diamond.paintFrame(empty, 5, color, aa);
		diamond.overwriteFrame(empty, 5, color, aa);
		CHECK(empty.isEmpty());
		for (const double n : { 0.5, 1.0, 2.0, 4.0, 16.0 })
		{
			const SuperEllipse shape{ 16, 14, 12, 8, n };
			actual = background;
			shape.paintFrame(actual, 7.5, color, aa);
			for (const Vec2 signs : { Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
			{
				expected = background;
				SuperEllipse{ shape.center, shape.axes * signs, n }.paintFrame(expected, 7.5, color, aa);
				CHECK((actual == expected));
			}
			for (const auto mode : { ImagePixel::BlendMode::SourceOver, ImagePixel::BlendMode::Overwrite })
			{
				actual = expected = Image{ 33, 29, Color{ 40, 100, 160, 255 } };
				ImageDraw::SuperEllipseFrame(actual, shape, 7.5, color, mode, aa);
				ImageDraw::SuperEllipseFrame(expected, shape, 7.5, color, mode, aa, ImageDraw::DstAlpha::AssumeOpaque);
				CHECK((actual == expected));
			}
		}
	}
	Image explicitAA = background, defaultAA = background, direct = background;
	const SuperEllipse shape{ 15.25, 13.75, 12, 8, 0.5 };
	shape.paintFrame(explicitAA, 5, color, EnableAntialiasing::Yes);
	shape.paintFrame(defaultAA, 5, color);
	ImageDraw::SuperEllipseFrame(direct, shape, 5, color);
	CHECK((explicitAA == defaultAA));
	CHECK((direct == defaultAA));
	explicitAA = defaultAA = background;
	shape.overwriteFrame(explicitAA, 5, color, EnableAntialiasing::Yes);
	shape.overwriteFrame(defaultAA, 5, color);
	CHECK((explicitAA == defaultAA));
}

TEST_CASE("ImageDraw.superellipse_frame_symmetry_and_clipping")
{
	for (const double n : { 0.5, 1.0, 1.5, 2.0, 4.0, 16.0, 32.0 })
		for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		{
			const SuperEllipse shape{ 24, 24, 19, 13, n };
			Image full{ 48, 48, Color{ 0, 0 } };
			shape.overwriteFrame(full, 3.25, Palette::White, aa);
			int32 symmetryError = 0;
			for (int32 y = 0; y < 48; ++y)
				for (int32 x = 0; x < 48; ++x)
				{
					symmetryError = Max(symmetryError, Abs(int32(full[y][x].a) - full[y][47 - x].a));
					symmetryError = Max(symmetryError, Abs(int32(full[y][x].a) - full[47 - y][x].a));
				}
			CHECK(symmetryError <= 1);
			for (const Point offset : { Point{ 0, 0 }, Point{ 20, 20 }, Point{ 47, 47 }, Point{ 80, 80 } })
			{
				Image clipped{ 21, 19, Color{ 0, 0 } }, expected = clipped;
				shape.movedBy(-offset).overwriteFrame(clipped, 3.25, Palette::White, aa);
				for (int32 y = 0; y < 19; ++y)
					for (int32 x = 0; x < 21; ++x)
						if ((x + offset.x < 48) && (y + offset.y < 48)) expected[y][x] = full[y + offset.y][x + offset.x];
				CAPTURE(n, bool(aa), offset);
				CHECK((clipped == expected));
			}
		}
}

TEST_CASE("ImageDraw.benchmark_superellipse_frames", "[.benchmark]")
{
	ankerl::nanobench::Bench bench;
	bench.title("SuperEllipse frame contours").unit("draw").epochs(7).minEpochIterations(2);
	for (const int32 size : { 32, 256, 1024 })
		for (const double n : { 0.5, 1.0, 2.0, 4.0, 16.0 })
		{
			const SuperEllipse shape{ Vec2{ size * 0.5 + 0.25, size * 0.5 + 0.75 }, size * 0.4, size * 0.24, n };
			Image image{ size, Color{ 40, 70, 100, 255 } };
			for (const double thickness : { 3.5, size * 0.15 })
				for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
					for (const bool paint : { false, true })
						for (const bool adaptive : { false, true })
						{
							const std::string name = std::to_string(size) + "/n=" + std::to_string(n)
								+ (thickness == 3.5 ? "/thin" : "/wide") + (bool(aa) ? "/AA" : "/NoAA")
								+ (paint ? "/paint" : "/overwrite") + (adaptive ? "/adaptive" : "/outer");
							image.fill(Color{ 40, 70, 100, 255 });
							bench.run(name, [&]
							{
								const Color color{ 211, 83, 147, 113 };
								const auto mode = paint ? ImagePixel::BlendMode::SourceOver : ImagePixel::BlendMode::Overwrite;
								if (adaptive) ImageDraw::SuperEllipseFrame(image, shape, thickness, color, mode, aa);
								else ImageDraw::ClosedLineString(image, shape.outer(QualityFactor{ 1.0 }), thickness, color, mode, aa);
								ankerl::nanobench::doNotOptimizeAway(image.data());
							});
							CHECK(std::any_of(image.begin(), image.end(), [](Color c) { return c.r != 40; }));
						}
		}
}

TEST_CASE("ImageDraw.ellipse_pie_matches_polar_samples")
{
	// Independent per-point polar-angle and implicit-ellipse reference. It does
	// not construct horizontal intervals or use the rasterizer's half-plane tests.
	for (const Ellipse ellipse : { Ellipse{ 16.31, 14.73, 13.4, 8.2 }, Ellipse{ 2.31, -0.27, 13.4, 8.2 },
		Ellipse{ 16.31, 14.73, 14, 0.35 }, Ellipse{ 16.31, 14.73, 0.35, 14 }, Ellipse{ 16.31, 14.73, 0.3, 0.2 } })
		for (const double start : { 0.0, Math::HalfPi, -Math::Pi, 0.37, -7.25 })
			for (const double sweep : { 0.001, 1.2, Math::Pi, 4.5, -1.2, -4.5, Math::TwoPi - 0.001 })
				for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
				{
					Image actual{ 35, 31, Color{ 0, 0 } }, expected = actual;
					ellipse.overwritePie(actual, start, sweep, Palette::White, aa);
					const int32 samples = aa ? 4 : 1;
					const double clockwiseStart = (sweep > 0 ? start : start + sweep);
					for (int32 y = 0; y < 31; ++y)
						for (int32 x = 0; x < 35; ++x)
						{
							int32 covered = 0;
							for (int32 sy = 0; sy < samples; ++sy)
								for (int32 sx = 0; sx < samples; ++sx)
								{
									const Vec2 delta{ x + (sx + 0.5) / samples - ellipse.x, y + (sy + 0.5) / samples - ellipse.y };
									const Vec2 unit = delta / ellipse.axes;
									double angle = std::fmod(std::atan2(delta.x, -delta.y) - clockwiseStart, Math::TwoPi);
									if (angle < 0) angle += Math::TwoPi;
									covered += ((unit.lengthSq() <= 1.0) && (angle <= Abs(sweep)));
								}
							const uint8 c = uint8((covered * 255 + samples * samples / 2) / (samples * samples));
							expected[y][x] = Color{ c, c };
						}
					CAPTURE(ellipse, start, sweep, bool(aa));
					CHECK((actual == expected));
				}
}

TEST_CASE("ImageDraw.ellipse_pie_direction_full_sweeps_and_empty_inputs")
{
	const Color color{ 211, 83, 147, 113 };
	const Ellipse ellipse{ 16.31, 14.73, 13.4, 8.2 };
	const Image background = Background(Size{ 35, 31 });
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		for (const bool paint : { false, true })
		{
			for (const double sweep : { Math::TwoPi, -Math::TwoPi, 7.5, -12.0 })
			{
				Image actual = background, expected = background;
				if (paint)
				{
					CHECK(&ellipse.paintPie(actual, 0.31, sweep, color, aa) == &ellipse);
					ellipse.paint(expected, color, aa);
				}
				else
				{
					CHECK(&ellipse.overwritePie(actual, 0.31, sweep, color, aa) == &ellipse);
					ellipse.overwrite(expected, color, aa);
				}
				CHECK((actual == expected));
			}
			for (const double sweep : { 0.001, 1.2, Math::Pi, 4.5, Math::TwoPi - 0.001 })
			{
				Image actual = background, reversed = background, rotated = background;
				const auto mode = paint ? ImagePixel::BlendMode::SourceOver : ImagePixel::BlendMode::Overwrite;
				ImageDraw::EllipsePie(actual, ellipse, 0.25, sweep, color, mode, aa);
				ImageDraw::EllipsePie(reversed, ellipse, 0.25 + sweep, -sweep, color, mode, aa);
				ImageDraw::EllipsePie(rotated, ellipse, 0.25 + Math::TwoPi * 3, sweep, color, mode, aa);
				CHECK((actual == reversed));
				CHECK((actual == rotated));
				Image opaque{ 35, 31, Color{ 40, 70, 100, 255 } }, general = opaque;
				ImageDraw::EllipsePie(opaque, ellipse, 0.25, sweep, color, mode, aa, ImageDraw::DstAlpha::AssumeOpaque);
				ImageDraw::EllipsePie(general, ellipse, 0.25, sweep, color, mode, aa);
				CHECK((opaque == general));
			}
			Image actual = background;
			for (const Ellipse invalid : { Ellipse{ 16, 14, 0, 8 }, Ellipse{ 16, 14, 13, 0 },
				Ellipse{ 16, 14, -13, 8 }, Ellipse{ 16, 14, 13, -8 } })
				for (const double sweep : { 1.2, -4.5, Math::TwoPi })
				{
					if (paint) invalid.paintPie(actual, 0.3, sweep, color, aa);
					else invalid.overwritePie(actual, 0.3, sweep, color, aa);
				}
			if (paint) ellipse.paintPie(actual, 0.3, 0, color, aa);
			else ellipse.overwritePie(actual, 0.3, 0, color, aa);
			CHECK((actual == background));
			Image empty;
			if (paint) ellipse.paintPie(empty, 0.3, 1.2, color, aa);
			else ellipse.overwritePie(empty, 0.3, 1.2, color, aa);
			CHECK(empty.isEmpty());
		}
	Image actual = background, explicitAA = background, direct = background;
	ellipse.paintPie(actual, 0.3, 4.5, color);
	ellipse.paintPie(explicitAA, 0.3, 4.5, color, EnableAntialiasing::Yes);
	ImageDraw::EllipsePie(direct, ellipse, 0.3, 4.5, color);
	CHECK((actual == explicitAA));
	CHECK((actual == direct));
	actual = explicitAA = background;
	ellipse.overwritePie(actual, 0.3, 4.5, color);
	ellipse.overwritePie(explicitAA, 0.3, 4.5, color, EnableAntialiasing::Yes);
	CHECK((actual == explicitAA));
}

TEST_CASE("ImageDraw.ellipse_pie_cardinal_bounds_and_circle_reduction")
{
	const Color color{ 211, 83, 147, 113 };
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
	{
		for (const double start : { 0.0, Math::HalfPi, Math::Pi, Math::Pi * 1.5 })
			for (const double sweep : { 0.001, 1.2, Math::HalfPi, Math::Pi, 4.5, -4.5 })
			{
				Image actual = Background(Size{ 35, 31 }), expected = actual;
				Ellipse{ 16.31, 14.73, 12.25, 12.25 }.paintPie(actual, start, sweep, color, aa);
				Circle{ 16.31, 14.73, 12.25 }.paintPie(expected, start, sweep, color, aa);
				CAPTURE(start, sweep, bool(aa));
				CHECK((actual == expected));
			}
	}
	// The center belongs to every nonempty pie. The start ray at zero degrees
	// contains a pixel-center column, including the ellipse's upper tangent.
	Image image{ 25, 25, Color{ 0, 0 } };
	const Ellipse ellipse{ 12.5, 12.5, 10, 6 };
	ellipse.overwritePie(image, 0, 0.001, Palette::White, EnableAntialiasing::No);
	CHECK(image[6][12] == Palette::White);
	CHECK(image[12][12] == Palette::White);
	CHECK(image[13][12].a == 0);
	CHECK(image[10][11].a == 0);
	CHECK(image[10][13].a == 0);
}

TEST_CASE("ImageDraw.benchmark_ellipse_pies", "[.benchmark]")
{
	ankerl::nanobench::Bench bench;
	bench.title("Ellipse pies: analytic spans and prebuilt polygons").unit("draw").epochs(7).minEpochIterations(2);
	for (const int32 size : { 32, 256, 1024 })
		for (const double aspect : { 0.6, 0.05 })
			for (const double sweep : { 0.15, 1.2, 4.5 })
			{
				const Ellipse ellipse{ Vec2{ size * 0.5 + 0.25, size * 0.5 + 0.75 }, size * 0.4, size * 0.4 * aspect };
				constexpr double Start = 0.37;
				// Convert geometric ray angles to ellipse parameters, then sample the arc
				// uniformly in that parameter so thin ellipses also have a useful reference.
				const auto parameter = [&](double angle)
				{
					return std::atan2(ellipse.b * std::sin(angle), ellipse.a * std::cos(angle));
				};
				const double startParameter = parameter(Start);
				double parameterSweep = (parameter(Start + sweep) - startParameter);
				if (parameterSweep < 0) parameterSweep += Math::TwoPi;
				const int32 segments = Max(2, int32(std::ceil(QualityFactor{ 2.0 }.toPointsPerCircle(ellipse.a).value() * parameterSweep / Math::TwoPi)));
				Array<Vec2> points{ ellipse.center };
				for (int32 i = 0; i <= segments; ++i)
				{
					points.push_back(ellipse.getPointByAngle(startParameter + parameterSweep * i / segments));
				}
				const Polygon polygon{ points };
				REQUIRE(polygon);
				Image image{ size, Color{ 40, 70, 100, 255 } };
				for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
					for (const bool paint : { false, true })
						for (const bool analytic : { false, true })
						{
							const std::string name = std::to_string(size) + "/aspect=" + std::to_string(aspect) + "/sweep=" + std::to_string(sweep)
								+ (bool(aa) ? "/AA" : "/NoAA") + (paint ? "/paint" : "/overwrite") + (analytic ? "/spans" : "/polygon");
							image.fill(Color{ 40, 70, 100, 255 });
							bench.run(name, [&]
							{
								const Color color{ 211, 83, 147, 113 };
								if (analytic)
								{
									if (paint) ellipse.paintPie(image, Start, sweep, color, aa);
									else ellipse.overwritePie(image, Start, sweep, color, aa);
								}
								else
								{
									if (paint) polygon.paint(image, color, aa);
									else polygon.overwrite(image, color, aa);
								}
								ankerl::nanobench::doNotOptimizeAway(image.data());
							});
						}
			}
}

TEST_CASE("ImageDraw.circle_segment_matches_implicit_samples")
{
	for (const Circle circle : { Circle{ 16.31, 14.73, 12.25 }, Circle{ 2.31, -0.27, 12.25 },
		Circle{ 16.31, 14.73, 0.3 }, Circle{ 16.31, 14.73, 1.2 }, Circle{ -30, 14.73, 12.25 } })
		for (const double midpoint : { 0.0, Math::HalfPi, Math::Pi, 0.37, -7.25 })
			for (const double ratio : { 0.001, 0.05, 0.5, 1.0, 1.5, 1.999 })
				for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
				{
					Image actual{ 35, 31, Color{ 0, 0 } }, expected = actual;
					const double height = (circle.r * ratio);
					circle.overwriteSegment(actual, midpoint, height, Palette::White, aa);
					const int32 samples = aa ? 4 : 1;
					// Independent point tests: implicit disk and projection onto the arc midpoint.
					for (int32 y = 0; y < 31; ++y)
						for (int32 x = 0; x < 35; ++x)
						{
							int32 covered = 0;
							for (int32 sy = 0; sy < samples; ++sy)
								for (int32 sx = 0; sx < samples; ++sx)
								{
									const Vec2 p{ x + (sx + 0.5) / samples - circle.x, y + (sy + 0.5) / samples - circle.y };
									covered += ((p.lengthSq() <= circle.r * circle.r)
										&& (circle.r - height <= p.x * std::sin(midpoint) - p.y * std::cos(midpoint)));
								}
							const uint8 c = uint8((covered * 255 + samples * samples / 2) / (samples * samples));
							expected[y][x] = Color{ c, c };
						}
					CAPTURE(circle, midpoint, ratio, bool(aa));
					CHECK((actual == expected));
				}
}

TEST_CASE("ImageDraw.circle_segment_angle_samples_and_direction")
{
	for (const Circle circle : { Circle{ 16.31, 14.73, 12.25 }, Circle{ 2.31, -0.27, 12.25 }, Circle{ 16.31, 14.73, 0.3 } })
		for (const double start : { 0.0, Math::HalfPi, 0.37, -7.25 })
			for (const double sweep : { 0.001, 1.2, Math::Pi, 4.5, -1.2, -4.5, Math::TwoPi - 0.001 })
				for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
				{
					Image actual{ 35, 31, Color{ 0, 0 } }, expected = actual, reversed = actual;
					circle.overwriteSegmentFromAngles(actual, start, sweep, Palette::White, aa);
					circle.overwriteSegmentFromAngles(reversed, start + sweep, -sweep, Palette::White, aa);
					const int32 samples = aa ? 4 : 1;
					// Construct the directed chord from the arc endpoints, independently of height conversion.
					const double clockwiseStart = (sweep < 0 ? start + sweep : start);
					const Vec2 a = circle.getPointByAngle(clockwiseStart);
					const Vec2 b = circle.getPointByAngle(clockwiseStart + Abs(sweep));
					for (int32 y = 0; y < 31; ++y)
						for (int32 x = 0; x < 35; ++x)
						{
							int32 covered = 0;
							for (int32 sy = 0; sy < samples; ++sy)
								for (int32 sx = 0; sx < samples; ++sx)
								{
									const Vec2 p{ x + (sx + 0.5) / samples, y + (sy + 0.5) / samples };
									covered += (((p - circle.center).lengthSq() <= circle.r * circle.r) && ((b - a).cross(p - a) <= 0));
								}
							const uint8 c = uint8((covered * 255 + samples * samples / 2) / (samples * samples));
							expected[y][x] = Color{ c, c };
						}
					CAPTURE(circle, start, sweep, bool(aa));
					CHECK((actual == expected));
					CHECK((actual == reversed));
				}
}

TEST_CASE("ImageDraw.circle_segment_full_empty_defaults_and_alpha")
{
	const Circle circle{ 16.31, 14.73, 12.25 };
	const Color color{ 211, 83, 147, 113 };
	const Image background = Background(Size{ 35, 31 });
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		for (const bool paint : { false, true })
		{
			const auto mode = paint ? ImagePixel::BlendMode::SourceOver : ImagePixel::BlendMode::Overwrite;
			for (const double height : { 2 * circle.r, 3 * circle.r })
			{
				Image actual = background, expected = background;
				if (paint) CHECK(&circle.paintSegment(actual, 0.31, height, color, aa) == &circle);
				else CHECK(&circle.overwriteSegment(actual, 0.31, height, color, aa) == &circle);
				ImageDraw::Fill(expected, circle, color, mode, aa);
				CHECK((actual == expected));
			}
			for (const double sweep : { Math::TwoPi, -Math::TwoPi, 7.5, -12.0 })
			{
				Image actual = background, expected = background;
				if (paint) CHECK(&circle.paintSegmentFromAngles(actual, 0.31, sweep, color, aa) == &circle);
				else CHECK(&circle.overwriteSegmentFromAngles(actual, 0.31, sweep, color, aa) == &circle);
				ImageDraw::Fill(expected, circle, color, mode, aa);
				CHECK((actual == expected));
			}
			Image actual = background;
			for (const double radius : { -12.25, 0.0 })
				for (const double height : { 3.0, 30.0 })
				{
					const Circle invalid{ circle.center, radius };
					ImageDraw::CircleSegment(actual, invalid, 0.3, height, color, mode, aa);
					ImageDraw::CircleSegmentFromAngles(actual, invalid, 0.3, height, color, mode, aa);
				}
			for (const double height : { -1.0, 0.0 }) ImageDraw::CircleSegment(actual, circle, 0.3, height, color, mode, aa);
			ImageDraw::CircleSegmentFromAngles(actual, circle, 0.3, 0, color, mode, aa);
			CHECK((actual == background));
			for (const double height : { 2.0, 12.25, 22.0, 30.0 })
			{
				Image opaque{ 35, 31, Color{ 40, 70, 100, 255 } }, general = opaque;
				ImageDraw::CircleSegment(opaque, circle, 0.37, height, color, mode, aa, ImageDraw::DstAlpha::AssumeOpaque);
				ImageDraw::CircleSegment(general, circle, 0.37, height, color, mode, aa);
				CHECK((opaque == general));
			}
			for (const double sweep : { 1.2, -4.5, Math::TwoPi })
			{
				Image opaque{ 35, 31, Color{ 40, 70, 100, 255 } }, general = opaque;
				ImageDraw::CircleSegmentFromAngles(opaque, circle, 0.37, sweep, color, mode, aa, ImageDraw::DstAlpha::AssumeOpaque);
				ImageDraw::CircleSegmentFromAngles(general, circle, 0.37, sweep, color, mode, aa);
				CHECK((opaque == general));
			}
		}
	for (const bool angles : { false, true })
		for (const bool paint : { false, true })
		{
			Image actual = background, direct = background, explicitAA = background;
			const auto mode = paint ? ImagePixel::BlendMode::SourceOver : ImagePixel::BlendMode::Overwrite;
			if (angles)
			{
				if (paint) circle.paintSegmentFromAngles(actual, 0.37, -4.5, color);
				else circle.overwriteSegmentFromAngles(actual, 0.37, -4.5, color);
				ImageDraw::CircleSegmentFromAngles(direct, circle, 0.37, -4.5, color, mode);
				ImageDraw::CircleSegmentFromAngles(explicitAA, circle, 0.37, -4.5, color, mode, EnableAntialiasing::Yes);
			}
			else
			{
				if (paint) circle.paintSegment(actual, 0.37, 18.5, color);
				else circle.overwriteSegment(actual, 0.37, 18.5, color);
				ImageDraw::CircleSegment(direct, circle, 0.37, 18.5, color, mode);
				ImageDraw::CircleSegment(explicitAA, circle, 0.37, 18.5, color, mode, EnableAntialiasing::Yes);
			}
			CHECK((actual == direct));
			CHECK((actual == explicitAA));
		}
}

TEST_CASE("ImageDraw.circle_segment_chord_boundary_and_half_circle")
{
	const Circle circle{ 12.5, 12.5, 6 };
	Image image{ 25, 25, Color{ 0, 0 } };
	circle.overwriteSegment(image, 0, 4, Palette::White, EnableAntialiasing::No);
	CHECK(image[6][12] == Palette::White);
	CHECK(image[10][12] == Palette::White);
	CHECK(image[11][12].a == 0);
	CHECK(image[5][12].a == 0);
	CHECK(image[10][18].a == 0);
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
	{
		Image actual{ 25, 25, Color{ 0, 0 } }, expected = actual;
		circle.overwriteSegment(actual, 0, circle.r, Palette::White, aa);
		// An upward semicircle, including its diameter and the left/right circle tangents.
		for (int32 y = 0; y < 25; ++y)
			for (int32 x = 0; x < 25; ++x)
			{
				const int32 samples = aa ? 4 : 1;
				int32 covered = 0;
				for (int32 sy = 0; sy < samples; ++sy)
					for (int32 sx = 0; sx < samples; ++sx)
					{
						const Vec2 p{ x + (sx + 0.5) / samples - circle.x, y + (sy + 0.5) / samples - circle.y };
						covered += ((p.y <= 0) && (p.lengthSq() <= 36));
					}
				const uint8 c = uint8((covered * 255 + samples * samples / 2) / (samples * samples));
				expected[y][x] = Color{ c, c };
			}
		CHECK((actual == expected));
	}
}

TEST_CASE("ImageDraw.benchmark_circle_segments", "[.benchmark]")
{
	ankerl::nanobench::Bench bench;
	bench.title("Circle segments: analytic spans and prebuilt polygons").unit("draw").epochs(7).minEpochIterations(2);
	for (const int32 size : { 32, 256, 1024 })
		for (const double ratio : { 0.05, 0.5, 1.8 })
			for (const double midpoint : { 0.0, 0.61 })
			{
				const Circle circle{ Vec2{ size * 0.5 + 0.25, size * 0.5 + 0.75 }, size * 0.4 };
				const double halfAngle = std::acos(1.0 - ratio);
				const int32 segments = Max(2, int32(std::ceil(QualityFactor{ 2.0 }.toPointsPerCircle(circle.r).value() * halfAngle / Math::Pi)));
				Array<Vec2> points;
				for (int32 i = 0; i <= segments; ++i) points.push_back(circle.getPointByAngle(midpoint - halfAngle + 2 * halfAngle * i / segments));
				const Polygon polygon{ points };
				REQUIRE(polygon);
				Image image{ size, Color{ 40, 70, 100, 255 } };
				for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
					for (const bool paint : { false, true })
						for (const bool analytic : { false, true })
						{
							const std::string name = std::to_string(size) + "/height=" + std::to_string(ratio) + "/angle=" + std::to_string(midpoint)
								+ (bool(aa) ? "/AA" : "/NoAA") + (paint ? "/paint" : "/overwrite") + (analytic ? "/spans" : "/polygon");
							image.fill(Color{ 40, 70, 100, 255 });
							bench.run(name, [&]
							{
								const Color color{ 211, 83, 147, 113 };
								if (analytic)
								{
									if (paint) circle.paintSegment(image, midpoint, circle.r * ratio, color, aa);
									else circle.overwriteSegment(image, midpoint, circle.r * ratio, color, aa);
								}
								else
								{
									if (paint) polygon.paint(image, color, aa);
									else polygon.overwrite(image, color, aa);
								}
								ankerl::nanobench::doNotOptimizeAway(image.data());
							});
						}
			}
}

TEST_CASE("ImageDraw.benchmark_circle_arcs", "[.benchmark]")
{
	ankerl::nanobench::Bench bench;
	bench.title("Circle arcs").unit("draw").epochs(7).minEpochIterations(2);
	for (const int32 size : { 32, 256, 1024 })
		for (const double sweep : { 0.15, 1.2, 4.5 })
			for (const double thickness : { 3.5, size * 0.2 })
			{
				const Circle circle{ size * 0.5 + 0.25, size * 0.5 + 0.75, size * 0.35 };
				Image image{ size, Color{ 40, 70, 100, 255 } };
				for (const LineCap cap : { LineCap::Flat, LineCap::Round, LineCap::Square })
					for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
						for (const bool paint : { false, true })
						{
							const std::string name = std::to_string(size) + "/sweep=" + std::to_string(sweep)
								+ (thickness == 3.5 ? "/thin" : "/wide") + (bool(aa) ? "/AA" : "/NoAA")
								+ (paint ? "/paint" : "/overwrite") + (cap == LineCap::Flat ? "/Flat" : (cap == LineCap::Round ? "/Round" : "/Square"));
							image.fill(Color{ 40, 70, 100, 255 });
							bench.run(name, [&]
							{
								const Color color{ 211, 83, 147, 113 };
								if (cap == LineCap::Flat)
								{
									if (paint) circle.paintArc(image, 0.37, sweep, thickness * 0.3, thickness * 0.7, color, aa);
									else circle.overwriteArc(image, 0.37, sweep, thickness * 0.3, thickness * 0.7, color, aa);
								}
								else
								{
									if (paint) circle.paintArc(image, cap, 0.37, sweep, thickness * 0.3, thickness * 0.7, color, aa);
									else circle.overwriteArc(image, cap, 0.37, sweep, thickness * 0.3, thickness * 0.7, color, aa);
								}
								ankerl::nanobench::doNotOptimizeAway(image.data());
							});
							CHECK(std::any_of(image.begin(), image.end(), [](Color c) { return c.r != 40; }));
						}
			}
}

namespace
{
	Image CircleArcReference(const Size size, const Circle& circle, const LineCap cap,
		const double start, const double sweep, const double inner, const double outer, const EnableAntialiasing aa)
	{
		Image expected{ size, Color{ 0, 0 } };
		const double ri = Max(0.0, circle.r - inner), ro = (circle.r + outer);
		const double mid = (ri + ro) * 0.5, half = (ro - ri) * 0.5;
		const double begin = (sweep < 0 ? start + sweep : start), end = (begin + Abs(sweep));
		const Vec2 directions[]{ { std::sin(begin), -std::cos(begin) }, { std::sin(end), -std::cos(end) } };
		const int32 samples = aa ? 4 : 1;
		for (int32 y = 0; y < size.y; ++y)
			for (int32 x = 0; x < size.x; ++x)
			{
				int32 covered = 0;
				for (int32 sy = 0; sy < samples; ++sy)
					for (int32 sx = 0; sx < samples; ++sx)
					{
						const Vec2 p{ x + (sx + 0.5) / samples - circle.x, y + (sy + 0.5) / samples - circle.y };
						double theta = std::fmod(std::atan2(p.x, -p.y) - begin, Math::TwoPi);
						if (theta < 0) theta += Math::TwoPi;
						bool inside = ((sweep != 0) && (ri * ri <= p.lengthSq()) && (p.lengthSq() <= ro * ro)
							&& ((p == Vec2{ 0, 0 }) || (theta <= Abs(sweep))));
						for (int32 i = 0; i < 2; ++i)
						{
							const Vec2 q = (p - directions[i] * mid);
							if (cap == LineCap::Round) inside |= (q.lengthSq() <= half * half);
							else if (cap == LineCap::Square)
							{
								const Vec2 tangent{ -directions[i].y, directions[i].x };
								const double along = q.dot(tangent) * (i == 0 ? -1 : 1);
								inside |= ((0 <= along) && (along <= half) && (Abs(q.dot(directions[i])) <= half));
							}
						}
						covered += inside;
					}
				const uint8 coverage = uint8((covered * 255 + samples * samples / 2) / (samples * samples));
				expected[y][x] = Color{ coverage, coverage };
			}
		return expected;
	}
}

TEST_CASE("ImageDraw.circle_arc_flat_sampling_reference")
{
	for (const Circle circle : { Circle{ 16.31, 14.73, 12.25 }, Circle{ 2.31, -0.27, 12.25 }, Circle{ 16.31, 14.73, 0.3 } })
		for (const Vec2 thickness : { Vec2{ 0.2, 0.1 }, Vec2{ 2.0, 3.0 }, Vec2{ 18, 1.5 } })
			for (const double start : { 0.0, Math::HalfPi, 0.37 })
				for (const double sweep : { 0.001, 1.2, Math::Pi, -4.5, Math::TwoPi - 0.001 })
					for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
					{
						Image actual{ 35, 31, Color{ 0, 0 } };
						circle.overwriteArc(actual, start, sweep, thickness.x, thickness.y, Palette::White, aa);
						CAPTURE(circle, thickness, start, sweep, bool(aa));
						CHECK((actual == CircleArcReference(actual.size(), circle, LineCap::Flat, start, sweep, thickness.x, thickness.y, aa)));
					}
}

TEST_CASE("ImageDraw.circle_arc_caps_sampling_reference")
{
	for (const LineCap cap : { LineCap::Round, LineCap::Square })
		for (const Circle circle : { Circle{ 16.31, 14.73, 12.25 }, Circle{ 2.31, -0.27, 12.25 }, Circle{ 16.31, 14.73, 0.3 } })
			for (const Vec2 thickness : { Vec2{ 0.2, 0.1 }, Vec2{ 2.0, 3.0 }, Vec2{ 18, 1.5 } })
				for (const double start : { 0.0, Math::HalfPi, 0.37 })
					for (const double sweep : { 0.0, 0.03, Math::Pi, -4.5, Math::TwoPi - 0.03 })
						for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
						{
							Image actual{ 35, 31, Color{ 0, 0 } }, reversed = actual;
							circle.overwriteArc(actual, cap, start, sweep, thickness.x, thickness.y, Palette::White, aa);
							circle.overwriteArc(reversed, cap, start + sweep, -sweep, thickness.x, thickness.y, Palette::White, aa);
							CAPTURE(cap, circle, thickness, start, sweep, bool(aa));
							CHECK((actual == CircleArcReference(actual.size(), circle, cap, start, sweep, thickness.x, thickness.y, aa)));
							CHECK((actual == reversed));
						}
}

TEST_CASE("ImageDraw.circle_arc_cap_full_empty_defaults_and_alpha")
{
	const Circle circle{ 16.31, 14.73, 12.25 };
	const Color color{ 211, 83, 147, 113 };
	const Image background = Background(Size{ 35, 31 });
	for (const LineCap cap : { LineCap::Flat, LineCap::Round, LineCap::Square })
		for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
			for (const bool paint : { false, true })
			{
				const auto mode = paint ? ImagePixel::BlendMode::SourceOver : ImagePixel::BlendMode::Overwrite;
				for (const double sweep : { Math::TwoPi, -Math::TwoPi, 7.5, -12.0 })
					for (const double inner : { 2.0, 18.0 })
					{
						Image actual = background, expected = background;
						if (paint) CHECK(&circle.paintArc(actual, cap, 0.31, sweep, inner, 3, color, aa) == &circle);
						else CHECK(&circle.overwriteArc(actual, cap, 0.31, sweep, inner, 3, color, aa) == &circle);
						ImageDraw::CircleFrame(expected, circle, inner, 3, color, mode, aa);
						CHECK((actual == expected));
					}
				Image actual = background;
				for (const Vec2 thickness : { Vec2{ -1, 2 }, Vec2{ 2, -1 }, Vec2{ 0, 0 } })
					for (const double sweep : { 0.0, 1.2, Math::TwoPi })
						ImageDraw::CircleArc(actual, circle, cap, 0.3, sweep, thickness.x, thickness.y, color, mode, aa);
				for (const double radius : { -12.25, 0.0 })
					ImageDraw::CircleArc(actual, Circle{ circle.center, radius }, cap, 0.3, 1.2, 2, 3, color, mode, aa);
				CHECK((actual == background));
				for (const double sweep : { 0.0, 0.03, -4.5, Math::TwoPi - 0.03, Math::TwoPi })
				{
					Image opaque{ 35, 31, Color{ 40, 70, 100, 255 } }, general = opaque;
					ImageDraw::CircleArc(opaque, circle, cap, 0.37, sweep, 2, 3, color, mode, aa, ImageDraw::DstAlpha::AssumeOpaque);
					ImageDraw::CircleArc(general, circle, cap, 0.37, sweep, 2, 3, color, mode, aa);
					CHECK((opaque == general));
				}
				if (cap == LineCap::Flat)
					for (const double sweep : { 0.0, 1.2, -4.5, Math::TwoPi })
					{
						Image explicitCap = background, old = background;
						ImageDraw::CircleArc(explicitCap, circle, cap, 0.37, sweep, 2, 3, color, mode, aa);
						if (paint) circle.paintArc(old, 0.37, sweep, 2, 3, color, aa);
						else circle.overwriteArc(old, 0.37, sweep, 2, 3, color, aa);
						CHECK((explicitCap == old));
					}
			}
	for (const LineCap cap : { LineCap::Flat, LineCap::Round, LineCap::Square })
		for (const bool paint : { false, true })
		{
			Image actual = background, direct = background, explicitAA = background;
			const auto mode = paint ? ImagePixel::BlendMode::SourceOver : ImagePixel::BlendMode::Overwrite;
			if (paint) circle.paintArc(actual, cap, 0.37, -4.5, 2, 3, color);
			else circle.overwriteArc(actual, cap, 0.37, -4.5, 2, 3, color);
			ImageDraw::CircleArc(direct, circle, cap, 0.37, -4.5, 2, 3, color, mode);
			ImageDraw::CircleArc(explicitAA, circle, cap, 0.37, -4.5, 2, 3, color, mode, EnableAntialiasing::Yes);
			CHECK((actual == direct));
			CHECK((actual == explicitAA));
		}
}

TEST_CASE("ImageDraw.circle_arc_cap_boundary_and_translucent_union")
{
	const Circle circle{ 12.5, 12.5, 6 };
	Image round{ 25, 25, Color{ 0, 0 } }, square = round, flat = round;
	for (const LineCap cap : { LineCap::Flat, LineCap::Round, LineCap::Square })
	{
		Image zero = flat;
		circle.overwriteArc(zero, cap, 0, Math::HalfPi, 0, 0, Palette::White, EnableAntialiasing::No);
		CHECK((zero == flat));
	}
	circle.overwriteArc(round, LineCap::Round, 0, 0, 2, 2, Palette::White, EnableAntialiasing::No);
	circle.overwriteArc(square, LineCap::Square, 0, 0, 2, 2, Palette::White, EnableAntialiasing::No);
	circle.overwriteArc(flat, LineCap::Flat, 0, 0, 2, 2, Palette::White, EnableAntialiasing::No);
	CHECK(std::all_of(flat.begin(), flat.end(), [](Color c) { return c.a == 0; }));
	CHECK(round[4][12] == Palette::White);
	CHECK(round[8][12] == Palette::White);
	CHECK(round[6][10] == Palette::White);
	CHECK(round[4][10].a == 0);
	CHECK(square[4][10] == Palette::White);
	CHECK(square[8][14] == Palette::White);
	circle.overwriteArc(flat, 0, Math::HalfPi, 2, 2, Palette::White, EnableAntialiasing::No);
	CHECK(flat[4][12] == Palette::White); // outer circle tangent
	CHECK(flat[8][12] == Palette::White); // inner circle tangent
	CHECK(flat[9][12].a == 0);

	for (const LineCap cap : { LineCap::Round, LineCap::Square })
		for (const double sweep : { 0.0, 0.03, Math::TwoPi - 0.03 })
			for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
			{
				const Circle shape{ 16.31, 14.73, 8.25 };
				const Image mask = CircleArcReference(Size{ 35, 31 }, shape, cap, 0.37, sweep, 3, 6, aa);
				for (const bool paint : { false, true })
					for (const uint8 alpha : { uint8{ 0 }, uint8{ 113 }, uint8{ 255 } })
					{
						Image actual = Background(mask.size()), expected = actual;
						const Color color{ 211, 83, 147, alpha };
						for (size_t i = 0; i < mask.size_bytes() / sizeof(Color); ++i) expected.data()[i] = Composite(color, expected.data()[i], mask.data()[i].a, paint);
						if (paint) shape.paintArc(actual, cap, 0.37, sweep, 3, 6, color, aa);
						else shape.overwriteArc(actual, cap, 0.37, sweep, 3, 6, color, aa);
						CAPTURE(cap, sweep, bool(aa), paint, alpha);
						CHECK((actual == expected));
					}
			}
}

namespace
{
	Image CircleFrameDistanceMask(const Size size, const Circle& circle, const double inner, const double outer)
	{
		Image mask{ size, Color{ 0, 0 } };
		const double innerRadius = (circle.r - inner), outerRadius = (circle.r + outer);
		// Full-image distance reference: no row spans or interior classification.
		const auto coverage = [](const double distance)
		{
			return static_cast<int32>(std::clamp(0.5 - distance, 0.0, 1.0) * 255.0 + 0.5);
		};
		for (int32 y = 0; y < size.y; ++y)
			for (int32 x = 0; x < size.x; ++x)
			{
				const double dx = (x + 0.5 - circle.x), dy = (y + 0.5 - circle.y);
				const double distance = std::sqrt(dx * dx + dy * dy);
				const int32 outerCoverage = coverage(distance - outerRadius);
				const int32 innerCoverage = (0 < innerRadius ? coverage(distance - innerRadius) : 0);
				const uint8 value = static_cast<uint8>(Max(0, outerCoverage - innerCoverage));
				mask[y][x] = Color{ value, value };
			}
		return mask;
	}
}

TEST_CASE("ImageDraw.circle_frame_aa_distance_reference")
{
	for (const Size size : { Size{ 37, 31 }, Size{ 1, 31 }, Size{ 37, 1 } })
		for (const Vec2 center : { Vec2{ 16.25, 14.75 }, Vec2{ 16.5, 14.5 }, Vec2{ -0.25, 0.75 }, Vec2{ -20.5, 14.5 } })
			for (const double radius : { 0.2, 0.5, 1.0, 2.5, 8.25, 13.0, 100.0 })
				for (const Vec2 thickness : { Vec2{ 0.0, 0.1 }, Vec2{ 0.1, 0.0 }, Vec2{ 0.25, 0.25 },
					Vec2{ 0.5, 0.5 }, Vec2{ 2, 3 }, Vec2{ radius - 0.25, 0.0 } })
				{
					if (thickness.x < 0) continue;
					const Circle circle{ center, radius };
					const Image mask = CircleFrameDistanceMask(size, circle, thickness.x, thickness.y);
					for (const bool paint : { false, true })
						for (const uint8 alpha : { uint8{ 0 }, uint8{ 113 }, uint8{ 255 } })
						{
							Image actual = Background(size), expected = actual;
							const Color color{ 211, 83, 147, alpha };
							for (size_t i = 0; i < (actual.size_bytes() / sizeof(Color)); ++i)
								expected.data()[i] = Composite(color, expected.data()[i], mask.data()[i].a, paint);
							if (paint) circle.paintFrame(actual, thickness.x, thickness.y, color);
							else circle.overwriteFrame(actual, thickness.x, thickness.y, color);
							CAPTURE(size, center, radius, thickness, paint, alpha);
							CHECK((actual == expected));
						}
				}
}

TEST_CASE("ImageDraw.circle_frame_aa_boundaries_and_alpha_mode")
{
	const Circle circle{ 12.5, 12.5, 6 };
	Image thin{ 25, 25, Color{ 0, 0 } };
	circle.overwriteFrame(thin, 0.25, 0.25, Palette::White);
	// Subtract the two independently rounded edge coverages: 191 - 64, not 128.
	CHECK(thin[12][18].a == 127);
	CHECK(thin[6][12].a == 127);
	CHECK(thin[12][12].a == 0);
	for (const double thickness : { 0.499999, 0.5, 0.500001, 2.0, 5.75 })
		for (const bool paint : { false, true })
		{
			const Color color{ 211, 83, 147, 113 };
			Image opaque{ 25, 25, Color{ 40, 70, 100, 255 } }, general = opaque, expected = opaque;
			const Image mask = CircleFrameDistanceMask(opaque.size(), circle, thickness, thickness);
			for (size_t i = 0; i < (expected.size_bytes() / sizeof(Color)); ++i)
				expected.data()[i] = Composite(color, expected.data()[i], mask.data()[i].a, paint);
			const auto mode = (paint ? ImagePixel::BlendMode::SourceOver : ImagePixel::BlendMode::Overwrite);
			ImageDraw::CircleFrame(opaque, circle, thickness, thickness, color, mode, EnableAntialiasing::Yes, ImageDraw::DstAlpha::AssumeOpaque);
			ImageDraw::CircleFrame(general, circle, thickness, thickness, color, mode);
			CAPTURE(thickness, paint);
			CHECK((opaque == expected));
			CHECK((general == expected));
		}
}

TEST_CASE("ImageDraw.circle_frame_aa_large_clipped_boundaries")
{
	for (const double radius : { 31.5, 256.25, 1024.5 })
		for (const Vec2 direction : { Vec2{ 1, 0 }, Vec2{ 0, 1 }, Vec2{ 0.6, 0.8 }, Vec2{ -0.6, -0.8 } })
			for (const double shift : { -0.001, 0.0, 0.001, 0.25 })
				for (const Vec2 thickness : { Vec2{ 0.03, 0.07 }, Vec2{ 0.25, 0.25 }, Vec2{ 0.5, 0.5 }, Vec2{ 2, 3 }, Vec2{ 20, 30 } })
				{
					const Circle circle{ Vec2{ 16.5 + shift, 14.5 } - direction * radius, radius };
					Image actual{ 37, 31, Color{ 0, 0 } };
					circle.overwriteFrame(actual, thickness.x, thickness.y, Palette::White);
					CAPTURE(radius, direction, shift, thickness);
					CHECK((actual == CircleFrameDistanceMask(actual.size(), circle, thickness.x, thickness.y)));
				}
}

TEST_CASE("ImageDraw.benchmark_circle_frames", "[.benchmark]")
{
	ankerl::nanobench::Bench bench;
	bench.title("AA Circle frames").unit("draw").epochs(9).minEpochIterations(3);
	for (const int32 size : { 8, 32, 256, 1024 })
		for (const double thickness : { 0.25, 3.5, size * 0.2 })
			for (const int32 placement : { 0, 1, 2 })
			{
				const Circle circle{ placement == 1 ? Vec2{ 0.25, -0.25 } : Vec2{ size * 0.5 + 0.25, size * 0.5 + 0.75 },
					(size * (placement == 2 ? 1.25 : 0.35)) };
				Image image{ size, Color{ 40, 70, 100, 255 } };
				for (const bool paint : { false, true })
					for (const uint8 alpha : { uint8{ 113 }, uint8{ 255 } })
					{
						const std::string name = std::to_string(size) + (thickness == 0.25 ? "/subpixel" : (thickness == 3.5 ? "/thin" : "/wide"))
							+ (placement == 0 ? "/inside" : (placement == 1 ? "/clipped" : "/hole"))
							+ (paint ? "/paint" : "/overwrite") + "/a=" + std::to_string(alpha);
						image.fill(Color{ 40, 70, 100, 255 });
						bench.run(name, [&]
						{
							const Color color{ 211, 83, 147, alpha };
							if (paint) circle.paintFrame(image, thickness * 0.3, thickness * 0.7, color);
							else circle.overwriteFrame(image, thickness * 0.3, thickness * 0.7, color);
							ankerl::nanobench::doNotOptimizeAway(image.data());
						});
						CHECK(std::any_of(image.begin(), image.end(), [](Color c) { return c.r != 40; }) == (placement != 2));
					}
			}
}
