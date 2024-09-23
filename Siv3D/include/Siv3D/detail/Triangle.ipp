//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
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

	constexpr Triangle::Triangle(const value_type sides) noexcept
		: p0{ 0.0, (-Math::InvSqrt3 * sides) }
		, p1{ (sides * 0.5), (sides / (2.0 * Math::Sqrt3)) }
		, p2{ -p1.x, p1.y } {}

	constexpr Triangle::Triangle(const Concept::Arithmetic auto sides) noexcept
		: Triangle{ static_cast<value_type>(sides) } {}

	Triangle::Triangle(const Concept::Arithmetic auto sides, const Concept::Arithmetic auto angle) noexcept
		: Triangle{ static_cast<value_type>(sides), static_cast<value_type>(angle) } {}

	constexpr Triangle::Triangle(const value_type x, const value_type y, const value_type sides) noexcept
		: Triangle{ Triangle{ sides }.moveBy(x, y) } {}

	constexpr Triangle::Triangle(const Concept::Arithmetic auto x, const Concept::Arithmetic auto y, const Concept::Arithmetic auto sides) noexcept
		: Triangle{ static_cast<value_type>(x), static_cast<value_type>(y), static_cast<value_type>(sides) } {}

	constexpr Triangle::Triangle(const position_type& pos, const value_type sides) noexcept
		: Triangle{ pos.x, pos.y, sides } {}

	constexpr Triangle::Triangle(const position_type& pos, const Concept::Arithmetic auto sides) noexcept
		: Triangle{ pos.x, pos.y, static_cast<value_type>(sides) } {}

	inline Triangle::Triangle(const value_type x, const value_type y, const value_type sides, const value_type angle) noexcept
		: Triangle{ Triangle{ sides, angle }.moveBy(x, y) } {}

	Triangle::Triangle(const Concept::Arithmetic auto x, const Concept::Arithmetic auto y, const Concept::Arithmetic auto sides, const value_type angle) noexcept
		: Triangle{ Triangle{ static_cast<value_type>(sides), static_cast<value_type>(angle) }.moveBy(static_cast<value_type>(x), static_cast<value_type>(y)) } {}

	inline Triangle::Triangle(const position_type& pos, const value_type sides, const value_type angle) noexcept
		: Triangle{ pos.x, pos.y, sides, angle } {}

	Triangle::Triangle(const position_type& pos, const Concept::Arithmetic auto sides, const value_type angle) noexcept
		: Triangle{ pos.x, pos.y, static_cast<value_type>(sides), angle } {}

	constexpr Triangle::Triangle(const value_type x0, const value_type y0, const value_type x1, const value_type y1, const value_type x2, const value_type y2) noexcept
		: p0{ x0, y0 }
		, p1{ x1, y1 }
		, p2{ x2, y2 } {}

	constexpr Triangle::Triangle(const Concept::Arithmetic auto x0, const Concept::Arithmetic auto y0, const Concept::Arithmetic auto x1, const Concept::Arithmetic auto y1, const Concept::Arithmetic auto x2, const Concept::Arithmetic auto y2) noexcept
		: p0{ static_cast<value_type>(x0), static_cast<value_type>(y0) }
		, p1{ static_cast<value_type>(x1), static_cast<value_type>(y1) }
		, p2{ static_cast<value_type>(x2), static_cast<value_type>(y2) } {}

	constexpr Triangle::Triangle(const position_type& _p0, const position_type& _p1, const position_type& _p2) noexcept
		: p0{ _p0 }
		, p1{ _p1 }
		, p2{ _p2 } {}

	////////////////////////////////////////////////////////////////
	//
	//	withP0
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle Triangle::withP0(const value_type _x, const value_type _y) const noexcept
	{
		return{ position_type{ _x, _y }, p1, p2 };
	}

	constexpr Triangle Triangle::withP0(const position_type _p0) const noexcept
	{
		return{ _p0, p1, p2 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withP1
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle Triangle::withP1(const value_type _x, const value_type _y) const noexcept
	{
		return{ p0, position_type{ _x, _y }, p2 };
	}

	constexpr Triangle Triangle::withP1(const position_type _p1) const noexcept
	{
		return{ p0, _p1, p2 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withP2
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle Triangle::withP2(const value_type _x, const value_type _y) const noexcept
	{
		return{ p0, p1, position_type{ _x, _y } };
	}

	constexpr Triangle Triangle::withP2(const position_type _p2) const noexcept
	{
		return{ p0, p1, _p2 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withCentroid
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle Triangle::withCentroid(const value_type _x, const value_type _y) const noexcept
	{
		return movedBy(position_type{ _x, _y } - centroid());
	}

	constexpr Triangle Triangle::withCentroid(const position_type _centroid) const noexcept
	{
		return movedBy(_centroid - centroid());
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle& Triangle::set(const value_type x0, const value_type y0, const value_type x1, const value_type y1, const value_type x2, const value_type y2) noexcept
	{
		p0.set(x0, y0);
		p1.set(x1, y1);
		p2.set(x2, y2);
		return *this;
	}

	constexpr Triangle& Triangle::set(const position_type& _p0, const position_type& _p1, const position_type& _p2) noexcept
	{
		p0.set(_p0);
		p1.set(_p1);
		p2.set(_p2);
		return *this;
	}

	constexpr Triangle& Triangle::set(const Triangle& triangle) noexcept
	{
		p0.set(triangle.p0);
		p1.set(triangle.p1);
		p2.set(triangle.p2);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setCentroid
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle& Triangle::setCentroid(const value_type x, const value_type y) noexcept
	{
		return moveBy(position_type{ x, y } - centroid());
	}

	constexpr Triangle& Triangle::setCentroid(const position_type pos) noexcept
	{
		return moveBy(pos - centroid());
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle Triangle::movedBy(const value_type x, const value_type y) const noexcept
	{
		return{ p0.movedBy(x, y), p1.movedBy(x, y), p2.movedBy(x, y) };
	}

	constexpr Triangle Triangle::movedBy(const position_type v) const noexcept
	{
		return{ p0.movedBy(v), p1.movedBy(v), p2.movedBy(v) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle& Triangle::moveBy(const value_type x, const value_type y) noexcept
	{
		p0.moveBy(x, y);
		p1.moveBy(x, y);
		p2.moveBy(x, y);
		return *this;
	}

	constexpr Triangle& Triangle::moveBy(const position_type v) noexcept
	{
		p0.moveBy(v);
		p1.moveBy(v);
		p2.moveBy(v);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromOrigin
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle Triangle::scaledFromOrigin(const value_type s) const noexcept
	{
		return{ (p0 * s), (p1 * s), (p2 * s) };
	}

	constexpr Triangle Triangle::scaledFromOrigin(const value_type sx, const value_type sy) const noexcept
	{
		return scaledFromOrigin(Vec2{ sx, sy });
	}

	constexpr Triangle Triangle::scaledFromOrigin(const Vec2 s) const noexcept
	{
		return{ (p0 * s), (p1 * s), (p2 * s) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFromOrigin
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle& Triangle::scaleFromOrigin(const value_type s) noexcept
	{
		p0 *= s;
		p1 *= s;
		p2 *= s;
		return *this;
	}

	constexpr Triangle& Triangle::scaleFromOrigin(const value_type sx, const value_type sy) noexcept
	{
		return scaleFromOrigin(Vec2{ sx, sy });
	}

	constexpr Triangle& Triangle::scaleFromOrigin(const Vec2 s) noexcept
	{
		p0 *= s;
		p1 *= s;
		p2 *= s;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFrom
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle Triangle::scaledFrom(const position_type pos, const value_type s) const noexcept
	{
		return{ (pos + (p0 - pos) * s), (pos + (p1 - pos) * s), (pos + (p2 - pos) * s) };
	}

	constexpr Triangle Triangle::scaledFrom(const position_type pos, const value_type sx, const value_type sy) const noexcept
	{
		return scaledFrom(pos, position_type{ sx, sy });
	}

	constexpr Triangle Triangle::scaledFrom(const position_type pos, const Vec2 s) const noexcept
	{
		return{ (pos + (p0 - pos) * s), (pos + (p1 - pos) * s), (pos + (p2 - pos) * s) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFrom
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle& Triangle::scaleFrom(const position_type pos, const value_type s) noexcept
	{
		p0 = (pos + (p0 - pos) * s);
		p1 = (pos + (p1 - pos) * s);
		p2 = (pos + (p2 - pos) * s);
		return *this;
	}

	constexpr Triangle& Triangle::scaleFrom(const position_type pos, const value_type sx, const value_type sy) noexcept
	{
		return scaleFrom(pos, position_type{ sx, sy });
	}

	constexpr Triangle& Triangle::scaleFrom(const position_type pos, const Vec2 s) noexcept
	{
		p0 = (pos + (p0 - pos) * s);
		p1 = (pos + (p1 - pos) * s);
		p2 = (pos + (p2 - pos) * s);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isClockwise
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Triangle::isClockwise() const noexcept
	{
		return Geometry2D::IsClockwise(p0, p1, p2);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasArea
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Triangle::hasArea() const noexcept
	{
		return (0.0 < area());
	}

	////////////////////////////////////////////////////////////////
	//
	//	centroidX, centroidY
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle::value_type Triangle::centroidX() const noexcept
	{
		return ((p0.x + p1.x + p2.x) / 3.0);
	}

	constexpr Triangle::value_type Triangle::centroidY() const noexcept
	{
		return ((p0.y + p1.y + p2.y) / 3.0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	centroid
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle::position_type Triangle::centroid() const noexcept
	{
		return{ ((p0.x + p1.x + p2.x) / 3.0), ((p0.y + p1.y + p2.y) / 3.0) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAtIndex
	//
	////////////////////////////////////////////////////////////////

	inline Triangle::position_type& Triangle::pointAtIndex(const size_t index)
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
		else
		{
			ThrowPointAtIndexOutOfRange();
		}
	}

	inline const Triangle::position_type& Triangle::pointAtIndex(const size_t index) const
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

	constexpr Line Triangle::sideAtIndex(const size_t index) const
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
			return{ p2, p0 };
		}
		else
		{
			ThrowSideAtIndexOutOfRange();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle::value_type Triangle::area() const noexcept
	{
		return (Abs((p0.x - p2.x) * (p1.y - p0.y) - (p0.x - p1.x) * (p2.y - p0.y)) * 0.5);
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle Triangle::lerp(const Triangle& other, const double f) const noexcept
	{
		return{ p0.lerp(other.p0, f), p1.lerp(other.p1, f), p2.lerp(other.p2, f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 Triangle::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool Triangle::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersect(*this, other);
	}
}
