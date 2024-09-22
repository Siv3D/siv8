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
	constexpr OffsetCircularBase<Float, Oclock>::OffsetCircularBase(const position_type& _center, const value_type _r, const value_type _theta) noexcept
		: center{ _center }
		, r{ _r }
		, theta{ _theta } {}

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock>::OffsetCircularBase(const position_type& _center, const Concept::Arithmetic auto _r, const Concept::Arithmetic auto _theta) noexcept
		: center{ _center }
		, r{ static_cast<value_type>(_r) }
		, theta{ static_cast<value_type>(_theta) } {}

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock>::OffsetCircularBase(const position_type& _center, const Arg::r_<value_type> _r, const Arg::theta_<value_type> _theta) noexcept
		: center{ _center }
		, r{ *_r }
		, theta{ *_theta } {}

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock>::OffsetCircularBase(const position_type& _center, const Arg::theta_<value_type> _theta, const Arg::r_<value_type> _r) noexcept
		: center{ _center }
		, r{ *_r }
		, theta{ *_theta } {}

	template <Concept::FloatingPoint Float, int32 Oclock>
	OffsetCircularBase<Float, Oclock>::OffsetCircularBase(const position_type& _center, const position_type& target) noexcept
		: center{ _center }
	{
		const auto v = (target - _center);	
		r = v.length();
		theta = ((r == 0) ? 0 : Offset(std::atan2(v.x, -v.y)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock> OffsetCircularBase<Float, Oclock>::operator +() const noexcept
	{
		return *this;
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	typename OffsetCircularBase<Float, Oclock>::position_type OffsetCircularBase<Float, Oclock>::operator +(const position_type v) const noexcept
	{
		return (toPosition() + v);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock> OffsetCircularBase<Float, Oclock>::operator -() const noexcept
	{
		return{ r, ClampAngle(theta - Math::Pi) };
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	typename OffsetCircularBase<Float, Oclock>::position_type OffsetCircularBase<Float, Oclock>::operator -(const position_type v) const noexcept
	{
		return (toPosition() - v);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withCenter
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock> OffsetCircularBase<Float, Oclock>::withCenter(const position_type _center) const noexcept
	{
		return{ _center, r, theta };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withBase
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	OffsetCircularBase<Float, Oclock> OffsetCircularBase<Float, Oclock>::withBase(const position_type _center) const noexcept
	{
		return OffsetCircular{ _center, toPosition() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withR
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock> OffsetCircularBase<Float, Oclock>::withR(const value_type _r) const noexcept
	{
		return{ center, _r, theta };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withTheta
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock> OffsetCircularBase<Float, Oclock>::withTheta(const value_type _theta) const noexcept
	{
		return{ center, r, _theta };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withTarget
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock> OffsetCircularBase<Float, Oclock>::withTarget(const position_type target) const noexcept
	{
		return OffsetCircular{ center, target };
	}

	////////////////////////////////////////////////////////////////
	//
	//	setCenter
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock>& OffsetCircularBase<Float, Oclock>::setCenter(const value_type x, const value_type y) noexcept
	{
		center.set(x, y);
		return *this;
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock>& OffsetCircularBase<Float, Oclock>::setCenter(const position_type _center) noexcept
	{
		center = _center;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setTarget
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	OffsetCircularBase<Float, Oclock>& OffsetCircularBase<Float, Oclock>::setTarget(const value_type x, const value_type y) noexcept
	{
		return (*this = OffsetCircular{ center, position_type{ x, y } });
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	OffsetCircularBase<Float, Oclock>& OffsetCircularBase<Float, Oclock>::setTarget(const position_type target) noexcept
	{
		return (*this = OffsetCircular{ center, target });
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock> OffsetCircularBase<Float, Oclock>::movedBy(const value_type x, const value_type y) const noexcept
	{
		return OffsetCircularBase{ center.movedBy(x, y), r, theta };
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock> OffsetCircularBase<Float, Oclock>::movedBy(const position_type v) const noexcept
	{
		return OffsetCircularBase{ center.movedBy(v), r, theta };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock>& OffsetCircularBase<Float, Oclock>::moveBy(const value_type x, const value_type y) noexcept
	{
		center.moveBy(x, y);
		return *this;
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock>& OffsetCircularBase<Float, Oclock>::moveBy(const position_type v) noexcept
	{
		center.moveBy(v);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock> OffsetCircularBase<Float, Oclock>::rotated(const value_type angle) const noexcept
	{
		return OffsetCircularBase{ *this }.rotate(angle);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr OffsetCircularBase<Float, Oclock>& OffsetCircularBase<Float, Oclock>::rotate(const value_type angle) noexcept
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
	OffsetCircularBase<Float, Oclock> OffsetCircularBase<Float, Oclock>::lerp(const OffsetCircularBase& other, double f) const noexcept
	{
		return OffsetCircularBase{ center.lerp(other.center, f), Math::Lerp(r, other.r, f), Math::LerpAngle(theta, other.theta, f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toFloat2, toVec2
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	Float2 OffsetCircularBase<Float, Oclock>::toFloat2() const noexcept
	{
		return{ (center.x + std::sin(theta + Clock()) * r),
				(center.y - std::cos(theta + Clock()) * r) };
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	Vec2 OffsetCircularBase<Float, Oclock>::toVec2() const noexcept
	{
		return{ (center.x + std::sin(theta + Clock()) * r),
				(center.y - std::cos(theta + Clock()) * r) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	fastToFloat2, fastToVec2
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	Float2 OffsetCircularBase<Float, Oclock>::fastToFloat2() const noexcept
	{
		const auto [s, c] = FastMath::SinCos(theta + Clock());
		return{ (center.x + s * r), (center.y - c * r) };
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	Vec2 OffsetCircularBase<Float, Oclock>::fastToVec2() const noexcept
	{
		const auto [s, c] = FastMath::SinCos(theta + Clock());
		return{ (center.x + s * r), (center.y - c * r) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toPosition
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	typename OffsetCircularBase<Float, Oclock>::position_type OffsetCircularBase<Float, Oclock>::toPosition() const noexcept
	{
		return{ (center.x + std::sin(theta + Clock()) * r),
				(center.y - std::cos(theta + Clock()) * r) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator position_type
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	OffsetCircularBase<Float, Oclock>::operator position_type() const noexcept
	{
		return{ (center.x + std::sin(theta + Clock()) * r),
				(center.y - std::cos(theta + Clock()) * r) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	asCircle
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	Circle OffsetCircularBase<Float, Oclock>::asCircle(const double _r) const noexcept
	{
		return toPosition().asCircle(_r);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	uint64 OffsetCircularBase<Float, Oclock>::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr Float OffsetCircularBase<Float, Oclock>::ClampAngle(const value_type theta) noexcept
	{
		if (theta <= -Math::Pi_v<Float>)
		{
			return (theta + Math::TwoPi_v<Float>);
		}
		else
		{
			return theta;
		}
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr Float OffsetCircularBase<Float, Oclock>::Clock() noexcept
	{
		return (Oclock * (Math::TwoPi_v<Float> / 12));
	}

	template <Concept::FloatingPoint Float, int32 Oclock>
	constexpr Float OffsetCircularBase<Float, Oclock>::Offset(const value_type theta) noexcept
	{
		return ClampAngle(theta - Clock());
	}
}
