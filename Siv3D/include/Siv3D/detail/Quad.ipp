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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad::Quad(const value_type x0, const value_type y0, const value_type x1, const value_type y1, const value_type x2, const value_type y2, const value_type x3, const value_type y3) noexcept
		: p0{ x0, y0 }
		, p1{ x1, y1 }
		, p2{ x2, y2 }
		, p3{ x3, y3 } {}

	constexpr Quad::Quad(const Concept::Arithmetic auto x0, const Concept::Arithmetic auto y0, const Concept::Arithmetic auto x1, const Concept::Arithmetic auto y1, const Concept::Arithmetic auto x2, const Concept::Arithmetic auto y2, const Concept::Arithmetic auto x3, const Concept::Arithmetic auto y3) noexcept
		: p0{ static_cast<value_type>(x0), static_cast<value_type>(y0) }
		, p1{ static_cast<value_type>(x1), static_cast<value_type>(y1) }
		, p2{ static_cast<value_type>(x2), static_cast<value_type>(y2) }
		, p3{ static_cast<value_type>(x3), static_cast<value_type>(y3) } {}

	constexpr Quad::Quad(const position_type& _p0, const position_type& _p1, const position_type& _p2, const position_type& _p3) noexcept
		: p0{ _p0 }
		, p1{ _p1 }
		, p2{ _p2 }
		, p3{ _p3 } {}

	////////////////////////////////////////////////////////////////
	//
	//	withP0
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad Quad::withP0(const value_type _x, const value_type _y) const noexcept
	{
		return{ position_type{ _x, _y }, p1, p2, p3 };
	}

	constexpr Quad Quad::withP0(const position_type _p0) const noexcept
	{
		return{ _p0, p1, p2, p3 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withP1
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad Quad::withP1(const value_type _x, const value_type _y) const noexcept
	{
		return{ p0, position_type{ _x, _y }, p2, p3 };
	}

	constexpr Quad Quad::withP1(const position_type _p1) const noexcept
	{
		return{ p0, _p1, p2, p3 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withP2
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad Quad::withP2(const value_type _x, const value_type _y) const noexcept
	{
		return{ p0, p1, position_type{ _x, _y }, p3 };
	}

	constexpr Quad Quad::withP2(const position_type _p2) const noexcept
	{
		return{ p0, p1, _p2, p3 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withP3
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad Quad::withP3(const value_type _x, const value_type _y) const noexcept
	{
		return{ p0, p1, p2, position_type{ _x, _y } };
	}

	constexpr Quad Quad::withP3(const position_type _p3) const noexcept
	{
		return{ p0, p1, p2, _p3 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad& Quad::set(const value_type x0, const value_type y0, const value_type x1, const value_type y1, const value_type x2, const value_type y2, const value_type x3, const value_type y3) noexcept
	{
		p0.set(x0, y0);
		p1.set(x1, y1);
		p2.set(x2, y2);
		p3.set(x3, y3);
		return *this;
	}

	constexpr Quad& Quad::set(const position_type& _p0, const position_type& _p1, const position_type& _p2, const position_type& _p3) noexcept
	{
		p0 = _p0;
		p1 = _p1;
		p2 = _p2;
		p3 = _p3;
		return *this;
	}

	constexpr Quad& Quad::set(const Quad& quad) noexcept
	{
		p0 = quad.p0;
		p1 = quad.p1;
		p2 = quad.p2;
		p3 = quad.p3;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad Quad::movedBy(const value_type x, const value_type y) const noexcept
	{
		return{ p0.movedBy(x, y), p1.movedBy(x, y), p2.movedBy(x, y), p3.movedBy(x, y) };
	}

	constexpr Quad Quad::movedBy(const position_type v) const noexcept
	{
		return movedBy(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad& Quad::moveBy(const value_type x, const value_type y) noexcept
	{
		p0.moveBy(x, y);
		p1.moveBy(x, y);
		p2.moveBy(x, y);
		p3.moveBy(x, y);
		return *this;
	}

	constexpr Quad& Quad::moveBy(const position_type v) noexcept
	{
		return moveBy(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromOrigin
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad Quad::scaledFromOrigin(const value_type s) const noexcept
	{
		return{ (p0 * s), (p1 * s), (p2 * s), (p3 * s) };
	}

	constexpr Quad Quad::scaledFromOrigin(const value_type sx, const value_type sy) const noexcept
	{
		return scaledFromOrigin(Vec2{ sx, sy });
	}

	constexpr Quad Quad::scaledFromOrigin(const Vec2 s) const noexcept
	{
		return{ (p0 * s), (p1 * s), (p2 * s), (p3 * s) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFromOrigin
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad& Quad::scaleFromOrigin(const value_type s) noexcept
	{
		p0 *= s;
		p1 *= s;
		p2 *= s;
		p3 *= s;
		return *this;
	}

	constexpr Quad& Quad::scaleFromOrigin(const value_type sx, const value_type sy) noexcept
	{
		return scaleFromOrigin(Vec2{ sx, sy });
	}

	constexpr Quad& Quad::scaleFromOrigin(const Vec2 s) noexcept
	{
		p0 *= s;
		p1 *= s;
		p2 *= s;
		p3 *= s;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFrom
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad Quad::scaledFrom(const position_type pos, const value_type s) const noexcept
	{
		return{ (pos + (p0 - pos) * s), (pos + (p1 - pos) * s), (pos + (p2 - pos) * s), (pos + (p3 - pos) * s) };
	}

	constexpr Quad Quad::scaledFrom(const position_type pos, const value_type sx, const value_type sy) const noexcept
	{
		return scaledFrom(pos, position_type{ sx, sy });
	}

	constexpr Quad Quad::scaledFrom(const position_type pos, const Vec2 s) const noexcept
	{
		return{ (pos + (p0 - pos) * s), (pos + (p1 - pos) * s), (pos + (p2 - pos) * s), (pos + (p3 - pos) * s) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFrom
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad& Quad::scaleFrom(const position_type pos, const value_type s) noexcept
	{
		p0 = (pos + (p0 - pos) * s);
		p1 = (pos + (p1 - pos) * s);
		p2 = (pos + (p2 - pos) * s);
		p3 = (pos + (p3 - pos) * s);
		return *this;
	}

	constexpr Quad& Quad::scaleFrom(const position_type pos, const value_type sx, const value_type sy) noexcept
	{
		return scaleFrom(pos, position_type{ sx, sy });
	}

	constexpr Quad& Quad::scaleFrom(const position_type pos, const Vec2 s) noexcept
	{
		p0 = (pos + (p0 - pos) * s);
		p1 = (pos + (p1 - pos) * s);
		p2 = (pos + (p2 - pos) * s);
		p3 = (pos + (p3 - pos) * s);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isClockwise
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Quad::isClockwise() const noexcept
	{
		return Geometry2D::IsClockwise(p0, p1, p2);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isConvex
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Quad::isConvex() const noexcept
	{
		const bool t0 = Geometry2D::IsClockwise(p0, p1, p2);
		const bool t1 = Geometry2D::IsClockwise(p1, p2, p3);
		const bool t2 = Geometry2D::IsClockwise(p2, p3, p0);
		const bool t3 = Geometry2D::IsClockwise(p3, p0, p1);
		return (t0 && t1 && t2 && t3);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasArea
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Quad::hasArea() const noexcept
	{
		return (0.0 < area());
	}

	////////////////////////////////////////////////////////////////
	//
	//	centroidX, centroidY
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad::value_type Quad::centroidX() const noexcept
	{
		return centroid().x;
	}

	constexpr Quad::value_type Quad::centroidY() const noexcept
	{
		return centroid().y;
	}

	////////////////////////////////////////////////////////////////
	//
	//	centroid
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad::position_type Quad::centroid() const noexcept
	{
		const Triangle t0{ p0, p1, p2 };
		const Triangle t1{ p2, p3, p0 };
		const double s0 = t0.area();
		const double s1 = t1.area();
		const position_type c0 = t0.centroid();
		const position_type c1 = t1.centroid();
		return c0.lerp(c1, (s1 / (s0 + s1)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAtIndex
	//
	////////////////////////////////////////////////////////////////

	inline Quad::position_type& Quad::pointAtIndex(const size_t index)
	{
		if (index == 0)
		{
			return p0;
		}
		else if (index == 1)
		{
			return p1;
		}
		else if (index == 2)
		{
			return p2;
		}
		else if (index == 3)
		{
			return p3;
		}
		else
		{
			ThrowPointAtIndexOutOfRange();
		}
	}

	inline const Quad::position_type& Quad::pointAtIndex(const size_t index) const
	{
		if (index == 0)
		{
			return p0;
		}
		else if (index == 1)
		{
			return p1;
		}
		else if (index == 2)
		{
			return p2;
		}
		else if (index == 3)
		{
			return p3;
		}
		else
		{
			ThrowPointAtIndexOutOfRange();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	sideAtIndex
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Quad::sideAtIndex(const size_t index) const
	{
		if (index == 0)
		{
			return{ p0, p1 };
		}
		else if (index == 1)
		{
			return{ p1, p2 };
		}
		else if (index == 2)
		{
			return{ p2, p3 };
		}
		else if (index == 3)
		{
			return{ p3, p0 };
		}
		else
		{
			ThrowSideAtIndexOutOfRange();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	triangleAtIndex
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle Quad::triangleAtIndex(const size_t index) const
	{
		if (index == 0)
		{
			return{ p0, p1, p3 };
		}
		else if (index == 1)
		{
			return{ p3, p1, p2 };
		}
		else
		{
			ThrowTriangleAtIndexOutOfRange();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad::value_type Quad::area() const noexcept
	{
		const value_type s0 = Abs((p0.x - p2.x) * (p1.y - p0.y) - (p0.x - p1.x) * (p2.y - p0.y));
		const value_type s1 = Abs((p2.x - p0.x) * (p3.y - p2.y) - (p2.x - p3.x) * (p0.y - p2.y));
		return ((s0 + s1) * 0.5);
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad Quad::lerp(const Quad& other, const double f) const noexcept
	{
		return{ p0.lerp(other.p0, f), p1.lerp(other.p1, f), p2.lerp(other.p2, f), p3.lerp(other.p3, f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 Quad::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool Quad::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersect(*this, other);
	}
}
