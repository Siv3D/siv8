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

# include <Siv3D/Polygon.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/Math.hpp>
# include <Siv3D/Shape2D.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/ImageDraw.hpp>
# include "PolygonDetail.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Polygon AddHole(const Array<Vec2>& outer, const PolygonHolesView currentInners, Array<Vec2> hole)
		{
			Array<Array<Vec2>> inners(Arg::reserve = (currentInners.size() + 1));
			{
				for (const auto& currentInner : currentInners)
				{
					inners.emplace_back(currentInner.begin(), currentInner.end());
				}
				
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
		: pImpl{ std::make_unique<PolygonDetail>(shape.vertices(), shape.indices(), shape.boundingRect()) } {}

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

	PolygonHolesView Polygon::inners() const noexcept
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
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void Polygon::clear()
	{
		pImpl = std::make_unique<PolygonDetail>();
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
	//	addHoles
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::addHoles(Array<Array<Vec2>> holes)
	{
		if (isEmpty())
		{
			return false;
		}

		holes.erase_all_if([](const Array<Vec2>& hole) { return (hole.size() < 3); });

		if (not holes)
		{
			return false;
		}

		for (auto& hole : holes)
		{
			if (Geometry2D::IsClockwise(hole))
			{
				hole.reverse();
			}
		}

		Array<Array<Vec2>> inners(Arg::reserve = (pImpl->inners().size() + holes.size()));
		{
			const auto& polygonInners = pImpl->inners();

			for (const auto& polygonInner : polygonInners)
			{
				inners.emplace_back(polygonInner.begin(), polygonInner.end());
			}

			inners.append(holes);
		}

		Polygon result{ pImpl->outer(), std::move(inners) };

		if (not result)
		{
			return false;
		}

		*this = std::move(result);

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::movedBy(const double x, const double y) const&
	{
		return movedBy(Vec2{ x, y });
	}

	Polygon Polygon::movedBy(const double x, const double y) && noexcept
	{
		moveBy(x, y);
		return std::move(*this);
	}

	Polygon Polygon::movedBy(const Vec2 v) const&
	{
		Polygon result{ *this };
		result.moveBy(v);
		return result;
	}

	Polygon Polygon::movedBy(const Vec2 v) && noexcept
	{
		moveBy(v);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	Polygon& Polygon::moveBy(const double x, const double y) noexcept
	{
		return moveBy(Vec2{ x, y });
	}

	Polygon& Polygon::moveBy(const Vec2 v) noexcept
	{
		pImpl->moveBy(v);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffset
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::withOffset(const double x, const double y) const&
	{
		return withOffset(Vec2{ x, y });
	}

	Polygon Polygon::withOffset(const double x, const double y) && noexcept
	{
		moveBy(x, y);
		return std::move(*this);
	}

	Polygon Polygon::withOffset(const Vec2 v) const&
	{
		Polygon result{ *this };
		result.moveBy(v);
		return result;
	}

	Polygon Polygon::withOffset(const Vec2 v) && noexcept
	{
		moveBy(v);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffsetX, withOffsetY
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::withOffsetX(const double x) const&
	{
		Polygon result{ *this };
		result.moveBy(Vec2{ x, 0.0 });
		return result;
	}

	Polygon Polygon::withOffsetX(const double x) && noexcept
	{
		moveBy(Vec2{ x, 0.0 });
		return std::move(*this);
	}

	Polygon Polygon::withOffsetY(const double y) const&
	{
		Polygon result{ *this };
		result.moveBy(Vec2{ 0.0, y });
		return result;
	}

	Polygon Polygon::withOffsetY(const double y) && noexcept
	{
		moveBy(Vec2{ 0.0, y });
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::rotated(const double angle) const&
	{
		return rotatedAt(Vec2{ 0, 0 }, angle);
	}

	Polygon Polygon::rotated(const double angle)&&
	{
		return std::move(*this).rotatedAt(Vec2{ 0, 0 }, angle);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedAt
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::rotatedAt(const Vec2 pos, const double angle) const&
	{
		Polygon result{ *this };
		result.rotateAt(pos, angle);
		return result;
	}

	Polygon Polygon::rotatedAt(const Vec2 pos, const double angle)&&
	{
		rotateAt(pos, angle);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate
	//
	////////////////////////////////////////////////////////////////

	Polygon& Polygon::rotate(const double angle)
	{
		return rotateAt(Vec2{ 0, 0 }, angle);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotateAt
	//
	////////////////////////////////////////////////////////////////

	Polygon& Polygon::rotateAt(const Vec2 pos, const double angle)
	{
		pImpl->rotateAt(pos, angle);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	transformed
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::transformed(const double s, const double c, const Vec2& pos) const&
	{
		Polygon result{ *this };
		result.transform(s, c, pos);
		return result;
	}

	Polygon Polygon::transformed(const double s, const double c, const Vec2& pos)&&
	{
		transform(s, c, pos);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	transform
	//
	////////////////////////////////////////////////////////////////

	Polygon& Polygon::transform(const double s, const double c, const Vec2& pos)
	{
		pImpl->transform(s, c, pos);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromOrigin
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::scaledFromOrigin(const double s) const&
	{
		Polygon result{ *this };
		result.scaleFromOrigin(s);
		return result;
	}

	Polygon Polygon::scaledFromOrigin(const double s)&&
	{
		scaleFromOrigin(s);
		return std::move(*this);
	}

	Polygon Polygon::scaledFromOrigin(const double sx, const double sy) const&
	{
		Polygon result{ *this };
		result.scaleFromOrigin(sx, sy);
		return result;
	}

	Polygon Polygon::scaledFromOrigin(const double sx, const double sy)&&
	{
		scaleFromOrigin(sx, sy);
		return std::move(*this);
	}

	Polygon Polygon::scaledFromOrigin(const Vec2 s) const&
	{
		Polygon result{ *this };
		result.scaleFromOrigin(s);
		return result;
	}

	Polygon Polygon::scaledFromOrigin(const Vec2 s)&&
	{
		scaleFromOrigin(s);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFromOrigin
	//
	////////////////////////////////////////////////////////////////

	Polygon& Polygon::scaleFromOrigin(const double s)
	{
		pImpl->scaleFromOrigin(s);
		return *this;
	}

	Polygon& Polygon::scaleFromOrigin(const double sx, const double sy)
	{
		pImpl->scaleFromOrigin(Vec2{ sx, sy });
		return *this;
	}

	Polygon& Polygon::scaleFromOrigin(const Vec2 s)
	{
		pImpl->scaleFromOrigin(s);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFrom
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::scaledFrom(const Vec2 pos, const double s) const&
	{
		Polygon result{ *this };
		result.scaleFrom(pos, s);
		return result;
	}

	Polygon Polygon::scaledFrom(const Vec2 pos, const double s)&&
	{
		scaleFrom(pos, s);
		return std::move(*this);
	}

	Polygon Polygon::scaledFrom(const Vec2 pos, const double sx, const double sy) const&
	{
		Polygon result{ *this };
		result.scaleFrom(pos, Vec2{ sx, sy });
		return result;
	}

	Polygon Polygon::scaledFrom(const Vec2 pos, const double sx, const double sy)&&
	{
		scaleFrom(pos, Vec2{ sx, sy });
		return std::move(*this);
	}

	Polygon Polygon::scaledFrom(const Vec2 pos, const Vec2 s) const&
	{
		Polygon result{ *this };
		result.scaleFrom(pos, s);
		return result;
	}

	Polygon Polygon::scaledFrom(const Vec2 pos, const Vec2 s)&&
	{
		scaleFrom(pos, s);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFrom
	//
	////////////////////////////////////////////////////////////////

	Polygon& Polygon::scaleFrom(const Vec2 pos, const double s)
	{
		pImpl->scaleFrom(pos, s);
		return *this;
	}

	Polygon& Polygon::scaleFrom(const Vec2 pos, const double sx, const double sy)
	{
		pImpl->scaleFrom(pos, Vec2{ sx, sy });
		return *this;
	}

	Polygon& Polygon::scaleFrom(const Vec2 pos, const Vec2 s)
	{
		pImpl->scaleFrom(pos, s);
		return *this;
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
	//	computeMiterBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::computeMiterBufferPolygon(const double distance) const
	{
		return pImpl->computeMiterBufferPolygon(distance);
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeRoundBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::computeRoundBufferPolygon(const double distance, const QualityFactor& qualityFactor) const
	{
		return pImpl->computeRoundBufferPolygon(distance, qualityFactor);
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
	//	outline
	//
	////////////////////////////////////////////////////////////////

	LineString Polygon::outline(const CloseRing closeRing) const
	{
		const auto& out = outer();

		if (out.isEmpty())
		{
			return{};
		}

		if (closeRing)
		{
			LineString points;
			points.reserve(out.size() + 1);
			points.append_range(out);
			points.push_back(out.front());
			return points;
		}
		else
		{
			return LineString{ out };
		}
	}

	LineString Polygon::outline(double distanceFromOrigin, double length) const
	{
		if (length <= 0.0)
		{
			distanceFromOrigin += length;
			length = -length;
		}

		const auto& out = outer();
		const size_t N = out.size();
		Array<double> lens(N);
		{
			for (size_t i = 0; i < (N - 1); ++i)
			{
				lens[i] = out[i].distanceFrom(out[i + 1]);
			}

			lens[N - 1] = out[N - 1].distanceFrom(out[0]);
		}
		const double perim = lens.sum();

		distanceFromOrigin = Math::Fmod(distanceFromOrigin, perim) + (distanceFromOrigin < 0 ? perim : 0);
		length = Min(length, perim);
		const double distanceToTarget = (distanceFromOrigin + length);

		LineString points;
		double currentLength = 0.0;

		for (size_t n = 0; n < (N * 2); ++n)
		{
			const size_t i = (n % N);
			const double len = lens[i];
			const Vec2 pFrom = out[i];
			const Vec2 pTo = out[(N <= (i + 1)) ? (i - (N - 1)) : (i + 1)];

			if (not points)
			{
				if ((distanceFromOrigin <= (currentLength + len)))
				{
					const Vec2 origin = (pFrom + (pTo - pFrom).setLength(distanceFromOrigin - currentLength));
					points << origin;

					if (distanceToTarget <= (currentLength + len))
					{
						const Vec2 target = (pFrom + (pTo - pFrom).setLength(distanceToTarget - currentLength));
						points << target;
						break;
					}

					points << pTo;
				}
			}
			else
			{
				if (distanceToTarget <= (currentLength + len))
				{
					const Vec2 target = (pFrom + (pTo - pFrom).setLength(distanceToTarget - currentLength));
					points << target;
					break;
				}

				points << pTo;
			}

			currentLength += len;
		}

		return points;
	}

	////////////////////////////////////////////////////////////////
	//
	//	append
	//
	////////////////////////////////////////////////////////////////
	
	bool Polygon::append(const RectF& other)
	{
		return pImpl->append(other);
	}

	bool Polygon::append(const Polygon& other)
	{
		return pImpl->append(other);
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
		return Geometry2D::Intersects(Cursor::PosF(), *this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	paint
	//
	////////////////////////////////////////////////////////////////

	const Polygon& Polygon::paint(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::Fill(dst, *this, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);
		return *this;
	}

	const Polygon& Polygon::paint(Image& dst, const Vec2& offset, const Color& color, const EnableAntialiasing enableAntialiasings) const
	{
		ImageDraw::Fill(dst, *this, offset, color, ImagePixel::BlendMode::SourceOver, enableAntialiasings);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwrite
	//
	////////////////////////////////////////////////////////////////

	const Polygon& Polygon::overwrite(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::Fill(dst, *this, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		return *this;
	}

	const Polygon& Polygon::overwrite(Image& dst, const Vec2& offset, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::Fill(dst, *this, offset, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	paintFrame
	//
	////////////////////////////////////////////////////////////////

	const Polygon& Polygon::paintFrame(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return paintFrame(dst, 1.0, color, enableAntialiasing);
	}

	const Polygon& Polygon::paintFrame(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::ClosedLineString(dst, outline(), thickness, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);

		for (const auto& hole : inners())
		{
			ImageDraw::ClosedLineString(dst, hole, thickness, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);
		}

		return *this;
	}

	void Polygon::paintFrame(Image& dst, const Vec2& offset, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		paintFrame(dst, offset, 1.0, color, enableAntialiasing);
	}

	void Polygon::paintFrame(Image& dst, const Vec2& offset, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::ClosedLineString(dst, outline(), offset, thickness, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);
	
		for (const auto& hole : inners())
		{
			ImageDraw::ClosedLineString(dst, hole, offset, thickness, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwriteFrame
	//
	////////////////////////////////////////////////////////////////

	const Polygon& Polygon::overwriteFrame(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return overwriteFrame(dst, 1.0, color, enableAntialiasing);
	}

	const Polygon& Polygon::overwriteFrame(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::ClosedLineString(dst, outline(), thickness, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		
		for (const auto& hole : inners())
		{
			ImageDraw::ClosedLineString(dst, hole, thickness, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		}
	
		return *this;
	}

	void Polygon::overwriteFrame(Image& dst, const Vec2& offset, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		overwriteFrame(dst, offset, 1.0, color, enableAntialiasing);
	}

	void Polygon::overwriteFrame(Image& dst, const Vec2& offset, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::ClosedLineString(dst, outline(), offset, thickness, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		
		for (const auto& hole : inners())
		{
			ImageDraw::ClosedLineString(dst, hole, offset, thickness, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		}
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

	void Polygon::draw(const Vec2& offset, const ColorF& color) const
	{
		pImpl->draw(Float2{ offset }, color);
	}

	const Polygon& Polygon::draw(const PatternParameters& pattern) const
	{
		pImpl->draw(none, pattern);
		return *this;
	}

	void Polygon::draw(const Vec2& offset, const PatternParameters& pattern) const
	{
		pImpl->draw(Float2{ offset }, pattern);
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawTransformed
	//
	////////////////////////////////////////////////////////////////

	void Polygon::drawTransformed(const double angle, const Vec2& pos, const ColorF& color) const
	{
		const auto [s, c] = FastMath::SinCos(angle);
		drawTransformed(s, c, pos, color);
	}

	void Polygon::drawTransformed(const double s, const double c, const Vec2& pos, const ColorF& color) const
	{
		pImpl->drawTransformed(s, c, pos, color);
	}

	void Polygon::drawTransformed(const double angle, const Vec2& pos, const PatternParameters& pattern) const
	{
		const auto [s, c] = FastMath::SinCos(angle);
		drawTransformed(s, c, pos, pattern);
	}

	void Polygon::drawTransformed(const double s, const double c, const Vec2& pos, const PatternParameters& pattern) const
	{
		pImpl->drawTransformed(s, c, pos, pattern);
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

	void Polygon::drawFrame(const Vec2& offset, const double thickness, const ColorF& color) const
	{
		pImpl->drawFrame(Float2{ offset }, thickness, color);
	}

	const Polygon& Polygon::drawFrame(const double thickness, const PatternParameters& pattern) const
	{
		pImpl->drawFrame(none, thickness, pattern);
		return *this;
	}

	void Polygon::drawFrame(const Vec2& offset, const double thickness, const PatternParameters& pattern) const
	{
		pImpl->drawFrame(Float2{ offset }, thickness, pattern);
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

	void Polygon::drawWireframe(const Vec2& offset, const double thickness, const ColorF& color) const
	{
		pImpl->drawWireframe(Float2{ offset }, thickness, color);
	}

	const Polygon& Polygon::drawWireframe(const double thickness, const PatternParameters& pattern) const
	{
		pImpl->drawWireframe(none, thickness, pattern);
		return *this;
	}

	void Polygon::drawWireframe(const Vec2& offset, const double thickness, const PatternParameters& pattern) const
	{
		pImpl->drawWireframe(Float2{ offset }, thickness, pattern);
	}

	////////////////////////////////////////////////////////////////
	//
	//	_detail
	//
	////////////////////////////////////////////////////////////////

	const Polygon::PolygonDetail* Polygon::_detail() const noexcept
	{
		return pImpl.get();
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

s3d::BufferContext::iterator fmt::formatter<s3d::Polygon, s3d::char32>::format(const s3d::Polygon& value, s3d::BufferContext& ctx) const
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
