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

	inline SimdFloat4::SimdFloat4(const float xyzw) noexcept
		: vec{ DirectX::XMVectorReplicate(xyzw) } {}

	inline SimdFloat4::SimdFloat4(const float x, const float y, const float z, const float w) noexcept
		: vec{ DirectX::XMVectorSet(x, y, z, w) } {}

	inline SimdFloat4::SimdFloat4(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y,
		const Concept::Arithmetic auto _z, const Concept::Arithmetic auto _w) noexcept
		: SimdFloat4{ static_cast<float>(_x), static_cast<float>(_y), static_cast<float>(_z), static_cast<float>(_w) } {}

	template <Concept::FloatingPoint U>
	inline SimdFloat4::SimdFloat4(const Vector2D<U>& xy, const Concept::Arithmetic auto _z, const Concept::Arithmetic auto _w) noexcept
		: SimdFloat4{ static_cast<float>(xy.x), static_cast<float>(xy.y), static_cast<float>(_z), static_cast<float>(_w) } {}

	template <Concept::FloatingPoint U>
	inline SimdFloat4::SimdFloat4(const Concept::Arithmetic auto _x, const Vector2D<U>& yz, const Concept::Arithmetic auto _w) noexcept
		: SimdFloat4{ static_cast<float>(_x), static_cast<float>(yz.x), static_cast<float>(yz.y), static_cast<float>(_w) } {}

	template <Concept::FloatingPoint U>
	inline SimdFloat4::SimdFloat4(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Vector2D<U>& zw) noexcept
		: SimdFloat4{ static_cast<float>(_x), static_cast<float>(_y), static_cast<float>(zw.x), static_cast<float>(zw.y) } {}

	template <Concept::FloatingPoint U, Concept::FloatingPoint V>
	inline SimdFloat4::SimdFloat4(const Vector2D<U>& xy, const Vector2D<V>& zw) noexcept
		: SimdFloat4{ static_cast<float>(xy.x), static_cast<float>(xy.y), static_cast<float>(zw.x), static_cast<float>(zw.y) } {}

	inline SimdFloat4::SimdFloat4(const Float3& xyz) noexcept
		: vec{ DirectX::XMLoadFloat3(
			static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&xyz))) } {}

	inline SimdFloat4::SimdFloat4(const Vec3& xyz) noexcept
		: SimdFloat4{ static_cast<float>(xyz.x), static_cast<float>(xyz.y), static_cast<float>(xyz.z), 0.0f } {}

	inline SimdFloat4::SimdFloat4(const Float3& xyz, const Concept::Arithmetic auto _w) noexcept
		: vec{ DirectX::XMVectorSetW(
			DirectX::XMLoadFloat3(static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&xyz))),
			static_cast<float>(_w)) } {}

	inline SimdFloat4::SimdFloat4(const Vec3& xyz, const Concept::Arithmetic auto _w) noexcept
		: SimdFloat4{ static_cast<float>(xyz.x), static_cast<float>(xyz.y),
			static_cast<float>(xyz.z), static_cast<float>(_w) } {}

	template <Concept::FloatingPoint U>
	inline SimdFloat4::SimdFloat4(const Vector3D<U>& xyz, const Concept::Arithmetic auto _w) noexcept
		: SimdFloat4{ static_cast<float>(xyz.x), static_cast<float>(xyz.y), static_cast<float>(xyz.z), static_cast<float>(_w) } {}

	template <Concept::FloatingPoint U>
	inline SimdFloat4::SimdFloat4(const Concept::Arithmetic auto _x, const Vector3D<U>& yzw) noexcept
		: SimdFloat4{ static_cast<float>(_x), static_cast<float>(yzw.x), static_cast<float>(yzw.y), static_cast<float>(yzw.z) } {}

	inline SimdFloat4::SimdFloat4(const Float4 xyzw) noexcept
		: SimdFloat4{ xyzw.x, xyzw.y, xyzw.z, xyzw.w } {}

	inline SimdFloat4::SimdFloat4(const Vec4 xyzw) noexcept
		: SimdFloat4{ static_cast<float>(xyzw.x), static_cast<float>(xyzw.y), static_cast<float>(xyzw.z), static_cast<float>(xyzw.w) } {}

	inline SimdFloat4::SimdFloat4(const aligned_float4 xyzw) noexcept
		: vec{ xyzw } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::operator +=(const SimdFloat4 v) & noexcept
	{
		vec = DirectX::XMVectorAdd(vec, v.vec);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::operator -=(const SimdFloat4 v) & noexcept
	{
		vec = DirectX::XMVectorSubtract(vec, v.vec);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *=
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::operator *=(const float s) & noexcept
	{
		vec = DirectX::XMVectorScale(vec, s);
		return *this;
	}

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::operator *=(const SimdFloat4 v) & noexcept
	{
		vec = DirectX::XMVectorMultiply(vec, v.vec);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /=
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::operator /=(const float s) & noexcept
	{
		vec = DirectX::XMVectorDivide(vec, DirectX::XMVectorReplicate(s));
		return *this;
	}

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::operator /=(const SimdFloat4 v) & noexcept
	{
		vec = DirectX::XMVectorDivide(vec, v.vec);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	epsilonEquals
	//
	////////////////////////////////////////////////////////////////

	inline bool SIV3D_VECTOR_CALL SimdFloat4::epsilonEquals(const SimdFloat4 other, const float epsilon) const noexcept
	{
		return DirectX::XMVector4NearEqual(vec, other.vec, DirectX::XMVectorReplicate(epsilon));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator aligned_float4
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4::operator aligned_float4() const noexcept
	{
		return vec;
	}

	////////////////////////////////////////////////////////////////
	//
	//	toFloat3, toFloat4
	//
	////////////////////////////////////////////////////////////////

	inline Float3 SimdFloat4::toFloat3() const noexcept
	{
		return{ DirectX::XMVectorGetX(vec), DirectX::XMVectorGetY(vec), DirectX::XMVectorGetZ(vec) };
	}

	inline Float4 SimdFloat4::toFloat4() const noexcept
	{
		return{ DirectX::XMVectorGetX(vec), DirectX::XMVectorGetY(vec), DirectX::XMVectorGetZ(vec), DirectX::XMVectorGetW(vec) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	elem
	//
	////////////////////////////////////////////////////////////////

	inline float SimdFloat4::elem(const size_t index) const noexcept
	{
		switch (index)
		{
		case 0:
			return DirectX::XMVectorGetX(vec);
		case 1:
			return DirectX::XMVectorGetY(vec);
		case 2:
			return DirectX::XMVectorGetZ(vec);
		case 3:
			return DirectX::XMVectorGetW(vec);
		default:
			return 0.0f;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getX, getY, getZ, getW
	//
	////////////////////////////////////////////////////////////////

	inline float SimdFloat4::getX() const noexcept
	{
		return DirectX::XMVectorGetX(vec);
	}

	inline float SimdFloat4::getY() const noexcept
	{
		return DirectX::XMVectorGetY(vec);
	}

	inline float SimdFloat4::getZ() const noexcept
	{
		return DirectX::XMVectorGetZ(vec);
	}

	inline float SimdFloat4::getW() const noexcept
	{
		return DirectX::XMVectorGetW(vec);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isZero, hasNaN, hasInf, isFinite
	//
	////////////////////////////////////////////////////////////////

	inline bool SimdFloat4::isZero() const noexcept
	{
		return DirectX::XMVector4Equal(vec, DirectX::XMVectorZero());
	}

	inline bool SimdFloat4::hasNaN() const noexcept
	{
		return DirectX::XMVector4IsNaN(vec);
	}

	inline bool SimdFloat4::hasInf() const noexcept
	{
		return DirectX::XMVector4IsInfinite(vec);
	}

	inline bool SimdFloat4::isFinite() const noexcept
	{
		return (not hasNaN() && not hasInf());
	}

	////////////////////////////////////////////////////////////////
	//
	//	setZero
	//
	////////////////////////////////////////////////////////////////

	inline void SimdFloat4::setZero() noexcept
	{
		vec = DirectX::XMVectorZero();
	}

	////////////////////////////////////////////////////////////////
	//
	//	withX, withY, withZ, withW
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::withX(const float _x) const noexcept
	{
		return DirectX::XMVectorSetX(vec, _x);
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::withY(const float _y) const noexcept
	{
		return DirectX::XMVectorSetY(vec, _y);
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::withZ(const float _z) const noexcept
	{
		return DirectX::XMVectorSetZ(vec, _z);
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::withW(const float _w) const noexcept
	{
		return DirectX::XMVectorSetW(vec, _w);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setX, setY, setZ, setW
	//
	////////////////////////////////////////////////////////////////

	inline void SimdFloat4::setX(const float _x) noexcept
	{
		vec = DirectX::XMVectorSetX(vec, _x);
	}

	inline void SimdFloat4::setY(const float _y) noexcept
	{
		vec = DirectX::XMVectorSetY(vec, _y);
	}

	inline void SimdFloat4::setZ(const float _z) noexcept
	{
		vec = DirectX::XMVectorSetZ(vec, _z);
	}

	inline void SimdFloat4::setW(const float _w) noexcept
	{
		vec = DirectX::XMVectorSetW(vec, _w);
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4& SimdFloat4::set(const float _x, const float _y, const float _z, const float _w) noexcept
	{
		vec = DirectX::XMVectorSet(_x, _y, _z, _w);
		return *this;
	}

	inline SimdFloat4& SimdFloat4::set(const Float4 xyzw) noexcept
	{
		return set(xyzw.x, xyzw.y, xyzw.z, xyzw.w);
	}

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::set(const SimdFloat4 xyzw) noexcept
	{
		vec = xyzw.vec;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy, moveBy
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::movedBy(const float _x, const float _y, const float _z, const float _w) const noexcept
	{
		return DirectX::XMVectorAdd(vec, DirectX::XMVectorSet(_x, _y, _z, _w));
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::movedBy(const SimdFloat4 v) const noexcept
	{
		return DirectX::XMVectorAdd(vec, v.vec);
	}

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::moveBy(const float _x, const float _y, const float _z, const float _w) noexcept
	{
		vec = DirectX::XMVectorAdd(vec, DirectX::XMVectorSet(_x, _y, _z, _w));
		return *this;
	}

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::moveBy(const SimdFloat4 v) noexcept
	{
		vec = DirectX::XMVectorAdd(vec, v.vec);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffset
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::withOffset(const float _x, const float _y, const float _z, const float _w) const noexcept
	{
		return movedBy(_x, _y, _z, _w);
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::withOffset(const SimdFloat4 v) const noexcept
	{
		return movedBy(v);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffsetX, withOffsetY, withOffsetZ, withOffsetW
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::withOffsetX(const float _x) const noexcept
	{
		return DirectX::XMVectorSetX(vec, (DirectX::XMVectorGetX(vec) + _x));
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::withOffsetY(const float _y) const noexcept
	{
		return DirectX::XMVectorSetY(vec, (DirectX::XMVectorGetY(vec) + _y));
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::withOffsetZ(const float _z) const noexcept
	{
		return DirectX::XMVectorSetZ(vec, (DirectX::XMVectorGetZ(vec) + _z));
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::withOffsetW(const float _w) const noexcept
	{
		return DirectX::XMVectorSetW(vec, (DirectX::XMVectorGetW(vec) + _w));
	}

	////////////////////////////////////////////////////////////////
	//
	//	dot
	//
	////////////////////////////////////////////////////////////////

	inline float SIV3D_VECTOR_CALL SimdFloat4::dot(const SimdFloat4 other) const noexcept
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Dot(vec, other.vec));
	}

	////////////////////////////////////////////////////////////////
	//
	//	length, lengthSq
	//
	////////////////////////////////////////////////////////////////

	inline float SimdFloat4::length() const noexcept
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Length(vec));
	}

	inline float SimdFloat4::lengthSq() const noexcept
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(vec));
	}

	////////////////////////////////////////////////////////////////
	//
	//	invLength
	//
	////////////////////////////////////////////////////////////////

	inline float SimdFloat4::invLength() const noexcept
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4ReciprocalLength(vec));
	}

	////////////////////////////////////////////////////////////////
	//
	//	reciprocal, fastReciprocal
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::reciprocal() const noexcept
	{
		return DirectX::XMVectorReciprocal(vec);
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::fastReciprocal() const noexcept
	{
		return DirectX::XMVectorReciprocalEst(vec);
	}

	////////////////////////////////////////////////////////////////
	//
	//	sqrt, fastSqrt
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::sqrt() const noexcept
	{
		return DirectX::XMVectorSqrt(vec);
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::fastSqrt() const noexcept
	{
		return DirectX::XMVectorSqrtEst(vec);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rsqrt, fastRsqrt
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::rsqrt() const noexcept
	{
		return DirectX::XMVectorReciprocalSqrt(vec);
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::fastRsqrt() const noexcept
	{
		return DirectX::XMVectorReciprocalSqrtEst(vec);
	}

	////////////////////////////////////////////////////////////////
	//
	//	normalize, normalized
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::normalize() noexcept
	{
		vec = DirectX::XMVector4Normalize(vec);
		return *this;
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::normalized() const noexcept
	{
		return DirectX::XMVector4Normalize(vec);
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::lerp(const SimdFloat4 other, const float f) const noexcept
	{
		return DirectX::XMVectorLerp(vec, other.vec, f);
	}

	////////////////////////////////////////////////////////////////
	//
	//	xy, yz, zw
	//
	////////////////////////////////////////////////////////////////

	inline Float2 SimdFloat4::xy() const noexcept
	{
		return{ DirectX::XMVectorGetX(vec), DirectX::XMVectorGetY(vec) };
	}

	inline Float2 SimdFloat4::yz() const noexcept
	{
		return{ DirectX::XMVectorGetY(vec), DirectX::XMVectorGetZ(vec) };
	}

	inline Float2 SimdFloat4::zw() const noexcept
	{
		return{ DirectX::XMVectorGetZ(vec), DirectX::XMVectorGetW(vec) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	xyz, yzw
	//
	////////////////////////////////////////////////////////////////

	inline Float3 SimdFloat4::xyz() const noexcept
	{
		return toFloat3();
	}

	inline Float3 SimdFloat4::yzw() const noexcept
	{
		return{ DirectX::XMVectorGetY(vec), DirectX::XMVectorGetZ(vec), DirectX::XMVectorGetW(vec) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	xyz0, xyz1
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::xyz0() const noexcept
	{
		return DirectX::XMVectorSetW(vec, 0.0f);
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::xyz1() const noexcept
	{
		return DirectX::XMVectorSetW(vec, 1.0f);
	}

	////////////////////////////////////////////////////////////////
	//
	//	xyzw, xxxx, yyyy, zzzz, wwww, wzyx
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::xyzw() const noexcept
	{
		return *this;
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::xxxx() const noexcept
	{
		return DirectX::XMVectorSplatX(vec);
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::yyyy() const noexcept
	{
		return DirectX::XMVectorSplatY(vec);
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::zzzz() const noexcept
	{
		return DirectX::XMVectorSplatZ(vec);
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::wwww() const noexcept
	{
		return DirectX::XMVectorSplatW(vec);
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::wzyx() const noexcept
	{
		return DirectX::XMVectorSwizzle<3, 2, 1, 0>(vec);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero, One, NaN, All
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::Zero() noexcept
	{
		return DirectX::XMVectorZero();
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::One() noexcept
	{
		return DirectX::XMVectorSplatOne();
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::NaN() noexcept
	{
		return DirectX::XMVectorSplatQNaN();
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::All(const float value) noexcept
	{
		return DirectX::XMVectorReplicate(value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	UnitX, UnitY, UnitZ, UnitW
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::UnitX() noexcept
	{
		return DirectX::g_XMIdentityR0.v;
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::UnitY() noexcept
	{
		return DirectX::g_XMIdentityR1.v;
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::UnitZ() noexcept
	{
		return DirectX::g_XMIdentityR2.v;
	}

	inline SimdFloat4 SIV3D_VECTOR_CALL SimdFloat4::UnitW() noexcept
	{
		return DirectX::g_XMIdentityR3.v;
	}
}
