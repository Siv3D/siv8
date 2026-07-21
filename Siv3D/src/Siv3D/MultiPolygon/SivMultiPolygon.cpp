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

# include <Siv3D/MultiPolygon.hpp>
# include <Siv3D/Polygon/PolygonDetail.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::movedBy(const double x, const double y) const&
	{
		MultiPolygon result{ *this };
		result.moveBy(x, y);
		return result;
	}

	MultiPolygon MultiPolygon::movedBy(const double x, const double y) && noexcept
	{
		moveBy(x, y);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::movedBy(const Vec2 v) const&
	{
		return movedBy(v.x, v.y);
	}

	MultiPolygon MultiPolygon::movedBy(const Vec2 v) && noexcept
	{
		moveBy(v);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon& MultiPolygon::moveBy(const double x, const double y) noexcept
	{
		for (auto& polygon : m_polygons)
		{
			polygon.moveBy(x, y);
		}

		return *this;
	}

	MultiPolygon& MultiPolygon::moveBy(const Vec2 v) noexcept
	{
		return moveBy(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffset
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::withOffset(const double x, const double y) const&
	{
		MultiPolygon result{ *this };
		result.moveBy(x, y);
		return result;
	}

	MultiPolygon MultiPolygon::withOffset(const double x, const double y) && noexcept
	{
		moveBy(x, y);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::withOffset(const Vec2 v) const&
	{
		MultiPolygon result{ *this };
		result.moveBy(v);
		return result;
	}

	MultiPolygon MultiPolygon::withOffset(const Vec2 v) && noexcept
	{
		moveBy(v);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffsetX, withOffsetY
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::withOffsetX(const double x) const&
	{
		MultiPolygon result{ *this };
		result.moveBy(x, 0.0);
		return result;
	}

	MultiPolygon MultiPolygon::withOffsetX(const double x) && noexcept
	{
		moveBy(x, 0.0);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::withOffsetY(const double y) const&
	{
		MultiPolygon result{ *this };
		result.moveBy(0.0, y);
		return result;
	}

	MultiPolygon MultiPolygon::withOffsetY(const double y) && noexcept
	{
		moveBy(0.0, y);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::rotated(const double angle) const&
	{
		MultiPolygon result{ *this };
		result.rotate(angle);
		return result;
	}

	MultiPolygon MultiPolygon::rotated(const double angle) &&
	{
		rotate(angle);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedAt
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::rotatedAt(const Vec2 pos, const double angle) const&
	{
		MultiPolygon result{ *this };
		result.rotateAt(pos, angle);
		return result;
	}

	MultiPolygon MultiPolygon::rotatedAt(const Vec2 pos, const double angle) &&
	{
		rotateAt(pos, angle);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon& MultiPolygon::rotate(const double angle)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.rotate(angle);
		}
		
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotateAt
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon& MultiPolygon::rotateAt(const Vec2 pos, const double angle)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.rotateAt(pos, angle);
		}
		
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	transformed
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::transformed(const double s, const double c, const Vec2& pos) const&
	{
		MultiPolygon result{ *this };
		result.transform(s, c, pos);
		return result;
	}

	MultiPolygon MultiPolygon::transformed(const double s, const double c, const Vec2& pos) &&
	{
		transform(s, c, pos);
		return std::move(*this);
	}
	
	////////////////////////////////////////////////////////////////
	//
	//	transform
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon& MultiPolygon::transform(const double s, const double c, const Vec2& pos)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.transform(s, c, pos);
		}
		
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromOrigin
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::scaledFromOrigin(const double s) const&
	{
		MultiPolygon result{ *this };
		result.scaleFromOrigin(s);
		return result;
	}

	MultiPolygon MultiPolygon::scaledFromOrigin(const double s) &&
	{
		scaleFromOrigin(s);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::scaledFromOrigin(const double sx, const double sy) const&
	{
		MultiPolygon result{ *this };
		result.scaleFromOrigin(sx, sy);
		return result;
	}

	MultiPolygon MultiPolygon::scaledFromOrigin(const double sx, const double sy) &&
	{
		scaleFromOrigin(sx, sy);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::scaledFromOrigin(const Vec2 s) const&
	{
		MultiPolygon result{ *this };
		result.scaleFromOrigin(s);
		return result;
	}

	MultiPolygon MultiPolygon::scaledFromOrigin(const Vec2 s) &&
	{
		scaleFromOrigin(s);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFromOrigin
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon& MultiPolygon::scaleFromOrigin(const double s)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.scaleFromOrigin(s);
		}
		
		return *this;
	}

	MultiPolygon& MultiPolygon::scaleFromOrigin(const double sx, const double sy)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.scaleFromOrigin(sx, sy);
		}
		
		return *this;
	}

	MultiPolygon& MultiPolygon::scaleFromOrigin(const Vec2 s)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.scaleFromOrigin(s);
		}
		
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFrom
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon MultiPolygon::scaledFrom(const Vec2 pos, const double s) const&
	{
		MultiPolygon result{ *this };
		result.scaleFrom(pos, s);
		return result;
	}

	MultiPolygon MultiPolygon::scaledFrom(const Vec2 pos, const double s) &&
	{
		scaleFrom(pos, s);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::scaledFrom(const Vec2 pos, const double sx, const double sy) const&
	{
		MultiPolygon result{ *this };
		result.scaleFrom(pos, sx, sy);
		return result;
	}

	MultiPolygon MultiPolygon::scaledFrom(const Vec2 pos, const double sx, const double sy) &&
	{
		scaleFrom(pos, sx, sy);
		return std::move(*this);
	}

	MultiPolygon MultiPolygon::scaledFrom(const Vec2 pos, const Vec2 s) const&
	{
		MultiPolygon result{ *this };
		result.scaleFrom(pos, s);
		return result;
	}

	MultiPolygon MultiPolygon::scaledFrom(const Vec2 pos, const Vec2 s) &&
	{
		scaleFrom(pos, s);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFrom
	//
	////////////////////////////////////////////////////////////////

	MultiPolygon& MultiPolygon::scaleFrom(const Vec2 pos, const double s)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.scaleFrom(pos, s);
		}
		
		return *this;
	}

	MultiPolygon& MultiPolygon::scaleFrom(const Vec2 pos, const double sx, const double sy)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.scaleFrom(pos, sx, sy);
		}
		
		return *this;
	}

	MultiPolygon& MultiPolygon::scaleFrom(const Vec2 pos, const Vec2 s)
	{
		for (auto& polygon : m_polygons)
		{
			polygon.scaleFrom(pos, s);
		}
		
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	double MultiPolygon::area() const noexcept
	{
		KahanSummation<double> totalArea;

		for (const auto& polygon : m_polygons)
		{
			totalArea += polygon.area();
		}

		return totalArea.value();
	}

	////////////////////////////////////////////////////////////////
	//
	//	perimeter
	//
	////////////////////////////////////////////////////////////////

	double MultiPolygon::perimeter() const noexcept
	{
		KahanSummation<double> totalPerimeter;

		for (const auto& polygon : m_polygons)
		{
			totalPerimeter += polygon.perimeter();
		}

		return totalPerimeter.value();
	}

	////////////////////////////////////////////////////////////////
	//
	//	centroid
	//
	////////////////////////////////////////////////////////////////

	Optional<Vec2> MultiPolygon::centroid() const noexcept
	{
		Optional<Vec2> reference;
		KahanSummation<double> totalArea;
		KahanSummation<double> weightedX;
		KahanSummation<double> weightedY;

		for (const auto& polygon : m_polygons)
		{
			const auto result = polygon._detail()->centroid();

			if (not result)
			{
				continue;
			}

			if (not reference)
			{
				reference = result->centroid;
			}

			totalArea += result->area;
			weightedX += ((result->centroid.x - reference->x) * result->area);
			weightedY += ((result->centroid.y - reference->y) * result->area);
		}

		const double area = totalArea.value();

		if ((not reference) || (area == 0.0))
		{
			return none;
		}

		return Vec2{ (reference->x + (weightedX.value() / area)), (reference->y + (weightedY.value() / area)) };
	}
}
