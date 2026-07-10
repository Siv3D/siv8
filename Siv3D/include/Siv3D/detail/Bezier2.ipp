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

	constexpr Bezier2::Bezier2(const position_type& _p0, const position_type& _p1, const position_type& _p2) noexcept
		: p0(_p0)
		, p1(_p1)
		, p2(_p2) {}

	////////////////////////////////////////////////////////////////
	//
	//	withP0
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier2 Bezier2::withP0(const value_type _x, const value_type _y) const noexcept
	{
		return{ position_type{ _x, _y }, p1, p2 };
	}

	constexpr Bezier2 Bezier2::withP0(const position_type _p0) const noexcept
	{
		return{ _p0, p1, p2 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withP1
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier2 Bezier2::withP1(const value_type _x, const value_type _y) const noexcept
	{
		return{ p0, position_type{ _x, _y }, p2 };
	}

	constexpr Bezier2 Bezier2::withP1(const position_type _p1) const noexcept
	{
		return{ p0, _p1, p2 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withP2
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier2 Bezier2::withP2(const value_type _x, const value_type _y) const noexcept
	{
		return{ p0, p1, position_type{ _x, _y } };
	}

	constexpr Bezier2 Bezier2::withP2(const position_type _p2) const noexcept
	{
		return{ p0, p1, _p2 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffset
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier2 Bezier2::withOffset(const value_type x, const value_type y) const noexcept
	{
		return{ p0.withOffset(x, y), p1.withOffset(x, y), p2.withOffset(x, y) };
	}

	constexpr Bezier2 Bezier2::withOffset(const position_type v) const noexcept
	{
		return{ p0.withOffset(v), p1.withOffset(v), p2.withOffset(v) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffsetX, withOffsetY
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier2 Bezier2::withOffsetX(const value_type x) const noexcept
	{
		return{ p0.withOffsetX(x), p1.withOffsetX(x), p2.withOffsetX(x) };
	}

	constexpr Bezier2 Bezier2::withOffsetY(const value_type y) const noexcept
	{
		return{ p0.withOffsetY(y), p1.withOffsetY(y), p2.withOffsetY(y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier2 Bezier2::movedBy(const value_type x, const value_type y) const noexcept
	{
		return{ p0.movedBy(x, y), p1.movedBy(x, y), p2.movedBy(x, y) };
	}

	constexpr Bezier2 Bezier2::movedBy(const position_type v) const noexcept
	{
		return{ p0.movedBy(v), p1.movedBy(v), p2.movedBy(v) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier2& Bezier2::moveBy(const value_type x, const value_type y) noexcept
	{
		p0.moveBy(x, y);
		p1.moveBy(x, y);
		p2.moveBy(x, y);
		return *this;
	}

	constexpr Bezier2& Bezier2::moveBy(const position_type v) noexcept
	{
		p0.moveBy(v);
		p1.moveBy(v);
		p2.moveBy(v);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	controlPoints
	//
	////////////////////////////////////////////////////////////////

	constexpr std::array<Bezier2::position_type, 3> Bezier2::controlPoints() const noexcept
	{
		return{ p0, p1, p2 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	controlPointAtIndex
	//
	////////////////////////////////////////////////////////////////

	inline Bezier2::position_type& Bezier2::controlPointAtIndex(const size_t index)
	{
		switch (index)
		{
		case 0: return p0;
		case 1: return p1;
		case 2: return p2;
		default: ThrowControlPointAtIndexOutOfRange();
		}
	}

	inline const Bezier2::position_type& Bezier2::controlPointAtIndex(const size_t index) const
	{
		switch (index)
		{
		case 0: return p0;
		case 1: return p1;
		case 2: return p2;
		default: ThrowControlPointAtIndexOutOfRange();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAt
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 Bezier2::pointAt(const double t) const noexcept
	{
		return ((1.0 - t) * (1.0 - t) * p0 + 2.0 * (1.0 - t) * t * p1 + t * t * p2);
	}

	////////////////////////////////////////////////////////////////
	//
	//	derivativeAt
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 Bezier2::derivativeAt(const double t) const noexcept
	{
		const Vec2 v0 = (p1 - p0);
		const Vec2 v1 = (p2 - p1);
		return ((v0 * (1.0 - t) + v1 * t) * 2.0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	secondDerivative
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 Bezier2::secondDerivative() const noexcept
	{
		return ((p2 - (p1 * 2.0) + p0) * 2.0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	split
	//
	////////////////////////////////////////////////////////////////

	constexpr std::pair<Bezier2, Bezier2> Bezier2::split(const double t) const noexcept
	{
		const Vec2 p01 = Math::Lerp(p0, p1, t);
		const Vec2 p12 = Math::Lerp(p1, p2, t);
		const Vec2 p012 = Math::Lerp(p01, p12, t);
		return{ Bezier2{ p0, p01, p012 }, Bezier2{ p012, p12, p2 } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse, reversed
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier2& Bezier2::reverse() noexcept
	{
		std::swap(p0, p2);
		return *this;
	}

	constexpr Bezier2 Bezier2::reversed() const noexcept
	{
		return{ p2, p1, p0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	controlTriangle
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle Bezier2::controlTriangle() const noexcept
	{
		return Triangle::FromPoints(p0, p1, p2);
	}

	////////////////////////////////////////////////////////////////
	//
	//	controlPointsBoundingRect
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF Bezier2::controlPointsBoundingRect() const noexcept
	{
		const auto x = MinMax({ p0.x, p1.x, p2.x });
		const auto y = MinMax({ p0.y, p1.y, p2.y });
		return{ x.min, y.min, (x.max - x.min), (y.max - y.min) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	p0p1, p1p2, p0p2
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Bezier2::p0p1() const noexcept
	{
		return{ p0, p1 };
	}

	constexpr Line Bezier2::p1p2() const noexcept
	{
		return{ p1, p2 };
	}

	constexpr Line Bezier2::p0p2() const noexcept
	{
		return{ p0, p2 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	p1p0, p2p1, p2p0
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Bezier2::p1p0() const noexcept
	{
		return{ p1, p0 };
	}

	constexpr Line Bezier2::p2p1() const noexcept
	{
		return{ p2, p1 };
	}

	constexpr Line Bezier2::p2p0() const noexcept
	{
		return{ p2, p0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier2 Bezier2::lerp(const Bezier2& other, const double f) const noexcept
	{
		return{ p0.lerp(other.p0, f), p1.lerp(other.p1, f), p2.lerp(other.p2, f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toBezier3
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3 Bezier2::toBezier3() const noexcept
	{
		const Vec2 c1 = (p0 + (2.0 / 3.0) * (p1 - p0));
		const Vec2 c2 = (p2 + (2.0 / 3.0) * (p1 - p2));
		return{ p0, c1, c2, p2 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 Bezier2::hash() const noexcept
	{
		return BitwiseHash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool Bezier2::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersects(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersectsAt
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	Optional<Array<Vec2>> Bezier2::intersectsAt(const Shape2DType& other) const
	{
		return Geometry2D::IntersectsAt(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromMidpoint
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier2 Bezier2::FromMidpoint(const position_type& p0, const position_type& midpoint, const position_type& p2) noexcept
	{
		return{ p0, ((midpoint * 2.0) - ((p0 + p2) * 0.5)), p2 };
	}
}
