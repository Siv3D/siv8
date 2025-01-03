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
	constexpr Vector4D<Float>::Vector4D(const value_type _x, const value_type _y, const value_type _z, const value_type _w) noexcept
		: x{ _x }
		, y{ _y }
		, z{ _z }
		, w{ _w } {}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>::Vector4D(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _z, const Concept::Arithmetic auto _w) noexcept
		: x{ static_cast<value_type>(_x) }
		, y{ static_cast<value_type>(_y) }
		, z{ static_cast<value_type>(_z) }
		, w{ static_cast<value_type>(_w) } {}

	template <Concept::FloatingPoint Float>
	template <Concept::FloatingPoint U>
	constexpr Vector4D<Float>::Vector4D(const Vector4D<U>& v) noexcept
		: x{ static_cast<value_type>(v.x) }
		, y{ static_cast<value_type>(v.y) }
		, z{ static_cast<value_type>(v.z) }
		, w{ static_cast<value_type>(v.w) } {}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>::Vector4D(const Vector2D<value_type>& xy, const Concept::Arithmetic auto _z, const Concept::Arithmetic auto _w) noexcept
		: x{ xy.x }
		, y{ xy.y }
		, z{ static_cast<value_type>(_z) }
		, w{ static_cast<value_type>(_w) } {}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>::Vector4D(const Concept::Arithmetic auto _x, const Vector2D<value_type>& yz, const Concept::Arithmetic auto _w) noexcept
		: x{ static_cast<value_type>(_x) }
		, y{ yz.x }
		, z{ yz.y }
		, w{ static_cast<value_type>(_w) } {}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>::Vector4D(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Vector2D<value_type>& zw) noexcept
		: x{ static_cast<value_type>(_x) }
		, y{ static_cast<value_type>(_y) }
		, z{ zw.x }
		, w{ zw.y } {}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>::Vector4D(const Vector2D<value_type>& xy, const Vector2D<value_type>& zw) noexcept
		: x{ xy.x }
		, y{ xy.y }
		, z{ zw.x }
		, w{ zw.y } {}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>::Vector4D(const Vector3D<value_type>& xyz, const Concept::Arithmetic auto _w) noexcept
		: x{ xyz.x }
		, y{ xyz.y }
		, z{ xyz.z }
		, w{ static_cast<value_type>(_w) } {}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>::Vector4D(const Concept::Arithmetic auto _x, const Vector3D<value_type>& yzw) noexcept
		: x{ static_cast<value_type>(_x) }
		, y{ yzw.x }
		, z{ yzw.y }
		, w{ yzw.z } {}

	////////////////////////////////////////////////////////////////
	//
	//	elem
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector4D<Float>::value_type Vector4D<Float>::elem(const size_t index) const noexcept
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
		else if (index == 3)
		{
			return w;
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
	constexpr typename Vector4D<Float>::value_type* Vector4D<Float>::getPointer() noexcept
	{
		return &x;
	}

	template <Concept::FloatingPoint Float>
	constexpr const typename Vector4D<Float>::value_type* Vector4D<Float>::getPointer() const noexcept
	{
		return &x;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::operator +() const noexcept
	{
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::operator +(const Vector4D v) const noexcept
	{
		return{ (x + v.x), (y + v.y), (z + v.z), (w + v.w) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::operator -() const noexcept
	{
		return{ -x, -y, -z, -w };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::operator -(const Vector4D v) const noexcept
	{
		return{ (x - v.x), (y - v.y), (z - v.z), (w - v.w) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::operator *(const value_type s) const noexcept
	{
		return{ (x * s), (y * s), (z * s), (w * s) };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::operator *(const Vector4D v) const noexcept
	{
		return{ (x * v.x), (y * v.y), (z * v.z), (w * v.w) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::operator /(const value_type s) const noexcept
	{
		const auto invS = (static_cast<value_type>(1.0) / s);

		return{ (x * invS), (y * invS), (z * invS), (w * invS) };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::operator /(const Vector4D v) const noexcept
	{
		return{ (x / v.x), (y / v.y), (z / v.z), (w / v.w) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::operator +=(const Vector4D v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::operator -=(const Vector4D v) noexcept
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *=
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::operator *=(const value_type s) noexcept
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::operator *=(const Vector4D v) noexcept
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /=
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::operator /=(const value_type s) noexcept
	{
		const auto invS = (static_cast<value_type>(1.0) / s);
		x *= invS;
		y *= invS;
		z *= invS;
		w *= invS;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::operator /=(const Vector4D v) noexcept
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	epsilonEquals
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr bool Vector4D<Float>::epsilonEquals(const Vector4D other, const value_type epsilon) const noexcept
	{
		return ((AbsDiff(x, other.x) <= epsilon)
			&&  (AbsDiff(y, other.y) <= epsilon)
			&&  (AbsDiff(z, other.z) <= epsilon)
			&&  (AbsDiff(w, other.w) <= epsilon));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasSameDirection
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr bool Vector4D<Float>::hasSameDirection(const Vector4D other) const noexcept
	{
		return (0 < dot(other));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasOppositeDirection
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr bool Vector4D<Float>::hasOppositeDirection(const Vector4D other) const noexcept
	{
		return (dot(other) < 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isZero
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr bool Vector4D<Float>::isZero() const noexcept
	{
		return ((x == 0)
			&&  (y == 0)
			&&  (z == 0)
			&&  (w == 0));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasNaN
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	bool Vector4D<Float>::hasNaN() const noexcept
	{
		return (std::isnan(x)
			||  std::isnan(y)
			||  std::isnan(z)
			||  std::isnan(w));
	}

	////////////////////////////////////////////////////////////////
	//
	//	minComponent, maxComponent
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector4D<Float>::value_type Vector4D<Float>::minComponent() const noexcept
	{
		return Min(Min(x, y), Min(z, w));
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector4D<Float>::value_type Vector4D<Float>::maxComponent() const noexcept
	{
		return Max(Max(x, y), Max(z, w));
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr void Vector4D<Float>::clear() noexcept
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	////////////////////////////////////////////////////////////////
	//
	//	withX, withY, withZ, withW
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::withX(const value_type _x) const noexcept
	{
		return{ _x, y, z, w };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::withY(const value_type _y) const noexcept
	{
		return{ x, _y, z, w };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::withZ(const value_type _z) const noexcept
	{
		return{ x, y, _z, w };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::withW(const value_type _w) const noexcept
	{
		return{ x, y, z, _w };
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::set(const Vector2D<value_type>& xy, const Vector2D<value_type>& zw) noexcept
	{
		x = xy.x;
		y = xy.y;
		z = zw.x;
		w = zw.y;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::set(const Vector2D<value_type>& xy, const value_type _z, const value_type _w) noexcept
	{
		x = xy.x;
		y = xy.y;
		z = _z;
		w = _w;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::set(const value_type _x, const Vector2D<value_type>& yz, const value_type _w) noexcept
	{
		x = _x;
		y = yz.x;
		z = yz.y;
		w = _w;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::set(const value_type _x, const value_type _y, const Vector2D<value_type>& zw) noexcept
	{
		x = _x;
		y = _y;
		z = zw.x;
		w = zw.y;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::set(const Vector3D<value_type>& xyz, const value_type _w) noexcept
	{
		x = xyz.x;
		y = xyz.y;
		z = xyz.z;
		w = _w;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::set(const value_type _x, const Vector3D<value_type>& yzw) noexcept
	{
		x = _x;
		y = yzw.x;
		z = yzw.y;
		w = yzw.z;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::set(const value_type _x, const value_type _y, const value_type _z, const value_type _w) noexcept
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::set(const Vector4D v) noexcept
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::movedBy(const value_type _x, const value_type _y, const value_type _z, const value_type _w) const noexcept
	{
		return{ (x + _x), (y + _y), (z + _z), (w + _w) };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::movedBy(const Vector4D v) const noexcept
	{
		return{ (x + v.x), (y + v.y), (z + v.z), (w + v.w) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::moveBy(const value_type _x, const value_type _y, const value_type _z, const value_type _w) noexcept
	{
		x += _x;
		y += _y;
		z += _z;
		w += _w;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float>& Vector4D<Float>::moveBy(const Vector4D v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	dot
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	template <Concept::FloatingPoint U>
	constexpr auto Vector4D<Float>::dot(const Vector4D<U> v) const noexcept
	{
		return ((x * v.x) + (y * v.y) + (z * v.z) + (w * v.w));
	}

	////////////////////////////////////////////////////////////////
	//
	//	length
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	typename Vector4D<Float>::value_type Vector4D<Float>::length() const noexcept
	{
		return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
	}

	////////////////////////////////////////////////////////////////
	//
	//	lengthSq
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector4D<Float>::value_type Vector4D<Float>::lengthSq() const noexcept
	{
		return ((x * x) + (y * y) + (z * z) + (w * w));
	}

	////////////////////////////////////////////////////////////////
	//
	//	invLength
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	typename Vector4D<Float>::value_type Vector4D<Float>::invLength() const noexcept
	{
		return (static_cast<value_type>(1.0) / length());
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanLength
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector4D<Float>::value_type Vector4D<Float>::manhattanLength() const noexcept
	{
		return (Abs(x) + Abs(y) + Abs(z) + Abs(w));
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanDistanceFrom
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector4D<Float>::value_type Vector4D<Float>::manhattanDistanceFrom(const value_type _x, const value_type _y, const value_type _z, const value_type _w) const noexcept
	{
		const auto xMinMax = MinMax(x, _x);
		const auto yMinMax = MinMax(y, _y);
		const auto zMinMax = MinMax(z, _z);
		const auto wMinMax = MinMax(w, _w);
		return ((xMinMax.second - xMinMax.first)
			  + (yMinMax.second - yMinMax.first)
			  + (zMinMax.second - zMinMax.first)
			  + (wMinMax.second - wMinMax.first));
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector4D<Float>::value_type Vector4D<Float>::manhattanDistanceFrom(const Vector4D v) const noexcept
	{
		return manhattanDistanceFrom(v.x, v.y, v.z, v.w);
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanDistanceTo
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector4D<Float>::value_type Vector4D<Float>::manhattanDistanceTo(const value_type _x, const value_type _y, const value_type _z, const value_type _w) const noexcept
	{
		return manhattanDistanceFrom(_x, _y, _z, _w);
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector4D<Float>::value_type Vector4D<Float>::manhattanDistanceTo(const Vector4D v) const noexcept
	{
		return manhattanDistanceFrom(v.x, v.y, v.z, v.w);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFrom
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	typename Vector4D<Float>::value_type Vector4D<Float>::distanceFrom(const value_type _x, const value_type _y, const value_type _z, const value_type _w) const noexcept
	{
		return std::sqrt(distanceFromSq(_x, _y, _z, _w));
	}

	template <Concept::FloatingPoint Float>
	typename Vector4D<Float>::value_type Vector4D<Float>::distanceFrom(const Vector4D v) const noexcept
	{
		return std::sqrt(distanceFromSq(v.x, v.y, v.z, v.w));
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFromSq
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector4D<Float>::value_type Vector4D<Float>::distanceFromSq(const value_type _x, const value_type _y, const value_type _z, const value_type _w) const noexcept
	{
		const value_type xx = (x - _x) * (x - _x);
		const value_type yy = (y - _y) * (y - _y);
		const value_type zz = (z - _z) * (z - _z);
		const value_type ww = (w - _w) * (w - _w);
		return (xx + yy + zz + ww);
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector4D<Float>::value_type Vector4D<Float>::distanceFromSq(const Vector4D v) const noexcept
	{
		return distanceFromSq(v.x, v.y, v.z, v.w);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceTo
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	typename Vector4D<Float>::value_type Vector4D<Float>::distanceTo(const value_type _x, const value_type _y, const value_type _z, const value_type _w) const noexcept
	{
		return distanceFrom(_x, _y, _z, _w);
	}

	template <Concept::FloatingPoint Float>
	typename Vector4D<Float>::value_type Vector4D<Float>::distanceTo(const Vector4D v) const noexcept
	{
		return distanceFrom(v.x, v.y, v.z, v.w);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceToSq
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector4D<Float>::value_type Vector4D<Float>::distanceToSq(const value_type _x, const value_type _y, const value_type _z, const value_type _w) const noexcept
	{
		return distanceFromSq(_x, _y, _z, _w);
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector4D<Float>::value_type Vector4D<Float>::distanceToSq(const Vector4D v) const noexcept
	{
		return distanceFromSq(v.x, v.y, v.z, v.w);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withLength
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	Vector4D<Float> Vector4D<Float>::withLength(const value_type _length) const noexcept
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
	Vector4D<Float>& Vector4D<Float>::setLength(const value_type _length) noexcept
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
	Vector4D<Float> Vector4D<Float>::withMaxLength(const value_type maxLength) const noexcept
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
	Vector4D<Float>& Vector4D<Float>::setMaxLength(const value_type maxLength) noexcept
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
	Vector4D<Float> Vector4D<Float>::normalized() const noexcept
	{
		const value_type lenSq = lengthSq();

		if (lenSq == 0)
		{
			return *this;
		}

		const value_type invLen = (static_cast<value_type>(1.0) / std::sqrt(lenSq));
		return{ (x * invLen), (y * invLen), (z * invLen), (w * invLen) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	normalized_or
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	Vector4D<Float> Vector4D<Float>::normalized_or(const Vector4D valueIfZero) const noexcept
	{
		const value_type lenSq = lengthSq();

		if (lenSq == 0)
		{
			return valueIfZero;
		}

		const value_type invLen = (static_cast<value_type>(1.0) / std::sqrt(lenSq));
		return{ (x * invLen), (y * invLen), (z * invLen), (w * invLen) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	normalize
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	Vector4D<Float>& Vector4D<Float>::normalize() noexcept
	{
		const value_type lenSq = lengthSq();

		if (lenSq == 0)
		{
			x = y = z = w = 0;
		}
		else
		{
			const value_type invLen = (static_cast<value_type>(1.0) / std::sqrt(lenSq));
			x *= invLen;
			y *= invLen;
			z *= invLen;
			w *= invLen;
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMidpoint
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::getMidpoint(const Vector4D other) const noexcept
	{
		constexpr value_type half(0.5);
		return{ (x * half + other.x * half), (y * half + other.y * half), (z * half + other.z * half), (w * half + other.w * half) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::lerp(const Vector4D other, const value_type f) const noexcept
	{
		return{ (x + (other.x - x) * f), (y + (other.y - y) * f), (z + (other.z - z) * f), (w + (other.w - w) * f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	uint64 Vector4D<Float>::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	xx, xy, xz, xw, yx, yy, yz, yw, zx, zy, zz, zw, wx, wy, wz, ww
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::xx() const noexcept
	{
		return{ x, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::xy() const noexcept
	{
		return{ x, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::xz() const noexcept
	{
		return{ x, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::xw() const noexcept
	{
		return{ x, w };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::yx() const noexcept
	{
		return{ y, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::yy() const noexcept
	{
		return{ y, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::yz() const noexcept
	{
		return{ y, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::yw() const noexcept
	{
		return{ y, w };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::zx() const noexcept
	{
		return{ z, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::zy() const noexcept
	{
		return{ z, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::zz() const noexcept
	{
		return{ z, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::zw() const noexcept
	{
		return{ z, w };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::wx() const noexcept
	{
		return{ w, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::wy() const noexcept
	{
		return{ w, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::wz() const noexcept
	{
		return{ w, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector4D<Float>::ww() const noexcept
	{
		return{ w, w };
	}

	////////////////////////////////////////////////////////////////
	//
	//	xxx, xyz, yyy, yxz, yzw, zzz, zyx, www, wzy
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector4D<Float>::xxx() const noexcept
	{
		return{ x, x, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector4D<Float>::xyz() const noexcept
	{
		return{ x, y, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector4D<Float>::yyy() const noexcept
	{
		return{ y, y, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector4D<Float>::yxz() const noexcept
	{
		return{ y, x, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector4D<Float>::yzw() const noexcept
	{
		return{ y, z, w };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector4D<Float>::zzz() const noexcept
	{
		return{ z, z, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector4D<Float>::zyx() const noexcept
	{
		return{ z, y, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector4D<Float>::www() const noexcept
	{
		return{ w, w, w };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector4D<Float>::wzy() const noexcept
	{
		return{ w, z, y };
	}

	////////////////////////////////////////////////////////////////
	//
	//	xyz0, xyz1
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::xyz0() const noexcept
	{
		return{ x, y, z, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::xyz1() const noexcept
	{
		return{ x, y, z, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	xyzw, xxxx, yyyy, zzzz, wwww, wzyx
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::xyzw() const noexcept
	{
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::xxxx() const noexcept
	{
		return{ x, x, x, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::yyyy() const noexcept
	{
		return{ y, y, y, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::zzzz() const noexcept
	{
		return{ z, z, z, z };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::wwww() const noexcept
	{
		return{ w, w, w, w };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::wzyx() const noexcept
	{
		return{ w, z, y, x };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::Zero() noexcept
	{
		return{ 0, 0, 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	One
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::One() noexcept
	{
		return{ 1, 1, 1, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	All
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::All(const value_type value) noexcept
	{
		return{ value, value, value, value };
	}

	////////////////////////////////////////////////////////////////
	//
	//	UnitX, UnitY, UnitZ, UnitW
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::UnitX() noexcept
	{
		return{ 1, 0, 0, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::UnitY() noexcept
	{
		return{ 0, 1, 0, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::UnitZ() noexcept
	{
		return{ 0, 0, 1, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector4D<Float>::UnitW() noexcept
	{
		return{ 0, 0, 0, 1 };
	}
}
