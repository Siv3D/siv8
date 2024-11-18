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

	template <class Type>
	constexpr Vector3D<Type>::Vector3D(const value_type _x, const value_type _y, const value_type _z) noexcept
		: x{ _x }
		, y{ _y }
		, z{ _z } {}

	template <class Type>
	constexpr Vector3D<Type>::Vector3D(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _z) noexcept
		: x{ static_cast<value_type>(_x) }
		, y{ static_cast<value_type>(_y) }
		, z{ static_cast<value_type>(_z) } {}

	template <class Type>
	constexpr Vector3D<Type>::Vector3D(const Point3D p) noexcept
		: x{ static_cast<value_type>(p.x) }
		, y{ static_cast<value_type>(p.y) }
		, z{ static_cast<value_type>(p.z) } {}

	template <class Type>
	template <class U>
	constexpr Vector3D<Type>::Vector3D(const Vector3D<U>& v) noexcept
		: x{ static_cast<value_type>(v.x) }
		, y{ static_cast<value_type>(v.y) }
		, z{ static_cast<value_type>(v.z) } {}

	template <class Type>
	constexpr Vector3D<Type>::Vector3D(const Vector2D<value_type>& xy, const Concept::Arithmetic auto _z) noexcept
		: x{ xy.x }
		, y{ xy.y }
		, z{ static_cast<value_type>(_z) } {}

	template <class Type>
	constexpr Vector3D<Type>::Vector3D(const Concept::Arithmetic auto _x, const Vector2D<value_type>& yz) noexcept
		: x{ static_cast<value_type>(_x) }
		, y{ yz.x }
		, z{ yz.y } {}

	////////////////////////////////////////////////////////////////
	//
	//	elem
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector3D<Type>::value_type Vector3D<Type>::elem(const size_t index) const noexcept
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

	template <class Type>
	constexpr typename Vector3D<Type>::value_type* Vector3D<Type>::getPointer() noexcept
	{
		return &x;
	}

	template <class Type>
	constexpr const typename Vector3D<Type>::value_type* Vector3D<Type>::getPointer() const noexcept
	{
		return &x;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::operator +() const noexcept
	{
		return *this;
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::operator +(const Vector3D v) const noexcept
	{
		return{ (x + v.x), (y + v.y), (z + v.z) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::operator -() const noexcept
	{
		return{ -x, -y, -z };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::operator -(const Vector3D v) const noexcept
	{
		return{ (x - v.x), (y - v.y), (z - v.z) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::operator *(const value_type s) const noexcept
	{
		return{ (x * s), (y * s), (z * s) };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::operator *(const Vector3D v) const noexcept
	{
		return{ (x * v.x), (y * v.y), (z * v.z) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::operator /(const value_type s) const noexcept
	{
		const auto invS = (static_cast<value_type>(1.0) / s);

		return{ (x * invS), (y * invS), (z * invS) };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::operator /(const Vector3D v) const noexcept
	{
		return{ (x / v.x), (y / v.y), (z / v.z) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type>& Vector3D<Type>::operator +=(const Vector3D v) noexcept
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

	template <class Type>
	constexpr Vector3D<Type>& Vector3D<Type>::operator -=(const Vector3D v) noexcept
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

	template <class Type>
	constexpr Vector3D<Type>& Vector3D<Type>::operator *=(const value_type s) noexcept
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	template <class Type>
	constexpr Vector3D<Type>& Vector3D<Type>::operator *=(const Vector3D v) noexcept
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

	template <class Type>
	constexpr Vector3D<Type>& Vector3D<Type>::operator /=(const value_type s) noexcept
	{
		const auto invS = (static_cast<value_type>(1.0) / s);
		x *= invS;
		y *= invS;
		z *= invS;
		return *this;
	}

	template <class Type>
	constexpr Vector3D<Type>& Vector3D<Type>::operator /=(const Vector3D v) noexcept
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

	template <class Type>
	constexpr bool Vector3D<Type>::epsilonEquals(const Vector3D other, const value_type epsilon) const noexcept
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

	template <class Type>
	constexpr bool Vector3D<Type>::hasSameDirection(const Vector3D other) const noexcept
	{
		return (0 < dot(other));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasOppositeDirection
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr bool Vector3D<Type>::hasOppositeDirection(const Vector3D other) const noexcept
	{
		return (dot(other) < 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isZero
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr bool Vector3D<Type>::isZero() const noexcept
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

	template <class Type>
	bool Vector3D<Type>::hasNaN() const noexcept
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

	template <class Type>
	constexpr typename Vector3D<Type>::value_type Vector3D<Type>::minComponent() const noexcept
	{
		return Min(Min(x, y), z);
	}

	template <class Type>
	constexpr typename Vector3D<Type>::value_type Vector3D<Type>::maxComponent() const noexcept
	{
		return Max(Max(x, y), z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr void Vector3D<Type>::clear() noexcept
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

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::withX(const value_type _x) const noexcept
	{
		return{ _x, y, z };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::withY(const value_type _y) const noexcept
	{
		return{ x, _y, z };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::withZ(const value_type _z) const noexcept
	{
		return{ x, y, _z };
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type>& Vector3D<Type>::set(const Vector2D<value_type>& xy, const value_type _z) noexcept
	{
		x = xy.x;
		y = xy.y;
		z = _z;
		return *this;
	}

	template <class Type>
	constexpr Vector3D<Type>& Vector3D<Type>::set(const value_type _x, const Vector2D<value_type>& yz) noexcept
	{
		x = _x;
		y = yz.x;
		z = yz.y;
		return *this;
	}

	template <class Type>
	constexpr Vector3D<Type>& Vector3D<Type>::set(const value_type _x, const value_type _y, const value_type _z) noexcept
	{
		x = _x;
		y = _y;
		z = _z;
		return *this;
	}

	template <class Type>
	constexpr Vector3D<Type>& Vector3D<Type>::set(const Vector3D v) noexcept
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

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::movedBy(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		return{ (x + _x), (y + _y), (z + _z) };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::movedBy(const Vector3D v) const noexcept
	{
		return{ (x + v.x), (y + v.y), (z + v.z) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type>& Vector3D<Type>::moveBy(const value_type _x, const value_type _y, const value_type _z) noexcept
	{
		x += _x;
		y += _y;
		z += _z;
		return *this;
	}

	template <class Type>
	constexpr Vector3D<Type>& Vector3D<Type>::moveBy(const Vector3D v) noexcept
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

	template <class Type>
	template <class U>
	constexpr auto Vector3D<Type>::dot(const Vector3D<U> v) const noexcept
	{
		return ((x * v.x) + (y * v.y) + (z * v.z));
	}

	////////////////////////////////////////////////////////////////
	//
	//	cross
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	template <class U>
	constexpr auto Vector3D<Type>::cross(const Vector3D<U> v) const noexcept
	{
		return Vector3D<CommonFloat_t<Type, U>>{
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

	template <class Type>
	template <class U>
	auto Vector3D<Type>::angleTo(const Vector3D<U> v) const noexcept
	{
		using ResultType = CommonFloat_t<Type, U>;

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

	template <class Type>
	template <class U>
	constexpr auto Vector3D<Type>::projectOnVector(const Vector3D<U> v) const noexcept
	{
		using ResultType = CommonFloat_t<Type, U>;

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

	template <class Type>
	template <class U>
	constexpr auto Vector3D<Type>::projectOnPlane(const Vector3D<U> planeNormal) const noexcept
	{
		return (*this - projectOnVector(planeNormal));
	}

	////////////////////////////////////////////////////////////////
	//
	//	length
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	typename Vector3D<Type>::value_type Vector3D<Type>::length() const noexcept
	{
		return std::sqrt((x * x) + (y * y) + (z * z));
	}

	////////////////////////////////////////////////////////////////
	//
	//	lengthSq
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector3D<Type>::value_type Vector3D<Type>::lengthSq() const noexcept
	{
		return ((x * x) + (y * y) + (z * z));
	}

	////////////////////////////////////////////////////////////////
	//
	//	invLength
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	typename Vector3D<Type>::value_type Vector3D<Type>::invLength() const noexcept
	{
		return (static_cast<value_type>(1.0) / length());
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanLength
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector3D<Type>::value_type Vector3D<Type>::manhattanLength() const noexcept
	{
		return (Abs(x) + Abs(y) + Abs(z));
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanDistanceFrom
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector3D<Type>::value_type Vector3D<Type>::manhattanDistanceFrom(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		const auto xMinMax = MinMax(x, _x);
		const auto yMinMax = MinMax(y, _y);
		const auto zMinMax = MinMax(z, _z);
		return ((xMinMax.second - xMinMax.first)
			  + (yMinMax.second - yMinMax.first)
			  + (zMinMax.second - zMinMax.first));
	}

	template <class Type>
	constexpr typename Vector3D<Type>::value_type Vector3D<Type>::manhattanDistanceFrom(const Vector3D v) const noexcept
	{
		return manhattanDistanceFrom(v.x, v.y, v.z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanDistanceTo
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector3D<Type>::value_type Vector3D<Type>::manhattanDistanceTo(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		return manhattanDistanceFrom(_x, _y, _z);
	}

	template <class Type>
	constexpr typename Vector3D<Type>::value_type Vector3D<Type>::manhattanDistanceTo(const Vector3D v) const noexcept
	{
		return manhattanDistanceFrom(v.x, v.y, v.z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFrom
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	typename Vector3D<Type>::value_type Vector3D<Type>::distanceFrom(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		return std::sqrt(distanceFromSq(_x, _y, _z));
	}

	template <class Type>
	typename Vector3D<Type>::value_type Vector3D<Type>::distanceFrom(const Vector3D v) const noexcept
	{
		return std::sqrt(distanceFromSq(v.x, v.y, v.z));
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFromSq
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector3D<Type>::value_type Vector3D<Type>::distanceFromSq(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		const value_type xx = (x - _x) * (x - _x);
		const value_type yy = (y - _y) * (y - _y);
		const value_type zz = (z - _z) * (z - _z);
		return (xx + yy + zz);
	}

	template <class Type>
	constexpr typename Vector3D<Type>::value_type Vector3D<Type>::distanceFromSq(const Vector3D v) const noexcept
	{
		return distanceFromSq(v.x, v.y, v.z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceTo
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	typename Vector3D<Type>::value_type Vector3D<Type>::distanceTo(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		return distanceFrom(_x, _y, _z);
	}

	template <class Type>
	typename Vector3D<Type>::value_type Vector3D<Type>::distanceTo(const Vector3D v) const noexcept
	{
		return distanceFrom(v.x, v.y, v.z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceToSq
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector3D<Type>::value_type Vector3D<Type>::distanceToSq(const value_type _x, const value_type _y, const value_type _z) const noexcept
	{
		return distanceFromSq(_x, _y, _z);
	}

	template <class Type>
	constexpr typename Vector3D<Type>::value_type Vector3D<Type>::distanceToSq(const Vector3D v) const noexcept
	{
		return distanceFromSq(v.x, v.y, v.z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withLength
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Vector3D<Type> Vector3D<Type>::withLength(const value_type _length) const noexcept
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

	template <class Type>
	Vector3D<Type>& Vector3D<Type>::setLength(const value_type _length) noexcept
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

	template <class Type>
	Vector3D<Type> Vector3D<Type>::withMaxLength(const value_type maxLength) const noexcept
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

	template <class Type>
	Vector3D<Type>& Vector3D<Type>::setMaxLength(const value_type maxLength) noexcept
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

	template <class Type>
	Vector3D<Type> Vector3D<Type>::normalized() const noexcept
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

	template <class Type>
	Vector3D<Type> Vector3D<Type>::normalized_or(const Vector3D valueIfZero) const noexcept
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

	template <class Type>
	Vector3D<Type>& Vector3D<Type>::normalize() noexcept
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

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::getMidpoint(const Vector3D other) const noexcept
	{
		constexpr value_type half(0.5);
		return{ (x * half + other.x * half), (y * half + other.y * half), (z * half + other.z * half) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::lerp(const Vector3D other, const value_type f) const noexcept
	{
		return{ (x + (other.x - x) * f), (y + (other.y - y) * f), (z + (other.z - z) * f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	asPoint3D
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Point3D Vector3D<Type>::asPoint3D() const noexcept
	{
		return{ static_cast<Point3D::value_type>(x), static_cast<Point3D::value_type>(y), static_cast<Point3D::value_type>(z) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	uint64 Vector3D<Type>::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	xx, xy, xz, yx, yy, yz, zx, zy, zz
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector3D<Type>::xx() const noexcept
	{
		return{ x, x };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector3D<Type>::xy() const noexcept
	{
		return{ x, y };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector3D<Type>::xz() const noexcept
	{
		return{ x, z };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector3D<Type>::yx() const noexcept
	{
		return{ y, x };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector3D<Type>::yy() const noexcept
	{
		return{ y, y };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector3D<Type>::yz() const noexcept
	{
		return{ y, z };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector3D<Type>::zx() const noexcept
	{
		return{ z, x };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector3D<Type>::zy() const noexcept
	{
		return{ z, y };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector3D<Type>::zz() const noexcept
	{
		return{ z, z };
	}

	////////////////////////////////////////////////////////////////
	//
	//	xxx, xyz, xzy, yyy, yxz, yzx, zzz, zxy, zyx
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::xxx() const noexcept
	{
		return{ x, x, x };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::xyz() const noexcept
	{
		return *this;
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::xzy() const noexcept
	{
		return{ x, z, y };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::yyy() const noexcept
	{
		return{ y, y, y };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::yxz() const noexcept
	{
		return{ y, x, z };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::yzx() const noexcept
	{
		return{ y, z, x };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::zzz() const noexcept
	{
		return{ z, z, z };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::zxy() const noexcept
	{
		return{ z, x, y };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::zyx() const noexcept
	{
		return{ z, y, x };
	}

	////////////////////////////////////////////////////////////////
	//
	//	xyz0, xyz1
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector4D<Type> Vector3D<Type>::xyz0() const noexcept
	{
		return{ x, y, z, 0 };
	}

	template <class Type>
	constexpr Vector4D<Type> Vector3D<Type>::xyz1() const noexcept
	{
		return{ x, y, z, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::Zero() noexcept
	{
		return{ 0, 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	One
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::One() noexcept
	{
		return{ 1, 1, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	All
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::All(const value_type value) noexcept
	{
		return{ value, value, value };
	}

	////////////////////////////////////////////////////////////////
	//
	//	UnitX, UnitY, UnitZ
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::UnitX() noexcept
	{
		return{ 1, 0, 0 };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::UnitY() noexcept
	{
		return{ 0, 1, 0 };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::UnitZ() noexcept
	{
		return{ 0, 0, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Left, Right, Up, Down, Forward, Backward
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::Left(const value_type length) noexcept
	{
		return{ -length, 0, 0 };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::Right(const value_type length) noexcept
	{
		return{ length, 0, 0 };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::Up(const value_type length) noexcept
	{
		return{ 0, length, 0 };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::Down(const value_type length) noexcept
	{
		return{ 0, -length, 0 };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::Forward(const value_type length) noexcept
	{
		return{ 0, 0, length };
	}

	template <class Type>
	constexpr Vector3D<Type> Vector3D<Type>::Backward(const value_type length) noexcept
	{
		return{ 0, 0, -length };
	}
}
