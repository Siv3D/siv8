//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Geometry2D/Overlaps.hpp>

TEST_CASE("Geometry2D.Overlaps.Ellipse")
{
	const Ellipse ellipse{ Vec2{ 0, 0 }, 5, 3 };

	CHECK(Geometry2D::Overlaps(RectF{ -2, -2, 4, 4 }, ellipse));
	CHECK(not Geometry2D::Overlaps(RectF{ 5, -2, 4, 4 }, ellipse));
	CHECK(Geometry2D::Intersects(RectF{ 5, -2, 4, 4 }, ellipse));

	CHECK(Geometry2D::Overlaps(Circle{ Vec2{ 9, 0 }, 5 }, ellipse));
	CHECK(not Geometry2D::Overlaps(Circle{ Vec2{ 10, 0 }, 5 }, ellipse));
	CHECK(Geometry2D::Intersects(Circle{ Vec2{ 10, 0 }, 5 }, ellipse));

	CHECK(Geometry2D::Overlaps(ellipse, Ellipse{ Vec2{ 9, 0 }, 5, 3 }));
	CHECK(not Geometry2D::Overlaps(ellipse, Ellipse{ Vec2{ 10, 0 }, 5, 3 }));
	CHECK(Geometry2D::Overlaps(ellipse, Ellipse{ Vec2{ 0, 0 }, 1, 1 }));
	CHECK(not Geometry2D::Overlaps(ellipse, Ellipse{ Vec2{ 0, 0 }, 0, 3 }));
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipse")
{
	const SuperEllipse superEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 4.0 };

	CHECK(Geometry2D::Overlaps(RectF{ -2, -2, 4, 4 }, superEllipse));
	CHECK(not Geometry2D::Overlaps(RectF{ 5, -2, 4, 4 }, superEllipse));
	CHECK(Geometry2D::Intersects(RectF{ 5, -2, 4, 4 }, superEllipse));

	CHECK(Geometry2D::Overlaps(Circle{ Vec2{ 9, 0 }, 5 }, superEllipse));
	CHECK(not Geometry2D::Overlaps(Circle{ Vec2{ 10, 0 }, 5 }, superEllipse));

	CHECK(Geometry2D::Overlaps(superEllipse,
		SuperEllipse{ Vec2{ 9, 0 }, SizeF{ 5, 3 }, 4.0 }));
	CHECK(not Geometry2D::Overlaps(superEllipse,
		SuperEllipse{ Vec2{ 10, 0 }, SizeF{ 5, 3 }, 4.0 }));
	CHECK(not Geometry2D::Overlaps(superEllipse,
		SuperEllipse{ Vec2{ 0, 0 }, SizeF{ 0, 3 }, 4.0 }));
}

TEST_CASE("Geometry2D.Overlaps.RoundRect")
{
	const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2.0 };

	CHECK(Geometry2D::Overlaps(RectF{ 9, 2, 5, 6 }, roundRect));
	CHECK(not Geometry2D::Overlaps(RectF{ 10, 2, 5, 6 }, roundRect));
	CHECK(Geometry2D::Intersects(RectF{ 10, 2, 5, 6 }, roundRect));

	CHECK(Geometry2D::Overlaps(Circle{ Vec2{ 14, 5 }, 5 }, roundRect));
	CHECK(not Geometry2D::Overlaps(Circle{ Vec2{ 15, 5 }, 5 }, roundRect));
	CHECK(Geometry2D::Intersects(Circle{ Vec2{ 15, 5 }, 5 }, roundRect));

	CHECK(Geometry2D::Overlaps(roundRect, RoundRect{ RectF{ 9, 0, 10, 10 }, 2.0 }));
	CHECK(not Geometry2D::Overlaps(roundRect, RoundRect{ RectF{ 10, 0, 10, 10 }, 2.0 }));

	CHECK(Geometry2D::Overlaps(Ellipse{ Vec2{ 14, 5 }, 5, 3 }, roundRect));
	CHECK(not Geometry2D::Overlaps(Ellipse{ Vec2{ 15, 5 }, 5, 3 }, roundRect));

	CHECK(not Geometry2D::Overlaps(RoundRect{ RectF{ 5, 0, 0, 10 }, 2.0 }, roundRect));
	CHECK(not Geometry2D::Overlaps(RoundRect{ RectF{ 0, 0, 0, 0 }, 2.0 }, roundRect));
}
