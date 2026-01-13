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
	//	getPos
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3::position_type Bezier3::getPos(const double t) const noexcept
	{
		const double u = (1.0 - t);
		return ((u * u * u) * p0
			+ (3.0 * u * u * t) * p1
			+ (3.0 * u * t * t) * p2
			+ (t * t * t) * p3);
	}
}
