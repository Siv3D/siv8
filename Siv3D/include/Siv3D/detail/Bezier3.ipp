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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3::Bezier3(const position_type& _p0, const position_type& _p1, const position_type& _p2, const position_type& _p3) noexcept
		: p0{ _p0 }
		, p1{ _p1 }
		, p2{ _p2 }
		, p3{ _p3 } {}

	////////////////////////////////////////////////////////////////
	//
	//	withP0
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3 Bezier3::withP0(const value_type _x, const value_type _y) const noexcept
	{
		return{ position_type{ _x, _y }, p1, p2, p3 };
	}

	constexpr Bezier3 Bezier3::withP0(const position_type _p0) const noexcept
	{
		return{ _p0, p1, p2, p3 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withP1
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3 Bezier3::withP1(const value_type _x, const value_type _y) const noexcept
	{
		return{ p0, position_type{ _x, _y }, p2, p3 };
	}

	constexpr Bezier3 Bezier3::withP1(const position_type _p1) const noexcept
	{
		return{ p0, _p1, p2, p3 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withP2
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3 Bezier3::withP2(const value_type _x, const value_type _y) const noexcept
	{
		return{ p0, p1, position_type{ _x, _y }, p3 };
	}

	constexpr Bezier3 Bezier3::withP2(const position_type _p2) const noexcept
	{
		return{ p0, p1, _p2, p3 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withP3
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3 Bezier3::withP3(const value_type _x, const value_type _y) const noexcept
	{
		return{ p0, p1, p2, position_type{ _x, _y } };
	}

	constexpr Bezier3 Bezier3::withP3(const position_type _p3) const noexcept
	{
		return{ p0, p1, p2, _p3 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffset
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3 Bezier3::withOffset(const value_type x, const value_type y) const noexcept
	{
		return{ p0.withOffset(x, y), p1.withOffset(x, y), p2.withOffset(x, y), p3.withOffset(x, y) };
	}

	constexpr Bezier3 Bezier3::withOffset(const position_type v) const noexcept
	{
		return{ p0.withOffset(v), p1.withOffset(v), p2.withOffset(v), p3.withOffset(v) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffsetX, withOffsetY
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3 Bezier3::withOffsetX(const value_type x) const noexcept
	{
		return{ p0.withOffsetX(x), p1.withOffsetX(x), p2.withOffsetX(x), p3.withOffsetX(x) };
	}

	constexpr Bezier3 Bezier3::withOffsetY(const value_type y) const noexcept
	{
		return{ p0.withOffsetY(y), p1.withOffsetY(y), p2.withOffsetY(y), p3.withOffsetY(y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3 Bezier3::movedBy(const value_type x, const value_type y) const noexcept
	{
		return{ p0.movedBy(x, y), p1.movedBy(x, y), p2.movedBy(x, y), p3.movedBy(x, y) };
	}

	constexpr Bezier3 Bezier3::movedBy(const position_type v) const noexcept
	{
		return{ p0.movedBy(v), p1.movedBy(v), p2.movedBy(v), p3.movedBy(v) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3& Bezier3::moveBy(const value_type x, const value_type y) noexcept
	{
		p0.moveBy(x, y);
		p1.moveBy(x, y);
		p2.moveBy(x, y);
		p3.moveBy(x, y);
		return *this;
	}

	constexpr Bezier3& Bezier3::moveBy(const position_type v) noexcept
	{
		p0.moveBy(v);
		p1.moveBy(v);
		p2.moveBy(v);
		p3.moveBy(v);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAtIndex
	//
	////////////////////////////////////////////////////////////////

	inline Bezier3::position_type& Bezier3::pointAtIndex(const size_t index)
	{
		switch (index)
		{
		case 0: return p0;
		case 1: return p1;
		case 2: return p2;
		case 3: return p3;
		default: ThrowPointAtIndexOutOfRange();
		}
	}

	inline const Bezier3::position_type& Bezier3::pointAtIndex(const size_t index) const
	{
		switch (index)
		{
		case 0: return p0;
		case 1: return p1;
		case 2: return p2;
		case 3: return p3;
		default: ThrowPointAtIndexOutOfRange();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAt
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3::position_type Bezier3::pointAt(const double t) const noexcept
	{
		const double u = (1.0 - t);
		return ((u * u * u) * p0
			+ (3.0 * u * u * t) * p1
			+ (3.0 * u * t * t) * p2
			+ (t * t * t) * p3);
	}

	////////////////////////////////////////////////////////////////
	//
	//	derivativeAt
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3::position_type Bezier3::derivativeAt(const double t) const noexcept
	{
		const double u = (1.0 - t);
		const position_type a = (p1 - p0);
		const position_type b = (p2 - p1);
		const position_type c = (p3 - p2);
		return (3.0 * (u * u) * a
			+ 6.0 * (u * t) * b
			+ 3.0 * (t * t) * c);
	}

	////////////////////////////////////////////////////////////////
	//
	//	secondDerivativeAt
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3::position_type Bezier3::secondDerivativeAt(const double t) const noexcept
	{
		const double u = (1.0 - t);
		const position_type a = (p2 - (p1 * 2.0) + p0);
		const position_type b = (p3 - (p2 * 2.0) + p1);
		return (6.0 * u * a + 6.0 * t * b);
	}

	////////////////////////////////////////////////////////////////
	//
	//	thirdDerivative
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3::position_type Bezier3::thirdDerivative() const noexcept
	{
		return ((p3 - (p2 * 3.0) + (p1 * 3.0) - p0) * 6.0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	split
	//
	////////////////////////////////////////////////////////////////

	constexpr std::pair<Bezier3, Bezier3> Bezier3::split(const double t) const noexcept
	{
		const Vec2 p01 = Math::Lerp(p0, p1, t);
		const Vec2 p12 = Math::Lerp(p1, p2, t);
		const Vec2 p23 = Math::Lerp(p2, p3, t);

		const Vec2 p012 = Math::Lerp(p01, p12, t);
		const Vec2 p123 = Math::Lerp(p12, p23, t);

		const Vec2 p0123 = Math::Lerp(p012, p123, t);

		return{ Bezier3{ p0, p01,  p012,  p0123 },
				Bezier3{ p0123, p123, p23,  p3    } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse, reversed
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3& Bezier3::reverse() noexcept
	{
		std::swap(p0, p3);
		std::swap(p1, p2);
		return *this;
	}

	constexpr Bezier3 Bezier3::reversed() const noexcept
	{
		return{ p3, p2, p1, p0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	controlPointsBoundingRect
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF Bezier3::controlPointsBoundingRect() const noexcept
	{
		const auto x = MinMax({ p0.x, p1.x, p2.x, p3.x });
		const auto y = MinMax({ p0.y, p1.y, p2.y, p3.y });
		return{ x.min, y.min, (x.max - x.min), (y.max - y.min) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	p0p1, p1p2, p2p3, p0p3
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Bezier3::p0p1() const noexcept
	{
		return{ p0, p1 };
	}

	constexpr Line Bezier3::p1p2() const noexcept
	{
		return{ p1, p2 };
	}

	constexpr Line Bezier3::p2p3() const noexcept
	{
		return{ p2, p3 };
	}

	constexpr Line Bezier3::p0p3() const noexcept
	{
		return{ p0, p3 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	p1p0, p2p1, p3p2, p3p0
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Bezier3::p1p0() const noexcept
	{
		return{ p1, p0 };
	}

	constexpr Line Bezier3::p2p1() const noexcept
	{
		return{ p2, p1 };
	}

	constexpr Line Bezier3::p3p2() const noexcept
	{
		return{ p3, p2 };
	}

	constexpr Line Bezier3::p3p0() const noexcept
	{
		return{ p3, p0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3 Bezier3::lerp(const Bezier3& other, const double f) const noexcept
	{
		return{ p0.lerp(other.p0, f), p1.lerp(other.p1, f), p2.lerp(other.p2, f), p3.lerp(other.p3, f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toBezier2
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier2 Bezier3::toBezier2() const noexcept
	{
		return{ p0, (((p1 + p2) * 3.0 - (p0 + p3)) * 0.25), p3 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 Bezier3::hash() const noexcept
	{
		return BitwiseHash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	bool Bezier3::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersect(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromHermite
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3 Bezier3::FromHermite(const position_type& p0, const Vec2& v0, const position_type& p3, const Vec2& v3) noexcept
	{
		return{ p0, (p0 + v0 / 3.0), (p3 - v3 / 3.0), p3 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromCatmullRom
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3 Bezier3::FromCatmullRom(const position_type& pPrev, const position_type& p0, const position_type& p3, const position_type& pNext, const double tension) noexcept
	{
		const double s = ((1.0 - tension) * 0.5);
		const Vec2 v0 = ((p3 - pPrev) * s);
		const Vec2 v3 = ((pNext - p0) * s);
		return FromHermite(p0, v0, p3, v3);
	}
}
