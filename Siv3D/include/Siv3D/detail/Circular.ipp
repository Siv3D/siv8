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

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr CircularBase<Float, Oclock>::CircularBase(const value_type _r, const value_type _theta) noexcept
		: r{ _r }
		, theta{ _theta } {}

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr CircularBase<Float, Oclock>::CircularBase(const Concept::Arithmetic auto _r, const value_type _theta) noexcept
		: r{ static_cast<value_type>(_r) }
		, theta{ _theta } {}

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr CircularBase<Float, Oclock>::CircularBase(const Arg::r_<value_type> _r, const Arg::theta_<value_type> _theta) noexcept
		: r{ *_r }
		, theta{ *_theta } {}

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr CircularBase<Float, Oclock>::CircularBase(const Arg::theta_<value_type> _theta, const Arg::r_<value_type> _r) noexcept
		: r{ *_r }
		, theta{ *_theta } {}

	template <Concept::FloatingPoint Float, int32 Oclock>
	CircularBase<Float, Oclock>::CircularBase(const position_type v) noexcept
		: r{ v.length() }
		, theta{ Offset(std::atan2(v.x, -v.y)) } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr CircularBase<Float, Oclock> CircularBase<Float, Oclock>::operator +() const noexcept
	{
		return *this;
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	typename CircularBase<Float, Oclock>::position_type CircularBase<Float, Oclock>::operator +(const position_type v) const noexcept
	{
		return (toPosition() + v);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr CircularBase<Float, Oclock> CircularBase<Float, Oclock>::operator -() const noexcept
	{
		return{ r, ClampAngle(theta - Math::Pi) };
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	typename CircularBase<Float, Oclock>::position_type CircularBase<Float, Oclock>::operator -(const position_type v) const noexcept
	{
		return (toPosition() - v);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withR
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr CircularBase<Float, Oclock> CircularBase<Float, Oclock>::withR(const value_type _r) const noexcept
	{
		return{ _r, theta };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withTheta
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr CircularBase<Float, Oclock> CircularBase<Float, Oclock>::withTheta(const value_type _theta) const noexcept
	{
		return{ r, _theta };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr CircularBase<Float, Oclock> CircularBase<Float, Oclock>::rotated(const value_type angle) const noexcept
	{
		return CircularBase{ *this }.rotate(angle);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr CircularBase<Float, Oclock>& CircularBase<Float, Oclock>::rotate(const value_type angle) noexcept
	{
		theta += angle;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	template<Concept::FloatingPoint Float, int32 Oclock>
	CircularBase<Float, Oclock> CircularBase<Float, Oclock>::lerp(const CircularBase& other, double f) const noexcept
	{
		return CircularBase{ Math::Lerp(r, other.r, f), Math::LerpAngle(theta, other.theta, f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toFloat2, toVec2
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	Float2 CircularBase<Float, Oclock>::toFloat2() const noexcept
	{
		return{ (std::sin(theta + Clock()) * r),
				(-std::cos(theta + Clock()) * r) };
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	Vec2 CircularBase<Float, Oclock>::toVec2() const noexcept
	{
		return{ (std::sin(theta + Clock()) * r),
				(-std::cos(theta + Clock()) * r) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	fastToFloat2, fastToVec2
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	Float2 CircularBase<Float, Oclock>::fastToFloat2() const noexcept
	{
		const auto [s, c] = FastMath::SinCos(theta + Clock());
		return{ (s * r), (-c * r) };
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	Vec2 CircularBase<Float, Oclock>::fastToVec2() const noexcept
	{
		const auto [s, c] = FastMath::SinCos(theta + Clock());
		return{ (s * r), (-c * r) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toPosition
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	typename CircularBase<Float, Oclock>::position_type CircularBase<Float, Oclock>::toPosition() const noexcept
	{
		return{ (std::sin(theta + Clock()) * r),
				(-std::cos(theta + Clock()) * r) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator position_type
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	CircularBase<Float, Oclock>::operator position_type() const noexcept
	{
		return{ (std::sin(theta + Clock()) * r),
				(-std::cos(theta + Clock()) * r) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	uint64 CircularBase<Float, Oclock>::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr Float CircularBase<Float, Oclock>::ClampAngle(const value_type theta) noexcept
	{
		if (theta <= -Math::Pi)
		{
			return (theta + Math::TwoPi_v<Float>);
		}
		else
		{
			return theta;
		}
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr Float CircularBase<Float, Oclock>::Clock() noexcept
	{
		return (Oclock * (Math::TwoPi_v<Float> / 12));
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr Float CircularBase<Float, Oclock>::Offset(const value_type theta) noexcept
	{
		return ClampAngle(theta - Clock());
	}
}
