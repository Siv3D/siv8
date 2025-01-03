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

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>::Vector3D(const value_type _x, const value_type _y, const value_type _z) noexcept
		: x{ _x }
		, y{ _y }
		, z{ _z } {}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>::Vector3D(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _z) noexcept
		: x{ static_cast<value_type>(_x) }
		, y{ static_cast<value_type>(_y) }
		, z{ static_cast<value_type>(_z) } {}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>::Vector3D(const Point3D p) noexcept
		: x{ static_cast<value_type>(p.x) }
		, y{ static_cast<value_type>(p.y) }
		, z{ static_cast<value_type>(p.z) } {}

	template <Concept::FloatingPoint Float>
	template <Concept::FloatingPoint U>
	constexpr Vector3D<Float>::Vector3D(const Vector3D<U>& v) noexcept
		: x{ static_cast<value_type>(v.x) }
		, y{ static_cast<value_type>(v.y) }
		, z{ static_cast<value_type>(v.z) } {}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>::Vector3D(const Vector2D<value_type>& xy, const Concept::Arithmetic auto _z) noexcept
		: x{ xy.x }
		, y{ xy.y }
		, z{ static_cast<value_type>(_z) } {}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>::Vector3D(const Concept::Arithmetic auto _x, const Vector2D<value_type>& yz) noexcept
		: x{ static_cast<value_type>(_x) }
		, y{ yz.x }
		, z{ yz.y } {}

	////////////////////////////////////////////////////////////////
	//
	//	elem
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type Vector3D<Float>::elem(const size_t index) const noexcept
	{
		if (index == 0)
		{
			return x;
		}
		else if (index == 1)
		{
			return y;
		}
		else if (index == 2)
		{
			return z;
		}
		else
		{
			return 0;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPointer
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type* Vector3D<Float>::getPointer() noexcept
	{
		return &x;
	}

	template <Concept::FloatingPoint Float>
	constexpr const typename Vector3D<Float>::value_type* Vector3D<Float>::getPointer() const noexcept
	{
		return &x;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::operator +() const noexcept
	{
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::operator +(const Vector3D v) const noexcept
	{
		return{ (x + v.x), (y + v.y), (z + v.z) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::operator -() const noexcept
	{
		return{ -x, -y, -z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::operator -(const Vector3D v) const noexcept
	{
		return{ (x - v.x), (y - v.y), (z - v.z) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::operator *(const value_type s) const noexcept
	{
		return{ (x * s), (y * s), (z * s) };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::operator *(const Vector3D v) const noexcept
	{
		return{ (x * v.x), (y * v.y), (z * v.z) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::operator /(const value_type s) const noexcept
	{
		const auto invS = (static_cast<value_type>(1.0) / s);

		return{ (x * invS), (y * invS), (z * invS) };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::operator /(const Vector3D v) const noexcept
	{
		return{ (x / v.x), (y / v.y), (z / v.z) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>& Vector3D<Float>::operator +=(const Vector3D v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>& Vector3D<Float>::operator -=(const Vector3D v) noexcept
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *=
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>& Vector3D<Float>::operator *=(const value_type s) noexcept
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>& Vector3D<Float>::operator *=(const Vector3D v) noexcept
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /=
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>& Vector3D<Float>::operator /=(const value_type s) noexcept
	{
		const auto invS = (static_cast<value_type>(1.0) / s);
		x *= invS;
		y *= invS;
		z *= invS;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>& Vector3D<Float>::operator /=(const Vector3D v) noexcept
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	epsilonEquals
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr bool Vector3D<Float>::epsilonEquals(const Vector3D other, const value_type epsilon) const noexcept
	{
		return ((AbsDiff(x, other.x) <= epsilon)
			&&  (AbsDiff(y, other.y) <= epsilon)
			&&  (AbsDiff(z, other.z) <= epsilon));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasSameDirection
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr bool Vector3D<Float>::hasSameDirection(const Vector3D other) const noexcept
	{
		return (0 < dot(other));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasOppositeDirection
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr bool Vector3D<Float>::hasOppositeDirection(const Vector3D other) const noexcept
	{
		return (dot(other) < 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isZero
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr bool Vector3D<Float>::isZero() const noexcept
	{
		return ((x == 0)
			&&  (y == 0)
			&&  (z == 0));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasNaN
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	bool Vector3D<Float>::hasNaN() const noexcept
	{
		return (std::isnan(x)
			||  std::isnan(y)
			||  std::isnan(z));
	}

	////////////////////////////////////////////////////////////////
	//
	//	minComponent, maxComponent
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type Vector3D<Float>::minComponent() const noexcept
	{
		return Min(Min(x, y), z);
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type Vector3D<Float>::maxComponent() const noexcept
	{
		return Max(Max(x, y), z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr void Vector3D<Float>::clear() noexcept
	{
		x = 0;
		y = 0;
		z = 0;
	}

	////////////////////////////////////////////////////////////////
	//
	//	withX, withY, withZ
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::withX(const value_type _x) const noexcept
	{
		return{ _x, y, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::withY(const value_type _y) const noexcept
	{
		return{ x, _y, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::withZ(const value_type _z) const noexcept
	{
		return{ x, y, _z };
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>& Vector3D<Float>::set(const Vector2D<value_type>& xy, const value_type _z) noexcept
	{
		x = xy.x;
		y = xy.y;
		z = _z;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>& Vector3D<Float>::set(const value_type _x, const Vector2D<value_type>& yz) noexcept
	{
		x = _x;
		y = yz.x;
		z = yz.y;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>& Vector3D<Float>::set(const value_type _x, const value_type _y, const value_type _z) noexcept
	{
		x = _x;
		y = _y;
		z = _z;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>& Vector3D<Float>::set(const Vector3D v) noexcept
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::movedBy(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		return{ (x + _x), (y + _y), (z + _z) };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::movedBy(const Vector3D v) const noexcept
	{
		return{ (x + v.x), (y + v.y), (z + v.z) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>& Vector3D<Float>::moveBy(const value_type _x, const value_type _y, const value_type _z) noexcept
	{
		x += _x;
		y += _y;
		z += _z;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float>& Vector3D<Float>::moveBy(const Vector3D v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	clamp, clamped
	//
	////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////
	//
	//	dot
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	template <Concept::FloatingPoint U>
	constexpr auto Vector3D<Float>::dot(const Vector3D<U> v) const noexcept
	{
		return ((x * v.x) + (y * v.y) + (z * v.z));
	}

	////////////////////////////////////////////////////////////////
	//
	//	cross
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	template <Concept::FloatingPoint U>
	constexpr auto Vector3D<Float>::cross(const Vector3D<U> v) const noexcept
	{
		return Vector3D<CommonFloat_t<Float, U>>{
			((y * v.z) - (z * v.y)),
			((z * v.x) - (x * v.z)),
			((x * v.y) - (y * v.x))
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	angleTo
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	template <Concept::FloatingPoint U>
	auto Vector3D<Float>::angleTo(const Vector3D<U> v) const noexcept
	{
		using ResultType = CommonFloat_t<Float, U>;

		const ResultType denom = std::sqrt(lengthSq() * v.lengthSq());

		if (denom < static_cast<ResultType>(0.0000001))
		{
			return Math::Pi_v<ResultType>;
		}

		const ResultType theta = (dot(v) / denom);

		return std::acos(Clamp(theta, static_cast<ResultType>(-1.0), static_cast<ResultType>(1.0)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	projectOnVector
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	template <Concept::FloatingPoint U>
	constexpr auto Vector3D<Float>::projectOnVector(const Vector3D<U> v) const noexcept
	{
		using ResultType = CommonFloat_t<Float, U>;

		if (v.lengthSq() < static_cast<ResultType>(0.0000001))
		{
			return Vector3D<ResultType>{ 0, 0, 0 };
		}

		return Vector3D<ResultType>{ v * (dot(v) / v.dot(v)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	projectOnPlane
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	template <Concept::FloatingPoint U>
	constexpr auto Vector3D<Float>::projectOnPlane(const Vector3D<U> planeNormal) const noexcept
	{
		return (*this - projectOnVector(planeNormal));
	}

	////////////////////////////////////////////////////////////////
	//
	//	length
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	typename Vector3D<Float>::value_type Vector3D<Float>::length() const noexcept
	{
		return std::sqrt((x * x) + (y * y) + (z * z));
	}

	////////////////////////////////////////////////////////////////
	//
	//	lengthSq
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type Vector3D<Float>::lengthSq() const noexcept
	{
		return ((x * x) + (y * y) + (z * z));
	}

	////////////////////////////////////////////////////////////////
	//
	//	invLength
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	typename Vector3D<Float>::value_type Vector3D<Float>::invLength() const noexcept
	{
		return (static_cast<value_type>(1.0) / length());
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanLength
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type Vector3D<Float>::manhattanLength() const noexcept
	{
		return (Abs(x) + Abs(y) + Abs(z));
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanDistanceFrom
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type Vector3D<Float>::manhattanDistanceFrom(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		const auto xMinMax = MinMax(x, _x);
		const auto yMinMax = MinMax(y, _y);
		const auto zMinMax = MinMax(z, _z);
		return ((xMinMax.second - xMinMax.first)
			  + (yMinMax.second - yMinMax.first)
			  + (zMinMax.second - zMinMax.first));
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type Vector3D<Float>::manhattanDistanceFrom(const Vector3D v) const noexcept
	{
		return manhattanDistanceFrom(v.x, v.y, v.z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanDistanceTo
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type Vector3D<Float>::manhattanDistanceTo(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		return manhattanDistanceFrom(_x, _y, _z);
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type Vector3D<Float>::manhattanDistanceTo(const Vector3D v) const noexcept
	{
		return manhattanDistanceFrom(v.x, v.y, v.z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFrom
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	typename Vector3D<Float>::value_type Vector3D<Float>::distanceFrom(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		return std::sqrt(distanceFromSq(_x, _y, _z));
	}

	template <Concept::FloatingPoint Float>
	typename Vector3D<Float>::value_type Vector3D<Float>::distanceFrom(const Vector3D v) const noexcept
	{
		return std::sqrt(distanceFromSq(v.x, v.y, v.z));
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFromSq
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type Vector3D<Float>::distanceFromSq(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		const value_type xx = (x - _x) * (x - _x);
		const value_type yy = (y - _y) * (y - _y);
		const value_type zz = (z - _z) * (z - _z);
		return (xx + yy + zz);
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type Vector3D<Float>::distanceFromSq(const Vector3D v) const noexcept
	{
		return distanceFromSq(v.x, v.y, v.z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceTo
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	typename Vector3D<Float>::value_type Vector3D<Float>::distanceTo(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		return distanceFrom(_x, _y, _z);
	}

	template <Concept::FloatingPoint Float>
	typename Vector3D<Float>::value_type Vector3D<Float>::distanceTo(const Vector3D v) const noexcept
	{
		return distanceFrom(v.x, v.y, v.z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceToSq
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type Vector3D<Float>::distanceToSq(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		return distanceFromSq(_x, _y, _z);
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector3D<Float>::value_type Vector3D<Float>::distanceToSq(const Vector3D v) const noexcept
	{
		return distanceFromSq(v.x, v.y, v.z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withLength
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	Vector3D<Float> Vector3D<Float>::withLength(const value_type _length) const noexcept
	{
		if (const value_type len = length())
		{
			return (*this * (_length / len));
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setLength
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	Vector3D<Float>& Vector3D<Float>::setLength(const value_type _length) noexcept
	{
		if (const value_type len = length())
		{
			*this *= (_length / len);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	withMaxLength
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	Vector3D<Float> Vector3D<Float>::withMaxLength(const value_type maxLength) const noexcept
	{
		if (const value_type len = length();
			maxLength < len)
		{
			return (*this * (maxLength / len));
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setMaxLength
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	Vector3D<Float>& Vector3D<Float>::setMaxLength(const value_type maxLength) noexcept
	{
		if (const value_type len = length();
			maxLength < len)
		{
			*this *= (maxLength / len);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	normalized
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	Vector3D<Float> Vector3D<Float>::normalized() const noexcept
	{
		const value_type lenSq = lengthSq();

		if (lenSq == 0)
		{
			return *this;
		}

		const value_type invLen = (static_cast<value_type>(1.0) / std::sqrt(lenSq));
		return{ (x * invLen), (y * invLen), (z * invLen) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	normalized_or
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	Vector3D<Float> Vector3D<Float>::normalized_or(const Vector3D valueIfZero) const noexcept
	{
		const value_type lenSq = lengthSq();

		if (lenSq == 0)
		{
			return valueIfZero;
		}

		const value_type invLen = (static_cast<value_type>(1.0) / std::sqrt(lenSq));
		return{ (x * invLen), (y * invLen), (z * invLen) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	normalize
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	Vector3D<Float>& Vector3D<Float>::normalize() noexcept
	{
		const value_type lenSq = lengthSq();

		if (lenSq == 0)
		{
			x = y = z = 0;
		}
		else
		{
			const value_type invLen = (static_cast<value_type>(1.0) / std::sqrt(lenSq));
			x *= invLen;
			y *= invLen;
			z *= invLen;
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMidpoint
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::getMidpoint(const Vector3D other) const noexcept
	{
		constexpr value_type half(0.5);
		return{ (x * half + other.x * half), (y * half + other.y * half), (z * half + other.z * half) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::lerp(const Vector3D other, const value_type f) const noexcept
	{
		return{ (x + (other.x - x) * f), (y + (other.y - y) * f), (z + (other.z - z) * f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	asPoint3D
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Point3D Vector3D<Float>::asPoint3D() const noexcept
	{
		return{ static_cast<Point3D::value_type>(x), static_cast<Point3D::value_type>(y), static_cast<Point3D::value_type>(z) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	uint64 Vector3D<Float>::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	xx, xy, xz, yx, yy, yz, zx, zy, zz
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector3D<Float>::xx() const noexcept
	{
		return{ x, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector3D<Float>::xy() const noexcept
	{
		return{ x, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector3D<Float>::xz() const noexcept
	{
		return{ x, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector3D<Float>::yx() const noexcept
	{
		return{ y, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector3D<Float>::yy() const noexcept
	{
		return{ y, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector3D<Float>::yz() const noexcept
	{
		return{ y, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector3D<Float>::zx() const noexcept
	{
		return{ z, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector3D<Float>::zy() const noexcept
	{
		return{ z, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector3D<Float>::zz() const noexcept
	{
		return{ z, z };
	}

	////////////////////////////////////////////////////////////////
	//
	//	xxx, xyz, xzy, yyy, yxz, yzx, zzz, zxy, zyx
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::xxx() const noexcept
	{
		return{ x, x, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::xyz() const noexcept
	{
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::xzy() const noexcept
	{
		return{ x, z, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::yyy() const noexcept
	{
		return{ y, y, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::yxz() const noexcept
	{
		return{ y, x, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::yzx() const noexcept
	{
		return{ y, z, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::zzz() const noexcept
	{
		return{ z, z, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::zxy() const noexcept
	{
		return{ z, x, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::zyx() const noexcept
	{
		return{ z, y, x };
	}

	////////////////////////////////////////////////////////////////
	//
	//	xyz0, xyz1
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector3D<Float>::xyz0() const noexcept
	{
		return{ x, y, z, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector3D<Float>::xyz1() const noexcept
	{
		return{ x, y, z, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::Zero() noexcept
	{
		return{ 0, 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	One
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::One() noexcept
	{
		return{ 1, 1, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	All
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::All(const value_type value) noexcept
	{
		return{ value, value, value };
	}

	////////////////////////////////////////////////////////////////
	//
	//	UnitX, UnitY, UnitZ
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::UnitX() noexcept
	{
		return{ 1, 0, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::UnitY() noexcept
	{
		return{ 0, 1, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::UnitZ() noexcept
	{
		return{ 0, 0, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Left, Right, Up, Down, Forward, Backward
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::Left(const value_type length) noexcept
	{
		return{ -length, 0, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::Right(const value_type length) noexcept
	{
		return{ length, 0, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::Up(const value_type length) noexcept
	{
		return{ 0, length, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::Down(const value_type length) noexcept
	{
		return{ 0, -length, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::Forward(const value_type length) noexcept
	{
		return{ 0, 0, length };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector3D<Float>::Backward(const value_type length) noexcept
	{
		return{ 0, 0, -length };
	}
}
