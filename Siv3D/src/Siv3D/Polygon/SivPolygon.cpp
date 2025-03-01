//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Polygon.hpp>
# include <Siv3D/Shape2D.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Mouse.hpp>
# include "PolygonDetail.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Polygon AddHole(const Array<Vec2>& outer, const Array<Array<Vec2>>& currentInners, Array<Vec2> hole)
		{
			Array<Array<Vec2>> inners(Arg::reserve = (currentInners.size() + 1));
			{
				inners.append(currentInners);
				inners.push_back(std::move(hole));
			}

			return Polygon{ outer, std::move(inners) };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Polygon::Polygon()
		: pImpl{ std::make_unique<PolygonDetail>() } {}

	Polygon::Polygon(const Polygon& polygon)
		: pImpl{ std::make_unique<PolygonDetail>(*polygon.pImpl) } {}

	Polygon::Polygon(Polygon&& polygon) noexcept
		: pImpl{ std::exchange(polygon.pImpl, std::make_unique<PolygonDetail>()) } {}

	Polygon::Polygon(std::initializer_list<Vec2> outer, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, Array<Array<Vec2>>{}, skipValidation) } {}

	Polygon::Polygon(const std::span<const Vec2> outer, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, Array<Array<Vec2>>{}, skipValidation) } {}

	Polygon::Polygon(std::initializer_list<Vec2> outer, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, Array<Array<Vec2>>{}, boundingRect, skipValidation) } {}

	Polygon::Polygon(const std::span<const Vec2> outer, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, Array<Array<Vec2>>{}, boundingRect, skipValidation) } {}

	Polygon::Polygon(std::initializer_list<Vec2> outer, Array<Array<Vec2>> holes, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, std::move(holes), skipValidation) } {}

	Polygon::Polygon(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, std::move(holes), skipValidation) } {}

	Polygon::Polygon(std::initializer_list<Vec2> outer, Array<Array<Vec2>> holes, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, std::move(holes), boundingRect, skipValidation) } {}

	Polygon::Polygon(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, std::move(holes), boundingRect, skipValidation) } {}

	Polygon::Polygon(std::initializer_list<Vec2> outer, Array<TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, std::move(indices), boundingRect, skipValidation) } {}

	Polygon::Polygon(const std::span<const Vec2> outer, Array<TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, std::move(indices), boundingRect, skipValidation) } {}

	Polygon::Polygon(std::initializer_list<Vec2> outer, Array<Array<Vec2>> holes, Array<Float2> vertices, Array<TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, std::move(holes), std::move(vertices), std::move(indices), boundingRect, skipValidation) } {}

	Polygon::Polygon(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, Array<Float2> vertices, Array<TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, std::move(holes), std::move(vertices), std::move(indices), boundingRect, skipValidation) } {}

	Polygon::Polygon(const Shape2D& shape)
		: pImpl{ std::make_unique<PolygonDetail>(shape.vertices(), shape.indices()) } {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	Polygon::~Polygon() = default;

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	Polygon& Polygon::operator =(const Polygon& other)
	{
		if (&other != this)
		{
			*pImpl = *other.pImpl;
		}
	
		return *this;
	}

	Polygon& Polygon::operator =(Polygon&& other) noexcept
	{
		if (&other != this)
		{
			pImpl = std::exchange(other.pImpl, std::make_unique<PolygonDetail>());
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::isEmpty() const noexcept
	{
		return pImpl->isEmpty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	Polygon::operator bool() const noexcept
	{
		return (not pImpl->isEmpty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasHoles
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::hasHoles() const noexcept
	{
		return (not pImpl->inners().empty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_holes
	//
	////////////////////////////////////////////////////////////////

	size_t Polygon::num_holes() const noexcept
	{
		return pImpl->inners().size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	outer
	//
	////////////////////////////////////////////////////////////////

	const Array<Vec2>& Polygon::outer() const noexcept
	{
		return pImpl->outer();
	}

	////////////////////////////////////////////////////////////////
	//
	//	inners
	//
	////////////////////////////////////////////////////////////////

	const Array<Array<Vec2>>& Polygon::inners() const noexcept
	{
		return pImpl->inners();
	}

	////////////////////////////////////////////////////////////////
	//
	//	vertices
	//
	////////////////////////////////////////////////////////////////

	const Array<Float2>& Polygon::vertices() const noexcept
	{
		return pImpl->vertices();
	}

	////////////////////////////////////////////////////////////////
	//
	//	indices
	//
	////////////////////////////////////////////////////////////////

	const Array<TriangleIndex>& Polygon::indices() const noexcept
	{
		return pImpl->indices();
	}

	////////////////////////////////////////////////////////////////
	//
	//	boundingRect
	//
	////////////////////////////////////////////////////////////////

	const RectF& Polygon::boundingRect() const noexcept
	{
		return pImpl->boundingRect();
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_triangles
	//
	////////////////////////////////////////////////////////////////

	size_t Polygon::num_triangles() const noexcept
	{
		return pImpl->indices().size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	triangle
	//
	////////////////////////////////////////////////////////////////

	Triangle Polygon::triangleAtIndex(const size_t index) const
	{
		const auto& indices = pImpl->indices();

		if (indices.size() <= index)
		{
			ThrowTriangleAtIndexOutOfRange();
		}

		const Float2* pVertices = pImpl->vertices().data();
		const auto& triangleIndex = indices[index];

		return{ pVertices[triangleIndex.i0], pVertices[triangleIndex.i1], pVertices[triangleIndex.i2] };
	}

	////////////////////////////////////////////////////////////////
	//
	//	addHole
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::addHole(const RectF& rect)
	{
		if (isEmpty())
		{
			return false;
		}

		if (Polygon result = AddHole(pImpl->outer(), pImpl->inners(), { rect.tl(), rect.bl(), rect.br(), rect.tr() }))
		{
			*this = std::move(result);
			return false;
		}
		else
		{
			return false;
		}
	}

	bool Polygon::addHole(const Triangle& triangle)
	{
		if (isEmpty())
		{
			return false;
		}

		if (Polygon result = AddHole(pImpl->outer(), pImpl->inners(), { triangle.p0, triangle.p2, triangle.p1 }))
		{
			*this = std::move(result);
			return true;
		}
		
		else
		{
			return false;
		}
	}

	bool Polygon::addHole(const Quad& quad)
	{
		if (isEmpty())
		{
			return false;
		}
		
		if (Polygon result = AddHole(pImpl->outer(), pImpl->inners(), { quad.p0, quad.p3, quad.p2, quad.p1 }))
		{
			*this = std::move(result);
			return true;
		}
		
		else
		{
			return false;
		}
	}

	bool Polygon::addHole(const Circle& circle, const PointsPerCircle& pointsPerCircle)
	{
		if (isEmpty())
		{
			return false;
		}

		Array<Vec2> hole = circle.outer(pointsPerCircle).reversed();

		if (Polygon result = AddHole(pImpl->outer(), pImpl->inners(), std::move(hole)))
		{
			*this = std::move(result);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Polygon::addHole(const Circle& circle, const QualityFactor& qualityFactor)
	{
		if (isEmpty())
		{
			return false;
		}
		
		Array<Vec2> hole = circle.outer(qualityFactor).reversed();
		
		if (Polygon result = AddHole(pImpl->outer(), pImpl->inners(), std::move(hole)))
		{
			*this = std::move(result);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Polygon::addHole(const Ellipse& ellipse, const PointsPerCircle& pointsPerCircle)
	{
		if (isEmpty())
		{
			return false;
		}

		Array<Vec2> hole = ellipse.outer(pointsPerCircle).reversed();

		if (Polygon result = AddHole(pImpl->outer(), pImpl->inners(), std::move(hole)))
		{
			*this = std::move(result);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Polygon::addHole(const Ellipse& ellipse, const QualityFactor& qualityFactor)
	{
		if (isEmpty())
		{
			return false;
		}

		Array<Vec2> hole = ellipse.outer(qualityFactor).reversed();

		if (Polygon result = AddHole(pImpl->outer(), pImpl->inners(), std::move(hole)))
		{
			*this = std::move(result);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Polygon::addHole(const RoundRect& roundRect, const PointsPerCircle& pointsPerCircle)
	{
		if (isEmpty())
		{
			return false;
		}

		Array<Vec2> hole = roundRect.outer(pointsPerCircle).reversed();

		if (Polygon result = AddHole(pImpl->outer(), pImpl->inners(), std::move(hole)))
		{
			*this = std::move(result);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Polygon::addHole(const RoundRect& roundRect, const QualityFactor& qualityFactor)
	{
		if (isEmpty())
		{
			return false;
		}

		Array<Vec2> hole = roundRect.outer(qualityFactor).reversed();

		if (Polygon result = AddHole(pImpl->outer(), pImpl->inners(), std::move(hole)))
		{
			*this = std::move(result);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Polygon::addHole(Array<Vec2> hole)
	{
		if (isEmpty())
		{
			return false;
		}

		if (hole.size() < 3)
		{
			return false;
		}

		if (Geometry2D::IsClockwise(hole))
		{
			hole.reverse();
		}

		if (Polygon result = AddHole(pImpl->outer(), pImpl->inners(), std::move(hole)))
		{
			*this = std::move(result);
			return true;
		}
		else
		{
			return false;
		}
	}







	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	double Polygon::area() const noexcept
	{
		return pImpl->area();
	}

	////////////////////////////////////////////////////////////////
	//
	//	perimeter
	//
	////////////////////////////////////////////////////////////////

	double Polygon::perimeter() const noexcept
	{
		return pImpl->perimeter();
	}

	////////////////////////////////////////////////////////////////
	//
	//	centroid
	//
	////////////////////////////////////////////////////////////////

	Vec2 Polygon::centroid() const
	{
		return pImpl->centroid();
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeConvexHull
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::computeConvexHull() const
	{
		return pImpl->computeConvexHull();
	}

	////////////////////////////////////////////////////////////////
	//
	//	calculateBuffer
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::calculateBuffer(const double distance) const
	{
		return pImpl->calculateBuffer(distance);
	}

	////////////////////////////////////////////////////////////////
	//
	//	calculateRoundBuffer
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::calculateRoundBuffer(const double distance, const QualityFactor& qualityFactor) const
	{
		return pImpl->calculateRoundBuffer(distance, qualityFactor);
	}

	////////////////////////////////////////////////////////////////
	//
	//	simplified
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::simplified(const double maxDistance) const
	{
		if (maxDistance <= 0.0)
		{
			return *this;
		}

		return pImpl->simplified(maxDistance);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::intersects(const Vec2& other) const
	{
		return pImpl->intersects(other);
	}

	bool Polygon::intersects(const Line& other) const
	{
		return pImpl->intersects(other);
	}

	bool Polygon::intersects(const Rect& other) const
	{
		return pImpl->intersects(other);
	}

	bool Polygon::intersects(const RectF& other) const
	{
		return pImpl->intersects(other);
	}

	bool Polygon::intersects(const Circle& other) const
	{
		return pImpl->intersects(other);
	}

	bool Polygon::intersects(const Ellipse& other) const
	{
		return pImpl->intersects(other);
	}

	bool Polygon::intersects(const Triangle& other) const
	{
		return pImpl->intersects(other);
	}

	bool Polygon::intersects(const Quad& other) const
	{
		return pImpl->intersects(other);
	}

	bool Polygon::intersects(const Polygon& other) const
	{
		return pImpl->intersects(*other.pImpl);
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftClicked, leftPressed, leftReleased
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::leftClicked() const noexcept
	{
		return (MouseL.down() && mouseOver());
	}

	bool Polygon::leftPressed() const noexcept
	{
		return (MouseL.pressed() && mouseOver());
	}

	bool Polygon::leftReleased() const noexcept
	{
		return (MouseL.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightClicked, rightPressed, rightReleased
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::rightClicked() const noexcept
	{
		return (MouseR.down() && mouseOver());
	}

	bool Polygon::rightPressed() const noexcept
	{
		return (MouseR.pressed() && mouseOver());
	}

	bool Polygon::rightReleased() const noexcept
	{
		return (MouseR.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	mouseOver
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::mouseOver() const noexcept
	{
		return Geometry2D::Intersect(Cursor::PosF(), *this);
	}









	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Polygon& Polygon::draw(const ColorF& color) const
	{
		pImpl->draw(none, color);
		return *this;
	}

	void Polygon::draw(const double x, const double y, const ColorF& color) const
	{
		pImpl->draw(Float2{ x, y }, color);
	}

	void Polygon::draw(const Vec2& pos, const ColorF& color) const
	{
		pImpl->draw(Float2{ pos }, color);
	}

	const Polygon& Polygon::draw(const PatternParameters& pattern) const
	{
		pImpl->draw(none, pattern);
		return *this;
	}

	void Polygon::draw(double x, double y, const PatternParameters& pattern) const
	{
		pImpl->draw(Float2{ x, y }, pattern);
	}

	void Polygon::draw(const Vec2& pos, const PatternParameters& pattern) const
	{
		pImpl->draw(Float2{ pos }, pattern);
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawFrame
	//
	////////////////////////////////////////////////////////////////

	const Polygon& Polygon::drawFrame(const double thickness, const ColorF& color) const
	{
		pImpl->drawFrame(none, thickness, color);
		return *this;
	}

	void Polygon::drawFrame(const double x, const double y, const double thickness, const ColorF& color) const
	{
		pImpl->drawFrame(Float2{ x, y }, thickness, color);
	}

	void Polygon::drawFrame(const Vec2& pos, const double thickness, const ColorF& color) const
	{
		pImpl->drawFrame(Float2{ pos }, thickness, color);
	}

	const Polygon& Polygon::drawFrame(const double thickness, const PatternParameters& pattern) const
	{
		pImpl->drawFrame(none, thickness, pattern);
		return *this;
	}

	void Polygon::drawFrame(const double x, const double y, const double thickness, const PatternParameters& pattern) const
	{
		pImpl->drawFrame(Float2{ x, y }, thickness, pattern);
	}

	void Polygon::drawFrame(const Vec2& pos, const double thickness, const PatternParameters& pattern) const
	{
		pImpl->drawFrame(Float2{ pos }, thickness, pattern);
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawWireframe
	//
	////////////////////////////////////////////////////////////////

	const Polygon& Polygon::drawWireframe(const double thickness, const ColorF& color) const
	{
		pImpl->drawWireframe(none, thickness, color);
		return *this;
	}

	void Polygon::drawWireframe(const double x, const double y, const double thickness, const ColorF& color) const
	{
		pImpl->drawWireframe(Float2{ x, y }, thickness, color);
	}

	void Polygon::drawWireframe(const Vec2& pos, const double thickness, const ColorF& color) const
	{
		pImpl->drawWireframe(Float2{ pos }, thickness, color);
	}

	const Polygon& Polygon::drawWireframe(const double thickness, const PatternParameters& pattern) const
	{
		pImpl->drawWireframe(none, thickness, pattern);
		return *this;
	}

	void Polygon::drawWireframe(const double x, const double y, const double thickness, const PatternParameters& pattern) const
	{
		pImpl->drawWireframe(Float2{ x, y }, thickness, pattern);
	}

	void Polygon::drawWireframe(const Vec2& pos, const double thickness, const PatternParameters& pattern) const
	{
		pImpl->drawWireframe(Float2{ pos }, thickness, pattern);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Validate
	//
	////////////////////////////////////////////////////////////////

	PolygonFailureType Polygon::Validate(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes)
	{
		return PolygonDetail::Validate(outer, holes);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Correct
	//
	////////////////////////////////////////////////////////////////

	Array<Polygon> Polygon::Correct(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes)
	{
		return PolygonDetail::Correct(outer, holes);
	}

	////////////////////////////////////////////////////////////////
	//
	//	CorrectOne
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::CorrectOne(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes)
	{
		Array<Polygon> polygons = Correct(outer, holes);

		if (polygons.isEmpty())
		{
			return Polygon{};
		}

		size_t largestIndex = 0;
		double largestArea = polygons[0].area();

		for (size_t i = 1; i < polygons.size(); ++i)
		{
			const double area = polygons[i].area();
			
			if (largestArea < area)
			{
				largestIndex = i;
				largestArea = area;
			}
		}

		return std::move(polygons[largestIndex]);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Polygon& value)
	{
		formatData.string.append(U"((");

		bool b = false;

		for (const auto& point : value.outer())
		{
			if (std::exchange(b, true))
			{
				formatData.string.push_back(U',');
			}

			Formatter(formatData, point);
		}

		formatData.string.push_back(U')');

		if (value.inners())
		{
			formatData.string.append(U",(");

			b = false;

			for (const auto& hole : value.inners())
			{
				if (std::exchange(b, true))
				{
					formatData.string.append(U",(");
				}

				bool b2 = false;

				for (const auto& point : hole)
				{
					if (std::exchange(b2, true))
					{
						formatData.string.push_back(U',');
					}

					Formatter(formatData, point);
				}

				formatData.string.push_back(U')');
			}
		}

		formatData.string.push_back(U')');
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void Polygon::ThrowTriangleAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Polygon::triangleAtIndex() index out of range" };
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

s3d::ParseContext::iterator fmt::formatter<s3d::Polygon, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Polygon, s3d::char32>::format(const s3d::Polygon& value, s3d::BufferContext& ctx)
{
	if (value.isEmpty())
	{
		return fmt::format_to(ctx.out(), U"()");
	}

	const std::u32string elementTag = (tag.empty() ? U"{}" : (U"{:" + tag + U"}"));

	auto it = fmt::format_to(ctx.out(), U"((");

	bool b = false;

	for (const auto& point : value.outer())
	{
		if (std::exchange(b, true))
		{
			it = fmt::format_to(it, U",");
		}

		it = fmt::format_to(it, elementTag, point);
	}

	it = fmt::format_to(it, U")");

	if (value.inners())
	{
		it = fmt::format_to(it, U",(");

		b = false;

		for (const auto& hole : value.inners())
		{
			if (std::exchange(b, true))
			{
				it = fmt::format_to(it, U",(");
			}

			bool b2 = false;

			for (const auto& point : hole)
			{
				if (std::exchange(b2, true))
				{
					it = fmt::format_to(it, U",");
				}

				it = fmt::format_to(it, elementTag, point);
			}

			it = fmt::format_to(it, U")");
		}
	}

	return fmt::format_to(it, U")");
}
